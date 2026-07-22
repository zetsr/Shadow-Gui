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
    // 语言控制与辅助函数
    // ============================================================================
    inline bool g_UseChinese = false; // 全局语言状态

    // 翻译辅助函数，返回对应的语言字符串
    inline const char* T(const char* en, const char* zh) {
        return g_UseChinese ? zh : en;
    }

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
            Shadow::Checkbox(std::string(T(U8("Show##"), U8("显示##"))).append(label), &show);
            Shadow::SameLine();
            Shadow::ShadowInputTextFlags flags = show ? Shadow::ShadowInputTextFlags_None : Shadow::ShadowInputTextFlags_Password;
            Shadow::InputText(label, pwd, flags);
        }

        // 3. 搜索框 (Button + SameLine + InputTextWithHint)
        inline bool SearchBox(std::string_view label, std::string& query) {
            bool clicked = Shadow::Button(std::string(T(U8("Search##"), U8("搜索##"))).append(label));
            Shadow::SameLine();
            Shadow::InputTextWithHint(label, T(U8("Type to search..."), U8("输入以搜索...")), query);
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

            std::string display = state ? std::string(T(U8("[ON] "), U8("[开] "))).append(label) : std::string(T(U8("[OFF] "), U8("[关] "))).append(label);

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

        // 12. 范围滑块组合 (Min/Max Slider 模拟)
        inline void RangeSliderFloat(std::string_view label, float& min_v, float& max_v, float min_limit, float max_limit) {
            Shadow::Text(label);
            Shadow::SameLine(120.f);
            Shadow::Text(T(U8("Min"), U8("最小")));
            Shadow::SameLine();
            Shadow::Slider(std::string(U8("##min")).append(label), &min_v, min_limit, max_v, 0.f, Shadow::ShadowSliderFlags_NoText | Shadow::ShadowSliderFlags_NoRightAlign);
            Shadow::SameLine();
            Shadow::Text(T(U8("Max"), U8("最大")));
            Shadow::SameLine();
            Shadow::Slider(std::string(U8("##max")).append(label), &max_v, min_v, max_limit, 0.f, Shadow::ShadowSliderFlags_NoText | Shadow::ShadowSliderFlags_NoRightAlign);
        }

        // 13. 分页导航控件 (Pagination)
        inline bool Pagination(int& current_page, int total_pages) {
            bool changed = false;
            if (Shadow::Button(T(U8("< Prev"), U8("< 上一页"))) && current_page > 1) {
                current_page--; changed = true;
            }
            Shadow::SameLine();
            std::string text = g_UseChinese
                ? (U8("第 ") + std::to_string(current_page) + U8(" 页，共 ") + std::to_string(total_pages) + U8(" 页"))
                : (U8("Page ") + std::to_string(current_page) + U8(" of ") + std::to_string(total_pages));

            Shadow::Text(text);
            Shadow::SameLine();
            if (Shadow::Button(T(U8("Next >"), U8("下一页 >"))) && current_page < total_pages) {
                current_page++; changed = true;
            }
            return changed;
        }

        // 14. 多选列表树 (Multi-Select List)
        inline bool MultiSelectList(std::string_view label, const std::vector<std::string>& items, std::vector<bool>& selections) {
            bool changed = false;
            if (Shadow::TreeNode(label, Shadow::ShadowTreeNodeFlags_Framed)) {
                for (size_t i = 0; i < items.size(); ++i) {
                    if (i >= selections.size()) break;

                    bool temp_val = selections[i];
                    if (Shadow::Checkbox(std::string(items[i]).append(U8("##multi_")).append(label), &temp_val)) {
                        selections[i] = temp_val;
                        changed = true;
                    }
                }
            }
            Shadow::TreePop();
            return changed;
        }

        // 15. 标签输入与管理 (Tags Input)
        inline void TagsInput(std::string_view label, std::vector<std::string>& tags, std::string& input_buffer) {
            Shadow::Text(label);
            Shadow::SameLine();
            Shadow::InputTextWithHint(std::string(U8("##")).append(label), T(U8("Add tag & click +"), U8("添加标签并点击 +")), input_buffer, Shadow::ShadowInputTextFlags_NoName, { Shadow::MeasureTextSize(input_buffer.size() > 0 ? input_buffer.c_str() : T(U8("Add tag & click +"), U8("添加标签并点击 +"))).x + Shadow::g_Ctx.Style.WindowPadding.x });
            Shadow::SameLine();
            if (Shadow::Button(std::string(U8("+##")).append(label)) && !input_buffer.empty()) {
                tags.push_back(input_buffer);
                input_buffer.clear();
            }

            // 绘制标签列表
            for (size_t i = 0; i < tags.size(); ++i) {
                Advanced::Badge(tags[i], Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_Button], Shadow::g_Ctx.Style.Colors[Shadow::GuiCol_Text]);
                Shadow::SameLine();
                if (Shadow::Button(std::string(U8("x##")).append(std::to_string(i)).append(label))) {
                    tags.erase(tags.begin() + i);
                    i--; // 抵消自增
                }
                if (i < tags.size() - 1) {
                    Shadow::SameLine();
                }
            }
            if (!tags.empty()) Shadow::Dummy({ 0, 5.f });
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

        if (Shadow::Begin(T(U8("Shadow GUI Standard Demo##DemoWindow"), U8("Shadow GUI 标准演示##DemoWindow")), win_flags)) {

            // 添加顶部切换语言控件
            Shadow::Switch(T(U8("切换为中文"), U8("Switch to English")), &g_UseChinese);
            Shadow::Separator();

            if (Shadow::BeginTabBar(T(U8("DemoTabBar"), U8("演示标签栏")), tab_flags)) {

                // ----------------------------------------------------------------
                // 第一页面: 全部标准控件与其全功能Flag测试 (补充了组合Flag演示)
                // ----------------------------------------------------------------
                if (Shadow::BeginTabItem(T(U8("Standard Controls"), U8("标准控件")))) {

                    if (Shadow::TreeNode(T(U8("Text Elements"), U8("文本元素")))) {
                        Shadow::Text(T(U8("Standard Text"), U8("标准文本")));
                        Shadow::HelpMarker(T(U8("Basic text output using Shadow::Text."), U8("使用 Shadow::Text 的基础文本输出。")));

                        Shadow::TextDisabled(T(U8("Disabled Text"), U8("禁用文本")));
                        Shadow::HelpMarker(T(U8("Text with disabled color using Shadow::TextDisabled."), U8("使用 Shadow::TextDisabled 的禁用颜色文本。")));

                        Shadow::TextColored({ 1.f, 0.5f, 0.5f, 1.f }, T(U8("Colored Text"), U8("彩色文本")));
                        Shadow::HelpMarker(T(U8("Text with custom RGB color using Shadow::TextColored."), U8("使用 Shadow::TextColored 的自定义 RGB 颜色文本。")));

                        Shadow::TextWrapped({ 0.5f, 1.f, 0.5f, 1.f }, T(U8("This is a long wrapped text that will automatically break into multiple lines depending on the window width..."), U8("这是一段很长的自动换行文本，它会根据窗口宽度自动折行...")));
                        Shadow::HelpMarker(T(U8("Wrapped text adjusts to window/clip width automatically using Shadow::TextWrapped."), U8("使用 Shadow::TextWrapped 根据窗口/裁剪宽度自动调整换行。")));
                    }
                    Shadow::TreePop(); // 无条件Pop

                    if (Shadow::TreeNode(T(U8("Basic Widgets"), U8("基础小部件")))) {
                        static bool bVal1 = false, bVal2 = true;
                        Shadow::Checkbox(T(U8("Checkbox 1"), U8("复选框 1")), &bVal1);
                        Shadow::HelpMarker(T(U8("Standard Checkbox widget."), U8("标准复选框小部件。")));

                        Shadow::Switch(T(U8("Switch 1"), U8("开关 1")), &bVal2);
                        Shadow::HelpMarker(T(U8("Standard Switch toggle widget."), U8("标准开关切换小部件。")));

                        if (Shadow::Button(T(U8("Click Me"), U8("点击我")))) {}
                        Shadow::HelpMarker(T(U8("Standard Button widget."), U8("标准按钮小部件。")));
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(T(U8("Selectables & Combos"), U8("可选项与下拉框")))) {
                        static bool sel1 = false, sel2 = true;
                        Shadow::Selectable(T(U8("Selectable 1"), U8("可选项 1")), &sel1);
                        Shadow::HelpMarker(T(U8("Selectable taking a boolean pointer (toggle mode)."), U8("接受布尔指针的可选项（切换模式）。")));

                        Shadow::Selectable(T(U8("Selectable 2 (No Pointer)"), U8("可选项 2 (无指针)")), sel2);
                        Shadow::HelpMarker(T(U8("Selectable taking a simple boolean (trigger mode)."), U8("接受简单布尔值的可选项（触发模式）。")));

                        static int combo_idx = 0;
                        std::vector<std::string> combo_items = g_UseChinese ?
                            std::vector<std::string>{ U8("项目 A"), U8("项目 B"), U8("项目 C") } :
                            std::vector<std::string>{ U8("Item A"), U8("Item B"), U8("Item C") };

                        Shadow::Combo(T(U8("Standard Combo"), U8("标准下拉框")), &combo_idx, combo_items);
                        Shadow::HelpMarker(T(U8("Standard combo box (ShadowComboFlags_None)."), U8("标准下拉框 (ShadowComboFlags_None)。")));

                        Shadow::Combo(T(U8("Combo NoText"), U8("无文本下拉框")), &combo_idx, combo_items, Shadow::ShadowComboFlags_NoText);
                        Shadow::HelpMarker(T(U8("Combo without left label (ShadowComboFlags_NoText)."), U8("没有左侧标签的下拉框 (ShadowComboFlags_NoText)。")));

                        Shadow::Combo(T(U8("Combo NoRightAlign"), U8("无右对齐下拉框")), &combo_idx, combo_items, Shadow::ShadowComboFlags_NoRightAlign);
                        Shadow::HelpMarker(T(U8("Combo tightly packed next to text (ShadowComboFlags_NoRightAlign)."), U8("紧靠文本的下拉框 (ShadowComboFlags_NoRightAlign)。")));

                        Shadow::Combo(T(U8("Combo FitText"), U8("自适应文本下拉框")), &combo_idx, combo_items, Shadow::ShadowComboFlags_FitText);
                        Shadow::HelpMarker(T(U8("Combo width adapts to selected text length (ShadowComboFlags_FitText)."), U8("下拉框宽度自适应选中文字长度 (ShadowComboFlags_FitText)。")));

                        // Combo Flags 组合演示
                        Shadow::Combo(T(U8("Combo NoText+NoRightAlign"), U8("无文本+无右对齐下拉框")), &combo_idx, combo_items, Shadow::ShadowComboFlags_NoText | Shadow::ShadowComboFlags_NoRightAlign);
                        Shadow::HelpMarker(T(U8("Combines NoText and NoRightAlign."), U8("结合无文本与无右对齐。")));

                        Shadow::Combo(T(U8("Combo FitText+NoRightAlign"), U8("自适应文本+无右对齐下拉框")), &combo_idx, combo_items, Shadow::ShadowComboFlags_FitText | Shadow::ShadowComboFlags_NoRightAlign);
                        Shadow::HelpMarker(T(U8("Combines FitText and NoRightAlign."), U8("结合自适应文本与无右对齐。")));

                        Shadow::Combo(T(U8("Combo NoText+FitText"), U8("无文本+自适应文本下拉框")), &combo_idx, combo_items, Shadow::ShadowComboFlags_NoText | Shadow::ShadowComboFlags_FitText);
                        Shadow::HelpMarker(T(U8("Combines NoText and FitText."), U8("结合无文本与自适应文本。")));

                        Shadow::Combo(T(U8("Combo NoText+NoRightAlign+FitText"), U8("无文本+无右对齐+自适应下拉框")), &combo_idx, combo_items, Shadow::ShadowComboFlags_NoText | Shadow::ShadowComboFlags_NoRightAlign | Shadow::ShadowComboFlags_FitText);
                        Shadow::HelpMarker(T(U8("Combines NoText, NoRightAlign, and FitText."), U8("结合无文本、无右对齐和自适应文本。")));
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(T(U8("Sliders"), U8("滑块")))) {
                        static float s1 = 0.5f, s2 = 1.0f, s3 = 50.f, s4 = 75.f;
                        Shadow::Slider(T(U8("Standard Slider"), U8("标准滑块")), &s1, 0.f, 1.f);
                        Shadow::HelpMarker(T(U8("Basic float slider (ShadowSliderFlags_None)."), U8("基础浮点滑块 (ShadowSliderFlags_None)。")));

                        Shadow::Slider(T(U8("Slider NoText"), U8("无文本滑块")), &s2, 0.f, 10.f, 0.f, Shadow::ShadowSliderFlags_NoText);
                        Shadow::HelpMarker(T(U8("Slider without label (ShadowSliderFlags_NoText)."), U8("无标签的滑块 (ShadowSliderFlags_NoText)。")));

                        Shadow::Slider(T(U8("Slider NoRightAlign"), U8("无右对齐滑块")), &s3, 0.f, 100.f, 1.f, Shadow::ShadowSliderFlags_NoRightAlign);
                        Shadow::HelpMarker(T(U8("Slider that doesn't stretch to right margin (ShadowSliderFlags_NoRightAlign)."), U8("不延伸到右边缘的滑块 (ShadowSliderFlags_NoRightAlign)。")));

                        // Slider Flags 组合演示
                        Shadow::Slider(T(U8("Slider NoText+NoRightAlign"), U8("无文本+无右对齐滑块")), &s4, 0.f, 100.f, 0.f, Shadow::ShadowSliderFlags_NoText | Shadow::ShadowSliderFlags_NoRightAlign);
                        Shadow::HelpMarker(T(U8("Combines NoText and NoRightAlign."), U8("结合无文本与无右对齐。")));
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(T(U8("Inputs (Text & Float)"), U8("输入框 (文本与浮点)")))) {
                        static std::string txt1 = U8(""), txt2 = U8("123"), txt3 = U8("secret");

                        Shadow::InputText(T(U8("Standard Input"), U8("标准输入")), txt1);
                        Shadow::HelpMarker(T(U8("Standard text input (ShadowInputTextFlags_None)."), U8("标准文本输入 (ShadowInputTextFlags_None)。")));

                        Shadow::InputTextWithHint(T(U8("Hint Input"), U8("提示输入")), T(U8("Enter your name..."), U8("输入你的名字...")), txt1);
                        Shadow::HelpMarker(T(U8("Input with background hint text."), U8("带有背景提示文本的输入框。")));

                        Shadow::InputText(T(U8("Decimal Only"), U8("仅限小数")), txt2, Shadow::ShadowInputTextFlags_CharsDecimal);
                        Shadow::HelpMarker(T(U8("Only numbers allowed (ShadowInputTextFlags_CharsDecimal)."), U8("仅允许输入数字 (ShadowInputTextFlags_CharsDecimal)。")));

                        Shadow::InputText(T(U8("Hexadecimal Only"), U8("仅限十六进制")), txt2, Shadow::ShadowInputTextFlags_CharsHexadecimal);
                        Shadow::HelpMarker(T(U8("Only hex allowed (ShadowInputTextFlags_CharsHexadecimal)."), U8("仅允许输入十六进制 (ShadowInputTextFlags_CharsHexadecimal)。")));

                        Shadow::InputText(T(U8("Scientific Only"), U8("仅限科学计数法")), txt2, Shadow::ShadowInputTextFlags_CharsScientific);
                        Shadow::HelpMarker(T(U8("Scientific numbers (ShadowInputTextFlags_CharsScientific)."), U8("科学计数法数字 (ShadowInputTextFlags_CharsScientific)。")));

                        Shadow::InputText(T(U8("Uppercase Only"), U8("仅限大写")), txt2, Shadow::ShadowInputTextFlags_CharsUppercase);
                        Shadow::HelpMarker(T(U8("Auto converts to uppercase (ShadowInputTextFlags_CharsUppercase)."), U8("自动转换为大写 (ShadowInputTextFlags_CharsUppercase)。")));

                        Shadow::InputText(T(U8("No Blanks"), U8("无空格")), txt2, Shadow::ShadowInputTextFlags_CharsNoBlank);
                        Shadow::HelpMarker(T(U8("Spaces restricted (ShadowInputTextFlags_CharsNoBlank)."), U8("禁止输入空格 (ShadowInputTextFlags_CharsNoBlank)。")));

                        Shadow::InputText(T(U8("Password"), U8("密码")), txt3, Shadow::ShadowInputTextFlags_Password);
                        Shadow::HelpMarker(T(U8("Text obfuscated with * (ShadowInputTextFlags_Password)."), U8("使用 * 混淆文本 (ShadowInputTextFlags_Password)。")));

                        Shadow::InputText(T(U8("Read Only"), U8("只读")), txt3, Shadow::ShadowInputTextFlags_ReadOnly);
                        Shadow::HelpMarker(T(U8("Cannot be modified (ShadowInputTextFlags_ReadOnly)."), U8("无法修改 (ShadowInputTextFlags_ReadOnly)。")));

                        Shadow::InputText(T(U8("Escape Clears"), U8("Esc清除")), txt2, Shadow::ShadowInputTextFlags_EscapeClearsAll);
                        Shadow::HelpMarker(T(U8("Press Esc to clear text (ShadowInputTextFlags_EscapeClearsAll)."), U8("按 Esc 清除文本 (ShadowInputTextFlags_EscapeClearsAll)。")));

                        Shadow::InputText(T(U8("Auto Select All"), U8("自动全选")), txt2, Shadow::ShadowInputTextFlags_AutoSelectAll);
                        Shadow::HelpMarker(T(U8("Highlights all text on click (ShadowInputTextFlags_AutoSelectAll)."), U8("点击时高亮显示所有文本 (ShadowInputTextFlags_AutoSelectAll)。")));

                        // InputText Flags 组合演示
                        static std::string txt_combo1 = U8("my_secret");
                        Shadow::InputText(T(U8("Pwd+AutoSel+EscClr"), U8("密码+全选+Esc清除")), txt_combo1, Shadow::ShadowInputTextFlags_Password | Shadow::ShadowInputTextFlags_AutoSelectAll | Shadow::ShadowInputTextFlags_EscapeClearsAll);
                        Shadow::HelpMarker(T(U8("Combines Password, AutoSelectAll, and EscapeClearsAll."), U8("结合密码、自动全选和Esc清除。")));

                        static std::string txt_combo2 = U8("12345");
                        Shadow::InputText(T(U8("Dec+NoBlank+AutoSel"), U8("小数+无空格+全选")), txt_combo2, Shadow::ShadowInputTextFlags_CharsDecimal | Shadow::ShadowInputTextFlags_CharsNoBlank | Shadow::ShadowInputTextFlags_AutoSelectAll);
                        Shadow::HelpMarker(T(U8("Combines CharsDecimal, CharsNoBlank, and AutoSelectAll."), U8("结合仅小数、无空格和自动全选。")));

                        static std::string txt_combo3 = U8("readonly");
                        Shadow::InputText(T(U8("ReadOnly+AutoSel"), U8("只读+全选")), txt_combo3, Shadow::ShadowInputTextFlags_ReadOnly | Shadow::ShadowInputTextFlags_AutoSelectAll);
                        Shadow::HelpMarker(T(U8("Combines ReadOnly and AutoSelectAll."), U8("结合只读和自动全选。")));

                        static std::string txt_combo4 = U8("UPPER_NO_SPACE");
                        Shadow::InputText(T(U8("Upper+NoBlank"), U8("大写+无空格")), txt_combo4, Shadow::ShadowInputTextFlags_CharsUppercase | Shadow::ShadowInputTextFlags_CharsNoBlank);
                        Shadow::HelpMarker(T(U8("Combines CharsUppercase and CharsNoBlank."), U8("结合仅大写和无空格。")));

                        static float f1 = 3.14f;
                        Shadow::InputFloat(T(U8("Float Input"), U8("浮点输入")), &f1);
                        Shadow::HelpMarker(T(U8("Standard float input."), U8("标准浮点输入。")));

                        Shadow::InputFloat(T(U8("Float EmptyRef"), U8("空引用的浮点")), &f1, 0.1f, 1.0f, "{:.2f}", Shadow::ShadowInputTextFlags_ParseEmptyRefVal | Shadow::ShadowInputTextFlags_DisplayEmptyRefVal);
                        Shadow::HelpMarker(T(U8("Handles empty value parsing and display."), U8("处理空值的解析和显示。")));
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(T(U8("Color Pickers"), U8("拾色器")))) {
                        static float r = 1.f, g = 0.f, b = 0.f, a = 1.f;
                        Shadow::ColorPicker(T(U8("Standard Picker"), U8("标准拾色器")), &r, &g, &b, &a);
                        Shadow::HelpMarker(T(U8("Standard RGBA Picker (ShadowColorPickerFlags_None)."), U8("标准 RGBA 拾色器 (ShadowColorPickerFlags_None)。")));

                        Shadow::ColorPicker(T(U8("Picker NoText"), U8("无文本拾色器")), &r, &g, &b, &a, Shadow::ShadowColorPickerFlags_NoText);
                        Shadow::HelpMarker(T(U8("Picker without left label (ShadowColorPickerFlags_NoText)."), U8("无左侧标签的拾色器 (ShadowColorPickerFlags_NoText)。")));

                        Shadow::ColorPicker(T(U8("Picker NoRightAlign"), U8("无右对齐拾色器")), &r, &g, &b, &a, Shadow::ShadowColorPickerFlags_NoRightAlign);
                        Shadow::HelpMarker(T(U8("Picker packed tightly (ShadowColorPickerFlags_NoRightAlign)."), U8("紧凑排列的拾色器 (ShadowColorPickerFlags_NoRightAlign)。")));

                        // ColorPicker Flags 组合演示
                        Shadow::ColorPicker(T(U8("Picker NoText+NoRightAlign"), U8("无文本+无右对齐拾色器")), &r, &g, &b, &a, Shadow::ShadowColorPickerFlags_NoText | Shadow::ShadowColorPickerFlags_NoRightAlign);
                        Shadow::HelpMarker(T(U8("Combines NoText and NoRightAlign."), U8("结合无文本与无右对齐。")));
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(T(U8("Hotkeys"), U8("快捷键")))) {
                        static int hk1 = 0;
                        static int hk2 = 0x41; // 'A'
                        static bool hk2_active = false;
                        static Shadow::HotkeyMode hk2_mode = Shadow::HotkeyMode::ToggleOn;

                        Shadow::HotKey(T(U8("Simple Hotkey"), U8("简单快捷键")), &hk1);
                        Shadow::HelpMarker(T(U8("Registers simple hotkey taking int* only."), U8("仅使用 int* 注册简单的快捷键。")));

                        Shadow::HotKey(T(U8("Advanced Hotkey"), U8("高级快捷键")), &hk2, &hk2_active, &hk2_mode);
                        Shadow::HelpMarker(T(U8("Hotkey with mode selection. Right-click to configure mode."), U8("具有模式选择的快捷键。右键点击配置模式。")));

                        Shadow::HotKey(T(U8("Hotkey NoText"), U8("无文本快捷键")), &hk2, &hk2_active, &hk2_mode, Shadow::ShadowHotkeyFlags_NoText);
                        Shadow::HelpMarker(T(U8("Hotkey without label (ShadowHotkeyFlags_NoText)."), U8("无标签快捷键 (ShadowHotkeyFlags_NoText)。")));

                        Shadow::HotKey(T(U8("Hotkey NoRightAlign"), U8("无右对齐快捷键")), &hk2, &hk2_active, &hk2_mode, Shadow::ShadowHotkeyFlags_NoRightAlign);
                        Shadow::HelpMarker(T(U8("Hotkey tightly packed (ShadowHotkeyFlags_NoRightAlign)."), U8("紧凑快捷键 (ShadowHotkeyFlags_NoRightAlign)。")));

                        Shadow::HotKey(T(U8("Hotkey NoStateDisplay"), U8("无状态显示快捷键")), &hk2, &hk2_active, &hk2_mode, Shadow::ShadowHotkeyFlags_NoStateDisplay);
                        Shadow::HelpMarker(T(U8("Hotkey without status indicator dot (ShadowHotkeyFlags_NoStateDisplay)."), U8("没有状态指示点的快捷键 (ShadowHotkeyFlags_NoStateDisplay)。")));

                        // Hotkeys Flags 组合演示
                        Shadow::HotKey(T(U8("HK NoText+NoRightAlign"), U8("快捷键 无文本+无右对齐")), &hk2, &hk2_active, &hk2_mode, Shadow::ShadowHotkeyFlags_NoText | Shadow::ShadowHotkeyFlags_NoRightAlign);
                        Shadow::HelpMarker(T(U8("Combines NoText and NoRightAlign."), U8("结合无文本与无右对齐。")));

                        Shadow::HotKey(T(U8("HK NoText+NoStateDisp"), U8("快捷键 无文本+无状态显示")), &hk2, &hk2_active, &hk2_mode, Shadow::ShadowHotkeyFlags_NoText | Shadow::ShadowHotkeyFlags_NoStateDisplay);
                        Shadow::HelpMarker(T(U8("Combines NoText and NoStateDisplay."), U8("结合无文本与无状态显示。")));

                        Shadow::HotKey(T(U8("HK NoRightAlign+NoStateDisp"), U8("快捷键 无右对齐+无状态显示")), &hk2, &hk2_active, &hk2_mode, Shadow::ShadowHotkeyFlags_NoRightAlign | Shadow::ShadowHotkeyFlags_NoStateDisplay);
                        Shadow::HelpMarker(T(U8("Combines NoRightAlign and NoStateDisplay."), U8("结合无右对齐与无状态显示。")));

                        Shadow::HotKey(T(U8("HK All Flags Combo"), U8("快捷键 组合所有标志")), &hk2, &hk2_active, &hk2_mode, Shadow::ShadowHotkeyFlags_NoText | Shadow::ShadowHotkeyFlags_NoRightAlign | Shadow::ShadowHotkeyFlags_NoStateDisplay);
                        Shadow::HelpMarker(T(U8("Combines NoText, NoRightAlign, and NoStateDisplay."), U8("结合无文本、无右对齐和无状态显示。")));
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(T(U8("Layout, Spacing & Stack Modifiers"), U8("布局、间距与堆栈修饰符")))) {
                        Shadow::Text(T(U8("Item 1"), U8("项目 1")));
                        Shadow::SameLine();
                        Shadow::Text(T(U8("Item 2 (Same Line)"), U8("项目 2 (同一行)")));
                        Shadow::HelpMarker(T(U8("Uses SameLine() without args to pack horizontally."), U8("使用无参数的 SameLine() 水平排列。")));

                        Shadow::Text(T(U8("Item 3"), U8("项目 3")));
                        Shadow::SameLine(150.f);
                        Shadow::Text(T(U8("Item 4 (SameLine offset)"), U8("项目 4 (SameLine 偏移)")));
                        Shadow::HelpMarker(T(U8("Uses SameLine(150.f) to force alignment to 150px."), U8("使用 SameLine(150.f) 强制对齐到 150 像素。")));

                        Shadow::Text(T(U8("Item 5"), U8("项目 5")));
                        Shadow::SameLine(0.f, 40.f);
                        Shadow::Text(T(U8("Item 6 (SameLine space)"), U8("项目 6 (SameLine 间距)")));
                        Shadow::HelpMarker(T(U8("Uses SameLine(0.f, 40.f) to force 40px spacing."), U8("使用 SameLine(0.f, 40.f) 强制设置 40 像素间距。")));

                        Shadow::Separator();
                        Shadow::HelpMarker(T(U8("Separator() line above."), U8("上方的分隔线 Separator()。")));

                        Shadow::Text(T(U8("Below is a 50x30 Dummy"), U8("下方是一个 50x30 的占位 (Dummy)")));
                        Shadow::Dummy({ 50.f, 30.f });
                        Shadow::Text(T(U8("After Dummy"), U8("占位之后")));
                        Shadow::HelpMarker(T(U8("Dummy inserts empty space manually."), U8("Dummy 用于手动插入空白区域。")));

                        Shadow::Spacing();
                        Shadow::HelpMarker(T(U8("Spacing() adds vertical padding between items."), U8("Spacing() 在项目之间添加垂直边距。")));

                        Shadow::PushTextWrapPos(150.f);
                        Shadow::Text(T(U8("This text is forcibly wrapped at 150 pixels due to PushTextWrapPos."), U8("这段文本因 PushTextWrapPos 被强制在 150 像素处换行。")));
                        Shadow::PopTextWrapPos();
                        Shadow::HelpMarker(T(U8("PushTextWrapPos / PopTextWrapPos Demonstration."), U8("PushTextWrapPos / PopTextWrapPos 演示。")));

                        Shadow::Vec2 cur = Shadow::g_Ctx.Cursor;
                        Shadow::PushClipRect(cur, { cur.x + 100.f, cur.y + Shadow::g_Ctx.ItemHeight });
                        Shadow::Text(T(U8("This long text will be physically clipped at 100px width."), U8("这段长文本将在 100 像素宽度处被物理裁剪。")));
                        Shadow::PopClipRect();
                        Shadow::HelpMarker(T(U8("PushClipRect / PopClipRect Demonstration."), U8("PushClipRect / PopClipRect 演示。")));

                        Shadow::PushFont(Shadow::g_Ctx.DefaultFont, 1.25f);
                        Shadow::Text(T(U8("Text with Font scale 1.25x"), U8("字体缩放 1.25 倍的文本")));
                        Shadow::PopFont();
                        Shadow::HelpMarker(T(U8("PushFont / PopFont Demonstration."), U8("PushFont / PopFont 演示。")));
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(T(U8("Disabled Status"), U8("禁用状态")))) {
                        static bool disable_all = true;
                        Shadow::Checkbox(T(U8("Disable the following group"), U8("禁用以下组")), &disable_all);

                        Shadow::BeginDisabled(disable_all);
                        Shadow::Button(T(U8("Disabled Button"), U8("禁用的按钮")));
                        static float dummy_f = 0.5f;
                        Shadow::Slider(T(U8("Disabled Slider"), U8("禁用的滑块")), &dummy_f, 0.f, 1.f);
                        Shadow::EndDisabled();

                        Shadow::HelpMarker(T(U8("Widgets bounded by BeginDisabled(bool) / EndDisabled()."), U8("由 BeginDisabled(bool) / EndDisabled() 边界控制的小部件。")));
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(T(U8("TreeNodes & Flags"), U8("树节点与标志")))) {
                        if (Shadow::TreeNode(T(U8("Default Open Node"), U8("默认展开节点")), Shadow::ShadowTreeNodeFlags_DefaultOpen)) {
                            Shadow::Text(T(U8("This is open by default."), U8("这是默认展开的。")));
                        }
                        Shadow::TreePop(); // 无条件Pop

                        if (Shadow::TreeNode(T(U8("Framed Node"), U8("带边框节点")), Shadow::ShadowTreeNodeFlags_Framed)) {
                            Shadow::Text(T(U8("This node has a background frame."), U8("该节点有一个背景边框。")));
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode(T(U8("FitText Framed Node"), U8("自适应文本边框节点")), Shadow::ShadowTreeNodeFlags_Framed | Shadow::ShadowTreeNodeFlags_FitText)) {
                            Shadow::Text(T(U8("The frame stops at the text end, ignoring right margin."), U8("边框在文本末尾停止，忽略右边距。")));
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode(T(U8("No Indent Node"), U8("无缩进节点")), Shadow::ShadowTreeNodeFlags_NoIndent)) {
                            Shadow::Text(T(U8("Children inside this node are not indented horizontally."), U8("该节点内部的子项不会在水平方向上缩进。")));
                        }
                        Shadow::TreePop();

                        // TreeNode Flags 组合演示
                        if (Shadow::TreeNode(T(U8("All Flags Combo Node"), U8("所有标志组合节点")), Shadow::ShadowTreeNodeFlags_Framed | Shadow::ShadowTreeNodeFlags_DefaultOpen | Shadow::ShadowTreeNodeFlags_FitText | Shadow::ShadowTreeNodeFlags_NoIndent)) {
                            Shadow::Text(T(U8("This node combines Framed, DefaultOpen, FitText, and NoIndent."), U8("该节点结合了边框、默认展开、自适应文本和无缩进。")));
                        }
                        Shadow::TreePop();
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(T(U8("Hovered Flags Demo"), U8("悬停标志演示")))) {
                        Shadow::Button(T(U8("Hover Me (Normal)"), U8("悬停我 (正常)")));
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_None)) {
                            Shadow::BeginTooltip(); Shadow::Text(T(U8("Normal Hover"), U8("正常悬停"))); Shadow::EndTooltip();
                        }

                        Shadow::Button(T(U8("Hover Me (Delay Short)"), U8("悬停我 (短延迟)")));
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_DelayShort)) {
                            Shadow::BeginTooltip(); Shadow::Text(T(U8("Short Delay Hover Tooltip"), U8("短延迟悬停提示"))); Shadow::EndTooltip();
                        }

                        Shadow::Button(T(U8("Hover Me (Delay Normal)"), U8("悬停我 (正常延迟)")));
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_DelayNormal)) {
                            Shadow::BeginTooltip(); Shadow::Text(T(U8("Normal Delay Hover Tooltip"), U8("正常延迟悬停提示"))); Shadow::EndTooltip();
                        }

                        Shadow::Button(T(U8("Hover Me (Stationary)"), U8("悬停我 (静止)")));
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_Stationary)) {
                            Shadow::BeginTooltip(); Shadow::Text(T(U8("Stationary Hover Tooltip"), U8("静止悬停提示"))); Shadow::EndTooltip();
                        }

                        // Hover Flags 组合演示
                        Shadow::Button(T(U8("Hover Me (DelayNormal + Stationary)"), U8("悬停我 (正常延迟 + 静止)")));
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_DelayNormal | Shadow::ShadowHoveredFlags_Stationary)) {
                            Shadow::BeginTooltip(); Shadow::Text(T(U8("DelayNormal + Stationary Hover Tooltip"), U8("正常延迟 + 静止悬停提示"))); Shadow::EndTooltip();
                        }

                        Shadow::BeginDisabled(true);
                        Shadow::Button(T(U8("Hover Disabled (DelayShort + Allow)"), U8("悬停禁用 (短延迟 + 允许)")));
                        if (Shadow::IsItemHovered(Shadow::ShadowHoveredFlags_DelayShort | Shadow::ShadowHoveredFlags_AllowWhenDisabled)) {
                            Shadow::BeginTooltip(); Shadow::Text(T(U8("DelayShort + AllowWhenDisabled Hover Tooltip"), U8("短延迟 + 允许禁用状态的悬停提示"))); Shadow::EndTooltip();
                        }
                        Shadow::EndDisabled();
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(T(U8("Window Flags Editor"), U8("窗口标志编辑器")))) {
                        static bool f_NoResize = false, f_NoMove = false, f_NoScrollbar = false, f_NoTitleBar = false, f_NoMouseInputs = false;

                        if (Shadow::Selectable(T(U8("NoResize"), U8("NoResize (禁止调整大小)")), &f_NoResize)) { if (f_NoResize) win_flags |= Shadow::ShadowWindowFlags_NoResize; else win_flags &= ~Shadow::ShadowWindowFlags_NoResize; }
                        if (Shadow::Selectable(T(U8("NoMove"), U8("NoMove (禁止移动)")), &f_NoMove)) { if (f_NoMove) win_flags |= Shadow::ShadowWindowFlags_NoMove; else win_flags &= ~Shadow::ShadowWindowFlags_NoMove; }
                        if (Shadow::Selectable(T(U8("NoScrollbar"), U8("NoScrollbar (无滚动条)")), &f_NoScrollbar)) { if (f_NoScrollbar) win_flags |= Shadow::ShadowWindowFlags_NoScrollbar; else win_flags &= ~Shadow::ShadowWindowFlags_NoScrollbar; }
                        if (Shadow::Selectable(T(U8("NoTitleBar"), U8("NoTitleBar (无标题栏)")), &f_NoTitleBar)) { if (f_NoTitleBar) win_flags |= Shadow::ShadowWindowFlags_NoTitleBar; else win_flags &= ~Shadow::ShadowWindowFlags_NoTitleBar; }
                        if (Shadow::Selectable(T(U8("NoMouseInputs"), U8("NoMouseInputs (无鼠标输入)")), &f_NoMouseInputs)) { if (f_NoMouseInputs) win_flags |= Shadow::ShadowWindowFlags_NoMouseInputs; else win_flags &= ~Shadow::ShadowWindowFlags_NoMouseInputs; }

                        static int align_idx = 0;
                        std::vector<std::string> aligns = g_UseChinese ?
                            std::vector<std::string>{ U8("左对齐"), U8("居中"), U8("右对齐") } :
                            std::vector<std::string>{ U8("Left"), U8("Center"), U8("Right") };

                        Shadow::Combo(T(U8("TextAlign (Applies to Title)"), U8("TextAlign (适用于标题)")), &align_idx, aligns);
                        win_flags &= ~(Shadow::ShadowWindowFlags_TextAlignCenter | Shadow::ShadowWindowFlags_TextAlignRight);
                        if (align_idx == 1) win_flags |= Shadow::ShadowWindowFlags_TextAlignCenter;
                        if (align_idx == 2) win_flags |= Shadow::ShadowWindowFlags_TextAlignRight;
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(T(U8("TabBar Flags Editor"), U8("标签栏标志编辑器")))) {
                        static bool t_Reorderable = true, t_FittingPolicyScroll = true, t_NoScrollbar = false;

                        if (Shadow::Selectable(T(U8("Reorderable"), U8("Reorderable (可重新排序)")), &t_Reorderable)) { if (t_Reorderable) tab_flags |= Shadow::ShadowTabBarFlags_Reorderable; else tab_flags &= ~Shadow::ShadowTabBarFlags_Reorderable; }
                        if (Shadow::Selectable(T(U8("FittingPolicyScroll"), U8("FittingPolicyScroll (适应滚动策略)")), &t_FittingPolicyScroll)) { if (t_FittingPolicyScroll) tab_flags |= Shadow::ShadowTabBarFlags_FittingPolicyScroll; else tab_flags &= ~Shadow::ShadowTabBarFlags_FittingPolicyScroll; }
                        if (Shadow::Selectable(T(U8("NoScrollbar"), U8("NoScrollbar (无滚动条)")), &t_NoScrollbar)) { if (t_NoScrollbar) tab_flags |= Shadow::ShadowTabBarFlags_NoScrollbar; else tab_flags &= ~Shadow::ShadowTabBarFlags_NoScrollbar; }
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(T(U8("Style Menu Colors Editor"), U8("样式菜单颜色编辑器")))) {
                        const char* bg_names_en[] = { U8("WindowBg"), U8("TitleBarBg"), U8("FrameBg"), U8("FrameBgHovered"), U8("PopupBg"), U8("Tab"), U8("TabHovered"), U8("TabActive"), U8("SwitchBg"), U8("SwitchBgHovered"), U8("SwitchBgActive"), U8("SwitchBgActiveHovered"), U8("DropdownActive") };
                        const char* bg_names_zh[] = { U8("窗口背景 (WindowBg)"), U8("标题栏背景 (TitleBarBg)"), U8("框架背景 (FrameBg)"), U8("框架背景悬停 (FrameBgHovered)"), U8("弹窗背景 (PopupBg)"), U8("标签 (Tab)"), U8("标签悬停 (TabHovered)"), U8("标签激活 (TabActive)"), U8("开关背景 (SwitchBg)"), U8("开关背景悬停 (SwitchBgHovered)"), U8("开关背景激活 (SwitchBgActive)"), U8("开关背景激活悬停 (SwitchBgActiveHovered)"), U8("下拉框激活 (DropdownActive)") };
                        int bg_ids[] = { Shadow::GuiCol_WindowBg, Shadow::GuiCol_TitleBarBg, Shadow::GuiCol_FrameBg, Shadow::GuiCol_FrameBgHovered, Shadow::GuiCol_PopupBg, Shadow::GuiCol_Tab, Shadow::GuiCol_TabHovered, Shadow::GuiCol_TabActive, Shadow::GuiCol_SwitchBg, Shadow::GuiCol_SwitchBgHovered, Shadow::GuiCol_SwitchBgActive, Shadow::GuiCol_SwitchBgActiveHovered, Shadow::GuiCol_DropdownActive };

                        const char* text_names_en[] = { U8("Text"), U8("TextDisabled"), U8("TextHighlight"), U8("ErrorText"), U8("TextShadow"), U8("TextOutline") };
                        const char* text_names_zh[] = { U8("文本 (Text)"), U8("禁用文本 (TextDisabled)"), U8("高亮文本 (TextHighlight)"), U8("错误文本 (ErrorText)"), U8("文本阴影 (TextShadow)"), U8("文本描边 (TextOutline)") };
                        int text_ids[] = { Shadow::GuiCol_Text, Shadow::GuiCol_TextDisabled, Shadow::GuiCol_TextHighlight, Shadow::GuiCol_ErrorText, Shadow::GuiCol_TextShadow, Shadow::GuiCol_TextOutline };

                        const char* btn_names_en[] = { U8("Button"), U8("ButtonHovered"), U8("SliderGrab"), U8("SliderKnob"), U8("CheckMark"), U8("Separator"), U8("ResizeGrip"), U8("ResizeGripHovered"), U8("ResizeGripActive"), U8("ActiveIndicator"), U8("InactiveIndicator"), U8("Border"), U8("PopupBorder"), U8("ControlDisabled"), U8("SwitchKnob") };
                        const char* btn_names_zh[] = { U8("按钮 (Button)"), U8("按钮悬停 (ButtonHovered)"), U8("滑块抓取 (SliderGrab)"), U8("滑块旋钮 (SliderKnob)"), U8("复选标记 (CheckMark)"), U8("分隔符 (Separator)"), U8("调整手柄 (ResizeGrip)"), U8("调整手柄悬停 (ResizeGripHovered)"), U8("调整手柄激活 (ResizeGripActive)"), U8("激活指示器 (ActiveIndicator)"), U8("未激活指示器 (InactiveIndicator)"), U8("边框 (Border)"), U8("弹窗边框 (PopupBorder)"), U8("控件禁用 (ControlDisabled)"), U8("开关旋钮 (SwitchKnob)") };
                        int btn_ids[] = { Shadow::GuiCol_Button, Shadow::GuiCol_ButtonHovered, Shadow::GuiCol_SliderGrab, Shadow::GuiCol_SliderKnob, Shadow::GuiCol_CheckMark, Shadow::GuiCol_Separator, Shadow::GuiCol_ResizeGrip, Shadow::GuiCol_ResizeGripHovered, Shadow::GuiCol_ResizeGripActive, Shadow::GuiCol_ActiveIndicator, Shadow::GuiCol_InactiveIndicator, Shadow::GuiCol_Border, Shadow::GuiCol_PopupBorder, Shadow::GuiCol_ControlDisabled, Shadow::GuiCol_SwitchKnob };

                        const char* cp_names_en[] = { U8("ColorPickerDark"), U8("ColorPickerLight"), U8("CheckerboardLight"), U8("CheckerboardDark"), U8("ColorPickerShadow") };
                        const char* cp_names_zh[] = { U8("拾色器暗部 (ColorPickerDark)"), U8("拾色器亮部 (ColorPickerLight)"), U8("棋盘格亮部 (CheckerboardLight)"), U8("棋盘格暗部 (CheckerboardDark)"), U8("拾色器阴影 (ColorPickerShadow)") };
                        int cp_ids[] = { Shadow::GuiCol_ColorPickerDark, Shadow::GuiCol_ColorPickerLight, Shadow::GuiCol_CheckerboardLight, Shadow::GuiCol_CheckerboardDark, Shadow::GuiCol_ColorPickerShadow };

                        if (Shadow::TreeNode(T(U8("Backgrounds"), U8("背景")))) {
                            for (size_t i = 0; i < sizeof(bg_ids) / sizeof(bg_ids[0]); ++i) {
                                Shadow::Color& c = Shadow::g_Ctx.Style.Colors[bg_ids[i]];
                                Shadow::ColorPicker(T(bg_names_en[i], bg_names_zh[i]), &c.r, &c.g, &c.b, &c.a);
                            }
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode(T(U8("Texts"), U8("文本")))) {
                            for (size_t i = 0; i < sizeof(text_ids) / sizeof(text_ids[0]); ++i) {
                                Shadow::Color& c = Shadow::g_Ctx.Style.Colors[text_ids[i]];
                                Shadow::ColorPicker(T(text_names_en[i], text_names_zh[i]), &c.r, &c.g, &c.b, &c.a);
                            }
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode(T(U8("Controls & Borders"), U8("控件与边框")))) {
                            for (size_t i = 0; i < sizeof(btn_ids) / sizeof(btn_ids[0]); ++i) {
                                Shadow::Color& c = Shadow::g_Ctx.Style.Colors[btn_ids[i]];
                                Shadow::ColorPicker(T(btn_names_en[i], btn_names_zh[i]), &c.r, &c.g, &c.b, &c.a);
                            }
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode(T(U8("Color Picker Specific"), U8("拾色器专属")))) {
                            for (size_t i = 0; i < sizeof(cp_ids) / sizeof(cp_ids[0]); ++i) {
                                Shadow::Color& c = Shadow::g_Ctx.Style.Colors[cp_ids[i]];
                                Shadow::ColorPicker(T(cp_names_en[i], cp_names_zh[i]), &c.r, &c.g, &c.b, &c.a);
                            }
                        }
                        Shadow::TreePop();
                    }
                    Shadow::TreePop();

                    if (Shadow::TreeNode(T(U8("ListBox Demos"), U8("列表框演示")))) {

                        if (Shadow::TreeNode(T(U8("ListBox with Selectables"), U8("使用可选项的列表框")))) {
                            static int selectedItem = 0;
                            std::vector<std::string> items = g_UseChinese ?
                                std::vector<std::string>{ U8("苹果"), U8("香蕉"), U8("樱桃"), U8("椰枣"), U8("接骨木莓"), U8("无花果"), U8("葡萄"), U8("蜜瓜"), U8("猕猴桃"), U8("柠檬") } :
                                std::vector<std::string>{ U8("Apple"), U8("Banana"), U8("Cherry"), U8("Date"), U8("Elderberry"), U8("Fig"), U8("Grape"), U8("Honeydew"), U8("Kiwi"), U8("Lemon") };

                            if (Shadow::BeginListBox(T(U8("FruitListBox"), U8("水果列表框")), { 200.f, 150.f })) {
                                for (int i = 0; i < static_cast<int>(items.size()); i++) {
                                    bool isSelected = (selectedItem == i);
                                    if (Shadow::Selectable(items[i], &isSelected)) {
                                        selectedItem = i;
                                    }
                                }
                            }
                            Shadow::EndListBox();
                            Shadow::HelpMarker(T(U8("ListBox demonstrating internal Selectable usage for item selection."), U8("演示内部可选项(Selectable)用法以选择项目的列表框。")));
                            Shadow::Text(g_UseChinese
                                ? (U8("已选择: ") + items[selectedItem])
                                : (U8("Selected: ") + items[selectedItem]));
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode(T(U8("ListBox with Color Texts"), U8("带有彩色文本的列表框")))) {
                            struct ColorEntry {
                                std::string name_en;
                                std::string name_zh;
                                Shadow::Color color;
                            };

                            std::vector<ColorEntry> colors = {
                                { U8("Red"),    U8("红色"),    { 1.0f, 0.0f, 0.0f, 1.0f } },
                                { U8("Green"),  U8("绿色"),    { 0.0f, 1.0f, 0.0f, 1.0f } },
                                { U8("Blue"),   U8("蓝色"),    { 0.0f, 0.0f, 1.0f, 1.0f } },
                                { U8("Yellow"), U8("黄色"),    { 1.0f, 1.0f, 0.0f, 1.0f } },
                                { U8("Cyan"),   U8("青色"),    { 0.0f, 1.0f, 1.0f, 1.0f } },
                                { U8("Magenta"),U8("洋红色"),  { 1.0f, 0.0f, 1.0f, 1.0f } },
                                { U8("Orange"), U8("橙色"),    { 1.0f, 0.5f, 0.0f, 1.0f } }
                            };

                            if (Shadow::BeginListBox(T(U8("ColorTextListBox"), U8("彩色文本列表框")), { 200.f, 150.f })) {
                                for (const auto& entry : colors) {
                                    Shadow::TextColored(entry.color, g_UseChinese ? entry.name_zh : entry.name_en);
                                }
                            }
                            Shadow::EndListBox();
                            Shadow::HelpMarker(T(U8("ListBox demonstrating TextColored items with various colors."), U8("演示使用 TextColored 显示各种颜色项目的列表框。")));
                        }
                        Shadow::TreePop();

                        if (Shadow::TreeNode(T(U8("ListBox with Color Pickers"), U8("带有拾色器的列表框")))) {
                            static std::vector<Shadow::Color> customColors = {
                                { 1.0f, 0.0f, 0.0f, 1.0f },  // Red
                                { 0.0f, 1.0f, 0.0f, 1.0f },  // Green
                                { 0.0f, 0.0f, 1.0f, 1.0f },  // Blue
                                { 1.0f, 1.0f, 0.0f, 1.0f },  // Yellow
                                { 0.0f, 1.0f, 1.0f, 1.0f }   // Cyan
                            };

                            if (Shadow::BeginListBox(T(U8("ColorPickerListBox"), U8("拾色器列表框")), { 300.f, 180.f })) {
                                for (int i = 0; i < static_cast<int>(customColors.size()); i++) {
                                    Shadow::ColorPicker(
                                        g_UseChinese
                                        ? (U8("颜色 ") + std::to_string(i + 1) + U8("##cp"))
                                        : (U8("Color ") + std::to_string(i + 1) + U8("##cp")),
                                        & customColors[i].r,
                                        & customColors[i].g,
                                        & customColors[i].b,
                                        & customColors[i].a
                                    );
                                }
                            }
                            Shadow::EndListBox();
                            Shadow::HelpMarker(T(U8("ListBox demonstrating embedded ColorPicker controls."), U8("演示嵌入 ColorPicker 控件的列表框。")));
                        }
                        Shadow::TreePop();

                    }
                    Shadow::TreePop();

                }
                Shadow::EndTabItem(); // 无条件EndTabItem

                // ----------------------------------------------------------------
                // 第二页面: 高级自定义组合控件 (一共19种)
                // ----------------------------------------------------------------
                if (Shadow::BeginTabItem(T(U8("Advanced Custom Controls"), U8("高级自定义组合控件")))) {
                    Advanced::SectionHeader(T(U8("Advanced Custom Composites (Total 19+)"), U8("高级自定义组合控件 (共计 19+ 种)")));

                    Advanced::PropertyRow(T(U8("Player Speed"), U8("玩家速度")), T(U8("420.0 (Readonly Property Demo)"), U8("420.0 (只读属性演示)")));
                    Shadow::HelpMarker(T(U8("This is Advanced Control 1: PropertyRow"), U8("这是高级控件 1：PropertyRow (属性行)")));

                    static std::string pwd = U8("my_secret");
                    static bool show_pwd = false;
                    Advanced::PasswordToggle(T(U8("API Key"), U8("API 密钥")), pwd, show_pwd);
                    Shadow::HelpMarker(T(U8("This is Advanced Control 2: PasswordToggle"), U8("这是高级控件 2：PasswordToggle (密码切换框)")));

                    static std::string search_query = U8("");
                    if (Advanced::SearchBox(T(U8("Database"), U8("数据库")), search_query)) {
                        // triggered
                    }
                    Shadow::HelpMarker(T(U8("This is Advanced Control 3: SearchBox"), U8("这是高级控件 3：SearchBox (搜索框)")));

                    static float prog = 0.65f;
                    Shadow::Slider(T(U8("Adjust Progress"), U8("调整进度")), &prog, 0.f, 1.f);
                    Advanced::ProgressBar(prog, T(U8("Loading Assets..."), U8("正在加载资源...")));
                    Shadow::HelpMarker(T(U8("This is Advanced Control 4: ProgressBar"), U8("这是高级控件 4：ProgressBar (进度条)")));

                    Shadow::Text(T(U8("Theme Colors:"), U8("主题颜色:")));
                    Shadow::SameLine(); Advanced::ColorButton(U8("cb1"), { 0.8f,0.2f,0.2f,1.f });
                    Shadow::SameLine(); Advanced::ColorButton(U8("cb2"), { 0.2f,0.8f,0.2f,1.f });
                    Shadow::SameLine(); Advanced::ColorButton(U8("cb3"), { 0.2f,0.2f,0.8f,1.f });
                    Shadow::HelpMarker(T(U8("This is Advanced Control 5: ColorButton"), U8("这是高级控件 5：ColorButton (颜色按钮)")));

                    static int list_idx = 0;
                    std::vector<std::string> list_items = g_UseChinese ?
                        std::vector<std::string>{ U8("突击步枪"), U8("狙击枪"), U8("霰弹枪") } :
                        std::vector<std::string>{ U8("Assault Rifle"), U8("Sniper"), U8("Shotgun") };
                    Advanced::ListSelector(T(U8("Weapon Preset"), U8("武器预设")), list_items, list_idx);
                    Shadow::HelpMarker(T(U8("This is Advanced Control 6: ListSelector"), U8("这是高级控件 6：ListSelector (列表选择器)")));

                    Shadow::Text(T(U8("Status Badges: "), U8("状态徽章: ")));
                    Shadow::SameLine(); Advanced::Badge(T(U8("NEW"), U8("全新")), { 0.8f,0.2f,0.2f,1.f }, { 1.f,1.f,1.f,1.f });
                    Shadow::SameLine(); Advanced::Badge(T(U8("PRO"), U8("专业")), { 0.8f,0.6f,0.1f,1.f }, { 0.1f,0.1f,0.1f,1.f });
                    Shadow::HelpMarker(T(U8("This is Advanced Control 7: Badge"), U8("这是高级控件 7：Badge (徽章)")));

                    static int step_val = 50;
                    Advanced::StepperInt(T(U8("Volume Step"), U8("音量步进")), step_val, 5, 0, 100);
                    Shadow::HelpMarker(T(U8("This is Advanced Control 8: StepperInt"), U8("这是高级控件 8：StepperInt (整数步进器)")));

                    static bool t_state = false;
                    Advanced::ToggleButton(T(U8("God Mode Status"), U8("无敌模式状态")), t_state);
                    Shadow::HelpMarker(T(U8("This is Advanced Control 9: ToggleButton"), U8("这是高级控件 9：ToggleButton (状态切换按钮)")));

                    static float slider_val = 0.75f;
                    Advanced::LabeledSlider(T(U8("Intensity Filter"), U8("强度过滤")), &slider_val, 0.f, 1.f);
                    Shadow::HelpMarker(T(U8("This is Advanced Control 10: LabeledSlider"), U8("这是高级控件 10：LabeledSlider (标签滑块)")));

                    Advanced::SectionHeader(T(U8("New Additions: Utility & Layout"), U8("新增内容：实用工具与布局")));
                    Shadow::HelpMarker(T(U8("This is Advanced Control 11: SectionHeader"), U8("这是高级控件 11：SectionHeader (分区标题)")));

                    // 12. 范围滑块组合
                    static float min_val = 10.f, max_val = 90.f;
                    Advanced::RangeSliderFloat(T(U8("Spawn Range"), U8("生成范围")), min_val, max_val, 0.f, 100.f);
                    Shadow::HelpMarker(T(U8("This is Advanced Control 12: RangeSliderFloat"), U8("这是高级控件 12：RangeSliderFloat (范围滑块)")));

                    // 13. 分页导航
                    static int page = 1;
                    Advanced::Pagination(page, 10);
                    Shadow::HelpMarker(T(U8("This is Advanced Control 13: Pagination"), U8("这是高级控件 13：Pagination (分页导航)")));

                    // 14. 标签输入与管理
                    static std::vector<std::string> my_tags = { U8("Player"), U8("Enemy") }; // 动态用户修改项，仅作初始演示值
                    static std::string tag_input = U8("");
                    Advanced::TagsInput(T(U8("Entity Tags"), U8("实体标签")), my_tags, tag_input);

                    Advanced::SectionHeader(T(U8("End of Advanced Demo"), U8("高级演示结束")));
                }
                Shadow::EndTabItem(); // 无条件EndTabItem

            }
            Shadow::EndTabBar(); // 无条件EndTabBar

        }
        Shadow::End(); // 无条件End
    }
} // namespace Shadow