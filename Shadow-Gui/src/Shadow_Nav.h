#pragma once
#include "Shadow.h"

namespace Shadow {
    namespace Nav {
        enum class NavState {
            Browsing,       // 列表浏览模式
            Interacting,    // 控件内部打字输入模式
            BindingKey      // 热键录入等待模式
        };

        struct NavMenuColumn {
            size_t Id = 0;
            std::string Title;
            std::string Subtitle = "";
            Vec2 Pos = { 100.f, 100.f };
            Vec2 Size = { 480.f, 760.f };
            int FocusIndex = 0;
            int ScrollOffset = 0;
            int TotalItems = 0;
            int ItemCounter = 0;
            size_t OpenSubmenuId = 0;
            std::vector<int> SelectableIndices;
        };

        struct NavContext {
            std::vector<NavMenuColumn> ColumnStack;
            int ActiveColumnIdx = 0;
            int RenderColumnIdx = 0;

            NavState State = NavState::Browsing;
            int SubFocus = 0; // 0 = Bind / 滑块, 1 = Mode
            bool CurrentItemConsumesLeftRight = false; // 标记当前选中的控件是否消费左右键

            // 栈计数器
            int BeginStack = 0;
            int TabBarStack = 0;
            int TabItemStack = 0;
            int TreeNodeStack = 0;

            // 按键连发计时：进入连发状态每 100ms 触发一次
            double NextRepeatTime[256] = { 0.0 };
            double RepeatDelay = 0.25; // 首次连击延迟 250ms
            double RepeatRate = 0.10;  // 连击频率 100ms

            // TabBar 状态
            size_t ActiveTabBarId = 0;
            int ActiveTabIdx = 0;
            std::vector<std::string> CurrentTabs;
            int TabCount = 0;

            // 动态排版参数
            float CharHeight = 16.f;
            float HeaderHeight = 44.f;
            float FooterHeight = 38.f;
            float RowHeight = 40.f;
            float PaddingX = 14.f;
            int MaxVisibleItems = 16;

            // Slider 输入缓冲区
            std::string SliderInputBuf;

            // ColorPicker 弹窗状态与指针
            bool CP_Open = false;
            size_t CP_ActiveId = 0;
            int CP_Focus = 0; // 0 = SV, 1 = Hue, 2 = Alpha
            float* CP_TargetR = nullptr;
            float* CP_TargetG = nullptr;
            float* CP_TargetB = nullptr;
            float* CP_TargetA = nullptr;

            int* HK_AssignTarget = nullptr;
        };

        inline NavContext g_Nav;

        // --- 文本逐字符精准裁切辅助函数 ---
        inline std::string ClipTextToWidth(std::string_view text, float maxWidth) {
            if (maxWidth <= 1.0f || text.empty()) return "";

            float totalW = MeasureTextSize(text).x;
            if (totalW <= maxWidth) return std::string(text);

            std::wstring wstr = ToWString(text);
            float currentW = 0.f;
            std::wstring clippedWstr;

            for (wchar_t ch : wstr) {
                float cw = MeasureCharWidth(ch);
                if (currentW + cw > maxWidth) break;
                currentW += cw;
                clippedWstr += ch;
            }

            if (clippedWstr.empty()) return "";

            int size_needed = WideCharToMultiByte(CP_UTF8, 0, clippedWstr.c_str(), static_cast<int>(clippedWstr.size()), nullptr, 0, nullptr, nullptr);
            std::string result(size_needed, 0);
            WideCharToMultiByte(CP_UTF8, 0, clippedWstr.c_str(), static_cast<int>(clippedWstr.size()), &result[0], size_needed, nullptr, nullptr);
            return result;
        }

        inline bool IsNavKey(int vk, bool allowRepeat = true) {
            if (g_Ctx.KeyPressed[vk]) {
                g_Nav.NextRepeatTime[vk] = g_Ctx.RealTimeSeconds + g_Nav.RepeatDelay;
                return true;
            }

            if (!allowRepeat || !g_Ctx.KeyStates[vk]) {
                return false;
            }

            if (g_Ctx.RealTimeSeconds >= g_Nav.NextRepeatTime[vk]) {
                g_Nav.NextRepeatTime[vk] = g_Ctx.RealTimeSeconds + g_Nav.RepeatRate;
                return true;
            }

            return false;
        }

        inline bool IsNavKeyJustPressed(int vk) {
            return g_Ctx.KeyPressed[vk];
        }

        // 获取按键在当前帧的平滑移动量（支持点按细腻单步，长按平滑加速）
        inline float GetNavKeyMoveDelta(int vk, float baseStep = 0.005f, float minSpeed = 0.15f, float maxSpeed = 2.0f, float accelDuration = 1.2f) {
            if (!g_Ctx.KeyStates[vk]) return 0.0f;

            // 点按初次按下第一帧：直接触发细腻基础步长
            if (g_Ctx.KeyPressed[vk]) {
                return baseStep;
            }

            // 长按状态检测
            double pressTime = g_Ctx.KeyPressTime[vk];
            if (pressTime > 0.0) {
                float holdDuration = static_cast<float>(g_Ctx.RealTimeSeconds - pressTime);
                float delay = static_cast<float>(g_Nav.RepeatDelay);
                if (holdDuration > delay) {
                    // 计算加速插值系数并基于 DeltaTime 进行平滑帧移动
                    float t = std::clamp((holdDuration - delay) / accelDuration, 0.0f, 1.0f);
                    float currentSpeed = minSpeed + t * (maxSpeed - minSpeed);
                    float dt = static_cast<float>(g_Ctx.DeltaTime);
                    return currentSpeed * dt;
                }
            }
            return 0.0f;
        }

        // Nav 专属错误检测与绘制
        inline void CheckAndDrawErrors() {
            std::string errorMsg;
            if (g_Nav.BeginStack > 0) errorMsg = std::format("ERROR: Nav::Begin() called {} time(s) without matching Nav::End()!", g_Nav.BeginStack);
            else if (g_Nav.BeginStack < 0) errorMsg = std::format("ERROR: Nav::End() called {} time(s) without matching Nav::Begin()!", -g_Nav.BeginStack);
            else if (g_Nav.TabBarStack > 0) errorMsg = std::format("ERROR: Nav::BeginTabBar() called {} time(s) without matching Nav::EndTabBar()!", g_Nav.TabBarStack);
            else if (g_Nav.TabBarStack < 0) errorMsg = std::format("ERROR: Nav::EndTabBar() called {} time(s) without matching BeginTabBar()!", -g_Nav.TabBarStack);
            else if (g_Nav.TabItemStack > 0) errorMsg = std::format("ERROR: Nav::BeginTabItem() called {} time(s) without matching Nav::EndTabItem()!", g_Nav.TabItemStack);
            else if (g_Nav.TabItemStack < 0) errorMsg = std::format("ERROR: Nav::EndTabItem() called {} time(s) without matching BeginTabItem()!", -g_Nav.TabItemStack);
            else if (g_Nav.TreeNodeStack > 0) errorMsg = std::format("ERROR: Nav::TreeNode() called {} time(s) without matching Nav::TreePop()!", g_Nav.TreeNodeStack);
            else if (g_Nav.TreeNodeStack < 0) errorMsg = std::format("ERROR: Nav::TreePop() called {} time(s) without matching Nav::TreeNode()!", -g_Nav.TreeNodeStack);

            if (!errorMsg.empty()) {
                GetWindowDrawList()->AddText(g_Ctx.DefaultFont, 1.0f, g_Ctx.Style.Colors[GuiCol_TextShadow], g_Ctx.Style.Colors[GuiCol_TextOutline], { 5.f, 5.f }, g_Ctx.Style.Colors[GuiCol_ErrorText], errorMsg);
            }
        }

        // --- 菜单生命周期 API ---

        inline bool Begin(std::string_view title, std::string_view subtitle = "", Vec2 default_pos = { 80.f, 80.f }, Vec2 default_size = { 480.f, 760.f }) {
            g_Nav.BeginStack++;

            std::string_view display; size_t rootId;
            ParseLabel(title, display, rootId);

            if (g_Nav.ColumnStack.empty()) {
                NavMenuColumn root;
                root.Id = rootId;
                root.Title = std::string(display);
                root.Subtitle = std::string(subtitle);
                root.Pos = default_pos;
                root.Size = default_size;
                g_Nav.ColumnStack.push_back(root);
                g_Nav.ActiveColumnIdx = 0;
            }

            auto& win = g_Ctx.Windows[rootId];
            if (win.Id == 0) {
                win.Id = rootId;
                win.Name = std::string(display);
                g_Ctx.WindowDisplayOrder.push_back(rootId);
                win.Pos = default_pos;
                win.Size = default_size;
            }
            win.LastAccessedFrame = g_Ctx.FrameCount;
            g_Ctx.CurrentWindow = &win;

            g_Nav.RenderColumnIdx = 0;
            for (auto& c : g_Nav.ColumnStack) {
                c.TotalItems = 0;
                c.ItemCounter = 0;
                c.SelectableIndices.clear();
            }

            g_Nav.ColumnStack[0].Title = std::string(display);
            g_Nav.ColumnStack[0].Subtitle = std::string(subtitle);
            g_Nav.ColumnStack[0].Pos = default_pos;
            g_Nav.ColumnStack[0].Size = default_size;
            g_Nav.CurrentTabs.clear();
            g_Nav.CurrentItemConsumesLeftRight = false;

            float charH = MeasureTextHeight("A");
            if (charH <= 0.f) charH = 16.f;
            g_Nav.CharHeight = charH;
            g_Nav.RowHeight = std::max(26.f, charH + g_Ctx.Style.FramePadding.y * 2.f + 12.f);
            g_Nav.HeaderHeight = std::max(28.f, charH + g_Ctx.Style.FramePadding.y * 2.f + 16.f);
            g_Nav.FooterHeight = std::max(24.f, charH + g_Ctx.Style.FramePadding.y * 2.f + 10.f);
            g_Nav.PaddingX = std::max(10.f, std::round(charH * 0.55f));

            float contentAreaH = g_Nav.ColumnStack[0].Size.y - g_Nav.HeaderHeight - g_Nav.FooterHeight;
            g_Nav.MaxVisibleItems = std::max(1, static_cast<int>(contentAreaH / g_Nav.RowHeight));

            for (size_t colIdx = 0; colIdx < g_Nav.ColumnStack.size(); ++colIdx) {
                const auto& c = g_Nav.ColumnStack[colIdx];
                if (colIdx == 0) {
                    GetWindowDrawList()->AddRectFilled(c.Pos, c.Size, g_Ctx.Style.Colors[GuiCol_WindowBg]);
                    GetWindowDrawList()->AddRect(c.Pos, c.Size, g_Ctx.Style.Colors[GuiCol_Border], 1.5f);
                }
                else {
                    Vec2 subWinPos = { c.Pos.x, c.Pos.y + g_Nav.HeaderHeight };
                    Vec2 subWinSize = { c.Size.x, c.Size.y - g_Nav.HeaderHeight };
                    GetWindowDrawList()->AddRectFilled(subWinPos, subWinSize, g_Ctx.Style.Colors[GuiCol_WindowBg]);
                    GetWindowDrawList()->AddRect(subWinPos, subWinSize, g_Ctx.Style.Colors[GuiCol_Border], 1.5f);
                }
            }

            return true;
        }

        inline bool BeginTabBar(std::string_view name) {
            g_Nav.TabBarStack++;

            std::string_view display; size_t id;
            ParseLabel(name, display, id);

            g_Nav.ActiveTabBarId = id;
            g_Nav.CurrentTabs.clear();
            return true;
        }

        inline bool BeginTabItem(std::string_view name) {
            g_Nav.TabItemStack++;

            std::string_view display; size_t id;
            ParseLabel(name, display, id);

            int tabIdx = static_cast<int>(g_Nav.CurrentTabs.size());
            g_Nav.CurrentTabs.push_back(std::string(display));
            return (tabIdx == g_Nav.ActiveTabIdx);
        }

        inline void EndTabItem() {
            g_Nav.TabItemStack--;
        }

        inline void EndTabBar() {
            g_Nav.TabBarStack--;

            g_Nav.TabCount = static_cast<int>(g_Nav.CurrentTabs.size());
            if (g_Nav.TabCount > 0) {
                g_Nav.ActiveTabIdx = (g_Nav.ActiveTabIdx % g_Nav.TabCount + g_Nav.TabCount) % g_Nav.TabCount;

                NavMenuColumn& col = g_Nav.ColumnStack[0];
                float tabWidth = col.Size.x / static_cast<float>(g_Nav.TabCount);
                for (int t = 0; t < g_Nav.TabCount; ++t) {
                    Vec2 tabPos = { col.Pos.x + t * tabWidth, col.Pos.y };
                    Vec2 tabSize = { tabWidth, g_Nav.HeaderHeight };
                    bool isCurTab = (t == g_Nav.ActiveTabIdx);

                    // Tab 文本裁切
                    std::string clippedTabName = ClipTextToWidth(g_Nav.CurrentTabs[t], tabWidth - 8.f);
                    float textW = MeasureTextSize(clippedTabName).x;

                    if (isCurTab) {
                        GetWindowDrawList()->AddRectFilled(tabPos, tabSize, g_Ctx.Style.Colors[GuiCol_TextHighlight]);
                        GetWindowDrawList()->AddText({ tabPos.x + (tabWidth - textW) * 0.5f, tabPos.y + (g_Nav.HeaderHeight - g_Nav.CharHeight) * 0.5f }, g_Ctx.Style.Colors[GuiCol_WindowBg], clippedTabName);
                    }
                    else {
                        GetWindowDrawList()->AddText({ tabPos.x + (tabWidth - textW) * 0.5f, tabPos.y + (g_Nav.HeaderHeight - g_Nav.CharHeight) * 0.5f }, g_Ctx.Style.Colors[GuiCol_TextDisabled], clippedTabName);
                    }
                }
                GetWindowDrawList()->AddLine({ col.Pos.x, col.Pos.y + g_Nav.HeaderHeight }, { col.Pos.x + col.Size.x, col.Pos.y + g_Nav.HeaderHeight }, g_Ctx.Style.Colors[GuiCol_Border], 1.5f);
            }
        }

        inline bool BeginItem(bool disabled, Vec2& outPos, Vec2& outSize, bool& outIsFocused) {
            NavMenuColumn& col = g_Nav.ColumnStack[g_Nav.RenderColumnIdx];
            int itemIndex = col.ItemCounter++;
            col.TotalItems++;

            if (!disabled) {
                col.SelectableIndices.push_back(itemIndex);
            }

            bool isColActive = (g_Nav.RenderColumnIdx == g_Nav.ActiveColumnIdx);
            outIsFocused = isColActive && (col.FocusIndex == itemIndex) && !disabled;

            if (outIsFocused) {
                if (itemIndex < col.ScrollOffset) {
                    col.ScrollOffset = itemIndex;
                }
                if (itemIndex >= col.ScrollOffset + g_Nav.MaxVisibleItems) {
                    col.ScrollOffset = itemIndex - g_Nav.MaxVisibleItems + 1;
                }
            }

            if (itemIndex < col.ScrollOffset || itemIndex >= col.ScrollOffset + g_Nav.MaxVisibleItems) {
                return false;
            }

            int visibleIdx = itemIndex - col.ScrollOffset;
            outPos = { col.Pos.x, col.Pos.y + g_Nav.HeaderHeight + visibleIdx * g_Nav.RowHeight };
            outSize = { col.Size.x, g_Nav.RowHeight };

            if (outIsFocused) {
                GetWindowDrawList()->AddRectFilled(outPos, outSize, g_Ctx.Style.Colors[GuiCol_TextHighlight]);
            }

            return true;
        }

        // --- 核心控件（严格限定在每列宽度内裁切） ---

        inline void TextColored(Color color, std::string_view text) {
            std::string_view display; size_t id;
            ParseLabel(text, display, id);

            Vec2 pos, size;
            bool isFocused = false;
            bool visible = BeginItem(true, pos, size, isFocused);
            if (!visible) return;

            float maxW = size.x - g_Nav.PaddingX * 2.f;
            std::string clipped = ClipTextToWidth(display, maxW);
            GetWindowDrawList()->AddText({ pos.x + g_Nav.PaddingX, pos.y + (size.y - g_Nav.CharHeight) * 0.5f }, color, clipped);
        }

        inline void Text(std::string_view text) {
            std::string_view display; size_t id;
            ParseLabel(text, display, id);

            Vec2 pos, size;
            bool isFocused = false;
            bool visible = BeginItem(true, pos, size, isFocused);
            if (!visible) return;

            float maxW = size.x - g_Nav.PaddingX * 2.f;
            std::string clipped = ClipTextToWidth(display, maxW);
            GetWindowDrawList()->AddText({ pos.x + g_Nav.PaddingX, pos.y + (size.y - g_Nav.CharHeight) * 0.5f }, g_Ctx.Style.Colors[GuiCol_Text], clipped);
        }

        inline bool Checkbox(std::string_view name, bool* value) {
            std::string_view display; size_t id;
            ParseLabel(name, display, id);

            bool disabled = IsDisabled();
            Vec2 pos, size;
            bool isFocused = false;
            bool visible = BeginItem(disabled, pos, size, isFocused);

            if (isFocused) {
                if (IsNavKeyJustPressed(VK_RETURN)) {
                    *value = !(*value);
                }
            }

            if (!visible) return *value;

            Color textColor = isFocused ? g_Ctx.Style.Colors[GuiCol_WindowBg] : (disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text]);
            Color boxColor = isFocused ? g_Ctx.Style.Colors[GuiCol_WindowBg] : (*value ? g_Ctx.Style.Colors[GuiCol_CheckMark] : g_Ctx.Style.Colors[GuiCol_Text]);

            float boxSize = std::max(14.f, std::round(g_Nav.CharHeight * 0.85f));
            Vec2 boxPos = { pos.x + g_Nav.PaddingX, pos.y + (size.y - boxSize) * 0.5f };
            if (*value) {
                GetWindowDrawList()->AddRectFilled(boxPos, { boxSize, boxSize }, boxColor);
            }
            else {
                GetWindowDrawList()->AddRect(boxPos, { boxSize, boxSize }, boxColor, 2.0f);
            }

            float textStartX = pos.x + g_Nav.PaddingX + boxSize + g_Nav.PaddingX * 0.7f;
            float maxTextW = (pos.x + size.x - g_Nav.PaddingX) - textStartX;
            std::string clipped = ClipTextToWidth(display, maxTextW);

            GetWindowDrawList()->AddText({ textStartX, pos.y + (size.y - g_Nav.CharHeight) * 0.5f }, textColor, clipped);
            return *value;
        }

        inline bool Button(std::string_view name) {
            std::string_view display; size_t id;
            ParseLabel(name, display, id);

            bool disabled = IsDisabled();
            Vec2 pos, size;
            bool isFocused = false;
            bool visible = BeginItem(disabled, pos, size, isFocused);

            bool triggered = false;
            if (isFocused && !disabled) {
                if (g_Ctx.KeyStates[VK_RETURN]) {
                    triggered = true;
                }
            }

            if (!visible) return triggered;

            Color textColor = isFocused ? g_Ctx.Style.Colors[GuiCol_WindowBg] : (disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text]);
            float maxTextW = size.x - g_Nav.PaddingX * 2.f;
            std::string clipped = ClipTextToWidth(display, maxTextW);

            GetWindowDrawList()->AddText({ pos.x + g_Nav.PaddingX, pos.y + (size.y - g_Nav.CharHeight) * 0.5f }, textColor, clipped);
            return triggered;
        }

        inline void Slider(std::string_view name, float* value, float min_val, float max_val, float step = 1.0f) {
            std::string_view display; size_t id;
            ParseLabel(name, display, id);

            bool disabled = IsDisabled();
            Vec2 pos, size;
            bool isFocused = false;
            bool visible = BeginItem(disabled, pos, size, isFocused);

            if (isFocused && !disabled) {
                g_Nav.CurrentItemConsumesLeftRight = true;

                if (g_Nav.State != NavState::Interacting) {
                    if (IsNavKey(VK_LEFT))  *value = std::clamp(*value - step, min_val, max_val);
                    if (IsNavKey(VK_RIGHT)) *value = std::clamp(*value + step, min_val, max_val);

                    if (IsNavKeyJustPressed(VK_RETURN)) {
                        g_Nav.State = NavState::Interacting;
                        g_Nav.SliderInputBuf = std::format("{:.2f}", *value);
                    }
                }
                else {
                    for (char c : g_Ctx.InputChars) {
                        if ((c >= '0' && c <= '9') || c == '.' || c == '-' || c == '+') {
                            g_Nav.SliderInputBuf += c;
                        }
                    }
                    if (IsNavKeyJustPressed(VK_BACK)) {
                        if (!g_Nav.SliderInputBuf.empty()) {
                            g_Nav.SliderInputBuf.pop_back();
                        }
                    }
                    if (IsNavKeyJustPressed(VK_ESCAPE)) {
                        g_Nav.State = NavState::Browsing;
                    }
                    if (IsNavKeyJustPressed(VK_RETURN)) {
                        if (!g_Nav.SliderInputBuf.empty()) {
                            float parsed = 0.f;
                            auto [ptr, ec] = std::from_chars(g_Nav.SliderInputBuf.data(), g_Nav.SliderInputBuf.data() + g_Nav.SliderInputBuf.size(), parsed);
                            if (ec == std::errc()) {
                                *value = std::clamp(parsed, min_val, max_val);
                            }
                        }
                        g_Nav.State = NavState::Browsing;
                    }
                }
            }

            if (!visible) return;

            Color textColor = isFocused ? g_Ctx.Style.Colors[GuiCol_WindowBg] : (disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text]);

            std::string valStr;
            if (g_Nav.State == NavState::Interacting && isFocused) {
                bool blink = (static_cast<uint64_t>(g_Ctx.RealTimeSeconds * 1000.0) / 400) % 2 == 0;
                valStr = std::format("[ {}{} ]", g_Nav.SliderInputBuf, blink ? "_" : " ");
            }
            else {
                valStr = std::format("< {:.2f} >", *value);
            }

            float valW = MeasureTextSize(valStr).x;
            float spacing = 12.f;
            float maxLabelW = size.x - g_Nav.PaddingX * 2.f - valW - spacing;

            std::string clippedLabel = ClipTextToWidth(display, maxLabelW);

            GetWindowDrawList()->AddText({ pos.x + g_Nav.PaddingX, pos.y + (size.y - g_Nav.CharHeight) * 0.5f }, textColor, clippedLabel);
            GetWindowDrawList()->AddText({ pos.x + size.x - valW - g_Nav.PaddingX, pos.y + (size.y - g_Nav.CharHeight) * 0.5f }, textColor, valStr);
        }

        inline bool TreeNode(std::string_view name, std::string_view subtitle = "") {
            g_Nav.TreeNodeStack++;

            std::string_view display; size_t id;
            ParseLabel(name, display, id);

            bool disabled = IsDisabled();
            Vec2 pos, size;
            bool isFocused = false;
            int parentColIdx = g_Nav.RenderColumnIdx;
            bool visible = BeginItem(disabled, pos, size, isFocused);

            NavMenuColumn& parentCol = g_Nav.ColumnStack[parentColIdx];

            if (isFocused) {
                parentCol.OpenSubmenuId = id;
            }
            else if (parentCol.OpenSubmenuId == id && (parentCol.FocusIndex != parentCol.ItemCounter - 1)) {
                if (g_Nav.ColumnStack.size() > static_cast<size_t>(parentColIdx + 1)) {
                    g_Nav.ColumnStack.resize(parentColIdx + 1);
                    g_Nav.ActiveColumnIdx = std::min(g_Nav.ActiveColumnIdx, parentColIdx);
                }
                parentCol.OpenSubmenuId = 0;
            }

            bool isSubmenuOpen = (g_Nav.ColumnStack.size() > static_cast<size_t>(parentColIdx + 1)) &&
                (g_Nav.ColumnStack[parentColIdx + 1].Id == id);

            if (isFocused && !disabled) {
                if (IsNavKeyJustPressed(VK_RETURN)) {
                    if (!isSubmenuOpen) {
                        g_Nav.ColumnStack.resize(parentColIdx + 1);

                        NavMenuColumn subCol;
                        subCol.Id = id;
                        subCol.Title = std::string(display);
                        subCol.Subtitle = std::string(subtitle);
                        subCol.Pos = { parentCol.Pos.x + parentCol.Size.x + 8.f, parentCol.Pos.y };
                        subCol.Size = parentCol.Size;
                        subCol.FocusIndex = 0;
                        subCol.ScrollOffset = 0;
                        subCol.ItemCounter = 0;
                        subCol.TotalItems = 0;
                        g_Nav.ColumnStack.push_back(subCol);
                        g_Nav.ActiveColumnIdx = static_cast<int>(g_Nav.ColumnStack.size()) - 1;
                        parentCol.OpenSubmenuId = id;
                        isSubmenuOpen = true;

                        // 吞掉当前帧的回车事件，避免新激活子菜单的首个控件同帧触发
                        g_Ctx.KeyPressed[VK_RETURN] = false;
                        g_Ctx.KeyStates[VK_RETURN] = false;
                    }
                    else {
                        g_Nav.ActiveColumnIdx = parentColIdx + 1;
                        // 吞掉当前帧的回车事件
                        g_Ctx.KeyPressed[VK_RETURN] = false;
                        g_Ctx.KeyStates[VK_RETURN] = false;
                    }
                }
            }

            if (visible) {
                Color textColor = isFocused ? g_Ctx.Style.Colors[GuiCol_WindowBg] : (disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text]);
                float arrowW = MeasureTextSize(">>").x;
                float maxLabelW = size.x - g_Nav.PaddingX * 2.f - arrowW - 14.f;

                std::string clipped = ClipTextToWidth(display, maxLabelW);

                GetWindowDrawList()->AddText({ pos.x + g_Nav.PaddingX, pos.y + (size.y - g_Nav.CharHeight) * 0.5f }, textColor, clipped);
                GetWindowDrawList()->AddText({ pos.x + size.x - arrowW - g_Nav.PaddingX, pos.y + (size.y - g_Nav.CharHeight) * 0.5f }, textColor, ">>");
            }

            if (isSubmenuOpen) {
                g_Nav.RenderColumnIdx = parentColIdx + 1;
            }

            return isSubmenuOpen;
        }

        inline void TreePop() {
            g_Nav.TreeNodeStack--;
            if (g_Nav.RenderColumnIdx > 0) {
                g_Nav.RenderColumnIdx--;
            }
        }

        inline void HotKey(std::string_view name, int* hotkey, bool* is_active, HotkeyMode* mode) {
            RegisterHotkey(hotkey, mode, is_active);

            std::string_view display; size_t id;
            ParseLabel(name, display, id);

            bool disabled = IsDisabled();
            Vec2 pos, size;
            bool isFocused = false;
            bool visible = BeginItem(disabled, pos, size, isFocused);

            if (isFocused && !disabled) {
                g_Nav.CurrentItemConsumesLeftRight = true;

                if (g_Nav.State != NavState::BindingKey) {
                    if (IsNavKeyJustPressed(VK_LEFT))  g_Nav.SubFocus = 0;
                    if (IsNavKeyJustPressed(VK_RIGHT)) g_Nav.SubFocus = 1;

                    if (IsNavKeyJustPressed(VK_RETURN)) {
                        if (g_Nav.SubFocus == 0) {
                            g_Nav.State = NavState::BindingKey;
                            g_Nav.HK_AssignTarget = hotkey;
                        }
                        else {
                            int m = (static_cast<int>(*mode) + 1) % 5;
                            *mode = static_cast<HotkeyMode>(m);
                        }
                    }
                }
                else {
                    for (int vk = 1; vk < 256; ++vk) {
                        if (vk == VK_RETURN) continue;
                        if (g_Ctx.KeyPressed[vk]) {
                            if (vk == VK_ESCAPE) *hotkey = 0;
                            else *hotkey = vk;
                            g_Nav.State = NavState::Browsing;
                            g_Nav.HK_AssignTarget = nullptr;
                            break;
                        }
                    }
                }
            }

            if (!visible) return;

            Color textColor = isFocused ? g_Ctx.Style.Colors[GuiCol_WindowBg] : (disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text]);

            std::string keyName = (g_Nav.State == NavState::BindingKey && isFocused) ? "[Press Key]" : std::format("[{}]", GetKeyName(*hotkey));
            std::vector<std::string> modeStrs = { "None", "Hold On", "Toggle", "Hold Off", "Always" };
            std::string modeStr = std::format("<{}>", modeStrs[static_cast<int>(*mode)]);

            float modeW = MeasureTextSize(modeStr).x;
            float keyW = MeasureTextSize(keyName).x;
            float spaceW = MeasureTextSize(" ").x * 1.5f;
            float totalRightW = keyW + modeW + spaceW;
            float maxLabelW = size.x - g_Nav.PaddingX * 2.f - totalRightW - 14.f;

            std::string clippedLabel = ClipTextToWidth(display, maxLabelW);

            GetWindowDrawList()->AddText({ pos.x + g_Nav.PaddingX, pos.y + (size.y - g_Nav.CharHeight) * 0.5f }, textColor, clippedLabel);

            Vec2 keyPos = { pos.x + size.x - totalRightW - g_Nav.PaddingX, pos.y + (size.y - g_Nav.CharHeight) * 0.5f };
            Vec2 modePos = { keyPos.x + keyW + spaceW, keyPos.y };

            if (isFocused) {
                if (g_Nav.SubFocus == 0) {
                    GetWindowDrawList()->AddRectFilled({ keyPos.x - 4.f, pos.y + 4.f }, { keyW + 8.f, size.y - 8.f }, g_Ctx.Style.Colors[GuiCol_WindowBg]);
                    GetWindowDrawList()->AddText(keyPos, g_Ctx.Style.Colors[GuiCol_TextHighlight], keyName);
                    GetWindowDrawList()->AddText(modePos, g_Ctx.Style.Colors[GuiCol_WindowBg], modeStr);
                }
                else {
                    GetWindowDrawList()->AddText(keyPos, g_Ctx.Style.Colors[GuiCol_WindowBg], keyName);
                    GetWindowDrawList()->AddRectFilled({ modePos.x - 4.f, pos.y + 4.f }, { modeW + 8.f, size.y - 8.f }, g_Ctx.Style.Colors[GuiCol_WindowBg]);
                    GetWindowDrawList()->AddText(modePos, g_Ctx.Style.Colors[GuiCol_TextHighlight], modeStr);
                }
            }
            else {
                GetWindowDrawList()->AddText(keyPos, textColor, keyName);
                GetWindowDrawList()->AddText(modePos, textColor, modeStr);
            }
        }

        inline void ColorPicker(std::string_view name, float* r, float* g, float* b, float* a = nullptr) {
            std::string_view display; size_t id;
            ParseLabel(name, display, id);

            bool disabled = IsDisabled();
            Vec2 pos, size;
            bool isFocused = false;
            bool visible = BeginItem(disabled, pos, size, isFocused);

            if (isFocused && !disabled) {
                if (IsNavKeyJustPressed(VK_RETURN)) {
                    g_Nav.CP_Open = !g_Nav.CP_Open;
                    if (g_Nav.CP_Open) {
                        g_Nav.CP_ActiveId = id;
                        g_Nav.CP_TargetR = r;
                        g_Nav.CP_TargetG = g;
                        g_Nav.CP_TargetB = b;
                        g_Nav.CP_TargetA = a;
                        g_Ctx.ColorPickerR = r;
                        g_Ctx.ColorPickerG = g;
                        g_Ctx.ColorPickerB = b;
                        g_Ctx.ColorPickerA = a;
                        g_Nav.CP_Focus = 0;
                        RGBtoHSV(*r, *g, *b, g_Ctx.ColorPickerH, g_Ctx.ColorPickerS, g_Ctx.ColorPickerV);
                    }
                    else {
                        if (g_Ctx.ActiveInputId == HashString("CPHexInput")) {
                            g_Ctx.ActiveInputId = 0;
                        }
                    }
                    g_Ctx.KeyPressed[VK_RETURN] = false;
                }
            }

            if (!visible) return;

            Color textColor = isFocused ? g_Ctx.Style.Colors[GuiCol_WindowBg] : (disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text]);

            float previewSize = std::max(16.f, std::round(g_Nav.CharHeight * 0.95f));
            float maxLabelW = size.x - g_Nav.PaddingX * 2.f - previewSize - 14.f;

            std::string clipped = ClipTextToWidth(display, maxLabelW);

            GetWindowDrawList()->AddText({ pos.x + g_Nav.PaddingX, pos.y + (size.y - g_Nav.CharHeight) * 0.5f }, textColor, clipped);

            Vec2 previewPos = { pos.x + size.x - previewSize - g_Nav.PaddingX, pos.y + (size.y - previewSize) * 0.5f };

            float globalAlpha = g_Ctx.Style.Colors[GuiCol_ColorPickerLight].a;
            if (disabled) globalAlpha *= 0.5f;

            Color cbLight = g_Ctx.Style.Colors[GuiCol_CheckerboardLight];
            Color cbDark = g_Ctx.Style.Colors[GuiCol_CheckerboardDark];

            float currentA = a ? *a : 1.0f;
            int checkerSize = std::max(3, static_cast<int>(previewSize / 4.f));

            for (int y = 0; y < static_cast<int>(previewSize); y += checkerSize) {
                for (int x = 0; x < static_cast<int>(previewSize); x += checkerSize) {
                    bool isWhite = ((x / checkerSize) + (y / checkerSize)) % 2 == 0;
                    Color bgCol = isWhite ? cbLight : cbDark;

                    float finalR = *r * currentA + bgCol.r * (1.f - currentA);
                    float finalG = *g * currentA + bgCol.g * (1.f - currentA);
                    float finalB = *b * currentA + bgCol.b * (1.f - currentA);

                    float drawX = previewPos.x + x;
                    float drawY = previewPos.y + y;
                    float w = std::min(static_cast<float>(checkerSize), previewSize - x);
                    float h = std::min(static_cast<float>(checkerSize), previewSize - y);

                    GetWindowDrawList()->AddRectFilled({ drawX, drawY }, { w, h }, { finalR, finalG, finalB, globalAlpha });
                }
            }
        }

        inline void ColorPicker(std::string_view name, Color* color) {
            if (color) {
                ColorPicker(name, &color->r, &color->g, &color->b, &color->a);
            }
        }

        // --- 结束渲染与按键分发（严格各列边界计算） ---

        inline void End() {
            g_Nav.BeginStack--;

            if (g_Nav.ColumnStack.empty()) {
                CheckAndDrawErrors();
                return;
            }

            for (int colIdx = 0; colIdx < static_cast<int>(g_Nav.ColumnStack.size()); ++colIdx) {
                NavMenuColumn& col = g_Nav.ColumnStack[colIdx];
                bool isActiveCol = (colIdx == g_Nav.ActiveColumnIdx);

                // 底部状态栏自适应与超长文本裁切防重叠
                Vec2 footerPos = { col.Pos.x, col.Pos.y + col.Size.y - g_Nav.FooterHeight };
                GetWindowDrawList()->AddRectFilled(footerPos, { col.Size.x, g_Nav.FooterHeight }, g_Ctx.Style.Colors[GuiCol_TitleBarBg]);
                GetWindowDrawList()->AddLine(footerPos, { col.Pos.x + col.Size.x, footerPos.y }, g_Ctx.Style.Colors[GuiCol_Border], 1.5f);

                // 右侧索引/页码文本构建与安全限制
                std::string subText;
                if (!col.Subtitle.empty()) {
                    subText = std::format("{} [{}/{}]", col.Subtitle, col.TotalItems > 0 ? (col.FocusIndex + 1) : 0, col.TotalItems);
                }
                else {
                    subText = std::format("[{}/{}]", col.TotalItems > 0 ? (col.FocusIndex + 1) : 0, col.TotalItems);
                }

                // 限制右侧文字最多占当前列宽度的 55%，超出时先裁切自身
                float maxSubW = col.Size.x * 0.55f;
                std::string clippedSubText = ClipTextToWidth(subText, maxSubW);
                float subW = MeasureTextSize(clippedSubText).x;

                // 左侧标题允许的最大宽度（严格限制在当前列 [col.Pos.x, col.Pos.x + col.Size.x] 之内）
                float maxTitleW = std::max(0.f, col.Size.x - subW - g_Nav.PaddingX * 2.f - 8.f);
                std::string clippedTitle = ClipTextToWidth(col.Title, maxTitleW);

                // 绘制当前列的底部信息
                GetWindowDrawList()->AddText({ footerPos.x + col.Size.x - subW - g_Nav.PaddingX, footerPos.y + (g_Nav.FooterHeight - g_Nav.CharHeight) * 0.5f }, g_Ctx.Style.Colors[GuiCol_TextDisabled], clippedSubText);
                GetWindowDrawList()->AddText({ footerPos.x + g_Nav.PaddingX, footerPos.y + (g_Nav.FooterHeight - g_Nav.CharHeight) * 0.5f }, g_Ctx.Style.Colors[GuiCol_Text], clippedTitle);

                // 外部滚动条自适应
                if (isActiveCol && col.TotalItems > g_Nav.MaxVisibleItems) {
                    float trackY = col.Pos.y + g_Nav.HeaderHeight;
                    float trackH = col.Size.y - g_Nav.HeaderHeight - g_Nav.FooterHeight;
                    float scrollbarW = 10.0f;
                    Vec2 trackPos = { col.Pos.x + col.Size.x + 4.f, trackY };

                    GetWindowDrawList()->AddRectFilled(trackPos, { scrollbarW, trackH }, g_Ctx.Style.Colors[GuiCol_FrameBg]);

                    float thumbH = std::max(28.f, (static_cast<float>(g_Nav.MaxVisibleItems) / col.TotalItems) * trackH);
                    float maxScroll = static_cast<float>(col.TotalItems - g_Nav.MaxVisibleItems);
                    float thumbY = trackY + (col.ScrollOffset / maxScroll) * (trackH - thumbH);

                    GetWindowDrawList()->AddRectFilled({ trackPos.x, thumbY }, { scrollbarW, thumbH }, g_Ctx.Style.Colors[GuiCol_SliderGrab]);
                }

                // 导航按键逻辑
                if (isActiveCol && g_Nav.State != NavState::BindingKey && !g_Nav.CP_Open) {
                    if (IsNavKey(VK_UP)) {
                        if (!col.SelectableIndices.empty()) {
                            auto it = std::lower_bound(col.SelectableIndices.begin(), col.SelectableIndices.end(), col.FocusIndex);
                            if (it == col.SelectableIndices.begin() || it == col.SelectableIndices.end()) {
                                col.FocusIndex = col.SelectableIndices.back();
                            }
                            else {
                                col.FocusIndex = *(--it);
                            }
                        }
                    }
                    if (IsNavKey(VK_DOWN)) {
                        if (!col.SelectableIndices.empty()) {
                            auto it = std::upper_bound(col.SelectableIndices.begin(), col.SelectableIndices.end(), col.FocusIndex);
                            if (it == col.SelectableIndices.end()) {
                                col.FocusIndex = col.SelectableIndices.front();
                            }
                            else {
                                col.FocusIndex = *it;
                            }
                        }
                    }

                    if (!g_Nav.CurrentItemConsumesLeftRight) {
                        if (colIdx == 0) {
                            if (IsNavKeyJustPressed(VK_LEFT) && g_Nav.TabCount > 0 && g_Nav.ColumnStack.size() == 1) {
                                g_Nav.ActiveTabIdx = (g_Nav.ActiveTabIdx - 1 + g_Nav.TabCount) % g_Nav.TabCount;
                                col.FocusIndex = 0;
                                col.ScrollOffset = 0;
                            }
                            if (IsNavKeyJustPressed(VK_RIGHT)) {
                                if (g_Nav.ColumnStack.size() > 1) {
                                    g_Nav.ActiveColumnIdx = 1;
                                }
                                else if (g_Nav.TabCount > 0) {
                                    g_Nav.ActiveTabIdx = (g_Nav.ActiveTabIdx + 1) % g_Nav.TabCount;
                                    col.FocusIndex = 0;
                                    col.ScrollOffset = 0;
                                }
                            }
                        }
                        else {
                            if (IsNavKeyJustPressed(VK_LEFT)) {
                                g_Nav.ActiveColumnIdx = colIdx - 1;
                            }
                            if (IsNavKeyJustPressed(VK_RIGHT)) {
                                if (colIdx + 1 < static_cast<int>(g_Nav.ColumnStack.size())) {
                                    g_Nav.ActiveColumnIdx = colIdx + 1;
                                }
                            }
                        }
                    }

                    if (IsNavKeyJustPressed(VK_TAB) && g_Nav.TabCount > 0) {
                        g_Nav.ActiveTabIdx = (g_Nav.ActiveTabIdx + 1) % g_Nav.TabCount;
                        col.FocusIndex = 0;
                        col.ScrollOffset = 0;
                        g_Nav.ColumnStack.resize(1);
                        g_Nav.ActiveColumnIdx = 0;
                    }

                    if (IsNavKeyJustPressed(VK_BACK)) {
                        if (g_Nav.ColumnStack.size() > 1) {
                            g_Nav.ColumnStack.pop_back();
                            g_Nav.ActiveColumnIdx = std::min(g_Nav.ActiveColumnIdx, static_cast<int>(g_Nav.ColumnStack.size()) - 1);
                        }
                    }
                }
            }

            // 2. 渲染 ColorPicker 调色盘
            if (g_Nav.CP_Open && g_Nav.CP_TargetR) {
                NavMenuColumn& lastCol = g_Nav.ColumnStack.back();
                float padding = g_Ctx.Style.CPPadding;
                float svSize = g_Ctx.Style.CPSVSize;
                float hueWidth = g_Ctx.Style.CPHueWidth;
                float alphaWidth = g_Ctx.Style.CPAlphaWidth;
                float spacing = g_Ctx.Style.CPSpacing;

                float hexBoxHeight = std::max(24.f, g_Ctx.ItemHeight + 4.f);
                float popupWidth = padding * 2.f + svSize + spacing * 2.f + hueWidth + (g_Nav.CP_TargetA ? (alphaWidth + spacing) : 0.f);
                float popupHeight = padding * 2.f + svSize + spacing + hexBoxHeight;

                Vec2 cpPos = { lastCol.Pos.x + lastCol.Size.x + 8.f, lastCol.Pos.y + g_Nav.HeaderHeight };
                Vec2 cpSize = { popupWidth, popupHeight };

                GetWindowDrawList()->AddRectFilled(cpPos, cpSize, g_Ctx.Style.Colors[GuiCol_PopupBg]);
                GetWindowDrawList()->AddRect(cpPos, cpSize, g_Ctx.Style.Colors[GuiCol_PopupBorder], 1.5f);

                Vec2 svPos = { cpPos.x + padding, cpPos.y + padding };
                Vec2 huePos = { svPos.x + svSize + spacing, svPos.y };
                Vec2 alphaPos = { huePos.x + hueWidth + spacing, svPos.y };
                Vec2 hexPos = { svPos.x, svPos.y + svSize + spacing };
                Vec2 hexSize = { popupWidth - padding * 2.f, hexBoxHeight };

                float pickerAlpha = g_Ctx.Style.Colors[GuiCol_ColorPickerLight].a;
                Color shadowCol = g_Ctx.Style.Colors[GuiCol_ColorPickerShadow];
                Color cbLight = g_Ctx.Style.Colors[GuiCol_CheckerboardLight];
                Color cbDark = g_Ctx.Style.Colors[GuiCol_CheckerboardDark];

                // SV 2D 方块
                int svSteps = std::max(2, static_cast<int>(svSize));
                float svStepSize = svSize / svSteps;
                for (int i = 0; i < svSteps; ++i) {
                    float s = (float)i / (svSteps - 1);
                    float pr, pg, pb;
                    HSVtoRGB(g_Ctx.ColorPickerH, s, 1.0f, pr, pg, pb);
                    GetWindowDrawList()->AddRectFilled({ svPos.x + i * svStepSize, svPos.y }, { svStepSize, svSize }, { pr, pg, pb, pickerAlpha });
                }
                for (int j = 0; j < svSteps; ++j) {
                    float v = 1.0f - (float)j / (svSteps - 1);
                    float drawAlpha = 1.0f - v;
                    GetWindowDrawList()->AddRectFilled({ svPos.x, svPos.y + j * svStepSize }, { svSize, svStepSize }, { shadowCol.r, shadowCol.g, shadowCol.b, drawAlpha * shadowCol.a });
                }

                // Hue 彩虹条
                int hueSteps = std::max(2, static_cast<int>(svSize));
                float hueStepSize = svSize / hueSteps;
                for (int i = 0; i < hueSteps; ++i) {
                    float h = (float)i / (hueSteps - 1);
                    float pr, pg, pb;
                    HSVtoRGB(h, 1.f, 1.f, pr, pg, pb);
                    GetWindowDrawList()->AddRectFilled({ huePos.x, huePos.y + i * hueStepSize }, { hueWidth, hueStepSize }, { pr, pg, pb, pickerAlpha });
                }

                // Alpha 棋盘与渐变
                if (g_Nav.CP_TargetA) {
                    int alphaSteps = std::max(2, static_cast<int>(svSize));
                    float alphaStepSize = svSize / alphaSteps;
                    int checkerSize = 5;
                    for (int i = 0; i < alphaSteps; ++i) {
                        float drawAlpha = 1.0f - (float)i / (alphaSteps - 1);
                        float drawY = alphaPos.y + i * alphaStepSize;
                        for (int x = 0; x < alphaWidth; x += checkerSize) {
                            float drawX = alphaPos.x + x;
                            float w = std::min((float)checkerSize, alphaWidth - x);
                            int checkY = static_cast<int>((drawY - alphaPos.y) / checkerSize);
                            int checkX = x / checkerSize;
                            bool isWhite = (checkX + checkY) % 2 == 0;
                            Color bgCol = isWhite ? cbLight : cbDark;

                            float finalR = *g_Nav.CP_TargetR * drawAlpha + bgCol.r * (1.f - drawAlpha);
                            float finalG = *g_Nav.CP_TargetG * drawAlpha + bgCol.g * (1.f - drawAlpha);
                            float finalB = *g_Nav.CP_TargetB * drawAlpha + bgCol.b * (1.f - drawAlpha);

                            GetWindowDrawList()->AddRectFilled({ drawX, drawY }, { w, alphaStepSize }, { finalR, finalG, finalB, pickerAlpha });
                        }
                    }
                }

                // Hex 输入框逻辑与渲染
                size_t hexId = HashString("CPHexInput");
                int hexFocusIdx = g_Nav.CP_TargetA ? 3 : 2;
                int totalCPFocus = hexFocusIdx + 1;

                if (g_Nav.CP_Focus == hexFocusIdx) {
                    g_Ctx.ActiveInputId = hexId;
                }
                else if (g_Ctx.ActiveInputId == hexId) {
                    g_Ctx.ActiveInputId = 0;
                }

                if (g_Ctx.ActiveInputId != hexId) {
                    uint8_t r8 = static_cast<uint8_t>(*g_Nav.CP_TargetR * 255.f);
                    uint8_t g8 = static_cast<uint8_t>(*g_Nav.CP_TargetG * 255.f);
                    uint8_t b8 = static_cast<uint8_t>(*g_Nav.CP_TargetB * 255.f);
                    uint8_t a8 = g_Nav.CP_TargetA ? static_cast<uint8_t>(*g_Nav.CP_TargetA * 255.f) : 255;
                    g_Ctx.InputBuffers[hexId] = std::format("{:02X}{:02X}{:02X}{:02X}", r8, g8, b8, a8);
                }

                bool hexChanged = InputTextEx(hexId, hexPos, hexSize, g_Ctx.InputBuffers[hexId], ShadowInputTextFlags_CharsHexadecimal | ShadowInputTextFlags_CharsUppercase, true);
                if (hexChanged) {
                    ApplyHexInput(hexId);
                }

                // 指示器光标
                Vec2 cursorSV = { svPos.x + g_Ctx.ColorPickerS * svSize, svPos.y + (1.f - g_Ctx.ColorPickerV) * svSize };
                GetWindowDrawList()->AddRect({ cursorSV.x - 4.f, cursorSV.y - 4.f }, { 8.f, 8.f }, g_Ctx.Style.Colors[GuiCol_ColorPickerDark]);
                GetWindowDrawList()->AddRect({ cursorSV.x - 3.f, cursorSV.y - 3.f }, { 6.f, 6.f }, g_Ctx.Style.Colors[GuiCol_ColorPickerLight]);
                GetWindowDrawList()->AddRectFilled({ cursorSV.x - 2.f, cursorSV.y - 2.f }, { 4.f, 4.f }, { *g_Nav.CP_TargetR, *g_Nav.CP_TargetG, *g_Nav.CP_TargetB, pickerAlpha });

                Vec2 cursorHue = { huePos.x - 2.f, huePos.y + g_Ctx.ColorPickerH * svSize - 2.f };
                GetWindowDrawList()->AddRect(cursorHue, { hueWidth + 4.f, 4.f }, g_Ctx.Style.Colors[GuiCol_ColorPickerDark]);
                GetWindowDrawList()->AddRect({ cursorHue.x + 1.f, cursorHue.y + 1.f }, { hueWidth + 2.f, 2.f }, g_Ctx.Style.Colors[GuiCol_ColorPickerLight]);

                if (g_Nav.CP_TargetA) {
                    Vec2 cursorAlpha = { alphaPos.x - 2.f, alphaPos.y + (1.f - *g_Nav.CP_TargetA) * svSize - 2.f };
                    GetWindowDrawList()->AddRect(cursorAlpha, { alphaWidth + 4.f, 4.f }, g_Ctx.Style.Colors[GuiCol_ColorPickerDark]);
                    GetWindowDrawList()->AddRect({ cursorAlpha.x + 1.f, cursorAlpha.y + 1.f }, { alphaWidth + 2.f, 2.f }, g_Ctx.Style.Colors[GuiCol_ColorPickerLight]);
                }

                // 焦点高亮边框
                if (g_Nav.CP_Focus == 0) GetWindowDrawList()->AddRect({ svPos.x - 2.f, svPos.y - 2.f }, { svSize + 4.f, svSize + 4.f }, g_Ctx.Style.Colors[GuiCol_TextHighlight], 1.5f);
                if (g_Nav.CP_Focus == 1) GetWindowDrawList()->AddRect({ huePos.x - 2.f, huePos.y - 2.f }, { hueWidth + 4.f, svSize + 4.f }, g_Ctx.Style.Colors[GuiCol_TextHighlight], 1.5f);
                if (g_Nav.CP_Focus == 2 && g_Nav.CP_TargetA) GetWindowDrawList()->AddRect({ alphaPos.x - 2.f, alphaPos.y - 2.f }, { alphaWidth + 4.f, svSize + 4.f }, g_Ctx.Style.Colors[GuiCol_TextHighlight], 1.5f);
                if (g_Nav.CP_Focus == hexFocusIdx) GetWindowDrawList()->AddRect({ hexPos.x - 2.f, hexPos.y - 2.f }, { hexSize.x + 4.f, hexSize.y + 4.f }, g_Ctx.Style.Colors[GuiCol_TextHighlight], 1.5f);

                // 按键逻辑
                if (IsNavKeyJustPressed(VK_TAB)) {
                    g_Nav.CP_Focus = (g_Nav.CP_Focus + 1) % totalCPFocus;
                    if (g_Nav.CP_Focus == hexFocusIdx) {
                        g_Ctx.ActiveInputId = hexId;
                        g_Ctx.InputCursorPos = static_cast<int>(g_Ctx.InputBuffers[hexId].size());
                        g_Ctx.InputSelectionStart = 0;
                        g_Ctx.InputSelectionEnd = static_cast<int>(g_Ctx.InputBuffers[hexId].size());
                    }
                    else {
                        if (g_Ctx.ActiveInputId == hexId) g_Ctx.ActiveInputId = 0;
                    }
                }

                bool isHexFocus = (g_Nav.CP_Focus == hexFocusIdx);

                // 当焦点在 Hex 输入框时，VK_BACK 仅用于文本退格删除，不关闭调色盘；非 Hex 焦点时 VK_BACK 正常作为返回键关闭调色盘
                if (IsNavKeyJustPressed(VK_ESCAPE) || (!isHexFocus && (IsNavKeyJustPressed(VK_BACK) || IsNavKeyJustPressed(VK_RETURN)))) {
                    g_Nav.CP_Open = false;
                    if (g_Ctx.ActiveInputId == hexId) g_Ctx.ActiveInputId = 0;
                }

                // 各轴调整步长
                static float moveStep = 0.005f;
                if (g_Nav.CP_Focus == 0) { // SV
                    float deltaS = GetNavKeyMoveDelta(VK_RIGHT, moveStep) - GetNavKeyMoveDelta(VK_LEFT, moveStep);
                    float deltaV = GetNavKeyMoveDelta(VK_UP, moveStep) - GetNavKeyMoveDelta(VK_DOWN, moveStep);

                    if (deltaS != 0.0f || deltaV != 0.0f) {
                        g_Ctx.ColorPickerS = std::clamp(g_Ctx.ColorPickerS + deltaS, 0.f, 1.f);
                        g_Ctx.ColorPickerV = std::clamp(g_Ctx.ColorPickerV + deltaV, 0.f, 1.f);
                        HSVtoRGB(g_Ctx.ColorPickerH, g_Ctx.ColorPickerS, g_Ctx.ColorPickerV, *g_Nav.CP_TargetR, *g_Nav.CP_TargetG, *g_Nav.CP_TargetB);
                    }
                }
                else if (g_Nav.CP_Focus == 1) { // Hue
                    float dec = std::max(GetNavKeyMoveDelta(VK_UP, moveStep), GetNavKeyMoveDelta(VK_LEFT, moveStep));
                    float inc = std::max(GetNavKeyMoveDelta(VK_DOWN, moveStep), GetNavKeyMoveDelta(VK_RIGHT, moveStep));
                    float deltaH = inc - dec;

                    if (deltaH != 0.0f) {
                        g_Ctx.ColorPickerH = std::clamp(g_Ctx.ColorPickerH + deltaH, 0.f, 1.f);
                        HSVtoRGB(g_Ctx.ColorPickerH, g_Ctx.ColorPickerS, g_Ctx.ColorPickerV, *g_Nav.CP_TargetR, *g_Nav.CP_TargetG, *g_Nav.CP_TargetB);
                    }
                }
                else if (g_Nav.CP_Focus == 2 && g_Nav.CP_TargetA) { // Alpha
                    float inc = std::max(GetNavKeyMoveDelta(VK_UP, moveStep), GetNavKeyMoveDelta(VK_RIGHT, moveStep));
                    float dec = std::max(GetNavKeyMoveDelta(VK_DOWN, moveStep), GetNavKeyMoveDelta(VK_LEFT, moveStep));
                    float deltaA = inc - dec;

                    if (deltaA != 0.0f) {
                        *g_Nav.CP_TargetA = std::clamp(*g_Nav.CP_TargetA + deltaA, 0.f, 1.f);
                    }
                }
            }

            CheckAndDrawErrors();

            g_Ctx.CurrentWindow = nullptr;
        }
    }
}