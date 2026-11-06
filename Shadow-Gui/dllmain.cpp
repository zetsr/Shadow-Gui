#include <windows.h>
#include <iostream>
#include "external/AOBScan/AOBScan.hpp"
#include "external/MinHook/include/MinHook.h"
#include "external/CppSDK/SDK.hpp"
#include "src/Shadow.h"
#include "src/Backend_Dumper7.h"
#include "src/Shadow_Demo.h"
#include "examples/Example.h"
#include "examples/Example_Nav.h"
#include "misc/fonts/CangErShuYuanTi_W03.h"

namespace Hook {
    bool bShowMenu = true;
    int keyMenu = VK_F1;
    float currentAlpha = 0.f;
    float fadeSpeed = 5.0f;

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

        if (bShowMenu || currentAlpha > 0.001f) {
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

        /*
        static bool bWasAnimating = false;
        static bool gameOriginalCursorState = false;

        bool isAnimating = (bShowMenu);

        if (isAnimating) {
            if (!bWasAnimating) {
                gameOriginalCursorState = GetMouseCursorVisible();
            }

            // SetMouseCursorVisible(true);
            bWasAnimating = true;
        }
        else if (bWasAnimating) {
            // SetMouseCursorVisible(gameOriginalCursorState);
            bWasAnimating = false;
        }
        */
        Shadow::Dumper7_NewFrame(canvas);
        Shadow::NewFrame();
        Shadow::UpdateAllHotkeyStates();

        float dt = Shadow::GetIO().DeltaTime;

        if (bShowMenu) {
            currentAlpha += fadeSpeed * dt;
            if (currentAlpha > 1.0f) currentAlpha = 1.0f;
        }
        else {
            currentAlpha -= fadeSpeed * dt;
            if (currentAlpha < 0.0f) currentAlpha = 0.0f;
        }

        Shadow::PushStyleVar(Shadow::GuiStyleVar_Alpha, currentAlpha);

        if (currentAlpha > 0.001f) {
            static bool bInit = false;

            /*
            if (!bInit) {
                Shadow::SetNextWindowPos({ 100.f, 100.f });
            }
            */

            /*
			Shadow::PushTextPixelSnap(true);
            Shadow::StyleColorsDark();
            Shadow::PushFont(Shadow::DefaultFont, G_SIZE);
            Example::DrawGUI(); // 不知道为啥在某些游戏会杀死FPS，不过标准控件并没有问题。
            Shadow::PopFont();

            if (!bInit) {
                Shadow::SetNextWindowPos({ 500.f, 100.f });
            }
            */

			Shadow::ShadowFont font = Shadow::LoadFontFromBuffer(g_Fonts::CangErShuYuanTi_W03, sizeof(g_Fonts::CangErShuYuanTi_W03));

            // Shadow::StyleColorsOcean();
            // Shadow::PushFont(font, G_SIZE);
			// Shadow::DefaultFont = font;

            Shadow::PushFont(font, 1.5f);
            Shadow::PushTextPixelSnap();
            Shadow::ShowDemoWindow();
            Shadow::PopTextPixelSnap();
            Shadow::PopFont();

            // Shadow::PopFont();

            // Shadow::StyleColorsGrey();
            // Shadow::PushFont(Shadow::DefaultFont, G_SIZE);
            // Example_Nav::DrawGUI();
            // Shadow::PopFont();
			// Shadow::PopTextPixelSnap();

            // bInit = true;

            auto drawList = Shadow::GetBackgroundDrawList();
            // 测试手动拼接圆角矩形
            /*
            drawList->AddCircleFilled({ 150.f, 200.f }, 50.f, { 1.f, 1.f, 1.f, 1.f }); // 1. 左半圆（实际画完整圆，靠矩形覆盖左半边或直接让它们重叠）
            drawList->AddCircleFilled({ 350.f, 200.f }, 50.f, { 1.f, 1.f, 1.f, 1.f }); // 2. 右半圆
            drawList->AddRectFilled({ 150.f, 150.f }, { 200.f, 100.f }, { 1.f, 1.f, 1.f, 1.f });   // 3. 中间矩形连接两端
            */

			// 圆角填充矩形和圆角矩形边框测试
            /*
            drawList->AddRectFilled({ 150.f, 150.f }, { 200.f, 100.f }, { 1.f, 1.f, 1.f, 0.5f }, 55.f, Shadow::ShadowDrawFlags_RoundCornersTop);
            drawList->AddRectFilled({ 550.f, 150.f }, { 200.f, 100.f }, { 1.f, 0.f, 0.f, 0.25f }, 25.f, Shadow::ShadowDrawFlags_RoundCornersLeft);
            drawList->AddRectFilled({ 1050.f, 150.f }, { 200.f, 100.f }, { 1.f, 0.f, 0.f, 1.f }, 5.f, Shadow::ShadowDrawFlags_RoundCornersBottom);
            drawList->AddRectFilled({ 1550.f, 150.f }, { 200.f, 100.f }, { 1.f, 0.f, 0.f, 1.f }, 10.f);

            drawList->AddRectFilled({ 150.f, 350.f }, { 200.f, 100.f }, { 1.f, 1.f, 1.f, 0.5f });
            drawList->AddRectFilled({ 550.f, 350.f }, { 200.f, 100.f }, { 1.f, 0.f, 0.f, 0.25f });
            drawList->AddRectFilled({ 1050.f, 350.f }, { 200.f, 100.f }, { 1.f, 0.f, 0.f, 1.f }, 0.f);

			drawList->AddRect({ 150.f, 550.f }, { 200.f, 100.f }, { 1.f, 1.f, 1.f, 0.5f }, 1.f, 55.f, Shadow::ShadowDrawFlags_RoundCornersTop);
            drawList->AddRect({ 550.f, 550.f }, { 200.f, 100.f }, { 1.f, 0.f, 0.f, 0.25f }, 1.f, 25.f, Shadow::ShadowDrawFlags_RoundCornersLeft);
            drawList->AddRect({ 1050.f, 550.f }, { 200.f, 100.f }, { 1.f, 0.f, 0.f, 1.f }, 1.f, 5.f, Shadow::ShadowDrawFlags_RoundCornersBottom);
            drawList->AddRect({ 1550.f, 550.f }, { 200.f, 100.f }, { 1.f, 0.f, 0.f, 1.f }, 1.f, 10.f);

            drawList->AddRect({ 150.f, 750.f }, { 200.f, 100.f }, { 1.f, 1.f, 1.f, 0.5f }, 1.f);
            drawList->AddRect({ 550.f, 750.f }, { 200.f, 100.f }, { 1.f, 0.f, 0.f, 0.25f }, 1.f);
            drawList->AddRect({ 1050.f, 750.f }, { 200.f, 100.f }, { 1.f, 0.f, 0.f, 1.f }, 1.f);
            */
        }

        Shadow::PopStyleVar();

        Shadow::Render();
        Shadow::Dumper7_RenderDrawData(Shadow::GetDrawData());
    }

    void FindPostRender() {
        std::string pattern = "40 55 53 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B FA 48 8B D9 E8 ? ? ? ? 48 85";

             // ASA 8B C2 35 ? ? ? ? 44
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