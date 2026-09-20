#pragma once
#include <string>
#include <vector>
#include <format>
#include <algorithm>

// Include original SDK path
#include "../external/CppSDK/SDK.hpp"
#include "../misc/textures/Shadow_Titlebar.h"

namespace Shadow {

    // ============================================================================
    // Basic custom composite: HelpMarker
    // ============================================================================
    inline void HelpMarker(std::string_view desc, float TextWrap = 200.f) {
        Shadow::SameLine();
        Shadow::TextColored(Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_TextDisabled], "(?)");
        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_DelayNone)) {
            Shadow::BeginTooltip();
            Shadow::PushTextWrapPos(TextWrap);
            Shadow::TextWrapped({ 1.0f, 1.0f, 1.0f, 1.0f }, desc.data());
            Shadow::PopTextWrapPos();
            Shadow::EndTooltip();
        }
    }

    // ============================================================================
    // Comprehensive Demo
    // ============================================================================
    inline void ShowDemoWindow() {
        static bool show_demo = true;
        if (!show_demo) return;

        static Shadow::ShadowWindowFlags win_flags = Shadow::ShadowWindowFlags_None;
        static Shadow::ShadowTabBarFlags tab_flags = Shadow::ShadowTabBarFlags_Reorderable | Shadow::ShadowTabBarFlags_FittingPolicyScroll;

        if (Shadow::Begin("Shadow Gui Demo##DemoWindow", win_flags)) {
            if (Shadow::BeginTabBar("DemoTabBar", tab_flags)) {

                // ----------------------------------------------------------------
                // Page 1: All standard controls and full flag tests (with combined flag demos)
                // ----------------------------------------------------------------
                if (Shadow::BeginTabItem("Standard Controls")) {

                    if (Shadow::TreeNode("Text Elements")) {
                        Shadow::Text("Standard Text");
                        Shadow::HelpMarker("Basic text output using Shadow::Text.");

                        Shadow::PushTextOutline();
                        Shadow::Text("Outlined Text");
                        Shadow::PopTextOutline();
                        Shadow::HelpMarker("using Shadow::PushTextOutline.");

                        Shadow::TextDisabled("Disabled Text");
                        Shadow::HelpMarker("Text with disabled color using Shadow::TextDisabled.");

                        Shadow::TextColored({ 1.f, 0.5f, 0.5f, 1.f }, "Colored Text");
                        Shadow::HelpMarker("Text with custom RGB color using Shadow::TextColored.");

                        Shadow::TextWrapped({ 0.5f, 1.f, 0.5f, 1.f }, "This is a long wrapped text that will automatically break into multiple lines depending on the window width...");
                        Shadow::HelpMarker("Wrapped text adjusts to window/clip width automatically using Shadow::TextWrapped.");
                    }
                    Shadow::TreePop(); // Unconditional Pop

                    if (Shadow::TreeNode("Basic Widgets")) {
                        static bool bVal1 = false, bVal2 = true;
                        Shadow::Checkbox("Checkbox 1", &bVal1);
                        Shadow::HelpMarker("Standard Checkbox widget.");

                        Shadow::Switch("Switch 1", &bVal2);
                        Shadow::HelpMarker("Standard Switch toggle widget.");

                        if (Shadow::Button("Click Me")) {}
                        Shadow::HelpMarker("Standard Button widget.");
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("Selectables & Combos")) {
                        static bool sel1 = false, sel2 = true;
                        Shadow::Selectable("Selectable 1", &sel1);
                        Shadow::HelpMarker("Selectable taking a boolean pointer (toggle mode).");

                        Shadow::Selectable("Selectable 2 (No Pointer)", sel2);
                        Shadow::HelpMarker("Selectable taking a simple boolean (trigger mode).");

                        static int combo_idx = 0;
                        std::vector<std::string> combo_items = { "Item A", "Item B", "Item C" };

                        Shadow::Combo("Standard Combo", &combo_idx, combo_items);
                        Shadow::HelpMarker("Standard combo box (ShadowComboFlags_None).");

                        Shadow::Combo("Combo NoText", &combo_idx, combo_items, Shadow::ShadowComboFlags_NoText);
                        Shadow::HelpMarker("Combo without left label (ShadowComboFlags_NoText).");

                        Shadow::Combo("Combo NoRightAlign", &combo_idx, combo_items, Shadow::ShadowComboFlags_NoRightAlign);
                        Shadow::HelpMarker("Combo tightly packed next to text (ShadowComboFlags_NoRightAlign).");

                        Shadow::Combo("Combo FitText", &combo_idx, combo_items, Shadow::ShadowComboFlags_FitText);
                        Shadow::HelpMarker("Combo width adapts to selected text length (ShadowComboFlags_FitText).");

                        // Combo Flags combined demos
                        Shadow::Combo("Combo NoText+NoRightAlign", &combo_idx, combo_items, Shadow::ShadowComboFlags_NoText | Shadow::ShadowComboFlags_NoRightAlign);
                        Shadow::HelpMarker("Combines NoText and NoRightAlign.");

                        Shadow::Combo("Combo FitText+NoRightAlign", &combo_idx, combo_items, Shadow::ShadowComboFlags_FitText | Shadow::ShadowComboFlags_NoRightAlign);
                        Shadow::HelpMarker("Combines FitText and NoRightAlign.");

                        Shadow::Combo("Combo NoText+FitText", &combo_idx, combo_items, Shadow::ShadowComboFlags_NoText | Shadow::ShadowComboFlags_FitText);
                        Shadow::HelpMarker("Combines NoText and FitText.");

                        Shadow::Combo("Combo NoText+NoRightAlign+FitText", &combo_idx, combo_items, Shadow::ShadowComboFlags_NoText | Shadow::ShadowComboFlags_NoRightAlign | Shadow::ShadowComboFlags_FitText);
                        Shadow::HelpMarker("Combines NoText, NoRightAlign, and FitText.");
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("Sliders")) {
                        static float s1 = 0.5f, s2 = 1.0f, s3 = 50.f, s4 = 75.f;
                        Shadow::Slider("Standard Slider", &s1, 0.f, 1.f);
                        Shadow::HelpMarker("Basic float slider (ShadowSliderFlags_None).");

                        Shadow::Slider("Slider NoText", &s2, 0.f, 10.f, 0.f, Shadow::ShadowSliderFlags_NoText);
                        Shadow::HelpMarker("Slider without label (ShadowSliderFlags_NoText).");

                        Shadow::Slider("Slider NoRightAlign", &s3, 0.f, 100.f, 1.f, Shadow::ShadowSliderFlags_NoRightAlign);
                        Shadow::HelpMarker("Slider that doesn't stretch to right margin (ShadowSliderFlags_NoRightAlign).");

                        // Slider Flags combined demos
                        Shadow::Slider("Slider NoText+NoRightAlign", &s4, 0.f, 100.f, 0.f, Shadow::ShadowSliderFlags_NoText | Shadow::ShadowSliderFlags_NoRightAlign);
                        Shadow::HelpMarker("Combines NoText and NoRightAlign.");
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("Inputs (Text & Float)")) {
                        static std::string txt1 = "", txt2 = "123", txt3 = "secret";

                        Shadow::InputText("Standard Input", txt1);
                        Shadow::HelpMarker("Standard text input (ShadowInputTextFlags_None).");

                        Shadow::InputTextWithHint("Hint Input", "Enter your name...", txt1);
                        Shadow::HelpMarker("Input with background hint text.");

                        Shadow::InputText("Decimal Only", txt2, Shadow::ShadowInputTextFlags_CharsDecimal);
                        Shadow::HelpMarker("Only numbers allowed (ShadowInputTextFlags_CharsDecimal).");

                        Shadow::InputText("Hexadecimal Only", txt2, Shadow::ShadowInputTextFlags_CharsHexadecimal);
                        Shadow::HelpMarker("Only hex allowed (ShadowInputTextFlags_CharsHexadecimal).");

                        Shadow::InputText("Scientific Only", txt2, Shadow::ShadowInputTextFlags_CharsScientific);
                        Shadow::HelpMarker("Scientific numbers (ShadowInputTextFlags_CharsScientific).");

                        Shadow::InputText("Uppercase Only", txt2, Shadow::ShadowInputTextFlags_CharsUppercase);
                        Shadow::HelpMarker("Auto converts to uppercase (ShadowInputTextFlags_CharsUppercase).");

                        Shadow::InputText("No Blanks", txt2, Shadow::ShadowInputTextFlags_CharsNoBlank);
                        Shadow::HelpMarker("Spaces restricted (ShadowInputTextFlags_CharsNoBlank).");

                        Shadow::InputText("Password", txt3, Shadow::ShadowInputTextFlags_Password);
                        Shadow::HelpMarker("Text obfuscated with * (ShadowInputTextFlags_Password).");

                        Shadow::InputText("Read Only", txt3, Shadow::ShadowInputTextFlags_ReadOnly);
                        Shadow::HelpMarker("Cannot be modified (ShadowInputTextFlags_ReadOnly).");

                        Shadow::InputText("Escape Clears", txt2, Shadow::ShadowInputTextFlags_EscapeClearsAll);
                        Shadow::HelpMarker("Press Esc to clear text (ShadowInputTextFlags_EscapeClearsAll).");

                        Shadow::InputText("Auto Select All", txt2, Shadow::ShadowInputTextFlags_AutoSelectAll);
                        Shadow::HelpMarker("Highlights all text on click (ShadowInputTextFlags_AutoSelectAll).");

                        static std::string txt_noname = "NoName";
                        Shadow::InputText("##NoNameInput", txt_noname, Shadow::ShadowInputTextFlags_NoName);
                        Shadow::HelpMarker("InputText with NoName flag, no label drawn (ShadowInputTextFlags_NoName).");

                        static std::string txt_align = "Center";
                        Shadow::InputText("Align Center", txt_align, Shadow::ShadowInputTextFlags_AlignCenter);
                        Shadow::HelpMarker("InputText with AlignCenter flag (ShadowInputTextFlags_AlignCenter).");

                        // InputText Flags combined demos
                        static std::string txt_combo1 = "my_secret";
                        Shadow::InputText("Pwd+AutoSel+EscClr", txt_combo1, Shadow::ShadowInputTextFlags_Password | Shadow::ShadowInputTextFlags_AutoSelectAll | Shadow::ShadowInputTextFlags_EscapeClearsAll);
                        Shadow::HelpMarker("Combines Password, AutoSelectAll, and EscapeClearsAll.");

                        static std::string txt_combo2 = "12345";
                        Shadow::InputText("Dec+NoBlank+AutoSel", txt_combo2, Shadow::ShadowInputTextFlags_CharsDecimal | Shadow::ShadowInputTextFlags_CharsNoBlank | Shadow::ShadowInputTextFlags_AutoSelectAll);
                        Shadow::HelpMarker("Combines CharsDecimal, CharsNoBlank, and AutoSelectAll.");

                        static std::string txt_combo3 = "readonly";
                        Shadow::InputText("ReadOnly+AutoSel", txt_combo3, Shadow::ShadowInputTextFlags_ReadOnly | Shadow::ShadowInputTextFlags_AutoSelectAll);
                        Shadow::HelpMarker("Combines ReadOnly and AutoSelectAll.");

                        static std::string txt_combo4 = "UPPER_NO_SPACE";
                        Shadow::InputText("Upper+NoBlank", txt_combo4, Shadow::ShadowInputTextFlags_CharsUppercase | Shadow::ShadowInputTextFlags_CharsNoBlank);
                        Shadow::HelpMarker("Combines CharsUppercase and CharsNoBlank.");

                        static float f1 = 3.14f;
                        Shadow::InputFloat("Float Input", &f1);
                        Shadow::HelpMarker("Standard float input.");

                        Shadow::InputFloat("Float EmptyRef", &f1, 0.1f, 1.0f, "{:.2f}", Shadow::ShadowInputTextFlags_ParseEmptyRefVal | Shadow::ShadowInputTextFlags_DisplayEmptyRefVal);
                        Shadow::HelpMarker("Handles empty value parsing and display.");
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("Color Pickers")) {
                        static float r = 1.f, g = 0.f, b = 0.f, a = 1.f;
                        Shadow::ColorPicker("Standard Picker", &r, &g, &b, &a);
                        Shadow::HelpMarker("Standard RGBA Picker (ShadowColorPickerFlags_None).");

                        Shadow::ColorPicker("Picker NoText", &r, &g, &b, &a, Shadow::ShadowColorPickerFlags_NoText);
                        Shadow::HelpMarker("Picker without left label (ShadowColorPickerFlags_NoText).");

                        Shadow::ColorPicker("Picker NoRightAlign", &r, &g, &b, &a, Shadow::ShadowColorPickerFlags_NoRightAlign);
                        Shadow::HelpMarker("Picker packed tightly (ShadowColorPickerFlags_NoRightAlign).");

                        // ColorPicker Flags combined demos
                        Shadow::ColorPicker("Picker NoText+NoRightAlign", &r, &g, &b, &a, Shadow::ShadowColorPickerFlags_NoText | Shadow::ShadowColorPickerFlags_NoRightAlign);
                        Shadow::HelpMarker("Combines NoText and NoRightAlign.");
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("Hotkeys")) {
                        static int hk1 = 0;
                        static int hk2 = 0x41; // 'A'
                        static bool hk2_active = false;
                        static Shadow::HotkeyMode hk2_mode = Shadow::HotkeyMode::ToggleOn;

                        Shadow::HotKey("Simple Hotkey", &hk1);
                        Shadow::HelpMarker("Registers simple hotkey taking int* only.");

                        Shadow::HotKey("Advanced Hotkey", &hk2, &hk2_active, &hk2_mode);
                        Shadow::HelpMarker("Hotkey with mode selection. Right-click to configure mode.");

                        Shadow::HotKey("Hotkey NoText", &hk2, &hk2_active, &hk2_mode, Shadow::ShadowHotkeyFlags_NoText);
                        Shadow::HelpMarker("Hotkey without label (ShadowHotkeyFlags_NoText).");

                        Shadow::HotKey("Hotkey NoRightAlign", &hk2, &hk2_active, &hk2_mode, Shadow::ShadowHotkeyFlags_NoRightAlign);
                        Shadow::HelpMarker("Hotkey tightly packed (ShadowHotkeyFlags_NoRightAlign).");

                        Shadow::HotKey("Hotkey NoStateDisplay", &hk2, &hk2_active, &hk2_mode, Shadow::ShadowHotkeyFlags_NoStateDisplay);
                        Shadow::HelpMarker("Hotkey without status indicator dot (ShadowHotkeyFlags_NoStateDisplay).");

                        // Hotkeys Flags combined demos
                        Shadow::HotKey("HK NoText+NoRightAlign", &hk2, &hk2_active, &hk2_mode, Shadow::ShadowHotkeyFlags_NoText | Shadow::ShadowHotkeyFlags_NoRightAlign);
                        Shadow::HelpMarker("Combines NoText and NoRightAlign.");

                        Shadow::HotKey("HK NoText+NoStateDisp", &hk2, &hk2_active, &hk2_mode, Shadow::ShadowHotkeyFlags_NoText | Shadow::ShadowHotkeyFlags_NoStateDisplay);
                        Shadow::HelpMarker("Combines NoText and NoStateDisplay.");

                        Shadow::HotKey("HK NoRightAlign+NoStateDisp", &hk2, &hk2_active, &hk2_mode, Shadow::ShadowHotkeyFlags_NoRightAlign | Shadow::ShadowHotkeyFlags_NoStateDisplay);
                        Shadow::HelpMarker("Combines NoRightAlign and NoStateDisplay.");

                        Shadow::HotKey("HK All Flags Combo", &hk2, &hk2_active, &hk2_mode, Shadow::ShadowHotkeyFlags_NoText | Shadow::ShadowHotkeyFlags_NoRightAlign | Shadow::ShadowHotkeyFlags_NoStateDisplay);
                        Shadow::HelpMarker("Combines NoText, NoRightAlign, and NoStateDisplay.");
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("Layout, Spacing & Stack Modifiers")) {
                        Shadow::Text("Item 1");
                        Shadow::SameLine();
                        Shadow::Text("Item 2 (Same Line)");
                        Shadow::HelpMarker("Uses SameLine() without args to pack horizontally.");

                        Shadow::Text("Item 3");
                        Shadow::SameLine(150.f);
                        Shadow::Text("Item 4 (SameLine offset)");
                        Shadow::HelpMarker("Uses SameLine(150.f) to force alignment to 150px.");

                        Shadow::Text("Item 5");
                        Shadow::SameLine(0.f, 40.f);
                        Shadow::Text("Item 6 (SameLine space)");
                        Shadow::HelpMarker("Uses SameLine(0.f, 40.f) to force 40px spacing.");

                        Shadow::Separator();
                        Shadow::HelpMarker("Separator() line above.");

                        Shadow::Text("Below is a 50x30 Dummy");
                        Shadow::Dummy({ 50.f, 30.f });
                        Shadow::Text("After Dummy");
                        Shadow::HelpMarker("Dummy inserts empty space manually.");

                        Shadow::Spacing();
                        Shadow::HelpMarker("Spacing() adds vertical padding between items.");

                        Shadow::PushTextWrapPos(150.f);
                        Shadow::Text("This text is forcibly wrapped at 150 pixels due to PushTextWrapPos.");
                        Shadow::PopTextWrapPos();
                        Shadow::HelpMarker("PushTextWrapPos / PopTextWrapPos Demonstration.");

                        Shadow::Vec2 cur = Shadow::g_Ctx.Cursor;
                        Shadow::PushClipRect(cur, { cur.x + 100.f, cur.y + Shadow::g_Ctx.ItemHeight });
                        Shadow::Text("This long text will be physically clipped at 100px width.");
                        Shadow::PopClipRect();
                        Shadow::HelpMarker("PushClipRect / PopClipRect Demonstration.");

                        Shadow::PushFont(Shadow::g_Ctx.DefaultFont, 1.5f);
                        Shadow::Text("Text with Font scale 1.5x");
                        Shadow::PopFont();
                        Shadow::HelpMarker("PushFont / PopFont Demonstration.");

                        Shadow::PushFont(Shadow::g_Ctx.DefaultFont, 3.f);
                        Shadow::Text("Text with Font scale 3.x");
                        Shadow::PopFont();
                        Shadow::HelpMarker("PushFont / PopFont Demonstration.");
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("Disabled Status")) {
                        static bool disable_all = true;
                        Shadow::Checkbox("Disable the following group", &disable_all);

                        Shadow::BeginDisabled(disable_all);
                        Shadow::Button("Disabled Button");
                        static float dummy_f = 0.5f;
                        Shadow::Slider("Disabled Slider", &dummy_f, 0.f, 1.f);
                        Shadow::EndDisabled();

                        Shadow::HelpMarker("Widgets bounded by BeginDisabled(bool) / EndDisabled().");
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("TreeNodes & Flags")) {
                        if (Shadow::TreeNode("Default Open Node", Shadow::ShadowTreeNodeFlags_DefaultOpen)) {
                            Shadow::Text("This is open by default.");
                        }
                        Shadow::TreePop(); // Unconditional Pop

                        if (Shadow::TreeNode("Framed Node", Shadow::ShadowTreeNodeFlags_Framed)) {
                            Shadow::Text("This node has a background frame.");
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode("FitText Framed Node", Shadow::ShadowTreeNodeFlags_Framed | Shadow::ShadowTreeNodeFlags_FitText)) {
                            Shadow::Text("The frame stops at the text end, ignoring right margin.");
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode("No Indent Node", Shadow::ShadowTreeNodeFlags_NoIndent)) {
                            Shadow::Text("Children inside this node are not indented horizontally.");
                        }
                        Shadow::TreePop();

                        // TreeNode Flags combined demos
                        if (Shadow::TreeNode("All Flags Combo Node", Shadow::ShadowTreeNodeFlags_Framed | Shadow::ShadowTreeNodeFlags_DefaultOpen | Shadow::ShadowTreeNodeFlags_FitText | Shadow::ShadowTreeNodeFlags_NoIndent)) {
                            Shadow::Text("This node combines Framed, DefaultOpen, FitText, and NoIndent.");
                        }
                        Shadow::TreePop();
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("Hovered Flags Demo")) {
                        Shadow::Button("Hover Me (Normal)");
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_None)) {
                            Shadow::BeginTooltip(); Shadow::Text("Normal Hover"); Shadow::EndTooltip();
                        }

                        Shadow::Button("Hover Me (Delay Short)");
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_DelayShort)) {
                            Shadow::BeginTooltip(); Shadow::Text("Short Delay Hover Tooltip"); Shadow::EndTooltip();
                        }

                        Shadow::Button("Hover Me (Delay Normal)");
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_DelayNormal)) {
                            Shadow::BeginTooltip(); Shadow::Text("Normal Delay Hover Tooltip"); Shadow::EndTooltip();
                        }

                        Shadow::Button("Hover Me (Stationary)");
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_Stationary)) {
                            Shadow::BeginTooltip(); Shadow::Text("Stationary Hover Tooltip"); Shadow::EndTooltip();
                        }

                        // Hover Flags combined demos
                        Shadow::Button("Hover Me (DelayNormal + Stationary)");
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_DelayNormal | Shadow::ShadowHoveredFlags_Stationary)) {
                            Shadow::BeginTooltip(); Shadow::Text("DelayNormal + Stationary Hover Tooltip"); Shadow::EndTooltip();
                        }

                        Shadow::BeginDisabled(true);
                        Shadow::Button("Hover Disabled (DelayShort + Allow)");
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_DelayShort | Shadow::ShadowHoveredFlags_AllowWhenDisabled)) {
                            Shadow::BeginTooltip(); Shadow::Text("DelayShort + AllowWhenDisabled Hover Tooltip"); Shadow::EndTooltip();
                        }
                        Shadow::EndDisabled();

                        Shadow::Button("Hover Me (AllowWhenBlockedByPopup)");
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_AllowWhenBlockedByPopup)) {
                            Shadow::BeginTooltip(); Shadow::Text("AllowWhenBlockedByPopup Hover Tooltip"); Shadow::EndTooltip();
                        }

                        Shadow::Button("Hover Me (AllowWhenBlockedByActiveItem)");
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_AllowWhenBlockedByActiveItem)) {
                            Shadow::BeginTooltip(); Shadow::Text("AllowWhenBlockedByActiveItem Hover Tooltip"); Shadow::EndTooltip();
                        }

                        Shadow::Button("Hover Me (DelayNormal + NoSharedDelay)");
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_DelayNormal | Shadow::ShadowHoveredFlags_NoSharedDelay)) {
                            Shadow::BeginTooltip(); Shadow::Text("DelayNormal + NoSharedDelay Hover Tooltip"); Shadow::EndTooltip();
                        }
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("Window Flags Editor")) {
                        static bool f_NoResize = false, f_NoMove = false, f_NoScrollbar = false, f_NoTitleBar = false, f_NoMouseInputs = false;

                        if (Shadow::Selectable("NoResize", &f_NoResize)) { if (f_NoResize) win_flags |= Shadow::ShadowWindowFlags_NoResize; else win_flags &= ~Shadow::ShadowWindowFlags_NoResize; }
                        if (Shadow::Selectable("NoMove", &f_NoMove)) { if (f_NoMove) win_flags |= Shadow::ShadowWindowFlags_NoMove; else win_flags &= ~Shadow::ShadowWindowFlags_NoMove; }
                        if (Shadow::Selectable("NoScrollbar", &f_NoScrollbar)) { if (f_NoScrollbar) win_flags |= Shadow::ShadowWindowFlags_NoScrollbar; else win_flags &= ~Shadow::ShadowWindowFlags_NoScrollbar; }
                        if (Shadow::Selectable("NoTitleBar", &f_NoTitleBar)) { if (f_NoTitleBar) win_flags |= Shadow::ShadowWindowFlags_NoTitleBar; else win_flags &= ~Shadow::ShadowWindowFlags_NoTitleBar; }
                        if (Shadow::Selectable("NoMouseInputs", &f_NoMouseInputs)) { if (f_NoMouseInputs) win_flags |= Shadow::ShadowWindowFlags_NoMouseInputs; else win_flags &= ~Shadow::ShadowWindowFlags_NoMouseInputs; }

                        static int align_idx = 0;
                        std::vector<std::string> aligns = { "Left", "Center", "Right" };

                        Shadow::Combo("TextAlign (Applies to Title)", &align_idx, aligns);
                        win_flags &= ~(Shadow::ShadowWindowFlags_TextAlignCenter | Shadow::ShadowWindowFlags_TextAlignRight);
                        if (align_idx == 1) win_flags |= Shadow::ShadowWindowFlags_TextAlignCenter;
                        if (align_idx == 2) win_flags |= Shadow::ShadowWindowFlags_TextAlignRight;
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("TabBar Flags Editor")) {
                        static bool t_Reorderable = true, t_FittingPolicyScroll = true, t_NoScrollbar = false;

                        if (Shadow::Selectable("Reorderable", &t_Reorderable)) { if (t_Reorderable) tab_flags |= Shadow::ShadowTabBarFlags_Reorderable; else tab_flags &= ~Shadow::ShadowTabBarFlags_Reorderable; }
                        if (Shadow::Selectable("FittingPolicyScroll", &t_FittingPolicyScroll)) { if (t_FittingPolicyScroll) tab_flags |= Shadow::ShadowTabBarFlags_FittingPolicyScroll; else tab_flags &= ~Shadow::ShadowTabBarFlags_FittingPolicyScroll; }
                        if (Shadow::Selectable("NoScrollbar", &t_NoScrollbar)) { if (t_NoScrollbar) tab_flags |= Shadow::ShadowTabBarFlags_NoScrollbar; else tab_flags &= ~Shadow::ShadowTabBarFlags_NoScrollbar; }
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("Style Menu Colors Editor")) {
                        const char* bg_names_en[] = { "WindowBg", "TitleBarBg", "FrameBg", "FrameBgHovered", "PopupBg", "Tab", "TabHovered", "TabActive", "SwitchBg", "SwitchBgHovered", "SwitchBgActive", "SwitchBgActiveHovered", "DropdownActive" };
                        int bg_ids[] = { Shadow::GuiCol_WindowBg, Shadow::GuiCol_TitleBarBg, Shadow::GuiCol_FrameBg, Shadow::GuiCol_FrameBgHovered, Shadow::GuiCol_PopupBg, Shadow::GuiCol_Tab, Shadow::GuiCol_TabHovered, Shadow::GuiCol_TabActive, Shadow::GuiCol_SwitchBg, Shadow::GuiCol_SwitchBgHovered, Shadow::GuiCol_SwitchBgActive, Shadow::GuiCol_SwitchBgActiveHovered, Shadow::GuiCol_DropdownActive };

                        const char* text_names_en[] = { "Text", "TextDisabled", "TextHighlight", "ErrorText", "TextShadow", "TextOutline" };
                        int text_ids[] = { Shadow::GuiCol_Text, Shadow::GuiCol_TextDisabled, Shadow::GuiCol_TextHighlight, Shadow::GuiCol_ErrorText, Shadow::GuiCol_TextShadow, Shadow::GuiCol_TextOutline };

                        const char* btn_names_en[] = { "Button", "ButtonHovered", "SliderGrab", "SliderKnob", "CheckMark", "Separator", "ResizeGrip", "ResizeGripHovered", "ResizeGripActive", "ActiveIndicator", "InactiveIndicator", "Border", "PopupBorder", "ControlDisabled", "SwitchKnob" };
                        int btn_ids[] = { Shadow::GuiCol_Button, Shadow::GuiCol_ButtonHovered, Shadow::GuiCol_SliderGrab, Shadow::GuiCol_SliderKnob, Shadow::GuiCol_CheckMark, Shadow::GuiCol_Separator, Shadow::GuiCol_ResizeGrip, Shadow::GuiCol_ResizeGripHovered, Shadow::GuiCol_ResizeGripActive, Shadow::GuiCol_ActiveIndicator, Shadow::GuiCol_InactiveIndicator, Shadow::GuiCol_Border, Shadow::GuiCol_PopupBorder, Shadow::GuiCol_ControlDisabled, Shadow::GuiCol_SwitchKnob };

                        const char* cp_names_en[] = { "ColorPickerDark", "ColorPickerLight", "CheckerboardLight", "CheckerboardDark", "ColorPickerShadow" };
                        int cp_ids[] = { Shadow::GuiCol_ColorPickerDark, Shadow::GuiCol_ColorPickerLight, Shadow::GuiCol_CheckerboardLight, Shadow::GuiCol_CheckerboardDark, Shadow::GuiCol_ColorPickerShadow };

                        if (Shadow::TreeNode("Backgrounds")) {
                            for (size_t i = 0; i < sizeof(bg_ids) / sizeof(bg_ids[0]); ++i) {
                                Shadow::Color& c = Shadow::g_Ctx.Style.Colors[bg_ids[i]];
                                Shadow::ColorPicker(bg_names_en[i], &c.r, &c.g, &c.b, &c.a);
                            }
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode("Texts")) {
                            for (size_t i = 0; i < sizeof(text_ids) / sizeof(text_ids[0]); ++i) {
                                Shadow::Color& c = Shadow::g_Ctx.Style.Colors[text_ids[i]];
                                Shadow::ColorPicker(text_names_en[i], &c.r, &c.g, &c.b, &c.a);
                            }
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode("Controls & Borders")) {
                            for (size_t i = 0; i < sizeof(btn_ids) / sizeof(btn_ids[0]); ++i) {
                                Shadow::Color& c = Shadow::g_Ctx.Style.Colors[btn_ids[i]];
                                Shadow::ColorPicker(btn_names_en[i], &c.r, &c.g, &c.b, &c.a);
                            }
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode("Color Picker Specific")) {
                            for (size_t i = 0; i < sizeof(cp_ids) / sizeof(cp_ids[0]); ++i) {
                                Shadow::Color& c = Shadow::g_Ctx.Style.Colors[cp_ids[i]];
                                Shadow::ColorPicker(cp_names_en[i], &c.r, &c.g, &c.b, &c.a);
                            }
                        }
                        Shadow::TreePop();
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("Style Metrics & Themes")) {
                        static int theme_idx = 0;
                        std::vector<std::string> themes = { "Dark", "Ocean", "Amethyst", "Grey" };
                        if (Shadow::Combo("Theme", &theme_idx, themes)) {
                            switch (theme_idx) {
                            case 0: Shadow::StyleColorsDark(); break;
                            case 1: Shadow::StyleColorsOcean(); break;
                            case 2: Shadow::StyleColorsAmethyst(); break;
                            case 3: Shadow::StyleColorsGrey(); break;
                            }
                        }
                        Shadow::HelpMarker("Theme switch demo using StyleColorsDark / Ocean / Amethyst / Grey.");

                        auto& style = Shadow::GetStyle();
                        Shadow::Slider("WindowPadding.x", &style.WindowPadding.x, 0.f, 64.f, 1.f);
                        Shadow::Slider("WindowPadding.y", &style.WindowPadding.y, 0.f, 64.f, 1.f);
                        Shadow::Slider("FramePadding.x", &style.FramePadding.x, 0.f, 32.f, 1.f);
                        Shadow::Slider("FramePadding.y", &style.FramePadding.y, 0.f, 32.f, 1.f);
                        Shadow::Slider("ItemSpacing.x", &style.ItemSpacing.x, 0.f, 64.f, 1.f);
                        Shadow::Slider("ItemSpacing.y", &style.ItemSpacing.y, 0.f, 64.f, 1.f);
                        Shadow::Slider("ScrollbarSize", &style.ScrollbarSize, 4.f, 32.f, 1.f);
                        Shadow::Slider("ScrollbarMargin", &style.ScrollbarMargin, 0.f, 32.f, 1.f);
                        Shadow::Slider("ResizeGripSize", &style.ResizeGripSize, 4.f, 32.f, 1.f);
                        Shadow::Slider("TabExtraWidth", &style.TabExtraWidth, 0.f, 64.f, 1.f);
                        Shadow::Slider("ControlOffsetMin", &style.ControlOffsetMin, 0.f, 400.f, 1.f);
                        Shadow::Slider("ControlOffsetRatio", &style.ControlOffsetRatio, 0.f, 1.f, 0.01f);
                        Shadow::Slider("FontScaleDpi", &style.FontScaleDpi, 0.5f, 3.f, 0.05f);
                        Shadow::HelpMarker("Style metrics editor using GetStyle().");
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("Clipboard")) {
                        static std::string clip_text = "Shadow Clipboard";
                        Shadow::InputText("Clipboard Text", clip_text);
                        if (Shadow::Button("Copy to Clipboard")) {
                            Shadow::SetClipboardText(clip_text);
                        }
                        Shadow::SameLine();
                        if (Shadow::Button("Paste from Clipboard")) {
                            clip_text = Shadow::GetClipboardText();
                        }
                        Shadow::HelpMarker("SetClipboardText / GetClipboardText demo.");
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("Window Pos/Size & Constraints")) {
                        Vec2 pos = Shadow::GetWindowPos();
                        Vec2 size = Shadow::GetWindowSize();
                        Shadow::Text(std::format("Window Pos: {:.1f}, {:.1f}", pos.x, pos.y));
                        Shadow::Text(std::format("Window Size: {:.1f}, {:.1f}", size.x, size.y));

                        if (Shadow::Button("SetWindowPos to 50,50")) {
                            Shadow::SetWindowPos({ 50.f, 50.f });
                        }
                        Shadow::SameLine();
                        if (Shadow::Button("SetNextWindowPos to 200,200")) {
                            Shadow::SetNextWindowPos({ 200.f, 200.f });
                        }
                        if (Shadow::Button("SetNextWindowSize 600x500")) {
                            Shadow::SetNextWindowSize({ 600.f, 500.f });
                        }
                        Shadow::SameLine();
                        if (Shadow::Button("SetNextWindowSizeConstraints 300x300 - 800x700")) {
                            Shadow::SetNextWindowSizeConstraints({ 300.f, 300.f }, { 800.f, 700.f });
                        }
                        Shadow::HelpMarker("Window position/size and constraints demo. Next window calls affect next frame Begin().");
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("DrawList Channels")) {
                        Vec2 p = Shadow::g_Ctx.Cursor;
                        Shadow::Dummy({ 220.f, 100.f });

                        ShadowDrawList* dl = Shadow::GetWindowDrawList();
                        dl->ChannelsSplit(2);
                        dl->SetChannel(0);
                        dl->AddRectFilled(p, { 120.f, 80.f }, { 1.f, 0.f, 0.f, 0.8f });
                        dl->SetChannel(1);
                        dl->AddRectFilled({ p.x + 60.f, p.y + 20.f }, { 120.f, 80.f }, { 0.f, 0.f, 1.f, 0.8f });
                        dl->ChannelsMerge();

                        Shadow::HelpMarker("ChannelsSplit / SetChannel / ChannelsMerge demo.");
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("DrawList Texture & PushTexture")) {
                        static SDK::UTexture2D* circleTex = Shadow::LoadTextureFromBuffer(Shadow_Titlebar::Logo, sizeof(Shadow_Titlebar::Logo));

                        Vec2 p = Shadow::g_Ctx.Cursor;
                        Shadow::Dummy({ 200.f, 80.f });

                        ShadowDrawList* dl = Shadow::GetWindowDrawList();
                        if (circleTex) {
                            dl->AddTexture(p, { 64.f, 64.f }, { 1.f, 1.f, 1.f, 1.f }, circleTex);
                            Shadow::PushTexture(circleTex);
                            dl->AddTexture({ p.x + 80.f, p.y }, { 64.f, 64.f }, { 1.f, 0.5f, 0.5f, 1.f });
                            Shadow::PopTexture();
                        }

                        Shadow::HelpMarker("AddTexture explicit texture and via PushTexture / PopTexture.");
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("Font, PixelSnap & ID Stack")) {
                        Shadow::PushFontNoSDF(Shadow::g_Ctx.DefaultFont, 1.5f);
                        Shadow::Text("PushFontNoSDF 1.5x");
                        Shadow::PopFont();
                        Shadow::HelpMarker("PushFontNoSDF / PopFont demo.");

                        Shadow::PushTextPixelSnap(true);
                        Shadow::Text("Pixel snapped text");
                        Shadow::PopTextPixelSnap();
                        Shadow::HelpMarker("PushTextPixelSnap / PopTextPixelSnap demo.");

                        Shadow::PushID("id_a");
                        Shadow::Button("Same Label##1");
                        Shadow::PopID();
                        Shadow::PushID("id_b");
                        Shadow::Button("Same Label##1");
                        Shadow::PopID();
                        Shadow::HelpMarker("PushID / PopID demo with same visible label.");
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("Popup Examples")) {
                        static int popup_counter = 0;
                        static std::string popupName = "my_popup##my_popup";

                        // Open popup button (and set position)
                        if (Shadow::Button("Open Popup")) {
                            Shadow::SetNextWindowPos({ Shadow::g_Ctx.MousePos.x + 20, Shadow::g_Ctx.MousePos.y + 20 });
                            Shadow::OpenPopup(Shadow::HashString(popupName));
                        }
                        Shadow::SameLine();
                        Shadow::TextColored(Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_TextDisabled],
                            "(Click to open a popup at mouse position)");

                        // Show current popup status
                        bool is_open = Shadow::IsPopupOpen(Shadow::HashString(popupName));
                        std::string status_text = std::format("Popup is currently: {}", is_open ? "Open" : "Closed");

                        Shadow::Text(status_text);
                        Shadow::SameLine();

                        if (Shadow::Button("Close Popup")) {
                            Shadow::CloseCurrentPopup();
                        }

                        // Define popup content
                        if (Shadow::BeginPopup(popupName, Shadow::ShadowWindowFlags_None)) {
                            Shadow::Text("This is a popup!");

                            // Counter display
                            std::string counter_text = std::format("Counter: {}", popup_counter);

                            Shadow::Text(counter_text);

                            if (Shadow::Button("Increment")) {
                                popup_counter++;
                            }

                            Shadow::SameLine();

                            if (Shadow::Button("Close")) {
                                Shadow::CloseCurrentPopup();
                            }
                        }
                        Shadow::EndPopup();
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("ListBox Demos")) {

                        if (Shadow::TreeNode("ListBox with Selectables")) {
                            static int selectedItem = 0;
                            std::vector<std::string> items = { "Apple", "Banana", "Cherry", "Date", "Elderberry", "Fig", "Grape", "Honeydew", "Kiwi", "Lemon" };

                            if (Shadow::BeginListBox("FruitListBox", { 200.f, 150.f })) {
                                for (int i = 0; i < static_cast<int>(items.size()); i++) {
                                    bool isSelected = (selectedItem == i);
                                    if (Shadow::Selectable(items[i], &isSelected)) {
                                        selectedItem = i;
                                    }
                                }
                            }
                            Shadow::EndListBox();
                            Shadow::HelpMarker("ListBox demonstrating internal Selectable usage for item selection.");
                            Shadow::Text(std::string("Selected: ") + items[selectedItem]);
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode("ListBox with Color Texts")) {
                            struct ColorEntry {
                                std::string name;
                                Shadow::Color color;
                            };

                            std::vector<ColorEntry> colors = {
                                { "Red",    { 1.0f, 0.0f, 0.0f, 1.0f } },
                                { "Green",  { 0.0f, 1.0f, 0.0f, 1.0f } },
                                { "Blue",   { 0.0f, 0.0f, 1.0f, 1.0f } },
                                { "Yellow", { 1.0f, 1.0f, 0.0f, 1.0f } },
                                { "Cyan",   { 0.0f, 1.0f, 1.0f, 1.0f } },
                                { "Magenta",{ 1.0f, 0.0f, 1.0f, 1.0f } },
                                { "Orange", { 1.0f, 0.5f, 0.0f, 1.0f } }
                            };

                            if (Shadow::BeginListBox("ColorTextListBox", { 200.f, 150.f })) {
                                for (const auto& entry : colors) {
                                    Shadow::TextColored(entry.color, entry.name);
                                }
                            }
                            Shadow::EndListBox();
                            Shadow::HelpMarker("ListBox demonstrating TextColored items with various colors.");
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode("ListBox with Color Pickers")) {
                            static std::vector<Shadow::Color> customColors = {
                                { 1.0f, 0.0f, 0.0f, 1.0f },  // Red
                                { 0.0f, 1.0f, 0.0f, 1.0f },  // Green
                                { 0.0f, 0.0f, 1.0f, 1.0f },  // Blue
                                { 1.0f, 1.0f, 0.0f, 1.0f },  // Yellow
                                { 0.0f, 1.0f, 1.0f, 1.0f }   // Cyan
                            };

                            if (Shadow::BeginListBox("ColorPickerListBox", { 300.f, 180.f })) {
                                for (int i = 0; i < static_cast<int>(customColors.size()); i++) {
                                    Shadow::ColorPicker(
                                        std::string("Color ") + std::to_string(i + 1) + "##cp",
                                        &customColors[i].r,
                                        &customColors[i].g,
                                        &customColors[i].b,
                                        &customColors[i].a
                                    );
                                }
                            }
                            Shadow::EndListBox();
                            Shadow::HelpMarker("ListBox demonstrating embedded ColorPicker controls.");
                        }
                        Shadow::TreePop();

                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode("IO")) {
                        Shadow::ShadowIO& IO = Shadow::GetIO();
                        float fps = IO.DeltaTime > 0 ? 1.f / IO.DeltaTime : 0.f;
                        std::string fps_str = std::format("FPS: {:.0f}", fps);
                        Shadow::Text(fps_str);
                        Shadow::HelpMarker("using Shadow::GetIO().DeltaTime to get FPS.");

                        float screen_w = IO.DisplaySize.x;
                        float screen_h = IO.DisplaySize.y;
                        std::string screen_size_str = std::format("Screen: {} / {}", screen_w, screen_h);
                        Shadow::Text(screen_size_str);
                    }
                    Shadow::TreePop();

                }
                Shadow::EndTabItem(); // Unconditional EndTabItem

            }
            Shadow::EndTabBar(); // Unconditional EndTabBar

        }
        Shadow::End(); // Unconditional End
    }
} // namespace Shadow