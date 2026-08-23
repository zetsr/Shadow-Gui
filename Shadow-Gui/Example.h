#pragma once

#ifndef U
#define U(str) reinterpret_cast<const char*>(u8##str)
#endif

#include <string>
#include <vector>
#include <format>
#include <algorithm>

#include "external/CppSDK/SDK.hpp"
#include "external/Shadow-Gui/include/Shadow.h"

namespace Example {
    inline void StyleColorsExample() {
        using namespace Shadow;
        auto& colors = g_Ctx.Style.Colors;

        colors[GuiCol_WindowBg] = { 0.003f, 0.003f, 0.004f, 1.000f };
        colors[GuiCol_PopupBg] = { 0.004f, 0.004f, 0.005f, 0.980f };
        colors[GuiCol_TitleBarBg] = { 0.002f, 0.002f, 0.003f, 1.000f };

        colors[GuiCol_Text] = { 0.850f, 0.850f, 0.880f, 1.000f };
        colors[GuiCol_TextHighlight] = { 1.000f, 1.000f, 1.000f, 1.000f };
        colors[GuiCol_TextDisabled] = { 0.140f, 0.140f, 0.160f, 1.000f };

        colors[GuiCol_FrameBg] = { 0.006f, 0.006f, 0.008f, 1.000f };
        colors[GuiCol_FrameBgHovered] = { 0.010f, 0.010f, 0.013f, 1.000f };

        colors[GuiCol_Button] = { 0.010f, 0.010f, 0.013f, 1.000f };
        colors[GuiCol_ButtonHovered] = { 0.020f, 0.020f, 0.025f, 1.000f };

        colors[GuiCol_Tab] = { 0.000f, 0.000f, 0.000f, 0.000f };
        colors[GuiCol_TabHovered] = { 0.008f, 0.008f, 0.010f, 1.000f };
        colors[GuiCol_TabActive] = { 0.015f, 0.015f, 0.018f, 1.000f };

        colors[GuiCol_SliderGrab] = { 0.008f, 0.160f, 0.920f, 1.000f };
        colors[GuiCol_SliderKnob] = { 1.000f, 1.000f, 1.000f, 1.000f };

        colors[GuiCol_CheckMark] = { 0.008f, 0.160f, 0.920f, 1.000f };
        colors[GuiCol_ActiveIndicator] = { 0.008f, 0.160f, 0.920f, 1.000f };
        colors[GuiCol_InactiveIndicator] = { 0.030f, 0.030f, 0.035f, 1.000f };

        colors[GuiCol_Border] = { 0.015f, 0.015f, 0.018f, 0.600f };
        colors[GuiCol_PopupBorder] = { 0.020f, 0.020f, 0.025f, 0.800f };
        colors[GuiCol_Separator] = { 0.012f, 0.012f, 0.015f, 1.000f };

        colors[GuiCol_ResizeGrip] = { 0.010f, 0.010f, 0.013f, 1.000f };
        colors[GuiCol_ResizeGripActive] = { 0.008f, 0.160f, 0.920f, 1.000f };
        colors[GuiCol_ResizeGripHovered] = { 0.015f, 0.220f, 0.980f, 1.000f };

        colors[GuiCol_ErrorText] = { 0.900f, 0.050f, 0.050f, 1.000f };
        colors[GuiCol_TextShadow] = { 0.000f, 0.000f, 0.000f, 1.000f };
        colors[GuiCol_TextOutline] = { 0.000f, 0.000f, 0.000f, 1.000f };
        colors[GuiCol_ColorPickerDark] = { 0.000f, 0.000f, 0.000f, 1.000f };
        colors[GuiCol_ColorPickerLight] = { 1.000f, 1.000f, 1.000f, 1.000f };

        colors[GuiCol_CheckerboardLight] = { 1.000f, 1.000f, 1.000f, 1.000f };
        colors[GuiCol_CheckerboardDark] = { 0.400f, 0.400f, 0.400f, 1.000f };
        colors[GuiCol_ColorPickerShadow] = { 0.000f, 0.000f, 0.000f, 1.000f };

        colors[GuiCol_ControlDisabled] = { 0.006f, 0.006f, 0.008f, 0.500f };

        colors[GuiCol_SwitchBg] = { 0.030f, 0.030f, 0.035f, 1.000f };
        colors[GuiCol_SwitchBgHovered] = { 0.045f, 0.045f, 0.052f, 1.000f };
        colors[GuiCol_SwitchBgActive] = { 0.008f, 0.160f, 0.920f, 1.000f };
        colors[GuiCol_SwitchBgActiveHovered] = { 0.015f, 0.220f, 0.980f, 1.000f };
        colors[GuiCol_SwitchKnob] = { 0.900f, 0.900f, 0.950f, 1.000f };

        colors[GuiCol_DropdownActive] = { 0.015f, 0.015f, 0.018f, 1.000f };
    }

    struct TabAnimationState
    {
        int CurrentTab = 0;
        int PreviousTab = 0;
        int TargetTab = 0;
        float TransitionProgress = 1.0f;
        float StartOffset = 0.0f;
        float TargetOffset = 0.0f;
        float CurrentOffset = 0.0f;
        bool IsAnimating = false;
    };

    inline static TabAnimationState g_TabAnim;

    inline float EaseOutCubic(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return 1.0f - std::pow(1.0f - t, 3.0f);
    }

    inline float Lerp(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

    struct ColumnLayout {
        float LeftX = 0.0f;
        float RightX = 0.0f;
        float LeftY = 0.0f;
        float RightY = 0.0f;
        float Width = 0.0f;
        float StartY = 0.0f;
        bool IsLeft = true;
    };
    inline ColumnLayout g_ColLayout;
    inline float g_OriginalWindowPaddingX = 0.0f;

    inline void BeginColumns(float startX, float startY, float totalWidth, float spacing) {
        g_ColLayout.Width = (totalWidth - spacing) / 2.0f;
        g_ColLayout.LeftX = startX;
        g_ColLayout.RightX = startX + g_ColLayout.Width + spacing;
        g_ColLayout.LeftY = startY;
        g_ColLayout.RightY = startY;
        g_ColLayout.StartY = startY;
        g_ColLayout.IsLeft = true;

        g_OriginalWindowPaddingX = Shadow::GetStyle().WindowPadding.x;
    }

    inline void SetColumn(bool isLeft) {
        if (g_ColLayout.IsLeft) g_ColLayout.LeftY = Shadow::g_Ctx.Cursor.y;
        else g_ColLayout.RightY = Shadow::g_Ctx.Cursor.y;

        g_ColLayout.IsLeft = isLeft;
        float targetX = isLeft ? g_ColLayout.LeftX : g_ColLayout.RightX;

        Shadow::GetStyle().WindowPadding.x = targetX - Shadow::g_Ctx.WindowPos.x;
        Shadow::g_Ctx.Cursor = { targetX, isLeft ? g_ColLayout.LeftY : g_ColLayout.RightY };
    }

    inline void EndColumns() {
        if (g_ColLayout.IsLeft) g_ColLayout.LeftY = Shadow::g_Ctx.Cursor.y;
        else g_ColLayout.RightY = Shadow::g_Ctx.Cursor.y;

        Shadow::GetStyle().WindowPadding.x = g_OriginalWindowPaddingX;
        Shadow::g_Ctx.Cursor.x = Shadow::g_Ctx.WindowPos.x + g_OriginalWindowPaddingX;
        Shadow::g_Ctx.Cursor.y = std::max(g_ColLayout.LeftY, g_ColLayout.RightY) + 10.0f;
    }

    inline void BeginPanel(const std::string& title) {
        Shadow::g_Ctx.Cursor.y += 4.0f;
        if (!title.empty()) {
            Shadow::TextDisabled(title);
            Shadow::g_Ctx.Cursor.y += 4.0f;
        }
    }

    inline void EndPanel() {
        Shadow::g_Ctx.Cursor.y += 12.0f;
    }

    namespace Menu {
        inline bool g_JoinNext = false;
        inline size_t g_RowCmdIndex = 0;
        inline Shadow::Vec2 g_RowStartPos;
        inline float g_StoredPaddingX = 0.0f;

        // 提取带 ## 隐藏符字符串的显示名称
        inline std::string GetDisplayName(const std::string& label) {
            size_t pos = label.find("##");
            return pos != std::string::npos ? label.substr(0, pos) : label;
        }

        // 调用此函数可以让下一个控件的背景完美衔接在上一个控件底部，形成连体块
        inline void JoinNext() {
            g_JoinNext = true;
        }

        inline void DrawRowBg(Shadow::Vec2 pos, float width, float height, size_t cmdIndex) {
            Shadow::ShadowDrawCmd bgCmd;
            bgCmd.type = Shadow::ShadowDrawCmdType::RectFilled;
            bgCmd.pos = pos;
            bgCmd.size = { width, height };
            bgCmd.color = Shadow::GetStyle().Colors[Shadow::GuiCol_FrameBgHovered]; // 行级卡片底色
            bgCmd.thickness = 1.0f;
            bgCmd.clippingEnabled = Shadow::g_Ctx.ClippingEnabled;
            bgCmd.clipMin = Shadow::g_Ctx.ClipMin;
            bgCmd.clipMax = Shadow::g_Ctx.ClipMax;

            Shadow::GetWindowDrawList()->CmdBuffer.insert(Shadow::GetWindowDrawList()->CmdBuffer.begin() + cmdIndex, bgCmd);
        }

        inline void BeginRow() {
            if (g_JoinNext) {
                // 抵消上一个控件产生的间距，使当前块与上一个块的底部完美重合
                Shadow::g_Ctx.Cursor.y -= Shadow::GetStyle().ItemSpacing.y;
                g_JoinNext = false;
            }
            g_RowCmdIndex = Shadow::GetWindowDrawList()->CmdBuffer.size();
            g_RowStartPos = Shadow::g_Ctx.Cursor;

            g_StoredPaddingX = Shadow::GetStyle().WindowPadding.x;

            // 为 Row 内部的内容增加内边距
            Shadow::GetStyle().WindowPadding.x += 12.0f;
            Shadow::g_Ctx.Cursor.x = Shadow::g_Ctx.WindowPos.x + Shadow::GetStyle().WindowPadding.x;
            Shadow::g_Ctx.Cursor.y += 8.0f; // 顶层内部边距
        }

        inline void EndRow() {
            Shadow::g_Ctx.Cursor.y -= Shadow::GetStyle().ItemSpacing.y;
            Shadow::g_Ctx.Cursor.y += 8.0f; // 底部内部边距

            float width = g_ColLayout.Width;
            float height = Shadow::g_Ctx.Cursor.y - g_RowStartPos.y;
            DrawRowBg(g_RowStartPos, width, height, g_RowCmdIndex);

            Shadow::GetStyle().WindowPadding.x = g_StoredPaddingX;
            Shadow::g_Ctx.Cursor.x = Shadow::g_Ctx.WindowPos.x + Shadow::GetStyle().WindowPadding.x;
            Shadow::g_Ctx.Cursor.y += Shadow::GetStyle().ItemSpacing.y;
        }

        // ======================= 控件封装 =======================

        inline bool Switch(const char* label, bool* v) {
            BeginRow();
            Shadow::Vec2 startPos = Shadow::g_Ctx.Cursor;

            Shadow::PushTextOutline();
            Shadow::GetWindowDrawList()->AddText({ startPos.x, startPos.y + Shadow::GetStyle().FramePadding.y }, Shadow::GetStyle().Colors[Shadow::GuiCol_Text], GetDisplayName(label));
            Shadow::PopTextOutline();

            float switchWidth = (Shadow::g_Ctx.ItemHeight - 4.0f) * 2.0f + 4.0f;

            // 完美对齐右侧边缘 (距离右侧内边距 12.0f) 
            // 注意: 减去 10.0f 以抵消 Shadow::Switch 在渲染时强行附带的无文本间距补偿
            Shadow::g_Ctx.Cursor.x = g_RowStartPos.x + g_ColLayout.Width - switchWidth - 12.0f - 10.0f;

            std::string hidden = std::string("##") + label;
            bool ret = Shadow::Switch(hidden.c_str(), v);

            EndRow();
            return ret;
        }

        inline bool Checkbox(const char* label, bool* v) {
            BeginRow();
            Shadow::Vec2 startPos = Shadow::g_Ctx.Cursor;

            Shadow::GetWindowDrawList()->AddText({ startPos.x, startPos.y + Shadow::GetStyle().FramePadding.y }, Shadow::GetStyle().Colors[Shadow::GuiCol_Text], GetDisplayName(label));

            float boxWidth = Shadow::g_Ctx.ItemHeight;
            Shadow::g_Ctx.Cursor.x = g_RowStartPos.x + g_ColLayout.Width - boxWidth - 12.0f;

            std::string hidden = std::string("##") + label;
            bool ret = Shadow::Checkbox(hidden.c_str(), v);

            EndRow();
            return ret;
        }

        inline void Slider(const char* label, float* v, float min, float max, float step, Shadow::ShadowSliderFlags flags = Shadow::ShadowSliderFlags_None) {
            BeginRow();
            Shadow::Vec2 startPos = Shadow::g_Ctx.Cursor;

            Shadow::GetWindowDrawList()->AddText({ startPos.x, startPos.y + Shadow::GetStyle().FramePadding.y }, Shadow::GetStyle().Colors[Shadow::GuiCol_Text], GetDisplayName(label));

            int prec = 3;
            if (step > 0.f) {
                prec = 0; float temp = step;
                while (temp < 0.999f && prec < 5) { temp *= 10.0f; prec++; }
            }

            std::string minStr = std::vformat(std::format("{{:.{}f}}", prec), std::make_format_args(min));
            std::string maxStr = std::vformat(std::format("{{:.{}f}}", prec), std::make_format_args(max));

            float wMin = Shadow::MeasureTextSize(minStr).x;
            float wMax = Shadow::MeasureTextSize(maxStr).x;

            float maxMeasuredTextW = std::max(wMin, wMax) + 4.0f;
            float valBoxWidth = std::max(45.0f, maxMeasuredTextW + Shadow::GetStyle().FramePadding.x * 2.f);

            float sliderWidth = g_ColLayout.Width * 0.40f;
            float rightEdge = g_RowStartPos.x + g_ColLayout.Width - 12.0f;

            Shadow::g_Ctx.Cursor.x = rightEdge - valBoxWidth - Shadow::GetStyle().ItemSpacing.x - sliderWidth;

            std::string hidden = std::string("##") + label;
            Shadow::Slider(hidden.c_str(), v, min, max, step, flags | Shadow::ShadowSliderFlags_NoRightAlign | Shadow::ShadowSliderFlags_NoText, { sliderWidth, 0.0f });

            EndRow();
        }

        inline bool Combo(const char* label, int* current_item, const std::vector<std::string>& items, Shadow::ShadowComboFlags flags = Shadow::ShadowComboFlags_None) {
            BeginRow();
            Shadow::Vec2 startPos = Shadow::g_Ctx.Cursor;

            Shadow::GetWindowDrawList()->AddText({ startPos.x, startPos.y + Shadow::GetStyle().FramePadding.y }, Shadow::GetStyle().Colors[Shadow::GuiCol_Text], GetDisplayName(label));
            float comboWidth = g_ColLayout.Width * 0.40f;

            if (flags & Shadow::ShadowComboFlags_FitText) {
                std::string currentText = (*current_item >= 0 && *current_item < static_cast<int>(items.size())) ? items[*current_item] : "Unknown";
                float triSize = Shadow::g_Ctx.ItemHeight * 0.5f;
                comboWidth = Shadow::GetStyle().FramePadding.x + Shadow::MeasureTextSize(currentText).x + Shadow::GetStyle().FramePadding.x + triSize + Shadow::GetStyle().FramePadding.x;
            }

            Shadow::g_Ctx.Cursor.x = g_RowStartPos.x + g_ColLayout.Width - comboWidth - 12.0f;

            std::string hidden = std::string("##") + label;
            bool ret = Shadow::Combo(hidden.c_str(), current_item, items, flags | Shadow::ShadowComboFlags_NoText | Shadow::ShadowComboFlags_NoRightAlign, { comboWidth, 0.0f });

            EndRow();
            return ret;
        }

        inline void ColorPicker(const char* label, Shadow::Color* color, Shadow::ShadowColorPickerFlags flags = Shadow::ShadowColorPickerFlags_None) {
            BeginRow();
            Shadow::Vec2 startPos = Shadow::g_Ctx.Cursor;

            Shadow::GetWindowDrawList()->AddText({ startPos.x, startPos.y + Shadow::GetStyle().FramePadding.y }, Shadow::GetStyle().Colors[Shadow::GuiCol_Text], GetDisplayName(label));

            float cpWidth = Shadow::g_Ctx.ItemHeight;
            Shadow::g_Ctx.Cursor.x = g_RowStartPos.x + g_ColLayout.Width - cpWidth - 12.0f;

            std::string hidden = std::string("##") + label;
            Shadow::ColorPicker(hidden.c_str(), color, flags | Shadow::ShadowColorPickerFlags_NoText | Shadow::ShadowColorPickerFlags_NoRightAlign, { cpWidth, 0.0f });

            EndRow();
        }

        inline bool SwitchColorPicker(const char* label, bool* v, Shadow::Color* color, Shadow::ShadowColorPickerFlags cpFlags = Shadow::ShadowColorPickerFlags_None) {
            BeginRow();
            Shadow::Vec2 startPos = Shadow::g_Ctx.Cursor;

            Shadow::GetWindowDrawList()->AddText({ startPos.x, startPos.y + Shadow::GetStyle().FramePadding.y }, Shadow::GetStyle().Colors[Shadow::GuiCol_Text], GetDisplayName(label));

            float switchWidth = (Shadow::g_Ctx.ItemHeight - 4.0f) * 2.0f + 4.0f;
            float cpWidth = Shadow::g_Ctx.ItemHeight;
            float spacing = Shadow::GetStyle().ItemSpacing.x;

            float rightEdge = g_RowStartPos.x + g_ColLayout.Width - 12.0f;

            Shadow::g_Ctx.Cursor.x = rightEdge - switchWidth - spacing - cpWidth;
            std::string hiddenCP = std::string("##CP_") + label;
            Shadow::ColorPicker(hiddenCP.c_str(), color, cpFlags | Shadow::ShadowColorPickerFlags_NoText | Shadow::ShadowColorPickerFlags_NoRightAlign, { cpWidth, 0.0f });

            Shadow::SameLine();

            Shadow::g_Ctx.Cursor.x = rightEdge - switchWidth - 10.0f;
            std::string hiddenSwitch = std::string("##") + label;
            bool ret = Shadow::Switch(hiddenSwitch.c_str(), v);

            EndRow();
            return ret;
        }

        // 高级热键 (带模式下拉框)
        inline void HotKey(const char* label, int* hotkey, bool* is_active, Shadow::HotkeyMode* mode, Shadow::ShadowHotkeyFlags flags = Shadow::ShadowHotkeyFlags_None) {
            BeginRow();
            Shadow::Vec2 startPos = Shadow::g_Ctx.Cursor;

            Shadow::GetWindowDrawList()->AddText({ startPos.x, startPos.y + Shadow::GetStyle().FramePadding.y }, Shadow::GetStyle().Colors[Shadow::GuiCol_Text], GetDisplayName(label));

            bool isAssigning = (Shadow::g_Ctx.AssigningHotkey == hotkey);
            std::string keyName = isAssigning ? "[Press Key]" : std::format("[{}]", Shadow::GetKeyName(*hotkey));

            float btnWidth = Shadow::MeasureTextSize(keyName).x + Shadow::GetStyle().FramePadding.x * 2.f;
            float dotSize = std::max(6.f, Shadow::g_Ctx.ItemHeight * 0.4f);
            float totalWidth = btnWidth + Shadow::GetStyle().ItemSpacing.x + dotSize;

            Shadow::g_Ctx.Cursor.x = g_RowStartPos.x + g_ColLayout.Width - totalWidth - 12.0f;

            std::string hidden = std::string("##") + label;
            Shadow::HotKey(hidden.c_str(), hotkey, is_active, mode, flags | Shadow::ShadowHotkeyFlags_NoText | Shadow::ShadowHotkeyFlags_NoRightAlign);

            EndRow();
        }

        // 简单热键 (仅按键绑定)
        inline bool HotKey(const char* label, int* hotkey, Shadow::ShadowHotkeyFlags flags = Shadow::ShadowHotkeyFlags_None) {
            BeginRow();
            Shadow::Vec2 startPos = Shadow::g_Ctx.Cursor;

            Shadow::GetWindowDrawList()->AddText({ startPos.x, startPos.y + Shadow::GetStyle().FramePadding.y }, Shadow::GetStyle().Colors[Shadow::GuiCol_Text], GetDisplayName(label));

            bool isAssigning = (Shadow::g_Ctx.AssigningHotkey == hotkey);
            std::string keyName = isAssigning ? "[Press Key]" : std::format("[{}]", Shadow::GetKeyName(*hotkey));
            float btnWidth = Shadow::MeasureTextSize(keyName).x + Shadow::GetStyle().FramePadding.x * 2.f;

            Shadow::g_Ctx.Cursor.x = g_RowStartPos.x + g_ColLayout.Width - btnWidth - 12.0f;

            std::string hidden = std::string("##") + label;
            bool ret = Shadow::HotKey(hidden.c_str(), hotkey, flags | Shadow::ShadowHotkeyFlags_NoText | Shadow::ShadowHotkeyFlags_NoRightAlign);

            EndRow();
            return ret;
        }

        inline bool InputText(const char* label, std::string& text, Shadow::ShadowInputTextFlags flags = Shadow::ShadowInputTextFlags_None) {
            BeginRow();
            Shadow::Vec2 startPos = Shadow::g_Ctx.Cursor;

            Shadow::GetWindowDrawList()->AddText({ startPos.x, startPos.y + Shadow::GetStyle().FramePadding.y }, Shadow::GetStyle().Colors[Shadow::GuiCol_Text], GetDisplayName(label));

            float inputWidth = g_ColLayout.Width * 0.40f;
            Shadow::g_Ctx.Cursor.x = g_RowStartPos.x + g_ColLayout.Width - inputWidth - 12.0f;

            std::string hidden = std::string("##") + label;
            bool ret = Shadow::InputText(hidden.c_str(), text, flags | Shadow::ShadowInputTextFlags_NoName, { inputWidth, 0.0f });

            EndRow();
            return ret;
        }

        inline bool InputFloat(const char* label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "{:.3f}", Shadow::ShadowInputTextFlags flags = Shadow::ShadowInputTextFlags_None) {
            BeginRow();
            Shadow::Vec2 startPos = Shadow::g_Ctx.Cursor;

            Shadow::GetWindowDrawList()->AddText({ startPos.x, startPos.y + Shadow::GetStyle().FramePadding.y }, Shadow::GetStyle().Colors[Shadow::GuiCol_Text], GetDisplayName(label));

            float inputWidth = g_ColLayout.Width * 0.40f;
            Shadow::g_Ctx.Cursor.x = g_RowStartPos.x + g_ColLayout.Width - inputWidth - 12.0f;

            std::string hidden = std::string("##") + label;
            bool ret = Shadow::InputFloat(hidden.c_str(), v, step, step_fast, format, flags | Shadow::ShadowInputTextFlags_NoName, { inputWidth, 0.0f });

            EndRow();
            return ret;
        }

        // 包含背景且自适应换行的描述行
        inline void TextDescription(const char* label) {
            BeginRow();
            Shadow::PushTextWrapPos(g_ColLayout.Width - 24.0f);
            Shadow::TextWrapped(Shadow::GetStyle().Colors[Shadow::GuiCol_TextDisabled], GetDisplayName(label));
            Shadow::PopTextWrapPos();
            EndRow();
        }
    }

    inline void RenderTabContent(int tabIndex)
    {
        const Shadow::GuiStyle& style = Shadow::GetStyle();

        switch (tabIndex)
        {
        case 0: {
            float availWidth = Shadow::g_Ctx.ClipMax.x - Shadow::g_Ctx.ClipMin.x;
            BeginColumns(Shadow::g_Ctx.Cursor.x, Shadow::g_Ctx.Cursor.y, availWidth, style.ItemSpacing.x * 2.0f);

            // 【左侧区域】
            SetColumn(true);
            BeginPanel(U("生命值"));
            {
                static bool godmode = true;
                Menu::Switch(U("无敌模式"), &godmode);

                // 孤立全宽按钮
                Shadow::Button(U("恢复至满血"), Shadow::Vec2{ g_ColLayout.Width, 0.0f });
            }
            EndPanel();

            BeginPanel(U("移动"));
            {
                static bool infinite_stamina = false;
                static float walk_speed = 4.0f;
                static float run_speed = 3.0f;
                static float crouch_speed = 5.0f;
                static bool infinite_jumps = false;
                static bool dont_tumble = false;
                static Shadow::Color col1 = { 1.f, 1.f, 1.f, 1.f };

                static int selectedTarget = 0;

                static const std::vector<std::string> tempOptions{
                    U("敌人"),
                    U("氏族"),
                    U("队友"),
                    U("AI"),
                    U("世界"),
                };


                Menu::Switch(U("无限体力"), &infinite_stamina);

                // 使用 JoinNext() 让三个 Slider 无缝拼接成一个大型卡片
                Menu::Slider(U("步行速度"), &walk_speed, 1.0f, 10.0f, 0.0f);
                Menu::JoinNext();
                Menu::Slider(U("奔跑速度"), &run_speed, 1.0f, 10.0f, 0.0f);
                Menu::JoinNext();
                Menu::Slider(U("下蹲速度"), &crouch_speed, 1.0f, 10.0f, 0.0f);

                Menu::Switch(U("无限跳跃"), &infinite_jumps);

                // 使用我们新封装的 SwitchColorPicker 组合控件，并向下缝合描述行
                Menu::SwitchColorPicker(U("防止跌倒"), &dont_tumble, &col1);
                Menu::JoinNext();
                Menu::TextDescription(U("禁用来自外部来源的跌倒效果"));

                Menu::Combo(U("阵营##target_combo"), &selectedTarget, tempOptions, Shadow::ShadowComboFlags_NoRightAlign | Shadow::ShadowComboFlags_FitText);

                float dt = Shadow::GetIO().DeltaTime;
                std::string fps = std::format("{:.1f}", dt > 0 ? 1.f / dt : 0.f);
                Menu::TextDescription(fps.c_str());
            }
            EndPanel();

            // 【右侧区域】
            SetColumn(false);
            BeginPanel(U("抓取"));
            {
                static bool high_strength = true;
                static bool unlimited_range = true;
                static bool no_overcharge = false;

                Menu::Switch(U("高强度"), &high_strength);
                Menu::JoinNext();
                Menu::TextDescription(U("仅当你是主机时可用"));

                Menu::Switch(U("无限距离"), &unlimited_range);
                Menu::Switch(U("无过载"), &no_overcharge);
            }
            EndPanel();

            BeginPanel(U("能力强化"));
            {
                Shadow::TextDisabled(U("为自己应用一个可用的升级"));

                Menu::BeginRow();
                Shadow::Button(U("蹲下休息"));
                Shadow::SameLine();
                Shadow::Button(U("冲刺速度"));
                Shadow::SameLine();
                Shadow::Button(U("发射"));
                Menu::EndRow();

                Menu::JoinNext();

                Menu::BeginRow();
                Shadow::Button(U("发射##2"));
                Shadow::SameLine();
                Shadow::Button(U("发射##3"));
                Menu::EndRow();
            }
            EndPanel();

            BeginPanel(U("医疗包"));
            {
                Shadow::TextDisabled(U("为自己使用一个可用的急救包"));

                Menu::BeginRow();
                Shadow::Button(U("大"));
                Shadow::SameLine();
                Shadow::Button(U("小"));
                Shadow::SameLine();
                Shadow::Button(U("中"));
                Menu::EndRow();
            }
            EndPanel();

            EndColumns();
            break;
        }

        case 1:
            Shadow::Text(U("这里是另一个选项卡"));
            break;

        default:
            break;
        }
    }

    struct TabDef {
        std::string Name;
        bool IsSeparator;
    };

    inline void DrawGUI()
    {
        static SDK::UFont* SansationBold18 = nullptr;
        static SDK::UFont* OpenSansRegular12 = nullptr;

        if (!SansationBold18) {
            SDK::UObject* _Font = SDK::UObject::FindObject("Font SansationBold18.SansationBold18");
            if (_Font && _Font->IsA(SDK::UFont::StaticClass())) SansationBold18 = (SDK::UFont*)_Font;
        }
        if (!OpenSansRegular12) {
            SDK::UObject* _Font = SDK::UObject::FindObject("Font OpenSansRegular12.OpenSansRegular12");
            if (_Font && _Font->IsA(SDK::UFont::StaticClass())) OpenSansRegular12 = (SDK::UFont*)_Font;
        }

        if (SansationBold18 && OpenSansRegular12) {
            Shadow::DefaultFont = OpenSansRegular12;
        }

        // StyleColorsExample();

        Shadow::GuiStyle& style = Shadow::GetStyle();
        Shadow::ShadowWindowFlags flags = Shadow::ShadowWindowFlags_NoTitleBar | Shadow::ShadowWindowFlags_NoScrollbar;

        Shadow::g_Ctx.WindowSizeConstraintMin = { 1200.f, 800.f };
        Shadow::g_Ctx.HasWindowSizeConstraints = true;

        if (Shadow::Begin("MainGUIWindow", flags))
        {
            // Shadow::PushFont(SansationBold18);
            Shadow::ShadowDrawList* drawList = Shadow::GetWindowDrawList();

            Shadow::Vec2 winPos = { std::floor(Shadow::GetWindowPos().x), std::floor(Shadow::GetWindowPos().y) };
            Shadow::Vec2 winSize = { std::floor(Shadow::GetWindowSize().x), std::floor(Shadow::GetWindowSize().y) };

            if (Shadow::g_Ctx.CurrentWindow) {
                Shadow::g_Ctx.CurrentWindow->ScrollY = 0.0f;
            }

            // 全面中文化侧边栏标签
            const std::vector<TabDef> tabs = {
                { U("数据管理"), false },
                { U("实体列表"), false },
                { U("视觉信息"), false },
                { U("---"), true },
                { U("物品"), false },
                { U("道具"), false },
                { U("关卡"), false },
                { U("杂项"), false },
                { U("---"), true },
                { U("全局设置"), false }
            };

            float originalWindowPaddingX = std::floor(style.WindowPadding.x);
            float originalFramePaddingX = std::floor(style.FramePadding.x);

            // 自动计算最长文本以避免侧边栏文字溢出
            float maxTextWidth = 0.0f;
            for (const auto& tab : tabs) {
                if (!tab.IsSeparator) {
                    float w = Shadow::MeasureTextSize(tab.Name).x;
                    if (w > maxTextWidth) maxTextWidth = w;
                }
            }
            float maxTextHeight = Shadow::MeasureTextSize("A").y;

            float sidebarPadding = originalWindowPaddingX;
            // 预留最少 100 的宽度，若是文本太长则自动扩展兼容
            float btnWidth = std::max(50.0f, maxTextWidth + originalFramePaddingX * 4.0f);
            float btnHeight = std::floor(maxTextHeight + (style.FramePadding.y * 2.0f) * 2.0f);
            float sidebarWidth = std::floor(btnWidth + (sidebarPadding * 2.0f));

            drawList->AddRectFilled(winPos, Shadow::Vec2{ sidebarWidth, winSize.y }, style.Colors[Shadow::GuiCol_WindowBg]);
            float contentLeftX = std::floor(sidebarWidth + style.ItemSpacing.x);
            float contentTopY = std::floor(winPos.y + style.WindowPadding.y);

            Shadow::Vec2 contentMin = { winPos.x + contentLeftX, contentTopY };
            Shadow::Vec2 contentMax = { std::floor(winPos.x + winSize.x - originalWindowPaddingX), std::floor(winPos.y + winSize.y - style.WindowPadding.y) };
            float contentHeight = contentMax.y - contentMin.y;

            float startY = std::floor(winPos.y + sidebarPadding);

            // Logo 标题
            Shadow::Vec2 titleSize = Shadow::MeasureTextSize(U("黑梦"));
            Shadow::Vec2 titlePos = { std::floor(winPos.x + sidebarPadding + (btnWidth - titleSize.x) * 0.5f), std::floor(startY) };
            drawList->AddText(titlePos, { 1.f, 1.f, 1.f, 1.f }, U("黑梦"));
            startY += std::floor(titleSize.y + style.ItemSpacing.y * 3.0f);

            int logicalTabIndex = 0;
            for (int i = 0; i < static_cast<int>(tabs.size()); ++i)
            {
                if (tabs[i].IsSeparator) {
                    // 通过精确获取可用高度的中点进行完美垂直居中绘制线段
                    float sepHeight = style.ItemSpacing.y * 2.0f + 2.0f;
                    float centerY = startY + sepHeight * 0.5f;

                    Shadow::Vec2 sepStart = { std::floor(winPos.x + sidebarPadding), std::floor(centerY) };
                    Shadow::Vec2 sepEnd = { std::floor(winPos.x + sidebarWidth - sidebarPadding), std::floor(centerY) };
                    drawList->AddLine(sepStart, sepEnd, style.Colors[Shadow::GuiCol_Separator], 2.0f);

                    startY += sepHeight;
                    continue;
                }

                Shadow::Vec2 btnPos = Shadow::Vec2{ std::floor(winPos.x + sidebarPadding), std::floor(startY) };
                Shadow::g_Ctx.Cursor = btnPos;

                bool isActive = (g_TabAnim.CurrentTab == logicalTabIndex);

                Shadow::Color originalBtnColor = style.Colors[Shadow::GuiCol_Button];
                if (isActive) {
                    style.Colors[Shadow::GuiCol_Button] = style.Colors[Shadow::GuiCol_TabActive];
                }

                style.FramePadding.x = originalFramePaddingX * 2.0f;

                if (Shadow::Button(tabs[i].Name, Shadow::Vec2{ btnWidth, btnHeight }))
                {
                    if (g_TabAnim.CurrentTab != logicalTabIndex)
                    {
                        if (g_TabAnim.IsAnimating)
                        {
                            g_TabAnim.PreviousTab = g_TabAnim.TargetTab;
                            g_TabAnim.StartOffset = g_TabAnim.CurrentOffset;
                        }
                        else
                        {
                            g_TabAnim.PreviousTab = g_TabAnim.CurrentTab;
                            g_TabAnim.StartOffset = 0.0f;
                        }

                        g_TabAnim.TargetTab = logicalTabIndex;
                        g_TabAnim.CurrentTab = logicalTabIndex;

                        int direction = (g_TabAnim.TargetTab > g_TabAnim.PreviousTab) ? 1 : -1;
                        g_TabAnim.TargetOffset = contentHeight * static_cast<float>(direction);

                        g_TabAnim.TransitionProgress = 0.0f;
                        g_TabAnim.IsAnimating = true;
                    }
                }

                style.FramePadding.x = originalFramePaddingX;
                style.Colors[Shadow::GuiCol_Button] = originalBtnColor;
                startY += std::floor(btnHeight + style.ItemSpacing.y);
                logicalTabIndex++;
            }
            // Shadow::PopFont();

            if (g_TabAnim.IsAnimating)
            {
                g_TabAnim.TransitionProgress += static_cast<float>(Shadow::g_Ctx.DeltaTime) / 0.25f;
                if (g_TabAnim.TransitionProgress >= 1.0f)
                {
                    g_TabAnim.TransitionProgress = 1.0f;
                    g_TabAnim.IsAnimating = false;
                    g_TabAnim.CurrentOffset = 0.0f;
                }
            }

            Shadow::Vec2 clipMin = Shadow::Vec2{ contentMin.x, contentMin.y };
            Shadow::Vec2 clipMax = Shadow::Vec2{ contentMax.x, contentMax.y };
            Shadow::PushClipRect(clipMin, clipMax);

            style.WindowPadding.x = contentLeftX;

            if (g_TabAnim.IsAnimating)
            {
                float eased = EaseOutCubic(g_TabAnim.TransitionProgress);
                g_TabAnim.CurrentOffset = std::floor(Lerp(g_TabAnim.StartOffset, g_TabAnim.TargetOffset, eased));
                Shadow::g_Ctx.Cursor = Shadow::Vec2{ contentMin.x, contentMin.y - g_TabAnim.CurrentOffset };
                RenderTabContent(g_TabAnim.PreviousTab);

                float nextY = (g_TabAnim.TargetOffset > 0)
                    ? (contentMin.y + contentHeight - g_TabAnim.CurrentOffset)
                    : (contentMin.y - contentHeight - g_TabAnim.CurrentOffset);

                Shadow::g_Ctx.Cursor = Shadow::Vec2{ contentMin.x, std::floor(nextY) };
                RenderTabContent(g_TabAnim.TargetTab);
            }
            else
            {
                Shadow::g_Ctx.Cursor = contentMin;
                RenderTabContent(g_TabAnim.CurrentTab);
            }

            style.WindowPadding.x = originalWindowPaddingX;
            Shadow::PopClipRect();
            Shadow::End();
        }
    }
}