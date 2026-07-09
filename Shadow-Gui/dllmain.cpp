#define U8(str) reinterpret_cast<const char*>(u8##str)
#include <windows.h>
#include <iostream>
#include "external/AOBScan/AOBScan.hpp"
#include "external/MinHook/include/MinHook.h"
#include "external/CppSDK/SDK.hpp"
#include "external/Shadow-Gui/include/Shadow.h"

namespace Hook {
    bool bShowMenu = true;
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

    void __fastcall hkPostRender(SDK::UGameViewportClient* rcx, SDK::UCanvas* canvas) {
        if (!canvas) return oPostRender(rcx, canvas);

        static bool bGodMode = false;
        static  float fSpeed = 1.0f;
        static  Shadow::Color cESP = { 1.0f, 0.0f, 0.0f, 1.0f };

        static  int selectedTarget = 0; // 0:敌人, 1:队友, 2:AI
        static  bool showHealth[3] = { true, true, true };  // 血量显示开关
        static  bool showBox[3] = { false, false, false };   // 方框显示开关
        static   bool showButton[3] = { false, false, false }; // 按钮显示开关

        // 下拉框选项列表
        static    std::vector<std::string> targetOptions = { U8("敌人"), U8("队友"), U8("AI") };

        static    int keyAimbot = VK_RBUTTON;
        static    bool bAimbotActive = false;
        static   Shadow::HotkeyMode modeAimbot = Shadow::HotkeyMode::HoldOn;

        // Shadow Gui内部会自动获取引擎默认字体
        if (!Shadow::DefaultFont) {
            static SDK::UFont* OpenSansRegular12 = nullptr;

            if (!OpenSansRegular12) {
                SDK::UObject* _Font = SDK::UObject::FindObject("Font OpenSansRegular12.OpenSansRegular12");
                if (_Font && _Font->IsA(SDK::UFont::StaticClass())) OpenSansRegular12 = (SDK::UFont*)_Font; Shadow::DefaultFont = OpenSansRegular12;
            }
        }

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
                    }
                    Shadow::EndTabItem();

                    // 修改 Combat 标签为中文
                    if (Shadow::BeginTabItem(U8("战斗##tab1"))) {
                        Shadow::CheckBox(U8("无敌模式##checkbox_1"), &bGodMode);
                        Shadow::Slider(U8("移动速度##slider_1"), &fSpeed, 1.0f, 10.0f, 0.1f);
                        Shadow::HotKey(U8("自瞄按键##hotkey_1"), &keyAimbot, &bAimbotActive, &modeAimbot);

                        if (Shadow::Button(U8("重置速度##btn_1"))) {
                            fSpeed = 1.0f;
                        }
                    }
                    Shadow::EndTabItem();

                    // 修改 Visuals 标签为中文，并添加新的内容
                    if (Shadow::BeginTabItem(U8("视觉##tab2"))) {
                        Shadow::ColorPicker(U8("ESP颜色##cp_1"), &cESP.r, &cESP.g, &cESP.b, &cESP.a);

                        // 添加目标选择下拉框
                        Shadow::ComboBox(U8("目标选择##target_combo"), &selectedTarget, targetOptions);

                        // 根据选择的目标显示对应的选项
                        if (selectedTarget == 0) {
                            Shadow::CheckBox(U8("敌人 - 血量##health_enemy"), &showHealth[0]);
                            Shadow::CheckBox(U8("敌人 - 方框##box_enemy"), &showBox[0]);
                            Shadow::CheckBox(U8("敌人 - 按钮##button_enemy"), &showButton[0]);
                        }
                        else if (selectedTarget == 1) {
                            Shadow::CheckBox(U8("队友 - 血量##health_teammate"), &showHealth[1]);
                            Shadow::CheckBox(U8("队友 - 方框##box_teammate"), &showBox[1]);
                            Shadow::CheckBox(U8("队友 - 按钮##button_teammate"), &showButton[1]);
                        }
                        else if (selectedTarget == 2) {
                            Shadow::CheckBox(U8("AI - 血量##health_ai"), &showHealth[2]);
                            Shadow::CheckBox(U8("AI - 方框##box_ai"), &showBox[2]);
                            Shadow::CheckBox(U8("AI - 按钮##button_ai"), &showButton[2]);
                        }
                    }
                    Shadow::EndTabItem();
                }
                Shadow::EndTabBar();
            }
            Shadow::End();
        }

        return oPostRender(rcx, canvas);
    }

    void FindPostRender() {
        std::string pattern = "8B C2 35 ?? ?? ?? ?? 44"; 

        // ASA 8B C2 35 ?? ?? ?? ?? 44
        // DRACONIA 48 8B 01 48 FF A0 ?? ?? ?? ?? CC CC CC CC CC CC 40 53 48 83 EC ?? 48 89
        // DOD 48 8B 01 48 FF A0 ?? ?? ?? ?? CC CC CC CC CC CC 48 89 5C 24 10 48 89 74 24 18 48 89 7C 24 20 55

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