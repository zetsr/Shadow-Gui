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

    // --- 新增：ImGui 风格的颜色枚举与样式系统 ---
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
        GuiCol_CheckMark,
        GuiCol_Separator,
        GuiCol_PopupBg,
        GuiCol_PopupBorder,
        GuiCol_ResizeGrip,
        GuiCol_ResizeGripHovered,
        GuiCol_Tab,
        GuiCol_TabHovered,
        GuiCol_TabActive,
        GuiCol_ActiveIndicator,
        GuiCol_InactiveIndicator,
        GuiCol_Border,
        GuiCol_COUNT
    };

    struct GuiStyle {
        Color Colors[GuiCol_COUNT];
    };

    // 提取 ColorPicker 的配置以保持统一
    namespace CPConfig {
        inline const float Padding = 8.f;
        inline const float SVSize = 200.f;
        inline const float HueWidth = 20.f;
        inline const float AlphaWidth = 20.f;
        inline const float Spacing = 8.f;
    }

    struct GuiContext {
        SDK::UCanvas* Canvas = nullptr;
        SDK::UFont* DefaultFont = nullptr;

        // 样式状态
        GuiStyle Style;
        bool StyleInitialized = false;

        // 鼠标状态
        Vec2 MousePos = { 0.f, 0.f };
        bool MouseDown = false;
        bool MouseClicked = false;
        bool RightMouseDown = false;
        bool RightMouseClicked = false;

        // 键盘与热键状态机
        bool KeyStates[256] = { false };
        bool HotkeyToggles[256] = { false };
        bool KeyPressed[256] = { false }; // 单帧触发状态
        int* AssigningHotkey = nullptr;

        // ComboBox / Dropdown 状态
        size_t ActiveDropdownId = 0;
        int* DropdownCurrentItem = nullptr;
        std::vector<std::string> DropdownItems;
        Vec2 DropdownPos = { 0.f, 0.f };
        Vec2 DropdownSize = { 0.f, 0.f };

        // 窗口状态
        Vec2 WindowPos = { 100.f, 100.f };
        Vec2 WindowSize = { 500.f, 400.f };
        bool IsDragging = false;
        Vec2 DragOffset = { 0.f, 0.f };

        // 栈平衡检查
        int BeginStack = 0;
        int TabBarStack = 0;
        int TabItemStack = 0;
        std::string LastErrorMsg;

        // 窗口调整大小
        bool IsResizing = false;
        Vec2 ResizeStartPos = { 0.f, 0.f };
        Vec2 ResizeStartSize = { 0.f, 0.f };
        float ResizeTriangleSize = 16.f;
        bool IsHoveringResize = false;

        // 剪裁区域
        bool ClippingEnabled = false;
        Vec2 ClipMin = { 0.f, 0.f };
        Vec2 ClipMax = { 0.f, 0.f };

        // 游标与布局
        Vec2 Cursor = { 0.f, 0.f };
        float Padding = 8.f;
        float ItemHeight = 20.f;

        // Tab状态
        size_t ActiveTabId = 0;
        size_t CurrentTabId = 0;
        bool InActiveTab = true;
        Vec2 TabCursor = { 0.f, 0.f };

        // ColorPicker 弹出层状态
        size_t ActiveColorPickerId = 0;
        float* ColorPickerR = nullptr;
        float* ColorPickerG = nullptr;
        float* ColorPickerB = nullptr;
        float* ColorPickerA = nullptr;
        Vec2 ColorPickerPos = { 0.f, 0.f };
        float ColorPickerH = 0.f;
        float ColorPickerS = 0.f;
        float ColorPickerV = 0.f;

        // ColorPicker 拖拽与输入状态
        bool IsDraggingSV = false;
        bool IsDraggingHue = false;
        bool IsDraggingAlpha = false;
        bool IsDraggingColorPicker = false;
        Vec2 ColorPickerDragOffset = { 0.f, 0.f };
        bool IsTypingHex = false;
        std::string HexInputBuffer;

        // Slider 状态
        size_t DraggingSliderId = 0;
        size_t FocusedSliderId = 0;
    };

    inline GuiContext g_Ctx;
    inline SDK::UFont*& DefaultFont = g_Ctx.DefaultFont;

    // 获取样式实例，供外部动态修改
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

        colors[GuiCol_CheckMark] = { 0.520f, 0.220f, 0.220f, 1.000f };
        colors[GuiCol_ActiveIndicator] = { 0.520f, 0.220f, 0.220f, 1.000f };
        colors[GuiCol_InactiveIndicator] = { 0.035f, 0.045f, 0.060f, 1.000f };

        colors[GuiCol_Border] = { 0.030f, 0.040f, 0.055f, 0.600f };
        colors[GuiCol_PopupBorder] = { 0.035f, 0.045f, 0.060f, 0.900f };
        colors[GuiCol_Separator] = { 0.025f, 0.032f, 0.045f, 1.000f };

        colors[GuiCol_ResizeGrip] = { 0.025f, 0.032f, 0.045f, 1.000f };
        colors[GuiCol_ResizeGripHovered] = { 0.520f, 0.220f, 0.220f, 1.000f };
    }

    // 允许放行透传给游戏的按键列表
    inline std::vector<int> AllowedKeys;
    inline void SetAllowedKeys(const std::vector<int>& keys) {
        AllowedKeys = keys;
    }
    inline bool IsKeyAllowed(int key) {
        return std::find(AllowedKeys.begin(), AllowedKeys.end(), key) != AllowedKeys.end();
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
            float popupWidth = CPConfig::Padding + CPConfig::SVSize + CPConfig::Spacing + CPConfig::HueWidth + CPConfig::Spacing + CPConfig::AlphaWidth + CPConfig::Padding;
            float popupHeight = CPConfig::Padding + CPConfig::SVSize + CPConfig::Spacing + hexBoxHeight + CPConfig::Padding;

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

    inline bool HandleKeyDown(int vk) {
        if (TryAssignKey(vk)) return true;
        if (!g_Ctx.KeyStates[vk]) {
            g_Ctx.HotkeyToggles[vk] = !g_Ctx.HotkeyToggles[vk];
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
        if (g_Ctx.HexInputBuffer.size() < 6) return;

        std::string_view hex = g_Ctx.HexInputBuffer;
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

    inline LRESULT Input(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_MOUSEMOVE:
            g_Ctx.MousePos.x = static_cast<float>(LOWORD(lParam));
            g_Ctx.MousePos.y = static_cast<float>(HIWORD(lParam));
            break;
        case WM_LBUTTONDOWN:
            if (HandleKeyDown(VK_LBUTTON)) return 0;
            g_Ctx.MouseDown = true;
            g_Ctx.MouseClicked = true;
            break;
        case WM_LBUTTONUP:
            HandleKeyUp(VK_LBUTTON);
            g_Ctx.MouseDown = false;
            g_Ctx.IsDragging = false;
            g_Ctx.IsResizing = false;
            break;
        case WM_RBUTTONDOWN:
            if (HandleKeyDown(VK_RBUTTON)) return 0;
            g_Ctx.RightMouseDown = true;
            g_Ctx.RightMouseClicked = true;
            break;
        case WM_RBUTTONUP:
            HandleKeyUp(VK_RBUTTON);
            g_Ctx.RightMouseDown = false;
            break;
        case WM_MBUTTONDOWN: if (HandleKeyDown(VK_MBUTTON)) return 0; break;
        case WM_MBUTTONUP:   HandleKeyUp(VK_MBUTTON); break;
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
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (wParam < 256) {
                if (g_Ctx.IsTypingHex) {
                    int vk = static_cast<int>(wParam);
                    if (vk == VK_BACK && !g_Ctx.HexInputBuffer.empty()) {
                        g_Ctx.HexInputBuffer.pop_back();
                    }
                    else if (vk == VK_RETURN || vk == VK_ESCAPE) {
                        g_Ctx.IsTypingHex = false;
                        if (vk == VK_RETURN) ApplyHexInput();
                    }
                    else if (g_Ctx.HexInputBuffer.size() < 8) {
                        char c = 0;
                        if (vk >= '0' && vk <= '9') c = vk;
                        else if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9) c = vk - VK_NUMPAD0 + '0';
                        else if (vk >= 'A' && vk <= 'F') c = vk;
                        else if (vk >= 'a' && vk <= 'f') c = vk - 'a' + 'A';
                        if (c != 0) g_Ctx.HexInputBuffer += c;
                    }
                    return 0;
                }
                if (HandleKeyDown(static_cast<int>(wParam))) return 0;
            }
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            if (wParam < 256) HandleKeyUp(static_cast<int>(wParam));
            break;
        }
        return 0;
    }

    inline void PushClipRect(Vec2 min, Vec2 max) {
        g_Ctx.ClippingEnabled = true;
        g_Ctx.ClipMin = min;
        g_Ctx.ClipMax = max;
    }

    inline void PopClipRect() {
        g_Ctx.ClippingEnabled = false;
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

    inline void DrawRect(Vec2 pos, Vec2 size, Color color) {
        if (!g_Ctx.Canvas) return;
        if (!IsRectVisible(pos, size)) return;
        ClipRect(pos, size);
        if (size.x <= 0.f || size.y <= 0.f) return;

        SDK::FLinearColor ueColor{ color.r, color.g, color.b, color.a };
        SDK::FVector2D uePos{ static_cast<double>(pos.x), static_cast<double>(pos.y) };
        SDK::FVector2D ueSize{ static_cast<double>(size.x), static_cast<double>(size.y) };

        if (g_Ctx.Canvas->DefaultTexture) {
            g_Ctx.Canvas->K2_DrawTexture(
                g_Ctx.Canvas->DefaultTexture,
                uePos,
                ueSize,
                SDK::FVector2D{ 0.0, 0.0 },
                SDK::FVector2D{ 1.0, 1.0 },
                ueColor,
                SDK::EBlendMode::BLEND_Translucent,
                0.0f,
                SDK::FVector2D{ 0.0, 0.0 }
            );
        }

        g_Ctx.Canvas->K2_DrawBox(uePos, ueSize, 1.0f, ueColor);
    }

    inline Vec2 MeasureTextSize(std::string_view text) {
        if (!g_Ctx.Canvas || !g_Ctx.DefaultFont) return { 0.f, 0.f };
        SDK::FVector2D scale{ 1.0, 1.0 };
        std::wstring wstr = ToWString(text);
        SDK::FVector2D size = g_Ctx.Canvas->K2_TextSize(g_Ctx.DefaultFont, SDK::FString(wstr.c_str()), scale);
        return { static_cast<float>(size.X), static_cast<float>(size.Y) };
    }

    inline float MeasureCharWidth(wchar_t ch) {
        if (!g_Ctx.Canvas || !g_Ctx.DefaultFont) return 0.f;
        std::wstring single(1, ch);
        SDK::FVector2D scale{ 1.0, 1.0 };
        SDK::FVector2D size = g_Ctx.Canvas->K2_TextSize(g_Ctx.DefaultFont, SDK::FString(single.c_str()), scale);
        return static_cast<float>(size.X);
    }

    inline float MeasureTextHeight(std::string_view text) {
        if (!g_Ctx.Canvas || !g_Ctx.DefaultFont || text.empty()) return 0.f;
        Vec2 size = MeasureTextSize(text);
        return size.y;
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
        if (!g_Ctx.Canvas || !g_Ctx.DefaultFont) return;
        Vec2 clippedPos = pos;
        bool shouldDraw = true;
        std::string clippedText = ClipTextString(text, pos, clippedPos, shouldDraw);
        if (!shouldDraw || clippedText.empty()) return;

        SDK::FLinearColor ueColor{ color.r, color.g, color.b, color.a };
        SDK::FVector2D uePos{ static_cast<double>(clippedPos.x), static_cast<double>(clippedPos.y) };
        SDK::FVector2D scale{ 1.0, 1.0 };
        SDK::FLinearColor shadow{ 0.f, 0.f, 0.f, 1.f };
        SDK::FVector2D shadowOff{ 1.0, 1.0 };
        SDK::FLinearColor outline{ 0.f, 0.f, 0.f, 0.f };
        std::wstring wstr = ToWString(clippedText);
        g_Ctx.Canvas->K2_DrawText(g_Ctx.DefaultFont, SDK::FString(wstr.c_str()), uePos, scale, ueColor, 0.f, shadow, shadowOff, false, false, false, outline);
    }

    inline float GetControlOffsetX() {
        return std::max(120.f, g_Ctx.WindowSize.x * 0.4f);
    }

    inline void CheckAndDrawErrors() {
        std::string errorMsg;
        if (g_Ctx.BeginStack > 0) errorMsg = std::format("ERROR: Begin() called {} time(s) without matching End()!", g_Ctx.BeginStack);
        else if (g_Ctx.BeginStack < 0) errorMsg = std::format("ERROR: End() called {} time(s) without matching Begin()!", -g_Ctx.BeginStack);
        else if (g_Ctx.TabBarStack > 0) errorMsg = std::format("ERROR: BeginTabBar() called {} time(s) without matching EndTabBar()!", g_Ctx.TabBarStack);
        else if (g_Ctx.TabBarStack < 0) errorMsg = std::format("ERROR: EndTabBar() called {} time(s) without matching BeginTabBar()!", -g_Ctx.TabBarStack);
        else if (g_Ctx.TabItemStack > 0) errorMsg = std::format("ERROR: BeginTabItem() called {} time(s) without matching EndTabItem()!", g_Ctx.TabItemStack);
        else if (g_Ctx.TabItemStack < 0) errorMsg = std::format("ERROR: EndTabItem() called {} time(s) without matching BeginTabItem()!", -g_Ctx.TabItemStack);

        if (!errorMsg.empty()) {
            bool oldClipping = g_Ctx.ClippingEnabled;
            g_Ctx.ClippingEnabled = false;
            DrawTextString(errorMsg, { 5.f, 5.f }, { 1.f, 0.196f, 0.196f, 1.f });
            g_Ctx.ClippingEnabled = oldClipping;
        }
    }

    inline void NewFrame(SDK::UCanvas* Canvas) {
        g_Ctx.Canvas = Canvas;
        if (!g_Ctx.DefaultFont) {
            if (SDK::UEngine::GetEngine()) { g_Ctx.DefaultFont = SDK::UEngine::GetEngine()->LargeFont; }
        }

        // --- 初始化主题 ---
        if (!g_Ctx.StyleInitialized) {
            StyleColorsOcean();
            g_Ctx.StyleInitialized = true;
        }

        Vec2 charSize = MeasureTextSize("A");
        g_Ctx.ItemHeight = charSize.y > 0.f ? charSize.y + 4.f : 20.f;
        g_Ctx.Padding = 8.f;
        g_Ctx.InActiveTab = true;
        g_Ctx.BeginStack = 0;
        g_Ctx.TabBarStack = 0;
        g_Ctx.TabItemStack = 0;
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

                if (itemHovered) {
                    DrawRect(itemPos, { dropWidth, g_Ctx.ItemHeight }, g_Ctx.Style.Colors[GuiCol_FrameBgHovered]);
                    if (g_Ctx.MouseClicked) {
                        *g_Ctx.DropdownCurrentItem = static_cast<int>(i);
                        g_Ctx.ActiveDropdownId = 0;
                        g_Ctx.MouseClicked = false;
                    }
                }

                Color textColor = (*g_Ctx.DropdownCurrentItem == static_cast<int>(i)) ? g_Ctx.Style.Colors[GuiCol_TextHighlight] : g_Ctx.Style.Colors[GuiCol_Text];
                DrawTextString(g_Ctx.DropdownItems[i], { itemPos.x + 8.f, itemPos.y + 2.f }, textColor);
            }

            if (g_Ctx.MouseClicked && !hoveringDropdown) { g_Ctx.ActiveDropdownId = 0; }
            if (hoveringDropdown) { g_Ctx.MouseClicked = false; }
        }

        if (g_Ctx.ActiveColorPickerId != 0 && g_Ctx.ColorPickerR) {
            Vec2 popupPos = g_Ctx.ColorPickerPos;
            float padding = CPConfig::Padding;
            float svSize = CPConfig::SVSize;
            float hueWidth = CPConfig::HueWidth;
            float alphaWidth = CPConfig::AlphaWidth;
            float spacing = CPConfig::Spacing;

            float hexBoxHeight = std::max(24.f, g_Ctx.ItemHeight + 4.f);
            float popupWidth = padding + svSize + spacing + hueWidth + spacing + alphaWidth + padding;
            float popupHeight = padding + svSize + spacing + hexBoxHeight + padding;

            Vec2 svPos = { popupPos.x + padding, popupPos.y + padding };
            Vec2 huePos = { svPos.x + svSize + spacing, svPos.y };
            Vec2 alphaPos = { huePos.x + hueWidth + spacing, svPos.y };
            Vec2 hexPos = { svPos.x, svPos.y + svSize + spacing };
            Vec2 hexSize = { popupWidth - padding * 2.f, hexBoxHeight };

            bool popupHovered = IsMouseHoveringRaw({ popupPos.x - 2.f, popupPos.y - 2.f }, { popupWidth + 4.f, popupHeight + 4.f });
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
                    *g_Ctx.ColorPickerA = 1.0f - std::clamp((g_Ctx.MousePos.y - alphaPos.y) / svSize, 0.f, 1.f);
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

            if (g_Ctx.MouseClicked && hexHovered) {
                g_Ctx.IsTypingHex = true;
                g_Ctx.MouseClicked = false;
                uint8_t r8 = static_cast<uint8_t>(*g_Ctx.ColorPickerR * 255.f);
                uint8_t g8 = static_cast<uint8_t>(*g_Ctx.ColorPickerG * 255.f);
                uint8_t b8 = static_cast<uint8_t>(*g_Ctx.ColorPickerB * 255.f);
                uint8_t a8 = static_cast<uint8_t>(*g_Ctx.ColorPickerA * 255.f);
                g_Ctx.HexInputBuffer = std::format("{:02X}{:02X}{:02X}{:02X}", r8, g8, b8, a8);
            }
            else if (g_Ctx.MouseDown && !hexHovered && g_Ctx.IsTypingHex) {
                g_Ctx.IsTypingHex = false;
                ApplyHexInput();
            }

            int svSteps = 40;
            float svStepSize = svSize / svSteps;
            for (int i = 0; i < svSteps; ++i) {
                for (int j = 0; j < svSteps; ++j) {
                    float s = (float)i / (svSteps - 1);
                    float v = 1.0f - (float)j / (svSteps - 1);
                    float r, g, b;
                    HSVtoRGB(g_Ctx.ColorPickerH, s, v, r, g, b);
                    DrawRect({ svPos.x + i * svStepSize, svPos.y + j * svStepSize }, { svStepSize + 0.5f, svStepSize + 0.5f }, { r, g, b, 1.f });
                }
            }

            int hueSteps = 40;
            float hueStepSize = svSize / hueSteps;
            for (int i = 0; i < hueSteps; ++i) {
                float h = (float)i / (hueSteps - 1);
                float r, g, b;
                HSVtoRGB(h, 1.f, 1.f, r, g, b);
                DrawRect({ huePos.x, huePos.y + i * hueStepSize }, { hueWidth, hueStepSize + 0.5f }, { r, g, b, 1.f });
            }

            int alphaSteps = 40;
            float alphaStepSize = svSize / alphaSteps;
            for (int i = 0; i < alphaSteps; ++i) {
                float a = 1.0f - (float)i / (alphaSteps - 1);
                DrawRect({ alphaPos.x, alphaPos.y + i * alphaStepSize }, { alphaWidth, alphaStepSize + 0.5f }, { *g_Ctx.ColorPickerR, *g_Ctx.ColorPickerG, *g_Ctx.ColorPickerB, a });
            }

            DrawRect(hexPos, hexSize, g_Ctx.Style.Colors[GuiCol_FrameBg]);

            std::string displayHex;
            if (g_Ctx.IsTypingHex) displayHex = "#" + g_Ctx.HexInputBuffer + "_";
            else {
                uint8_t r8 = static_cast<uint8_t>(*g_Ctx.ColorPickerR * 255.f);
                uint8_t g8 = static_cast<uint8_t>(*g_Ctx.ColorPickerG * 255.f);
                uint8_t b8 = static_cast<uint8_t>(*g_Ctx.ColorPickerB * 255.f);
                uint8_t a8 = static_cast<uint8_t>(*g_Ctx.ColorPickerA * 255.f);
                displayHex = std::format("#{:02X}{:02X}{:02X}{:02X}", r8, g8, b8, a8);
            }

            Vec2 textSize = MeasureTextSize(displayHex);
            float textY = hexPos.y + (hexBoxHeight - textSize.y) / 2.f;
            DrawTextString(displayHex, { hexPos.x + 8.f, textY }, g_Ctx.Style.Colors[GuiCol_Text]);

            Vec2 cursorSV = { svPos.x + g_Ctx.ColorPickerS * svSize, svPos.y + (1.f - g_Ctx.ColorPickerV) * svSize };
            DrawRect({ cursorSV.x - 4.f, cursorSV.y - 4.f }, { 8.f, 8.f }, { 0.f, 0.f, 0.f, 1.f });
            DrawRect({ cursorSV.x - 3.f, cursorSV.y - 3.f }, { 6.f, 6.f }, { 1.f, 1.f, 1.f, 1.f });
            DrawRect({ cursorSV.x - 2.f, cursorSV.y - 2.f }, { 4.f, 4.f }, { *g_Ctx.ColorPickerR, *g_Ctx.ColorPickerG, *g_Ctx.ColorPickerB, 1.f });

            Vec2 cursorHue = { huePos.x - 2.f, huePos.y + g_Ctx.ColorPickerH * svSize - 2.f };
            DrawRect(cursorHue, { hueWidth + 4.f, 4.f }, { 0.f, 0.f, 0.f, 1.f });
            DrawRect({ cursorHue.x + 1.f, cursorHue.y + 1.f }, { hueWidth + 2.f, 2.f }, { 1.f, 1.f, 1.f, 1.f });

            Vec2 cursorAlpha = { alphaPos.x - 2.f, alphaPos.y + (1.f - *g_Ctx.ColorPickerA) * svSize - 2.f };
            DrawRect(cursorAlpha, { alphaWidth + 4.f, 4.f }, { 0.f, 0.f, 0.f, 1.f });
            DrawRect({ cursorAlpha.x + 1.f, cursorAlpha.y + 1.f }, { alphaWidth + 2.f, 2.f }, { 1.f, 1.f, 1.f, 1.f });

            if (g_Ctx.MouseClicked && !popupHovered) {
                g_Ctx.ActiveColorPickerId = 0;
                g_Ctx.IsTypingHex = false;
            }
            if (popupHovered) g_Ctx.MouseClicked = false;
        }
    }

    inline bool Begin(std::string_view name) {
        std::string_view display; size_t id;
        ParseLabel(name, display, id);
        g_Ctx.BeginStack++;
        float titleBarHeight = std::max(30.f, g_Ctx.ItemHeight + 10.f);
        Vec2 wholeWindowSize = g_Ctx.WindowSize;
        bool hoveringWholeWindow = IsMouseHovering(g_Ctx.WindowPos, wholeWindowSize);

        float triSize = g_Ctx.ResizeTriangleSize;
        Vec2 triPos = { g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - triSize, g_Ctx.WindowPos.y + g_Ctx.WindowSize.y - triSize };
        g_Ctx.IsHoveringResize = IsMouseHoveringRaw(triPos, { triSize, triSize });

        bool isOtherDragging = (g_Ctx.DraggingSliderId != 0) || g_Ctx.IsDraggingSV || g_Ctx.IsDraggingHue || g_Ctx.IsDraggingAlpha || g_Ctx.IsDraggingColorPicker;

        if (!g_Ctx.IsDragging && hoveringWholeWindow && g_Ctx.MouseClicked && !g_Ctx.IsHoveringResize && !isOtherDragging) {
            g_Ctx.IsDragging = true;
            g_Ctx.DragOffset.x = g_Ctx.MousePos.x - g_Ctx.WindowPos.x;
            g_Ctx.DragOffset.y = g_Ctx.MousePos.y - g_Ctx.WindowPos.y;
        }
        if (g_Ctx.IsDragging && isOtherDragging) g_Ctx.IsDragging = false;

        if (!g_Ctx.IsResizing && g_Ctx.IsHoveringResize && g_Ctx.MouseClicked && !isOtherDragging) {
            g_Ctx.IsResizing = true;
            g_Ctx.ResizeStartPos = g_Ctx.MousePos;
            g_Ctx.ResizeStartSize = g_Ctx.WindowSize;
        }
        if (g_Ctx.IsResizing && isOtherDragging) g_Ctx.IsResizing = false;

        if (g_Ctx.IsDragging) {
            g_Ctx.WindowPos.x = g_Ctx.MousePos.x - g_Ctx.DragOffset.x;
            g_Ctx.WindowPos.y = g_Ctx.MousePos.y - g_Ctx.DragOffset.y;
        }
        if (g_Ctx.IsResizing) {
            Vec2 delta = { g_Ctx.MousePos.x - g_Ctx.ResizeStartPos.x, g_Ctx.MousePos.y - g_Ctx.ResizeStartPos.y };
            g_Ctx.WindowSize.x = std::max(200.f, g_Ctx.ResizeStartSize.x + delta.x);
            g_Ctx.WindowSize.y = std::max(150.f, g_Ctx.ResizeStartSize.y + delta.y);
            if (!g_Ctx.MouseDown) g_Ctx.IsResizing = false;
        }

        DrawRect(g_Ctx.WindowPos, g_Ctx.WindowSize, g_Ctx.Style.Colors[GuiCol_WindowBg]);
        DrawRect(g_Ctx.WindowPos, { g_Ctx.WindowSize.x, titleBarHeight }, g_Ctx.Style.Colors[GuiCol_TitleBarBg]);
        DrawTextString(display, { g_Ctx.WindowPos.x + 10.f, g_Ctx.WindowPos.y + 7.f }, g_Ctx.Style.Colors[GuiCol_Text]);

        {
            float x = g_Ctx.WindowPos.x + g_Ctx.WindowSize.x - triSize;
            float y = g_Ctx.WindowPos.y + g_Ctx.WindowSize.y - triSize;
            Color triColor = (g_Ctx.IsHoveringResize || g_Ctx.IsResizing) ? g_Ctx.Style.Colors[GuiCol_ResizeGripHovered] : g_Ctx.Style.Colors[GuiCol_ResizeGrip];

            float pad = 3.f;
            SDK::FLinearColor ueColor{ triColor.r, triColor.g, triColor.b, triColor.a };
            SDK::FVector2D p1{ static_cast<double>(x + pad), static_cast<double>(y + triSize - pad) };
            SDK::FVector2D p2{ static_cast<double>(x + triSize - pad), static_cast<double>(y + triSize - pad) };
            SDK::FVector2D p3{ static_cast<double>(x + triSize - pad), static_cast<double>(y + pad) };

            if (g_Ctx.Canvas) {
                g_Ctx.Canvas->K2_DrawLine(p1, p2, 1.0f, ueColor);
                g_Ctx.Canvas->K2_DrawLine(p2, p3, 1.0f, ueColor);
                g_Ctx.Canvas->K2_DrawLine(p3, p1, 1.0f, ueColor);
            }
        }

        g_Ctx.Cursor = { g_Ctx.WindowPos.x + g_Ctx.Padding, g_Ctx.WindowPos.y + titleBarHeight + g_Ctx.Padding };
        PushClipRect(
            { g_Ctx.WindowPos.x, g_Ctx.WindowPos.y + titleBarHeight },
            { g_Ctx.WindowPos.x + g_Ctx.WindowSize.x, g_Ctx.WindowPos.y + g_Ctx.WindowSize.y }
        );
        return true;
    }

    inline void End() {
        g_Ctx.BeginStack--;
        PopClipRect();
        if (!g_Ctx.MouseDown) g_Ctx.IsResizing = false;
        RenderPopups();
        g_Ctx.MouseClicked = false;
        g_Ctx.RightMouseClicked = false;
        memset(g_Ctx.KeyPressed, 0, sizeof(g_Ctx.KeyPressed));
        CheckAndDrawErrors();
    }

    inline bool BeginTabBar(std::string_view name) {
        std::string_view display; size_t id; ParseLabel(name, display, id);
        g_Ctx.TabBarStack++;
        g_Ctx.TabCursor = g_Ctx.Cursor;
        g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;

        DrawRect({ g_Ctx.WindowPos.x, g_Ctx.Cursor.y }, { g_Ctx.WindowSize.x, 2.f }, g_Ctx.Style.Colors[GuiCol_Separator]);
        g_Ctx.Cursor.y += 2.f + g_Ctx.Padding;
        return true;
    }

    inline void EndTabBar() {
        g_Ctx.TabBarStack--;
    }

    inline bool BeginTabItem(std::string_view name) {
        std::string_view display; size_t id; ParseLabel(name, display, id);
        g_Ctx.TabItemStack++;

        Vec2 tabSize = MeasureTextSize(display);
        tabSize.x += 20.f; tabSize.y = g_Ctx.ItemHeight;

        bool hovered = IsMouseHovering(g_Ctx.TabCursor, tabSize);
        if (hovered && g_Ctx.MouseClicked) {
            if (g_Ctx.ActiveTabId != id) g_Ctx.FocusedSliderId = 0;
            g_Ctx.ActiveTabId = id;
        }
        if (g_Ctx.ActiveTabId == 0) g_Ctx.ActiveTabId = id;

        bool isActive = (g_Ctx.ActiveTabId == id);
        g_Ctx.InActiveTab = isActive;

        Color bgColor = isActive ? g_Ctx.Style.Colors[GuiCol_TabActive] : (hovered ? g_Ctx.Style.Colors[GuiCol_TabHovered] : g_Ctx.Style.Colors[GuiCol_Tab]);
        Color textColor = isActive ? g_Ctx.Style.Colors[GuiCol_TextHighlight] : g_Ctx.Style.Colors[GuiCol_TextDisabled];

        DrawRect(g_Ctx.TabCursor, tabSize, bgColor);
        DrawTextString(display, { g_Ctx.TabCursor.x + 10.f, g_Ctx.TabCursor.y + 2.f }, textColor);

        g_Ctx.TabCursor.x += tabSize.x + 5.f;
        return isActive;
    }

    inline void EndTabItem() {
        g_Ctx.TabItemStack--;
        g_Ctx.InActiveTab = true;
    }

    inline bool ComboBox(std::string_view name, int* current_item, const std::vector<std::string>& items) {
        if (!g_Ctx.InActiveTab) return false;
        std::string_view display; size_t id; ParseLabel(name, display, id);

        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, g_Ctx.ItemHeight })) {
            g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
            return false;
        }

        DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + 2.f }, g_Ctx.Style.Colors[GuiCol_Text]);

        float textWidth = MeasureTextSize(display).x;
        float controlOffsetX = textWidth + g_Ctx.Padding;

        std::string currentText = (*current_item >= 0 && *current_item < static_cast<int>(items.size())) ? items[*current_item] : "Unknown";
        float currentTextWidth = MeasureTextSize(currentText).x;
        float boxWidth = currentTextWidth + 30.f;

        Vec2 boxPos = { g_Ctx.Cursor.x + controlOffsetX, g_Ctx.Cursor.y };
        Vec2 boxSize = { boxWidth, g_Ctx.ItemHeight };

        bool hovered = IsMouseHovering(boxPos, boxSize);
        if (hovered && g_Ctx.MouseClicked) {
            if (g_Ctx.ActiveDropdownId == id) g_Ctx.ActiveDropdownId = 0;
            else {
                g_Ctx.ActiveDropdownId = id;
                g_Ctx.DropdownCurrentItem = current_item;
                g_Ctx.DropdownItems = items;
                g_Ctx.DropdownPos = { boxPos.x, boxPos.y + boxSize.y };
                g_Ctx.DropdownSize = { boxWidth, 0.f };
            }
            g_Ctx.MouseClicked = false;
        }

        DrawRect(boxPos, boxSize, hovered ? g_Ctx.Style.Colors[GuiCol_FrameBgHovered] : g_Ctx.Style.Colors[GuiCol_FrameBg]);

        DrawTextString(currentText, { boxPos.x + 8.f, boxPos.y + 2.f }, g_Ctx.Style.Colors[GuiCol_Text]);

        float triSize = 6.f;
        Vec2 triPos = { boxPos.x + boxSize.x - 12.f, boxPos.y + boxSize.y / 2.f - triSize / 2.f };
        if (g_Ctx.Canvas && IsRectFullyVisible(triPos, { triSize, triSize })) {
            Color dtCol = g_Ctx.Style.Colors[GuiCol_TextDisabled];
            SDK::FLinearColor ueColor{ dtCol.r, dtCol.g, dtCol.b, dtCol.a };
            SDK::FVector2D p1{ static_cast<double>(triPos.x), static_cast<double>(triPos.y) };
            SDK::FVector2D p2{ static_cast<double>(triPos.x + triSize), static_cast<double>(triPos.y) };
            SDK::FVector2D p3{ static_cast<double>(triPos.x + triSize / 2.f), static_cast<double>(triPos.y + triSize) };
            g_Ctx.Canvas->K2_DrawLine(p1, p2, 1.0f, ueColor);
            g_Ctx.Canvas->K2_DrawLine(p2, p3, 1.0f, ueColor);
            g_Ctx.Canvas->K2_DrawLine(p3, p1, 1.0f, ueColor);
        }

        g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
        return hovered && g_Ctx.MouseClicked;
    }

    inline void CheckBox(std::string_view name, bool* value) {
        if (!g_Ctx.InActiveTab) return;
        std::string_view display; size_t id; ParseLabel(name, display, id);

        Vec2 boxSize = { g_Ctx.ItemHeight, g_Ctx.ItemHeight };

        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, g_Ctx.ItemHeight })) {
            g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
            return;
        }

        float textWidth = MeasureTextSize(display).x;
        Vec2 interactSize = { boxSize.x + 10.f + textWidth, g_Ctx.ItemHeight };

        bool hovered = IsMouseHovering(g_Ctx.Cursor, interactSize);
        if (hovered && g_Ctx.MouseClicked) { *value = !(*value); }

        DrawRect(g_Ctx.Cursor, boxSize, hovered ? g_Ctx.Style.Colors[GuiCol_FrameBgHovered] : g_Ctx.Style.Colors[GuiCol_FrameBg]);
        if (*value) {
            float checkPad = std::max(4.f, boxSize.x * 0.2f);
            DrawRect({ g_Ctx.Cursor.x + checkPad, g_Ctx.Cursor.y + checkPad }, { boxSize.x - checkPad * 2.f, boxSize.y - checkPad * 2.f }, g_Ctx.Style.Colors[GuiCol_CheckMark]);
        }

        DrawTextString(display, { g_Ctx.Cursor.x + boxSize.x + 10.f, g_Ctx.Cursor.y + 2.f }, g_Ctx.Style.Colors[GuiCol_Text]);
        g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
    }

    inline bool Button(std::string_view name) {
        if (!g_Ctx.InActiveTab) return false;
        std::string_view display; size_t id; ParseLabel(name, display, id);

        Vec2 size = MeasureTextSize(display);
        size.x += 20.f; size.y = g_Ctx.ItemHeight;

        if (!IsRectVisible(g_Ctx.Cursor, size)) {
            g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
            return false;
        }

        bool hovered = IsMouseHovering(g_Ctx.Cursor, size);
        bool clicked = hovered && g_Ctx.MouseClicked;

        DrawRect(g_Ctx.Cursor, size, hovered ? g_Ctx.Style.Colors[GuiCol_ButtonHovered] : g_Ctx.Style.Colors[GuiCol_Button]);
        DrawTextString(display, { g_Ctx.Cursor.x + 10.f, g_Ctx.Cursor.y + 2.f }, g_Ctx.Style.Colors[GuiCol_Text]);

        g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
        return clicked;
    }

    inline void Slider(std::string_view name, float* value, float min_val, float max_val, float step = 0.f) {
        if (!g_Ctx.InActiveTab) return;
        std::string_view display; size_t id; ParseLabel(name, display, id);

        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, g_Ctx.ItemHeight })) {
            g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
            return;
        }

        // 修改：计算向左对齐控件文本的偏移量
        float textWidth = MeasureTextSize(display).x;
        float controlOffsetX = textWidth + g_Ctx.Padding;

        float sliderWidth = std::max(50.f, g_Ctx.WindowSize.x - controlOffsetX - g_Ctx.Padding * 8.f);
        Vec2 sliderPos = { g_Ctx.Cursor.x + controlOffsetX, g_Ctx.Cursor.y };
        Vec2 size = { sliderWidth, g_Ctx.ItemHeight };

        bool hovered = IsMouseHovering(sliderPos, size);

        if (g_Ctx.MouseClicked) {
            if (hovered) { g_Ctx.DraggingSliderId = id; g_Ctx.FocusedSliderId = id; }
            else if (g_Ctx.FocusedSliderId == id) g_Ctx.FocusedSliderId = 0;
        }

        if (!g_Ctx.MouseDown && g_Ctx.DraggingSliderId == id) g_Ctx.DraggingSliderId = 0;

        if (g_Ctx.DraggingSliderId == id) {
            float ratio = std::clamp((g_Ctx.MousePos.x - sliderPos.x) / sliderWidth, 0.f, 1.f);
            float newVal = min_val + ratio * (max_val - min_val);
            if (step > 0.f) newVal = std::round(newVal / step) * step;
            *value = std::clamp(newVal, min_val, max_val);
        }

        if (g_Ctx.FocusedSliderId == id) {
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

        DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + 2.f }, g_Ctx.Style.Colors[GuiCol_Text]);
        DrawRect(sliderPos, size, hovered ? g_Ctx.Style.Colors[GuiCol_FrameBgHovered] : g_Ctx.Style.Colors[GuiCol_FrameBg]);

        float fillWidth = std::clamp((*value - min_val) / (max_val - min_val), 0.f, 1.f) * sliderWidth;
        DrawRect(sliderPos, { fillWidth, size.y }, g_Ctx.Style.Colors[GuiCol_SliderGrab]);

        if (g_Ctx.FocusedSliderId == id) {
            Color border = g_Ctx.Style.Colors[GuiCol_Border];
            DrawRect({ sliderPos.x - 1.f, sliderPos.y - 1.f }, { size.x + 2.f, 1.f }, border);
            DrawRect({ sliderPos.x - 1.f, sliderPos.y + size.y }, { size.x + 2.f, 1.f }, border);
            DrawRect({ sliderPos.x - 1.f, sliderPos.y }, { 1.f, size.y }, border);
            DrawRect({ sliderPos.x + size.x, sliderPos.y }, { 1.f, size.y }, border);
        }

        std::string valStr = std::format("{:.2f}", *value);
        DrawTextString(valStr, { sliderPos.x + sliderWidth + 10.f, sliderPos.y + 2.f }, g_Ctx.Style.Colors[GuiCol_Text]);

        g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
    }

    inline void ColorPicker(std::string_view name, float* r, float* g, float* b, float* a) {
        if (!g_Ctx.InActiveTab) return;
        std::string_view display; size_t id; ParseLabel(name, display, id);

        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, g_Ctx.ItemHeight })) {
            g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
            return;
        }

        DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + 2.f }, g_Ctx.Style.Colors[GuiCol_Text]);

        float textWidth = MeasureTextSize(display).x;
        float controlOffsetX = textWidth + g_Ctx.Padding;

        Vec2 boxPos = { g_Ctx.Cursor.x + controlOffsetX, g_Ctx.Cursor.y };
        Vec2 boxSize = { 40.f, g_Ctx.ItemHeight };

        bool hovered = IsMouseHovering(boxPos, boxSize);
        if (hovered && g_Ctx.MouseClicked) {
            if (g_Ctx.ActiveColorPickerId == id) g_Ctx.ActiveColorPickerId = 0;
            else {
                g_Ctx.ActiveColorPickerId = id;
                g_Ctx.ColorPickerR = r; g_Ctx.ColorPickerG = g; g_Ctx.ColorPickerB = b; g_Ctx.ColorPickerA = a;
                g_Ctx.ColorPickerPos = { boxPos.x + boxSize.x + 10.f, boxPos.y };
                RGBtoHSV(*r, *g, *b, g_Ctx.ColorPickerH, g_Ctx.ColorPickerS, g_Ctx.ColorPickerV);
            }
            g_Ctx.MouseClicked = false;
        }

        DrawRect(boxPos, boxSize, { *r, *g, *b, *a });

        if (hovered || g_Ctx.ActiveColorPickerId == id) {
            Color border = g_Ctx.Style.Colors[GuiCol_Border];
            DrawRect({ boxPos.x - 1.f, boxPos.y - 1.f }, { boxSize.x + 2.f, 1.f }, border);
            DrawRect({ boxPos.x - 1.f, boxPos.y + boxSize.y }, { boxSize.x + 2.f, 1.f }, border);
            DrawRect({ boxPos.x - 1.f, boxPos.y }, { 1.f, boxSize.y }, border);
            DrawRect({ boxPos.x + boxSize.x, boxPos.y }, { 1.f, boxSize.y }, border);
        }

        g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
    }

    inline bool HotKey(std::string_view name, int* hotkey) {
        if (!g_Ctx.InActiveTab) return false;
        std::string_view display; size_t id; ParseLabel(name, display, id);

        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, g_Ctx.ItemHeight })) {
            g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
            return false;
        }

        DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + 2.f }, g_Ctx.Style.Colors[GuiCol_Text]);

        float textWidth = MeasureTextSize(display).x;
        float controlOffsetX = textWidth + g_Ctx.Padding;

        bool isAssigning = (g_Ctx.AssigningHotkey == hotkey);
        std::string keyName = isAssigning ? "[Press Key]" : std::format("[{}]", GetKeyName(*hotkey));

        Vec2 btnSize = { MeasureTextSize(keyName).x + 16.f, g_Ctx.ItemHeight };
        Vec2 btnPos = { g_Ctx.Cursor.x + controlOffsetX, g_Ctx.Cursor.y };

        bool btnHovered = IsMouseHovering(btnPos, btnSize);
        if (btnHovered && g_Ctx.MouseClicked) {
            g_Ctx.AssigningHotkey = hotkey;
            g_Ctx.MouseClicked = false;
        }

        DrawRect(btnPos, btnSize, btnHovered ? g_Ctx.Style.Colors[GuiCol_ButtonHovered] : g_Ctx.Style.Colors[GuiCol_Button]);
        DrawTextString(keyName, { btnPos.x + 8.f, btnPos.y + 2.f }, g_Ctx.Style.Colors[GuiCol_Text]);

        g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
        return *hotkey != 0 && g_Ctx.KeyStates[*hotkey];
    }

    inline void HotKey(std::string_view name, int* hotkey, bool* is_active, HotkeyMode* hotkey_mode) {
        if (!g_Ctx.InActiveTab) return;
        std::string_view display; size_t id; ParseLabel(name, display, id);

        if (!IsRectVisible(g_Ctx.Cursor, { g_Ctx.WindowSize.x, g_Ctx.ItemHeight })) {
            g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
            return;
        }

        DrawTextString(display, { g_Ctx.Cursor.x, g_Ctx.Cursor.y + 2.f }, g_Ctx.Style.Colors[GuiCol_Text]);

        float textWidth = MeasureTextSize(display).x;
        float controlOffsetX = textWidth + g_Ctx.Padding;

        bool isAssigning = (g_Ctx.AssigningHotkey == hotkey);
        std::vector<std::string> modeStrs = { "None", "Hold On", "Toggle On", "Hold Off", "Always On" };

        std::string keyName = isAssigning ? "[Press Key]" : std::format("[{}]", GetKeyName(*hotkey));

        Vec2 btnSize = { MeasureTextSize(keyName).x + 16.f, g_Ctx.ItemHeight };
        Vec2 btnPos = { g_Ctx.Cursor.x + controlOffsetX, g_Ctx.Cursor.y };

        bool btnHovered = IsMouseHovering(btnPos, btnSize);

        if (btnHovered) {
            if (g_Ctx.MouseClicked) {
                g_Ctx.AssigningHotkey = hotkey;
                g_Ctx.MouseClicked = false;
            }
            else if (g_Ctx.RightMouseClicked) {
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

        DrawRect(btnPos, btnSize, btnHovered ? g_Ctx.Style.Colors[GuiCol_ButtonHovered] : g_Ctx.Style.Colors[GuiCol_Button]);
        DrawTextString(keyName, { btnPos.x + 8.f, btnPos.y + 2.f }, g_Ctx.Style.Colors[GuiCol_Text]);

        switch (*hotkey_mode) {
        case HotkeyMode::None:      *is_active = false; break;
        case HotkeyMode::HoldOn:    *is_active = g_Ctx.KeyStates[*hotkey]; break;
        case HotkeyMode::HoldOff:   *is_active = !g_Ctx.KeyStates[*hotkey]; break;
        case HotkeyMode::ToggleOn:  *is_active = g_Ctx.HotkeyToggles[*hotkey]; break;
        case HotkeyMode::AlwaysOn:  *is_active = true; break;
        }

        float dotSize = std::max(6.f, g_Ctx.ItemHeight * 0.4f);
        float dotOffset = (g_Ctx.ItemHeight - dotSize) / 2.f;

        Color indicatorColor = *is_active ? g_Ctx.Style.Colors[GuiCol_ActiveIndicator] : g_Ctx.Style.Colors[GuiCol_InactiveIndicator];
        DrawRect({ btnPos.x + btnSize.x + 10.f, g_Ctx.Cursor.y + dotOffset }, { dotSize, dotSize }, indicatorColor);

        g_Ctx.Cursor.y += g_Ctx.ItemHeight + g_Ctx.Padding;
    }
}