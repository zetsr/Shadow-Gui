#pragma once
#include <windows.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <format>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <cmath>
#include <bit>
#include <charconv>
#include <array>
#include <ranges>

#include "../../CppSDK/SDK.hpp"

namespace Shadow {
    enum class HotkeyMode {
        None,
        HoldOn,
        ToggleOn,
        HoldOff,
        AlwaysOn
    };

    struct Vec2 { float x, y; };
    struct Color { float r, g, b, a; };

    // 提取 ColorPicker 的配置以保持统一
    namespace CPConfig {
        inline const float Padding = 8.f;
        inline const float SVSize = 200.f;
        inline const float HueWidth = 20.f;
        inline const float AlphaWidth = 20.f;
        inline const float Spacing = 8.f;
    }

    struct GuiContext {
        SDK::UCanvas* Canvas = nullptr;
        SDK::UFont* DefaultFont = nullptr;

        // 鼠标状态
        Vec2 MousePos = { 0.f, 0.f };
        bool MouseDown = false;
        bool MouseClicked = false;
        bool RightMouseDown = false;     // 新增：右键按下状态
        bool RightMouseClicked = false;  // 新增：右键单击触发

        // 键盘与热键状态机
        bool KeyStates[256] = { false };
        bool HotkeyToggles[256] = { false };
        bool KeyPressed[256] = { false }; // 单帧触发状态
        int* AssigningHotkey = nullptr;

        // ComboBox / Dropdown 状态 (新增)
        size_t ActiveDropdownId = 0;
        int* DropdownCurrentItem = nullptr;
        std::vector<std::string> DropdownItems;
        Vec2 DropdownPos = { 0.f, 0.f };
        Vec2 DropdownSize = { 0.f, 0.f };

        // 窗口状态
        Vec2 WindowPos = { 100.f, 100.f };
        Vec2 WindowSize = { 500.f, 400.f };
        bool IsDragging = false;
        Vec2 DragOffset = { 0.f, 0.f };

        // 栈平衡检查
        int BeginStack = 0;
        int TabBarStack = 0;
        int TabItemStack = 0;
        std::string LastErrorMsg;

        // 窗口调整大小
        bool IsResizing = false;
        Vec2 ResizeStartPos = { 0.f, 0.f };
        Vec2 ResizeStartSize = { 0.f, 0.f };
        float ResizeTriangleSize = 16.f;
        bool IsHoveringResize = false;

        // 剪裁区域
        bool ClippingEnabled = false;
        Vec2 ClipMin = { 0.f, 0.f };
        Vec2 ClipMax = { 0.f, 0.f };

        // 游标与布局
        Vec2 Cursor = { 0.f, 0.f };
        float Padding = 8.f;
        float ItemHeight = 20.f;

        // Tab状态
        size_t ActiveTabId = 0;
        size_t CurrentTabId = 0;
        bool InActiveTab = true;
        Vec2 TabCursor = { 0.f, 0.f };

        // ColorPicker 弹出层状态
        size_t ActiveColorPickerId = 0;
        float* ColorPickerR = nullptr;
        float* ColorPickerG = nullptr;
        float* ColorPickerB = nullptr;
        float* ColorPickerA = nullptr;
        Vec2 ColorPickerPos = { 0.f, 0.f };
        float ColorPickerH = 0.f;
        float ColorPickerS = 0.f;
        float ColorPickerV = 0.f;

        // ColorPicker 拖拽与输入状态
        bool IsDraggingSV = false;
        bool IsDraggingHue = false;
        bool IsDraggingAlpha = false;
        bool IsDraggingColorPicker = false;
        Vec2 ColorPickerDragOffset = { 0.f, 0.f };
        bool IsTypingHex = false;
        std::string HexInputBuffer;

        // Slider 状态
        size_t DraggingSliderId = 0;
        size_t FocusedSliderId = 0;
    };

    inline GuiContext g_Ctx;
    inline SDK::UFont*& DefaultFont = g_Ctx.DefaultFont;

    // 允许放行透传给游戏的按键列表
    inline std::vector<int> AllowedKeys;
    inline void SetAllowedKeys(const std::vector<int>& keys) {
        AllowedKeys = keys;
    }
    inline bool IsKeyAllowed(int key) {
        return std::find(AllowedKeys.begin(), AllowedKeys.end(), key) != AllowedKeys.end();
    }

    inline constexpr size_t HashString(std::string_view str) noexcept {
        size_t hash = 14695981039346656037ULL;
        for (char c : str) {
            hash ^= static_cast<size_t>(c);
            hash *= 1099511628211ULL;
        }
        return hash;
    }

    inline void ParseLabel(std::string_view raw, std::string_view& display_name, size_t& id) {
        id = HashString(raw);
        size_t pos = raw.find("##");
        display_name = (pos != std::string_view::npos) ? raw.substr(0, pos) : raw;
    }

    inline std::wstring ToWString(std::string_view utf8_str) {
        if (utf8_str.empty()) return L"";
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8_str.data(), static_cast<int>(utf8_str.size()), nullptr, 0);
        std::wstring wstr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, utf8_str.data(), static_cast<int>(utf8_str.size()), &wstr[0], size_needed);
        return wstr;
    }

    // VK键码转字符串名称
    inline std::string GetKeyName(int vk) {
        if (vk == 0) return "None";
        switch (vk) {
        case VK_LBUTTON: return "Mouse 1";
        case VK_RBUTTON: return "Mouse 2";
        case VK_MBUTTON: return "Mouse 3";
        case VK_XBUTTON1: return "Mouse 4";
        case VK_XBUTTON2: return "Mouse 5";
        case VK_BACK: return "Backspace";
        case VK_TAB: return "Tab";
        case VK_RETURN: return "Enter";
        case VK_SHIFT: return "Shift";
        case VK_CONTROL: return "Ctrl";
        case VK_MENU: return "Alt";
        case VK_CAPITAL: return "Caps Lock";
        case VK_ESCAPE: return "Esc";
        case VK_SPACE: return "Space";
        case VK_PRIOR: return "Page Up";
        case VK_NEXT: return "Page Down";
        case VK_END: return "End";
        case VK_HOME: return "Home";
        case VK_LEFT: return "Left";
        case VK_UP: return "Up";
        case VK_RIGHT: return "Right";
        case VK_DOWN: return "Down";
        case VK_INSERT: return "Insert";
        case VK_DELETE: return "Delete";
        case VK_LWIN: return "LWin";
        case VK_RWIN: return "RWin";
        case VK_NUMPAD0: return "Num 0";
        case VK_NUMPAD1: return "Num 1";
        case VK_NUMPAD2: return "Num 2";
        case VK_NUMPAD3: return "Num 3";
        case VK_NUMPAD4: return "Num 4";
        case VK_NUMPAD5: return "Num 5";
        case VK_NUMPAD6: return "Num 6";
        case VK_NUMPAD7: return "Num 7";
        case VK_NUMPAD8: return "Num 8";
        case VK_NUMPAD9: return "Num 9";
        case VK_MULTIPLY: return "Num *";
        case VK_ADD: return "Num +";
        case VK_SUBTRACT: return "Num -";
        case VK_DECIMAL: return "Num .";
        case VK_DIVIDE: return "Num /";
        case VK_F1: return "F1";
        case VK_F2: return "F2";
        case VK_F3: return "F3";
        case VK_F4: return "F4";
        case VK_F5: return "F5";
        case VK_F6: return "F6";
        case VK_F7: return "F7";
        case VK_F8: return "F8";
        case VK_F9: return "F9";
        case VK_F10: return "F10";
        case VK_F11: return "F11";
        case VK_F12: return "F12";
        case VK_LSHIFT: return "LShift";
        case VK_RSHIFT: return "RShift";
        case VK_LCONTROL: return "LCtrl";
        case VK_RCONTROL: return "RCtrl";
        case VK_LMENU: return "LAlt";
        case VK_RMENU: return "RAlt";
        }
        if (vk >= '0' && vk <= '9') return std::string(1, static_cast<char>(vk));
        if (vk >= 'A' && vk <= 'Z') return std::string(1, static_cast<char>(vk));
        return std::format("Unk {:X}", vk);
    }

    inline void RGBtoHSV(float r, float g, float b, float& h, float& s, float& v) {
        float max_val = std::max({ r, g, b });
        float min_val = std::min({ r, g, b });
        v = max_val;
        float delta = max_val - min_val;
        if (max_val > 0.0f) { s = delta / max_val; }
        else { s = 0.0f; h = 0.0f; return; }
        if (delta == 0.0f) { h = 0.0f; return; }
        if (r >= max_val) { h = (g - b) / delta; }
        else if (g >= max_val) { h = 2.0f + (b - r) / delta; }
        else { h = 4.0f + (r - g) / delta; }
        h *= 60.0f;
        if (h < 0.0f) h += 360.0f;
        h /= 360.0f;
    }

    inline void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b) {
        if (s <= 0.0f) { r = g = b = v; return; }
        h *= 360.0f;
        if (h >= 360.0f) h = 0.0f;
        h /= 60.0f;
        int i = static_cast<int>(h);
        float ff = h - i;
        float p = v * (1.0f - s);
        float q = v * (1.0f - (s * ff));
        float t = v * (1.0f - (s * (1.0f - ff)));
        switch (i) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: default: r = v; g = p; b = q; break;
        }
    }

    inline bool IsMouseHoveringRaw(Vec2 pos, Vec2 size) {
        return g_Ctx.MousePos.x >= pos.x && g_Ctx.MousePos.x <= pos.x + size.x &&
            g_Ctx.MousePos.y >= pos.y && g_Ctx.MousePos.y <= pos.y + size.y;
    }

    inline bool IsMouseHovering(Vec2 pos, Vec2 size) {
        // 如果下拉框开启，检测鼠标是否悬停在下拉框上，以此阻挡底层UI的Hover判定
        if (g_Ctx.ActiveDropdownId != 0) {
            float dropWidth = g_Ctx.DropdownSize.x;
            float dropHeight = g_Ctx.DropdownItems.size() * g_Ctx.ItemHeight;
            if (IsMouseHoveringRaw({ g_Ctx.DropdownPos.x - 1.f, g_Ctx.DropdownPos.y - 1.f }, { dropWidth + 2.f, dropHeight + 2.f })) {
                return false;
            }
        }

        if (g_Ctx.ActiveColorPickerId != 0) {
            Vec2 popupPos = g_Ctx.ColorPickerPos;
            float hexBoxHeight = std::max(24.f, g_Ctx.ItemHeight + 4.f);
            float popupWidth = CPConfig::Padding + CPConfig::SVSize + CPConfig::Spacing + CPConfig::HueWidth + CPConfig::Spacing + CPConfig::AlphaWidth + CPConfig::Padding;
            float popupHeight = CPConfig::Padding + CPConfig::SVSize + CPConfig::Spacing + hexBoxHeight + CPConfig::Padding;

            if (IsMouseHoveringRaw({ popupPos.x - 2.f, popupPos.y - 2.f }, { popupWidth + 4.f, popupHeight + 4.f })) {
                return false;
            }
        }
        return IsMouseHoveringRaw(pos, size);
    }

    inline bool TryAssignKey(int vk) {
        if (g_Ctx.AssigningHotkey) {
            if (vk == VK_ESCAPE) {
                *g_Ctx.AssigningHotkey = 0; // Esc清空绑定
            }
            else {
                *g_Ctx.AssigningHotkey = vk;
            }
            g_Ctx.AssigningHotkey = nullptr;
            return true;
        }
        return false;
    }

    inline bool HandleKeyDown(int vk) {
        if (TryAssignKey(vk)) return true;
        if (!g_Ctx.KeyStates[vk]) {
            g_Ctx.HotkeyToggles[vk] = !g_Ctx.HotkeyToggles[vk];
            g_Ctx.KeyPressed[vk] = true;
        }
        g_Ctx.KeyStates[vk] = true;
        return false;
    }

    inline void HandleKeyUp(int vk) {
        g_Ctx.KeyStates[vk] = false;
    }

    // constexpr 查找表
    inline constexpr std::array<int, 256> HEX_TABLE = []() constexpr {
        std::array<int, 256> table{};
        table.fill(-1);
        for (int i = 0; i < 10; ++i) {
            table['0' + i] = i;
        }
        for (int i = 0; i < 6; ++i) {
            table['A' + i] = 10 + i;
            table['a' + i] = 10 + i;
        }
        return table;
        }();

    inline void ApplyHexInput() {
        if (g_Ctx.HexInputBuffer.size() < 6) return;

        std::string_view hex = g_Ctx.HexInputBuffer;
        hex = hex.substr(0, std::min(hex.size(), size_t(8)));

        // 批量验证
        auto hexDigits = hex | std::views::transform([](char c) {
            return HEX_TABLE[static_cast<unsigned char>(c)];
            });

        // 如果有任何 -1，说明包含非法字符
        if (std::ranges::any_of(hexDigits, [](int v) { return v < 0; })) {
            return;
        }

        // 直接转换为数组
        std::array<int, 4> rgba{};
        for (size_t i = 0; i < 4 && i * 2 + 1 < hex.size(); ++i) {
            int high = HEX_TABLE[static_cast<unsigned char>(hex[i * 2])];
            int low = HEX_TABLE[static_cast<unsigned char>(hex[i * 2 + 1])];
            rgba[i] = (high << 4) | low;
        }

        // 如果缺少 alpha，默认为 255
        if (hex.size() < 8) rgba[3] = 255;

        auto [r, g, b, a] = rgba;

        *g_Ctx.ColorPickerR = r / 255.f;
        *g_Ctx.ColorPickerG = g / 255.f;
        *g_Ctx.ColorPickerB = b / 255.f;
        if (g_Ctx.ColorPickerA) *g_Ctx.ColorPickerA = a / 255.f;

        RGBtoHSV(*g_Ctx.ColorPickerR, *g_Ctx.ColorPickerG, *g_Ctx.ColorPickerB,
            g_Ctx.ColorPickerH, g_Ctx.ColorPickerS, g_Ctx.ColorPickerV);
    }

    inline LRESULT Input(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_MOUSEMOVE:
            g_Ctx.MousePos.x = static_cast<float>(LOWORD(lParam));
            g_Ctx.MousePos.y = static_cast<float>(HIWORD(lParam));
            break;
        case WM_LBUTTONDOWN:
            if (HandleKeyDown(VK_LBUTTON)) return 0;
            g_Ctx.MouseDown = true;
            g_Ctx.MouseClicked = true;
            break;
        case WM_LBUTTONUP:
            HandleKeyUp(VK_LBUTTON);
            g_Ctx.MouseDown = false;
            g_Ctx.IsDragging = false;
            g_Ctx.IsResizing = false;
            break;
        case WM_RBUTTONDOWN:
            if (HandleKeyDown(VK_RBUTTON)) return 0;
            g_Ctx.RightMouseDown = true;
            g_Ctx.RightMouseClicked = true;
            break;
        case WM_RBUTTONUP:
            HandleKeyUp(VK_RBUTTON);
            g_Ctx.RightMouseDown = false;
            break;
        case WM_MBUTTONDOWN: if (HandleKeyDown(VK_MBUTTON)) return 0; break;
        case WM_MBUTTONUP:   HandleKeyUp(VK_MBUTTON); break;
        case WM_XBUTTONDOWN: {
            int vk = (HIWORD(wParam) == XBUTTON1) ? VK_XBUTTON1 : VK_XBUTTON2;
            if (HandleKeyDown(vk)) return 0;
            break;
        }
        case WM_XBUTTONUP: {
            int vk = (HIWORD(wParam) == XBUTTON1) ? VK_XBUTTON1 : VK_XBUTTON2;
            HandleKeyUp(vk);
            break;
        }
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (wParam < 256) {
                if (g_Ctx.IsTypingHex) {
                    int vk = static_cast<int>(wParam);
                    if (vk == VK_BACK && !g_Ctx.HexInputBuffer.empty()) {
                        g_Ctx.HexInputBuffer.pop_back();
                    }
                    else if (vk == VK_RETURN || vk == VK_ESCAPE) {
                        g_Ctx.IsTypingHex = false;
                        if (vk == VK_RETURN) ApplyHexInput();
                    }
                    else if (g_Ctx.HexInputBuffer.size() < 8) {
                        char c = 0;
                        if (vk >= '0' && vk <= '9') c = vk;
                        else if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9) c = vk - VK_NUMPAD0 + '0';
                        else if (vk >= 'A' && vk <= 'F') c = vk;
                        else if (vk >= 'a' && vk <= 'f') c = vk - 'a' + 'A';
                        if (c != 0) g_Ctx.HexInputBuffer += c;
                    }
                    return 0; // 拦截Hex键盘输入，防止影响快捷键绑定
                }
                if (HandleKeyDown(static_cast<int>(wParam))) return 0;
            }
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            if (wParam < 256) HandleKeyUp(static_cast<int>(wParam));
            break;
        }
        return 0;
    }

    // 剪裁辅助函数：设置剪裁区域
    inline void PushClipRect(Vec2 min, Vec2 max) {
        g_Ctx.ClippingEnabled = true;
        g_Ctx.ClipMin = min;
        g_Ctx.ClipMax = max;
    }

    // 剪裁辅助函数：清除剪裁区域
    inline void PopClipRect() {
        g_Ctx.ClippingEnabled = false;
    }

    // 检查矩形是否在剪裁区域内（至少部分可见）
    inline bool IsRectVisible(Vec2 pos, Vec2 size) {
        if (!g_Ctx.ClippingEnabled) return true;

        // 检查矩形是否完全在剪裁区域之外
        if (pos.x + size.x < g_Ctx.ClipMin.x || pos.x > g_Ctx.ClipMax.x ||
            pos.y + size.y < g_Ctx.ClipMin.y || pos.y > g_Ctx.ClipMax.y) {
            return false;
        }
        return true;
    }

    // 检查矩形是否完全在剪裁区域内
    inline bool IsRectFullyVisible(Vec2 pos, Vec2 size) {
        if (!g_Ctx.ClippingEnabled) return true;
        return pos.x >= g_Ctx.ClipMin.x && pos.y >= g_Ctx.ClipMin.y &&
            (pos.x + size.x) <= g_Ctx.ClipMax.x && (pos.y + size.y) <= g_Ctx.ClipMax.y;
    }

    // 裁剪矩形到剪裁区域
    inline void ClipRect(Vec2& pos, Vec2& size) {
        if (!g_Ctx.ClippingEnabled) return;

        // 裁剪左边界
        if (pos.x < g_Ctx.ClipMin.x) {
            size.x -= (g_Ctx.ClipMin.x - pos.x);
            pos.x = g_Ctx.ClipMin.x;
        }
        // 裁剪上边界
        if (pos.y < g_Ctx.ClipMin.y) {
            size.y -= (g_Ctx.ClipMin.y - pos.y);
            pos.y = g_Ctx.ClipMin.y;
        }
        // 裁剪右边界
        if (pos.x + size.x > g_Ctx.ClipMax.x) {
            size.x = g_Ctx.ClipMax.x - pos.x;
        }
        // 裁剪下边界
        if (pos.y + size.y > g_Ctx.ClipMax.y) {
            size.y = g_Ctx.ClipMax.y - pos.y;
        }

        // 确保尺寸不为负
        if (size.x < 0.f) size.x = 0.f;
        if (size.y < 0.f) size.y = 0.f;
    }

    inline void DrawRect(Vec2 pos, Vec2 size, Color color) {
        if (!g_Ctx.Canvas) return;

        // 检查是否可见
        if (!IsRectVisible(pos, size)) return;

        // 裁剪矩形
        ClipRect(pos, size);
        if (size.x <= 0.f || size.y <= 0.f) return;

        SDK::FLinearColor ueColor{ color.r, color.g, color.b, color.a };
        SDK::FVector2D uePos{ static_cast<double>(pos.x), static_cast<double>(pos.y) };
        SDK::FVector2D ueSize{ static_cast<double>(size.x), static_cast<double>(size.y) };

        // 使用 DefaultTexture 绘制实心或半透明矩形填充
        if (g_Ctx.Canvas->DefaultTexture) {
            g_Ctx.Canvas->K2_DrawTexture(
                g_Ctx.Canvas->DefaultTexture,
                uePos,
                ueSize,
                SDK::FVector2D{ 0.0, 0.0 },
                SDK::FVector2D{ 1.0, 1.0 },
                ueColor,
                SDK::EBlendMode::BLEND_Translucent,
                0.0f,
                SDK::FVector2D{ 0.0, 0.0 }
            );
        }

        // 额外叠加一层原有的线框，以保留菜单锐利的边界线条感
        g_Ctx.Canvas->K2_DrawBox(uePos, ueSize, 1.0f, ueColor);
    }

    inline Vec2 MeasureTextSize(std::string_view text) {
        if (!g_Ctx.Canvas || !g_Ctx.DefaultFont) return { 0.f, 0.f };
        SDK::FVector2D scale{ 1.0, 1.0 };
        std::wstring wstr = ToWString(text);
        SDK::FVector2D size = g_Ctx.Canvas->K2_TextSize(g_Ctx.DefaultFont, SDK::FString(wstr.c_str()), scale);
        return { static_cast<float>(size.X), static_cast<float>(size.Y) };
    }

    // 测量单个字符的宽度
    inline float MeasureCharWidth(wchar_t ch) {
        if (!g_Ctx.Canvas || !g_Ctx.DefaultFont) return 0.f;
        std::wstring single(1, ch);
        SDK::FVector2D scale{ 1.0, 1.0 };
        SDK::FVector2D size = g_Ctx.Canvas->K2_TextSize(g_Ctx.DefaultFont, SDK::FString(single.c_str()), scale);
        return static_cast<float>(size.X);
    }

    // 测量文本的整体高度
    inline float MeasureTextHeight(std::string_view text) {
        if (!g_Ctx.Canvas || !g_Ctx.DefaultFont || text.empty()) return 0.f;
        Vec2 size = MeasureTextSize(text);
        return size.y;
    }

    // 裁剪文本：根据剪裁区域裁剪文本字符串
    // 返回裁剪后的文本，以及是否需要调整位置
    inline std::string ClipTextString(std::string_view text, Vec2 pos, Vec2& outPos, bool& shouldDraw) {
        shouldDraw = true;
        outPos = pos;

        if (!g_Ctx.ClippingEnabled) {
            return std::string(text);
        }

        // 垂直裁剪：如果文本底部超出剪裁区域底部，整个文本不绘制
        float textHeight = MeasureTextHeight(text);
        if (pos.y + textHeight > g_Ctx.ClipMax.y) {
            shouldDraw = false;
            return "";
        }

        // 如果文本顶部在剪裁区域之上，整个文本不绘制
        if (pos.y < g_Ctx.ClipMin.y) {
            shouldDraw = false;
            return "";
        }

        // 水平裁剪：从右侧逐字符删除
        std::wstring wstr = ToWString(text);
        if (wstr.empty()) {
            shouldDraw = false;
            return "";
        }

        // 检查是否完全在剪裁区域右侧之外
        if (pos.x >= g_Ctx.ClipMax.x) {
            shouldDraw = false;
            return "";
        }

        // 检查是否需要从左边裁剪
        if (pos.x < g_Ctx.ClipMin.x) {
            // 计算需要从左边删除多少字符
            float currentX = pos.x;
            std::wstring clippedWstr;
            bool foundStart = false;

            for (size_t i = 0; i < wstr.size(); ++i) {
                float charWidth = MeasureCharWidth(wstr[i]);
                if (!foundStart) {
                    if (currentX + charWidth > g_Ctx.ClipMin.x) {
                        foundStart = true;
                        clippedWstr += wstr[i];
                        outPos.x = currentX;
                    }
                    currentX += charWidth;
                }
                else {
                    clippedWstr += wstr[i];
                }
            }

            if (!foundStart) {
                shouldDraw = false;
                return "";
            }
            wstr = clippedWstr;
        }

        // 从右边裁剪：循环删除超出右边界的字符
        float totalWidth = 0.f;
        for (wchar_t ch : wstr) {
            totalWidth += MeasureCharWidth(ch);
        }

        while (!wstr.empty() && outPos.x + totalWidth > g_Ctx.ClipMax.x) {
            // 删除最后一个字符
            wchar_t lastChar = wstr.back();
            float lastCharWidth = MeasureCharWidth(lastChar);
            totalWidth -= lastCharWidth;
            wstr.pop_back();
        }

        if (wstr.empty()) {
            shouldDraw = false;
            return "";
        }

        // 将裁剪后的宽字符串转换回UTF-8
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
        std::string result(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &result[0], size_needed, nullptr, nullptr);

        shouldDraw = true;
        return result;
    }

    inline void DrawTextString(std::string_view text, Vec2 pos, Color color) {
        if (!g_Ctx.Canvas || !g_Ctx.DefaultFont) return;

        // 使用新的文本裁剪函数
        Vec2 clippedPos = pos;
        bool shouldDraw = true;
        std::string clippedText = ClipTextString(text, pos, clippedPos, shouldDraw);

        if (!shouldDraw || clippedText.empty()) return;

        SDK::FLinearColor ueColor{ color.r, color.g, color.b, color.a };
        SDK::FVector2D uePos{ static_cast<double>(clippedPos.x), static_cast<double>(clippedPos.y) };
        SDK::FVector2D scale{ 1.0, 1.0 };
        SDK::FLinearColor shadow{ 0.f, 0.f, 0.f, 1.f };
        SDK::FVector2D shadowOff{ 1.0, 1.0 };
        SDK::FLinearColor outline{ 0.f, 0.f, 0.f, 0.f };

        std::wstring wstr = ToWString(clippedText);
        g_Ctx.Canvas->K2_DrawText(g_Ctx.DefaultFont, SDK::FString(wstr.c_str()), uePos, scale, ueColor, 0.f, shadow, shadowOff, false, false, false, outline);
    }

    // 动态获取右半部分的统一控制起始X位置
    inline float GetControlOffsetX() {
        return std::max(120.f, g_Ctx.WindowSize.x * 0.4f);
    }

    // 检查栈平衡并绘制错误信息
    inline void CheckAndDrawErrors() {
        std::string errorMsg;

        if (g_Ctx.BeginStack > 0) {
            errorMsg = std::format("ERROR: Begin() called {} time(s) without matching End()!", g_Ctx.BeginStack);
        }
        else if (g_Ctx.BeginStack < 0) {
            errorMsg = std::format("ERROR: End() called {} time(s) without matching Begin()!", -g_Ctx.BeginStack);
        }
        else if (g_Ctx.TabBarStack > 0) {
            errorMsg = std::format("ERROR: BeginTabBar() called {} time(s) without matching EndTabBar()!", g_Ctx.TabBarStack);
        }
        else if (g_Ctx.TabBarStack < 0) {
            errorMsg = std::format("ERROR: EndTabBar() called {} time(s) without matching BeginTabBar()!", -g_Ctx.TabBarStack);
        }
        else if (g_Ctx.TabItemStack > 0) {
            errorMsg = std::format("ERROR: BeginTabItem() called {} time(s) without matching EndTabItem()!", g_Ctx.TabItemStack);
        }
        else if (g_Ctx.TabItemStack < 0) {
            errorMsg = std::format("ERROR: EndTabItem() called {} time(s) without matching BeginTabItem()!", -g_Ctx.TabItemStack);
        }

        if (!errorMsg.empty()) {
            // 临时禁用剪裁以确保错误信息始终可见
            bool oldClipping = g_Ctx.ClippingEnabled;
            g_Ctx.ClippingEnabled = false;

            // 绘制错误文本在屏幕左上角 (x=5, y=5)，颜色为 (255, 50, 50, 255)
            DrawTextString(errorMsg, { 5.f, 5.f }, { 1.f, 0.196f, 0.196f, 1.f });

            // 恢复剪裁状态
            g_Ctx.ClippingEnabled = oldClipping;
        }
    }

    inline void NewFrame(SDK::UCanvas* Canvas) {
        g_Ctx.Canvas = Canvas;
        if (!g_Ctx.DefaultFont) {
            if (SDK::UEngine::GetEngine()) { g_Ctx.DefaultFont = SDK::UEngine::GetEngine()->LargeFont; }
        }

        // 获取字体大小并动态设定控件全局的高度和间隔
        Vec2 charSize = MeasureTextSize("A");
        g_Ctx.ItemHeight = charSize.y > 0.f ? charSize.y + 4.f : 20.f;
        g_Ctx.Padding = 8.f;

        g_Ctx.InActiveTab = true;

        // 重置栈计数器
        g_Ctx.BeginStack = 0;
        g_Ctx.TabBarStack = 0;
        g_Ctx.TabItemStack = 0;
    }

    inline void RenderPopups() {
        // --- 绘制下拉框 (Dropdown/ComboBox) ---
        if (g_Ctx.ActiveDropdownId != 0 && g_Ctx.DropdownCurrentItem) {
            float dropWidth = g_Ctx.DropdownSize.x;
            float dropHeight = g_Ctx.DropdownItems.size() * g_Ctx.ItemHeight;
            Vec2 dropPos = g_Ctx.DropdownPos;

            // 绘制弹出层背景和边框
            DrawRect({ dropPos.x - 1.f, dropPos.y - 1.f }, { dropWidth + 2.f, dropHeight + 2.f }, { 0.4f, 0.4f, 0.4f, 1.f });
            DrawRect(dropPos, { dropWidth, dropHeight }, { 0.15f, 0.15f, 0.15f, 1.f });

            bool hoveringDropdown = IsMouseHoveringRaw(dropPos, { dropWidth, dropHeight });

            for (size_t i = 0; i < g_Ctx.DropdownItems.size(); ++i) {
                Vec2 itemPos = { dropPos.x, dropPos.y + i * g_Ctx.ItemHeight };
                bool itemHovered = IsMouseHoveringRaw(itemPos, { dropWidth, g_Ctx.ItemHeight });

                if (itemHovered) {
                    DrawRect(itemPos, { dropWidth, g_Ctx.ItemHeight }, { 0.3f, 0.3f, 0.3f, 1.f });
                    if (g_Ctx.MouseClicked) {
                        *g_Ctx.DropdownCurrentItem = static_cast<int>(i);
                        g_Ctx.ActiveDropdownId = 0;
                        g_Ctx.MouseClicked = false; // 吞噬点击
                    }
                }

                Color textColor = (*g_Ctx.DropdownCurrentItem == static_cast<int>(i)) ? Color{ 1.f, 1.f, 0.f, 1.f } : Color{ 1.f, 1.f, 1.f, 1.f };
                DrawTextString(g_Ctx.DropdownItems[i], { itemPos.x + 8.f, itemPos.y + 2.f }, textColor);
            }

            // 点击外部关闭下拉框
            if (g_Ctx.MouseClicked && !hoveringDropdown) {
                g_Ctx.ActiveDropdownId = 0;
            }
            if (hoveringDropdown) {
                g_Ctx.MouseClicked = false;
            }
        }

        // --- 原有绘制 ColorPicker 的逻辑 ---
        if (g_Ctx.ActiveColorPickerId != 0 && g_Ctx.ColorPickerR) {
            Vec2 popupPos = g_Ctx.ColorPickerPos;
            float padding = CPConfig::Padding;
            float svSize = CPConfig::SVSize;
            float hueWidth = CPConfig::HueWidth;
            float alphaWidth = CPConfig::AlphaWidth;
            float spacing = CPConfig::Spacing;

            float hexBoxHeight = std::max(24.f, g_Ctx.ItemHeight + 4.f);
            float popupWidth = padding + svSize + spacing + hueWidth + spacing + alphaWidth + padding;
            float popupHeight = padding + svSize + spacing + hexBoxHeight + padding;

            Vec2 svPos = { popupPos.x + padding, popupPos.y + padding };
            Vec2 huePos = { svPos.x + svSize + spacing, svPos.y };
            Vec2 alphaPos = { huePos.x + hueWidth + spacing, svPos.y };
            Vec2 hexPos = { svPos.x, svPos.y + svSize + spacing };
            Vec2 hexSize = { popupWidth - padding * 2.f, hexBoxHeight };

            bool popupHovered = IsMouseHoveringRaw({ popupPos.x - 2.f, popupPos.y - 2.f }, { popupWidth + 4.f, popupHeight + 4.f });
            bool svHovered = IsMouseHoveringRaw(svPos, { svSize, svSize });
            bool hueHovered = IsMouseHoveringRaw(huePos, { hueWidth, svSize });
            bool alphaHovered = IsMouseHoveringRaw(alphaPos, { alphaWidth, svSize });
            bool hexHovered = IsMouseHoveringRaw(hexPos, hexSize);

            if (g_Ctx.MouseDown) {
                if (!g_Ctx.IsDraggingSV && !g_Ctx.IsDraggingHue && !g_Ctx.IsDraggingAlpha && !g_Ctx.IsDraggingColorPicker) {
                    if (svHovered) g_Ctx.IsDraggingSV = true;
                    else if (hueHovered) g_Ctx.IsDraggingHue = true;
                    else if (alphaHovered) g_Ctx.IsDraggingAlpha = true;
                    else if (popupHovered && !hexHovered) {
                        if (g_Ctx.MouseClicked) {
                            g_Ctx.IsDraggingColorPicker = true;
                            g_Ctx.ColorPickerDragOffset.x = g_Ctx.MousePos.x - popupPos.x;
                            g_Ctx.ColorPickerDragOffset.y = g_Ctx.MousePos.y - popupPos.y;
                            g_Ctx.MouseClicked = false;
                        }
                    }
                }

                if (g_Ctx.IsDraggingSV) {
                    g_Ctx.ColorPickerS = std::clamp((g_Ctx.MousePos.x - svPos.x) / svSize, 0.f, 1.f);
                    g_Ctx.ColorPickerV = 1.0f - std::clamp((g_Ctx.MousePos.y - svPos.y) / svSize, 0.f, 1.f);
                    HSVtoRGB(g_Ctx.ColorPickerH, g_Ctx.ColorPickerS, g_Ctx.ColorPickerV, *g_Ctx.ColorPickerR, *g_Ctx.ColorPickerG, *g_Ctx.ColorPickerB);
                }
                else if (g_Ctx.IsDraggingHue) {
                    g_Ctx.ColorPickerH = std::clamp((g_Ctx.MousePos.y - huePos.y) / svSize, 0.f, 1.f);
                    HSVtoRGB(g_Ctx.ColorPickerH, g_Ctx.ColorPickerS, g_Ctx.ColorPickerV, *g_Ctx.ColorPickerR, *g_Ctx.ColorPickerG, *g_Ctx.ColorPickerB);
                }
                else if (g_Ctx.IsDraggingAlpha) {
                    *g_Ctx.ColorPickerA = 1.0f - std::clamp((g_Ctx.MousePos.y - alphaPos.y) / svSize, 0.f, 1.f);
                }
                else if (g_Ctx.IsDraggingColorPicker) {
                    g_Ctx.ColorPickerPos.x = g_Ctx.MousePos.x - g_Ctx.ColorPickerDragOffset.x;
                    g_Ctx.ColorPickerPos.y = g_Ctx.MousePos.y - g_Ctx.ColorPickerDragOffset.y;
                    popupPos = g_Ctx.ColorPickerPos;

                    svPos = { popupPos.x + padding, popupPos.y + padding };
                    huePos = { svPos.x + svSize + spacing, svPos.y };
                    alphaPos = { huePos.x + hueWidth + spacing, svPos.y };
                    hexPos = { svPos.x, svPos.y + svSize + spacing };
                    popupHovered = IsMouseHoveringRaw({ popupPos.x - 2.f, popupPos.y - 2.f }, { popupWidth + 4.f, popupHeight + 4.f });
                }
            }
            else {
                g_Ctx.IsDraggingSV = false;
                g_Ctx.IsDraggingHue = false;
                g_Ctx.IsDraggingAlpha = false;
                g_Ctx.IsDraggingColorPicker = false;
            }

            DrawRect({ popupPos.x - 2.f, popupPos.y - 2.f }, { popupWidth + 4.f, popupHeight + 4.f }, { 0.3f, 0.3f, 0.3f, 1.f });
            DrawRect(popupPos, { popupWidth, popupHeight }, { 0.08f, 0.08f, 0.08f, 1.f });

            if (g_Ctx.MouseClicked && hexHovered) {
                g_Ctx.IsTypingHex = true;
                g_Ctx.MouseClicked = false;

                uint8_t r8 = static_cast<uint8_t>(*g_Ctx.ColorPickerR * 255.f);
                uint8_t g8 = static_cast<uint8_t>(*g_Ctx.ColorPickerG * 255.f);
                uint8_t b8 = static_cast<uint8_t>(*g_Ctx.ColorPickerB * 255.f);
                uint8_t a8 = static_cast<uint8_t>(*g_Ctx.ColorPickerA * 255.f);
                g_Ctx.HexInputBuffer = std::format("{:02X}{:02X}{:02X}{:02X}", r8, g8, b8, a8);
            }
            else if (g_Ctx.MouseDown && !hexHovered && g_Ctx.IsTypingHex) {
                g_Ctx.IsTypingHex = false;
                ApplyHexInput();
            }

            int svSteps = 40;
            float svStepSize = svSize / svSteps;
            for (int i = 0; i < svSteps; ++i) {
                for (int j = 0; j < svSteps; ++j) {
                    float s = (float)i / (svSteps - 1);
                    float v = 1.0f - (float)j / (svSteps - 1);
                    float r, g, b;
                    HSVtoRGB(g_Ctx.ColorPickerH, s, v, r, g, b);
                    DrawRect({ svPos.x + i * svStepSize, svPos.y + j * svStepSize }, { svStepSize + 0.5f, svStepSize + 0.5f }, { r, g, b, 1.f });
                }
            }

            int hueSteps = 40;
            float hueStepSize = svSize / hueSteps;
            for (int i = 0; i < hueSteps; ++i) {
                float h = (float)i / (hueSteps - 1);
                float r, g, b;
                HSVtoRGB(h, 1.f, 1.f, r, g, b);
                DrawRect({ huePos.x, huePos.y + i * hueStepSize }, { hueWidth, hueStepSize + 0.5f }, { r, g, b, 1.f });
            }

            int alphaSteps = 40;
            float alphaStepSize = svSize / alphaSteps;
            for (int i = 0; i < alphaSteps; ++i) {
                float a = 1.0f - (float)i / (alphaSteps - 1);
                DrawRect({ alphaPos.x, alphaPos.y + i * alphaStepSize }, { alphaWidth, alphaStepSize + 0.5f }, { *g_Ctx.ColorPickerR, *g_Ctx.ColorPickerG, *g_Ctx.ColorPickerB, a });
            }

            DrawRect(hexPos, hexSize, { 0.15f, 0.15f, 0.15f, 1.f });

            std::string displayHex;
            if (g_Ctx.IsTypingHex) {
                displayHex = "#" + g_Ctx.HexInputBuffer + "_";
            }
            else {
                uint8_t r8 = static_cast<uint8_t>(*g_Ctx.ColorPickerR * 255.f);
                uint8_t g8 = static_cast<uint8_t>(*g_Ctx.ColorPickerG * 255.f);
                uint8_t b8 = static_cast<uint8_t>(*g_Ctx.ColorPickerB * 255.f);
                uint8_t a8 = static_cast<uint8_t>(*g_Ctx.ColorPickerA * 255.f);
                displayHex = std::format("#{:02X}{:02X}{:02X}{:02X}", r8, g8, b8, a8);
            }

            Vec2 textSize = MeasureTextSize(displayHex);
            float textY = hexPos.y + (hexBoxHeight - textSize.y) / 2.f;
            DrawTextString(displayHex, { hexPos.x + 8.f, textY }, { 1.f, 1.f, 1.f, 1.f });

            Vec2 cursorSV = { svPos.x + g_Ctx.ColorPickerS * svSize, svPos.y + (1.f - g_Ctx.ColorPickerV) * svSize };
            DrawRect({ cursorSV.x - 4.f, cursorSV.y - 4.f }, { 8.f, 8.f }, { 0.f, 0.f, 0.f, 1.f });
            DrawRect({ cursorSV.x - 3.f, cursorSV.y - 3.f }, { 6.f, 6.f }, { 1.f, 1.f, 1.f, 1.f });
            DrawRect({ cursorSV.x - 2.f, cursorSV.y - 2.f }, { 4.f, 4.f }, { *g_Ctx.ColorPickerR, *g_Ctx.ColorPickerG, *g_Ctx.ColorPickerB, 1.f });

            Vec2 cursorHue = { huePos.x - 2.f, huePos.y + g_Ctx.ColorPickerH * svSize - 2.f };
            DrawRect(cursorHue, { hueWidth + 4.f, 4.f }, { 0.f, 0.f, 0.f, 1.f });
            DrawRect({ cursorHue.x + 1.f, cursorHue.y + 1.f }, { hueWidth + 2.f, 2.f }, { 1.f, 1.f, 1.f, 1.f });

            Vec2 cursorAlpha = { alphaPos.x - 2.f, alphaPos.y + (1.f - *g_Ctx.ColorPickerA) * svSize - 2.f };
            DrawRect(cursorAlpha, { alphaWidth + 4.f, 4.f }, { 0.f, 0.f, 0.f, 1.f });
            DrawRect({ cursorAlpha.x + 1.f, cursorAlpha.y + 1.f }, { alphaWidth + 2.f, 2.f }, { 1.f, 1.f, 1.f, 1.f });

            if (g_Ctx.MouseClicked && !popupHovered) {
                g_Ctx.ActiveColorPickerId = 0;
                g_Ctx.IsTypingHex = false;
            }
            if (popupHovered) {
                g_Ctx.MouseClicked = false;
            }
        }
    }

    inline bool Begin(std::string_view name) {
        std::string_view display; size_t id;
        ParseLabel(name, display, id);

        // 栈平衡检查：递增 Begin 栈计数
        g_Ctx.BeginStack++;

        // 动态标题栏高度
        float titleBarHeight = std::max(30.f, g_Ctx.ItemHeight + 10.f);

        // --- 1. 提前处理鼠标输入与窗口坐标更新，防止渲染滞后导致拖拽发生漂移 ---

        // 整个窗口区域都可以拖拽（不仅仅是标题栏）
        Vec2 wholeWindowSize = g_Ctx.WindowSize;
        bool hoveringWholeWindow = IsMouseHovering(g_Ctx.WindowPos, wholeWindowSize);

        // 检查是否悬停在调整大小三角形上
        float triSize = g_Ctx.ResizeTriangleSize;
        Vec2 triPos = {
            g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - triSize,
            g_Ctx.WindowPos.y + g_Ctx.WindowSize.y - triSize
        };
        g_Ctx.IsHoveringResize = IsMouseHoveringRaw(triPos, { triSize, triSize });

        // 检查是否有其他控件正在被拖拽
        bool isOtherDragging = (g_Ctx.DraggingSliderId != 0) ||
            g_Ctx.IsDraggingSV ||
            g_Ctx.IsDraggingHue ||
            g_Ctx.IsDraggingAlpha ||
            g_Ctx.IsDraggingColorPicker;

        // 只有当前没有其他控件被拖拽，且IsDragging已经是true（之前已激活），才继续拖拽
        // 新激活拖拽需要检查 isOtherDragging
        if (!g_Ctx.IsDragging && hoveringWholeWindow && g_Ctx.MouseClicked && !g_Ctx.IsHoveringResize && !isOtherDragging) {
            g_Ctx.IsDragging = true;
            g_Ctx.DragOffset.x = g_Ctx.MousePos.x - g_Ctx.WindowPos.x;
            g_Ctx.DragOffset.y = g_Ctx.MousePos.y - g_Ctx.WindowPos.y;
        }

        // 如果正在拖拽，检查是否应该停止
        if (g_Ctx.IsDragging && isOtherDragging) {
            g_Ctx.IsDragging = false;
        }

        // 处理调整大小（同样需要检查是否有其他控件正在被拖拽）
        if (!g_Ctx.IsResizing && g_Ctx.IsHoveringResize && g_Ctx.MouseClicked && !isOtherDragging) {
            g_Ctx.IsResizing = true;
            g_Ctx.ResizeStartPos = g_Ctx.MousePos;
            g_Ctx.ResizeStartSize = g_Ctx.WindowSize;
        }

        // 如果正在调整大小，检查是否应该停止
        if (g_Ctx.IsResizing && isOtherDragging) {
            g_Ctx.IsResizing = false;
        }

        if (g_Ctx.IsDragging) {
            g_Ctx.WindowPos.x = g_Ctx.MousePos.x - g_Ctx.DragOffset.x;
            g_Ctx.WindowPos.y = g_Ctx.MousePos.y - g_Ctx.DragOffset.y;
        }

        if (g_Ctx.IsResizing) {
            Vec2 delta = {
                g_Ctx.MousePos.x - g_Ctx.ResizeStartPos.x,
                g_Ctx.MousePos.y - g_Ctx.ResizeStartPos.y
            };

            // 设置最小尺寸，防止缩小到消失
            float minWidth = 200.f;
            float minHeight = 150.f;

            g_Ctx.WindowSize.x = std::max(minWidth, g_Ctx.ResizeStartSize.x + delta.x);
            g_Ctx.WindowSize.y = std::max(minHeight, g_Ctx.ResizeStartSize.y + delta.y);

            if (!g_Ctx.MouseDown) {
                g_Ctx.IsResizing = false;
            }
        }

        // --- 2. 此时再开始绘制背景，使用更新后的 WindowPos 和 WindowSize ---

        DrawRect(g_Ctx.WindowPos, g_Ctx.WindowSize, { 0.1f, 0.1f, 0.1f, 0.9f });
        DrawRect(g_Ctx.WindowPos, { g_Ctx.WindowSize.x, titleBarHeight }, { 0.2f, 0.2f, 0.2f, 1.f });
        DrawTextString(display, { g_Ctx.WindowPos.x + 10.f, g_Ctx.WindowPos.y + 7.f }, { 1.f, 1.f, 1.f, 1.f });

        // 绘制调整大小的三角形
        {
            float x = g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - triSize;
            float y = g_Ctx.WindowPos.y + g_Ctx.WindowSize.y - triSize;

            // 三角形颜色：悬停或拖拽时变亮
            Color triColor = (g_Ctx.IsHoveringResize || g_Ctx.IsResizing) ?
                Color{ 0.8f, 0.8f, 0.8f, 1.f } :
                Color{ 0.5f, 0.5f, 0.5f, 1.f };

            // 绘制三角形背景（小矩形区域）
            DrawRect({ x, y }, { triSize, triSize }, { 0.15f, 0.15f, 0.15f, 0.9f });

            // 使用线条绘制三角形
            float pad = 3.f;
            SDK::FLinearColor ueColor{ triColor.r, triColor.g, triColor.b, triColor.a };

            // 三角形三个顶点
            SDK::FVector2D p1{ static_cast<double>(x + pad), static_cast<double>(y + triSize - pad) };
            SDK::FVector2D p2{ static_cast<double>(x + triSize - pad), static_cast<double>(y + triSize - pad) };
            SDK::FVector2D p3{ static_cast<double>(x + triSize - pad), static_cast<double>(y + pad) };

            if (g_Ctx.Canvas) {
                g_Ctx.Canvas->K2_DrawLine(p1, p2, 1.0f, ueColor);
                g_Ctx.Canvas->K2_DrawLine(p2, p3, 1.0f, ueColor);
                g_Ctx.Canvas->K2_DrawLine(p3, p1, 1.0f, ueColor);
            }
        }

        // 设置光标起始位置
        g_Ctx.Cursor = { g_Ctx.WindowPos.x + g_Ctx.Padding, g_Ctx.WindowPos.y + titleBarHeight + g_Ctx.Padding };

        // 设置剪裁区域为窗口内部区域（不包括标题栏上方，但包括整个窗口内部）
        PushClipRect(
            { g_Ctx.WindowPos.x, g_Ctx.WindowPos.y + titleBarHeight },
            { g_Ctx.WindowPos.x + g_Ctx.WindowSize.x, g_Ctx.WindowPos.y + g_Ctx.WindowSize.y }
        );

        return true;
    }

    inline void End() {
        // 栈平衡检查：递减 Begin 栈计数
        g_Ctx.BeginStack--;

        // 清除剪裁区域
        PopClipRect();

        // 清理调整大小状态
        if (!g_Ctx.MouseDown) {
            g_Ctx.IsResizing = false;
        }
        RenderPopups();
        g_Ctx.MouseClicked = false;
        g_Ctx.RightMouseClicked = false; // 新增清除右键单帧状态
        // 清理单帧按键状态
        memset(g_Ctx.KeyPressed, 0, sizeof(g_Ctx.KeyPressed));

        // 检查并绘制错误信息
        CheckAndDrawErrors();
    }

    inline bool BeginTabBar(std::string_view name) {
        std::string_view display; size_t id; ParseLabel(name, display, id);

        // 栈平衡检查：递增 TabBar 栈计数
        g_Ctx.TabBarStack++;

        g_Ctx.TabCursor = g_Ctx.Cursor;
        g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
        DrawRect({ g_Ctx.WindowPos.x, g_Ctx.Cursor.y }, { g_Ctx.WindowSize.x, 2.f }, { 0.3f, 0.3f, 0.3f, 1.f });
        g_Ctx.Cursor.y += 2.f + g_Ctx.Padding; // 修复对齐：增加分割线后的下边距，防止内部控件紧贴顶部线段
        return true;
    }

    inline void EndTabBar() {
        // 栈平衡检查：递减 TabBar 栈计数
        g_Ctx.TabBarStack--;
    }

    inline bool BeginTabItem(std::string_view name) {
        std::string_view display; size_t id; ParseLabel(name, display, id);

        // 栈平衡检查：递增 TabItem 栈计数
        g_Ctx.TabItemStack++;

        Vec2 tabSize = MeasureTextSize(display);
        tabSize.x += 20.f; tabSize.y = g_Ctx.ItemHeight;

        bool hovered = IsMouseHovering(g_Ctx.TabCursor, tabSize);
        if (hovered && g_Ctx.MouseClicked) {
            if (g_Ctx.ActiveTabId != id) {
                // 切换Tab时自动清空Slider等焦点状态
                g_Ctx.FocusedSliderId = 0;
            }
            g_Ctx.ActiveTabId = id;
        }
        if (g_Ctx.ActiveTabId == 0) g_Ctx.ActiveTabId = id;

        bool isActive = (g_Ctx.ActiveTabId == id);
        g_Ctx.InActiveTab = isActive;

        Color bgColor = isActive ? Color{ 0.3f, 0.3f, 0.3f, 1.f } : (hovered ? Color{ 0.2f, 0.2f, 0.2f, 1.f } : Color{ 0.15f, 0.15f, 0.15f, 1.f });
        DrawRect(g_Ctx.TabCursor, tabSize, bgColor);
        DrawTextString(display, { g_Ctx.TabCursor.x + 10.f, g_Ctx.TabCursor.y + 2.f }, isActive ? Color{ 1.f, 1.f, 1.f, 1.f } : Color{ 0.7f, 0.7f, 0.7f, 1.f });

        g_Ctx.TabCursor.x += tabSize.x + 5.f;
        return isActive;
    }

    inline void EndTabItem() {
        // 栈平衡检查：递减 TabItem 栈计数
        g_Ctx.TabItemStack--;
        g_Ctx.InActiveTab = true;
    }

    inline bool ComboBox(std::string_view name, int* current_item, const std::vector<std::string>& items) {
        if (!g_Ctx.InActiveTab) return false;
        std::string_view display; size_t id; ParseLabel(name, display, id);

        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, g_Ctx.ItemHeight })) {
            g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
            return false;
        }

        DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + 2.f }, { 1.f, 1.f, 1.f, 1.f });

        float controlOffsetX = GetControlOffsetX();
        float boxWidth = std::max(100.f, g_Ctx.WindowSize.x - controlOffsetX - g_Ctx.Padding * 8.f);
        Vec2 boxPos = { g_Ctx.Cursor.x + controlOffsetX, g_Ctx.Cursor.y };
        Vec2 boxSize = { boxWidth, g_Ctx.ItemHeight };

        bool hovered = IsMouseHovering(boxPos, boxSize);
        if (hovered && g_Ctx.MouseClicked) {
            if (g_Ctx.ActiveDropdownId == id) {
                g_Ctx.ActiveDropdownId = 0;
            }
            else {
                g_Ctx.ActiveDropdownId = id;
                g_Ctx.DropdownCurrentItem = current_item;
                g_Ctx.DropdownItems = items;
                g_Ctx.DropdownPos = { boxPos.x, boxPos.y + boxSize.y };
                g_Ctx.DropdownSize = { boxWidth, 0.f };
            }
            g_Ctx.MouseClicked = false;
        }

        DrawRect(boxPos, boxSize, hovered ? Color{ 0.4f, 0.4f, 0.4f, 1.f } : Color{ 0.3f, 0.3f, 0.3f, 1.f });

        std::string currentText = (*current_item >= 0 && *current_item < static_cast<int>(items.size())) ? items[*current_item] : "Unknown";
        DrawTextString(currentText, { boxPos.x + 8.f, boxPos.y + 2.f }, { 1.f, 1.f, 1.f, 1.f });

        // 绘制下拉框展开三角：如果三角形区域由于滚动越过了边界导致部分不可见，则直接隐藏它以避免画线溢出剪裁框
        float triSize = 6.f;
        Vec2 triPos = { boxPos.x + boxSize.x - 12.f, boxPos.y + boxSize.y / 2.f - triSize / 2.f };
        if (g_Ctx.Canvas && IsRectFullyVisible(triPos, { triSize, triSize })) {
            SDK::FLinearColor ueColor{ 0.8f, 0.8f, 0.8f, 1.f };
            SDK::FVector2D p1{ static_cast<double>(triPos.x), static_cast<double>(triPos.y) };
            SDK::FVector2D p2{ static_cast<double>(triPos.x + triSize), static_cast<double>(triPos.y) };
            SDK::FVector2D p3{ static_cast<double>(triPos.x + triSize / 2.f), static_cast<double>(triPos.y + triSize) };
            g_Ctx.Canvas->K2_DrawLine(p1, p2, 1.0f, ueColor);
            g_Ctx.Canvas->K2_DrawLine(p2, p3, 1.0f, ueColor);
            g_Ctx.Canvas->K2_DrawLine(p3, p1, 1.0f, ueColor);
        }

        g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
        return hovered && g_Ctx.MouseClicked;
    }

    inline void CheckBox(std::string_view name, bool* value) {
        if (!g_Ctx.InActiveTab) return;
        std::string_view display; size_t id; ParseLabel(name, display, id);

        Vec2 boxSize = { g_Ctx.ItemHeight, g_Ctx.ItemHeight };

        // 检查是否在可见区域内
        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, g_Ctx.ItemHeight })) {
            g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
            return;
        }

        bool hovered = IsMouseHovering(g_Ctx.Cursor, { g_Ctx.WindowSize.x, g_Ctx.ItemHeight });

        if (hovered && g_Ctx.MouseClicked) { *value = !(*value); }

        DrawRect(g_Ctx.Cursor, boxSize, hovered ? Color{ 0.4f, 0.4f, 0.4f, 1.f } : Color{ 0.3f, 0.3f, 0.3f, 1.f });
        if (*value) {
            float checkPad = std::max(4.f, boxSize.x * 0.2f);
            DrawRect({ g_Ctx.Cursor.x + checkPad, g_Ctx.Cursor.y + checkPad }, { boxSize.x - checkPad * 2.f, boxSize.y - checkPad * 2.f }, { 0.2f, 0.8f, 0.2f, 1.f });
        }

        DrawTextString(display, { g_Ctx.Cursor.x + boxSize.x + 10.f, g_Ctx.Cursor.y + 2.f }, { 1.f, 1.f, 1.f, 1.f });
        g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
    }

    inline bool Button(std::string_view name) {
        if (!g_Ctx.InActiveTab) return false;
        std::string_view display; size_t id; ParseLabel(name, display, id);

        Vec2 size = MeasureTextSize(display);
        size.x += 20.f; size.y = g_Ctx.ItemHeight;

        // 检查是否在可见区域内
        if (!IsRectVisible(g_Ctx.Cursor, size)) {
            g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
            return false;
        }

        bool hovered = IsMouseHovering(g_Ctx.Cursor, size);
        bool clicked = hovered && g_Ctx.MouseClicked;

        DrawRect(g_Ctx.Cursor, size, hovered ? Color{ 0.4f, 0.4f, 0.4f, 1.f } : Color{ 0.3f, 0.3f, 0.3f, 1.f });
        DrawTextString(display, { g_Ctx.Cursor.x + 10.f, g_Ctx.Cursor.y + 2.f }, { 1.f, 1.f, 1.f, 1.f });

        g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
        return clicked;
    }

    inline void Slider(std::string_view name, float* value, float min_val, float max_val, float step = 0.f) {
        if (!g_Ctx.InActiveTab) return;
        std::string_view display; size_t id; ParseLabel(name, display, id);

        // 检查是否在可见区域内
        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, g_Ctx.ItemHeight })) {
            g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
            return;
        }

        float controlOffsetX = GetControlOffsetX();
        float sliderWidth = std::max(50.f, g_Ctx.WindowSize.x - controlOffsetX - g_Ctx.Padding * 8.f);
        Vec2 sliderPos = { g_Ctx.Cursor.x + controlOffsetX, g_Ctx.Cursor.y };
        Vec2 size = { sliderWidth, g_Ctx.ItemHeight };

        bool hovered = IsMouseHovering(sliderPos, size);

        // 处理点击获取焦点/拖拽
        if (g_Ctx.MouseClicked) {
            if (hovered) {
                g_Ctx.DraggingSliderId = id;
                g_Ctx.FocusedSliderId = id;
            }
            else {
                if (g_Ctx.FocusedSliderId == id) g_Ctx.FocusedSliderId = 0;
            }
        }

        if (!g_Ctx.MouseDown && g_Ctx.DraggingSliderId == id) {
            g_Ctx.DraggingSliderId = 0;
        }

        if (g_Ctx.DraggingSliderId == id) {
            float ratio = std::clamp((g_Ctx.MousePos.x - sliderPos.x) / sliderWidth, 0.f, 1.f);
            float newVal = min_val + ratio * (max_val - min_val);
            if (step > 0.f) {
                newVal = std::round(newVal / step) * step;
            }
            *value = std::clamp(newVal, min_val, max_val);
        }

        // 支持方向键微调
        if (g_Ctx.FocusedSliderId == id) {
            if (g_Ctx.KeyPressed[VK_LEFT]) {
                float modifyVal = *value - (step > 0.f ? step : (max_val - min_val) * 0.01f);
                if (step > 0.f) modifyVal = std::round(modifyVal / step) * step;
                *value = std::clamp(modifyVal, min_val, max_val);
            }
            if (g_Ctx.KeyPressed[VK_RIGHT]) {
                float modifyVal = *value + (step > 0.f ? step : (max_val - min_val) * 0.01f);
                if (step > 0.f) modifyVal = std::round(modifyVal / step) * step;
                *value = std::clamp(modifyVal, min_val, max_val);
            }
        }

        DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + 2.f }, { 1.f, 1.f, 1.f, 1.f });
        DrawRect(sliderPos, size, { 0.3f, 0.3f, 0.3f, 1.f });

        float fillWidth = std::clamp((*value - min_val) / (max_val - min_val), 0.f, 1.f) * sliderWidth;
        DrawRect(sliderPos, { fillWidth, size.y }, { 0.2f, 0.6f, 1.0f, 1.f });

        // 焦点黄框提示支持键盘操控
        if (g_Ctx.FocusedSliderId == id) {
            DrawRect({ sliderPos.x - 1.f, sliderPos.y - 1.f }, { size.x + 2.f, 1.f }, { 1.f, 1.f, 0.f, 1.f });
            DrawRect({ sliderPos.x - 1.f, sliderPos.y + size.y }, { size.x + 2.f, 1.f }, { 1.f, 1.f, 0.f, 1.f });
            DrawRect({ sliderPos.x - 1.f, sliderPos.y }, { 1.f, size.y }, { 1.f, 1.f, 0.f, 1.f });
            DrawRect({ sliderPos.x + size.x, sliderPos.y }, { 1.f, size.y }, { 1.f, 1.f, 0.f, 1.f });
        }

        std::string valStr = std::format("{:.2f}", *value);
        DrawTextString(valStr, { sliderPos.x + sliderWidth + 10.f, sliderPos.y + 2.f }, { 1.f, 1.f, 1.f, 1.f });

        g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
    }

    inline void ColorPicker(std::string_view name, float* r, float* g, float* b, float* a) {
        if (!g_Ctx.InActiveTab) return;
        std::string_view display; size_t id; ParseLabel(name, display, id);

        // 检查是否在可见区域内
        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, g_Ctx.ItemHeight })) {
            g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
            return;
        }

        DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + 2.f }, { 1.f, 1.f, 1.f, 1.f });

        float controlOffsetX = GetControlOffsetX();
        Vec2 boxPos = { g_Ctx.Cursor.x + controlOffsetX, g_Ctx.Cursor.y };
        Vec2 boxSize = { 40.f, g_Ctx.ItemHeight };

        bool hovered = IsMouseHovering(boxPos, boxSize);
        if (hovered && g_Ctx.MouseClicked) {
            if (g_Ctx.ActiveColorPickerId == id) {
                g_Ctx.ActiveColorPickerId = 0;
            }
            else {
                g_Ctx.ActiveColorPickerId = id;
                g_Ctx.ColorPickerR = r; g_Ctx.ColorPickerG = g; g_Ctx.ColorPickerB = b; g_Ctx.ColorPickerA = a;
                g_Ctx.ColorPickerPos = { boxPos.x + boxSize.x + 10.f, boxPos.y };
                RGBtoHSV(*r, *g, *b, g_Ctx.ColorPickerH, g_Ctx.ColorPickerS, g_Ctx.ColorPickerV);
            }
            g_Ctx.MouseClicked = false;
        }

        DrawRect(boxPos, boxSize, { *r, *g, *b, *a });

        if (hovered || g_Ctx.ActiveColorPickerId == id) {
            DrawRect({ boxPos.x - 1.f, boxPos.y - 1.f }, { boxSize.x + 2.f, 1.f }, { 1.f, 1.f, 1.f, 1.f });
            DrawRect({ boxPos.x - 1.f, boxPos.y + boxSize.y }, { boxSize.x + 2.f, 1.f }, { 1.f, 1.f, 1.f, 1.f });
            DrawRect({ boxPos.x - 1.f, boxPos.y }, { 1.f, boxSize.y }, { 1.f, 1.f, 1.f, 1.f });
            DrawRect({ boxPos.x + boxSize.x, boxPos.y }, { 1.f, boxSize.y }, { 1.f, 1.f, 1.f, 1.f });
        }

        g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
    }

    // 修复返回值为bool，现在你可以在外部直接用 if (Shadow::HotKey(..)) 判定热键是否生效，完全支持鼠标
    inline bool HotKey(std::string_view name, int* hotkey) {
        if (!g_Ctx.InActiveTab) return false;
        std::string_view display; size_t id; ParseLabel(name, display, id);

        // 检查是否在可见区域内
        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, g_Ctx.ItemHeight })) {
            g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
            return false;
        }

        DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + 2.f }, { 1.f, 1.f, 1.f, 1.f });

        float controlOffsetX = GetControlOffsetX();
        bool isAssigning = (g_Ctx.AssigningHotkey == hotkey);
        std::string keyName = isAssigning ? "[Press Key]" : std::format("[{}]", GetKeyName(*hotkey));

        Vec2 btnSize = { MeasureTextSize(keyName).x + 16.f, g_Ctx.ItemHeight };
        btnSize.x = std::max(btnSize.x, 80.f);
        Vec2 btnPos = { g_Ctx.Cursor.x + controlOffsetX, g_Ctx.Cursor.y };

        bool btnHovered = IsMouseHovering(btnPos, btnSize);
        if (btnHovered && g_Ctx.MouseClicked) {
            g_Ctx.AssigningHotkey = hotkey;
            g_Ctx.MouseClicked = false;
        }

        DrawRect(btnPos, btnSize, btnHovered ? Color{ 0.4f, 0.4f, 0.4f, 1.f } : Color{ 0.3f, 0.3f, 0.3f, 1.f });
        DrawTextString(keyName, { btnPos.x + 8.f, btnPos.y + 2.f }, { 1.f, 1.f, 1.f, 1.f });

        g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
        return *hotkey != 0 && g_Ctx.KeyStates[*hotkey];
    }

    inline void HotKey(std::string_view name, int* hotkey, bool* is_active, HotkeyMode* hotkey_mode) {
        if (!g_Ctx.InActiveTab) return;
        std::string_view display; size_t id; ParseLabel(name, display, id);

        // 检查是否在可见区域内
        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, g_Ctx.ItemHeight })) {
            g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
            return;
        }

        DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + 2.f }, { 1.f, 1.f, 1.f, 1.f });

        float controlOffsetX = GetControlOffsetX();
        bool isAssigning = (g_Ctx.AssigningHotkey == hotkey);
        std::vector<std::string> modeStrs = { "None", "Hold On", "Toggle On", "Hold Off", "Always On" };

        std::string keyName = isAssigning ? "[Press Key]" : std::format("[{}]", GetKeyName(*hotkey));

        Vec2 btnSize = { MeasureTextSize(keyName).x + 16.f, g_Ctx.ItemHeight };
        btnSize.x = std::max(btnSize.x, 80.f);
        Vec2 btnPos = { g_Ctx.Cursor.x + controlOffsetX, g_Ctx.Cursor.y };

        bool btnHovered = IsMouseHovering(btnPos, btnSize);

        if (btnHovered) {
            if (g_Ctx.MouseClicked) {
                // 左键设置按键
                g_Ctx.AssigningHotkey = hotkey;
                g_Ctx.MouseClicked = false;
            }
            else if (g_Ctx.RightMouseClicked) {
                // 右键直接呼出 Hotkey Mode 的下拉菜单，不再显示额外的框
                if (g_Ctx.ActiveDropdownId == id) {
                    g_Ctx.ActiveDropdownId = 0;
                }
                else {
                    g_Ctx.ActiveDropdownId = id;
                    g_Ctx.DropdownCurrentItem = reinterpret_cast<int*>(hotkey_mode);
                    g_Ctx.DropdownItems = modeStrs;
                    g_Ctx.DropdownPos = { btnPos.x, btnPos.y + btnSize.y };
                    g_Ctx.DropdownSize = { 100.f, 0.f };
                }
                g_Ctx.RightMouseClicked = false;
            }
        }

        // 绘制唯一的主按钮
        DrawRect(btnPos, btnSize, btnHovered ? Color{ 0.4f, 0.4f, 0.4f, 1.f } : Color{ 0.3f, 0.3f, 0.3f, 1.f });
        DrawTextString(keyName, { btnPos.x + 8.f, btnPos.y + 2.f }, { 1.f, 1.f, 1.f, 1.f });

        // 计算当前是否被激活
        switch (*hotkey_mode) {
        case HotkeyMode::None:      *is_active = false; break;
        case HotkeyMode::HoldOn:    *is_active = g_Ctx.KeyStates[*hotkey]; break;
        case HotkeyMode::HoldOff:   *is_active = !g_Ctx.KeyStates[*hotkey]; break;
        case HotkeyMode::ToggleOn:  *is_active = g_Ctx.HotkeyToggles[*hotkey]; break;
        case HotkeyMode::AlwaysOn:  *is_active = true; break;
        }

        // 绘制状态指示小圆（方）点，紧贴在主按钮的右侧
        float dotSize = std::max(6.f, g_Ctx.ItemHeight * 0.4f);
        float dotOffset = (g_Ctx.ItemHeight - dotSize) / 2.f;
        DrawRect({ btnPos.x + btnSize.x + 10.f, g_Ctx.Cursor.y + dotOffset }, { dotSize, dotSize },
            *is_active ? Color{ 0.f, 1.f, 0.f, 1.f } : Color{ 1.f, 0.f, 0.f, 1.f });

        g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
    }
}