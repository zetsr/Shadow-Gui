/*
Credit:
    https://github.com/ocornut/imgui
    https://github.com/Encryqed/Dumper-7
    https://aistudio.google.com/
*/

#pragma once
#include <windows.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <format>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <cmath>
#include <bit>
#include <charconv>
#include <array>
#include <ranges>
#include <cstdio>

#include "../../CppSDK/SDK.hpp"

namespace Shadow {
    enum class HotkeyMode {
        None,
        HoldOn,
        ToggleOn,
        HoldOff,
        AlwaysOn
    };

    struct Vec2 { float x, y; };
    struct Color { float r, g, b, a; };

    // 颜色枚举与样式
    enum GuiCol_ {
        GuiCol_WindowBg,
        GuiCol_TitleBarBg,
        GuiCol_Text,
        GuiCol_TextDisabled,
        GuiCol_TextHighlight,
        GuiCol_Button,
        GuiCol_ButtonHovered,
        GuiCol_FrameBg,
        GuiCol_FrameBgHovered,
        GuiCol_SliderGrab,
        GuiCol_SliderKnob,
        GuiCol_CheckMark,
        GuiCol_Separator,
        GuiCol_PopupBg,
        GuiCol_PopupBorder,
        GuiCol_ResizeGrip,
        GuiCol_ResizeGripHovered,
        GuiCol_ResizeGripActive,
        GuiCol_Tab,
        GuiCol_TabHovered,
        GuiCol_TabActive,
        GuiCol_ActiveIndicator,
        GuiCol_InactiveIndicator,
        GuiCol_Border,
        GuiCol_ErrorText,
        GuiCol_TextShadow,
        GuiCol_TextOutline,
        GuiCol_ColorPickerDark,
        GuiCol_ColorPickerLight,
        GuiCol_CheckerboardLight,
        GuiCol_CheckerboardDark,
        GuiCol_ColorPickerShadow,
        GuiCol_ControlDisabled,
        GuiCol_SwitchBg,
        GuiCol_SwitchBgHovered,
        GuiCol_SwitchBgActive,
        GuiCol_SwitchBgActiveHovered,
        GuiCol_SwitchKnob,
        GuiCol_DropdownActive,

        GuiCol_COUNT
    };

    enum ShadowWindowFlags_ {
        ShadowWindowFlags_None = 0,
        ShadowWindowFlags_NoResize = 1 << 0,
        ShadowWindowFlags_NoMove = 1 << 1,
        ShadowWindowFlags_NoScrollbar = 1 << 2,
        ShadowWindowFlags_NoTitleBar = 1 << 5,
        ShadowWindowFlags_NoMouseInputs = 1 << 6,

        ShadowWindowFlags_TextAlignLeft = 0,
        ShadowWindowFlags_TextAlignCenter = 1 << 3,
        ShadowWindowFlags_TextAlignRight = 1 << 4,
    };
    using ShadowWindowFlags = int;

    enum ShadowTabBarFlags_ {
        ShadowTabBarFlags_None = 0,
        ShadowTabBarFlags_Reorderable = 1 << 0,
        ShadowTabBarFlags_FittingPolicyScroll = 1 << 1,
        ShadowTabBarFlags_NoScrollbar = 1 << 2,
    };
    using ShadowTabBarFlags = int;

    enum ShadowColorPickerFlags_ {
        ShadowColorPickerFlags_None = 0,
        ShadowColorPickerFlags_NoText = 1 << 0,
        ShadowColorPickerFlags_NoRightAlign = 1 << 1,
    };
    using ShadowColorPickerFlags = int;

    enum ShadowSliderFlags_ {
        ShadowSliderFlags_None = 0,
        ShadowSliderFlags_NoText = 1 << 0,
        ShadowSliderFlags_NoRightAlign = 1 << 1,
    };
    using ShadowSliderFlags = int;

    enum ShadowHotkeyFlags_ {
        ShadowHotkeyFlags_None = 0,
        ShadowHotkeyFlags_NoText = 1 << 0,
        ShadowHotkeyFlags_NoRightAlign = 1 << 1,
        ShadowHotkeyFlags_NoStateDisplay = 1 << 2,
    };
    using ShadowHotkeyFlags = int;

    enum ShadowComboFlags_ {
        ShadowComboFlags_None = 0,
        ShadowComboFlags_NoText = 1 << 0,
        ShadowComboFlags_NoRightAlign = 1 << 1,
        ShadowComboFlags_FitText = 1 << 2,
    };
    using ShadowComboFlags = int;

    enum ShadowHoveredFlags_ {
        ShadowHoveredFlags_None = 0,
        ShadowHoveredFlags_AllowWhenBlockedByPopup = 1 << 0,
        ShadowHoveredFlags_AllowWhenBlockedByActiveItem = 1 << 1,
        ShadowHoveredFlags_AllowWhenDisabled = 1 << 2,
        ShadowHoveredFlags_Stationary = 1 << 3,
        ShadowHoveredFlags_DelayNone = 1 << 4,
        ShadowHoveredFlags_DelayShort = 1 << 5,
        ShadowHoveredFlags_DelayNormal = 1 << 6,
        ShadowHoveredFlags_NoSharedDelay = 1 << 7,
    };
    using ShadowHoveredFlags = int;

    enum ShadowTreeNodeFlags_ {
        ShadowTreeNodeFlags_None = 0,
        ShadowTreeNodeFlags_DefaultOpen = 1 << 0,
        ShadowTreeNodeFlags_Framed = 1 << 1,
        ShadowTreeNodeFlags_FitText = 1 << 2,
        ShadowTreeNodeFlags_NoIndent = 1 << 3
    };
    using ShadowTreeNodeFlags = int;

    enum ShadowInputTextFlags_ {
        ShadowInputTextFlags_None = 0,
        ShadowInputTextFlags_CharsDecimal = 1 << 0,
        ShadowInputTextFlags_CharsHexadecimal = 1 << 1,
        ShadowInputTextFlags_CharsScientific = 1 << 2,
        ShadowInputTextFlags_CharsUppercase = 1 << 3,
        ShadowInputTextFlags_CharsNoBlank = 1 << 4,
        ShadowInputTextFlags_EscapeClearsAll = 1 << 5,
        ShadowInputTextFlags_ReadOnly = 1 << 6,
        ShadowInputTextFlags_Password = 1 << 7,
        ShadowInputTextFlags_AutoSelectAll = 1 << 8,
        ShadowInputTextFlags_ParseEmptyRefVal = 1 << 9,
        ShadowInputTextFlags_DisplayEmptyRefVal = 1 << 10,
        ShadowInputTextFlags_NoName = 1 << 11
    };
    using ShadowInputTextFlags = int;

    struct GuiStyle {
        Color Colors[GuiCol_COUNT];

        // 尺寸与间距标志配置
        Vec2 WindowPadding = { 16.f, 16.f }; // 窗口内边距
        Vec2 FramePadding = { 8.f, 2.f };    // 控件内文本与边缘的间距 (X为水平边距, Y为垂直居中补偿)
        Vec2 ItemSpacing = { 10.f, 8.f };    // 控件之间的水平与垂直间距
        float ScrollbarSize = 10.f;          // 滚动条宽度
        float ScrollbarMargin = 4.f;         // 滚动条与右侧边缘的间距
        float ResizeGripSize = 16.f;         // 右下角缩放手柄尺寸
        float TabExtraWidth = 20.f;          // Tab标签额外预留宽度

        // 控件特定配置
        float ControlOffsetMin = 120.f;      // 右侧互动控件对齐的最小偏移量
        float ControlOffsetRatio = 0.4f;     // 右侧互动控件偏移量占窗口宽度的比例

        // ColorPicker 弹出层专属配置
        float CPPadding = 8.f;
        float CPSVSize = 200.f;
        float CPHueWidth = 20.f;
        float CPAlphaWidth = 20.f;
        float CPSpacing = 8.f;

        // 窗口最小尺寸 和 FontScaleDpi
        Vec2 WindowMinSize = { 200.f, 150.f };
        float FontScaleDpi = 1.0f;           // 默认 1.0，用于文本与菜单自适应
    };

    struct TabDisplayInfo {
        size_t id;
        Vec2 pos;
        Vec2 size;
        std::string display;
        SDK::UFont* font = nullptr;
        float fontScale = 1.0f;
    };

    struct FontContext {
        SDK::UFont* Font;
        float Scale;
    };

    enum class ShadowDrawCmdType {
        Line,
        Rect,
        RectFilled,
        Text,
        TriangleFilled
    };

    struct ShadowDrawCmd {
        ShadowDrawCmdType type;
        Vec2 pos;
        Vec2 size;
        Color color;
        float thickness;
        std::string text;
        SDK::UFont* font;
        float fontScale;
        bool clippingEnabled;
        Vec2 clipMin;
        Vec2 clipMax;
        Vec2 p1, p2, p3;
    };

    struct ListBoxState {
        Vec2 ParentWindowPos;
        Vec2 ParentWindowSize;
        Vec2 ParentCursor;
        float ParentContentStartY;
        float ParentScrollY;
        float ParentContentHeight;
        bool ParentIsScrollApplied;
        ShadowWindowFlags ParentWindowFlags;
        float ParentCurrentScrollbarWidth;
        Vec2 ParentWindowPadding;
        float ParentIndentX;

        size_t Id;
        Vec2 Pos;
        Vec2 Size;
    };

    struct GuiContext {
        SDK::UCanvas* Canvas = nullptr;
        SDK::UFont* DefaultFont = nullptr;

        GuiStyle Style;
        bool StyleInitialized = false;

        Vec2 MousePos = { 0.f, 0.f };
        bool MouseDown = false;
        bool MouseClicked = false;
        bool RightMouseDown = false;
        bool RightMouseClicked = false;
        float MouseWheel = 0.f;

        bool KeyStates[256] = { false };
        bool HotkeyToggles[256] = { false };
        bool KeyPressed[256] = { false };
        int* AssigningHotkey = nullptr;

        size_t ActiveDropdownId = 0;
        int* DropdownCurrentItem = nullptr;
        std::vector<std::string> DropdownItems;
        Vec2 DropdownPos = { 0.f, 0.f };
        Vec2 DropdownSize = { 0.f, 0.f };

        Vec2 WindowPos = { 100.f, 100.f };
        Vec2 WindowSize = { 500.f, 400.f };
        bool IsDragging = false;
        Vec2 DragOffset = { 0.f, 0.f };

        bool HasWindowSizeConstraints = false;
        Vec2 WindowSizeConstraintMin = { 0.f, 0.f };
        Vec2 WindowSizeConstraintMax = { 10000.f, 10000.f };

        std::vector<FontContext> FontStack;

        int BeginStack = 0;
        int TabBarStack = 0;
        int TabItemStack = 0;
        int TreeNodeStack = 0;
        std::string LastErrorMsg;

        bool IsResizing = false;
        Vec2 ResizeStartPos = { 0.f, 0.f };
        Vec2 ResizeStartSize = { 0.f, 0.f };
        bool IsHoveringResize = false;

        bool ClippingEnabled = false;
        Vec2 ClipMin = { 0.f, 0.f };
        Vec2 ClipMax = { 0.f, 0.f };
        std::vector<std::pair<Vec2, Vec2>> ClipStack;

        Vec2 Cursor = { 0.f, 0.f };
        float ItemHeight = 20.f;

        float ScrollY = 0.f;
        float ContentHeight = 0.f;
        bool IsDraggingScrollbar = false;
        float ScrollDragOffset = 0.f;
        float ContentStartY = 0.f;
        bool IsScrollApplied = false;

        size_t ActiveTabId = 0;
        size_t CurrentTabId = 0;
        bool InActiveTab = true;
        Vec2 TabCursor = { 0.f, 0.f };

        size_t ActiveColorPickerId = 0;
        float* ColorPickerR = nullptr;
        float* ColorPickerG = nullptr;
        float* ColorPickerB = nullptr;
        float* ColorPickerA = nullptr;
        Vec2 ColorPickerPos = { 0.f, 0.f };
        float ColorPickerH = 0.f;
        float ColorPickerS = 0.f;
        float ColorPickerV = 0.f;

        bool IsDraggingSV = false;
        bool IsDraggingHue = false;
        bool IsDraggingAlpha = false;
        bool IsDraggingColorPicker = false;
        Vec2 ColorPickerDragOffset = { 0.f, 0.f };
        bool IsTypingHex = false;
        std::string HexInputBuffer;

        size_t DraggingSliderId = 0;
        size_t FocusedSliderId = 0;

        size_t ActiveInputId = 0;
        int InputCursorPos = 0;
        int InputSelectionStart = -1;
        int InputSelectionEnd = -1;
        std::vector<char> InputChars;
        std::unordered_map<size_t, std::string> InputBuffers;

        std::unordered_map<size_t, float> SliderInputWidthCache;
        std::unordered_map<size_t, size_t> SliderInputLengthCache;

        ShadowWindowFlags CurrentWindowFlags = ShadowWindowFlags_None;
        ShadowTabBarFlags CurrentTabBarFlags = ShadowTabBarFlags_None;
        size_t CurrentTabBarId = 0;

        std::unordered_map<size_t, std::vector<size_t>> TabOrderMap;
        std::unordered_map<size_t, std::vector<size_t>> TabAppearedThisFrame;

        std::unordered_map<size_t, std::vector<TabDisplayInfo>> TabBarDisplayCache;
        std::unordered_map<size_t, std::unordered_map<size_t, float>> TabBarLayoutX;
        std::unordered_map<size_t, float> TabWidthCache;

        size_t DraggingTabId = 0;
        size_t DraggingTabBarId = 0;
        float DraggingTabGrabOffsetX = 0.f;
        float DraggingTabCurrentX = 0.f;

        std::unordered_map<size_t, float> TabBarScrollX;
        size_t DraggingTabBarScrollId = 0;
        float TabBarScrollDragOffset = 0.f;

        Vec2 TabBarOrigin = { 0.f, 0.f };
        float TabBarViewWidth = 0.f;
        float TabBarContentWidth = 0.f;
        float TabBarContentWidthAccum = 0.f;
        bool TabBarNeedsScrollbar = false;

        std::vector<std::pair<Vec2, Vec2>> TabBarHoverRects;
        std::vector<std::pair<Vec2, Vec2>> TabBarHoverRectsPending;

        float LastItemMaxX = 0.f;
        std::vector<bool> DisabledStack;

        Vec2 LastItemMin = { 0.f, 0.f };
        Vec2 LastItemMax = { 0.f, 0.f };
        bool LastItemDisabled = false;
        size_t LastItemId = 0;

        size_t HoveredIdPreviousFrame = 0;
        size_t HoveredIdCurrentFrame = 0;
        size_t LastHoveredIdEval = 0;
        uint64_t HoveredIdTimerStart = 0;
        bool HoveredIdStationaryTriggered = false;
        bool HoveredIdDelayTriggered = false;

        Vec2 MousePosPrev = { 0.f, 0.f };
        uint64_t MouseStationaryStartTime = 0;
        bool MouseIsStationary = false;

        uint64_t SharedDelayExpirationTime = 0;
        bool SharedDelayActive = false;

        int WidgetCount = 0;

        std::unordered_map<size_t, Vec2> TooltipSizeCache;
        Vec2 CurrentTooltipSize = { 0.f, 0.f };
        bool InTooltip = false;

        Vec2 BackupWindowPos = { 0.f, 0.f };
        Vec2 BackupWindowSize = { 0.f, 0.f };
        Vec2 BackupCursor = { 0.f, 0.f };
        float BackupContentStartY = 0.f;
        float BackupScrollY = 0.f;
        float BackupLastItemMaxX = 0.f;
        bool BackupIsScrollApplied = false;
        ShadowWindowFlags BackupCurrentWindowFlags = ShadowWindowFlags_None;

        std::vector<ShadowDrawCmd> TooltipDrawCommands;

        float CurrentScrollbarWidth = 0.f;

        std::vector<float> TextWrapPosStack;

        std::unordered_map<size_t, bool> TreeNodeOpenStates;
        float IndentX = 0.f;
        float BackupIndentX = 0.f;
        std::vector<bool> TreeNodeNoIndentStack;

        bool BackupClippingEnabled = false;
        Vec2 BackupClipMin = { 0.f, 0.f };
        Vec2 BackupClipMax = { 0.f, 0.f };
        std::vector<std::pair<Vec2, Vec2>> BackupClipStack;

        int ListBoxStack = 0;
        std::vector<ListBoxState> ListBoxStateStack;
        std::unordered_map<size_t, float> ListBoxScrollY;
        std::unordered_map<size_t, float> ListBoxContentHeight;
        size_t DraggingListBoxScrollId = 0;
        float ListBoxScrollDragOffset = 0.f;

        size_t HoveredListBoxIdCurrentFrame = 0;
        size_t HoveredListBoxIdPreviousFrame = 0;
    };

    inline GuiContext g_Ctx;
    inline SDK::UFont*& DefaultFont = g_Ctx.DefaultFont;

    inline GuiStyle& GetStyle() {
        return g_Ctx.Style;
    }

    // 深海主题
    inline void StyleColorsOcean() {
        auto& colors = g_Ctx.Style.Colors;

        colors[GuiCol_WindowBg] = { 0.015f, 0.020f, 0.028f, 0.960f };
        colors[GuiCol_PopupBg] = { 0.012f, 0.016f, 0.024f, 0.980f };
        colors[GuiCol_TitleBarBg] = { 0.008f, 0.012f, 0.016f, 1.000f };

        colors[GuiCol_Text] = { 0.180f, 0.210f, 0.260f, 1.000f };
        colors[GuiCol_TextHighlight] = { 0.700f, 0.740f, 0.800f, 1.000f };
        colors[GuiCol_TextDisabled] = { 0.080f, 0.100f, 0.130f, 1.000f };

        colors[GuiCol_FrameBg] = { 0.022f, 0.028f, 0.038f, 1.000f };
        colors[GuiCol_FrameBgHovered] = { 0.035f, 0.045f, 0.060f, 1.000f };

        colors[GuiCol_Button] = { 0.025f, 0.032f, 0.045f, 1.000f };
        colors[GuiCol_ButtonHovered] = { 0.045f, 0.055f, 0.075f, 1.000f };

        colors[GuiCol_Tab] = { 0.008f, 0.012f, 0.016f, 1.000f };
        colors[GuiCol_TabHovered] = { 0.035f, 0.045f, 0.060f, 1.000f };
        colors[GuiCol_TabActive] = { 0.015f, 0.020f, 0.028f, 1.000f };

        colors[GuiCol_SliderGrab] = { 0.140f, 0.185f, 0.250f, 1.000f };
        colors[GuiCol_SliderKnob] = { 0.860f, 0.860f, 0.860f, 1.000f };

        colors[GuiCol_CheckMark] = { 0.520f, 0.220f, 0.220f, 1.000f };
        colors[GuiCol_ActiveIndicator] = { 0.520f, 0.220f, 0.220f, 1.000f };
        colors[GuiCol_InactiveIndicator] = { 0.035f, 0.045f, 0.060f, 1.000f };

        colors[GuiCol_Border] = { 0.030f, 0.040f, 0.055f, 0.600f };
        colors[GuiCol_PopupBorder] = { 0.035f, 0.045f, 0.060f, 0.900f };
        colors[GuiCol_Separator] = { 0.025f, 0.032f, 0.045f, 1.000f };

        colors[GuiCol_ResizeGrip] = { 0.025f, 0.032f, 0.045f, 1.000f };
        colors[GuiCol_ResizeGripActive] = { 0.520f, 0.220f, 0.220f, 1.000f };
        colors[GuiCol_ResizeGripHovered] = { 0.300f, 0.130f, 0.130f, 1.000f };

        colors[GuiCol_ErrorText] = { 1.000f, 0.196f, 0.196f, 1.000f };
        colors[GuiCol_TextShadow] = { 0.000f, 0.000f, 0.000f, 1.000f };
        colors[GuiCol_TextOutline] = { 0.000f, 0.000f, 0.000f, 0.000f };
        colors[GuiCol_ColorPickerDark] = { 0.000f, 0.000f, 0.000f, 1.000f };
        colors[GuiCol_ColorPickerLight] = { 1.000f, 1.000f, 1.000f, 1.000f };

        colors[GuiCol_CheckerboardLight] = { 1.000f, 1.000f, 1.000f, 1.000f };
        colors[GuiCol_CheckerboardDark] = { 0.700f, 0.700f, 0.700f, 1.000f };
        colors[GuiCol_ColorPickerShadow] = { 0.000f, 0.000f, 0.000f, 1.000f };

        colors[GuiCol_ControlDisabled] = { 0.022f, 0.028f, 0.038f, 0.500f };

        colors[GuiCol_SwitchBg] = { 0.022f, 0.028f, 0.038f, 1.000f };
        colors[GuiCol_SwitchBgHovered] = { 0.035f, 0.045f, 0.060f, 1.000f };
        colors[GuiCol_SwitchBgActive] = { 0.520f, 0.220f, 0.220f, 1.000f };
        colors[GuiCol_SwitchBgActiveHovered] = { 0.620f, 0.270f, 0.270f, 1.000f };
        colors[GuiCol_SwitchKnob] = { 0.750f, 0.750f, 0.750f, 1.000f };

        colors[GuiCol_DropdownActive] = { 0.028f, 0.036f, 0.049f, 1.000f };
    }

    // 紫曜主题
    inline void StyleColorsAmethyst() {
        auto& colors = g_Ctx.Style.Colors;

        colors[GuiCol_WindowBg] = { 0.058f, 0.042f, 0.078f, 0.940f };
        colors[GuiCol_PopupBg] = { 0.070f, 0.050f, 0.095f, 0.970f };
        colors[GuiCol_TitleBarBg] = { 0.040f, 0.028f, 0.058f, 1.000f };

        colors[GuiCol_Text] = { 0.860f, 0.830f, 0.900f, 1.000f };
        colors[GuiCol_TextHighlight] = { 0.960f, 0.940f, 0.980f, 1.000f };
        colors[GuiCol_TextDisabled] = { 0.400f, 0.370f, 0.440f, 1.000f };

        colors[GuiCol_FrameBg] = { 0.100f, 0.075f, 0.135f, 0.850f };
        colors[GuiCol_FrameBgHovered] = { 0.140f, 0.100f, 0.185f, 0.900f };

        colors[GuiCol_Button] = { 0.115f, 0.085f, 0.155f, 0.880f };
        colors[GuiCol_ButtonHovered] = { 0.165f, 0.120f, 0.220f, 0.920f };

        colors[GuiCol_Tab] = { 0.048f, 0.034f, 0.068f, 1.000f };
        colors[GuiCol_TabHovered] = { 0.135f, 0.098f, 0.185f, 0.900f };
        colors[GuiCol_TabActive] = { 0.090f, 0.065f, 0.125f, 1.000f };

        colors[GuiCol_SliderGrab] = { 0.430f, 0.280f, 0.620f, 1.000f };
        colors[GuiCol_SliderKnob] = { 0.860f, 0.860f, 0.860f, 1.000f };

        colors[GuiCol_CheckMark] = { 0.560f, 0.360f, 0.780f, 1.000f };
        colors[GuiCol_ActiveIndicator] = { 0.560f, 0.360f, 0.780f, 1.000f };
        colors[GuiCol_InactiveIndicator] = { 0.130f, 0.095f, 0.175f, 1.000f };

        colors[GuiCol_Border] = { 0.220f, 0.160f, 0.310f, 0.450f };
        colors[GuiCol_PopupBorder] = { 0.260f, 0.190f, 0.360f, 0.700f };
        colors[GuiCol_Separator] = { 0.115f, 0.085f, 0.155f, 0.600f };

        colors[GuiCol_ResizeGrip] = { 0.115f, 0.085f, 0.155f, 0.500f };
        colors[GuiCol_ResizeGripActive] = { 0.560f, 0.360f, 0.780f, 1.000f };
        colors[GuiCol_ResizeGripHovered] = { 0.400f, 0.260f, 0.560f, 0.850f };

        colors[GuiCol_ErrorText] = { 1.000f, 0.196f, 0.196f, 1.000f };
        colors[GuiCol_TextShadow] = { 0.000f, 0.000f, 0.000f, 1.000f };
        colors[GuiCol_TextOutline] = { 0.000f, 0.000f, 0.000f, 0.000f };
        colors[GuiCol_ColorPickerDark] = { 0.000f, 0.000f, 0.000f, 1.000f };
        colors[GuiCol_ColorPickerLight] = { 1.000f, 1.000f, 1.000f, 1.000f };

        colors[GuiCol_CheckerboardLight] = { 1.000f, 1.000f, 1.000f, 1.000f };
        colors[GuiCol_CheckerboardDark] = { 0.700f, 0.700f, 0.700f, 1.000f };
        colors[GuiCol_ColorPickerShadow] = { 0.000f, 0.000f, 0.000f, 1.000f };

        colors[GuiCol_ControlDisabled] = { 0.100f, 0.075f, 0.135f, 0.450f };

        colors[GuiCol_SwitchBg] = { 0.100f, 0.075f, 0.135f, 0.850f };
        colors[GuiCol_SwitchBgHovered] = { 0.140f, 0.100f, 0.185f, 0.900f };
        colors[GuiCol_SwitchBgActive] = { 0.560f, 0.360f, 0.780f, 1.000f };
        colors[GuiCol_SwitchBgActiveHovered] = { 0.660f, 0.460f, 0.880f, 1.000f };
        colors[GuiCol_SwitchKnob] = { 0.750f, 0.750f, 0.750f, 1.000f };

        colors[GuiCol_DropdownActive] = { 0.120f, 0.088f, 0.160f, 0.875f };
    }

    inline Vec2 GetWindowSize() {
        return g_Ctx.WindowSize;
    }

    inline Vec2 GetWindowPos() {
        return g_Ctx.WindowPos;
    }

    inline void SetWindowPos(Vec2 pos) {
        g_Ctx.WindowPos = pos;
    }

    inline void SetNextWindowSizeConstraints(Vec2 min_size, Vec2 max_size) {
        g_Ctx.HasWindowSizeConstraints = true;
        g_Ctx.WindowSizeConstraintMin = min_size;
        g_Ctx.WindowSizeConstraintMax = max_size;
    }

    // 允许放行透传给游戏的按键列表
    inline std::vector<int> AllowedKeys;
    inline void SetAllowedKeys(const std::vector<int>& keys) {
        AllowedKeys = keys;
    }

    // 允许放行透传给游戏的鼠标消息列表
    inline std::vector<UINT> AllowedMouseMsgs;
    inline void SetAllowedMouseMsgs(const std::vector<UINT>& msgs) {
        AllowedMouseMsgs = msgs;
    }

    inline bool IsKeyAllowed(int key) {
        return std::find(AllowedKeys.begin(), AllowedKeys.end(), key) != AllowedKeys.end();
    }

    inline bool IsMouseMsgAllowed(UINT uMsg) {
        // 先判断它到底是不是鼠标相关消息
        bool isMouseMsg = (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || (uMsg == WM_INPUT);

        // 如果不是鼠标消息，直接返回 false
        if (!isMouseMsg) {
            return false;
        }

        return std::find(AllowedMouseMsgs.begin(), AllowedMouseMsgs.end(), uMsg) != AllowedMouseMsgs.end();
    }

    inline constexpr size_t HashString(std::string_view str) noexcept {
        size_t hash = 14695981039346656037ULL;
        for (char c : str) {
            hash ^= static_cast<size_t>(c);
            hash *= 1099511628211ULL;
        }
        return hash;
    }

    inline void ParseLabel(std::string_view raw, std::string_view& display_name, size_t& id) {
        id = HashString(raw);
        size_t pos = raw.find("##");
        display_name = (pos != std::string_view::npos) ? raw.substr(0, pos) : raw;
    }

    inline std::wstring ToWString(std::string_view utf8_str) {
        if (utf8_str.empty()) return L"";
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8_str.data(), static_cast<int>(utf8_str.size()), nullptr, 0);
        std::wstring wstr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, utf8_str.data(), static_cast<int>(utf8_str.size()), &wstr[0], size_needed);
        return wstr;
    }

    // VK键码转字符串名称
    inline std::string GetKeyName(int vk) {
        if (vk == 0) return "None";
        switch (vk) {
        case VK_LBUTTON: return "Mouse 1";
        case VK_RBUTTON: return "Mouse 2";
        case VK_MBUTTON: return "Mouse 3";
        case VK_XBUTTON1: return "Mouse 4";
        case VK_XBUTTON2: return "Mouse 5";
        case VK_BACK: return "Backspace";
        case VK_TAB: return "Tab";
        case VK_RETURN: return "Enter";
        case VK_SHIFT: return "Shift";
        case VK_CONTROL: return "Ctrl";
        case VK_MENU: return "Alt";
        case VK_CAPITAL: return "Caps Lock";
        case VK_ESCAPE: return "Esc";
        case VK_SPACE: return "Space";
        case VK_PRIOR: return "Page Up";
        case VK_NEXT: return "Page Down";
        case VK_END: return "End";
        case VK_HOME: return "Home";
        case VK_LEFT: return "Left";
        case VK_UP: return "Up";
        case VK_RIGHT: return "Right";
        case VK_DOWN: return "Down";
        case VK_INSERT: return "Insert";
        case VK_DELETE: return "Delete";
        case VK_LWIN: return "LWin";
        case VK_RWIN: return "RWin";
        case VK_NUMPAD0: return "Num 0";
        case VK_NUMPAD1: return "Num 1";
        case VK_NUMPAD2: return "Num 2";
        case VK_NUMPAD3: return "Num 3";
        case VK_NUMPAD4: return "Num 4";
        case VK_NUMPAD5: return "Num 5";
        case VK_NUMPAD6: return "Num 6";
        case VK_NUMPAD7: return "Num 7";
        case VK_NUMPAD8: return "Num 8";
        case VK_NUMPAD9: return "Num 9";
        case VK_MULTIPLY: return "Num *";
        case VK_ADD: return "Num +";
        case VK_SUBTRACT: return "Num -";
        case VK_DECIMAL: return "Num .";
        case VK_DIVIDE: return "Num /";
        case VK_F1: return "F1";
        case VK_F2: return "F2";
        case VK_F3: return "F3";
        case VK_F4: return "F4";
        case VK_F5: return "F5";
        case VK_F6: return "F6";
        case VK_F7: return "F7";
        case VK_F8: return "F8";
        case VK_F9: return "F9";
        case VK_F10: return "F10";
        case VK_F11: return "F11";
        case VK_F12: return "F12";
        case VK_LSHIFT: return "LShift";
        case VK_RSHIFT: return "RShift";
        case VK_LCONTROL: return "LCtrl";
        case VK_RCONTROL: return "RCtrl";
        case VK_LMENU: return "LAlt";
        case VK_RMENU: return "RAlt";
        }
        if (vk >= '0' && vk <= '9') return std::string(1, static_cast<char>(vk));
        if (vk >= 'A' && vk <= 'Z') return std::string(1, static_cast<char>(vk));
        return std::format("Unk {:X}", vk);
    }

    inline void RGBtoHSV(float r, float g, float b, float& h, float& s, float& v) {
        float max_val = std::max({ r, g, b });
        float min_val = std::min({ r, g, b });
        v = max_val;
        float delta = max_val - min_val;
        if (max_val > 0.0f) { s = delta / max_val; }
        else { s = 0.0f; h = 0.0f; return; }
        if (delta == 0.0f) { h = 0.0f; return; }
        if (r >= max_val) { h = (g - b) / delta; }
        else if (g >= max_val) { h = 2.0f + (b - r) / delta; }
        else { h = 4.0f + (r - g) / delta; }
        h *= 60.0f;
        if (h < 0.0f) h += 360.0f;
        h /= 360.0f;
    }

    inline void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b) {
        if (s <= 0.0f) { r = g = b = v; return; }
        h *= 360.0f;
        if (h >= 360.0f) h = 0.0f;
        h /= 60.0f;
        int i = static_cast<int>(h);
        float ff = h - i;
        float p = v * (1.0f - s);
        float q = v * (1.0f - (s * ff));
        float t = v * (1.0f - (s * (1.0f - ff)));
        switch (i) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: default: r = v; g = p; b = q; break;
        }
    }

    inline bool IsMouseHoveringRaw(Vec2 pos, Vec2 size) {
        return g_Ctx.MousePos.x >= pos.x && g_Ctx.MousePos.x <= pos.x + size.x &&
            g_Ctx.MousePos.y >= pos.y && g_Ctx.MousePos.y <= pos.y + size.y;
    }

    inline bool IsMouseHovering(Vec2 pos, Vec2 size) {
        // 如果当前窗口禁用了鼠标输入，直接返回 false
        if (g_Ctx.CurrentWindowFlags & ShadowWindowFlags_NoMouseInputs) {
            return false;
        }

        if (g_Ctx.ActiveDropdownId != 0) {
            float dropWidth = g_Ctx.DropdownSize.x;
            float dropHeight = g_Ctx.DropdownItems.size() * g_Ctx.ItemHeight;
            if (IsMouseHoveringRaw({ g_Ctx.DropdownPos.x - 1.f, g_Ctx.DropdownPos.y - 1.f }, { dropWidth + 2.f, dropHeight + 2.f })) {
                return false;
            }
        }

        if (g_Ctx.ActiveColorPickerId != 0) {
            Vec2 popupPos = g_Ctx.ColorPickerPos;
            float hexBoxHeight = std::max(24.f, g_Ctx.ItemHeight + 4.f);
            float popupWidth = g_Ctx.Style.CPPadding * 2.f + g_Ctx.Style.CPSVSize + g_Ctx.Style.CPSpacing * 2.f + g_Ctx.Style.CPHueWidth + g_Ctx.Style.CPAlphaWidth;
            float popupHeight = g_Ctx.Style.CPPadding * 2.f + g_Ctx.Style.CPSVSize + g_Ctx.Style.CPSpacing + hexBoxHeight;

            if (IsMouseHoveringRaw({ popupPos.x - 2.f, popupPos.y - 2.f }, { popupWidth + 4.f, popupHeight + 4.f })) {
                return false;
            }
        }
        return IsMouseHoveringRaw(pos, size);
    }

    inline bool TryAssignKey(int vk) {
        if (g_Ctx.AssigningHotkey) {
            if (vk == VK_ESCAPE) {
                *g_Ctx.AssigningHotkey = 0;
            }
            else {
                *g_Ctx.AssigningHotkey = vk;
            }
            g_Ctx.AssigningHotkey = nullptr;
            return true;
        }
        return false;
    }

    inline bool HandleKeyDown(int vk, bool isRepeat = false) {
        if (TryAssignKey(vk)) return true;
        if (!g_Ctx.KeyStates[vk]) {
            g_Ctx.HotkeyToggles[vk] = !g_Ctx.HotkeyToggles[vk];
            g_Ctx.KeyPressed[vk] = true;
        }
        else if (isRepeat) {
            g_Ctx.KeyPressed[vk] = true;
        }
        g_Ctx.KeyStates[vk] = true;
        return false;
    }

    inline void HandleKeyUp(int vk) {
        g_Ctx.KeyStates[vk] = false;
    }

    inline constexpr std::array<int, 256> HEX_TABLE = []() constexpr {
        std::array<int, 256> table{};
        table.fill(-1);
        for (int i = 0; i < 10; ++i) {
            table['0' + i] = i;
        }
        for (int i = 0; i < 6; ++i) {
            table['A' + i] = 10 + i;
            table['a' + i] = 10 + i;
        }
        return table;
        }();

    inline void ApplyHexInput() {
        auto it = g_Ctx.InputBuffers.find(HashString("CPHexInput"));
        if (it == g_Ctx.InputBuffers.end()) return;
        std::string_view hex = it->second;

        if (hex.size() < 6) return;
        hex = hex.substr(0, std::min(hex.size(), size_t(8)));

        auto hexDigits = hex | std::views::transform([](char c) {
            return HEX_TABLE[static_cast<unsigned char>(c)];
            });

        if (std::ranges::any_of(hexDigits, [](int v) { return v < 0; })) {
            return;
        }

        std::array<int, 4> rgba{};
        for (size_t i = 0; i < 4 && i * 2 + 1 < hex.size(); ++i) {
            int high = HEX_TABLE[static_cast<unsigned char>(hex[i * 2])];
            int low = HEX_TABLE[static_cast<unsigned char>(hex[i * 2 + 1])];
            rgba[i] = (high << 4) | low;
        }

        if (hex.size() < 8) rgba[3] = 255;

        auto [r, g, b, a] = rgba;

        *g_Ctx.ColorPickerR = r / 255.f;
        *g_Ctx.ColorPickerG = g / 255.f;
        *g_Ctx.ColorPickerB = b / 255.f;
        if (g_Ctx.ColorPickerA) *g_Ctx.ColorPickerA = a / 255.f;

        RGBtoHSV(*g_Ctx.ColorPickerR, *g_Ctx.ColorPickerG, *g_Ctx.ColorPickerB,
            g_Ctx.ColorPickerH, g_Ctx.ColorPickerS, g_Ctx.ColorPickerV);
    }

    // 新增：存储热键的完整信息（包括模式指针和is_active指针）
    struct RegisteredHotkeyInfo {
        int* hotkey;
        HotkeyMode* mode;
        bool* is_active;
    };

    // 替换原来的简单vector为带完整信息的vector
    inline std::vector<RegisteredHotkeyInfo> g_RegisteredHotkeyInfos;

    // 新增：检查某个按键是否是已注册的热键
    inline bool IsHotkeyRegistered(int vk) {
        for (auto& info : g_RegisteredHotkeyInfos) {
            if (*info.hotkey == vk && vk != 0) {
                return true;
            }
        }
        return false;
    }

    // 新增：更新所有已注册热键的 is_active 状态（每帧调用，无论菜单是否打开）
    inline void UpdateAllHotkeyStates() {
        for (auto& info : g_RegisteredHotkeyInfos) {
            if (info.is_active && info.mode) {
                switch (*info.mode) {
                case HotkeyMode::None:
                    *info.is_active = false;
                    break;
                case HotkeyMode::HoldOn:
                    *info.is_active = g_Ctx.KeyStates[*info.hotkey];
                    break;
                case HotkeyMode::HoldOff:
                    *info.is_active = !g_Ctx.KeyStates[*info.hotkey];
                    break;
                case HotkeyMode::ToggleOn:
                    *info.is_active = g_Ctx.HotkeyToggles[*info.hotkey];
                    break;
                case HotkeyMode::AlwaysOn:
                    *info.is_active = true;
                    break;
                }
            }
            else if (info.is_active) {
                // 没有模式指针（简单版本的HotKey），is_active 就是 hotkey 是否被按下
                *info.is_active = g_Ctx.KeyStates[*info.hotkey];
            }
        }
    }

    // 新增：处理全局热键输入（菜单关闭时也需要调用）
    inline bool ProcessGlobalHotkeys(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) {
            if (wParam < 256) {
                int vk = static_cast<int>(wParam);
                if (IsHotkeyRegistered(vk)) {
                    bool isRepeat = (lParam & (1 << 30)) != 0;
                    HandleKeyDown(vk, isRepeat);
                    return true;
                }
            }
        }
        else if (uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP) {
            if (wParam < 256) {
                int vk = static_cast<int>(wParam);
                if (IsHotkeyRegistered(vk)) {
                    HandleKeyUp(vk);
                    return true;
                }
            }
        }
        return false;
    }

    // 修改注册函数，现在需要提供完整信息
    inline void RegisterHotkey(int* hotkey, HotkeyMode* mode, bool* is_active) {
        // 检查是否已注册（通过hotkey指针判断）
        for (auto& info : g_RegisteredHotkeyInfos) {
            if (info.hotkey == hotkey) {
                // 已注册，更新信息
                info.mode = mode;
                info.is_active = is_active;
                return;
            }
        }
        // 未注册，添加新条目
        g_RegisteredHotkeyInfos.push_back({ hotkey, mode, is_active });
    }

    inline LRESULT Input(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_MOUSEWHEEL: {
            short zDelta = HIWORD(wParam);
            g_Ctx.MouseWheel += static_cast<float>(zDelta) / 120.f;
            break;
        }
        case WM_MOUSEMOVE:
            g_Ctx.MousePos.x = static_cast<float>(LOWORD(lParam));
            g_Ctx.MousePos.y = static_cast<float>(HIWORD(lParam));
            break;
        case WM_LBUTTONDBLCLK:  // [新增] 补充鼠标左键双击消息，防止快速连点丢失事件
        case WM_LBUTTONDOWN: {
            // [新增] 判断之前是否处于按住状态，防止按键连发 spam 导致的连续触发
            bool wasDown = g_Ctx.KeyStates[VK_LBUTTON];
            if (HandleKeyDown(VK_LBUTTON)) return 0;
            g_Ctx.MouseDown = true;
            if (!wasDown) {
                g_Ctx.MouseClicked = true;
            }
            break;
        }
        case WM_LBUTTONUP:
            HandleKeyUp(VK_LBUTTON);
            g_Ctx.MouseDown = false;
            g_Ctx.IsDragging = false;
            g_Ctx.IsResizing = false;
            break;
        case WM_RBUTTONDBLCLK:  // [新增] 补充鼠标右键双击消息
        case WM_RBUTTONDOWN: {
            bool wasDown = g_Ctx.KeyStates[VK_RBUTTON];
            if (HandleKeyDown(VK_RBUTTON)) return 0;
            g_Ctx.RightMouseDown = true;
            if (!wasDown) {
                g_Ctx.RightMouseClicked = true;
            }
            break;
        }
        case WM_RBUTTONUP:
            HandleKeyUp(VK_RBUTTON);
            g_Ctx.RightMouseDown = false;
            break;
        case WM_MBUTTONDBLCLK:  // [新增] 补充鼠标中键双击消息
        case WM_MBUTTONDOWN: if (HandleKeyDown(VK_MBUTTON)) return 0; break;
        case WM_MBUTTONUP:   HandleKeyUp(VK_MBUTTON); break;
        case WM_XBUTTONDBLCLK:  // [新增] 补充鼠标侧键双击消息 (解决 Hold On 模式丢失触发的 BUG)
        case WM_XBUTTONDOWN: {
            int vk = (HIWORD(wParam) == XBUTTON1) ? VK_XBUTTON1 : VK_XBUTTON2;
            if (HandleKeyDown(vk)) return 0;
            break;
        }
        case WM_XBUTTONUP: {
            int vk = (HIWORD(wParam) == XBUTTON1) ? VK_XBUTTON1 : VK_XBUTTON2;
            HandleKeyUp(vk);
            break;
        }
        case WM_CHAR: {
            if (g_Ctx.ActiveInputId != 0) {
                if (wParam >= 32 && wParam < 127) {
                    g_Ctx.InputChars.push_back(static_cast<char>(wParam));
                }
                return 0;
            }
            break;
        }
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (wParam < 256) {
                int vk = static_cast<int>(wParam);
                // 如果该按键是已注册的热键，跳过 Input 处理（已在 ProcessGlobalHotkeys 中处理）
                if (!IsHotkeyRegistered(vk)) {
                    bool isRepeat = (lParam & (1 << 30)) != 0;
                    if (HandleKeyDown(vk, isRepeat)) return 0;
                }
                if (g_Ctx.ActiveInputId != 0) return 0;
            }
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            if (wParam < 256) {
                int vk = static_cast<int>(wParam);
                // 如果该按键是已注册的热键，跳过 Input 处理（已在 ProcessGlobalHotkeys 中处理）
                if (!IsHotkeyRegistered(vk)) {
                    HandleKeyUp(vk);
                }
                if (g_Ctx.ActiveInputId != 0) return 0;
            }
            break;
        }
        return 0;
    }

    inline void PushClipRect(Vec2 min, Vec2 max) {
        g_Ctx.ClipStack.push_back({ min, max });
        g_Ctx.ClippingEnabled = true;

        if (g_Ctx.ClipStack.size() > 1) {
            Vec2 prevMin = g_Ctx.ClipMin;
            Vec2 prevMax = g_Ctx.ClipMax;
            g_Ctx.ClipMin.x = std::max(prevMin.x, min.x);
            g_Ctx.ClipMin.y = std::max(prevMin.y, min.y);
            g_Ctx.ClipMax.x = std::min(prevMax.x, max.x);
            g_Ctx.ClipMax.y = std::min(prevMax.y, max.y);
        }
        else {
            g_Ctx.ClipMin = min;
            g_Ctx.ClipMax = max;
        }
    }

    inline void PopClipRect() {
        if (!g_Ctx.ClipStack.empty()) {
            g_Ctx.ClipStack.pop_back();
        }

        if (g_Ctx.ClipStack.empty()) {
            g_Ctx.ClippingEnabled = false;
        }
        else {
            // 从头重建当前的剪裁交集区域
            g_Ctx.ClipMin = g_Ctx.ClipStack[0].first;
            g_Ctx.ClipMax = g_Ctx.ClipStack[0].second;
            for (size_t i = 1; i < g_Ctx.ClipStack.size(); ++i) {
                g_Ctx.ClipMin.x = std::max(g_Ctx.ClipMin.x, g_Ctx.ClipStack[i].first.x);
                g_Ctx.ClipMin.y = std::max(g_Ctx.ClipMin.y, g_Ctx.ClipStack[i].first.y);
                g_Ctx.ClipMax.x = std::min(g_Ctx.ClipMax.x, g_Ctx.ClipStack[i].second.x);
                g_Ctx.ClipMax.y = std::min(g_Ctx.ClipMax.y, g_Ctx.ClipStack[i].second.y);
            }
        }
    }

    inline bool IsRectVisible(Vec2 pos, Vec2 size) {
        if (!g_Ctx.ClippingEnabled) return true;
        if (pos.x + size.x < g_Ctx.ClipMin.x || pos.x > g_Ctx.ClipMax.x ||
            pos.y + size.y < g_Ctx.ClipMin.y || pos.y > g_Ctx.ClipMax.y) {
            return false;
        }
        return true;
    }

    inline bool IsRectFullyVisible(Vec2 pos, Vec2 size) {
        if (!g_Ctx.ClippingEnabled) return true;
        return pos.x >= g_Ctx.ClipMin.x && pos.y >= g_Ctx.ClipMin.y &&
            (pos.x + size.x) <= g_Ctx.ClipMax.x && (pos.y + size.y) <= g_Ctx.ClipMax.y;
    }

    inline void ClipRect(Vec2& pos, Vec2& size) {
        if (!g_Ctx.ClippingEnabled) return;

        if (pos.x < g_Ctx.ClipMin.x) {
            size.x -= (g_Ctx.ClipMin.x - pos.x);
            pos.x = g_Ctx.ClipMin.x;
        }
        if (pos.y < g_Ctx.ClipMin.y) {
            size.y -= (g_Ctx.ClipMin.y - pos.y);
            pos.y = g_Ctx.ClipMin.y;
        }
        if (pos.x + size.x > g_Ctx.ClipMax.x) {
            size.x = g_Ctx.ClipMax.x - pos.x;
        }
        if (pos.y + size.y > g_Ctx.ClipMax.y) {
            size.y = g_Ctx.ClipMax.y - pos.y;
        }

        if (size.x < 0.f) size.x = 0.f;
        if (size.y < 0.f) size.y = 0.f;
    }

    inline void DrawLine(Vec2 start, Vec2 end, Color color, float thickness = 1.0f) {
        if (g_Ctx.InTooltip) {
            g_Ctx.TooltipDrawCommands.push_back({ ShadowDrawCmdType::Line, start, end, color, thickness, "", nullptr, 1.0f, g_Ctx.ClippingEnabled, g_Ctx.ClipMin, g_Ctx.ClipMax });
            return;
        }

        if (!g_Ctx.Canvas || !g_Ctx.Canvas->DefaultTexture) return;

        float dx = end.x - start.x;
        float dy = end.y - start.y;
        float length = std::sqrt(dx * dx + dy * dy);
        if (length <= 0.0f) return;

        float angle = std::atan2(dy, dx) * (180.0f / 3.14159265358979323846f);

        SDK::FVector2D uePos{ static_cast<float>(start.x), static_cast<float>(start.y) };
        SDK::FVector2D ueSize{ static_cast<float>(length), static_cast<float>(thickness) };
        SDK::FLinearColor ueColor{ color.r, color.g, color.b, color.a };

        g_Ctx.Canvas->K2_DrawTexture(
            g_Ctx.Canvas->DefaultTexture,
            uePos,
            ueSize,
            SDK::FVector2D{ 0.0f, 0.0f },
            SDK::FVector2D{ 1.0f, 1.0f },
            ueColor,
            SDK::EBlendMode::BLEND_Translucent,
            angle,
            SDK::FVector2D{ 0.0f, 0.5f }
        );
    }

    inline void DrawRect(Vec2 pos, Vec2 size, Color color) {
        if (g_Ctx.InTooltip) {
            g_Ctx.TooltipDrawCommands.push_back({ ShadowDrawCmdType::Rect, pos, size, color, 1.0f, "", nullptr, 1.0f, g_Ctx.ClippingEnabled, g_Ctx.ClipMin, g_Ctx.ClipMax });
            return;
        }

        if (!g_Ctx.Canvas) return;
        if (!IsRectVisible(pos, size)) return;
        ClipRect(pos, size);
        if (size.x <= 0.f || size.y <= 0.f) return;

        SDK::FLinearColor ueColor{ color.r, color.g, color.b, color.a };
        SDK::FVector2D uePos{ static_cast<float>(pos.x), static_cast<float>(pos.y) };
        SDK::FVector2D ueSize{ static_cast<float>(size.x), static_cast<float>(size.y) };

        if (g_Ctx.Canvas->DefaultTexture) {
            g_Ctx.Canvas->K2_DrawTexture(
                g_Ctx.Canvas->DefaultTexture,
                uePos,
                ueSize,
                SDK::FVector2D{ 0.0f, 0.0f },
                SDK::FVector2D{ 1.0f, 1.0f },
                ueColor,
                SDK::EBlendMode::BLEND_Translucent,
                0.0f,
                SDK::FVector2D{ 0.0f, 0.0f }
            );
        }

        DrawLine({ pos.x, pos.y }, { pos.x + size.x, pos.y }, color);
        DrawLine({ pos.x + size.x, pos.y }, { pos.x + size.x, pos.y + size.y }, color);
        DrawLine({ pos.x + size.x, pos.y + size.y }, { pos.x, pos.y + size.y }, color);
        DrawLine({ pos.x, pos.y + size.y }, { pos.x, pos.y }, color);
    }

    inline void DrawRectFilled(Vec2 pos, Vec2 size, Color color) {
        if (g_Ctx.InTooltip) {
            g_Ctx.TooltipDrawCommands.push_back({ ShadowDrawCmdType::RectFilled, pos, size, color, 1.0f, "", nullptr, 1.0f, g_Ctx.ClippingEnabled, g_Ctx.ClipMin, g_Ctx.ClipMax });
            return;
        }

        if (!g_Ctx.Canvas) return;
        if (!IsRectVisible(pos, size)) return;
        ClipRect(pos, size);
        if (size.x <= 0.f || size.y <= 0.f) return;

        SDK::FLinearColor ueColor{ color.r, color.g, color.b, color.a };
        SDK::FVector2D uePos{ static_cast<float>(pos.x), static_cast<float>(pos.y) };
        SDK::FVector2D ueSize{ static_cast<float>(size.x), static_cast<float>(size.y) };

        if (g_Ctx.Canvas->DefaultTexture) {
            g_Ctx.Canvas->K2_DrawTexture(
                g_Ctx.Canvas->DefaultTexture,
                uePos,
                ueSize,
                SDK::FVector2D{ 0.0f, 0.0f },
                SDK::FVector2D{ 1.0f, 1.0f },
                ueColor,
                SDK::EBlendMode::BLEND_Translucent,
                0.0f,
                SDK::FVector2D{ 0.0f, 0.0f }
            );
        }
    }

    inline void DrawTriangleFilled(Vec2 p1, Vec2 p2, Vec2 p3, Color color) {
        if (g_Ctx.InTooltip) {
            ShadowDrawCmd cmd{};
            cmd.type = ShadowDrawCmdType::TriangleFilled;
            cmd.pos = { 0, 0 }; cmd.size = { 0, 0 };
            cmd.color = color;
            cmd.thickness = 1.0f;
            cmd.font = nullptr;
            cmd.fontScale = 1.0f;
            cmd.clippingEnabled = g_Ctx.ClippingEnabled;
            cmd.clipMin = g_Ctx.ClipMin;
            cmd.clipMax = g_Ctx.ClipMax;
            cmd.p1 = p1; cmd.p2 = p2; cmd.p3 = p3;
            g_Ctx.TooltipDrawCommands.push_back(cmd);
            return;
        }

        Vec2 v[3] = { p1, p2, p3 };
        if (v[0].y > v[1].y) std::swap(v[0], v[1]);
        if (v[0].y > v[2].y) std::swap(v[0], v[2]);
        if (v[1].y > v[2].y) std::swap(v[1], v[2]);

        if (std::abs(v[0].y - v[2].y) < 0.5f) return;

        auto interpolateX = [](float y, const Vec2& a, const Vec2& b) {
            if (std::abs(a.y - b.y) < 0.001f) return (a.x + b.x) * 0.5f;
            return a.x + (b.x - a.x) * (y - a.y) / (b.y - a.y);
            };

        int startY = static_cast<int>(std::round(v[0].y));
        int endY = static_cast<int>(std::round(v[2].y));

        for (int y = startY; y < endY; ++y) {
            float fy = static_cast<float>(y) + 0.5f;
            float xa = interpolateX(fy, v[0], v[2]);
            float xb = (fy < v[1].y) ? interpolateX(fy, v[0], v[1]) : interpolateX(fy, v[1], v[2]);

            if (xa > xb) std::swap(xa, xb);

            float x_min = std::ceil(xa - 0.5f);
            float x_max = std::floor(xb - 0.5f);
            float w = x_max - x_min + 1.0f;

            if (w > 0.0f) {
                DrawRectFilled({ x_min, static_cast<float>(y) }, { w, 1.0f }, color);
            }
        }
    }

    inline Vec2 MeasureTextSize(std::string_view text) {
        SDK::UFont* font = g_Ctx.DefaultFont;
        float scaleVal = 1.0f;
        if (!g_Ctx.FontStack.empty()) {
            font = g_Ctx.FontStack.back().Font;
            scaleVal = g_Ctx.FontStack.back().Scale;
        }
        if (!g_Ctx.Canvas || !font) return { 0.f, 0.f };

        float finalScale = scaleVal * g_Ctx.Style.FontScaleDpi;
        SDK::FVector2D scale{ static_cast<float>(finalScale), static_cast<float>(finalScale) };

        std::wstring wstr = ToWString(text);
        SDK::FVector2D size = g_Ctx.Canvas->K2_TextSize(font, SDK::FString(wstr.c_str()), scale);
        return { static_cast<float>(size.X), static_cast<float>(size.Y) };
    }

    inline float MeasureCharWidth(wchar_t ch) {
        SDK::UFont* font = g_Ctx.DefaultFont;
        float scaleVal = 1.0f;
        if (!g_Ctx.FontStack.empty()) {
            font = g_Ctx.FontStack.back().Font;
            scaleVal = g_Ctx.FontStack.back().Scale;
        }
        if (!g_Ctx.Canvas || !font) return 0.f;

        float finalScale = scaleVal * g_Ctx.Style.FontScaleDpi;
        SDK::FVector2D scale{ static_cast<float>(finalScale), static_cast<float>(finalScale) };

        std::wstring single(1, ch);
        SDK::FVector2D size = g_Ctx.Canvas->K2_TextSize(font, SDK::FString(single.c_str()), scale);
        return static_cast<float>(size.X);
    }

    inline float MeasureTextHeight(std::string_view text) {
        if (!g_Ctx.Canvas || !g_Ctx.DefaultFont || text.empty()) return 0.f;
        Vec2 size = MeasureTextSize(text);
        return size.y;
    }

    inline void UpdateItemHeight() {
        Vec2 charSize = MeasureTextSize("A");
        g_Ctx.ItemHeight = charSize.y > 0.f ? charSize.y + g_Ctx.Style.FramePadding.y * 2.f : 20.f;
        g_Ctx.SliderInputWidthCache.clear();
        g_Ctx.SliderInputLengthCache.clear();
    }

    inline void PushFont(SDK::UFont* font, float scale = 1.0f) {
        g_Ctx.FontStack.push_back({ font, scale });
        UpdateItemHeight();
    }

    inline void PopFont() {
        if (!g_Ctx.FontStack.empty()) {
            g_Ctx.FontStack.pop_back();
            UpdateItemHeight();
        }
    }

    inline void PushTextWrapPos(float wrap_pos_x = 0.0f) {
        // 如果传入的是 0.0f（默认参数）或负数，则使用当前窗口的默认右边界
        if (wrap_pos_x <= 0.0f) {
            float right_margin = g_Ctx.Style.WindowPadding.x + g_Ctx.CurrentScrollbarWidth + g_Ctx.Style.ScrollbarMargin;
            wrap_pos_x = g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - right_margin;
        }
        else {
            wrap_pos_x = g_Ctx.Cursor.x + wrap_pos_x;
        }

        // 如果堆栈不为空，需要将新的位置与上一个位置进行比较，取较小值，确保嵌套时换行越来越靠左。
        if (!g_Ctx.TextWrapPosStack.empty()) {
            wrap_pos_x = std::min(wrap_pos_x, g_Ctx.TextWrapPosStack.back());
        }

        g_Ctx.TextWrapPosStack.push_back(wrap_pos_x);
    }

    inline void PopTextWrapPos() {
        if (!g_Ctx.TextWrapPosStack.empty()) {
            g_Ctx.TextWrapPosStack.pop_back();
        }
    }

    inline std::string ClipTextString(std::string_view text, Vec2 pos, Vec2& outPos, bool& shouldDraw) {
        shouldDraw = true;
        outPos = pos;
        if (!g_Ctx.ClippingEnabled) return std::string(text);

        float textHeight = MeasureTextHeight(text);
        if (pos.y + textHeight > g_Ctx.ClipMax.y || pos.y < g_Ctx.ClipMin.y) {
            shouldDraw = false;
            return "";
        }

        std::wstring wstr = ToWString(text);
        if (wstr.empty() || pos.x >= g_Ctx.ClipMax.x) {
            shouldDraw = false;
            return "";
        }

        if (pos.x < g_Ctx.ClipMin.x) {
            float currentX = pos.x;
            std::wstring clippedWstr;
            bool foundStart = false;
            for (size_t i = 0; i < wstr.size(); ++i) {
                float charWidth = MeasureCharWidth(wstr[i]);
                if (!foundStart) {
                    if (currentX + charWidth > g_Ctx.ClipMin.x) {
                        foundStart = true;
                        clippedWstr += wstr[i];
                        outPos.x = currentX;
                    }
                    currentX += charWidth;
                }
                else {
                    clippedWstr += wstr[i];
                }
            }
            if (!foundStart) {
                shouldDraw = false;
                return "";
            }
            wstr = clippedWstr;
        }

        float totalWidth = 0.f;
        for (wchar_t ch : wstr) totalWidth += MeasureCharWidth(ch);

        while (!wstr.empty() && outPos.x + totalWidth > g_Ctx.ClipMax.x) {
            wchar_t lastChar = wstr.back();
            float lastCharWidth = MeasureCharWidth(lastChar);
            totalWidth -= lastCharWidth;
            wstr.pop_back();
        }

        if (wstr.empty()) {
            shouldDraw = false;
            return "";
        }

        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
        std::string result(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &result[0], size_needed, nullptr, nullptr);
        shouldDraw = true;
        return result;
    }

    inline void DrawTextString(std::string_view text, Vec2 pos, Color color) {
        SDK::UFont* font = g_Ctx.DefaultFont;
        float scaleVal = 1.0f;
        if (!g_Ctx.FontStack.empty()) {
            font = g_Ctx.FontStack.back().Font;
            scaleVal = g_Ctx.FontStack.back().Scale;
        }

        if (g_Ctx.InTooltip) {
            g_Ctx.TooltipDrawCommands.push_back({ ShadowDrawCmdType::Text, pos, {0, 0}, color, 1.0f, std::string(text), font, scaleVal, g_Ctx.ClippingEnabled, g_Ctx.ClipMin, g_Ctx.ClipMax });
            return;
        }

        if (!g_Ctx.Canvas || !font) return;

        Vec2 clippedPos = pos;
        bool shouldDraw = true;
        std::string clippedText = ClipTextString(text, pos, clippedPos, shouldDraw);
        if (!shouldDraw || clippedText.empty()) return;

        float finalScale = scaleVal * g_Ctx.Style.FontScaleDpi;
        SDK::FVector2D scale{ static_cast<float>(finalScale), static_cast<float>(finalScale) };

        SDK::FLinearColor ueColor{ color.r, color.g, color.b, color.a };
        SDK::FVector2D uePos{ static_cast<float>(clippedPos.x), static_cast<float>(clippedPos.y) };

        Color sCol = g_Ctx.Style.Colors[GuiCol_TextShadow];
        Color oCol = g_Ctx.Style.Colors[GuiCol_TextOutline];
        SDK::FLinearColor shadow{ sCol.r, sCol.g, sCol.b, sCol.a };
        SDK::FLinearColor outline{ oCol.r, oCol.g, oCol.b, oCol.a };
        SDK::FVector2D shadowOff{ 1.0f, 1.0f };

        std::wstring wstr = ToWString(clippedText);
        g_Ctx.Canvas->K2_DrawText(font, SDK::FString(wstr.c_str()), uePos, scale, ueColor,
            0.0f, shadow, shadowOff,
            false, false, false, outline);
    }

    // --- 剪贴板操作 ---
    inline void SetClipboardText(const std::string& text) {
        if (OpenClipboard(nullptr)) {
            EmptyClipboard();
            HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
            if (hMem) {
                memcpy(GlobalLock(hMem), text.c_str(), text.size() + 1);
                GlobalUnlock(hMem);
                SetClipboardData(CF_TEXT, hMem);
            }
            CloseClipboard();
        }
    }

    inline std::string GetClipboardText() {
        std::string result;
        if (OpenClipboard(nullptr)) {
            HANDLE hData = GetClipboardData(CF_TEXT);
            if (hData) {
                char* pszText = static_cast<char*>(GlobalLock(hData));
                if (pszText) {
                    result = pszText;
                }
                GlobalUnlock(hData);
            }
            CloseClipboard();
        }
        return result;
    }

    inline float GetControlOffsetX() {
        return std::max(g_Ctx.Style.ControlOffsetMin, g_Ctx.WindowSize.x * g_Ctx.Style.ControlOffsetRatio);
    }

    inline float GetRightMargin() {
        return g_Ctx.Style.WindowPadding.x + g_Ctx.CurrentScrollbarWidth + g_Ctx.Style.ScrollbarMargin;
    }

    inline bool IsDisabled() {
        return !g_Ctx.DisabledStack.empty() && g_Ctx.DisabledStack.back();
    }

    inline void SetLastItemInfo(Vec2 min, Vec2 max, size_t id, bool disabled = false) {
        g_Ctx.LastItemMin = min;
        g_Ctx.LastItemMax = max;
        g_Ctx.LastItemId = id;
        g_Ctx.LastItemDisabled = disabled;

        if (g_Ctx.InTooltip) {
            g_Ctx.CurrentTooltipSize.x = std::max(g_Ctx.CurrentTooltipSize.x, max.x - g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x);
            g_Ctx.CurrentTooltipSize.y = std::max(g_Ctx.CurrentTooltipSize.y, max.y - g_Ctx.WindowPos.y + g_Ctx.Style.WindowPadding.y);
        }
    }

    inline void BeginDisabled(bool disabled = true) {
        bool current = IsDisabled();
        // 如果外层已经被禁用，内层强制继承禁用状态
        g_Ctx.DisabledStack.push_back(current || disabled);
    }

    inline void EndDisabled() {
        if (!g_Ctx.DisabledStack.empty()) {
            g_Ctx.DisabledStack.pop_back();
        }
    }

    inline bool TreeNode(std::string_view name, ShadowTreeNodeFlags flags = ShadowTreeNodeFlags_None, Vec2 size_arg = { 0.f, 0.f }) {
        if (!g_Ctx.InActiveTab) return false;
        std::string_view display; size_t id; ParseLabel(name, display, id);
        g_Ctx.WidgetCount++;
        g_Ctx.TreeNodeStack++;

        if (g_Ctx.TreeNodeOpenStates.find(id) == g_Ctx.TreeNodeOpenStates.end()) {
            g_Ctx.TreeNodeOpenStates[id] = (flags & ShadowTreeNodeFlags_DefaultOpen) != 0;
        }

        bool isOpen = g_Ctx.TreeNodeOpenStates[id];
        bool isFramed = (flags & ShadowTreeNodeFlags_Framed) != 0;
        bool isFitText = (flags & ShadowTreeNodeFlags_FitText) != 0;
        bool noIndent = (flags & ShadowTreeNodeFlags_NoIndent) != 0;

        float itemHeight = size_arg.y > 0.f ? size_arg.y : g_Ctx.ItemHeight;
        float arrowSize = itemHeight * 0.55f;
        float textWidth = MeasureTextSize(display).x;
        Vec2 interactSize = { arrowSize + 10.f + textWidth, itemHeight };

        if (isFramed) {
            if (isFitText) {
                interactSize.x = g_Ctx.Style.FramePadding.x + arrowSize + 8.f + textWidth + g_Ctx.Style.FramePadding.x;
            }
            else {
                float rightMargin = GetRightMargin();
                interactSize.x = std::max(interactSize.x, g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - rightMargin - g_Ctx.Cursor.x);
            }
        }

        if (size_arg.x > 0.f) interactSize.x = size_arg.x;

        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, itemHeight })) {
            g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
            if (!noIndent) g_Ctx.IndentX += 20.f;
            g_Ctx.TreeNodeNoIndentStack.push_back(noIndent);
            g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
            return isOpen;
        }

        bool disabled = IsDisabled();
        bool hovered = !disabled && IsMouseHovering(g_Ctx.Cursor, interactSize);

        if (hovered && g_Ctx.MouseClicked) {
            isOpen = !isOpen;
            g_Ctx.TreeNodeOpenStates[id] = isOpen;
            g_Ctx.MouseClicked = false;
        }

        Color textColor = disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text];
        Color arrowColor = disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text];
        if (hovered && !disabled) {
            textColor = g_Ctx.Style.Colors[GuiCol_TextHighlight];
            arrowColor = g_Ctx.Style.Colors[GuiCol_TextHighlight];
        }

        if (isFramed) {
            Color bgColor = disabled ? g_Ctx.Style.Colors[GuiCol_ControlDisabled] : (hovered ? g_Ctx.Style.Colors[GuiCol_FrameBgHovered] : g_Ctx.Style.Colors[GuiCol_FrameBg]);
            DrawRectFilled(g_Ctx.Cursor, interactSize, bgColor);
        }

        float centerY = g_Ctx.Cursor.y + itemHeight * 0.5f;
        float arrowX = g_Ctx.Cursor.x + (isFramed ? g_Ctx.Style.FramePadding.x : 0.f);

        if (isOpen) {
            Vec2 p1 = { arrowX, centerY - arrowSize * 0.25f };
            Vec2 p2 = { arrowX + arrowSize, centerY - arrowSize * 0.25f };
            Vec2 p3 = { arrowX + arrowSize * 0.5f, centerY + arrowSize * 0.25f };
            DrawTriangleFilled(p1, p2, p3, arrowColor);
        }
        else {
            Vec2 p1 = { arrowX + arrowSize * 0.25f, centerY - arrowSize * 0.5f };
            Vec2 p2 = { arrowX + arrowSize * 0.75f, centerY };
            Vec2 p3 = { arrowX + arrowSize * 0.25f, centerY + arrowSize * 0.5f };
            DrawTriangleFilled(p1, p2, p3, arrowColor);
        }

        float textStartX = std::round(arrowX + arrowSize + 8.f);
        DrawTextString(display, { textStartX, g_Ctx.Cursor.y + g_Ctx.Style.FramePadding.y + (itemHeight - g_Ctx.ItemHeight) * 0.5f }, textColor);

        SetLastItemInfo(g_Ctx.Cursor, { g_Ctx.Cursor.x + interactSize.x, g_Ctx.Cursor.y + itemHeight }, id, disabled);
        g_Ctx.LastItemMaxX = g_Ctx.Cursor.x + interactSize.x;

        g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
        if (!noIndent) g_Ctx.IndentX += 20.f;
        g_Ctx.TreeNodeNoIndentStack.push_back(noIndent);
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;

        return isOpen;
    }

    inline void TreePop() {
        if (!g_Ctx.InActiveTab) return;
        g_Ctx.TreeNodeStack--;

        bool noIndent = false;
        if (!g_Ctx.TreeNodeNoIndentStack.empty()) {
            noIndent = g_Ctx.TreeNodeNoIndentStack.back();
            g_Ctx.TreeNodeNoIndentStack.pop_back();
        }

        if (!noIndent) {
            g_Ctx.IndentX = std::max(0.f, g_Ctx.IndentX - 20.f);
        }

        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
    }

    inline void Spacing(Vec2 size_arg = { 0.f, 0.f }) {
        if (!g_Ctx.InActiveTab) return;
        g_Ctx.Cursor.y += size_arg.y > 0.f ? size_arg.y : g_Ctx.Style.ItemSpacing.y;
        if (size_arg.x > 0.f) g_Ctx.Cursor.x += size_arg.x;
    }

    inline void SameLine(float offset_from_start_x = 0.0f, float spacing = -1.0f) {
        if (!g_Ctx.InActiveTab) return;
        // 撤销上一个控件产生的Y轴换行递增
        g_Ctx.Cursor.y -= (g_Ctx.ItemHeight + g_Ctx.Style.ItemSpacing.y);

        if (offset_from_start_x > 0.0f) {
            g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + offset_from_start_x + g_Ctx.IndentX;
        }
        else {
            float sp = (spacing < 0.0f) ? g_Ctx.Style.ItemSpacing.x : spacing;
            g_Ctx.Cursor.x = g_Ctx.LastItemMaxX + sp;
        }
    }

    inline bool IsItemHovered(ShadowHoveredFlags flags = ShadowHoveredFlags_None) {
        if (!g_Ctx.InActiveTab) return false;

        Vec2 min = g_Ctx.LastItemMin;
        Vec2 max = g_Ctx.LastItemMax;
        size_t id = g_Ctx.LastItemId;

        bool disabled = g_Ctx.LastItemDisabled;
        if (disabled && !(flags & ShadowHoveredFlags_AllowWhenDisabled)) {
            return false;
        }

        // 检查视口裁剪遮挡
        if (!IsRectVisible(min, { max.x - min.x, max.y - min.y })) {
            return false;
        }

        // [修改] 检查是否被处于激活状态的控件打断（增加 ListBox 滚动条拖拽阻塞）
        bool hasActiveItem = g_Ctx.ActiveInputId != 0 || g_Ctx.DraggingSliderId != 0 ||
            g_Ctx.ActiveDropdownId != 0 || g_Ctx.ActiveColorPickerId != 0 ||
            g_Ctx.IsDragging || g_Ctx.IsResizing || g_Ctx.IsDraggingScrollbar ||
            g_Ctx.DraggingTabId != 0 || g_Ctx.DraggingTabBarScrollId != 0 ||
            g_Ctx.DraggingListBoxScrollId != 0;

        if (hasActiveItem && !(flags & ShadowHoveredFlags_AllowWhenBlockedByActiveItem)) {
            // 例外：如果当前激活的正是该控件本身，则放行
            bool isSelfActive = false;
            if (id != 0) {
                if (g_Ctx.DraggingSliderId == id ||
                    g_Ctx.ActiveDropdownId == id ||
                    g_Ctx.ActiveColorPickerId == id) {
                    isSelfActive = true;
                }
                size_t sliderInputId = id ^ HashString("_sliderInput");
                if (g_Ctx.ActiveInputId == sliderInputId) {
                    isSelfActive = true;
                }
            }
            if (!isSelfActive) {
                return false;
            }
        }

        bool hoveringRaw = IsMouseHoveringRaw(min, { max.x - min.x, max.y - min.y });
        bool hoveringClipped = IsMouseHovering(min, { max.x - min.x, max.y - min.y });

        // 物理位置根本不在控件内
        if (!hoveringRaw) return false;

        // 被 Popup 弹窗挡住但没有对应的豁免 Flag
        if (!hoveringClipped && !(flags & ShadowHoveredFlags_AllowWhenBlockedByPopup)) {
            return false;
        }

        // --- 开始处理悬停时间和静止逻辑 ---
        // 赋予无 id 控件 (如 Text) 一个独有编号
        size_t currentId = (id != 0) ? id : (static_cast<size_t>(g_Ctx.WidgetCount) + 1000000ULL);
        g_Ctx.HoveredIdCurrentFrame = currentId;
        uint64_t currentTime = GetTickCount64();

        // 状态更新锁定 (防止同一帧重复调用导致重置)
        if (g_Ctx.LastHoveredIdEval != currentId) {
            if (g_Ctx.HoveredIdPreviousFrame != currentId) {
                g_Ctx.HoveredIdTimerStart = currentTime;
                g_Ctx.HoveredIdStationaryTriggered = false;
                g_Ctx.HoveredIdDelayTriggered = false;
            }
            g_Ctx.LastHoveredIdEval = currentId;
        }

        if (g_Ctx.MouseIsStationary) {
            g_Ctx.HoveredIdStationaryTriggered = true;
        }

        // 静止锁定判定：要求至少停顿过一次
        if (flags & ShadowHoveredFlags_Stationary) {
            if (!g_Ctx.HoveredIdStationaryTriggered) {
                return false;
            }
        }

        uint32_t requiredDelay = 0;
        if (flags & ShadowHoveredFlags_DelayNormal) requiredDelay = 400;
        else if (flags & ShadowHoveredFlags_DelayShort) requiredDelay = 150;

        if (requiredDelay > 0) {
            bool useSharedDelay = !(flags & ShadowHoveredFlags_NoSharedDelay);
            if (useSharedDelay && g_Ctx.SharedDelayActive && currentTime <= g_Ctx.SharedDelayExpirationTime) {
                // 共享延迟生效（无缝切换），直接跳过等待
            }
            else {
                // 需要鼠标真正“停驻”并开始读秒，计时起点取悬停或鼠标静止的最晚时间
                uint64_t waitTime = currentTime - std::max(g_Ctx.HoveredIdTimerStart, g_Ctx.MouseStationaryStartTime);
                if (!g_Ctx.HoveredIdDelayTriggered) {
                    if (waitTime >= requiredDelay) {
                        g_Ctx.HoveredIdDelayTriggered = true;
                    }
                    else {
                        return false;
                    }
                }
            }

            // 延续全局的丝滑切换共享窗口
            if (useSharedDelay && (g_Ctx.HoveredIdDelayTriggered || requiredDelay == 0)) {
                g_Ctx.SharedDelayActive = true;
                g_Ctx.SharedDelayExpirationTime = currentTime + 250; // 维持0.25秒的继承窗口
            }
        }

        return true;
    }

    inline bool BeginListBox(std::string_view name, Vec2 size) {
        g_Ctx.ListBoxStack++;
        if (!g_Ctx.InActiveTab) return false;
        std::string_view display; size_t id; ParseLabel(name, display, id);
        g_Ctx.WidgetCount++;

        // [新增] 如果具备有效的显示名称，先将其作为标题独立渲染在 Box 上方，消除视觉混淆
        if (!display.empty()) {
            DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + g_Ctx.Style.FramePadding.y }, g_Ctx.Style.Colors[GuiCol_Text]);
            g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Style.ItemSpacing.y;
        }

        float boxWidth = size.x;
        if (boxWidth <= 0.f) {
            boxWidth = std::max(10.f, g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - GetRightMargin() - g_Ctx.Cursor.x);
        }
        float boxHeight = size.y;
        if (boxHeight <= 0.f) {
            boxHeight = g_Ctx.ItemHeight * 5.f;
        }

        Vec2 boxPos = g_Ctx.Cursor;

        // 备份父级容器状态
        ListBoxState backup;
        backup.ParentWindowPos = g_Ctx.WindowPos;
        backup.ParentWindowSize = g_Ctx.WindowSize;
        backup.ParentCursor = g_Ctx.Cursor;
        backup.ParentContentStartY = g_Ctx.ContentStartY;
        backup.ParentScrollY = g_Ctx.ScrollY;
        backup.ParentContentHeight = g_Ctx.ContentHeight;
        backup.ParentIsScrollApplied = g_Ctx.IsScrollApplied;
        backup.ParentWindowFlags = g_Ctx.CurrentWindowFlags;
        backup.ParentCurrentScrollbarWidth = g_Ctx.CurrentScrollbarWidth;
        backup.ParentWindowPadding = g_Ctx.Style.WindowPadding;
        backup.ParentIndentX = g_Ctx.IndentX; // [新增] 保存先前的缩进状态
        backup.Id = id;
        backup.Pos = boxPos;
        backup.Size = { boxWidth, boxHeight };

        g_Ctx.ListBoxStateStack.push_back(backup);

        // [新增] 清空缩进距离，使 ListBox 内部的控件强制靠边对齐
        g_Ctx.IndentX = 0.f;

        // 绘制背景与边框
        DrawRectFilled(boxPos, { boxWidth, boxHeight }, g_Ctx.Style.Colors[GuiCol_FrameBg]);
        DrawRect(boxPos, { boxWidth, boxHeight }, g_Ctx.Style.Colors[GuiCol_Border]);

        // 应用子级容器状态
        g_Ctx.WindowPos = boxPos;
        g_Ctx.WindowSize = { boxWidth, boxHeight };
        g_Ctx.CurrentWindowFlags = ShadowWindowFlags_NoResize | ShadowWindowFlags_NoMove | ShadowWindowFlags_NoTitleBar;
        g_Ctx.Style.WindowPadding = { g_Ctx.Style.FramePadding.x, g_Ctx.Style.FramePadding.y };

        g_Ctx.Cursor = { boxPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX, boxPos.y + g_Ctx.Style.WindowPadding.y };
        g_Ctx.ContentStartY = g_Ctx.Cursor.y;

        float maxScroll = std::max(0.f, g_Ctx.ListBoxContentHeight[id] - boxHeight);
        float& scrollY = g_Ctx.ListBoxScrollY[id];
        scrollY = std::clamp(scrollY, 0.f, maxScroll);
        g_Ctx.ScrollY = scrollY;
        g_Ctx.IsScrollApplied = true;

        if (maxScroll > 0.f) {
            g_Ctx.CurrentScrollbarWidth = g_Ctx.Style.ScrollbarSize;
        }
        else {
            g_Ctx.CurrentScrollbarWidth = 0.f;
        }

        bool hoveringListBox = IsMouseHovering(boxPos, { boxWidth, boxHeight });
        if (hoveringListBox) {
            // [新增] 标记 ListBox 被悬停控制，阻挡主菜单获取焦点
            g_Ctx.HoveredListBoxIdCurrentFrame = id;

            // 处理滚轮事件并在消费后清空标志位
            if (g_Ctx.MouseWheel != 0.f) {
                scrollY -= g_Ctx.MouseWheel * 30.f;
                scrollY = std::clamp(scrollY, 0.f, maxScroll);
                g_Ctx.ScrollY = scrollY;
                g_Ctx.MouseWheel = 0.f;
            }
        }

        g_Ctx.Cursor.y -= g_Ctx.ScrollY;

        PushClipRect(boxPos, { boxPos.x + boxWidth, boxPos.y + boxHeight });

        return true;
    }

    inline void EndListBox() {
        g_Ctx.ListBoxStack--;
        if (!g_Ctx.InActiveTab) return;
        if (g_Ctx.ListBoxStateStack.empty()) return;

        ListBoxState backup = g_Ctx.ListBoxStateStack.back();
        g_Ctx.ListBoxStateStack.pop_back();

        float actualCursorY = g_Ctx.Cursor.y + (g_Ctx.IsScrollApplied ? g_Ctx.ScrollY : 0.f);
        g_Ctx.ListBoxContentHeight[backup.Id] = actualCursorY - g_Ctx.ContentStartY + g_Ctx.Style.WindowPadding.y;

        PopClipRect();

        float viewHeight = backup.Size.y;
        float contentHeight = g_Ctx.ListBoxContentHeight[backup.Id];
        float maxScroll = std::max(0.f, contentHeight - viewHeight);
        float& scrollY = g_Ctx.ListBoxScrollY[backup.Id];

        g_Ctx.CurrentScrollbarWidth = 0.f;

        // 如果内容超高，渲染并处理右侧独立滚动条
        if (contentHeight > viewHeight) {
            float scrollbarWidth = g_Ctx.Style.ScrollbarSize;
            float scrollbarMarginRight = g_Ctx.Style.ScrollbarMargin;

            Vec2 trackPos = { backup.Pos.x + backup.Size.x - scrollbarWidth - scrollbarMarginRight, backup.Pos.y };
            Vec2 trackSize = { scrollbarWidth, viewHeight };

            DrawRect(trackPos, trackSize, g_Ctx.Style.Colors[GuiCol_FrameBg]);

            float thumbHeight = std::max(20.f, (viewHeight / contentHeight) * trackSize.y);
            float thumbY = trackPos.y + (maxScroll > 0.f ? (scrollY / maxScroll) * (trackSize.y - thumbHeight) : 0.f);
            Vec2 thumbPos = { trackPos.x, thumbY };
            Vec2 thumbSize = { scrollbarWidth, thumbHeight };

            bool hoveringThumb = IsMouseHovering(thumbPos, thumbSize);
            bool hoveringTrack = IsMouseHovering(trackPos, trackSize);

            if (g_Ctx.MouseClicked && hoveringThumb) {
                g_Ctx.DraggingListBoxScrollId = backup.Id;
                g_Ctx.ListBoxScrollDragOffset = g_Ctx.MousePos.y - thumbPos.y;
                g_Ctx.MouseClicked = false; // [新增] 点击即销毁，阻断鼠标向主菜单透传
            }
            else if (g_Ctx.MouseClicked && hoveringTrack) {
                if (g_Ctx.MousePos.y < thumbPos.y) scrollY -= viewHeight;
                else scrollY += viewHeight;
                scrollY = std::clamp(scrollY, 0.f, maxScroll);
                g_Ctx.MouseClicked = false; // [新增]
            }

            if (g_Ctx.DraggingListBoxScrollId == backup.Id) {
                if (g_Ctx.MouseDown) {
                    float newThumbY = g_Ctx.MousePos.y - g_Ctx.ListBoxScrollDragOffset;
                    float ratio = (newThumbY - trackPos.y) / std::max(1.f, trackSize.y - thumbHeight);
                    scrollY = std::clamp(ratio * maxScroll, 0.f, maxScroll);
                }
                else {
                    g_Ctx.DraggingListBoxScrollId = 0;
                }
            }

            Color thumbColor = (g_Ctx.DraggingListBoxScrollId == backup.Id)
                ? g_Ctx.Style.Colors[GuiCol_SliderGrab]
                : (hoveringThumb ? g_Ctx.Style.Colors[GuiCol_FrameBgHovered] : g_Ctx.Style.Colors[GuiCol_Border]);
            DrawRect(thumbPos, thumbSize, thumbColor);
        }
        else {
            if (g_Ctx.DraggingListBoxScrollId == backup.Id) {
                g_Ctx.DraggingListBoxScrollId = 0;
            }
        }

        // 恢复父级容器状态
        g_Ctx.WindowPos = backup.ParentWindowPos;
        g_Ctx.WindowSize = backup.ParentWindowSize;
        g_Ctx.Cursor = backup.ParentCursor;
        g_Ctx.ContentStartY = backup.ParentContentStartY;
        g_Ctx.ScrollY = backup.ParentScrollY;
        g_Ctx.ContentHeight = backup.ParentContentHeight;
        g_Ctx.IsScrollApplied = backup.ParentIsScrollApplied;
        g_Ctx.CurrentWindowFlags = backup.ParentWindowFlags;
        g_Ctx.CurrentScrollbarWidth = backup.ParentCurrentScrollbarWidth;
        g_Ctx.Style.WindowPadding = backup.ParentWindowPadding;
        g_Ctx.IndentX = backup.ParentIndentX; // [新增] 恢复之前外层的树级或者缩进层级

        // 设置本 ListBox 占用尺寸
        SetLastItemInfo(backup.Pos, { backup.Pos.x + backup.Size.x, backup.Pos.y + backup.Size.y }, backup.Id, false);
        g_Ctx.LastItemMaxX = backup.Pos.x + backup.Size.x;

        // 推进光标
        g_Ctx.Cursor.y += backup.Size.y + g_Ctx.Style.ItemSpacing.y;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
    }

    inline bool InputTextEx(size_t id, Vec2 pos, Vec2 size, std::string& text, ShadowInputTextFlags flags = 0, bool is_popup = false, std::string_view hint = "") {
        bool disabled = IsDisabled();
        bool hovered = is_popup ? IsMouseHoveringRaw(pos, size) : IsMouseHovering(pos, size);

        bool isReadOnly = (flags & ShadowInputTextFlags_ReadOnly) != 0;
        bool isPassword = (flags & ShadowInputTextFlags_Password) != 0;

        if (g_Ctx.MouseClicked) {
            if (hovered && !disabled) {
                g_Ctx.ActiveInputId = id;
                g_Ctx.InputCursorPos = static_cast<int>(text.size());
                g_Ctx.InputSelectionStart = -1;
                g_Ctx.InputSelectionEnd = -1;
                g_Ctx.MouseClicked = false;

                if (flags & ShadowInputTextFlags_AutoSelectAll) {
                    g_Ctx.InputSelectionStart = 0;
                    g_Ctx.InputSelectionEnd = static_cast<int>(text.size());
                    g_Ctx.InputCursorPos = static_cast<int>(text.size());
                }
            }
            else if (g_Ctx.ActiveInputId == id) {
                g_Ctx.ActiveInputId = 0;
            }
        }

        bool isActive = (g_Ctx.ActiveInputId == id) && !disabled;
        bool valueChanged = false;

        if (isActive) {
            g_Ctx.InputCursorPos = std::clamp(g_Ctx.InputCursorPos, 0, static_cast<int>(text.size()));
            bool ctrlDown = g_Ctx.KeyStates[VK_CONTROL];
            bool shiftDown = g_Ctx.KeyStates[VK_SHIFT];

            auto DeleteSelection = [&]() {
                if (isReadOnly) return false;
                if (g_Ctx.InputSelectionStart != -1 && g_Ctx.InputSelectionEnd != -1 && g_Ctx.InputSelectionStart != g_Ctx.InputSelectionEnd) {
                    int s = std::clamp(std::min(g_Ctx.InputSelectionStart, g_Ctx.InputSelectionEnd), 0, (int)text.size());
                    int e = std::clamp(std::max(g_Ctx.InputSelectionStart, g_Ctx.InputSelectionEnd), 0, (int)text.size());
                    text.erase(s, e - s);
                    g_Ctx.InputCursorPos = s;
                    g_Ctx.InputSelectionStart = g_Ctx.InputSelectionEnd = -1;
                    valueChanged = true;
                    return true;
                }
                return false;
                };

            if (!isReadOnly) {
                for (char c : g_Ctx.InputChars) {
                    if (flags & ShadowInputTextFlags_CharsNoBlank) if (c == ' ' || c == '\t') continue;
                    if (flags & ShadowInputTextFlags_CharsUppercase) if (c >= 'a' && c <= 'z') c -= 32;
                    if (flags & ShadowInputTextFlags_CharsDecimal) if (!(c >= '0' && c <= '9') && c != '.' && c != '+' && c != '-' && c != '*' && c != '/') continue;
                    if (flags & ShadowInputTextFlags_CharsHexadecimal) if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F')) continue;
                    if (flags & ShadowInputTextFlags_CharsScientific) if (!(c >= '0' && c <= '9') && c != '.' && c != '+' && c != '-' && c != '*' && c != '/' && c != 'e' && c != 'E') continue;

                    DeleteSelection();
                    text.insert(text.begin() + g_Ctx.InputCursorPos, c);
                    g_Ctx.InputCursorPos++;
                    valueChanged = true;
                }
            }

            if (ctrlDown) {
                if (g_Ctx.KeyPressed['A']) {
                    g_Ctx.InputSelectionStart = 0;
                    g_Ctx.InputSelectionEnd = static_cast<int>(text.size());
                    g_Ctx.InputCursorPos = static_cast<int>(text.size());
                }
                if (g_Ctx.KeyPressed['C'] || g_Ctx.KeyPressed['X']) {
                    if (!isPassword) {
                        if (g_Ctx.InputSelectionStart != -1 && g_Ctx.InputSelectionEnd != -1 && g_Ctx.InputSelectionStart != g_Ctx.InputSelectionEnd) {
                            int s = std::clamp(std::min(g_Ctx.InputSelectionStart, g_Ctx.InputSelectionEnd), 0, (int)text.size());
                            int e = std::clamp(std::max(g_Ctx.InputSelectionStart, g_Ctx.InputSelectionEnd), 0, (int)text.size());
                            SetClipboardText(text.substr(s, e - s));
                            if (g_Ctx.KeyPressed['X'] && !isReadOnly) DeleteSelection();
                        }
                        else if (g_Ctx.KeyPressed['C']) {
                            SetClipboardText(text);
                        }
                    }
                }
                if (g_Ctx.KeyPressed['V'] && !isReadOnly) {
                    std::string clip = GetClipboardText();
                    if (!clip.empty()) {
                        std::string filtered;
                        for (char c : clip) {
                            if (c < 32 || c >= 127) continue;
                            if (flags & ShadowInputTextFlags_CharsNoBlank) if (c == ' ' || c == '\t') continue;
                            if (flags & ShadowInputTextFlags_CharsUppercase) if (c >= 'a' && c <= 'z') c -= 32;
                            if (flags & ShadowInputTextFlags_CharsDecimal) if (!(c >= '0' && c <= '9') && c != '.' && c != '+' && c != '-' && c != '*' && c != '/') continue;
                            if (flags & ShadowInputTextFlags_CharsHexadecimal) if (!(c >= '0' && c <= '9') && !(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F')) continue;
                            if (flags & ShadowInputTextFlags_CharsScientific) if (!(c >= '0' && c <= '9') && c != '.' && c != '+' && c != '-' && c != '*' && c != '/' && c != 'e' && c != 'E') continue;
                            filtered += c;
                        }
                        if (!filtered.empty()) {
                            DeleteSelection();
                            text.insert(g_Ctx.InputCursorPos, filtered);
                            g_Ctx.InputCursorPos += static_cast<int>(filtered.size());
                            valueChanged = true;
                        }
                    }
                }
            }
            else {
                if (g_Ctx.KeyPressed[VK_LEFT]) {
                    if (shiftDown) { if (g_Ctx.InputSelectionStart == -1) g_Ctx.InputSelectionStart = g_Ctx.InputCursorPos; }
                    else { g_Ctx.InputSelectionStart = g_Ctx.InputSelectionEnd = -1; }
                    if (g_Ctx.InputCursorPos > 0) g_Ctx.InputCursorPos--;
                    if (shiftDown) g_Ctx.InputSelectionEnd = g_Ctx.InputCursorPos;
                }
                if (g_Ctx.KeyPressed[VK_RIGHT]) {
                    if (shiftDown) { if (g_Ctx.InputSelectionStart == -1) g_Ctx.InputSelectionStart = g_Ctx.InputCursorPos; }
                    else { g_Ctx.InputSelectionStart = g_Ctx.InputSelectionEnd = -1; }
                    if (g_Ctx.InputCursorPos < static_cast<int>(text.size())) g_Ctx.InputCursorPos++;
                    if (shiftDown) g_Ctx.InputSelectionEnd = g_Ctx.InputCursorPos;
                }
                if (g_Ctx.KeyPressed[VK_BACK] && !isReadOnly) {
                    if (!DeleteSelection() && g_Ctx.InputCursorPos > 0) {
                        text.erase(g_Ctx.InputCursorPos - 1, 1);
                        g_Ctx.InputCursorPos--;
                        valueChanged = true;
                    }
                }
                if (g_Ctx.KeyPressed[VK_DELETE] && !isReadOnly) {
                    if (!DeleteSelection() && g_Ctx.InputCursorPos < static_cast<int>(text.size())) {
                        text.erase(g_Ctx.InputCursorPos, 1);
                        valueChanged = true;
                    }
                }
                if (g_Ctx.KeyPressed[VK_HOME]) {
                    if (shiftDown) { if (g_Ctx.InputSelectionStart == -1) g_Ctx.InputSelectionStart = g_Ctx.InputCursorPos; }
                    else { g_Ctx.InputSelectionStart = g_Ctx.InputSelectionEnd = -1; }
                    g_Ctx.InputCursorPos = 0;
                    if (shiftDown) g_Ctx.InputSelectionEnd = g_Ctx.InputCursorPos;
                }
                if (g_Ctx.KeyPressed[VK_END]) {
                    if (shiftDown) { if (g_Ctx.InputSelectionStart == -1) g_Ctx.InputSelectionStart = g_Ctx.InputCursorPos; }
                    else { g_Ctx.InputSelectionStart = g_Ctx.InputSelectionEnd = -1; }
                    g_Ctx.InputCursorPos = static_cast<int>(text.size());
                    if (shiftDown) g_Ctx.InputSelectionEnd = g_Ctx.InputCursorPos;
                }
                if (g_Ctx.KeyPressed[VK_RETURN] || g_Ctx.KeyPressed[VK_ESCAPE]) {
                    if (g_Ctx.KeyPressed[VK_ESCAPE] && (flags & ShadowInputTextFlags_EscapeClearsAll)) {
                        if (!text.empty() && !isReadOnly) {
                            text.clear();
                            g_Ctx.InputCursorPos = 0;
                            g_Ctx.InputSelectionStart = -1;
                            g_Ctx.InputSelectionEnd = -1;
                            valueChanged = true;
                        }
                        else {
                            g_Ctx.ActiveInputId = 0;
                        }
                    }
                    else {
                        g_Ctx.ActiveInputId = 0;
                    }
                }
            }
        }

        Color bgColor = disabled ? g_Ctx.Style.Colors[GuiCol_ControlDisabled] : (isActive ? g_Ctx.Style.Colors[GuiCol_FrameBgHovered] : (hovered ? g_Ctx.Style.Colors[GuiCol_FrameBgHovered] : g_Ctx.Style.Colors[GuiCol_FrameBg]));
        DrawRectFilled(pos, size, bgColor);
        PushClipRect(pos, { pos.x + size.x, pos.y + size.y });

        float textX = pos.x + g_Ctx.Style.FramePadding.x;
        float textY = pos.y + g_Ctx.Style.FramePadding.y;

        std::string displayText = text;
        if (isPassword) displayText.assign(text.size(), '*');

        if (isActive && g_Ctx.InputSelectionStart != -1 && g_Ctx.InputSelectionEnd != -1 && g_Ctx.InputSelectionStart != g_Ctx.InputSelectionEnd) {
            int s = std::clamp(std::min(g_Ctx.InputSelectionStart, g_Ctx.InputSelectionEnd), 0, (int)displayText.size());
            int e = std::clamp(std::max(g_Ctx.InputSelectionStart, g_Ctx.InputSelectionEnd), 0, (int)displayText.size());
            std::string preStr = displayText.substr(0, s);
            std::string selStr = displayText.substr(s, e - s);
            float preW = MeasureTextSize(preStr).x;
            float selW = MeasureTextSize(selStr).x;
            DrawRectFilled({ textX + preW, pos.y + 2.f }, { selW, size.y - 4.f }, g_Ctx.Style.Colors[GuiCol_SliderGrab]);
        }

        if (text.empty() && !isActive && !hint.empty()) {
            Color hintColor = g_Ctx.Style.Colors[GuiCol_TextDisabled];
            DrawTextString(hint, { textX, textY }, hintColor);
        }
        else {
            Color textColor = disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text];
            DrawTextString(displayText, { textX, textY }, textColor);
        }

        if (isActive) {
            std::string preCursor = displayText.substr(0, g_Ctx.InputCursorPos);
            float curW = MeasureTextSize(preCursor).x;
            if ((GetTickCount64() / 500) % 2 == 0) {
                Color cursorColor = disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text];
                DrawRectFilled({ textX + curW, textY }, { 2.f, size.y - 4.f }, cursorColor);
            }
        }
        PopClipRect();

        return valueChanged;
    }

    inline void CheckAndDrawErrors() {
        std::string errorMsg;
        if (g_Ctx.BeginStack > 0) errorMsg = std::format("ERROR: Begin() called {} time(s) without matching End()!", g_Ctx.BeginStack);
        else if (g_Ctx.BeginStack < 0) errorMsg = std::format("ERROR: End() called {} time(s) without matching Begin()!", -g_Ctx.BeginStack);
        else if (g_Ctx.TabBarStack > 0) errorMsg = std::format("ERROR: BeginTabBar() called {} time(s) without matching EndTabBar()!", g_Ctx.TabBarStack);
        else if (g_Ctx.TabBarStack < 0) errorMsg = std::format("ERROR: EndTabBar() called {} time(s) without matching BeginTabBar()!", -g_Ctx.TabBarStack);
        else if (g_Ctx.TabItemStack > 0) errorMsg = std::format("ERROR: BeginTabItem() called {} time(s) without matching EndTabItem()!", g_Ctx.TabItemStack);
        else if (g_Ctx.TabItemStack < 0) errorMsg = std::format("ERROR: EndTabItem() called {} time(s) without matching BeginTabItem()!", -g_Ctx.TabItemStack);
        else if (g_Ctx.TreeNodeStack > 0) errorMsg = std::format("ERROR: TreeNode() called {} time(s) without matching TreePop()!", g_Ctx.TreeNodeStack);
        else if (g_Ctx.TreeNodeStack < 0) errorMsg = std::format("ERROR: TreePop() called {} time(s) without matching TreeNode()!", -g_Ctx.TreeNodeStack);
        else if (g_Ctx.ListBoxStack > 0) errorMsg = std::format("ERROR: BeginListBox() called {} time(s) without matching EndListBox()!", g_Ctx.ListBoxStack);
        else if (g_Ctx.ListBoxStack < 0) errorMsg = std::format("ERROR: EndListBox() called {} time(s) without matching BeginListBox()!", -g_Ctx.ListBoxStack);
        else if (g_Ctx.FontStack.size() > 0) errorMsg = std::format("ERROR: PushFont() called {} time(s) without matching PopFont()!", g_Ctx.FontStack.size());
        else if (g_Ctx.ClipStack.size() > 0) errorMsg = std::format("ERROR: PushClipRect() called {} time(s) without matching PopClipRect()!", g_Ctx.ClipStack.size());
        else if (g_Ctx.DisabledStack.size() > 0) errorMsg = std::format("ERROR: BeginDisabled() called {} time(s) without matching EndDisabled()!", g_Ctx.DisabledStack.size());
        else if (g_Ctx.TextWrapPosStack.size() > 0) errorMsg = std::format("ERROR: PushTextWrapPos() called {} time(s) without matching PopTextWrapPos()!", g_Ctx.TextWrapPosStack.size());
        else if (g_Ctx.InTooltip) errorMsg = "ERROR: BeginTooltip() called without matching EndTooltip()!";

        if (!errorMsg.empty()) {
            bool oldClipping = g_Ctx.ClippingEnabled;
            g_Ctx.ClippingEnabled = false;
            DrawTextString(errorMsg, { 5.f, 5.f }, g_Ctx.Style.Colors[GuiCol_ErrorText]);
            g_Ctx.ClippingEnabled = oldClipping;
        }
    }

    inline void TextColored(Color color, std::string_view text, Vec2 size_arg = { 0.f, 0.f }) {
        if (!g_Ctx.InActiveTab) return;

        Vec2 size = MeasureTextSize(text);
        float itemWidth = size_arg.x > 0.f ? size_arg.x : size.x;
        float itemHeight = size_arg.y > 0.f ? size_arg.y : g_Ctx.ItemHeight;

        if (!IsRectVisible(g_Ctx.Cursor, { itemWidth, itemHeight })) {
            g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
            g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
            return;
        }

        bool disabled = IsDisabled();
        Color drawColor;

        if (disabled) {
            Color disabledColor = g_Ctx.Style.Colors[GuiCol_TextDisabled];
            drawColor = { disabledColor.r, disabledColor.g, disabledColor.b, disabledColor.a * color.a };
        }
        else {
            Color textColor = g_Ctx.Style.Colors[GuiCol_Text];
            drawColor = { color.r, color.g, color.b, color.a * textColor.a };
        }

        DrawTextString(text, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + g_Ctx.Style.FramePadding.y + (itemHeight - g_Ctx.ItemHeight) * 0.5f }, drawColor);

        SetLastItemInfo(g_Ctx.Cursor, { g_Ctx.Cursor.x + itemWidth, g_Ctx.Cursor.y + itemHeight }, ++g_Ctx.WidgetCount, disabled);
        g_Ctx.LastItemMaxX = g_Ctx.Cursor.x + itemWidth;
        g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
    }

    inline void TextWrapped(Color color, std::string_view text, Vec2 size_arg = { 0.f, 0.f }) {
        if (!g_Ctx.InActiveTab) return;

        bool disabled = IsDisabled();
        Color drawColor;
        if (disabled) {
            Color disabledColor = g_Ctx.Style.Colors[GuiCol_TextDisabled];
            drawColor = { disabledColor.r, disabledColor.g, disabledColor.b, disabledColor.a * color.a };
        }
        else {
            Color textColor = g_Ctx.Style.Colors[GuiCol_Text];
            drawColor = { color.r, color.g, color.b, color.a * textColor.a };
        }

        float wrapMaxX = g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - GetRightMargin();
        if (g_Ctx.ClippingEnabled) {
            wrapMaxX = std::min(wrapMaxX, g_Ctx.ClipMax.x);
        }

        if (!g_Ctx.TextWrapPosStack.empty()) {
            wrapMaxX = g_Ctx.TextWrapPosStack.back();
        }

        float maxLineWidth = wrapMaxX - g_Ctx.Cursor.x;
        if (size_arg.x > 0.f) maxLineWidth = size_arg.x;
        if (maxLineWidth <= 1.0f) maxLineWidth = 1.0f;

        std::wstring wtext = ToWString(text);
        float startY = g_Ctx.Cursor.y;
        float maxLineX = g_Ctx.Cursor.x;

        std::wstring currentLine;
        float currentLineWidth = 0.f;

        auto FlushLine = [&](bool addSpacing) {
            if (!currentLine.empty()) {
                int size_needed = WideCharToMultiByte(CP_UTF8, 0, currentLine.c_str(), static_cast<int>(currentLine.size()), nullptr, 0, nullptr, nullptr);
                std::string utf8Line(size_needed, 0);
                WideCharToMultiByte(CP_UTF8, 0, currentLine.c_str(), static_cast<int>(currentLine.size()), &utf8Line[0], size_needed, nullptr, nullptr);

                if (IsRectVisible(g_Ctx.Cursor, { currentLineWidth, g_Ctx.ItemHeight })) {
                    DrawTextString(utf8Line, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + g_Ctx.Style.FramePadding.y }, drawColor);
                }
                maxLineX = std::max(maxLineX, g_Ctx.Cursor.x + currentLineWidth);
            }
            if (addSpacing) {
                g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Style.ItemSpacing.y;
                g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
            }
            currentLine.clear();
            currentLineWidth = 0.f;
            };

        size_t i = 0;
        size_t len = wtext.length();

        while (i < len) {
            if (wtext[i] == L'\n') {
                FlushLine(true);
                i++;
                continue;
            }

            size_t wordStart = i;
            bool isSpace = (wtext[i] == L' ');
            if (isSpace) {
                while (i < len && wtext[i] == L' ') {
                    i++;
                }
            }
            else {
                while (i < len && wtext[i] != L' ' && wtext[i] != L'\n') {
                    i++;
                }
            }

            std::wstring token = wtext.substr(wordStart, i - wordStart);

            if (isSpace) {
                float tokenWidth = 0.f;
                for (wchar_t ch : token) tokenWidth += MeasureCharWidth(ch);
                if (currentLineWidth + tokenWidth <= maxLineWidth) {
                    currentLine += token;
                    currentLineWidth += tokenWidth;
                }
                else {
                    FlushLine(true);
                }
            }
            else {
                float tokenWidth = 0.f;
                for (wchar_t ch : token) tokenWidth += MeasureCharWidth(ch);

                if (currentLineWidth + tokenWidth <= maxLineWidth) {
                    currentLine += token;
                    currentLineWidth += tokenWidth;
                }
                else if (currentLineWidth > 0.f) {
                    FlushLine(true);
                    if (tokenWidth <= maxLineWidth) {
                        currentLine = token;
                        currentLineWidth = tokenWidth;
                    }
                    else {
                        for (wchar_t ch : token) {
                            float chWidth = MeasureCharWidth(ch);
                            if (currentLineWidth + chWidth > maxLineWidth && currentLineWidth > 0.f) FlushLine(true);
                            currentLine += ch;
                            currentLineWidth += chWidth;
                        }
                    }
                }
                else {
                    for (wchar_t ch : token) {
                        float chWidth = MeasureCharWidth(ch);
                        if (currentLineWidth + chWidth > maxLineWidth && currentLineWidth > 0.f) FlushLine(true);
                        currentLine += ch;
                        currentLineWidth += chWidth;
                    }
                }
            }
        }

        if (!currentLine.empty()) {
            FlushLine(false);
            g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Style.ItemSpacing.y;
        }

        float finalY = g_Ctx.Cursor.y - g_Ctx.Style.ItemSpacing.y;
        float blockHeight = finalY - startY;
        if (size_arg.y > 0.f && size_arg.y > blockHeight) {
            g_Ctx.Cursor.y += (size_arg.y - blockHeight);
            finalY = startY + size_arg.y;
        }

        SetLastItemInfo({ g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX, startY }, { maxLineX, finalY }, ++g_Ctx.WidgetCount, disabled);
        g_Ctx.LastItemMaxX = maxLineX;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
    }

    inline void Separator(Vec2 size_arg = { 0.f, 0.f }) {
        if (!g_Ctx.InActiveTab) return;

        float itemHeight = size_arg.y > 0.f ? size_arg.y : 4.f;
        float x1 = g_Ctx.Cursor.x;
        float y = g_Ctx.Cursor.y + itemHeight * 0.5f;
        float x2 = size_arg.x > 0.f ? (g_Ctx.Cursor.x + size_arg.x) : (g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - GetRightMargin());

        if (IsRectVisible({ x1, g_Ctx.Cursor.y }, { x2 - x1, itemHeight })) {
            DrawLine({ x1, y }, { x2, y }, g_Ctx.Style.Colors[GuiCol_Separator], 1.0f);
        }

        SetLastItemInfo({ x1, g_Ctx.Cursor.y }, { x2, g_Ctx.Cursor.y + itemHeight }, ++g_Ctx.WidgetCount, false);

        g_Ctx.LastItemMaxX = x2;
        g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
    }

    inline void Dummy(Vec2 size) {
        if (!g_Ctx.InActiveTab) return;

        SetLastItemInfo(g_Ctx.Cursor, { g_Ctx.Cursor.x + size.x, g_Ctx.Cursor.y + size.y }, ++g_Ctx.WidgetCount, false);
        g_Ctx.LastItemMaxX = g_Ctx.Cursor.x + size.x;

        g_Ctx.Cursor.y += size.y + g_Ctx.Style.ItemSpacing.y;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
    }

    inline void Text(std::string_view text, Vec2 size_arg = { 0.f, 0.f }) {
        if (!g_Ctx.InActiveTab) return;

        Vec2 size = MeasureTextSize(text);
        float itemWidth = size_arg.x > 0.f ? size_arg.x : size.x;
        float itemHeight = size_arg.y > 0.f ? size_arg.y : g_Ctx.ItemHeight;

        if (!IsRectVisible(g_Ctx.Cursor, { itemWidth, itemHeight })) {
            g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
            g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
            return;
        }

        bool disabled = IsDisabled();
        Color drawColor = disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text];

        DrawTextString(text, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + g_Ctx.Style.FramePadding.y + (itemHeight - g_Ctx.ItemHeight) * 0.5f }, drawColor);

        SetLastItemInfo(g_Ctx.Cursor, { g_Ctx.Cursor.x + itemWidth, g_Ctx.Cursor.y + itemHeight }, ++g_Ctx.WidgetCount, disabled);
        g_Ctx.LastItemMaxX = g_Ctx.Cursor.x + itemWidth;
        g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
    }

    inline void TextDisabled(std::string_view text, Vec2 size_arg = { 0.f, 0.f }) {
        if (!g_Ctx.InActiveTab) return;

        Vec2 size = MeasureTextSize(text);
        float itemWidth = size_arg.x > 0.f ? size_arg.x : size.x;
        float itemHeight = size_arg.y > 0.f ? size_arg.y : g_Ctx.ItemHeight;

        if (!IsRectVisible(g_Ctx.Cursor, { itemWidth, itemHeight })) {
            g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
            g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
            return;
        }

        Color drawColor = g_Ctx.Style.Colors[GuiCol_TextDisabled];
        DrawTextString(text, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + g_Ctx.Style.FramePadding.y + (itemHeight - g_Ctx.ItemHeight) * 0.5f }, drawColor);

        SetLastItemInfo(g_Ctx.Cursor, { g_Ctx.Cursor.x + itemWidth, g_Ctx.Cursor.y + itemHeight }, ++g_Ctx.WidgetCount, true);
        g_Ctx.LastItemMaxX = g_Ctx.Cursor.x + itemWidth;
        g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
    }

    inline void NewFrame(SDK::UCanvas* Canvas) {
        g_Ctx.Canvas = Canvas;
        if (!g_Ctx.DefaultFont) {
            if (SDK::UEngine::GetEngine()) { g_Ctx.DefaultFont = SDK::UEngine::GetEngine()->LargeFont; }
        }

        if (!g_Ctx.StyleInitialized) {
            StyleColorsOcean();
            g_Ctx.StyleInitialized = true;
        }

        g_Ctx.FontStack.clear();
        UpdateItemHeight();

        g_Ctx.InActiveTab = true;
        g_Ctx.BeginStack = 0;
        g_Ctx.TabBarStack = 0;
        g_Ctx.TabItemStack = 0;
        g_Ctx.TreeNodeStack = 0;
        g_Ctx.ListBoxStack = 0;
        g_Ctx.IndentX = 0.f;
        g_Ctx.TreeNodeNoIndentStack.clear();

        g_Ctx.DisabledStack.clear();

        g_Ctx.TabBarHoverRects = std::move(g_Ctx.TabBarHoverRectsPending);
        g_Ctx.TabBarHoverRectsPending.clear();

        float dx = g_Ctx.MousePos.x - g_Ctx.MousePosPrev.x;
        float dy = g_Ctx.MousePos.y - g_Ctx.MousePosPrev.y;
        if (dx * dx + dy * dy > 4.0f) {
            g_Ctx.MouseStationaryStartTime = GetTickCount64();
            g_Ctx.MouseIsStationary = false;
            g_Ctx.MousePosPrev = g_Ctx.MousePos;
        }
        else {
            if (GetTickCount64() - g_Ctx.MouseStationaryStartTime >= 150) {
                g_Ctx.MouseIsStationary = true;
            }
        }

        g_Ctx.HoveredIdPreviousFrame = g_Ctx.HoveredIdCurrentFrame;
        g_Ctx.HoveredIdCurrentFrame = 0;
        g_Ctx.LastHoveredIdEval = 0;
        g_Ctx.WidgetCount = 0;

        // [新增] 更新 ListBox 焦点追踪
        g_Ctx.HoveredListBoxIdPreviousFrame = g_Ctx.HoveredListBoxIdCurrentFrame;
        g_Ctx.HoveredListBoxIdCurrentFrame = 0;

        if (GetTickCount64() > g_Ctx.SharedDelayExpirationTime) {
            g_Ctx.SharedDelayActive = false;
        }
    }

    inline void RenderPopups() {
        if (g_Ctx.ActiveDropdownId != 0 && g_Ctx.DropdownCurrentItem) {
            float dropWidth = g_Ctx.DropdownSize.x;
            float dropHeight = g_Ctx.DropdownItems.size() * g_Ctx.ItemHeight;
            Vec2 dropPos = g_Ctx.DropdownPos;

            DrawRect({ dropPos.x - 1.f, dropPos.y - 1.f }, { dropWidth + 2.f, dropHeight + 2.f }, g_Ctx.Style.Colors[GuiCol_PopupBorder]);
            DrawRect(dropPos, { dropWidth, dropHeight }, g_Ctx.Style.Colors[GuiCol_PopupBg]);

            bool hoveringDropdown = IsMouseHoveringRaw(dropPos, { dropWidth, dropHeight });

            for (size_t i = 0; i < g_Ctx.DropdownItems.size(); ++i) {
                Vec2 itemPos = { dropPos.x, dropPos.y + i * g_Ctx.ItemHeight };
                bool itemHovered = IsMouseHoveringRaw(itemPos, { dropWidth, g_Ctx.ItemHeight });

                // 判断此项是否为当前的选中状态（Hotkey当前模式 / Combo当前选择项）
                bool isCurrentItem = (*g_Ctx.DropdownCurrentItem == static_cast<int>(i));

                if (itemHovered) {
                    // 鼠标悬停时的最高亮背景色
                    DrawRectFilled(itemPos, { dropWidth, g_Ctx.ItemHeight }, g_Ctx.Style.Colors[GuiCol_FrameBgHovered]);
                    if (g_Ctx.MouseClicked) {
                        *g_Ctx.DropdownCurrentItem = static_cast<int>(i);
                        g_Ctx.ActiveDropdownId = 0;
                        g_Ctx.MouseClicked = false;
                    }
                }
                else if (isCurrentItem) {
                    // 当处于当前状态、且没有被鼠标指向时，使用较暗的高亮背景色
                    DrawRectFilled(itemPos, { dropWidth, g_Ctx.ItemHeight }, g_Ctx.Style.Colors[GuiCol_DropdownActive]);
                }

                Color textColor = isCurrentItem ? g_Ctx.Style.Colors[GuiCol_TextHighlight] : g_Ctx.Style.Colors[GuiCol_Text];
                DrawTextString(g_Ctx.DropdownItems[i], { itemPos.x + g_Ctx.Style.FramePadding.x, itemPos.y + g_Ctx.Style.FramePadding.y }, textColor);
            }

            if (g_Ctx.MouseClicked && !hoveringDropdown) { g_Ctx.ActiveDropdownId = 0; }
            if (hoveringDropdown) { g_Ctx.MouseClicked = false; }
        }

        if (g_Ctx.ActiveColorPickerId != 0 && g_Ctx.ColorPickerR) {
            Vec2 popupPos = g_Ctx.ColorPickerPos;

            float padding = g_Ctx.Style.CPPadding;
            float svSize = g_Ctx.Style.CPSVSize;
            float hueWidth = g_Ctx.Style.CPHueWidth;
            float alphaWidth = g_Ctx.Style.CPAlphaWidth;
            float spacing = g_Ctx.Style.CPSpacing;

            float hexBoxHeight = std::max(24.f, g_Ctx.ItemHeight + 4.f);
            float popupWidth = padding * 2.f + svSize + spacing * 2.f + hueWidth + alphaWidth;
            float popupHeight = padding * 2.f + svSize + spacing + hexBoxHeight;

            bool popupHovered = IsMouseHoveringRaw({ popupPos.x - 2.f, popupPos.y - 2.f }, { popupWidth + 4.f, popupHeight + 4.f });
            if (g_Ctx.MouseClicked && !popupHovered) {
                g_Ctx.ActiveColorPickerId = 0;
                g_Ctx.ActiveInputId = 0;
            }

            if (g_Ctx.ActiveColorPickerId != 0) {
                Vec2 svPos = { popupPos.x + padding, popupPos.y + padding };
                Vec2 huePos = { svPos.x + svSize + spacing, svPos.y };
                Vec2 alphaPos = { huePos.x + hueWidth + spacing, svPos.y };
                Vec2 hexPos = { svPos.x, svPos.y + svSize + spacing };
                Vec2 hexSize = { popupWidth - padding * 2.f, hexBoxHeight };

                bool svHovered = IsMouseHoveringRaw(svPos, { svSize, svSize });
                bool hueHovered = IsMouseHoveringRaw(huePos, { hueWidth, svSize });
                bool alphaHovered = IsMouseHoveringRaw(alphaPos, { alphaWidth, svSize });
                bool hexHovered = IsMouseHoveringRaw(hexPos, hexSize);

                if (g_Ctx.MouseDown) {
                    if (!g_Ctx.IsDraggingSV && !g_Ctx.IsDraggingHue && !g_Ctx.IsDraggingAlpha && !g_Ctx.IsDraggingColorPicker) {
                        if (svHovered) g_Ctx.IsDraggingSV = true;
                        else if (hueHovered) g_Ctx.IsDraggingHue = true;
                        else if (alphaHovered) g_Ctx.IsDraggingAlpha = true;
                        else if (popupHovered && !hexHovered) {
                            if (g_Ctx.MouseClicked) {
                                g_Ctx.IsDraggingColorPicker = true;
                                g_Ctx.ColorPickerDragOffset.x = g_Ctx.MousePos.x - popupPos.x;
                                g_Ctx.ColorPickerDragOffset.y = g_Ctx.MousePos.y - popupPos.y;
                                g_Ctx.MouseClicked = false;
                            }
                        }
                    }

                    if (g_Ctx.IsDraggingSV) {
                        g_Ctx.ColorPickerS = std::clamp((g_Ctx.MousePos.x - svPos.x) / svSize, 0.f, 1.f);
                        g_Ctx.ColorPickerV = 1.0f - std::clamp((g_Ctx.MousePos.y - svPos.y) / svSize, 0.f, 1.f);
                        HSVtoRGB(g_Ctx.ColorPickerH, g_Ctx.ColorPickerS, g_Ctx.ColorPickerV, *g_Ctx.ColorPickerR, *g_Ctx.ColorPickerG, *g_Ctx.ColorPickerB);
                    }
                    else if (g_Ctx.IsDraggingHue) {
                        g_Ctx.ColorPickerH = std::clamp((g_Ctx.MousePos.y - huePos.y) / svSize, 0.f, 1.f);
                        HSVtoRGB(g_Ctx.ColorPickerH, g_Ctx.ColorPickerS, g_Ctx.ColorPickerV, *g_Ctx.ColorPickerR, *g_Ctx.ColorPickerG, *g_Ctx.ColorPickerB);
                    }
                    else if (g_Ctx.IsDraggingAlpha) {
                        if (g_Ctx.ColorPickerA) {
                            *g_Ctx.ColorPickerA = 1.0f - std::clamp((g_Ctx.MousePos.y - alphaPos.y) / svSize, 0.f, 1.f);
                        }
                    }
                    else if (g_Ctx.IsDraggingColorPicker) {
                        g_Ctx.ColorPickerPos.x = g_Ctx.MousePos.x - g_Ctx.ColorPickerDragOffset.x;
                        g_Ctx.ColorPickerPos.y = g_Ctx.MousePos.y - g_Ctx.ColorPickerDragOffset.y;
                        popupPos = g_Ctx.ColorPickerPos;

                        svPos = { popupPos.x + padding, popupPos.y + padding };
                        huePos = { svPos.x + svSize + spacing, svPos.y };
                        alphaPos = { huePos.x + hueWidth + spacing, svPos.y };
                        hexPos = { svPos.x, svPos.y + svSize + spacing };
                        popupHovered = IsMouseHoveringRaw({ popupPos.x - 2.f, popupPos.y - 2.f }, { popupWidth + 4.f, popupHeight + 4.f });
                    }
                }
                else {
                    g_Ctx.IsDraggingSV = false;
                    g_Ctx.IsDraggingHue = false;
                    g_Ctx.IsDraggingAlpha = false;
                    g_Ctx.IsDraggingColorPicker = false;
                }

                DrawRect({ popupPos.x - 2.f, popupPos.y - 2.f }, { popupWidth + 4.f, popupHeight + 4.f }, g_Ctx.Style.Colors[GuiCol_PopupBorder]);
                DrawRect(popupPos, { popupWidth, popupHeight }, g_Ctx.Style.Colors[GuiCol_PopupBg]);

                // 提取各样式颜色，消除硬编码
                float pickerAlpha = g_Ctx.Style.Colors[GuiCol_ColorPickerLight].a; // 全局 Alpha 参考
                Color shadowCol = g_Ctx.Style.Colors[GuiCol_ColorPickerShadow];
                Color cbLight = g_Ctx.Style.Colors[GuiCol_CheckerboardLight];
                Color cbDark = g_Ctx.Style.Colors[GuiCol_CheckerboardDark];

                int svSteps = std::max(2, static_cast<int>(svSize));
                float svStepSize = svSize / svSteps;

                // 1. 绘制调色SV面板
                for (int i = 0; i < svSteps; ++i) {
                    float s = (float)i / (svSteps - 1);
                    float r, g, b;
                    HSVtoRGB(g_Ctx.ColorPickerH, s, 1.0f, r, g, b);
                    DrawRectFilled({ svPos.x + i * svStepSize, svPos.y }, { svStepSize, svSize }, { r, g, b, pickerAlpha });
                }
                for (int j = 0; j < svSteps; ++j) {
                    float v = 1.0f - (float)j / (svSteps - 1);
                    float alpha = 1.0f - v;
                    // 使用 shadowCol 代替硬编码的黑块渐变层
                    DrawRectFilled({ svPos.x, svPos.y + j * svStepSize }, { svSize, svStepSize }, { shadowCol.r, shadowCol.g, shadowCol.b, alpha * shadowCol.a });
                }

                // 2. 绘制色相(Hue)长条
                int hueSteps = std::max(2, static_cast<int>(svSize));
                float hueStepSize = svSize / hueSteps;
                for (int i = 0; i < hueSteps; ++i) {
                    float h = (float)i / (hueSteps - 1);
                    float r, g, b;
                    HSVtoRGB(h, 1.f, 1.f, r, g, b);
                    DrawRectFilled({ huePos.x, huePos.y + i * hueStepSize }, { hueWidth, hueStepSize }, { r, g, b, pickerAlpha });
                }

                // 3. 绘制透明度(Alpha)长条
                int alphaSteps = std::max(2, static_cast<int>(svSize));
                float alphaStepSize = svSize / alphaSteps;
                int checkerSize = 5;

                for (int i = 0; i < alphaSteps; ++i) {
                    float a = 1.0f - (float)i / (alphaSteps - 1);
                    float drawY = alphaPos.y + i * alphaStepSize;

                    for (int x = 0; x < alphaWidth; x += checkerSize) {
                        float drawX = alphaPos.x + x;
                        float w = std::min((float)checkerSize, alphaWidth - x);

                        int checkY = static_cast<int>((drawY - alphaPos.y) / checkerSize);
                        int checkX = x / checkerSize;
                        bool isWhite = (checkX + checkY) % 2 == 0;
                        Color bgCol = isWhite ? cbLight : cbDark;

                        float finalR = *g_Ctx.ColorPickerR * a + bgCol.r * (1.f - a);
                        float finalG = *g_Ctx.ColorPickerG * a + bgCol.g * (1.f - a);
                        float finalB = *g_Ctx.ColorPickerB * a + bgCol.b * (1.f - a);

                        DrawRectFilled({ drawX, drawY }, { w, alphaStepSize }, { finalR, finalG, finalB, pickerAlpha });
                    }
                }

                size_t hexId = HashString("CPHexInput");
                if (g_Ctx.ActiveInputId != hexId) {
                    uint8_t r8 = static_cast<uint8_t>(*g_Ctx.ColorPickerR * 255.f);
                    uint8_t g8 = static_cast<uint8_t>(*g_Ctx.ColorPickerG * 255.f);
                    uint8_t b8 = static_cast<uint8_t>(*g_Ctx.ColorPickerB * 255.f);
                    uint8_t a8 = g_Ctx.ColorPickerA ? static_cast<uint8_t>(*g_Ctx.ColorPickerA * 255.f) : 255;
                    g_Ctx.InputBuffers[hexId] = std::format("{:02X}{:02X}{:02X}{:02X}", r8, g8, b8, a8);
                }

                bool hexChanged = InputTextEx(hexId, hexPos, hexSize, g_Ctx.InputBuffers[hexId], ShadowInputTextFlags_CharsHexadecimal | ShadowInputTextFlags_CharsUppercase, true);
                if (hexChanged) {
                    ApplyHexInput();
                }

                Vec2 cursorSV = { svPos.x + g_Ctx.ColorPickerS * svSize, svPos.y + (1.f - g_Ctx.ColorPickerV) * svSize };
                DrawRect({ cursorSV.x - 4.f, cursorSV.y - 4.f }, { 8.f, 8.f }, g_Ctx.Style.Colors[GuiCol_ColorPickerDark]);
                DrawRect({ cursorSV.x - 3.f, cursorSV.y - 3.f }, { 6.f, 6.f }, g_Ctx.Style.Colors[GuiCol_ColorPickerLight]);

                // 修复3：将吸管内部颜色块由 DrawRect (线框) 改为 DrawRectFilled (实心块)，并将写死的 Alpha 替换为具有动画效果的 pickerAlpha
                DrawRectFilled({ cursorSV.x - 2.f, cursorSV.y - 2.f }, { 4.f, 4.f }, { *g_Ctx.ColorPickerR, *g_Ctx.ColorPickerG, *g_Ctx.ColorPickerB, pickerAlpha });

                Vec2 cursorHue = { huePos.x - 2.f, huePos.y + g_Ctx.ColorPickerH * svSize - 2.f };
                DrawRect(cursorHue, { hueWidth + 4.f, 4.f }, g_Ctx.Style.Colors[GuiCol_ColorPickerDark]);
                DrawRect({ cursorHue.x + 1.f, cursorHue.y + 1.f }, { hueWidth + 2.f, 2.f }, g_Ctx.Style.Colors[GuiCol_ColorPickerLight]);

                if (g_Ctx.ColorPickerA) {
                    Vec2 cursorAlpha = { alphaPos.x - 2.f, alphaPos.y + (1.f - *g_Ctx.ColorPickerA) * svSize - 2.f };
                    DrawRect(cursorAlpha, { alphaWidth + 4.f, 4.f }, g_Ctx.Style.Colors[GuiCol_ColorPickerDark]);
                    DrawRect({ cursorAlpha.x + 1.f, cursorAlpha.y + 1.f }, { alphaWidth + 2.f, 2.f }, g_Ctx.Style.Colors[GuiCol_ColorPickerLight]);
                }

                if (popupHovered) g_Ctx.MouseClicked = false;
            }
        }
    }

    inline bool Begin(std::string_view name, ShadowWindowFlags flags = ShadowWindowFlags_None) {
        std::string_view display; size_t id;
        ParseLabel(name, display, id);
        g_Ctx.BeginStack++;
        g_Ctx.IsScrollApplied = false;
        g_Ctx.CurrentWindowFlags = flags;

        float min_x = g_Ctx.Style.WindowMinSize.x;
        float min_y = g_Ctx.Style.WindowMinSize.y;
        float max_x = FLT_MAX;
        float max_y = FLT_MAX;

        if (g_Ctx.HasWindowSizeConstraints) {
            min_x = std::max(min_x, g_Ctx.WindowSizeConstraintMin.x);
            min_y = std::max(min_y, g_Ctx.WindowSizeConstraintMin.y);
            max_x = std::max(min_x, g_Ctx.WindowSizeConstraintMax.x);
            max_y = std::max(min_y, g_Ctx.WindowSizeConstraintMax.y);
            g_Ctx.HasWindowSizeConstraints = false;
        }

        g_Ctx.WindowSize.x = std::clamp(g_Ctx.WindowSize.x, min_x, max_x);
        g_Ctx.WindowSize.y = std::clamp(g_Ctx.WindowSize.y, min_y, max_y);

        bool noResize = (flags & ShadowWindowFlags_NoResize) != 0;
        bool noMove = (flags & ShadowWindowFlags_NoMove) != 0;
        bool noScrollbar = (flags & ShadowWindowFlags_NoScrollbar) != 0;
        bool noTitleBar = (flags & ShadowWindowFlags_NoTitleBar) != 0;
        bool noMouseInputs = (flags & ShadowWindowFlags_NoMouseInputs) != 0;

        float titleBarHeight = noTitleBar ? 0.f : std::max(30.f, g_Ctx.ItemHeight + 10.f);
        Vec2 wholeWindowSize = g_Ctx.WindowSize;

        bool hoveringWholeWindow = noMouseInputs ? false : IsMouseHovering(g_Ctx.WindowPos, wholeWindowSize);

        float triSize = g_Ctx.Style.ResizeGripSize;
        Vec2 triPos = { g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - triSize, g_Ctx.WindowPos.y + g_Ctx.WindowSize.y - triSize };
        g_Ctx.IsHoveringResize = (!noResize && !noMouseInputs) ? IsMouseHoveringRaw(triPos, { triSize, triSize }) : false;

        // [修改] 增加 g_Ctx.DraggingListBoxScrollId != 0 阻断主菜单被拖拽
        bool isOtherDragging = (g_Ctx.DraggingSliderId != 0) || g_Ctx.IsDraggingSV || g_Ctx.IsDraggingHue || g_Ctx.IsDraggingAlpha || g_Ctx.IsDraggingColorPicker || g_Ctx.IsDraggingScrollbar || g_Ctx.DraggingTabId != 0 || g_Ctx.DraggingTabBarScrollId != 0 || g_Ctx.DraggingListBoxScrollId != 0;

        if (hoveringWholeWindow && g_Ctx.MouseClicked) {
            g_Ctx.ActiveInputId = 0;
        }

        float viewHeight = (g_Ctx.WindowPos.y + g_Ctx.WindowSize.y) - g_Ctx.ContentStartY - g_Ctx.Style.ResizeGripSize - 4.f;
        if (viewHeight < 10.f) viewHeight = 10.f;
        float maxScroll = std::max(0.f, g_Ctx.ContentHeight - viewHeight);
        g_Ctx.ScrollY = std::clamp(g_Ctx.ScrollY, 0.f, maxScroll);

        bool hasPopupOpen = g_Ctx.ActiveDropdownId != 0 || g_Ctx.ActiveColorPickerId != 0;

        bool hoveringAnyTabBar = false;
        for (auto& [rectPos, rectSize] : g_Ctx.TabBarHoverRects) {
            if (IsMouseHoveringRaw(rectPos, rectSize)) {
                hoveringAnyTabBar = true;
                break;
            }
        }

        bool overListBox = (g_Ctx.HoveredListBoxIdPreviousFrame != 0);

        if (!noMouseInputs && hoveringWholeWindow && g_Ctx.MouseWheel != 0.f && !hasPopupOpen && !hoveringAnyTabBar && !overListBox) {
            g_Ctx.ScrollY -= g_Ctx.MouseWheel * 30.f;
            g_Ctx.ScrollY = std::clamp(g_Ctx.ScrollY, 0.f, maxScroll);
        }

        bool hoveringScrollbar = false;
        float scrollbarWidth = g_Ctx.Style.ScrollbarSize;
        float scrollbarMarginRight = g_Ctx.Style.ScrollbarMargin;
        if (!noScrollbar && g_Ctx.ContentHeight > viewHeight) {
            Vec2 trackPos = { g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - scrollbarWidth - scrollbarMarginRight, g_Ctx.ContentStartY };
            Vec2 trackSize = { scrollbarWidth, viewHeight };

            float thumbHeight = std::max(20.f, (viewHeight / g_Ctx.ContentHeight) * trackSize.y);
            float thumbY = trackPos.y + (g_Ctx.ScrollY / maxScroll) * (trackSize.y - thumbHeight);
            Vec2 thumbPos = { trackPos.x, thumbY };
            Vec2 thumbSize = { scrollbarWidth, thumbHeight };

            bool hoveringThumb = noMouseInputs ? false : IsMouseHoveringRaw(thumbPos, thumbSize);
            bool hoveringTrack = noMouseInputs ? false : IsMouseHoveringRaw(trackPos, trackSize);

            if (hoveringTrack || hoveringThumb) { hoveringScrollbar = true; }

            if (!overListBox) {
                if (!noMouseInputs && g_Ctx.MouseClicked && hoveringThumb) {
                    g_Ctx.IsDraggingScrollbar = true;
                    g_Ctx.ScrollDragOffset = g_Ctx.MousePos.y - thumbPos.y;
                }
                else if (!noMouseInputs && g_Ctx.MouseClicked && hoveringTrack) {
                    if (g_Ctx.MousePos.y < thumbPos.y) g_Ctx.ScrollY -= viewHeight;
                    else g_Ctx.ScrollY += viewHeight;
                    g_Ctx.ScrollY = std::clamp(g_Ctx.ScrollY, 0.f, maxScroll);
                }
            }

            if (g_Ctx.IsDraggingScrollbar) {
                if (g_Ctx.MouseDown) {
                    float newThumbY = g_Ctx.MousePos.y - g_Ctx.ScrollDragOffset;
                    float ratio = (newThumbY - trackPos.y) / std::max(1.f, trackSize.y - thumbHeight);
                    g_Ctx.ScrollY = std::clamp(ratio * maxScroll, 0.f, maxScroll);
                }
                else {
                    g_Ctx.IsDraggingScrollbar = false;
                }
            }
        }
        else {
            g_Ctx.IsDraggingScrollbar = false;
        }

        if (!noMove && !noMouseInputs && !g_Ctx.IsDragging && hoveringWholeWindow && g_Ctx.MouseClicked && !g_Ctx.IsHoveringResize && !isOtherDragging && !hoveringScrollbar) {
            g_Ctx.IsDragging = true;
            g_Ctx.DragOffset.x = g_Ctx.MousePos.x - g_Ctx.WindowPos.x;
            g_Ctx.DragOffset.y = g_Ctx.MousePos.y - g_Ctx.WindowPos.y;
        }
        if (g_Ctx.IsDragging && isOtherDragging) g_Ctx.IsDragging = false;

        if (!noResize && !noMouseInputs && !g_Ctx.IsResizing && g_Ctx.IsHoveringResize && g_Ctx.MouseClicked && !isOtherDragging) {
            g_Ctx.IsResizing = true;
            g_Ctx.ResizeStartPos = g_Ctx.MousePos;
            g_Ctx.ResizeStartSize = g_Ctx.WindowSize;
        }
        if (g_Ctx.IsResizing && isOtherDragging) g_Ctx.IsResizing = false;

        if (!noMove && !noMouseInputs && g_Ctx.IsDragging) {
            g_Ctx.WindowPos.x = g_Ctx.MousePos.x - g_Ctx.DragOffset.x;
            g_Ctx.WindowPos.y = g_Ctx.MousePos.y - g_Ctx.DragOffset.y;
        }

        if (!noResize && !noMouseInputs && g_Ctx.IsResizing) {
            Vec2 delta = { g_Ctx.MousePos.x - g_Ctx.ResizeStartPos.x, g_Ctx.MousePos.y - g_Ctx.ResizeStartPos.y };
            g_Ctx.WindowSize.x = std::clamp(g_Ctx.ResizeStartSize.x + delta.x, min_x, max_x);
            g_Ctx.WindowSize.y = std::clamp(g_Ctx.ResizeStartSize.y + delta.y, min_y, max_y);
            if (!g_Ctx.MouseDown) g_Ctx.IsResizing = false;
        }

        DrawRect(g_Ctx.WindowPos, g_Ctx.WindowSize, g_Ctx.Style.Colors[GuiCol_WindowBg]);

        if (!noTitleBar) {
            DrawRect(g_Ctx.WindowPos, { g_Ctx.WindowSize.x, titleBarHeight }, g_Ctx.Style.Colors[GuiCol_TitleBarBg]);

            {
                float titleTextX = g_Ctx.WindowPos.x + 10.f;
                if (flags & ShadowWindowFlags_TextAlignCenter) {
                    float textW = MeasureTextSize(display).x;
                    titleTextX = g_Ctx.WindowPos.x + (g_Ctx.WindowSize.x - textW) * 0.5f;
                }
                else if (flags & ShadowWindowFlags_TextAlignRight) {
                    float textW = MeasureTextSize(display).x;
                    titleTextX = g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - 10.f - textW;
                }
                DrawTextString(display, { titleTextX, g_Ctx.WindowPos.y + 7.f }, g_Ctx.Style.Colors[GuiCol_Text]);
            }
        }

        g_Ctx.IndentX = 0.f;
        g_Ctx.Cursor = { g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX, g_Ctx.WindowPos.y + titleBarHeight + g_Ctx.Style.WindowPadding.y };
        g_Ctx.ContentStartY = g_Ctx.Cursor.y;

        PushClipRect(
            g_Ctx.WindowPos,
            { g_Ctx.WindowPos.x + g_Ctx.WindowSize.x, g_Ctx.WindowPos.y + g_Ctx.WindowSize.y }
        );
        return true;
    }

    inline void BeginTooltip() {
        g_Ctx.InTooltip = true;

        g_Ctx.BackupWindowPos = g_Ctx.WindowPos;
        g_Ctx.BackupWindowSize = g_Ctx.WindowSize;
        g_Ctx.BackupCursor = g_Ctx.Cursor;
        g_Ctx.BackupContentStartY = g_Ctx.ContentStartY;
        g_Ctx.BackupScrollY = g_Ctx.ScrollY;
        g_Ctx.BackupLastItemMaxX = g_Ctx.LastItemMaxX;
        g_Ctx.BackupIsScrollApplied = g_Ctx.IsScrollApplied;
        g_Ctx.BackupCurrentWindowFlags = g_Ctx.CurrentWindowFlags;
        g_Ctx.BackupIndentX = g_Ctx.IndentX;

        g_Ctx.BackupClippingEnabled = g_Ctx.ClippingEnabled;
        g_Ctx.BackupClipMin = g_Ctx.ClipMin;
        g_Ctx.BackupClipMax = g_Ctx.ClipMax;
        g_Ctx.BackupClipStack = g_Ctx.ClipStack;

        g_Ctx.ClippingEnabled = false;
        g_Ctx.ClipStack.clear();

        g_Ctx.WindowPos = { g_Ctx.MousePos.x + 15.f, g_Ctx.MousePos.y + 15.f };

        size_t tooltipKey = g_Ctx.HoveredIdCurrentFrame;
        Vec2 bgSize = g_Ctx.TooltipSizeCache[tooltipKey];
        if (bgSize.x < 10.f) bgSize.x = 30.f;
        if (bgSize.y < 10.f) bgSize.y = 30.f;

        DrawRect({ g_Ctx.WindowPos.x - 1.f, g_Ctx.WindowPos.y - 1.f }, { bgSize.x + 2.f, bgSize.y + 2.f }, g_Ctx.Style.Colors[GuiCol_PopupBorder]);
        DrawRectFilled(g_Ctx.WindowPos, bgSize, g_Ctx.Style.Colors[GuiCol_PopupBg]);

        g_Ctx.WindowSize = bgSize;
        g_Ctx.IndentX = 0.f;
        g_Ctx.Cursor = { g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX, g_Ctx.WindowPos.y + g_Ctx.Style.WindowPadding.y };
        g_Ctx.ContentStartY = g_Ctx.Cursor.y;
        g_Ctx.ScrollY = 0.f;
        g_Ctx.IsScrollApplied = false;
        g_Ctx.CurrentWindowFlags = ShadowWindowFlags_NoResize | ShadowWindowFlags_NoMove | ShadowWindowFlags_NoScrollbar;

        PushClipRect(g_Ctx.WindowPos, { g_Ctx.WindowPos.x + bgSize.x, g_Ctx.WindowPos.y + bgSize.y });
        g_Ctx.CurrentTooltipSize = { 0.f, 0.f };
    }

    inline void EndTooltip() {
        PopClipRect();

        size_t tooltipKey = g_Ctx.HoveredIdCurrentFrame;
        g_Ctx.TooltipSizeCache[tooltipKey] = g_Ctx.CurrentTooltipSize;

        g_Ctx.WindowPos = g_Ctx.BackupWindowPos;
        g_Ctx.WindowSize = g_Ctx.BackupWindowSize;
        g_Ctx.Cursor = g_Ctx.BackupCursor;
        g_Ctx.ContentStartY = g_Ctx.BackupContentStartY;
        g_Ctx.ScrollY = g_Ctx.BackupScrollY;
        g_Ctx.LastItemMaxX = g_Ctx.BackupLastItemMaxX;
        g_Ctx.IsScrollApplied = g_Ctx.BackupIsScrollApplied;
        g_Ctx.CurrentWindowFlags = g_Ctx.BackupCurrentWindowFlags;
        g_Ctx.IndentX = g_Ctx.BackupIndentX;

        g_Ctx.ClippingEnabled = g_Ctx.BackupClippingEnabled;
        g_Ctx.ClipMin = g_Ctx.BackupClipMin;
        g_Ctx.ClipMax = g_Ctx.BackupClipMax;
        g_Ctx.ClipStack = g_Ctx.BackupClipStack;

        g_Ctx.InTooltip = false;
    }

    inline void RenderTooltipCommands() {
        if (g_Ctx.TooltipDrawCommands.empty()) return;

        bool backupInTooltip = g_Ctx.InTooltip;
        bool backupClipping = g_Ctx.ClippingEnabled;
        Vec2 backupClipMin = g_Ctx.ClipMin;
        Vec2 backupClipMax = g_Ctx.ClipMax;
        std::vector<FontContext> backupFontStack = g_Ctx.FontStack;

        g_Ctx.InTooltip = false; // 临时解封禁止绘制锁定

        for (const auto& cmd : g_Ctx.TooltipDrawCommands) {
            // 恢复该指令生成时局部的裁剪区域状态
            g_Ctx.ClippingEnabled = cmd.clippingEnabled;
            g_Ctx.ClipMin = cmd.clipMin;
            g_Ctx.ClipMax = cmd.clipMax;

            if (cmd.type == ShadowDrawCmdType::Line) {
                DrawLine(cmd.pos, cmd.size, cmd.color, cmd.thickness);
            }
            else if (cmd.type == ShadowDrawCmdType::Rect) {
                DrawRect(cmd.pos, cmd.size, cmd.color);
            }
            else if (cmd.type == ShadowDrawCmdType::RectFilled) {
                DrawRectFilled(cmd.pos, cmd.size, cmd.color);
            }
            else if (cmd.type == ShadowDrawCmdType::Text) {
                // 恢复对应的独立字体栈
                g_Ctx.FontStack.push_back({ cmd.font, cmd.fontScale });
                DrawTextString(cmd.text, cmd.pos, cmd.color);
                g_Ctx.FontStack.pop_back();
            }
            // [新增] 处理 Tooltip 延迟绘制的三角形
            else if (cmd.type == ShadowDrawCmdType::TriangleFilled) {
                DrawTriangleFilled(cmd.p1, cmd.p2, cmd.p3, cmd.color);
            }
        }

        g_Ctx.TooltipDrawCommands.clear();

        g_Ctx.InTooltip = backupInTooltip;
        g_Ctx.ClippingEnabled = backupClipping;
        g_Ctx.ClipMin = backupClipMin;
        g_Ctx.ClipMax = backupClipMax;
        g_Ctx.FontStack = backupFontStack;
    }

    inline void End() {
        g_Ctx.BeginStack--;

        float actualCursorY = g_Ctx.Cursor.y + (g_Ctx.IsScrollApplied ? g_Ctx.ScrollY : 0.f);
        g_Ctx.ContentHeight = actualCursorY - g_Ctx.ContentStartY;

        PopClipRect();

        bool noResize = (g_Ctx.CurrentWindowFlags & ShadowWindowFlags_NoResize) != 0;
        bool noScrollbar = (g_Ctx.CurrentWindowFlags & ShadowWindowFlags_NoScrollbar) != 0;

        float viewHeight = (g_Ctx.WindowPos.y + g_Ctx.WindowSize.y) - g_Ctx.ContentStartY - g_Ctx.Style.ResizeGripSize - 4.f;
        if (viewHeight < 10.f) viewHeight = 10.f;

        g_Ctx.CurrentScrollbarWidth = 0.f;

        if (!noScrollbar && g_Ctx.ContentHeight > viewHeight) {
            g_Ctx.CurrentScrollbarWidth = g_Ctx.Style.ScrollbarSize;

            float maxScroll = g_Ctx.ContentHeight - viewHeight;
            float scrollbarWidth = g_Ctx.Style.ScrollbarSize;
            float scrollbarMarginRight = g_Ctx.Style.ScrollbarMargin;
            Vec2 trackPos = { g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - scrollbarWidth - scrollbarMarginRight, g_Ctx.ContentStartY };
            Vec2 trackSize = { scrollbarWidth, viewHeight };

            DrawRect(trackPos, trackSize, g_Ctx.Style.Colors[GuiCol_FrameBg]);

            float thumbHeight = std::max(20.f, (viewHeight / g_Ctx.ContentHeight) * trackSize.y);
            float thumbY = trackPos.y + (g_Ctx.ScrollY / maxScroll) * (trackSize.y - thumbHeight);
            Vec2 thumbPos = { trackPos.x, thumbY };
            Vec2 thumbSize = { scrollbarWidth, thumbHeight };

            bool hoveringThumb = IsMouseHoveringRaw(thumbPos, thumbSize);
            Color thumbColor = g_Ctx.IsDraggingScrollbar ? g_Ctx.Style.Colors[GuiCol_SliderGrab] : (hoveringThumb ? g_Ctx.Style.Colors[GuiCol_FrameBgHovered] : g_Ctx.Style.Colors[GuiCol_Border]);
            DrawRect(thumbPos, thumbSize, thumbColor);
        }

        if (!noResize) {
            float triSize = g_Ctx.Style.ResizeGripSize;
            float x = g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - triSize;
            float y = g_Ctx.WindowPos.y + g_Ctx.WindowSize.y - triSize;

            Color triColor = g_Ctx.IsResizing
                ? g_Ctx.Style.Colors[GuiCol_ResizeGripActive]
                : (g_Ctx.IsHoveringResize ? g_Ctx.Style.Colors[GuiCol_ResizeGripHovered] : g_Ctx.Style.Colors[GuiCol_ResizeGrip]);

            float pad = 3.f;
            // [修改] 右下角的调整手柄更改为支持实心绘制的光栅化三角形
            Vec2 p1 = { x + pad, y + triSize - pad };
            Vec2 p2 = { x + triSize - pad, y + triSize - pad };
            Vec2 p3 = { x + triSize - pad, y + pad };

            DrawTriangleFilled(p1, p2, p3, triColor);
        }

        if (!g_Ctx.MouseDown) g_Ctx.IsResizing = false;
        RenderPopups();

        RenderTooltipCommands();

        g_Ctx.MouseClicked = false;
        g_Ctx.RightMouseClicked = false;

        memset(g_Ctx.KeyPressed, 0, sizeof(g_Ctx.KeyPressed));
        CheckAndDrawErrors();
        g_Ctx.InputChars.clear();

        g_Ctx.MouseWheel = 0.f;
    }

    inline bool BeginTabBar(std::string_view name, ShadowTabBarFlags flags = ShadowTabBarFlags_None) {
        std::string_view display; size_t id; ParseLabel(name, display, id);
        g_Ctx.TabBarStack++;
        g_Ctx.CurrentTabBarFlags = flags;
        g_Ctx.CurrentTabBarId = id;

        // [修改] 提前在 BeginTabBar 处理松开鼠标时的拖拽状态清理，防止 TabItem 松开瞬间消失一帧
        if (!g_Ctx.MouseDown && g_Ctx.DraggingTabBarId == id) {
            g_Ctx.DraggingTabId = 0;
            g_Ctx.DraggingTabBarId = 0;
        }

        g_Ctx.TabCursor = g_Ctx.Cursor;
        g_Ctx.TabBarOrigin = g_Ctx.Cursor;

        bool fittingScroll = (flags & ShadowTabBarFlags_FittingPolicyScroll) != 0;
        bool noScrollbar = (flags & ShadowTabBarFlags_NoScrollbar) != 0;

        float prevContentWidth = g_Ctx.TabBarContentWidth;
        float viewWidth = g_Ctx.WindowSize.x - (g_Ctx.Style.WindowPadding.x * 2.f);
        g_Ctx.TabBarViewWidth = viewWidth;
        bool needsScrollbar = fittingScroll && !noScrollbar && (prevContentWidth > viewWidth);
        g_Ctx.TabBarNeedsScrollbar = needsScrollbar;

        {
            auto& order = g_Ctx.TabOrderMap[id];
            auto& layout = g_Ctx.TabBarLayoutX[id];
            layout.clear();
            float accum = 0.f;
            for (size_t oid : order) {
                layout[oid] = accum;
                float w = 0.f;
                auto itw = g_Ctx.TabWidthCache.find(oid);
                if (itw != g_Ctx.TabWidthCache.end()) w = itw->second;
                accum += w + 5.f;
            }
        }

        g_Ctx.TabBarContentWidthAccum = 0.f;

        g_Ctx.TabBarDisplayCache[id].clear();
        g_Ctx.TabAppearedThisFrame[id].clear();

        g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Style.ItemSpacing.y;

        float scrollbarReserve = needsScrollbar ? (g_Ctx.Style.ScrollbarSize + 4.f) : 0.f;
        g_Ctx.Cursor.y += scrollbarReserve;

        DrawRect({ g_Ctx.WindowPos.x, g_Ctx.Cursor.y }, { g_Ctx.WindowSize.x, 2.f }, g_Ctx.Style.Colors[GuiCol_Separator]);
        g_Ctx.Cursor.y += 2.f + g_Ctx.Style.WindowPadding.y;

        g_Ctx.ContentStartY = g_Ctx.Cursor.y;
        g_Ctx.Cursor.y -= g_Ctx.ScrollY;
        g_Ctx.IsScrollApplied = true;

        return true;
    }

    inline void EndTabBar() {
        size_t tabBarId = g_Ctx.CurrentTabBarId;
        bool fittingScroll = (g_Ctx.CurrentTabBarFlags & ShadowTabBarFlags_FittingPolicyScroll) != 0;
        bool reorderable = (g_Ctx.CurrentTabBarFlags & ShadowTabBarFlags_Reorderable) != 0;
        bool noScrollbar = (g_Ctx.CurrentTabBarFlags & ShadowTabBarFlags_NoScrollbar) != 0;

        g_Ctx.TabBarContentWidth = g_Ctx.TabBarContentWidthAccum;

        float viewWidth = g_Ctx.TabBarViewWidth;
        float maxScrollX = std::max(0.f, g_Ctx.TabBarContentWidth - viewWidth);
        float& scrollX = g_Ctx.TabBarScrollX[tabBarId];
        scrollX = std::clamp(scrollX, 0.f, maxScrollX);

        Vec2 tabRowPos = g_Ctx.TabBarOrigin;
        Vec2 tabRowSize = { viewWidth, g_Ctx.ItemHeight };
        bool hoveringTabRow = IsMouseHovering(tabRowPos, tabRowSize);
        bool overListBox = (g_Ctx.HoveredListBoxIdCurrentFrame != 0); // [新增] 判断本帧内是否悬停了 ListBox

        if (reorderable && g_Ctx.DraggingTabId != 0 && g_Ctx.DraggingTabBarId == tabBarId) {
            if (g_Ctx.MouseDown) {
                g_Ctx.DraggingTabCurrentX = g_Ctx.MousePos.x - g_Ctx.DraggingTabGrabOffsetX;

                auto& order = g_Ctx.TabOrderMap[tabBarId];
                auto& layout = g_Ctx.TabBarLayoutX[tabBarId];

                auto itSelf = std::find(order.begin(), order.end(), g_Ctx.DraggingTabId);
                if (itSelf != order.end()) {
                    size_t selfIdx = static_cast<size_t>(itSelf - order.begin());

                    float selfWidth = 0.f;
                    auto itw = g_Ctx.TabWidthCache.find(g_Ctx.DraggingTabId);
                    if (itw != g_Ctx.TabWidthCache.end()) selfWidth = itw->second;

                    float draggedLogicalX = (g_Ctx.DraggingTabCurrentX - g_Ctx.TabBarOrigin.x) + scrollX;
                    float draggedCenterX = draggedLogicalX + selfWidth * 0.5f;

                    if (selfIdx > 0) {
                        size_t leftId = order[selfIdx - 1];
                        auto itL = layout.find(leftId);
                        if (itL != layout.end()) {
                            float leftWidth = 0.f;
                            auto itlw = g_Ctx.TabWidthCache.find(leftId);
                            if (itlw != g_Ctx.TabWidthCache.end()) leftWidth = itlw->second;
                            float leftCenter = itL->second + leftWidth * 0.5f;
                            if (draggedCenterX < leftCenter) {
                                std::swap(order[selfIdx - 1], order[selfIdx]);
                            }
                        }
                    }

                    itSelf = std::find(order.begin(), order.end(), g_Ctx.DraggingTabId);
                    selfIdx = static_cast<size_t>(itSelf - order.begin());
                    if (selfIdx + 1 < order.size()) {
                        size_t rightId = order[selfIdx + 1];
                        auto itR = layout.find(rightId);
                        if (itR != layout.end()) {
                            float rightWidth = 0.f;
                            auto itrw = g_Ctx.TabWidthCache.find(rightId);
                            if (itrw != g_Ctx.TabWidthCache.end()) rightWidth = itrw->second;
                            float rightCenter = itR->second + rightWidth * 0.5f;
                            if (draggedCenterX > rightCenter) {
                                std::swap(order[selfIdx], order[selfIdx + 1]);
                            }
                        }
                    }
                }
            }
            else {
                g_Ctx.DraggingTabId = 0;
                g_Ctx.DraggingTabBarId = 0;
            }
        }

        if (reorderable && g_Ctx.DraggingTabId != 0 && g_Ctx.DraggingTabBarId == tabBarId) {
            for (const auto& tabInfo : g_Ctx.TabBarDisplayCache[tabBarId]) {
                if (tabInfo.id == g_Ctx.DraggingTabId) {
                    Vec2 clipMin = g_Ctx.TabBarOrigin;
                    Vec2 clipMax = { g_Ctx.TabBarOrigin.x + g_Ctx.TabBarViewWidth, g_Ctx.TabBarOrigin.y + tabInfo.size.y };

                    Color bgColor = g_Ctx.Style.Colors[GuiCol_TabActive];
                    Color textColor = g_Ctx.Style.Colors[GuiCol_TextHighlight];

                    if (tabInfo.font) { PushFont(tabInfo.font, tabInfo.fontScale); }

                    PushClipRect(clipMin, { g_Ctx.TabBarOrigin.x + g_Ctx.TabBarViewWidth, clipMax.y });
                    DrawRect(tabInfo.pos, tabInfo.size, bgColor);
                    DrawTextString(tabInfo.display, { tabInfo.pos.x + g_Ctx.Style.TabExtraWidth / 2.f, tabInfo.pos.y + g_Ctx.Style.FramePadding.y }, textColor);
                    PopClipRect();

                    if (tabInfo.font) { PopFont(); }
                    break;
                }
            }
        }

        float scrollBarTop = 0.f, scrollBarBottom = 0.f;
        if (fittingScroll && !noScrollbar && g_Ctx.TabBarNeedsScrollbar) {
            float barHeight = g_Ctx.Style.ScrollbarSize;
            Vec2 trackPos = { tabRowPos.x, tabRowPos.y + g_Ctx.ItemHeight + 2.f };
            Vec2 trackSize = { viewWidth, barHeight };

            DrawRect(trackPos, trackSize, g_Ctx.Style.Colors[GuiCol_FrameBg]);

            float thumbWidth = std::max(20.f, (viewWidth / g_Ctx.TabBarContentWidth) * trackSize.x);
            float thumbX = trackPos.x + (maxScrollX > 0.f ? (scrollX / maxScrollX) * (trackSize.x - thumbWidth) : 0.f);
            Vec2 thumbPos = { thumbX, trackPos.y };
            Vec2 thumbSize = { thumbWidth, barHeight };

            bool hoveringThumb = IsMouseHoveringRaw(thumbPos, thumbSize);
            bool hoveringTrack = IsMouseHoveringRaw(trackPos, trackSize);

            // [修改] 如果处于 ListBox 范围内，阻止标签页横向滚动条响应点击
            if (!overListBox) {
                if (g_Ctx.MouseClicked && hoveringThumb) {
                    g_Ctx.DraggingTabBarScrollId = tabBarId;
                    g_Ctx.TabBarScrollDragOffset = g_Ctx.MousePos.x - thumbPos.x;
                    g_Ctx.MouseClicked = false;
                }
                else if (g_Ctx.MouseClicked && hoveringTrack) {
                    if (g_Ctx.MousePos.x < thumbPos.x) scrollX -= viewWidth * 0.5f;
                    else scrollX += viewWidth * 0.5f;
                    scrollX = std::clamp(scrollX, 0.f, maxScrollX);
                    g_Ctx.MouseClicked = false;
                }
            }

            if (g_Ctx.DraggingTabBarScrollId == tabBarId) {
                if (g_Ctx.MouseDown) {
                    float newThumbX = g_Ctx.MousePos.x - g_Ctx.TabBarScrollDragOffset;
                    float ratio = (newThumbX - trackPos.x) / std::max(1.f, trackSize.x - thumbWidth);
                    scrollX = std::clamp(ratio * maxScrollX, 0.f, maxScrollX);
                }
                else {
                    g_Ctx.DraggingTabBarScrollId = 0;
                }
            }

            Color thumbColor = (g_Ctx.DraggingTabBarScrollId == tabBarId)
                ? g_Ctx.Style.Colors[GuiCol_SliderGrab]
                : (hoveringThumb ? g_Ctx.Style.Colors[GuiCol_FrameBgHovered] : g_Ctx.Style.Colors[GuiCol_Border]);
            DrawRect(thumbPos, thumbSize, thumbColor);

            bool hoveringForWheel = hoveringTabRow || hoveringTrack;
            // [修改] 拦截滚轮
            if (hoveringForWheel && g_Ctx.MouseWheel != 0.f && !overListBox) {
                scrollX -= g_Ctx.MouseWheel * 30.f;
                scrollX = std::clamp(scrollX, 0.f, maxScrollX);
                g_Ctx.MouseWheel = 0.f;
            }

            scrollBarTop = trackPos.y;
            scrollBarBottom = trackPos.y + trackSize.y;
        }
        else if (fittingScroll) {
            // [修改] 拦截滚轮
            if (hoveringTabRow && g_Ctx.MouseWheel != 0.f && !overListBox) {
                scrollX -= g_Ctx.MouseWheel * 30.f;
                scrollX = std::clamp(scrollX, 0.f, maxScrollX);
                g_Ctx.MouseWheel = 0.f;
            }
            g_Ctx.DraggingTabBarScrollId = 0;
        }

        float unionTop = tabRowPos.y;
        float unionBottom = (scrollBarBottom > 0) ? scrollBarBottom : (tabRowPos.y + g_Ctx.ItemHeight);
        Vec2 fullTabBarRectPos = { tabRowPos.x, unionTop };
        Vec2 fullTabBarRectSize = { viewWidth, unionBottom - unionTop };
        g_Ctx.TabBarHoverRectsPending.push_back({ fullTabBarRectPos, fullTabBarRectSize });

        g_Ctx.TabBarStack--;
    }

    inline bool BeginTabItem(std::string_view name) {
        std::string_view display; size_t id; ParseLabel(name, display, id);
        g_Ctx.TabItemStack++;

        size_t tabBarId = g_Ctx.CurrentTabBarId;
        bool reorderable = (g_Ctx.CurrentTabBarFlags & ShadowTabBarFlags_Reorderable) != 0;
        bool fittingScroll = (g_Ctx.CurrentTabBarFlags & ShadowTabBarFlags_FittingPolicyScroll) != 0;

        auto& order = g_Ctx.TabOrderMap[tabBarId];
        if (std::find(order.begin(), order.end(), id) == order.end()) {
            order.push_back(id);
        }
        g_Ctx.TabAppearedThisFrame[tabBarId].push_back(id);

        Vec2 tabSize = MeasureTextSize(display);
        tabSize.x += g_Ctx.Style.TabExtraWidth; tabSize.y = g_Ctx.ItemHeight;

        g_Ctx.TabWidthCache[id] = tabSize.x;

        float offsetX;
        auto& layout = g_Ctx.TabBarLayoutX[tabBarId];
        auto itLayout = layout.find(id);
        if (itLayout != layout.end()) {
            offsetX = itLayout->second;
        }
        else {
            float maxEnd = 0.f;
            for (auto& [oid, x] : layout) {
                float w = 0.f;
                auto itw = g_Ctx.TabWidthCache.find(oid);
                if (itw != g_Ctx.TabWidthCache.end()) w = itw->second;
                maxEnd = std::max(maxEnd, x + w + 5.f);
            }
            offsetX = maxEnd;
            layout[id] = offsetX;
        }

        float scrollX = fittingScroll ? g_Ctx.TabBarScrollX[tabBarId] : 0.f;

        bool isDraggingSelf = reorderable && g_Ctx.DraggingTabId == id && g_Ctx.DraggingTabBarId == tabBarId;

        Vec2 tabPos;
        if (isDraggingSelf) {
            tabPos = { g_Ctx.DraggingTabCurrentX, g_Ctx.TabBarOrigin.y };
        }
        else {
            tabPos = { g_Ctx.TabBarOrigin.x + offsetX - scrollX, g_Ctx.TabBarOrigin.y };
        }

        SDK::UFont* currentFont = g_Ctx.DefaultFont;
        float currentScale = 1.0f;
        if (!g_Ctx.FontStack.empty()) {
            currentFont = g_Ctx.FontStack.back().Font;
            currentScale = g_Ctx.FontStack.back().Scale;
        }

        g_Ctx.TabBarDisplayCache[tabBarId].push_back({ id, tabPos, tabSize, std::string(display), currentFont, currentScale });

        g_Ctx.TabBarContentWidthAccum += tabSize.x + 5.f;

        Vec2 clipMin = g_Ctx.TabBarOrigin;
        Vec2 clipMax = { g_Ctx.TabBarOrigin.x + g_Ctx.TabBarViewWidth, g_Ctx.TabBarOrigin.y + tabSize.y };
        bool tabVisible = !(tabPos.x + tabSize.x < clipMin.x || tabPos.x > clipMax.x);

        bool hovered = tabVisible && IsMouseHovering(tabPos, tabSize) && IsRectVisible(tabPos, tabSize);

        if (hovered && g_Ctx.MouseClicked) {
            if (g_Ctx.ActiveTabId != id) {
                g_Ctx.FocusedSliderId = 0;
                g_Ctx.ActiveInputId = 0;
                g_Ctx.ScrollY = 0.f;
            }
            g_Ctx.ActiveTabId = id;

            if (reorderable) {
                g_Ctx.DraggingTabId = id;
                g_Ctx.DraggingTabBarId = tabBarId;
                g_Ctx.DraggingTabGrabOffsetX = g_Ctx.MousePos.x - tabPos.x;
                g_Ctx.DraggingTabCurrentX = tabPos.x;
                isDraggingSelf = true; // [修改] 手动更新标记，防止当前帧继续触发下方的原生绘制，从而避免重叠闪烁
            }
            g_Ctx.MouseClicked = false;
        }
        if (g_Ctx.ActiveTabId == 0) g_Ctx.ActiveTabId = id;

        bool isActive = (g_Ctx.ActiveTabId == id);
        g_Ctx.InActiveTab = isActive;

        if (!isDraggingSelf) {
            Color bgColor = isActive ? g_Ctx.Style.Colors[GuiCol_TabActive] : (hovered ? g_Ctx.Style.Colors[GuiCol_TabHovered] : g_Ctx.Style.Colors[GuiCol_Tab]);
            Color textColor = isActive ? g_Ctx.Style.Colors[GuiCol_TextHighlight] : g_Ctx.Style.Colors[GuiCol_TextDisabled];

            if (tabVisible) {
                PushClipRect(clipMin, { g_Ctx.TabBarOrigin.x + g_Ctx.TabBarViewWidth, clipMax.y });
                DrawRect(tabPos, tabSize, bgColor);
                DrawTextString(display, { tabPos.x + g_Ctx.Style.TabExtraWidth / 2.f, tabPos.y + g_Ctx.Style.FramePadding.y }, textColor);
                PopClipRect();
            }
        }

        if (isActive) {
            float activeScrollbarWidth = g_Ctx.CurrentScrollbarWidth;

            PushClipRect(
                { g_Ctx.WindowPos.x, g_Ctx.ContentStartY },
                { g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - activeScrollbarWidth, g_Ctx.WindowPos.y + g_Ctx.WindowSize.y - g_Ctx.Style.ResizeGripSize }
            );
        }

        return isActive;
    }

    inline void EndTabItem() {
        if (g_Ctx.InActiveTab) {
            PopClipRect(); // 当前页结束，弹出页面内部专用的剪裁区域
        }
        g_Ctx.TabItemStack--;
        g_Ctx.InActiveTab = true;
    }

    inline bool Combo(std::string_view name, int* current_item, const std::vector<std::string>& items, ShadowComboFlags flags = ShadowComboFlags_None, Vec2 size_arg = { 0.f, 0.f }) {
        if (!g_Ctx.InActiveTab) return false;
        std::string_view display; size_t id; ParseLabel(name, display, id);
        g_Ctx.WidgetCount++;

        float itemHeight = size_arg.y > 0.f ? size_arg.y : g_Ctx.ItemHeight;

        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, itemHeight })) {
            g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
            g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
            return false;
        }

        bool disabled = IsDisabled();
        bool noText = (flags & ShadowComboFlags_NoText) != 0;
        bool noRightAlign = (flags & ShadowComboFlags_NoRightAlign) != 0;
        bool fitText = (flags & ShadowComboFlags_FitText) != 0;

        float textWidth = 0.f;
        Color textColor = disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text];

        if (!noText) {
            DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + g_Ctx.Style.FramePadding.y + (itemHeight - g_Ctx.ItemHeight) * 0.5f }, textColor);
            textWidth = MeasureTextSize(display).x;
        }

        float controlOffsetX = GetControlOffsetX();
        float rightMargin = GetRightMargin();

        std::string currentText = (*current_item >= 0 && *current_item < static_cast<int>(items.size())) ? items[*current_item] : "Unknown";
        float triSize = itemHeight * 0.5f;

        float boxWidth;
        if (size_arg.x > 0.f) {
            boxWidth = size_arg.x;
        }
        else {
            if (fitText) {
                boxWidth = g_Ctx.Style.FramePadding.x + MeasureTextSize(currentText).x + g_Ctx.Style.FramePadding.x + triSize + g_Ctx.Style.FramePadding.x;
            }
            else {
                if (noRightAlign) {
                    float startX = g_Ctx.Cursor.x + (noText ? 0.f : textWidth + 10.f);
                    boxWidth = std::max(50.f, g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - rightMargin - startX);
                }
                else {
                    boxWidth = std::max(100.f, g_Ctx.WindowSize.x - controlOffsetX - rightMargin);
                }
            }
        }

        Vec2 boxPos;
        if (noRightAlign) {
            boxPos = { g_Ctx.Cursor.x + (noText ? 0.f : textWidth + 10.f), g_Ctx.Cursor.y };
        }
        else {
            if (fitText) {
                boxPos = { g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - rightMargin - boxWidth, g_Ctx.Cursor.y };
            }
            else {
                boxPos = { g_Ctx.WindowPos.x + controlOffsetX, g_Ctx.Cursor.y };
            }
        }

        Vec2 boxSize = { boxWidth, itemHeight };
        bool hovered = !disabled && IsMouseHovering(boxPos, boxSize);
        bool toggled = false;

        if (hovered && g_Ctx.MouseClicked) {
            g_Ctx.IsDragging = false;

            if (g_Ctx.ActiveDropdownId == id) g_Ctx.ActiveDropdownId = 0;
            else {
                g_Ctx.ActiveDropdownId = id;
                g_Ctx.DropdownCurrentItem = current_item;
                g_Ctx.DropdownItems = items;
                g_Ctx.DropdownPos = { boxPos.x, boxPos.y + boxSize.y };
                g_Ctx.DropdownSize = { boxWidth, 0.f };
            }
            g_Ctx.MouseClicked = false;
            toggled = true;
        }

        Color bgColor = disabled ? g_Ctx.Style.Colors[GuiCol_ControlDisabled] : (hovered ? g_Ctx.Style.Colors[GuiCol_FrameBgHovered] : g_Ctx.Style.Colors[GuiCol_FrameBg]);
        DrawRectFilled(boxPos, boxSize, bgColor);

        DrawTextString(currentText, { boxPos.x + g_Ctx.Style.FramePadding.x, boxPos.y + g_Ctx.Style.FramePadding.y + (itemHeight - g_Ctx.ItemHeight) * 0.5f }, textColor);

        Vec2 triPos = { boxPos.x + boxSize.x - g_Ctx.Style.FramePadding.x - triSize, boxPos.y + boxSize.y / 2.f - triSize / 2.f };
        if (IsRectVisible(triPos, { triSize, triSize })) {
            Color triCol = disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text];
            Vec2 p1 = { triPos.x, triPos.y + triSize * 0.25f };
            Vec2 p2 = { triPos.x + triSize, triPos.y + triSize * 0.25f };
            Vec2 p3 = { triPos.x + triSize * 0.5f, triPos.y + triSize * 0.75f };
            DrawTriangleFilled(p1, p2, p3, triCol);
        }

        SetLastItemInfo({ std::min(g_Ctx.Cursor.x, boxPos.x), g_Ctx.Cursor.y }, { boxPos.x + boxSize.x, g_Ctx.Cursor.y + itemHeight }, id, disabled);
        g_Ctx.LastItemMaxX = boxPos.x + boxSize.x;
        g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;

        return toggled;
    }

    inline bool Checkbox(std::string_view name, bool* value, Vec2 size_arg = { 0.f, 0.f }) {
        if (!g_Ctx.InActiveTab) return false;
        std::string_view display; size_t id; ParseLabel(name, display, id);
        g_Ctx.WidgetCount++;

        float itemHeight = size_arg.y > 0.f ? size_arg.y : g_Ctx.ItemHeight;
        Vec2 boxSize = { size_arg.x > 0.f ? size_arg.x : itemHeight, itemHeight };

        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, itemHeight })) {
            g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
            g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
            return false;
        }

        float textWidth = MeasureTextSize(display).x;
        Vec2 interactSize = { boxSize.x + 10.f + textWidth, itemHeight };

        bool disabled = IsDisabled();
        bool hovered = !disabled && IsMouseHovering(g_Ctx.Cursor, interactSize);

        if (hovered && g_Ctx.MouseClicked) {
            *value = !(*value);
        }

        Color bgColor = disabled ? g_Ctx.Style.Colors[GuiCol_ControlDisabled] : (hovered ? g_Ctx.Style.Colors[GuiCol_FrameBgHovered] : g_Ctx.Style.Colors[GuiCol_FrameBg]);
        DrawRectFilled(g_Ctx.Cursor, boxSize, bgColor);

        if (*value) {
            float checkPad = boxSize.x * 0.2f;
            Color checkCol = g_Ctx.Style.Colors[GuiCol_CheckMark];
            if (disabled) checkCol.a *= 0.5f;
            DrawRectFilled({ g_Ctx.Cursor.x + checkPad, g_Ctx.Cursor.y + checkPad }, { boxSize.x - checkPad * 2.f, boxSize.y - checkPad * 2.f }, checkCol);
        }

        Color textColor = disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text];
        DrawTextString(display, { g_Ctx.Cursor.x + boxSize.x + 10.f, g_Ctx.Cursor.y + g_Ctx.Style.FramePadding.y + (itemHeight - g_Ctx.ItemHeight) * 0.5f }, textColor);

        SetLastItemInfo(g_Ctx.Cursor, { g_Ctx.Cursor.x + interactSize.x, g_Ctx.Cursor.y + itemHeight }, id, disabled);
        g_Ctx.LastItemMaxX = g_Ctx.Cursor.x + interactSize.x;
        g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;

        return *value;
    }

    inline bool Switch(std::string_view name, bool* value, Vec2 size_arg = { 0.f, 0.f }) {
        if (!g_Ctx.InActiveTab) return false;
        std::string_view display; size_t id; ParseLabel(name, display, id);
        g_Ctx.WidgetCount++;

        float itemHeight = size_arg.y > 0.f ? size_arg.y : g_Ctx.ItemHeight;
        float padding = 2.f;
        float knobSize = itemHeight - padding * 2.f;
        float width = size_arg.x > 0.f ? size_arg.x : (knobSize * 2.f + padding * 2.f);
        Vec2 boxSize = { width, itemHeight };

        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, itemHeight })) {
            g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
            g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
            return false;
        }

        float textWidth = MeasureTextSize(display).x;
        Vec2 interactSize = { textWidth + 10.f + boxSize.x, itemHeight };

        bool disabled = IsDisabled();
        bool hovered = !disabled && IsMouseHovering(g_Ctx.Cursor, interactSize);
        if (hovered && g_Ctx.MouseClicked) { *value = !(*value); }

        Color textColor = disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text];
        DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + g_Ctx.Style.FramePadding.y + (itemHeight - g_Ctx.ItemHeight) * 0.5f }, textColor);

        Vec2 boxPos = { g_Ctx.Cursor.x + textWidth + 10.f, g_Ctx.Cursor.y };

        Color bgColor;
        if (disabled) {
            bgColor = g_Ctx.Style.Colors[GuiCol_ControlDisabled];
        }
        else {
            if (*value) {
                bgColor = hovered ? g_Ctx.Style.Colors[GuiCol_SwitchBgActiveHovered] : g_Ctx.Style.Colors[GuiCol_SwitchBgActive];
            }
            else {
                bgColor = hovered ? g_Ctx.Style.Colors[GuiCol_SwitchBgHovered] : g_Ctx.Style.Colors[GuiCol_SwitchBg];
            }
        }

        DrawRectFilled(boxPos, boxSize, bgColor);

        Color knobColor = g_Ctx.Style.Colors[GuiCol_SwitchKnob];
        if (disabled) knobColor.a *= 0.5f;

        float knobX = *value ? (boxPos.x + width - padding - knobSize) : (boxPos.x + padding);
        DrawRectFilled({ knobX, boxPos.y + padding }, { knobSize, knobSize }, knobColor);

        SetLastItemInfo(g_Ctx.Cursor, { g_Ctx.Cursor.x + interactSize.x, g_Ctx.Cursor.y + itemHeight }, id, disabled);
        g_Ctx.LastItemMaxX = g_Ctx.Cursor.x + interactSize.x;
        g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;

        return *value;
    }

    inline bool Selectable(std::string_view name, bool* p_selected = nullptr, Vec2 size_arg = { 0.f, 0.f }) {
        if (!g_Ctx.InActiveTab) return false;
        std::string_view display; size_t id; ParseLabel(name, display, id);
        g_Ctx.WidgetCount++;

        float rightMargin = GetRightMargin();
        float width = size_arg.x > 0.f ? size_arg.x : std::max(10.f, g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - rightMargin - g_Ctx.Cursor.x);
        float itemHeight = size_arg.y > 0.f ? size_arg.y : g_Ctx.ItemHeight;
        Vec2 size = { width, itemHeight };

        if (!IsRectVisible(g_Ctx.Cursor, size)) {
            g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
            g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
            return false;
        }

        bool disabled = IsDisabled();
        bool hovered = !disabled && IsMouseHovering(g_Ctx.Cursor, size);
        bool clicked = hovered && g_Ctx.MouseClicked;

        if (clicked && p_selected) {
            *p_selected = !*p_selected;
        }

        bool selected = p_selected ? *p_selected : false;

        Color bgColor = { 0, 0, 0, 0 };
        if (disabled) {
            if (selected) bgColor = g_Ctx.Style.Colors[GuiCol_ControlDisabled];
        }
        else {
            if (hovered && selected) bgColor = g_Ctx.Style.Colors[GuiCol_TabHovered];
            else if (hovered) bgColor = g_Ctx.Style.Colors[GuiCol_FrameBgHovered];
            else if (selected) bgColor = g_Ctx.Style.Colors[GuiCol_TabActive];
        }

        if (bgColor.a > 0.0f) {
            DrawRectFilled(g_Ctx.Cursor, size, bgColor);
        }

        Color textColor = disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text];
        DrawTextString(display, { g_Ctx.Cursor.x + g_Ctx.Style.FramePadding.x, g_Ctx.Cursor.y + g_Ctx.Style.FramePadding.y + (itemHeight - g_Ctx.ItemHeight) * 0.5f }, textColor);

        SetLastItemInfo(g_Ctx.Cursor, { g_Ctx.Cursor.x + size.x, g_Ctx.Cursor.y + size.y }, id, disabled);
        g_Ctx.LastItemMaxX = g_Ctx.Cursor.x + size.x;
        g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;

        return clicked;
    }

    inline bool Selectable(std::string_view name, bool selected, Vec2 size_arg = { 0.f, 0.f }) {
        bool temp = selected;
        return Selectable(name, &temp, size_arg);
    }

    inline bool InputTextWithHint(std::string_view name, std::string_view hint, std::string& text, ShadowInputTextFlags flags = 0, Vec2 size_arg = { 0.f, 0.f }) {
        if (!g_Ctx.InActiveTab) return false;
        std::string_view display; size_t id; ParseLabel(name, display, id);
        g_Ctx.WidgetCount++;

        float itemHeight = size_arg.y > 0.f ? size_arg.y : g_Ctx.ItemHeight;

        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, itemHeight })) {
            g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
            g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
            return false;
        }

        bool disabled = IsDisabled();
        bool noName = (flags & ShadowInputTextFlags_NoName) != 0;

        if (!noName) {
            Color textColor = disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text];
            DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + g_Ctx.Style.FramePadding.y + (itemHeight - g_Ctx.ItemHeight) * 0.5f }, textColor);
        }

        float controlOffsetX = GetControlOffsetX();
        float rightMargin = GetRightMargin();

        Vec2 boxPos;
        float boxWidth;

        if (noName) {
            boxPos = { g_Ctx.Cursor.x, g_Ctx.Cursor.y };
            boxWidth = std::max(50.f, g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - rightMargin - boxPos.x);
        }
        else {
            boxPos = { g_Ctx.WindowPos.x + controlOffsetX, g_Ctx.Cursor.y };
            boxWidth = std::max(50.f, g_Ctx.WindowSize.x - controlOffsetX - rightMargin);
        }

        if (size_arg.x > 0.f) boxWidth = size_arg.x;
        Vec2 boxSize = { boxWidth, itemHeight };

        bool changed = InputTextEx(id, boxPos, boxSize, text, flags, false, hint);

        SetLastItemInfo({ std::min(g_Ctx.Cursor.x, boxPos.x), g_Ctx.Cursor.y }, { boxPos.x + boxSize.x, g_Ctx.Cursor.y + itemHeight }, id, disabled);
        g_Ctx.LastItemMaxX = boxPos.x + boxSize.x;
        g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;

        return changed;
    }

    inline bool InputText(std::string_view name, std::string& text, ShadowInputTextFlags flags = 0, Vec2 size_arg = { 0.f, 0.f }) {
        return InputTextWithHint(name, "", text, flags, size_arg);
    }

    inline bool InputFloat(std::string_view name, float* v, float step = 0.0f, float step_fast = 0.0f, std::string_view format = "{:.3f}", ShadowInputTextFlags flags = 0, Vec2 size_arg = { 0.f, 0.f }) {
        if (!g_Ctx.InActiveTab) return false;
        std::string_view display; size_t id; ParseLabel(name, display, id);
        g_Ctx.WidgetCount++;

        float itemHeight = size_arg.y > 0.f ? size_arg.y : g_Ctx.ItemHeight;

        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, itemHeight })) {
            g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
            g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
            return false;
        }

        bool disabled = IsDisabled();
        bool noName = (flags & ShadowInputTextFlags_NoName) != 0;

        if (!noName) {
            Color textColor = disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text];
            DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + g_Ctx.Style.FramePadding.y + (itemHeight - g_Ctx.ItemHeight) * 0.5f }, textColor);
        }

        size_t inputId = id ^ HashString("_inputFloat");

        if (g_Ctx.ActiveInputId != inputId) {
            if ((flags & ShadowInputTextFlags_DisplayEmptyRefVal) && std::abs(*v) < 0.000001f) {
                g_Ctx.InputBuffers[inputId] = "";
            }
            else {
                g_Ctx.InputBuffers[inputId] = std::vformat(format, std::make_format_args(*v));
            }
        }

        float controlOffsetX = GetControlOffsetX();
        float rightMargin = GetRightMargin();

        Vec2 boxPos;
        float boxWidth;

        if (noName) {
            boxPos = { g_Ctx.Cursor.x, g_Ctx.Cursor.y };
            boxWidth = std::max(50.f, g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - rightMargin - boxPos.x);
        }
        else {
            boxPos = { g_Ctx.WindowPos.x + controlOffsetX, g_Ctx.Cursor.y };
            boxWidth = std::max(50.f, g_Ctx.WindowSize.x - controlOffsetX - rightMargin);
        }

        if (size_arg.x > 0.f) boxWidth = size_arg.x;
        Vec2 boxSize = { boxWidth, itemHeight };

        ShadowInputTextFlags effectiveFlags = flags | ShadowInputTextFlags_CharsScientific;

        bool changed = InputTextEx(inputId, boxPos, boxSize, g_Ctx.InputBuffers[inputId], effectiveFlags);
        bool valueChanged = false;

        if (changed && !disabled) {
            std::string& str = g_Ctx.InputBuffers[inputId];
            if (str.empty() && (flags & ShadowInputTextFlags_ParseEmptyRefVal)) {
                *v = 0.0f;
                valueChanged = true;
            }
            else if (!str.empty()) {
                float parsed;
                auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), parsed);
                if (ec == std::errc()) {
                    *v = parsed;
                    valueChanged = true;
                }
            }
        }

        SetLastItemInfo({ std::min(g_Ctx.Cursor.x, boxPos.x), g_Ctx.Cursor.y }, { boxPos.x + boxSize.x, g_Ctx.Cursor.y + itemHeight }, id, disabled);
        g_Ctx.LastItemMaxX = boxPos.x + boxSize.x;
        g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;

        return valueChanged;
    }

    inline bool Button(std::string_view name, Vec2 size_arg = { 0.f, 0.f }) {
        if (!g_Ctx.InActiveTab) return false;
        std::string_view display; size_t id; ParseLabel(name, display, id);
        g_Ctx.WidgetCount++;

        Vec2 size = MeasureTextSize(display);
        size.x += g_Ctx.Style.FramePadding.x * 2.f;
        size.y = g_Ctx.ItemHeight;

        if (size_arg.x > 0.f) size.x = size_arg.x;
        if (size_arg.y > 0.f) size.y = size_arg.y;

        if (!IsRectVisible(g_Ctx.Cursor, size)) {
            g_Ctx.Cursor.y += size.y + g_Ctx.Style.ItemSpacing.y;
            g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
            return false;
        }

        bool disabled = IsDisabled();
        bool hovered = !disabled && IsMouseHovering(g_Ctx.Cursor, size);
        bool clicked = hovered && g_Ctx.MouseClicked;

        Color bgColor = disabled ? g_Ctx.Style.Colors[GuiCol_ControlDisabled] : (hovered ? g_Ctx.Style.Colors[GuiCol_ButtonHovered] : g_Ctx.Style.Colors[GuiCol_Button]);
        Color textColor = disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text];

        DrawRectFilled(g_Ctx.Cursor, size, bgColor);
        DrawTextString(display, { g_Ctx.Cursor.x + g_Ctx.Style.FramePadding.x, g_Ctx.Cursor.y + g_Ctx.Style.FramePadding.y + (size.y - g_Ctx.ItemHeight) * 0.5f }, textColor);

        SetLastItemInfo(g_Ctx.Cursor, { g_Ctx.Cursor.x + size.x, g_Ctx.Cursor.y + size.y }, id, disabled);
        g_Ctx.LastItemMaxX = g_Ctx.Cursor.x + size.x;
        g_Ctx.Cursor.y += size.y + g_Ctx.Style.ItemSpacing.y;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
        return clicked;
    }

    inline void Slider(std::string_view name, float* value, float min_val, float max_val, float step = 0.f, ShadowSliderFlags flags = ShadowSliderFlags_None, Vec2 size_arg = { 0.f, 0.f }) {
        if (!g_Ctx.InActiveTab) return;
        std::string_view display; size_t id; ParseLabel(name, display, id);
        g_Ctx.WidgetCount++;

        float itemHeight = size_arg.y > 0.f ? size_arg.y : g_Ctx.ItemHeight;

        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, itemHeight })) {
            g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
            g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
            return;
        }

        bool disabled = IsDisabled();
        bool noText = (flags & ShadowSliderFlags_NoText) != 0;
        bool noRightAlign = (flags & ShadowSliderFlags_NoRightAlign) != 0;

        float textWidth = 0.f;
        Color textColor = disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text];
        if (!noText) {
            DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + g_Ctx.Style.FramePadding.y + (itemHeight - g_Ctx.ItemHeight) * 0.5f }, textColor);
            textWidth = MeasureTextSize(display).x;
        }

        size_t sliderInputId = id ^ HashString("_sliderInput");

        if (g_Ctx.ActiveInputId != sliderInputId) {
            int prec = 3;
            if (step > 0.f) {
                prec = 0;
                float temp = step;
                while (temp < 0.999f && prec < 5) {
                    temp *= 10.0f;
                    prec++;
                }
            }
            g_Ctx.InputBuffers[sliderInputId] = std::format("{:.{}f}", *value, prec);
        }

        float controlOffsetX = GetControlOffsetX();
        float rightMargin = GetRightMargin();

        std::string& inputStr = g_Ctx.InputBuffers[sliderInputId];
        size_t currentLen = inputStr.length();

        auto it_width = g_Ctx.SliderInputWidthCache.find(sliderInputId);
        auto it_len = g_Ctx.SliderInputLengthCache.find(sliderInputId);

        if (it_len == g_Ctx.SliderInputLengthCache.end() || it_len->second != currentLen || it_width == g_Ctx.SliderInputWidthCache.end()) {
            g_Ctx.SliderInputWidthCache[sliderInputId] = MeasureTextSize(inputStr).x + g_Ctx.Style.FramePadding.x * 2.f;
            g_Ctx.SliderInputLengthCache[sliderInputId] = currentLen;
        }
        float valBoxWidth = g_Ctx.SliderInputWidthCache[sliderInputId];

        Vec2 sliderPos;
        float sliderWidth;

        if (noRightAlign) {
            sliderPos = { g_Ctx.Cursor.x + (noText ? 0.f : textWidth + 10.f), g_Ctx.Cursor.y };
            sliderWidth = std::max(50.f, g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - rightMargin - sliderPos.x - valBoxWidth - g_Ctx.Style.ItemSpacing.x);
        }
        else {
            sliderPos = { g_Ctx.WindowPos.x + controlOffsetX, g_Ctx.Cursor.y };
            sliderWidth = std::max(50.f, g_Ctx.WindowSize.x - controlOffsetX - valBoxWidth - g_Ctx.Style.ItemSpacing.x - rightMargin);
        }

        if (size_arg.x > 0.f) sliderWidth = size_arg.x;

        Vec2 size = { sliderWidth, itemHeight };
        Vec2 valBoxPos = { sliderPos.x + sliderWidth + g_Ctx.Style.ItemSpacing.x, sliderPos.y };
        Vec2 valBoxSize = { valBoxWidth, itemHeight };

        bool hovered = !disabled && IsMouseHovering(sliderPos, size);

        if (!disabled && g_Ctx.MouseClicked) {
            if (hovered) { g_Ctx.DraggingSliderId = id; g_Ctx.FocusedSliderId = id; }
            else if (g_Ctx.FocusedSliderId == id) g_Ctx.FocusedSliderId = 0;
        }

        if (!g_Ctx.MouseDown && g_Ctx.DraggingSliderId == id) g_Ctx.DraggingSliderId = 0;

        if (!disabled && g_Ctx.DraggingSliderId == id) {
            float ratio = std::clamp((g_Ctx.MousePos.x - sliderPos.x) / sliderWidth, 0.f, 1.f);
            float newVal = min_val + ratio * (max_val - min_val);
            if (step > 0.f) newVal = std::round(newVal / step) * step;
            *value = std::clamp(newVal, min_val, max_val);
        }

        if (!disabled && g_Ctx.FocusedSliderId == id) {
            if (g_Ctx.KeyPressed[VK_LEFT]) {
                float modifyVal = *value - (step > 0.f ? step : (max_val - min_val) * 0.01f);
                if (step > 0.f) modifyVal = std::round(modifyVal / step) * step;
                *value = std::clamp(modifyVal, min_val, max_val);
            }
            if (g_Ctx.KeyPressed[VK_RIGHT]) {
                float modifyVal = *value + (step > 0.f ? step : (max_val - min_val) * 0.01f);
                if (step > 0.f) modifyVal = std::round(modifyVal / step) * step;
                *value = std::clamp(modifyVal, min_val, max_val);
            }
        }

        Color bgColor = disabled ? g_Ctx.Style.Colors[GuiCol_ControlDisabled] : (hovered ? g_Ctx.Style.Colors[GuiCol_FrameBgHovered] : g_Ctx.Style.Colors[GuiCol_FrameBg]);
        DrawRectFilled(sliderPos, size, bgColor);

        float fillWidth = std::clamp((*value - min_val) / (max_val - min_val), 0.f, 1.f) * sliderWidth;
        Color grabCol = g_Ctx.Style.Colors[GuiCol_SliderGrab];
        if (disabled) grabCol.a *= 0.5f;
        DrawRectFilled(sliderPos, { fillWidth, size.y }, grabCol);

        float knobWidth = 4.0f;
        float knobX = sliderPos.x + fillWidth - knobWidth * 0.5f;
        knobX = std::clamp(knobX, sliderPos.x, sliderPos.x + sliderWidth - knobWidth);

        Color knobCol = g_Ctx.Style.Colors[GuiCol_SliderKnob];
        if (disabled) knobCol.a *= 0.5f;
        DrawRectFilled({ knobX, sliderPos.y }, { knobWidth, size.y }, knobCol);

        if (g_Ctx.FocusedSliderId == id) {
            Color border = g_Ctx.Style.Colors[GuiCol_Border];
            DrawRectFilled({ sliderPos.x - 1.f, sliderPos.y - 1.f }, { size.x + 2.f, 1.f }, border);
            DrawRectFilled({ sliderPos.x - 1.f, sliderPos.y + size.y }, { size.x + 2.f, 1.f }, border);
            DrawRectFilled({ sliderPos.x - 1.f, sliderPos.y }, { 1.f, size.y }, border);
            DrawRectFilled({ sliderPos.x + size.x, sliderPos.y }, { 1.f, size.y }, border);
        }

        bool changed = InputTextEx(sliderInputId, valBoxPos, valBoxSize, g_Ctx.InputBuffers[sliderInputId], ShadowInputTextFlags_CharsDecimal);

        if (changed && !disabled) {
            const std::string& str = g_Ctx.InputBuffers[sliderInputId];
            float newVal;
            auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), newVal);

            if (ec == std::errc() && ptr > str.data()) {
                if (step > 0.f) newVal = std::round(newVal / step) * step;
                *value = std::clamp(newVal, min_val, max_val);
            }
        }

        SetLastItemInfo({ std::min(g_Ctx.Cursor.x, sliderPos.x), g_Ctx.Cursor.y }, { valBoxPos.x + valBoxSize.x, g_Ctx.Cursor.y + itemHeight }, id, disabled);
        g_Ctx.LastItemMaxX = valBoxPos.x + valBoxSize.x;
        g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
    }

    inline void ColorPicker(std::string_view name, float* r, float* g, float* b, float* a, ShadowColorPickerFlags flags = ShadowColorPickerFlags_None, Vec2 size_arg = { 0.f, 0.f }) {
        if (!g_Ctx.InActiveTab) return;
        std::string_view display; size_t id; ParseLabel(name, display, id);
        g_Ctx.WidgetCount++;

        float itemHeight = size_arg.y > 0.f ? size_arg.y : g_Ctx.ItemHeight;

        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, itemHeight })) {
            g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
            g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
            return;
        }

        bool disabled = IsDisabled();
        bool noText = (flags & ShadowColorPickerFlags_NoText) != 0;
        bool noRightAlign = (flags & ShadowColorPickerFlags_NoRightAlign) != 0;

        float textWidth = 0.f;
        if (!noText) {
            Color textColor = disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text];
            DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + g_Ctx.Style.FramePadding.y + (itemHeight - g_Ctx.ItemHeight) * 0.5f }, textColor);
            textWidth = MeasureTextSize(display).x;
        }

        Vec2 boxSize = { size_arg.x > 0.f ? size_arg.x : itemHeight, itemHeight };
        Vec2 boxPos;

        if (noRightAlign) {
            boxPos = { g_Ctx.Cursor.x + (noText ? 0.f : textWidth + 10.f), g_Ctx.Cursor.y };
        }
        else {
            float rightMargin = GetRightMargin();
            boxPos = { g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - rightMargin - boxSize.x, g_Ctx.Cursor.y };
        }

        bool hovered = !disabled && IsMouseHovering(boxPos, boxSize);

        if (hovered && g_Ctx.MouseClicked) {
            g_Ctx.IsDragging = false;

            if (g_Ctx.ActiveColorPickerId == id) g_Ctx.ActiveColorPickerId = 0;
            else {
                g_Ctx.ActiveColorPickerId = id;
                g_Ctx.ColorPickerR = r; g_Ctx.ColorPickerG = g; g_Ctx.ColorPickerB = b; g_Ctx.ColorPickerA = a;

                float popupWidth = g_Ctx.Style.CPPadding * 2.f + g_Ctx.Style.CPSVSize + g_Ctx.Style.CPSpacing * 2.f + g_Ctx.Style.CPHueWidth + g_Ctx.Style.CPAlphaWidth;
                g_Ctx.ColorPickerPos = { boxPos.x + boxSize.x + g_Ctx.Style.ItemSpacing.x, boxPos.y + itemHeight + 4.f };
                RGBtoHSV(*r, *g, *b, g_Ctx.ColorPickerH, g_Ctx.ColorPickerS, g_Ctx.ColorPickerV);
            }
            g_Ctx.MouseClicked = false;
        }

        float globalAlpha = g_Ctx.Style.Colors[GuiCol_ColorPickerLight].a;
        if (disabled) globalAlpha *= 0.5f;

        Color cbLight = g_Ctx.Style.Colors[GuiCol_CheckerboardLight];
        Color cbDark = g_Ctx.Style.Colors[GuiCol_CheckerboardDark];

        float currentA = a ? *a : 1.0f;
        int checkerSize = 5;

        for (int y = 0; y < boxSize.y; y += checkerSize) {
            for (int x = 0; x < boxSize.x; x += checkerSize) {
                bool isWhite = ((x / checkerSize) + (y / checkerSize)) % 2 == 0;
                Color bgCol = isWhite ? cbLight : cbDark;

                float finalR = *r * currentA + bgCol.r * (1.f - currentA);
                float finalG = *g * currentA + bgCol.g * (1.f - currentA);
                float finalB = *b * currentA + bgCol.b * (1.f - currentA);

                float drawX = boxPos.x + x;
                float drawY = boxPos.y + y;
                float w = std::min((float)checkerSize, boxSize.x - x);
                float h = std::min((float)checkerSize, boxSize.y - y);

                DrawRectFilled({ drawX, drawY }, { w, h }, { finalR, finalG, finalB, globalAlpha });
            }
        }

        if (hovered || g_Ctx.ActiveColorPickerId == id) {
            Color border = g_Ctx.Style.Colors[GuiCol_Border];
            DrawLine({ boxPos.x, boxPos.y }, { boxPos.x + boxSize.x, boxPos.y }, border);
            DrawLine({ boxPos.x + boxSize.x, boxPos.y }, { boxPos.x + boxSize.x, boxPos.y + boxSize.y }, border);
            DrawLine({ boxPos.x + boxSize.x, boxPos.y + boxSize.y }, { boxPos.x, boxPos.y + boxSize.y }, border);
            DrawLine({ boxPos.x, boxPos.y + boxSize.y }, { boxPos.x, boxPos.y }, border);
        }

        SetLastItemInfo({ std::min(g_Ctx.Cursor.x, boxPos.x), g_Ctx.Cursor.y }, { boxPos.x + boxSize.x, g_Ctx.Cursor.y + itemHeight }, id, disabled);
        g_Ctx.LastItemMaxX = boxPos.x + boxSize.x;
        g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
    }

    inline bool HotKey(std::string_view name, int* hotkey, ShadowHotkeyFlags flags = ShadowHotkeyFlags_None, Vec2 size_arg = { 0.f, 0.f }) {
        RegisterHotkey(hotkey, nullptr, nullptr);

        if (!g_Ctx.InActiveTab) return false;
        std::string_view display; size_t id; ParseLabel(name, display, id);
        g_Ctx.WidgetCount++;

        float itemHeight = size_arg.y > 0.f ? size_arg.y : g_Ctx.ItemHeight;

        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, itemHeight })) {
            g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
            g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
            return false;
        }

        bool disabled = IsDisabled();
        bool noText = (flags & ShadowHotkeyFlags_NoText) != 0;
        bool noRightAlign = (flags & ShadowHotkeyFlags_NoRightAlign) != 0;

        float textWidth = 0.f;
        Color textColor = disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text];

        if (!noText) {
            DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + g_Ctx.Style.FramePadding.y + (itemHeight - g_Ctx.ItemHeight) * 0.5f }, textColor);
            textWidth = MeasureTextSize(display).x;
        }

        bool isAssigning = (g_Ctx.AssigningHotkey == hotkey);
        std::string keyName = isAssigning ? "[Press Key]" : std::format("[{}]", GetKeyName(*hotkey));

        Vec2 btnSize = { MeasureTextSize(keyName).x + g_Ctx.Style.FramePadding.x * 2.f, itemHeight };
        if (size_arg.x > 0.f) btnSize.x = size_arg.x;
        Vec2 btnPos;

        if (noRightAlign) {
            btnPos = { g_Ctx.Cursor.x + (noText ? 0.f : textWidth + 10.f), g_Ctx.Cursor.y };
        }
        else {
            float rightMargin = GetRightMargin();
            btnPos = { g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - rightMargin - btnSize.x, g_Ctx.Cursor.y };
        }

        bool btnHovered = !disabled && IsMouseHovering(btnPos, btnSize);
        if (btnHovered && g_Ctx.MouseClicked) {
            g_Ctx.IsDragging = false;
            g_Ctx.AssigningHotkey = hotkey;
            g_Ctx.MouseClicked = false;
        }

        Color bgColor = disabled ? g_Ctx.Style.Colors[GuiCol_ControlDisabled] : (btnHovered ? g_Ctx.Style.Colors[GuiCol_ButtonHovered] : g_Ctx.Style.Colors[GuiCol_Button]);
        DrawRectFilled(btnPos, btnSize, bgColor);
        DrawTextString(keyName, { btnPos.x + g_Ctx.Style.FramePadding.x, btnPos.y + g_Ctx.Style.FramePadding.y + (itemHeight - g_Ctx.ItemHeight) * 0.5f }, textColor);

        SetLastItemInfo({ std::min(g_Ctx.Cursor.x, btnPos.x), g_Ctx.Cursor.y }, { btnPos.x + btnSize.x, g_Ctx.Cursor.y + itemHeight }, id, disabled);
        g_Ctx.LastItemMaxX = btnPos.x + btnSize.x;
        g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
        return *hotkey != 0 && g_Ctx.KeyStates[*hotkey];
    }

    inline void HotKey(std::string_view name, int* hotkey, bool* is_active, HotkeyMode* hotkey_mode, ShadowHotkeyFlags flags = ShadowHotkeyFlags_None, Vec2 size_arg = { 0.f, 0.f }) {
        RegisterHotkey(hotkey, hotkey_mode, is_active);

        if (!g_Ctx.InActiveTab) return;
        std::string_view display; size_t id; ParseLabel(name, display, id);
        g_Ctx.WidgetCount++;

        float itemHeight = size_arg.y > 0.f ? size_arg.y : g_Ctx.ItemHeight;

        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, itemHeight })) {
            g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
            g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
            return;
        }

        bool disabled = IsDisabled();
        bool noText = (flags & ShadowHotkeyFlags_NoText) != 0;
        bool noRightAlign = (flags & ShadowHotkeyFlags_NoRightAlign) != 0;
        bool noStateDisplay = (flags & ShadowHotkeyFlags_NoStateDisplay) != 0;

        float textWidth = 0.f;
        Color textColor = disabled ? g_Ctx.Style.Colors[GuiCol_TextDisabled] : g_Ctx.Style.Colors[GuiCol_Text];

        if (!noText) {
            DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + g_Ctx.Style.FramePadding.y + (itemHeight - g_Ctx.ItemHeight) * 0.5f }, textColor);
            textWidth = MeasureTextSize(display).x;
        }

        bool isAssigning = (g_Ctx.AssigningHotkey == hotkey);
        std::vector<std::string> modeStrs = { "None", "Hold On", "Toggle On", "Hold Off", "Always On" };

        std::string keyName = isAssigning ? "[Press Key]" : std::format("[{}]", GetKeyName(*hotkey));
        Vec2 btnSize = { MeasureTextSize(keyName).x + g_Ctx.Style.FramePadding.x * 2.f, itemHeight };
        if (size_arg.x > 0.f) btnSize.x = size_arg.x;

        float dotSize = std::max(6.f, itemHeight * 0.4f);
        float dotOffset = (itemHeight - dotSize) / 2.f;

        Vec2 btnPos;
        if (noRightAlign) {
            btnPos = { g_Ctx.Cursor.x + (noText ? 0.f : textWidth + 10.f), g_Ctx.Cursor.y };
        }
        else {
            float rightMargin = GetRightMargin();
            if (noStateDisplay) {
                btnPos = { g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - rightMargin - btnSize.x, g_Ctx.Cursor.y };
            }
            else {
                btnPos = { g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - rightMargin - dotSize - g_Ctx.Style.ItemSpacing.x - btnSize.x, g_Ctx.Cursor.y };
            }
        }

        bool btnHovered = !disabled && IsMouseHovering(btnPos, btnSize);

        if (btnHovered) {
            if (g_Ctx.MouseClicked) {
                g_Ctx.IsDragging = false;
                g_Ctx.AssigningHotkey = hotkey;
                g_Ctx.MouseClicked = false;
            }
            else if (g_Ctx.RightMouseClicked) {
                g_Ctx.IsDragging = false;

                if (g_Ctx.ActiveDropdownId == id) g_Ctx.ActiveDropdownId = 0;
                else {
                    g_Ctx.ActiveDropdownId = id;
                    g_Ctx.DropdownCurrentItem = reinterpret_cast<int*>(hotkey_mode);
                    g_Ctx.DropdownItems = modeStrs;
                    g_Ctx.DropdownPos = { btnPos.x, btnPos.y + btnSize.y };
                    g_Ctx.DropdownSize = { 100.f, 0.f };
                }
                g_Ctx.RightMouseClicked = false;
            }
        }

        Color bgColor = disabled ? g_Ctx.Style.Colors[GuiCol_ControlDisabled] : (btnHovered ? g_Ctx.Style.Colors[GuiCol_ButtonHovered] : g_Ctx.Style.Colors[GuiCol_Button]);
        DrawRectFilled(btnPos, btnSize, bgColor);
        DrawTextString(keyName, { btnPos.x + g_Ctx.Style.FramePadding.x, btnPos.y + g_Ctx.Style.FramePadding.y + (itemHeight - g_Ctx.ItemHeight) * 0.5f }, textColor);

        switch (*hotkey_mode) {
        case HotkeyMode::None:      *is_active = false; break;
        case HotkeyMode::HoldOn:    *is_active = g_Ctx.KeyStates[*hotkey]; break;
        case HotkeyMode::HoldOff:   *is_active = !g_Ctx.KeyStates[*hotkey]; break;
        case HotkeyMode::ToggleOn:  *is_active = g_Ctx.HotkeyToggles[*hotkey]; break;
        case HotkeyMode::AlwaysOn:  *is_active = true; break;
        }

        if (!noStateDisplay) {
            Color indicatorColor = *is_active ? g_Ctx.Style.Colors[GuiCol_ActiveIndicator] : g_Ctx.Style.Colors[GuiCol_InactiveIndicator];
            if (disabled) indicatorColor.a *= 0.5f;
            DrawRectFilled({ btnPos.x + btnSize.x + g_Ctx.Style.ItemSpacing.x, g_Ctx.Cursor.y + dotOffset }, { dotSize, dotSize }, indicatorColor);
        }

        float maxX = noStateDisplay ? (btnPos.x + btnSize.x) : (btnPos.x + btnSize.x + g_Ctx.Style.ItemSpacing.x + dotSize);
        SetLastItemInfo({ std::min(g_Ctx.Cursor.x, btnPos.x), g_Ctx.Cursor.y }, { maxX, g_Ctx.Cursor.y + itemHeight }, id, disabled);

        g_Ctx.LastItemMaxX = maxX;
        g_Ctx.Cursor.y += itemHeight + g_Ctx.Style.ItemSpacing.y;
        g_Ctx.Cursor.x = g_Ctx.WindowPos.x + g_Ctx.Style.WindowPadding.x + g_Ctx.IndentX;
    }
}