# ✨ Shadow Gui

<img width="1059" height="701" alt="QQ20260710-205858" src="https://github.com/user-attachments/assets/2853dbf8-210c-4189-82eb-c9635ca197b0" />

### ☕ 赞助 / Buy me a coffee

* 您可以通过赞助支持我们的开发，感谢大家的支持！
* If this project helps you, feel free to support my work!
* **USDT (TRC-20):** `THzBDDbBkDh3nXRkCEeG4p5r733tWeAdib`

## Credits

* [ocornut/imgui](https://github.com/ocornut/imgui)
* [Encryqed/Dumper-7](https://github.com/Encryqed/Dumper-7)
* [Google AI Studio](https://aistudio.google.com/)

## 使用方法 / Usage

### 快速开始 / Quick Start

* `#include "Shadow-Gui/include/Shadow.h"`
* 修改 `Shadow.h` 的 `#include "../../CppSDK/SDK.hpp"` 为实际路径
* Modify `#include "../../CppSDK/SDK.hpp"` in `Shadow.h` to the actual path
* 在`UGameViewportClient::PostRender` 运行 `Shadow::NewFrame(Canvas);`
* Run `Shadow::NewFrame(Canvas);` in `UGameViewportClient::PostRender`

### 示例 / Example

#### 输入处理 / Input Processing

```cpp
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
```

#### 创建菜单 / Creating Menu

```cpp
void __fastcall hkPostRender(SDK::UGameViewportClient* rcx, SDK::UCanvas* canvas) {

    // 放行常用移动按键给游戏，避免菜单打开时玩家动不了
    // Pass common movement keys through to the game to prevent players from getting stuck when the menu is open
    Shadow::SetAllowedKeys({ 'W', 'A', 'S', 'D', VK_SPACE });

    // 开始新的一帧
    // Begin a new frame
    Shadow::NewFrame(canvas);

    // 更新所有热键状态
    // Update all hotkey states
    Shadow::UpdateAllHotkeyStates();

    if (Shadow::Begin("Main Menu##main_window", Shadow::ShadowWindowFlags_NoResize)) {
        if (Shadow::BeginTabBar("MainTabs##tabs", Shadow::ShadowTabBarFlags_Reorderable)) {
            if (Shadow::BeginTabItem("Misc##tab0")) {
                Shadow::TextColored({ 0.0f, 1.0f, 0.0f, 1.0f }, U8("你好！"));
            }
            Shadow::EndTabItem();
        }
        Shadow::EndTabBar();
    }
    Shadow::End();
}

```

## API

### 📌 基础控件 / Basic Controls

### Shadow::TextColored

绘制一行彩色文本。
Draws a line of colored text.

```cpp
void Shadow::TextColored(Color color, std::string_view text);

```

### Shadow::Button

绘制一个按钮。当按钮在当前帧被点击时，返回 `true`。
Draws a button. Returns `true` when the button is clicked in the current frame.

```cpp
bool Shadow::Button(std::string_view name);

```

### Shadow::Checkbox

绘制一个复选框，并绑定一个布尔值变量来存储其状态。
Draws a checkbox and binds a boolean variable to store its state.

```cpp
void Shadow::Checkbox(std::string_view name, bool* value);

```

### Shadow::Switch

绘制一个开关，并绑定一个布尔值变量来存储其状态。
Draws a switch and binds a boolean variable to store its state.

```cpp
void Shadow::Switch(std::string_view name, bool* value);

```

### Shadow::Slider

绘制一个数值滑动调节条，支持鼠标拖拽、键盘左右键微调以及点击文本框直接输入数值。
Draws a numeric slider that supports mouse dragging, keyboard left/right arrow key fine-tuning, and clicking the text box for direct numeric input.

```cpp
void Shadow::Slider(std::string_view name, float* value, float min_val, float max_val, float step = 0.f);

```

### Shadow::Combo

绘制一个下拉列表选项框。当选项发生更改被选中时，返回 `true`。
Draws a dropdown combo selection box. Returns `true` when the option changes and is selected.

```cpp
bool Shadow::Combo(std::string_view name, int* current_item, const std::vector<std::string>& items);

```

### Shadow::ColorPicker

绘制一个支持色相、饱和度、明度及透明度的高级颜色选择器。可选传入 Alpha 指针，若传入 `nullptr` 则不显示透明度调节。
Draws an advanced color picker that supports hue, saturation, value, and alpha. Optionally takes an Alpha pointer; if `nullptr` is passed, the alpha adjustment bar will not be displayed.

```cpp
void Shadow::ColorPicker(std::string_view name, float* r, float* g, float* b, float* a);

```

### Shadow::HotKey (基础版) / (Basic Version)

绘制一个热键绑定控件。当玩家处于绑定状态时可按下对应按键进行设定。返回 `true` 代表该热键当前正被按下。
Draws a hotkey binding control. When the player is in binding state, they can press a corresponding key to set it. Returns `true` if the hotkey is currently being pressed.

```cpp
bool Shadow::HotKey(std::string_view name, int* hotkey);

```

### Shadow::HotKey (高级版) / (Advanced Version)

绘制一个高级热键绑定控件，支持鼠标右键点击弹出菜单，选择按键触发模式（如按下开启、长按开启等）。绑定 `is_active` 状态。
Draws an advanced hotkey binding control. Supports right-clicking to open a popup menu to select key trigger modes (e.g., hold to turn on, toggle to turn on, etc.). Binds to the `is_active` state.

```cpp
void Shadow::HotKey(std::string_view name, int* hotkey, bool* is_active, HotkeyMode* hotkey_mode);

```

---

### 📌 窗口与帧生命周期 / Window and Frame Lifecycle

### Shadow::NewFrame

开始并初始化新的一帧，设置绘制画布并清理上一帧状态。通常在 `UGameViewportClient::PostRender` 的最开始调用。
Begins and initializes a new frame, sets the drawing canvas, and clears the state of the previous frame. Usually called at the very beginning of `UGameViewportClient::PostRender`.

```cpp
void Shadow::NewFrame(SDK::UCanvas* Canvas);

```

### Shadow::Begin

创建或开始绘制一个窗口。可通过 `flags` 设置窗口属性（如禁止移动、无缩放手柄等）。
Creates or begins drawing a window. Window attributes can be configured via `flags` (e.g., no movement, no resize handle, etc.).

```cpp
bool Shadow::Begin(std::string_view name, ShadowWindowFlags flags = ShadowWindowFlags_None);

```

### Shadow::End

结束当前窗口的绘制与事件处理。必须与 `Begin` 成对出现。
Ends the drawing and event processing of the current window. Must be paired with `Begin`.

```cpp
void Shadow::End();

```

---

### 📌 标签页系统 (TabBar) / Tab Bar System

### Shadow::BeginTabBar

开始绘制一个标签栏区域（Tab Bar）。支持通过 `flags` 开启拖拽重排或超出横向滚动等属性。
Begins drawing a tab bar area. Supports enabling features like drag-and-drop reordering or horizontal scrolling when full via `flags`.

```cpp
bool Shadow::BeginTabBar(std::string_view name, ShadowTabBarFlags flags = ShadowTabBarFlags_None);

```

### Shadow::EndTabBar

结束标签栏区域的绘制。必须与 `BeginTabBar` 成对出现。
Ends the drawing of the tab bar area. Must be paired with `BeginTabBar`.

```cpp
void Shadow::EndTabBar();

```

### Shadow::BeginTabItem

开始绘制一个具体的标签页（Tab Item）。返回 `true` 表示该标签页处于选中激活状态，此时方可渲染其内部控件。
Begins drawing a specific tab item. Returns `true` if the tab item is selected and active, at which point its internal controls can be rendered.

```cpp
bool Shadow::BeginTabItem(std::string_view name);

```

### Shadow::EndTabItem

结束当前标签页的内容绘制。必须与 `BeginTabItem` 成对出现。
Ends the content drawing of the current tab item. Must be paired with `BeginTabItem`.

```cpp
void Shadow::EndTabItem();

```

---

### 📌 布局与状态控制 / Layout and State Control

### Shadow::Spacing

在垂直方向上追加一段间距（按当前主题 `ItemSpacing.y` 尺寸）。
Appends a spacing in the vertical direction (based on the current theme's `ItemSpacing.y` size).

```cpp
void Shadow::Spacing();

```

### Shadow::SameLine

取消控件的默认垂直换行，使得下一个将要绘制的控件紧挨着放在当前行末的右侧。
Cancels the default vertical line break of controls, making the next control to be drawn placed immediately to the right of the end of the current line.

```cpp
void Shadow::SameLine(float offset_from_start_x = 0.0f, float spacing = -1.0f);

```

### Shadow::BeginDisabled

开始一个禁用交互区块。区块内的所有输入控件将变灰，且不可被点击与交互。
Begins a disabled interaction block. All input controls within the block will turn gray and cannot be clicked or interacted with.

```cpp
void Shadow::BeginDisabled(bool disabled = true);

```

### Shadow::EndDisabled

结束当前的禁用交互区块。
Ends the current disabled interaction block.

```cpp
void Shadow::EndDisabled();

```

### Shadow::IsDisabled

获取当前上下文是否正处于禁用交互状态。
Gets whether the current context is currently in a disabled interaction state.

```cpp
bool Shadow::IsDisabled();

```

---

### 📌 全局输入与按键拦截 / Global Input and Key Interception

### Shadow::Input

处理窗口消息，拦截游戏界面的键盘鼠标及滚轮输入。需挂钩 `WndProc` 并传入。
Processes window messages, intercepting keyboard, mouse, and scroll wheel input on the game interface. Needs to be hooked in `WndProc` and passed in.

```cpp
LRESULT Shadow::Input(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

```

### Shadow::ProcessGlobalHotkeys

处理全局热键输入，即使 GUI 菜单关闭时，也能够响应并更新已注册热键的状态。
Processes global hotkey input, allowing responses and updating the status of registered hotkeys even when the GUI menu is closed.

```cpp
bool Shadow::ProcessGlobalHotkeys(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

```

### Shadow::UpdateAllHotkeyStates

每帧更新所有已注册热键的工作模式和逻辑激活状态（例如计算切换、长按等模式的结果）。
Updates the working mode and logical activation state of all registered hotkeys every frame (e.g., calculating results for toggle, hold, etc. modes).

```cpp
void Shadow::UpdateAllHotkeyStates();

```

### Shadow::SetAllowedKeys

设置允许透传给游戏后台的白名单按键列表（比如用来防止打开菜单时不能控制角色移动）。
Sets the whitelist of keys allowed to pass through to the game background (e.g., used to prevent the inability to control character movement when the menu is open).

```cpp
void Shadow::SetAllowedKeys(const std::vector<int>& keys);

```

### Shadow::SetAllowedMouseMsgs

设置允许透传给游戏后台的白名单鼠标消息列表（如视角控制所需的消息）。
Sets the whitelist of mouse messages allowed to pass through to the game background (e.g., messages required for camera control).

```cpp
void Shadow::SetAllowedMouseMsgs(const std::vector<UINT>& msgs);

```

### Shadow::IsKeyAllowed

检查指定虚拟按键码是否在允许透传的白名单列表中。
Checks if the specified virtual key code is in the whitelist of keys allowed to pass through.

```cpp
bool Shadow::IsKeyAllowed(int key);

```

### Shadow::IsMouseMsgAllowed

检查指定的鼠标窗口消息是否在允许透传的白名单列表中。
Checks if the specified mouse window message is in the whitelist of mouse messages allowed to pass through.

```cpp
bool Shadow::IsMouseMsgAllowed(UINT uMsg);

```

### Shadow::RegisterHotkey

手动在后端注册一个全局热键及触发模式。
Manually registers a global hotkey and its trigger mode in the backend.

```cpp
void Shadow::RegisterHotkey(int* hotkey, HotkeyMode* mode, bool* is_active);

```

### Shadow::IsHotkeyRegistered

判断指定的虚拟按键是否已经被注册成了有效热键。
Determines whether the specified virtual key has already been registered as a valid hotkey.

```cpp
bool Shadow::IsHotkeyRegistered(int vk);

```

---

### 📌 窗口属性与外观主题 / Window Properties and Appearance Themes

### Shadow::GetStyle

获取全局 GUI 样式实例。可以通过修改该对象实时调节颜色、控件间距、边距及各项尺寸。
Gets the global GUI style instance. You can modify this object in real time to adjust colors, control spacing, padding, and various sizes.

```cpp
GuiStyle& Shadow::GetStyle();

```

### Shadow::StyleColorsOcean

应用“深海（Ocean）”蓝色调 GUI 主题配色。
Applies the "Ocean" blue-toned GUI theme color scheme.

```cpp
void Shadow::StyleColorsOcean();

```

### Shadow::StyleColorsAmethyst

应用“紫曜（Amethyst）”紫色调 GUI 主题配色。
Applies the "Amethyst" purple-toned GUI theme color scheme.

```cpp
void Shadow::StyleColorsAmethyst();

```

### Shadow::GetWindowSize

获取当前操作窗口的宽高尺寸。
Gets the width and height dimensions of the current operating window.

```cpp
Vec2 Shadow::GetWindowSize();

```

### Shadow::GetWindowPos

获取当前操作窗口左上角在屏幕上的坐标。
Gets the coordinates of the top-left corner of the current operating window on the screen.

```cpp
Vec2 Shadow::GetWindowPos();

```

### Shadow::SetWindowPos

手动覆盖并设置当前窗口的起始绘制位置。
Manually overrides and sets the starting drawing position of the current window.

```cpp
void Shadow::SetWindowPos(Vec2 pos);

```

### Shadow::SetNextWindowSizeConstraints

对下一个将要渲染的窗口，限制其用户可自由拖放缩放的最小及最大尺寸。
Restricts the minimum and maximum dimensions for user resizing on the next window to be rendered.

```cpp
void Shadow::SetNextWindowSizeConstraints(Vec2 min_size, Vec2 max_size);

```

---

### 📌 字体与文本处理 / Fonts and Text Processing

### Shadow::PushFont

将指定的 `UFont` 与缩放比例压入字体栈中，后续所渲染的文本（包括组件自带）将使用该字体。
Pushes the specified `UFont` and scale factor into the font stack. Subsequently rendered text (including built-in components) will use this font.

```cpp
void Shadow::PushFont(SDK::UFont* font, float scale = 1.0f);

```

### Shadow::PopFont

弹出并恢复到上一个（或默认）字体上下文。
Pops and restores to the previous (or default) font context.

```cpp
void Shadow::PopFont();

```

### Shadow::MeasureTextSize

测量指定文本字符串在当前字体与 DPI 缩放配置下的像素宽高。
Measures the pixel width and height of the specified text string under the current font and DPI scale configuration.

```cpp
Vec2 Shadow::MeasureTextSize(std::string_view text);

```

### Shadow::MeasureTextHeight

测量指定文本字符串在当前字体与 DPI 缩放配置下的高度。
Measures the height of the specified text string under the current font and DPI scale configuration.

```cpp
float Shadow::MeasureTextHeight(std::string_view text);

```

### Shadow::MeasureCharWidth

测量单个宽字符在当前字体配置下的宽度。
Measures the width of a single wide character under the current font configuration.

```cpp
float Shadow::MeasureCharWidth(wchar_t ch);

```

---

### 📌 底层绘制与剪裁引擎 / Low-level Drawing and Clipping Engine

### Shadow::DrawTextString

使用绝对坐标，底层调用 Canvas 绘制不加任何额外封装的文本。
Uses absolute coordinates to call Canvas at the low level to draw raw text without any extra encapsulation.

```cpp
void Shadow::DrawTextString(std::string_view text, Vec2 pos, Color color);

```

### Shadow::DrawLine

在画布上绘制一条指定颜色和粗细的半透明图层直线。
Draws a semi-transparent layer straight line of the specified color and thickness on the canvas.

```cpp
void Shadow::DrawLine(Vec2 start, Vec2 end, Color color, float thickness = 1.0f);

```

### Shadow::DrawRect

绘制一个具有指定颜色线条边界的空心矩形框。
Draws a hollow rectangular box with specified color outlines.

```cpp
void Shadow::DrawRect(Vec2 pos, Vec2 size, Color color);

```

### Shadow::DrawRectFilled

绘制一个使用指定颜色填充内部的实心矩形块。
Draws a solid rectangular block filled internally with the specified color.

```cpp
void Shadow::DrawRectFilled(Vec2 pos, Vec2 size, Color color);

```

### Shadow::PushClipRect

推入自定义显示裁剪矩形边界，超出此边界的内容将不会被渲染（主要用于滚动列表或 TabBar 视图区域控制）。
Pushes a custom display clipping rectangular boundary; content exceeding this boundary will not be rendered (mainly used for scroll lists or TabBar viewport area control).

```cpp
void Shadow::PushClipRect(Vec2 min, Vec2 max);

```

### Shadow::PopClipRect

弹出并销毁最近的一层自定义显示裁剪边界。
Pops and destroys the most recent layer of custom display clipping boundaries.

```cpp
void Shadow::PopClipRect();

```

### Shadow::IsRectVisible

判断当前指定的矩形坐标范围是否在渲染边界内（只要有一部分可见即返回 `true`）。
Determines whether the currently specified rectangle coordinate range is within the rendering boundaries (returns `true` if even a part of it is visible).

```cpp
bool Shadow::IsRectVisible(Vec2 pos, Vec2 size);

```

### Shadow::IsRectFullyVisible

判断当前指定的矩形坐标范围是否完全存在于渲染剪裁边界之内。
Determines whether the currently specified rectangle coordinate range lies entirely within the rendering clip boundaries.

```cpp
bool Shadow::IsRectFullyVisible(Vec2 pos, Vec2 size);

```

---

### 📌 系统功能及辅助工具 / System Functions and Utilities

### Shadow::SetClipboardText

将指定的文本字符串写入 Windows 系统剪贴板中。
Writes the specified text string into the Windows system clipboard.

```cpp
void Shadow::SetClipboardText(const std::string& text);

```

### Shadow::GetClipboardText

从 Windows 系统剪贴板中提取并返回当中文本。
Extracts and returns the text content from the Windows system clipboard.

```cpp
std::string Shadow::GetClipboardText();

```

### Shadow::GetKeyName

将 Windows 虚拟键码（VK Code）翻译转换为如 "Mouse 1"、"Ctrl"、"Enter" 这样的人类可读字符串名。
Translates and converts Windows virtual key codes (VK Codes) into human-readable string names like "Mouse 1", "Ctrl", "Enter".

```cpp
std::string Shadow::GetKeyName(int vk);

```

### Shadow::HashString

将字符串转换为唯一哈希值以做内部控件的唯一 ID 注册。
Converts a string into a unique hash value for internal control unique ID registration.

```cpp
constexpr size_t Shadow::HashString(std::string_view str) noexcept;

```

### Shadow::ToWString

提供将 UTF-8 多字节字符串安全转换为 Windows 宽字符集 (`std::wstring`) 格式的实用方法。
Provides a utility method to safely convert UTF-8 multi-byte strings to Windows wide character set (`std::wstring`) format.

```cpp
std::wstring Shadow::ToWString(std::string_view utf8_str);

```

### Shadow::RGBtoHSV

将红绿蓝（RGB）色彩空间值转换为色相/饱和度/明度（HSV）值。
Converts Red-Green-Blue (RGB) color space values to Hue/Saturation/Value (HSV) values.

```cpp
void Shadow::RGBtoHSV(float r, float g, float b, float& h, float& s, float& v);

```

### Shadow::HSVtoRGB

将色相/饱和度/明度（HSV）值转换为红绿蓝（RGB）色彩空间值。
Converts Hue/Saturation/Value (HSV) values to Red-Green-Blue (RGB) color space values.

```cpp
void Shadow::HSVtoRGB(float h, float s, float v, float& r, float& g, float& b);

```

---

## 数据结构与枚举 / Data Structures and Enums

### 📌 热键模式 (HotkeyMode) / Hotkey Mode (HotkeyMode)

用于 `Shadow::HotKey` (高级版)，定义热键的触发逻辑。
Used for `Shadow::HotKey` (Advanced Version), defining the trigger logic of hotkeys.

```cpp
enum class HotkeyMode {
    None,       // 无模式（默认不激活） / No mode (inactive by default)
    HoldOn,     // 按住时激活，松开后失效 / Active when held, invalid after release
    ToggleOn,   // 切换模式（按一次激活，再按一次关闭） / Toggle mode (press once to activate, press again to turn off)
    HoldOff,    // 松开时激活（不按的时候为激活状态，按下后关闭） / Active when released (remains active when not pressed, turns off when pressed)
    AlwaysOn    // 永远保持激活状态，无视按键 / Always active, ignoring key presses
};

```

---

### 📌 窗口标志位 (ShadowWindowFlags_) / Window Flags (ShadowWindowFlags_)

用于 `Shadow::Begin`，控制窗口的行为与标题栏对齐方式。支持通过按位或 `|` 组合使用。
Used for `Shadow::Begin`, controlling window behavior and title bar alignment. Supports bitwise OR `|` combinations.

```cpp
enum ShadowWindowFlags_ {
    ShadowWindowFlags_None = 0,                 // 默认，无特殊标志 / Default, no special flags
    ShadowWindowFlags_NoResize = 1 << 0,        // 禁止通过右下角手柄缩放窗口 / Disallow window resizing via the bottom-right handle
    ShadowWindowFlags_NoMove = 1 << 1,          // 禁止通过拖拽标题栏与背景移动窗口 / Disallow moving the window by dragging the title bar and background
    ShadowWindowFlags_NoScrollbar = 1 << 2,     // 隐藏窗口垂直滚动条（但仍可通过滚轮滚动） / Hide the window's vertical scrollbar (scrolling via wheel still works)
    
    ShadowWindowFlags_TextAlignLeft = 0,        // 标题栏文本左对齐（默认） / Left-align title bar text (default)
    ShadowWindowFlags_TextAlignCenter = 1 << 3, // 标题栏文本居中对齐 / Center-align title bar text
    ShadowWindowFlags_TextAlignRight = 1 << 4,  // 标题栏文本右对齐 / Right-align title bar text
};

```

---

### 📌 标签栏标志位 (ShadowTabBarFlags_) / Tab Bar Flags (ShadowTabBarFlags_)

用于 `Shadow::BeginTabBar`，控制标签栏的排版与交互逻辑。支持通过按位或 `|` 组合使用。
Used for `Shadow::BeginTabBar`, controlling the layout and interaction logic of the tab bar. Supports bitwise OR `|` combinations.

```cpp
enum ShadowTabBarFlags_ {
    ShadowTabBarFlags_None = 0,                     // 默认，无特殊标志 / Default, no special flags
    ShadowTabBarFlags_Reorderable = 1 << 0,         // 允许玩家通过鼠标拖拽重新排列标签页(Tab)的顺序 / Allow players to reorder tabs by dragging with the mouse
    ShadowTabBarFlags_FittingPolicyScroll = 1 << 1, // 当标签页总宽度超出窗口边界时，开启水平滚动策略 / Enable horizontal scrolling policy when total tab width exceeds window boundaries
    ShadowTabBarFlags_NoScrollbar = 1 << 2,         // 隐藏标签栏水平滚动条（但仍可通过滚轮滚动） / Hide the tab bar's horizontal scrollbar (scrolling via wheel still works)
};

```

---

### 📌 颜色枚举 (GuiCol_) / Color Enums (GuiCol_)

用于 `g_Ctx.Style.Colors` 数组索引，代表 GUI 库中所有的可配色的元素。开发者可以通过替换这些索引处的颜色值来自定义主题。
Used as array indices for `g_Ctx.Style.Colors`, representing all colorable elements in the GUI library. Developers can customize themes by replacing color values at these indices.

```cpp
enum GuiCol_ {
    // 基础背景 / Basic Backgrounds
    GuiCol_WindowBg,             // 窗口主背景色 / Main background color of the window
    GuiCol_TitleBarBg,           // 窗口标题栏背景色 / Background color of the window title bar
    GuiCol_PopupBg,              // 弹出层(下拉框、颜色选择器)背景色 / Background color of popups (dropdowns, color pickers)
    GuiCol_PopupBorder,          // 弹出层边框色 / Border color of popups
    GuiCol_FrameBg,              // 控件(输入框、滑动条背景等)基础背景色 / Basic background color of controls (input boxes, slider backgrounds, etc.)
    GuiCol_FrameBgHovered,       // 控件鼠标悬停时的背景色 / Background color of controls when mouse is hovered

    // 文本颜色 / Text Colors
    GuiCol_Text,                 // 基础文本颜色 / Basic text color
    GuiCol_TextDisabled,         // 禁用状态或提示文本颜色 / Color for disabled state or hint text
    GuiCol_TextHighlight,        // 高亮文本颜色(如当前选中的Tab或下拉项) / Highlighted text color (e.g., currently selected Tab or dropdown item)
    GuiCol_ErrorText,            // 报错信息文本颜色 / Color for error message text
    GuiCol_TextShadow,           // 文本阴影颜色 (底层引擎使用) / Text shadow color (used by the low-level engine)
    GuiCol_TextOutline,          // 文本描边颜色 (底层引擎使用) / Text outline color (used by the low-level engine)

    // 交互控件 / Interactive Controls
    GuiCol_Button,               // 按钮基础颜色 / Basic color of buttons
    GuiCol_ButtonHovered,        // 按钮鼠标悬停颜色 / Button color when mouse is hovered
    GuiCol_SliderGrab,           // 滑动条拖拽滑块/选中填充颜色 / Slider grab / selected fill color
    GuiCol_CheckMark,            // 复选框打勾标识的颜色 / Color of the checkbox check mark
    GuiCol_ActiveIndicator,      // 热键状态开启时的指示灯颜色 / Indicator light color when hotkey state is active
    GuiCol_InactiveIndicator,    // 热键状态关闭时的指示灯颜色 / Indicator light color when hotkey state is inactive
    GuiCol_ControlDisabled,      // 被禁用控件的覆盖背景色 / Overlay background color of disabled controls

    // 边界与手柄 / Boundaries and Handles
    GuiCol_Separator,            // 分割线/标签栏底线颜色 / Color of separators / tab bar bottom lines
    GuiCol_Border,               // 基础边框线颜色 / Basic border line color
    GuiCol_ResizeGrip,           // 右下角缩放手柄默认颜色 / Default color of the bottom-right resize grip
    GuiCol_ResizeGripHovered,    // 缩放手柄鼠标悬停颜色 / Resize grip color when mouse is hovered
    GuiCol_ResizeGripActive,     // 缩放手柄鼠标按下拖拽时的颜色 / Resize grip color when pressed and dragged

    // 标签页 (Tab) / Tab Pages (Tab)
    GuiCol_Tab,                  // 未选中的标签页颜色 / Color of unselected tabs
    GuiCol_TabHovered,           // 未选中但鼠标悬停的标签页颜色 / Color of unselected but hovered tabs
    GuiCol_TabActive,            // 当前选中的标签页颜色 / Color of the currently selected tab

    // 颜色选择器 (ColorPicker) 专属 / ColorPicker Exclusive
    GuiCol_ColorPickerDark,      // 颜色选择器内部深色游标/边框 / Inner dark cursor/border of the color picker
    GuiCol_ColorPickerLight,     // 颜色选择器内部浅色游标/边框(及全局Alpha参考) / Inner light cursor/border of the color picker (and global Alpha reference)
    GuiCol_CheckerboardLight,    // 颜色选择器透明度棋盘格-浅色块 / Color picker alpha checkerboard - light block
    GuiCol_CheckerboardDark,     // 颜色选择器透明度棋盘格-深色块 / Color picker alpha checkerboard - dark block
    GuiCol_ColorPickerShadow,    // 颜色选择器明度(V)渐变遮罩色 / Color picker value (V) gradient mask color

    GuiCol_COUNT                 // 枚举最大值（用于数组大小分配） / Maximum enum value (used for array size allocation)
};

```

---

### 📌 全局样式配置 (GuiStyle) / Global Style Configuration (GuiStyle)

存放所有关于界面尺寸、间距及颜色的设定。可以通过 `Shadow::GetStyle()` 获取它的引用并动态修改。
Stores all settings regarding interface sizes, spacing, and colors. Its reference can be retrieved via `Shadow::GetStyle()` and modified dynamically.

```cpp
struct GuiStyle {
    Color Colors[GuiCol_COUNT];          // 所有界面元素的颜色数组 / Array of colors for all interface elements

    // 尺寸与间距标志配置 / Size and Spacing Configurations
    Vec2 WindowPadding = { 16.f, 16.f }; // 窗口内部四周的留白边距 (X=水平, Y=垂直) / Inside padding on all sides of the window (X=Horizontal, Y=Vertical)
    Vec2 FramePadding = { 8.f, 2.f };    // 控件内部边缘与文本内容的间距 (X为水平边距, Y为垂直居中补偿) / Padding between control inner edge and text content (X is horizontal padding, Y is vertical centering compensation)
    Vec2 ItemSpacing = { 10.f, 8.f };    // 相邻控件之间的水平与垂直间距 / Horizontal and vertical spacing between adjacent controls
    float ScrollbarSize = 10.f;          // 滚动条的宽度 / Width of the scrollbar
    float ScrollbarMargin = 4.f;         // 滚动条与右侧边缘/其他元素的间距 / Spacing between scrollbar and right edge/other elements
    float ResizeGripSize = 16.f;         // 右下角缩放窗口手柄的尺寸 / Size of the bottom-right window resizing grip
    float TabExtraWidth = 20.f;          // Tab标签在文字宽度之外额外预留的宽度 / Extra padding width reserved for Tab labels beyond text width

    // 控件特定对齐配置 (右侧对齐布局用) / Control Specific Alignment Configurations (For right-aligned layout)
    float ControlOffsetMin = 120.f;      // 右侧互动控件(Slider, Hotkey等)对齐的最小左侧偏移绝对值 / Minimum left absolute offset for aligning right interactive controls (Slider, Hotkey, etc.)
    float ControlOffsetRatio = 0.4f;     // 右侧互动控件偏移量占整体窗口宽度的比例 / Ratio of right interactive control offset to the overall window width

    // ColorPicker 弹出层专属尺寸配置 / ColorPicker Popup Exclusive Size Configurations
    float CPPadding = 8.f;               // 颜色选择器面板的内边距 / Inner padding of the color picker panel
    float CPSVSize = 200.f;              // 色彩(SV)方形选取面板的尺寸(宽高相同) / Size of the color (SV) square selection panel (same width and height)
    float CPHueWidth = 20.f;             // 色相(Hue)竖条的宽度 / Width of the Hue vertical bar
    float CPAlphaWidth = 20.f;           // 透明度(Alpha)竖条的宽度 / Width of the Alpha vertical bar
    float CPSpacing = 8.f;               // 面板内各个区块之间的间距 / Spacing between various sections within the panel

    // 窗口与全局缩放 / Window and Global Scaling
    Vec2 WindowMinSize = { 200.f, 150.f };// 窗口允许被缩放至的最小尺寸限制 / Minimum size constraint allowed for window resizing
    float FontScaleDpi = 1.0f;           // 默认 1.0，全局控制文本与菜单自适应渲染缩放比 / Default 1.0, globally controls adaptive rendering scale ratio for text and menus
};

```
