#pragma once
#include <string>
#include <vector>
#include <format>
#include <algorithm>

// 包含原始SDK路径
#include "external/CppSDK/SDK.hpp"

// U8 宏定义
#ifndef U8
#define U8(str) reinterpret_cast<const char*>(u8##str)
#endif

namespace Shadow {

    // ============================================================================
    // 基础自定义组合控件：HelpMarker
    // ============================================================================
    inline void HelpMarker(std::string_view desc, float TextWrap = 200.f) {
        Shadow::SameLine();
        Shadow::TextColored(Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_TextDisabled], U8("(?)"));
        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_DelayNone)) {
            Shadow::BeginTooltip();
            Shadow::PushTextWrapPos(TextWrap);
            Shadow::TextWrapped({ 1.0f, 1.0f, 1.0f, 1.0f }, desc.data());
            Shadow::PopTextWrapPos();
            Shadow::EndTooltip();
        }
    }

    // ============================================================================
    // 高级自定义组合控件 (11种+)
    // ============================================================================
    namespace Advanced {

        // 1. 属性行 (左侧文字 + 右侧对齐文字)
        inline void PropertyRow(std::string_view propName, std::string_view propValue) {
            Shadow::TextDisabled(propName);
            Shadow::SameLine(150.f);
            Shadow::Text(propValue);
        }

        // 2. 密码切换框 (Checkbox + SameLine + InputText)
        inline void PasswordToggle(std::string_view label, std::string& pwd, bool& show) {
            Shadow::Checkbox(std::string(U8("Show##")).append(label), &show);
            Shadow::SameLine();
            Shadow::ShadowInputTextFlags flags = show ? Shadow::ShadowInputTextFlags_None : Shadow::ShadowInputTextFlags_Password;
            Shadow::InputText(label, pwd, flags);
        }

        // 3. 搜索框 (Button + SameLine + InputTextWithHint)
        inline bool SearchBox(std::string_view label, std::string& query) {
            bool clicked = Shadow::Button(std::string(U8("Search##")).append(label));
            Shadow::SameLine();
            Shadow::InputTextWithHint(label, U8("Type to search..."), query);
            return clicked;
        }

        // 4. 进度条 (自定义 DrawRectFilled + 文字居中覆盖)
        inline void ProgressBar(float fraction, std::string_view overlay = "") {
            Shadow::Vec2 cur = Shadow::g_Ctx.Cursor;
            float width = Shadow::g_Ctx.WindowSize.x - Shadow::g_Ctx.Style.WindowPadding.x * 2.f - Shadow::g_Ctx.IndentX;
            float height = Shadow::g_Ctx.ItemHeight;
            Shadow::Vec2 size = { std::max(50.f, width), height };

            Shadow::Color bg = Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_FrameBg];
            Shadow::Color fg = Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_SliderGrab];
            Shadow::DrawRectFilled(cur, size, bg);
            Shadow::DrawRectFilled(cur, { size.x * std::clamp(fraction, 0.f, 1.f), size.y }, fg);

            std::string text = overlay.empty() ? std::to_string(static_cast<int>(fraction * 100)) + "%" : std::string(overlay);
            Shadow::Vec2 textSize = Shadow::MeasureTextSize(text);
            Shadow::Vec2 textPos = { cur.x + (size.x - textSize.x) * 0.5f, cur.y + Shadow::g_Ctx.Style.FramePadding.y };
            Shadow::DrawTextString(text, textPos, Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_Text]);

            Shadow::Dummy(size);
        }

        // 5. 颜色色块按钮 (自定义实心块渲染 + 悬停交互)
        inline bool ColorButton(std::string_view id, Shadow::Color col) {
            Shadow::Vec2 cur = Shadow::g_Ctx.Cursor;
            Shadow::Vec2 size = { Shadow::g_Ctx.ItemHeight, Shadow::g_Ctx.ItemHeight };
            bool clicked = false;
            if (Shadow::IsRectVisible(cur, size)) {
                bool hovered = Shadow::IsMouseHovering(cur, size);
                if (hovered && Shadow::g_Ctx.MouseClicked) clicked = true;
                Shadow::Color border = hovered ? Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_TextHighlight] : Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_Border];
                Shadow::DrawRectFilled(cur, size, col);
                Shadow::DrawRect(cur, size, border);
            }
            Shadow::SetLastItemInfo(cur, { cur.x + size.x, cur.y + size.y }, Shadow::HashString(id), false);
            Shadow::g_Ctx.LastItemMaxX = cur.x + size.x;
            Shadow::g_Ctx.Cursor.y += size.y + Shadow::g_Ctx.Style.ItemSpacing.y;
            Shadow::g_Ctx.Cursor.x = Shadow::g_Ctx.WindowPos.x + Shadow::g_Ctx.Style.WindowPadding.x + Shadow::g_Ctx.IndentX;
            return clicked;
        }

        // 6. 列表选择器 (TreeNode Framed + Selectables组合)
        inline bool ListSelector(std::string_view label, const std::vector<std::string>& items, int& selectedIndex) {
            bool changed = false;
            if (Shadow::TreeNode(label, Shadow::ShadowTreeNodeFlags_Framed)) {
                for (int i = 0; i < static_cast<int>(items.size()); ++i) {
                    bool isSelected = (selectedIndex == i);
                    if (Shadow::Selectable(std::string(items[i]).append(U8("##")).append(label), &isSelected)) {
                        selectedIndex = i;
                        changed = true;
                    }
                }
            }
            Shadow::TreePop(); // 无条件Pop
            return changed;
        }

        // 7. 徽章标签 (自动根据文字计算宽度绘制色块)
        inline void Badge(std::string_view text, Shadow::Color bgCol, Shadow::Color textCol) {
            Shadow::Vec2 cur = Shadow::g_Ctx.Cursor;
            Shadow::Vec2 textSize = Shadow::MeasureTextSize(text);
            Shadow::Vec2 padding = { 8.f, 2.f };
            Shadow::Vec2 size = { textSize.x + padding.x * 2.f, textSize.y + padding.y * 2.f };

            Shadow::DrawRectFilled(cur, size, bgCol);
            Shadow::DrawTextString(text, { cur.x + padding.x, cur.y + padding.y }, textCol);

            Shadow::Dummy(size);
        }

        // 8. 整数步进器 (Button + SameLine + Text + SameLine + Button)
        inline bool StepperInt(std::string_view label, int& v, int step = 1, int vmin = 0, int vmax = 100) {
            bool changed = false;
            Shadow::Text(label);
            Shadow::SameLine(120.f);
            if (Shadow::Button(std::string(U8("-##")).append(label))) { v = std::max(vmin, v - step); changed = true; }
            Shadow::SameLine();
            Shadow::TextColored(Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_TextHighlight], std::to_string(v));
            Shadow::SameLine();
            if (Shadow::Button(std::string(U8("+##")).append(label))) { v = std::min(vmax, v + step); changed = true; }
            return changed;
        }

        // 9. 状态切换按钮 (动态变更文字和背景色的交互矩形)
        inline bool ToggleButton(std::string_view label, bool& state) {
            Shadow::Color bg = state ? Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_ButtonHovered] : Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_Button];
            Shadow::Color fg = state ? Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_TextHighlight] : Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_TextDisabled];

            std::string display = state ? std::string(U8("[ON] ")).append(label) : std::string(U8("[OFF] ")).append(label);

            Shadow::Vec2 cur = Shadow::g_Ctx.Cursor;
            Shadow::Vec2 textSize = Shadow::MeasureTextSize(display);
            Shadow::Vec2 size = { textSize.x + Shadow::g_Ctx.Style.FramePadding.x * 2.f, Shadow::g_Ctx.ItemHeight };

            bool clicked = false;
            if (Shadow::IsRectVisible(cur, size)) {
                bool hovered = Shadow::IsMouseHovering(cur, size);
                if (hovered && Shadow::g_Ctx.MouseClicked) { state = !state; clicked = true; }
                Shadow::Color drawBg = hovered ? Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_FrameBgHovered] : bg;
                Shadow::DrawRectFilled(cur, size, drawBg);
                Shadow::DrawTextString(display, { cur.x + Shadow::g_Ctx.Style.FramePadding.x, cur.y + Shadow::g_Ctx.Style.FramePadding.y }, fg);
            }
            Shadow::SetLastItemInfo(cur, { cur.x + size.x, cur.y + size.y }, Shadow::HashString(label), false);
            Shadow::g_Ctx.LastItemMaxX = cur.x + size.x;
            Shadow::g_Ctx.Cursor.y += size.y + Shadow::g_Ctx.Style.ItemSpacing.y;
            Shadow::g_Ctx.Cursor.x = Shadow::g_Ctx.WindowPos.x + Shadow::g_Ctx.Style.WindowPadding.x + Shadow::g_Ctx.IndentX;

            return clicked;
        }

        // 10. 分区标题 (Separator + TextColored + Separator)
        inline void SectionHeader(std::string_view label) {
            Shadow::Separator();
            Shadow::TextColored(Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_TextHighlight], label);
            Shadow::Separator();
        }

        // 11. 自定义带左侧文字紧凑型拉条 (Text + SameLine + 无右侧间距Slider)
        inline void LabeledSlider(std::string_view label, float* v, float min_v, float max_v) {
            Shadow::Text(label);
            Shadow::SameLine();
            Shadow::Slider(std::string(U8("##")).append(label), v, min_v, max_v, 0.f, Shadow::ShadowSliderFlags_NoText | Shadow::ShadowSliderFlags_NoRightAlign);
        }

    } // namespace Advanced

    // ============================================================================
    // 综合展示 Demo
    // ============================================================================
    inline void ShowDemoWindow() {
        static bool show_demo = true;
        if (!show_demo) return;

        static Shadow::ShadowWindowFlags win_flags = Shadow::ShadowWindowFlags_None;
        static Shadow::ShadowTabBarFlags tab_flags = Shadow::ShadowTabBarFlags_Reorderable | Shadow::ShadowTabBarFlags_FittingPolicyScroll;

        if (Shadow::Begin(U8("Shadow GUI Standard Demo##DemoWindow"), win_flags)) {

            if (Shadow::BeginTabBar(U8("DemoTabBar"), tab_flags)) {

                // ----------------------------------------------------------------
                // 第一页面: 全部标准控件与其全功能Flag测试
                // ----------------------------------------------------------------
                if (Shadow::BeginTabItem(U8("Standard Controls"))) {

                    if (Shadow::TreeNode(U8("Text Elements"))) {
                        Shadow::Text(U8("Standard Text"));
                        Shadow::HelpMarker(U8("Basic text output using Shadow::Text."));

                        Shadow::TextDisabled(U8("Disabled Text"));
                        Shadow::HelpMarker(U8("Text with disabled color using Shadow::TextDisabled."));

                        Shadow::TextColored({ 1.f, 0.5f, 0.5f, 1.f }, U8("Colored Text"));
                        Shadow::HelpMarker(U8("Text with custom RGB color using Shadow::TextColored."));

                        Shadow::TextWrapped({ 0.5f, 1.f, 0.5f, 1.f }, U8("This is a long wrapped text that will automatically break into multiple lines depending on the window width..."));
                        Shadow::HelpMarker(U8("Wrapped text adjusts to window/clip width automatically using Shadow::TextWrapped."));
                    }
                    Shadow::TreePop(); // 无条件Pop

                    if (Shadow::TreeNode(U8("Basic Widgets"))) {
                        static bool bVal1 = false, bVal2 = true;
                        Shadow::Checkbox(U8("Checkbox 1"), &bVal1);
                        Shadow::HelpMarker(U8("Standard Checkbox widget."));

                        Shadow::Switch(U8("Switch 1"), &bVal2);
                        Shadow::HelpMarker(U8("Standard Switch toggle widget."));

                        if (Shadow::Button(U8("Click Me"))) {}
                        Shadow::HelpMarker(U8("Standard Button widget."));
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(U8("Selectables & Combos"))) {
                        static bool sel1 = false, sel2 = true;
                        Shadow::Selectable(U8("Selectable 1"), &sel1);
                        Shadow::HelpMarker(U8("Selectable taking a boolean pointer (toggle mode)."));

                        Shadow::Selectable(U8("Selectable 2 (No Pointer)"), sel2);
                        Shadow::HelpMarker(U8("Selectable taking a simple boolean (trigger mode)."));

                        static int combo_idx = 0;
                        std::vector<std::string> combo_items = { U8("Item A"), U8("Item B"), U8("Item C") };

                        Shadow::Combo(U8("Standard Combo"), &combo_idx, combo_items);
                        Shadow::HelpMarker(U8("Standard combo box (ShadowComboFlags_None)."));

                        Shadow::Combo(U8("Combo NoText"), &combo_idx, combo_items, Shadow::ShadowComboFlags_NoText);
                        Shadow::HelpMarker(U8("Combo without left label (ShadowComboFlags_NoText)."));

                        Shadow::Combo(U8("Combo NoRightAlign"), &combo_idx, combo_items, Shadow::ShadowComboFlags_NoRightAlign);
                        Shadow::HelpMarker(U8("Combo tightly packed next to text (ShadowComboFlags_NoRightAlign)."));

                        Shadow::Combo(U8("Combo FitText"), &combo_idx, combo_items, Shadow::ShadowComboFlags_FitText);
                        Shadow::HelpMarker(U8("Combo width adapts to selected text length (ShadowComboFlags_FitText)."));
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(U8("Sliders"))) {
                        static float s1 = 0.5f, s2 = 1.0f, s3 = 50.f;
                        Shadow::Slider(U8("Standard Slider"), &s1, 0.f, 1.f);
                        Shadow::HelpMarker(U8("Basic float slider (ShadowSliderFlags_None)."));

                        Shadow::Slider(U8("Slider NoText"), &s2, 0.f, 10.f, 0.f, Shadow::ShadowSliderFlags_NoText);
                        Shadow::HelpMarker(U8("Slider without label (ShadowSliderFlags_NoText)."));

                        Shadow::Slider(U8("Slider NoRightAlign"), &s3, 0.f, 100.f, 1.f, Shadow::ShadowSliderFlags_NoRightAlign);
                        Shadow::HelpMarker(U8("Slider that doesn't stretch to right margin (ShadowSliderFlags_NoRightAlign)."));
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(U8("Inputs (Text & Float)"))) {
                        static std::string txt1 = U8(""), txt2 = U8("123"), txt3 = U8("secret");

                        Shadow::InputText(U8("Standard Input"), txt1);
                        Shadow::HelpMarker(U8("Standard text input (ShadowInputTextFlags_None)."));

                        Shadow::InputTextWithHint(U8("Hint Input"), U8("Enter your name..."), txt1);
                        Shadow::HelpMarker(U8("Input with background hint text."));

                        Shadow::InputText(U8("Decimal Only"), txt2, Shadow::ShadowInputTextFlags_CharsDecimal);
                        Shadow::HelpMarker(U8("Only numbers allowed (ShadowInputTextFlags_CharsDecimal)."));

                        Shadow::InputText(U8("Hexadecimal Only"), txt2, Shadow::ShadowInputTextFlags_CharsHexadecimal);
                        Shadow::HelpMarker(U8("Only hex allowed (ShadowInputTextFlags_CharsHexadecimal)."));

                        Shadow::InputText(U8("Scientific Only"), txt2, Shadow::ShadowInputTextFlags_CharsScientific);
                        Shadow::HelpMarker(U8("Scientific numbers (ShadowInputTextFlags_CharsScientific)."));

                        Shadow::InputText(U8("Uppercase Only"), txt2, Shadow::ShadowInputTextFlags_CharsUppercase);
                        Shadow::HelpMarker(U8("Auto converts to uppercase (ShadowInputTextFlags_CharsUppercase)."));

                        Shadow::InputText(U8("No Blanks"), txt2, Shadow::ShadowInputTextFlags_CharsNoBlank);
                        Shadow::HelpMarker(U8("Spaces restricted (ShadowInputTextFlags_CharsNoBlank)."));

                        Shadow::InputText(U8("Password"), txt3, Shadow::ShadowInputTextFlags_Password);
                        Shadow::HelpMarker(U8("Text obfuscated with * (ShadowInputTextFlags_Password)."));

                        Shadow::InputText(U8("Read Only"), txt3, Shadow::ShadowInputTextFlags_ReadOnly);
                        Shadow::HelpMarker(U8("Cannot be modified (ShadowInputTextFlags_ReadOnly)."));

                        Shadow::InputText(U8("Escape Clears"), txt2, Shadow::ShadowInputTextFlags_EscapeClearsAll);
                        Shadow::HelpMarker(U8("Press Esc to clear text (ShadowInputTextFlags_EscapeClearsAll)."));

                        Shadow::InputText(U8("Auto Select All"), txt2, Shadow::ShadowInputTextFlags_AutoSelectAll);
                        Shadow::HelpMarker(U8("Highlights all text on click (ShadowInputTextFlags_AutoSelectAll)."));

                        static float f1 = 3.14f;
                        Shadow::InputFloat(U8("Float Input"), &f1);
                        Shadow::HelpMarker(U8("Standard float input."));

                        Shadow::InputFloat(U8("Float EmptyRef"), &f1, 0.1f, 1.0f, "{:.2f}", Shadow::ShadowInputTextFlags_ParseEmptyRefVal | Shadow::ShadowInputTextFlags_DisplayEmptyRefVal);
                        Shadow::HelpMarker(U8("Handles empty value parsing and display."));
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(U8("Color Pickers"))) {
                        static float r = 1.f, g = 0.f, b = 0.f, a = 1.f;
                        Shadow::ColorPicker(U8("Standard Picker"), &r, &g, &b, &a);
                        Shadow::HelpMarker(U8("Standard RGBA Picker (ShadowColorPickerFlags_None)."));

                        Shadow::ColorPicker(U8("Picker NoText"), &r, &g, &b, &a, Shadow::ShadowColorPickerFlags_NoText);
                        Shadow::HelpMarker(U8("Picker without left label (ShadowColorPickerFlags_NoText)."));

                        Shadow::ColorPicker(U8("Picker NoRightAlign"), &r, &g, &b, &a, Shadow::ShadowColorPickerFlags_NoRightAlign);
                        Shadow::HelpMarker(U8("Picker packed tightly (ShadowColorPickerFlags_NoRightAlign)."));
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(U8("Hotkeys"))) {
                        static int hk1 = 0;
                        static int hk2 = 0x41; // 'A'
                        static bool hk2_active = false;
                        static Shadow::HotkeyMode hk2_mode = Shadow::HotkeyMode::ToggleOn;

                        Shadow::HotKey(U8("Simple Hotkey"), &hk1);
                        Shadow::HelpMarker(U8("Registers simple hotkey taking int* only."));

                        Shadow::HotKey(U8("Advanced Hotkey"), &hk2, &hk2_active, &hk2_mode);
                        Shadow::HelpMarker(U8("Hotkey with mode selection. Right-click to configure mode."));

                        Shadow::HotKey(U8("Hotkey NoText"), &hk2, &hk2_active, &hk2_mode, Shadow::ShadowHotkeyFlags_NoText);
                        Shadow::HelpMarker(U8("Hotkey without label (ShadowHotkeyFlags_NoText)."));

                        Shadow::HotKey(U8("Hotkey NoRightAlign"), &hk2, &hk2_active, &hk2_mode, Shadow::ShadowHotkeyFlags_NoRightAlign);
                        Shadow::HelpMarker(U8("Hotkey tightly packed (ShadowHotkeyFlags_NoRightAlign)."));

                        Shadow::HotKey(U8("Hotkey NoStateDisplay"), &hk2, &hk2_active, &hk2_mode, Shadow::ShadowHotkeyFlags_NoStateDisplay);
                        Shadow::HelpMarker(U8("Hotkey without status indicator dot (ShadowHotkeyFlags_NoStateDisplay)."));
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(U8("Layout, Spacing & Stack Modifiers"))) {
                        Shadow::Text(U8("Item 1"));
                        Shadow::SameLine();
                        Shadow::Text(U8("Item 2 (Same Line)"));
                        Shadow::HelpMarker(U8("Uses SameLine() without args to pack horizontally."));

                        Shadow::Text(U8("Item 3"));
                        Shadow::SameLine(150.f);
                        Shadow::Text(U8("Item 4 (SameLine offset)"));
                        Shadow::HelpMarker(U8("Uses SameLine(150.f) to force alignment to 150px."));

                        Shadow::Text(U8("Item 5"));
                        Shadow::SameLine(0.f, 40.f);
                        Shadow::Text(U8("Item 6 (SameLine space)"));
                        Shadow::HelpMarker(U8("Uses SameLine(0.f, 40.f) to force 40px spacing."));

                        Shadow::Separator();
                        Shadow::HelpMarker(U8("Separator() line above."));

                        Shadow::Text(U8("Below is a 50x30 Dummy"));
                        Shadow::Dummy({ 50.f, 30.f });
                        Shadow::Text(U8("After Dummy"));
                        Shadow::HelpMarker(U8("Dummy inserts empty space manually."));

                        Shadow::Spacing();
                        Shadow::HelpMarker(U8("Spacing() adds vertical padding between items."));

                        // Text Wrap Stack
                        Shadow::PushTextWrapPos(150.f);
                        Shadow::Text(U8("This text is forcibly wrapped at 150 pixels due to PushTextWrapPos."));
                        Shadow::PopTextWrapPos();
                        Shadow::HelpMarker(U8("PushTextWrapPos / PopTextWrapPos Demonstration."));

                        // Clip Rect Stack
                        Shadow::Vec2 cur = Shadow::g_Ctx.Cursor;
                        Shadow::PushClipRect(cur, { cur.x + 100.f, cur.y + Shadow::g_Ctx.ItemHeight });
                        Shadow::Text(U8("This long text will be physically clipped at 100px width."));
                        Shadow::PopClipRect();
                        Shadow::HelpMarker(U8("PushClipRect / PopClipRect Demonstration."));

                        // Font Stack
                        Shadow::PushFont(Shadow::g_Ctx.DefaultFont, 1.25f);
                        Shadow::Text(U8("Text with Font scale 1.25x"));
                        Shadow::PopFont();
                        Shadow::HelpMarker(U8("PushFont / PopFont Demonstration."));
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(U8("Disabled Status"))) {
                        static bool disable_all = true;
                        Shadow::Checkbox(U8("Disable the following group"), &disable_all);

                        Shadow::BeginDisabled(disable_all);
                        Shadow::Button(U8("Disabled Button"));
                        static float dummy_f = 0.5f;
                        Shadow::Slider(U8("Disabled Slider"), &dummy_f, 0.f, 1.f);
                        Shadow::EndDisabled();

                        Shadow::HelpMarker(U8("Widgets bounded by BeginDisabled(bool) / EndDisabled()."));
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(U8("TreeNodes & Flags"))) {
                        if (Shadow::TreeNode(U8("Default Open Node"), Shadow::ShadowTreeNodeFlags_DefaultOpen)) {
                            Shadow::Text(U8("This is open by default."));
                        }
                        Shadow::TreePop(); // 无条件Pop

                        if (Shadow::TreeNode(U8("Framed Node"), Shadow::ShadowTreeNodeFlags_Framed)) {
                            Shadow::Text(U8("This node has a background frame."));
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode(U8("FitText Framed Node"), Shadow::ShadowTreeNodeFlags_Framed | Shadow::ShadowTreeNodeFlags_FitText)) {
                            Shadow::Text(U8("The frame stops at the text end, ignoring right margin."));
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode(U8("No Indent Node"), Shadow::ShadowTreeNodeFlags_NoIndent)) {
                            Shadow::Text(U8("Children inside this node are not indented horizontally."));
                        }
                        Shadow::TreePop();
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(U8("Hovered Flags Demo"))) {
                        Shadow::Button(U8("Hover Me (Normal)"));
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_None)) {
                            Shadow::BeginTooltip(); Shadow::Text(U8("Normal Hover")); Shadow::EndTooltip();
                        }

                        Shadow::Button(U8("Hover Me (Delay Short)"));
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_DelayShort)) {
                            Shadow::BeginTooltip(); Shadow::Text(U8("Short Delay Hover Tooltip")); Shadow::EndTooltip();
                        }

                        Shadow::Button(U8("Hover Me (Delay Normal)"));
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_DelayNormal)) {
                            Shadow::BeginTooltip(); Shadow::Text(U8("Normal Delay Hover Tooltip")); Shadow::EndTooltip();
                        }

                        Shadow::Button(U8("Hover Me (Stationary)"));
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_Stationary)) {
                            Shadow::BeginTooltip(); Shadow::Text(U8("Stationary Hover Tooltip")); Shadow::EndTooltip();
                        }

                        Shadow::BeginDisabled(true);
                        Shadow::Button(U8("Disabled Button Hover"));
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_AllowWhenDisabled)) {
                            Shadow::BeginTooltip(); Shadow::Text(U8("Hover triggered despite being disabled!")); Shadow::EndTooltip();
                        }
                        Shadow::EndDisabled();
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(U8("Window Flags Editor"))) {
                        static bool f_NoResize = false, f_NoMove = false, f_NoScrollbar = false, f_NoTitleBar = false, f_NoMouseInputs = false;

                        if (Shadow::Selectable(U8("NoResize"), &f_NoResize)) { if (f_NoResize) win_flags |= Shadow::ShadowWindowFlags_NoResize; else win_flags &= ~Shadow::ShadowWindowFlags_NoResize; }
                        if (Shadow::Selectable(U8("NoMove"), &f_NoMove)) { if (f_NoMove) win_flags |= Shadow::ShadowWindowFlags_NoMove; else win_flags &= ~Shadow::ShadowWindowFlags_NoMove; }
                        if (Shadow::Selectable(U8("NoScrollbar"), &f_NoScrollbar)) { if (f_NoScrollbar) win_flags |= Shadow::ShadowWindowFlags_NoScrollbar; else win_flags &= ~Shadow::ShadowWindowFlags_NoScrollbar; }
                        if (Shadow::Selectable(U8("NoTitleBar"), &f_NoTitleBar)) { if (f_NoTitleBar) win_flags |= Shadow::ShadowWindowFlags_NoTitleBar; else win_flags &= ~Shadow::ShadowWindowFlags_NoTitleBar; }
                        if (Shadow::Selectable(U8("NoMouseInputs"), &f_NoMouseInputs)) { if (f_NoMouseInputs) win_flags |= Shadow::ShadowWindowFlags_NoMouseInputs; else win_flags &= ~Shadow::ShadowWindowFlags_NoMouseInputs; }

                        static int align_idx = 0;
                        std::vector<std::string> aligns = { U8("Left"), U8("Center"), U8("Right") };
                        Shadow::Combo(U8("TextAlign (Applies to Title)"), &align_idx, aligns);
                        win_flags &= ~(Shadow::ShadowWindowFlags_TextAlignCenter | Shadow::ShadowWindowFlags_TextAlignRight);
                        if (align_idx == 1) win_flags |= Shadow::ShadowWindowFlags_TextAlignCenter;
                        if (align_idx == 2) win_flags |= Shadow::ShadowWindowFlags_TextAlignRight;
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(U8("TabBar Flags Editor"))) {
                        static bool t_Reorderable = true, t_FittingPolicyScroll = true, t_NoScrollbar = false;

                        if (Shadow::Selectable(U8("Reorderable"), &t_Reorderable)) { if (t_Reorderable) tab_flags |= Shadow::ShadowTabBarFlags_Reorderable; else tab_flags &= ~Shadow::ShadowTabBarFlags_Reorderable; }
                        if (Shadow::Selectable(U8("FittingPolicyScroll"), &t_FittingPolicyScroll)) { if (t_FittingPolicyScroll) tab_flags |= Shadow::ShadowTabBarFlags_FittingPolicyScroll; else tab_flags &= ~Shadow::ShadowTabBarFlags_FittingPolicyScroll; }
                        if (Shadow::Selectable(U8("NoScrollbar"), &t_NoScrollbar)) { if (t_NoScrollbar) tab_flags |= Shadow::ShadowTabBarFlags_NoScrollbar; else tab_flags &= ~Shadow::ShadowTabBarFlags_NoScrollbar; }
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(U8("Style Menu Colors Editor"))) {
                        const char* bg_names[] = { U8("WindowBg"), U8("TitleBarBg"), U8("FrameBg"), U8("FrameBgHovered"), U8("PopupBg"), U8("Tab"), U8("TabHovered"), U8("TabActive"), U8("SwitchBg"), U8("SwitchBgHovered"), U8("SwitchBgActive"), U8("SwitchBgActiveHovered"), U8("DropdownActive") };
                        int bg_ids[] = { Shadow::GuiCol_WindowBg, Shadow::GuiCol_TitleBarBg, Shadow::GuiCol_FrameBg, Shadow::GuiCol_FrameBgHovered, Shadow::GuiCol_PopupBg, Shadow::GuiCol_Tab, Shadow::GuiCol_TabHovered, Shadow::GuiCol_TabActive, Shadow::GuiCol_SwitchBg, Shadow::GuiCol_SwitchBgHovered, Shadow::GuiCol_SwitchBgActive, Shadow::GuiCol_SwitchBgActiveHovered, Shadow::GuiCol_DropdownActive };

                        const char* text_names[] = { U8("Text"), U8("TextDisabled"), U8("TextHighlight"), U8("ErrorText"), U8("TextShadow"), U8("TextOutline") };
                        int text_ids[] = { Shadow::GuiCol_Text, Shadow::GuiCol_TextDisabled, Shadow::GuiCol_TextHighlight, Shadow::GuiCol_ErrorText, Shadow::GuiCol_TextShadow, Shadow::GuiCol_TextOutline };

                        const char* btn_names[] = { U8("Button"), U8("ButtonHovered"), U8("SliderGrab"), U8("SliderKnob"), U8("CheckMark"), U8("Separator"), U8("ResizeGrip"), U8("ResizeGripHovered"), U8("ResizeGripActive"), U8("ActiveIndicator"), U8("InactiveIndicator"), U8("Border"), U8("PopupBorder"), U8("ControlDisabled"), U8("SwitchKnob") };
                        int btn_ids[] = { Shadow::GuiCol_Button, Shadow::GuiCol_ButtonHovered, Shadow::GuiCol_SliderGrab, Shadow::GuiCol_SliderKnob, Shadow::GuiCol_CheckMark, Shadow::GuiCol_Separator, Shadow::GuiCol_ResizeGrip, Shadow::GuiCol_ResizeGripHovered, Shadow::GuiCol_ResizeGripActive, Shadow::GuiCol_ActiveIndicator, Shadow::GuiCol_InactiveIndicator, Shadow::GuiCol_Border, Shadow::GuiCol_PopupBorder, Shadow::GuiCol_ControlDisabled, Shadow::GuiCol_SwitchKnob };

                        const char* cp_names[] = { U8("ColorPickerDark"), U8("ColorPickerLight"), U8("CheckerboardLight"), U8("CheckerboardDark"), U8("ColorPickerShadow") };
                        int cp_ids[] = { Shadow::GuiCol_ColorPickerDark, Shadow::GuiCol_ColorPickerLight, Shadow::GuiCol_CheckerboardLight, Shadow::GuiCol_CheckerboardDark, Shadow::GuiCol_ColorPickerShadow };

                        if (Shadow::TreeNode(U8("Backgrounds"))) {
                            for (size_t i = 0; i < sizeof(bg_ids) / sizeof(bg_ids[0]); ++i) {
                                Shadow::Color& c = Shadow::g_Ctx.Style.Colors[bg_ids[i]];
                                Shadow::ColorPicker(bg_names[i], &c.r, &c.g, &c.b, &c.a);
                            }
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode(U8("Texts"))) {
                            for (size_t i = 0; i < sizeof(text_ids) / sizeof(text_ids[0]); ++i) {
                                Shadow::Color& c = Shadow::g_Ctx.Style.Colors[text_ids[i]];
                                Shadow::ColorPicker(text_names[i], &c.r, &c.g, &c.b, &c.a);
                            }
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode(U8("Controls & Borders"))) {
                            for (size_t i = 0; i < sizeof(btn_ids) / sizeof(btn_ids[0]); ++i) {
                                Shadow::Color& c = Shadow::g_Ctx.Style.Colors[btn_ids[i]];
                                Shadow::ColorPicker(btn_names[i], &c.r, &c.g, &c.b, &c.a);
                            }
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode(U8("Color Picker Specific"))) {
                            for (size_t i = 0; i < sizeof(cp_ids) / sizeof(cp_ids[0]); ++i) {
                                Shadow::Color& c = Shadow::g_Ctx.Style.Colors[cp_ids[i]];
                                Shadow::ColorPicker(cp_names[i], &c.r, &c.g, &c.b, &c.a);
                            }
                        }
                        Shadow::TreePop();
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(U8("ListBox Demos"))) {

                        // ============================================
                        // 示例1：ListBox 内部使用 Selectable
                        // ============================================
                        if (Shadow::TreeNode(U8("ListBox with Selectables"))) {
                            static int selectedItem = 0;
                            std::vector<std::string> items = {
                                U8("Apple"), U8("Banana"), U8("Cherry"),
                                U8("Date"), U8("Elderberry"), U8("Fig"),
                                U8("Grape"), U8("Honeydew"), U8("Kiwi"), U8("Lemon")
                            };

                            if (Shadow::BeginListBox(U8("FruitListBox"), { 200.f, 150.f })) {
                                for (int i = 0; i < static_cast<int>(items.size()); i++) {
                                    bool isSelected = (selectedItem == i);
                                    if (Shadow::Selectable(items[i], &isSelected)) {
                                        selectedItem = i;
                                    }
                                }
                            }
                            Shadow::EndListBox();
                            Shadow::HelpMarker(U8("ListBox demonstrating internal Selectable usage for item selection."));

                            // 显示当前选中项
                            Shadow::Text(std::format("Selected: {}", items[selectedItem]));
                        }
                        Shadow::TreePop();

                        // ============================================
                        // 示例2：ListBox 内部使用 Color Text
                        // ============================================
                        if (Shadow::TreeNode(U8("ListBox with Color Texts"))) {
                            struct ColorEntry {
                                std::string name;
                                Shadow::Color color;
                            };

                            std::vector<ColorEntry> colors = {
                                { U8("Red"),    { 1.0f, 0.0f, 0.0f, 1.0f } },
                                { U8("Green"),  { 0.0f, 1.0f, 0.0f, 1.0f } },
                                { U8("Blue"),   { 0.0f, 0.0f, 1.0f, 1.0f } },
                                { U8("Yellow"), { 1.0f, 1.0f, 0.0f, 1.0f } },
                                { U8("Cyan"),   { 0.0f, 1.0f, 1.0f, 1.0f } },
                                { U8("Magenta"),{ 1.0f, 0.0f, 1.0f, 1.0f } },
                                { U8("Orange"), { 1.0f, 0.5f, 0.0f, 1.0f } },
                                { U8("Purple"), { 0.5f, 0.0f, 0.5f, 1.0f } },
                                { U8("Pink"),   { 1.0f, 0.7f, 0.7f, 1.0f } },
                                { U8("Lime"),   { 0.5f, 1.0f, 0.0f, 1.0f } },
                                { U8("Teal"),   { 0.0f, 0.5f, 0.5f, 1.0f } },
                                { U8("Brown"),  { 0.6f, 0.3f, 0.0f, 1.0f } },
                                { U8("Gray"),   { 0.5f, 0.5f, 0.5f, 1.0f } },
                                { U8("White"),  { 1.0f, 1.0f, 1.0f, 1.0f } }
                            };

                            if (Shadow::BeginListBox(U8("ColorTextListBox"), { 200.f, 150.f })) {
                                for (const auto& entry : colors) {
                                    Shadow::TextColored(entry.color, entry.name);
                                }
                            }
                            Shadow::EndListBox();
                            Shadow::HelpMarker(U8("ListBox demonstrating TextColored items with various colors."));
                        }
                        Shadow::TreePop();

                        // ============================================
                        // 示例3：ListBox 内部使用 Color Picker
                        // ============================================
                        if (Shadow::TreeNode(U8("ListBox with Color Pickers"))) {
                            static std::vector<Shadow::Color> customColors = {
                                { 1.0f, 0.0f, 0.0f, 1.0f },  // Red
                                { 0.0f, 1.0f, 0.0f, 1.0f },  // Green
                                { 0.0f, 0.0f, 1.0f, 1.0f },  // Blue
                                { 1.0f, 1.0f, 0.0f, 1.0f },  // Yellow
                                { 0.0f, 1.0f, 1.0f, 1.0f }   // Cyan
                            };

                            if (Shadow::BeginListBox(U8("ColorPickerListBox"), { 300.f, 180.f })) {
                                for (int i = 0; i < static_cast<int>(customColors.size()); i++) {
                                    Shadow::ColorPicker(
                                        std::format("Color {}##cp", i + 1),
                                        &customColors[i].r,
                                        &customColors[i].g,
                                        &customColors[i].b,
                                        &customColors[i].a
                                    );
                                }
                            }
                            Shadow::EndListBox();
                            Shadow::HelpMarker(U8("ListBox demonstrating embedded ColorPicker controls. Use scroll to see all items."));

                            // 显示当前所有颜色预览
                            Shadow::Text(U8("Color Previews:"));
                            for (int i = 0; i < static_cast<int>(customColors.size()); i++) {
                                Shadow::SameLine();
                                Advanced::ColorButton(std::format("prev_{}", i), customColors[i]);
                            }
                        }
                        Shadow::TreePop();

                    }
                    Shadow::TreePop();

                }
                Shadow::EndTabItem(); // 无条件EndTabItem

                // ----------------------------------------------------------------
                // 第二页面: 高级自定义组合控件 (至少10种)
                // ----------------------------------------------------------------
                if (Shadow::BeginTabItem(U8("Advanced Custom Controls"))) {
                    Advanced::SectionHeader(U8("Advanced Custom Composites (Total 11+)"));

                    Advanced::PropertyRow(U8("Player Speed"), U8("420.0 (Readonly Property Demo)"));
                    Shadow::HelpMarker(U8("This is Advanced Control 1: PropertyRow"));

                    static std::string pwd = U8("my_secret");
                    static bool show_pwd = false;
                    Advanced::PasswordToggle(U8("API Key"), pwd, show_pwd);
                    Shadow::HelpMarker(U8("This is Advanced Control 2: PasswordToggle"));

                    static std::string search_query = U8("");
                    if (Advanced::SearchBox(U8("Database"), search_query)) {
                        // triggered
                    }
                    Shadow::HelpMarker(U8("This is Advanced Control 3: SearchBox"));

                    static float prog = 0.65f;
                    Shadow::Slider(U8("Adjust Progress"), &prog, 0.f, 1.f);
                    Advanced::ProgressBar(prog, U8("Loading Assets..."));
                    Shadow::HelpMarker(U8("This is Advanced Control 4: ProgressBar"));

                    Shadow::Text(U8("Theme Colors:"));
                    Shadow::SameLine(); Advanced::ColorButton(U8("cb1"), { 0.8f,0.2f,0.2f,1.f });
                    Shadow::SameLine(); Advanced::ColorButton(U8("cb2"), { 0.2f,0.8f,0.2f,1.f });
                    Shadow::SameLine(); Advanced::ColorButton(U8("cb3"), { 0.2f,0.2f,0.8f,1.f });
                    Shadow::HelpMarker(U8("This is Advanced Control 5: ColorButton"));

                    static int list_idx = 0;
                    std::vector<std::string> list_items = { U8("Assault Rifle"), U8("Sniper"), U8("Shotgun") };
                    Advanced::ListSelector(U8("Weapon Preset"), list_items, list_idx);
                    Shadow::HelpMarker(U8("This is Advanced Control 6: ListSelector"));

                    Shadow::Text(U8("Status Badges: "));
                    Shadow::SameLine(); Advanced::Badge(U8("NEW"), { 0.8f,0.2f,0.2f,1.f }, { 1.f,1.f,1.f,1.f });
                    Shadow::SameLine(); Advanced::Badge(U8("PRO"), { 0.8f,0.6f,0.1f,1.f }, { 0.1f,0.1f,0.1f,1.f });
                    Shadow::HelpMarker(U8("This is Advanced Control 7: Badge"));

                    static int step_val = 50;
                    Advanced::StepperInt(U8("Volume Step"), step_val, 5, 0, 100);
                    Shadow::HelpMarker(U8("This is Advanced Control 8: StepperInt"));

                    static bool t_state = false;
                    Advanced::ToggleButton(U8("God Mode Status"), t_state);
                    Shadow::HelpMarker(U8("This is Advanced Control 9: ToggleButton"));

                    static float slider_val = 0.75f;
                    Advanced::LabeledSlider(U8("Intensity Filter"), &slider_val, 0.f, 1.f);
                    Shadow::HelpMarker(U8("This is Advanced Control 10: LabeledSlider"));

                    Advanced::SectionHeader(U8("End of Advanced Demo"));
                    Shadow::HelpMarker(U8("This is Advanced Control 11: SectionHeader"));
                }
                Shadow::EndTabItem(); // 无条件EndTabItem

            }
            Shadow::EndTabBar(); // 无条件EndTabBar

        }
        Shadow::End(); // 无条件End
    }

} // namespace Shadow