#include <windows.h>
#undef min
#undef max
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <algorithm>
#include <cstring>

class AOB {
public:
    struct Result {
        std::vector<void*> addresses;
        bool operator!() const { return addresses.empty(); }
        operator bool() const { return !addresses.empty(); }
        void* operator[](size_t i) const { return addresses[i]; }
        size_t size() const { return addresses.size(); }
    };

    // 优化点 2: 缓存对齐。将字节和掩码交替存储，使 CPU 一次内存读取即可获得比对所需的所有信息
    struct PatternEntry {
        uint8_t byte;
        uint8_t mask; // 1 为精确匹配，0 为通配符
    };

    struct PatternData {
        size_t length;
        bool has_known_byte;
        size_t first_known_idx;
        uint8_t first_known_byte;
        std::vector<PatternEntry> entries;
    };

    static Result Scan(const std::string& pattern) {
        PatternData pmd = PreprocessPattern(pattern);
        if (pmd.entries.empty()) return {};

        // 1. 获取主模块 PE 头信息
        HMODULE hModule = GetModuleHandleA(NULL);
        if (!hModule) return {};

        PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
        PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((uint8_t*)hModule + pDosHeader->e_lfanew);

        // 获取 .text 段范围
        uintptr_t start_addr = (uintptr_t)hModule + pNtHeaders->OptionalHeader.BaseOfCode;
        size_t total_size = pNtHeaders->OptionalHeader.SizeOfCode;
        uintptr_t end_addr = start_addr + total_size;

        // 验证内存可读性（仅验证一次，避免在扫描循环中调用）
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery((LPCVOID)start_addr, &mbi, sizeof(mbi))) return {};
        if (mbi.State != MEM_COMMIT || (mbi.Protect & PAGE_GUARD)) return {};

        // 2. 优化点 1: 预分块 + 固定线程数，消除 std::async 调度开销和原子操作竞争
        unsigned int num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 4;

        // 如果扫描范围太小，单线程更划算
        if (total_size < 4096) num_threads = 1;

        std::vector<std::thread> workers;
        std::vector<std::vector<void*>> thread_results(num_threads);

        // 计算每个线程的负责区域
        size_t chunk_size = total_size / num_threads;

        for (unsigned int i = 0; i < num_threads; ++i) {
            uintptr_t thread_start = start_addr + (i * chunk_size);
            uintptr_t thread_end;

            if (i == num_threads - 1) {
                thread_end = end_addr; // 最后一个线程处理剩余所有部分
            }
            else {
                // 每个块必须向后重叠特征码长度-1，防止特征码正好被分在两个块之间
                thread_end = thread_start + chunk_size + pmd.length - 1;
                // 确保不越过总边界
                if (thread_end > end_addr) thread_end = end_addr;
            }

            workers.emplace_back(ScanSection, thread_start, thread_end, std::ref(pmd), std::ref(thread_results[i]));
        }

        // 等待所有线程完成
        for (auto& t : workers) {
            if (t.joinable()) t.join();
        }

        // 3. 汇总结果
        std::vector<void*> total_results;
        for (const auto& local_vec : thread_results) {
            total_results.insert(total_results.end(), local_vec.begin(), local_vec.end());
        }

        return { total_results };
    }

private:
    // 优化点 3: 移除 SEH (__try/__except)，允许编译器执行更激进的 SIMD/自动矢量化
    // 由于我们严格限定在 .text 段且预先校验了 VirtualQuery，移除 SEH 是安全的
    static void ScanSection(uintptr_t start, uintptr_t end, const PatternData& pmd, std::vector<void*>& found) {
        if (end - start < pmd.length) return;

        const uint8_t* scan_ptr = (const uint8_t*)start;
        const uint8_t* scan_end = (const uint8_t*)end - pmd.length;

        const PatternEntry* entries = pmd.entries.data();
        const size_t pat_len = pmd.length;

        if (pmd.has_known_byte) {
            const uint8_t first_byte = pmd.first_known_byte;
            const size_t first_idx = pmd.first_known_idx;

            while (scan_ptr <= scan_end) {
                // 利用 memchr 的硬件加速（SSE2/AVX）快速跳过不匹配的字节
                const uint8_t* match = (const uint8_t*)memchr(scan_ptr + first_idx, first_byte, (scan_end - scan_ptr) + 1);
                if (!match) break;

                scan_ptr = match - first_idx;

                bool is_match = true;
                // 这里的循环现在是 Cache-friendly 的，byte 和 mask 挨在一起
                for (size_t i = 0; i < pat_len; ++i) {
                    if (entries[i].mask && scan_ptr[i] != entries[i].byte) {
                        is_match = false;
                        break;
                    }
                }

                if (is_match) {
                    found.push_back((void*)scan_ptr);
                }
                scan_ptr++;
            }
        }
        else {
            // 处理极端情况：全通配符
            while (scan_ptr <= scan_end) {
                found.push_back((void*)scan_ptr);
                scan_ptr++;
            }
        }
    }

    static PatternData PreprocessPattern(const std::string& pattern) {
        PatternData pmd;
        pmd.has_known_byte = false;
        pmd.first_known_idx = 0;
        pmd.first_known_byte = 0;

        std::stringstream ss(pattern);
        std::string item;
        while (ss >> item) {
            if (item == "?" || item == "??") {
                pmd.entries.push_back({ 0x00, 0 });
            }
            else {
                char* endptr;
                uint8_t val = static_cast<uint8_t>(std::strtoul(item.c_str(), &endptr, 16));
                if (endptr != item.c_str()) {
                    if (!pmd.has_known_byte) {
                        pmd.has_known_byte = true;
                        pmd.first_known_idx = pmd.entries.size();
                        pmd.first_known_byte = val;
                    }
                    pmd.entries.push_back({ val, 1 });
                }
            }
        }
        pmd.length = pmd.entries.size();
        return pmd;
    }
};