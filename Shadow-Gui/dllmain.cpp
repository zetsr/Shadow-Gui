#define U8(str) reinterpret_cast<const char*>(u8##str)
#include <windows.h>
#include <iostream>
#include "external/AOBScan/AOBScan.hpp"
#include "external/MinHook/include/MinHook.h"
#include "external/CppSDK/SDK.hpp"
#include "external/Shadow-Gui/include/Shadow.h"
#include "shadow_demo.h"

namespace Hook {
    bool bShowMenu = false;
    int keyMenu = VK_F1;

    HWND g_hwnd = NULL;
    WNDPROC oWndProc = NULL;

    BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == GetCurrentProcessId()) {
            if (IsWindowVisible(hwnd) && GetWindow(hwnd, GW_OWNER) == NULL) {
                LONG style = GetWindowLongPtr(hwnd, GWL_STYLE);
                if ((style & WS_CAPTION) || (style & WS_POPUP)) {
                    g_hwnd = hwnd;
                    return FALSE;
                }
            }
        }
        return TRUE;
    }

    bool GetMouseCursorVisible() {
        if (auto world = SDK::UWorld::GetWorld()) {
            if (world->OwningGameInstance && world->OwningGameInstance->LocalPlayers.IsValidIndex(0)) {
                if (auto pc = world->OwningGameInstance->LocalPlayers[0]->PlayerController) {
                    return pc->bShowMouseCursor;
                }
            }
        }
        return false;
    }

    void SetMouseCursorVisible(bool visible) {
        if (auto world = SDK::UWorld::GetWorld()) {
            if (world->OwningGameInstance && world->OwningGameInstance->LocalPlayers.IsValidIndex(0)) {
                if (auto pc = world->OwningGameInstance->LocalPlayers[0]->PlayerController) {
                    pc->bShowMouseCursor = visible;
                }
            }
        }
    }

    LRESULT APIENTRY WndProcHook(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        // 始终处理全局热键（菜单打开或关闭都需要）
        Shadow::ProcessGlobalHotkeys(hwnd, uMsg, wParam, lParam);

        // 菜单切换键处理（如果正在分配热键，则不切换菜单）
        if (uMsg == WM_KEYDOWN && wParam == keyMenu && !Shadow::g_Ctx.AssigningHotkey) {
            bool isFirstPress = ((lParam & (1 << 30)) == 0);
            if (isFirstPress) {
                bShowMenu = !bShowMenu;
            }
        }

        if (bShowMenu) {
            // 键盘消息处理
            if (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP || uMsg == WM_CHAR) {
                // 如果是切换键本身，放行给游戏（防止菜单无法关闭）
                if (wParam == keyMenu) {
                    return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
                }

                // 如果是已注册的全局热键，放行给游戏（热键状态已在 ProcessGlobalHotkeys 中更新）
                if (Shadow::IsHotkeyRegistered(static_cast<int>(wParam))) {
                    return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
                }

                // 先让框架处理按键（更新 KeyPressed, KeyStates 等）
                Shadow::Input(hwnd, uMsg, wParam, lParam);

                // 如果是允许放行的按键，再转发给游戏
                if (Shadow::IsKeyAllowed(static_cast<int>(wParam))) {
                    // 注意：这里不能再次调用 Shadow::Input，因为上面已经调用过了
                    return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
                }

                // 非允许按键，被菜单吃掉
                return 1;
            }

            // 如果是鼠标消息，且不在白名单里，直接阻塞
            if (!Shadow::IsMouseMsgAllowed(uMsg)) {
                Shadow::Input(hwnd, uMsg, wParam, lParam);
                return 1;
            }

            // 鼠标消息在白名单里，也需要先处理，再放行
            Shadow::Input(hwnd, uMsg, wParam, lParam);
            return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
        }

        // 菜单关闭时，也要处理输入以更新全局鼠标状态
        Shadow::Input(hwnd, uMsg, wParam, lParam);
        return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
    }

    typedef void(__fastcall* tPostRender)(SDK::UGameViewportClient* _this, SDK::UCanvas* Canvas);
    tPostRender oPostRender = nullptr;

    void __fastcall hkPostRender(SDK::UGameViewportClient* rcx, SDK::UCanvas* canvas) {
        oPostRender(rcx, canvas);

        Shadow::SetAllowedKeys({ 'W', 'A', 'S', 'D', VK_SPACE }); // 放行常用移动按键

        static bool bWasAnimating = false;
        static bool gameOriginalCursorState = false;

        bool isAnimating = (bShowMenu);

        if (isAnimating) {
            if (!bWasAnimating) {
                gameOriginalCursorState = GetMouseCursorVisible();
            }

            SetMouseCursorVisible(true);
            bWasAnimating = true;
        }
        else if (bWasAnimating) {
            SetMouseCursorVisible(gameOriginalCursorState);
            bWasAnimating = false;
        }

        Shadow::NewFrame(canvas);
        Shadow::UpdateAllHotkeyStates();

        if (bShowMenu) {
            Shadow::ShowDemoWindow();

        }
    }

    void FindPostRender() {
        std::string pattern = "8B C2 35 ?? ?? ?? ?? 44";

             // ASA 8B C2 35 ?? ?? ?? ?? 44
        // DRACONIA 48 8B 01 48 FF A0 ?? ?? ?? ?? CC CC CC CC CC CC 40 53 48 83 EC ?? 48 89
             // DOD 48 8B 01 48 FF A0 ?? ?? ?? ?? CC CC CC CC CC CC 48 89 5C 24 10 48 89 74 24 18 48 89 7C 24 20 55
            // ISLE 48 8B 01 48 FF A0 ?? ?? ?? ?? CC CC CC CC CC CC 40 53 48 83 EC ?? 48 89 6C

        AOB::Result ok = AOB::Scan(pattern);

        if (ok && ok.size() > 0) {
            void* targetAddr = ok[0];

            if (MH_CreateHook(targetAddr, &hkPostRender, reinterpret_cast<LPVOID*>(&oPostRender)) == MH_OK) {
                MH_EnableHook(targetAddr);
            }
        }
    }

    void HookWndProc() {
        EnumWindows(EnumWindowsProc, 0);
        if (g_hwnd) {
            oWndProc = (WNDPROC)SetWindowLongPtr(g_hwnd, GWLP_WNDPROC, (LONG_PTR)WndProcHook);
        }
    }

    void StartAllHooks() {
        SDK::UWorld* pWorld = nullptr;
        while (!pWorld) {
            pWorld = SDK::UWorld::GetWorld();
            if (pWorld && pWorld->OwningGameInstance) break;
            Sleep(1000);
        }

        if (pWorld) {
            Hook::HookWndProc();
            Hook::FindPostRender();
        }
    }
}

void MT(LPVOID lpParam) {
    if (MH_Initialize() != MH_OK) return;
    Hook::StartAllHooks();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        if (HANDLE h = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MT, hModule, 0, nullptr)) {
            CloseHandle(h);
        }
        break;
    case DLL_PROCESS_DETACH:
        if (Hook::g_hwnd && Hook::oWndProc) {
            SetWindowLongPtr(Hook::g_hwnd, GWLP_WNDPROC, (LONG_PTR)Hook::oWndProc);
        }
        MH_Uninitialize();
        break;
    }
    return TRUE;
}