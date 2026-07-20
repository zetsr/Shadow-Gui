#define U8(str) reinterpret_cast<const char*>(u8##str)
#include <windows.h>
#include <iostream>
#include "external/AOBScan/AOBScan.hpp"
#include "external/MinHook/include/MinHook.h"
#include "external/CppSDK/SDK.hpp"
#include "external/Shadow-Gui/include/Shadow.h"

namespace Hook {
    bool bShowMenu = false;
    int keyMenu = VK_F1;

    float currentAlpha = 0.0f;
    const float fadeSpeed = 5.0f;

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

    void HelpMarker(std::string_view desc) {
        // 1. 同行紧跟上一个控件
        Shadow::SameLine();

        // 2. 记录当前 (?) 准备绘制的绝对坐标
        Shadow::Vec2 pos = Shadow::g_Ctx.Cursor;
        Shadow::Vec2 textSize = Shadow::MeasureTextSize("(?)");
        Shadow::Vec2 itemSize = { textSize.x, Shadow::g_Ctx.ItemHeight };

        // 3. 绘制带有禁用文本颜色的 (?) 标识
        Shadow::TextColored(Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_TextDisabled], "(?)");

        // 4. 判断鼠标是否悬停在刚才绘制的 (?) 上
        if (Shadow::IsMouseHovering(pos, itemSize)) {
            // 计算提示文本的尺寸与背景框尺寸
            Shadow::Vec2 descSize = Shadow::MeasureTextSize(desc);
            Shadow::Vec2 boxSize = {
                descSize.x + Shadow::g_Ctx.Style.FramePadding.x * 2.f,
                descSize.y + Shadow::g_Ctx.Style.FramePadding.y * 2.f
            };

            // 提示框生成的绝对坐标（在鼠标右下方偏移一些，防止遮挡指针）
            Shadow::Vec2 tooltipPos = { Shadow::g_Ctx.MousePos.x + 15.f, Shadow::g_Ctx.MousePos.y + 15.f };

            // 5. 渲染 Tooltip 样式（使用暗色弹窗背景与高亮文本）
            // 关键：为了防止提示框被当前窗口的裁剪区（ClipRect）切掉，可以在绘制前临时清除裁剪，绘制后恢复
            bool oldClipping = Shadow::g_Ctx.ClippingEnabled;
            Shadow::g_Ctx.ClippingEnabled = false;

            // 绘制背景与边框
            Shadow::DrawRectFilled(tooltipPos, boxSize, Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_PopupBg]);
            Shadow::DrawRect(tooltipPos, boxSize, Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_PopupBorder]);

            // 绘制提示文本
            Shadow::Vec2 textPos = { tooltipPos.x + Shadow::g_Ctx.Style.FramePadding.x, tooltipPos.y + Shadow::g_Ctx.Style.FramePadding.y };
            Shadow::DrawTextString(desc, textPos, Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_TextHighlight]);

            Shadow::g_Ctx.ClippingEnabled = oldClipping;
        }
    }

    typedef void(__fastcall* tPostRender)(SDK::UGameViewportClient* _this, SDK::UCanvas* Canvas);
    tPostRender oPostRender = nullptr;

    void __fastcall hkPostRender(SDK::UGameViewportClient* rcx, SDK::UCanvas* canvas) {
        oPostRender(rcx, canvas);

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

        static SDK::UFont* OpenSansRegular12 = nullptr;
        static SDK::UFont* SansationBold18 = nullptr;
        static SDK::UFont* NotoSansSC = nullptr;

        /*
        // Shadow Gui内部会自动获取引擎默认字体
        if (!Shadow::DefaultFont) {
            if (!OpenSansRegular12) {
                SDK::UObject* _Font = SDK::UObject::FindObject("Font OpenSansRegular12.OpenSansRegular12");
                if (_Font && _Font->IsA(SDK::UFont::StaticClass())) OpenSansRegular12 = (SDK::UFont*)_Font; // Shadow::DefaultFont = OpenSansRegular12;
            }

            if (!SansationBold18) {
                SDK::UObject* _Font = SDK::UObject::FindObject("Font SansationBold18.SansationBold18");
                if (_Font && _Font->IsA(SDK::UFont::StaticClass())) SansationBold18 = (SDK::UFont*)_Font; // Shadow::DefaultFont = SansationBold18;
            }

            if (!NotoSansSC) {
                SDK::UObject* _Font = SDK::UObject::FindObject("Font NotoSansSC-Regular_Font.NotoSansSC-Regular_Font");
                if (_Font && _Font->IsA(SDK::UFont::StaticClass())) NotoSansSC = (SDK::UFont*)_Font; // Shadow::DefaultFont = NotoSansSC;
            }

            // if (SDK::UEngine::GetEngine()) { OpenSansRegular12 = SDK::UEngine::GetEngine()->MediumFont; }
        }
        */

        Shadow::SetAllowedKeys({ 'W', 'A', 'S', 'D', VK_SPACE }); // 放行常用移动按键

        // 1. 使用 C++20/23 chrono 库计算每帧的 Delta Time
        static auto lastTime = std::chrono::steady_clock::now();
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        float dt = deltaTime.count();

        // 2. 根据 bShowMenu 目标状态平滑计算 currentAlpha
        if (bShowMenu) {
            currentAlpha += fadeSpeed * dt;
            if (currentAlpha > 1.0f) currentAlpha = 1.0f;
        }
        else {
            currentAlpha -= fadeSpeed * dt;
            if (currentAlpha < 0.0f) currentAlpha = 0.0f;
        }

        // 3. 鼠标显示逻辑切换
        static bool bWasAnimating = false;
        static bool gameOriginalCursorState = false; // 用于备份游戏原本的光标状态

        bool isAnimating = (bShowMenu || currentAlpha > 0.001f);

        if (isAnimating) {
            // 【刚刚打开菜单的第一帧】备份游戏当下的鼠标状态
            if (!bWasAnimating) {
                gameOriginalCursorState = GetMouseCursorVisible();
            }

            // 只要外挂菜单在显示或淡出，强制显示鼠标
            SetMouseCursorVisible(true);
            bWasAnimating = true;
        }
        else if (bWasAnimating) {
            // 【菜单彻底关闭的第一帧】将游戏原本的状态还原回去
            SetMouseCursorVisible(gameOriginalCursorState);
            bWasAnimating = false;
        }

        Shadow::NewFrame(canvas);
        Shadow::UpdateAllHotkeyStates();

        Shadow::StyleColorsAmethyst();
        // Shadow::StyleColorsOcean();
        // Shadow::GetStyle().WindowMinSize = { 200, 150 };

        Shadow::SetNextWindowSizeConstraints({ static_cast<float>(canvas->SizeX * 0.3), static_cast<float>(canvas->SizeY * 0.4) }, { static_cast<float>(canvas->SizeX), static_cast<float>(canvas->SizeY) });

        for (auto& color : Shadow::GetStyle().Colors) {
            color.a *= currentAlpha; // 仅修改或叠加当前的 Alpha 动画系数
        }

        if (bAimbotActive) {
            canvas->K2_DrawBox({ 10, 10 }, { 20, 20 }, 1.0f, { 0.0f, 1.0f, 0.0f, 1.0f });
        }

        if (currentAlpha > 0.001f) {
            static bool bWindowNoResize = true;
            static bool bWindowNoMove = false;
            static bool bWindowNoScrollbar = true;

            static bool bTabReorderable = true;
            static bool bTabFittingScroll = true;
            static bool bTabNoScrollbar = true;

            static int selectedTextAlign = 1;
            static const std::vector<std::string> alignOptions = { U8("左对齐"), U8("居中对齐"), U8("右对齐") };

            static int selecteddpiO = 1;
            static const std::vector<std::string> dpiO = { U8("75%"), U8("100%"), U8("125%"), U8("150%") };

            switch (selecteddpiO) {
            case 0: Shadow::GetStyle().FontScaleDpi = 0.75f; break;
            case 1: Shadow::GetStyle().FontScaleDpi = 1.0f; break;
            case 2: Shadow::GetStyle().FontScaleDpi = 1.25f; break;
            case 3: Shadow::GetStyle().FontScaleDpi = 1.5f; break;
            }

            int currentWindowFlags = 0;
            if (bWindowNoResize)    currentWindowFlags |= Shadow::ShadowWindowFlags_NoResize;
            if (bWindowNoMove)      currentWindowFlags |= Shadow::ShadowWindowFlags_NoMove;
            if (bWindowNoScrollbar) currentWindowFlags |= Shadow::ShadowWindowFlags_NoScrollbar;

            if (selectedTextAlign == 0)      currentWindowFlags |= Shadow::ShadowWindowFlags_TextAlignLeft;
            else if (selectedTextAlign == 1) currentWindowFlags |= Shadow::ShadowWindowFlags_TextAlignCenter;
            else if (selectedTextAlign == 2) currentWindowFlags |= Shadow::ShadowWindowFlags_TextAlignRight;

            int currentTabBarFlags = 0;
            if (bTabReorderable)   currentTabBarFlags |= Shadow::ShadowTabBarFlags_Reorderable;
            if (bTabFittingScroll) currentTabBarFlags |= Shadow::ShadowTabBarFlags_FittingPolicyScroll;
            if (bTabNoScrollbar)   currentTabBarFlags |= Shadow::ShadowTabBarFlags_NoScrollbar;

            if (Shadow::Begin(U8("测试菜单##main_window"), currentWindowFlags)) {

                if (Shadow::BeginTabBar("MainTabs##tabs", currentTabBarFlags)) {
                    if (Shadow::BeginTabItem(U8("设置##tab0"))) {
                        // Shadow::PushFont(SansationBold18, Shadow::GetStyle().FontScaleDpi);
                        Shadow::HotKey(U8("菜单按键##menu_key"), &keyMenu, Shadow::ShadowHotkeyFlags_NoRightAlign);

						static bool bhello = false;
                        static bool bnohello = false;

						Shadow::Switch(U8("你好！##bhello_1"), &bhello);
                        Shadow::SameLine();

                        Shadow::Switch(U8("你好1！##bhello_1"), &bhello);
                        Shadow::SameLine();

                        Shadow::Switch(U8("你好2！##bhello_1"), &bhello);
                        Shadow::SameLine();

                        Shadow::Switch(U8("你好3！##bhello_1"), &bhello);

                        Shadow::BeginDisabled(bhello);
                        Shadow::Switch(U8("好的！##bnohello_1"), &bnohello);
                        Shadow::EndDisabled();

                        Shadow::Text(U8("这是一段普通文本"));
                        Shadow::TextDisabled(U8("这是一段禁用颜色的文本"));

                        // Shadow::PopFont();
                    }
                    Shadow::EndTabItem();

                    if (Shadow::BeginTabItem(U8("战斗##tab1"))) {
                        Shadow::Checkbox(U8("无敌模式##checkbox_1"), &bGodMode);
                        Shadow::SameLine();
                        Shadow::Slider(U8("移动速度##slider_1"), &fSpeed, 1.0f, 10.0f, 0.1f, Shadow::ShadowSliderFlags_NoRightAlign);
                        Shadow::Slider(U8("移动速度##slider_2"), &fSpeed, 1.0f, 10.0f, 0.1f, Shadow::ShadowSliderFlags_NoRightAlign);
                        Shadow::HotKey(U8("自瞄按键##hotkey_1"), &keyAimbot, &bAimbotActive, &modeAimbot, Shadow::ShadowHotkeyFlags_NoRightAlign | Shadow::ShadowHotkeyFlags_NoStateDisplay);
                        HelpMarker("开启后按下热键会自动锁定敌方目标。"); // 紧跟在控件后面

                        if (Shadow::Button(U8("重置速度##btn_1"))) {
                            fSpeed = 1.0f;
                        }
                        Shadow::SameLine();
                        Shadow::Button(U8("重置速度2##btn_2"));
                        Shadow::SameLine();
                        Shadow::Button(U8("重置速度3##btn_3"));
                        Shadow::Spacing();
                        Shadow::Spacing();
                        Shadow::Spacing();
                        Shadow::Spacing();
                        Shadow::Button(U8("重置速度4##btn_4"));

                        static bool testswitchcb = false;
                        Shadow::Checkbox(U8("测试开关##testswitchcb"), &testswitchcb);
                        if (testswitchcb) {
                            Shadow::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, U8("测试开关已开启"));
                        }

                        static bool btestdisabled = false;
                        Shadow::Checkbox(U8("测试禁用开关##test_disabled_cb1"), &btestdisabled);
                        Shadow::SameLine();
                        Shadow::TextColored({ 0.0f, 1.0f, 0.0f, 1.0f }, U8("第一个测试文本"));

                        Shadow::BeginDisabled(btestdisabled);
                        Shadow::TextColored({ 0.0f, 1.0f, 0.0f, 1.0f }, U8("第二个测试文本"));
                        Shadow::Checkbox(U8("测试开关##test_cb_1"), &bWindowNoResize);
                        Shadow::EndDisabled();

                        Shadow::Checkbox(U8("禁用窗口缩放##flag_w_no_resize"), &bWindowNoResize);
                        Shadow::Checkbox(U8("禁用窗口移动##flag_w_no_move"), &bWindowNoMove);
                        Shadow::Checkbox(U8("禁用窗口滚动条##flag_w_no_scroll"), &bWindowNoScrollbar);

                        Shadow::Combo(U8("标题对齐方式##flag_w_align_combo"), &selectedTextAlign, alignOptions, Shadow::ShadowComboFlags_NoRightAlign | Shadow::ShadowComboFlags_FitText);
                        Shadow::Combo(U8("缩放DPI##dpiscale_combo"), &selecteddpiO, dpiO);

                        Shadow::Checkbox(U8("启用标签拖拽##flag_t_reorder"), &bTabReorderable);
                        Shadow::Checkbox(U8("标签溢出滚动##flag_t_fit_scroll"), &bTabFittingScroll);
                        Shadow::Checkbox(U8("禁用标签滚动条##flag_t_no_scroll"), &bTabNoScrollbar);
                    }
                    Shadow::EndTabItem();

                    // 修改 Visuals 标签为中文，并添加新的内容
                    if (Shadow::BeginTabItem(U8("视觉##tab2"))) {

                        // 添加目标选择下拉框
                        Shadow::Combo(U8("目标选择##target_combo"), &selectedTarget, targetOptions);

                        // 根据选择的目标显示对应的选项
                        if (selectedTarget == 0) {
                            Shadow::Checkbox(U8("敌人 - 血量##health_enemy"), &showHealth[0]);
                            Shadow::SameLine();
                            Shadow::ColorPicker(U8("ESP颜色##cp_1"), &cESP.r, &cESP.g, &cESP.b, &cESP.a, Shadow::ShadowColorPickerFlags_NoText);
                            Shadow::Checkbox(U8("敌人 - 方框##box_enemy"), &showBox[0]);
                            Shadow::SameLine();
                            Shadow::ColorPicker(U8("ESP颜色##cp_2"), &cESP.r, &cESP.g, &cESP.b, &cESP.a, Shadow::ShadowColorPickerFlags_NoText | Shadow::ShadowColorPickerFlags_NoRightAlign);
                            Shadow::Checkbox(U8("敌人 - 按钮##button_enemy"), &showButton[0]);
                            Shadow::SameLine();
                            Shadow::ColorPicker(U8("ESP颜色##cp_3"), &cESP.r, &cESP.g, &cESP.b, &cESP.a);
                        }
                        else if (selectedTarget == 1) {
                            Shadow::Checkbox(U8("队友 - 血量##health_teammate"), &showHealth[1]);
                            Shadow::Checkbox(U8("队友 - 方框##box_teammate"), &showBox[1]);
                            Shadow::Checkbox(U8("队友 - 按钮##button_teammate"), &showButton[1]);
                        }
                        else if (selectedTarget == 2) {
                            Shadow::Checkbox(U8("AI - 血量##health_ai"), &showHealth[2]);
                            Shadow::Checkbox(U8("AI - 方框##box_ai"), &showBox[2]);
                            Shadow::Checkbox(U8("AI - 按钮##button_ai"), &showButton[2]);
                        }
                    }
                    Shadow::EndTabItem();

                    if (Shadow::BeginTabItem(U8("测试1##tab1"))) {

                    }
                    Shadow::EndTabItem();

                    if (Shadow::BeginTabItem(U8("测试2##tab2"))) {

                    }
                    Shadow::EndTabItem();

                    if (Shadow::BeginTabItem(U8("测试3##tab3"))) {

                    }
                    Shadow::EndTabItem();

                    if (Shadow::BeginTabItem(U8("测试4##tab4"))) {

                    }
                    Shadow::EndTabItem();
                }
                Shadow::EndTabBar();
            }
            Shadow::End();
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