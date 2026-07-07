#define U8(str) reinterpret_cast<const char*>(u8##str)
#include <windows.h>
#include <iostream>
#include "external/AOBScan/AOBScan.hpp"
#include "external/MinHook/include/MinHook.h"
#include "external/CppSDK/SDK.hpp"
#include "external/Shadow-Gui/include/Shadow.h"

namespace Hook {
    bool bShowMenu = true;
    bool bGodMode = false;
    float fSpeed = 1.0f;
    Shadow::Color cESP = { 1.0f, 0.0f, 0.0f, 1.0f };

    int keyAimbot = VK_RBUTTON;
    bool bAimbotActive = false;
    Shadow::HotkeyMode modeAimbot = Shadow::HotkeyMode::HoldOn;

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
        Shadow::Input(hwnd, uMsg, wParam, lParam);

        if (uMsg == WM_KEYDOWN && wParam == keyMenu && !Shadow::g_Ctx.AssigningHotkey) {
            bShowMenu = !bShowMenu;
        }

        if (bShowMenu) {
            if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN ||
                uMsg == WM_RBUTTONUP || uMsg == WM_MOUSEWHEEL || uMsg == WM_LBUTTONDBLCLK || uMsg == WM_RBUTTONDBLCLK) {
                return 1;
            }

            if (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP || uMsg == WM_CHAR) {
                if (wParam == keyMenu) {
                    return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
                }

                if (!Shadow::IsKeyAllowed(static_cast<int>(wParam))) {
                    return 1;
                }
            }
        }

        return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
    }

    typedef void(__fastcall* tPostRender)(SDK::UGameViewportClient* _this, SDK::UCanvas* Canvas);
    tPostRender oPostRender = nullptr;

    void __fastcall hkPostRender(SDK::UGameViewportClient* rcx, SDK::UCanvas* canvas, void* r8, void* r9) {
        if (!canvas) return oPostRender(rcx, canvas);

        // Shadow Gui内部会自动获取引擎默认字体
        /*
        if (!Shadow::DefaultFont) {
            static SDK::UFont* OpenSansRegular12 = nullptr;

            if (!OpenSansRegular12) {
                SDK::UObject* _Font = SDK::UObject::FindObject("Font OpenSansRegular12.OpenSansRegular12");
                if (_Font && _Font->IsA(SDK::UFont::StaticClass())) OpenSansRegular12 = (SDK::UFont*)_Font; Shadow::DefaultFont = OpenSansRegular12;
            }
        }
        */

        Shadow::SetAllowedKeys({ 'W', 'A', 'S', 'D', VK_SPACE }); // 放行常用移动按键

        static bool bWasMenuOpen = false;
        if (bShowMenu != bWasMenuOpen) {
            SetMouseCursorVisible(bShowMenu);
            bWasMenuOpen = bShowMenu;
        }
        else if (bShowMenu) {
            SetMouseCursorVisible(true);
        }

        Shadow::NewFrame(canvas);

        if (bShowMenu) {
            if (Shadow::Begin(U8("测试菜单##main_window"))) {
                if (Shadow::BeginTabBar("MainTabs##tabs")) {

                    if (Shadow::BeginTabItem(U8("设置##tab0"))) {
                        Shadow::HotKey(U8("菜单按键##menu_key"), &keyMenu);
                        Shadow::EndTabItem();
                    }

                    if (Shadow::BeginTabItem("Combat##tab1")) {
                        Shadow::CheckBox("God Mode##checkbox_1", &bGodMode);
                        Shadow::Slider("Run Speed##slider_1", &fSpeed, 1.0f, 10.0f, 0.1f);
                        Shadow::HotKey("Aimbot Key##hotkey_1", &keyAimbot, &bAimbotActive, &modeAimbot);

                        if (Shadow::Button("Reset Speed##btn_1")) {
                            fSpeed = 1.0f;
                        }

                        Shadow::EndTabItem();
                    }

                    if (Shadow::BeginTabItem("Visuals##tab2")) {
                        Shadow::ColorPicker("ESP Color##cp_1", &cESP.r, &cESP.g, &cESP.b, &cESP.a);
                        Shadow::EndTabItem();
                    }

                    Shadow::EndTabBar();
                }
            }
            Shadow::End();
        }

        return oPostRender(rcx, canvas);
    }

    void FindPostRender() {
        std::string pattern = "8B C2 35 ?? ?? ?? ?? 44";
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
            Sleep(100);
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