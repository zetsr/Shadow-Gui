# ✨ Shadow Gui

<img width="1059" height="701" alt="QQ20260710-205858" src="https://github.com/user-attachments/assets/2853dbf8-210c-4189-82eb-c9635ca197b0" />

## Credits

- [ocornut/imgui](https://github.com/ocornut/imgui)
- [Encryqed/Dumper-7](https://github.com/Encryqed/Dumper-7)
- [Google AI Studio](https://aistudio.google.com/)

## 使用方法

### 快速开始

- `#include "Shadow-Gui/include/Shadow.h"`
- 修改 `Shadow.h` 的 `#include "../../CppSDK/SDK.hpp"` 为实际路径
- 在`UGameViewportClient::PostRender` 运行 `Shadow::NewFrame(Canvas);`

### 示例

#### 输入处理
```cpp
LRESULT APIENTRY WndProcHook(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // 处理全局热键
    Shadow::ProcessGlobalHotkeys(hwnd, uMsg, wParam, lParam);

    // 处理输入
    Shadow::Input(hwnd, uMsg, wParam, lParam);

    // 菜单切换
    if (uMsg == WM_KEYDOWN && wParam == keyMenu && !Shadow::g_Ctx.AssigningHotkey) {
        bool isFirstPress = ((lParam & (1 << 30)) == 0);

        if (isFirstPress) {
            bShowMenu = !bShowMenu;
        }
    }

    // 如果菜单打开
    if (bShowMenu || currentAlpha > 0.001f) {
        // 如果是鼠标消息，且不在白名单里，直接阻塞
        if (!Shadow::IsMouseMsgAllowed(uMsg)) {
            return 1;
        }

        // 键盘消息处理
        if (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP || uMsg == WM_CHAR) {
            // 如果是已注册的全局热键，放行给游戏
            if (Shadow::IsHotkeyRegistered(static_cast<int>(wParam))) {
                return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
            }

            // 如果不在允许列表中，阻塞该按键
            if (!Shadow::IsKeyAllowed(static_cast<int>(wParam))) {
                return 1;
            }
        }
    }

    return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
}
```

#### 创建菜单
```cpp
void __fastcall hkPostRender(SDK::UGameViewportClient* rcx, SDK::UCanvas* canvas) {

	// 放行常用移动按键给游戏，避免菜单打开时玩家动不了
    Shadow::SetAllowedKeys({ 'W', 'A', 'S', 'D', VK_SPACE });

    // 开始新的一帧
	Shadow::NewFrame(canvas);

	// 更新所有热键状态
    Shadow::UpdateAllHotkeyStates();

	if (Shadow::Begin("Main Menu##main_window", Shadow::ShadowWindowFlags_NoResize)) {
        if (Shadow::BeginTabBar("MainTabs##tabs", Shadow::ShadowTabBarFlags_Reorderable)) {
            if (Shadow::BeginTabItem(U8("Misc##tab0"))) {
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

### 📌 基础控件

### Shadow::TextColored
绘制一行彩色文本。
```cpp
void Shadow::TextColored(Color color, std::string_view text);
```

### Shadow::Button
绘制一个按钮。当按钮在当前帧被点击时，返回 `true`。
```cpp
bool Shadow::Button(std::string_view name);
```

### Shadow::Checkbox
绘制一个复选框，并绑定一个布尔值变量来存储其状态。
```cpp
void Shadow::Checkbox(std::string_view name, bool* value);
```

### Shadow::Switch
绘制一个开关，并绑定一个布尔值变量来存储其状态。
```cpp
void Shadow::Switch(std::string_view name, bool* value);
```

### Shadow::Slider
绘制一个数值滑动调节条，支持鼠标拖拽、键盘左右键微调以及点击文本框直接输入数值。
```cpp
void Shadow::Slider(std::string_view name, float* value, float min_val, float max_val, float step = 0.f);
```

### Shadow::Combo
绘制一个下拉列表选项框。当选项发生更改被选中时，返回 `true`。
```cpp
bool Shadow::Combo(std::string_view name, int* current_item, const std::vector<std::string>& items);
```

### Shadow::ColorPicker
绘制一个支持色相、饱和度、明度及透明度的高级颜色选择器。可选传入 Alpha 指针，若传入 `nullptr` 则不显示透明度调节。
```cpp
void Shadow::ColorPicker(std::string_view name, float* r, float* g, float* b, float* a);
```

### Shadow::HotKey (基础版)
绘制一个热键绑定控件。当玩家处于绑定状态时可按下对应按键进行设定。返回 `true` 代表该热键当前正被按下。
```cpp
bool Shadow::HotKey(std::string_view name, int* hotkey);
```

### Shadow::HotKey (高级版)
绘制一个高级热键绑定控件，支持鼠标右键点击弹出菜单，选择按键触发模式（如按下开启、长按开启等）。绑定 `is_active` 状态。
```cpp
void Shadow::HotKey(std::string_view name, int* hotkey, bool* is_active, HotkeyMode* hotkey_mode);
```

---

### 📌 窗口与帧生命周期

### Shadow::NewFrame
开始并初始化新的一帧，设置绘制画布并清理上一帧状态。通常在 `UGameViewportClient::PostRender` 的最开始调用。
```cpp
void Shadow::NewFrame(SDK::UCanvas* Canvas);
```

### Shadow::Begin
创建或开始绘制一个窗口。可通过 `flags` 设置窗口属性（如禁止移动、无缩放手柄等）。
```cpp
bool Shadow::Begin(std::string_view name, ShadowWindowFlags flags = ShadowWindowFlags_None);
```

### Shadow::End
结束当前窗口的绘制与事件处理。必须与 `Begin` 成对出现。
```cpp
void Shadow::End();
```

---

### 📌 标签页系统 (TabBar)

### Shadow::BeginTabBar
开始绘制一个标签栏区域（Tab Bar）。支持通过 `flags` 开启拖拽重排或超出横向滚动等属性。
```cpp
bool Shadow::BeginTabBar(std::string_view name, ShadowTabBarFlags flags = ShadowTabBarFlags_None);
```

### Shadow::EndTabBar
结束标签栏区域的绘制。必须与 `BeginTabBar` 成对出现。
```cpp
void Shadow::EndTabBar();
```

### Shadow::BeginTabItem
开始绘制一个具体的标签页（Tab Item）。返回 `true` 表示该标签页处于选中激活状态，此时方可渲染其内部控件。
```cpp
bool Shadow::BeginTabItem(std::string_view name);
```

### Shadow::EndTabItem
结束当前标签页的内容绘制。必须与 `BeginTabItem` 成对出现。
```cpp
void Shadow::EndTabItem();
```

---

### 📌 布局与状态控制

### Shadow::Spacing
在垂直方向上追加一段间距（按当前主题 `ItemSpacing.y` 尺寸）。
```cpp
void Shadow::Spacing();
```

### Shadow::SameLine
取消控件的默认垂直换行，使得下一个将要绘制的控件紧挨着放在当前行末的右侧。
```cpp
void Shadow::SameLine(float offset_from_start_x = 0.0f, float spacing = -1.0f);
```

### Shadow::BeginDisabled
开始一个禁用交互区块。区块内的所有输入控件将变灰，且不可被点击与交互。
```cpp
void Shadow::BeginDisabled(bool disabled = true);
```

### Shadow::EndDisabled
结束当前的禁用交互区块。
```cpp
void Shadow::EndDisabled();
```

### Shadow::IsDisabled
获取当前上下文是否正处于禁用交互状态。
```cpp
bool Shadow::IsDisabled();
```

---

### 📌 全局输入与按键拦截

### Shadow::Input
处理窗口消息，拦截游戏界面的键盘鼠标及滚轮输入。需挂钩 `WndProc` 并传入。
```cpp
LRESULT Shadow::Input(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
```

### Shadow::ProcessGlobalHotkeys
处理全局热键输入，即使 GUI 菜单关闭时，也能够响应并更新已注册热键的状态。
```cpp
bool Shadow::ProcessGlobalHotkeys(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
```

### Shadow::UpdateAllHotkeyStates
每帧更新所有已注册热键的工作模式和逻辑激活状态（例如计算切换、长按等模式的结果）。
```cpp
void Shadow::UpdateAllHotkeyStates();
```

### Shadow::SetAllowedKeys
设置允许透传给游戏后台的白名单按键列表（比如用来防止打开菜单时不能控制角色移动）。
```cpp
void Shadow::SetAllowedKeys(const std::vector<int>& keys);
```

### Shadow::SetAllowedMouseMsgs
设置允许透传给游戏后台的白名单鼠标消息列表（如视角控制所需的消息）。
```cpp
void Shadow::SetAllowedMouseMsgs(const std::vector<UINT>& msgs);
```

### Shadow::IsKeyAllowed
检查指定虚拟按键码是否在允许透传的白名单列表中。
```cpp
bool Shadow::IsKeyAllowed(int key);
```

### Shadow::IsMouseMsgAllowed
检查指定的鼠标窗口消息是否在允许透传的白名单列表中。
```cpp
bool Shadow::IsMouseMsgAllowed(UINT uMsg);
```

### Shadow::RegisterHotkey
手动在后端注册一个全局热键及触发模式。
```cpp
void Shadow::RegisterHotkey(int* hotkey, HotkeyMode* mode, bool* is_active);
```

### Shadow::IsHotkeyRegistered
判断指定的虚拟按键是否已经被注册成了有效热键。
```cpp
bool Shadow::IsHotkeyRegistered(int vk);
```

---

### 📌 窗口属性与外观主题

### Shadow::GetStyle
获取全局 GUI 样式实例。可以通过修改该对象实时调节颜色、控件间距、边距及各项尺寸。
```cpp
GuiStyle& Shadow::GetStyle();
```

### Shadow::StyleColorsOcean
应用“深海（Ocean）”蓝色调 GUI 主题配色。
```cpp
void Shadow::StyleColorsOcean();
```

### Shadow::StyleColorsAmethyst
应用“紫曜（Amethyst）”紫色调 GUI 主题配色。
```cpp
void Shadow::StyleColorsAmethyst();
```

### Shadow::GetWindowSize
获取当前操作窗口的宽高尺寸。
```cpp
Vec2 Shadow::GetWindowSize();
```

### Shadow::GetWindowPos
获取当前操作窗口左上角在屏幕上的坐标。
```cpp
Vec2 Shadow::GetWindowPos();
```

### Shadow::SetWindowPos
手动覆盖并设置当前窗口的起始绘制位置。
```cpp
void Shadow::SetWindowPos(Vec2 pos);
```

### Shadow::SetNextWindowSizeConstraints
对下一个将要渲染的窗口，限制其用户可自由拖放缩放的最小及最大尺寸。
```cpp
void Shadow::SetNextWindowSizeConstraints(Vec2 min_size, Vec2 max_size);
```

---

### 📌 字体与文本处理

### Shadow::PushFont
将指定的 `UFont` 与缩放比例压入字体栈中，后续所渲染的文本（包括组件自带）将使用该字体。
```cpp
void Shadow::PushFont(SDK::UFont* font, float scale = 1.0f);
```

### Shadow::PopFont
弹出并恢复到上一个（或默认）字体上下文。
```cpp
void Shadow::PopFont();
```

### Shadow::MeasureTextSize
测量指定文本字符串在当前字体与 DPI 缩放配置下的像素宽高。
```cpp
Vec2 Shadow::MeasureTextSize(std::string_view text);
```

### Shadow::MeasureTextHeight
测量指定文本字符串在当前字体与 DPI 缩放配置下的高度。
```cpp
float Shadow::MeasureTextHeight(std::string_view text);
```

### Shadow::MeasureCharWidth
测量单个宽字符在当前字体配置下的宽度。
```cpp
float Shadow::MeasureCharWidth(wchar_t ch);
```

---

### 📌 底层绘制与剪裁引擎

### Shadow::DrawTextString
使用绝对坐标，底层调用 Canvas 绘制不加任何额外封装的文本。
```cpp
void Shadow::DrawTextString(std::string_view text, Vec2 pos, Color color);
```

### Shadow::DrawLine
在画布上绘制一条指定颜色和粗细的半透明图层直线。
```cpp
void Shadow::DrawLine(Vec2 start, Vec2 end, Color color, float thickness = 1.0f);
```

### Shadow::DrawRect
绘制一个具有指定颜色线条边界的空心矩形框。
```cpp
void Shadow::DrawRect(Vec2 pos, Vec2 size, Color color);
```

### Shadow::DrawRectFilled
绘制一个使用指定颜色填充内部的实心矩形块。
```cpp
void Shadow::DrawRectFilled(Vec2 pos, Vec2 size, Color color);
```

### Shadow::PushClipRect
推入自定义显示裁剪矩形边界，超出此边界的内容将不会被渲染（主要用于滚动列表或 TabBar 视图区域控制）。
```cpp
void Shadow::PushClipRect(Vec2 min, Vec2 max);
```

### Shadow::PopClipRect
弹出并销毁最近的一层自定义显示裁剪边界。
```cpp
void Shadow::PopClipRect();
```

### Shadow::IsRectVisible
判断当前指定的矩形坐标范围是否在渲染边界内（只要有一部分可见即返回 `true`）。
```cpp
bool Shadow::IsRectVisible(Vec2 pos, Vec2 size);
```

### Shadow::IsRectFullyVisible
判断当前指定的矩形坐标范围是否完全存在于渲染剪裁边界之内。
```cpp
bool Shadow::IsRectFullyVisible(Vec2 pos, Vec2 size);
```

---

### 📌 系统功能及辅助工具

### Shadow::SetClipboardText
将指定的文本字符串写入 Windows 系统剪贴板中。
```cpp
void Shadow::SetClipboardText(const std::string& text);
```

### Shadow::GetClipboardText
从 Windows 系统剪贴板中提取并返回当中文本。
```cpp
std::string Shadow::GetClipboardText();
```

### Shadow::GetKeyName
将 Windows 虚拟键码（VK Code）翻译转换为如 "Mouse 1"、"Ctrl"、"Enter" 这样的人类可读字符串名。
```cpp
std::string Shadow::GetKeyName(int vk);
```

### Shadow::HashString
将字符串转换为唯一哈希值以做内部控件的唯一 ID 注册。
```cpp
constexpr size_t Shadow::HashString(std::string_view str) noexcept;
```

### Shadow::ToWString
提供将 UTF-8 多字节字符串安全转换为 Windows 宽字符集 (`std::wstring`) 格式的实用方法。
```cpp
std::wstring Shadow::ToWString(std::string_view utf8_str);
```

### Shadow::RGBtoHSV
将红绿蓝（RGB）色彩空间值转换为色相/饱和度/明度（HSV）值。
```cpp
void Shadow::RGBtoHSV(float r, float g, float b, float& h, float& s, float& v);
```

### Shadow::HSVtoRGB
将色相/饱和度/明度（HSV）值转换为红绿蓝（RGB）色彩空间值。
```cpp
void Shadow::HSVtoRGB(float h, float s, float v, float& r, float& g, float& b);
```

---

## 数据结构与枚举

### 📌 热键模式 (HotkeyMode)
用于 `Shadow::HotKey` (高级版)，定义热键的触发逻辑。
```cpp
enum class HotkeyMode {
    None,       // 无模式（默认不激活）
    HoldOn,     // 按住时激活，松开后失效
    ToggleOn,   // 切换模式（按一次激活，再按一次关闭）
    HoldOff,    // 松开时激活（不按的时候为激活状态，按下后关闭）
    AlwaysOn    // 永远保持激活状态，无视按键
};
```

---

### 📌 窗口标志位 (ShadowWindowFlags_)
用于 `Shadow::Begin`，控制窗口的行为与标题栏对齐方式。支持通过按位或 `|` 组合使用。
```cpp
enum ShadowWindowFlags_ {
    ShadowWindowFlags_None = 0,                 // 默认，无特殊标志
    ShadowWindowFlags_NoResize = 1 << 0,        // 禁止通过右下角手柄缩放窗口
    ShadowWindowFlags_NoMove = 1 << 1,          // 禁止通过拖拽标题栏与背景移动窗口
    ShadowWindowFlags_NoScrollbar = 1 << 2,     // 隐藏窗口垂直滚动条（但仍可通过滚轮滚动）
    
    ShadowWindowFlags_TextAlignLeft = 0,        // 标题栏文本左对齐（默认）
    ShadowWindowFlags_TextAlignCenter = 1 << 3, // 标题栏文本居中对齐
    ShadowWindowFlags_TextAlignRight = 1 << 4,  // 标题栏文本右对齐
};
```

---

### 📌 标签栏标志位 (ShadowTabBarFlags_)
用于 `Shadow::BeginTabBar`，控制标签栏的排版与交互逻辑。支持通过按位或 `|` 组合使用。
```cpp
enum ShadowTabBarFlags_ {
    ShadowTabBarFlags_None = 0,                     // 默认，无特殊标志
    ShadowTabBarFlags_Reorderable = 1 << 0,         // 允许玩家通过鼠标拖拽重新排列标签页(Tab)的顺序
    ShadowTabBarFlags_FittingPolicyScroll = 1 << 1, // 当标签页总宽度超出窗口边界时，开启水平滚动策略
    ShadowTabBarFlags_NoScrollbar = 1 << 2,         // 隐藏标签栏水平滚动条（但仍可通过滚轮滚动）
};
```

---

### 📌 颜色枚举 (GuiCol_)
用于 `g_Ctx.Style.Colors` 数组索引，代表 GUI 库中所有的可配色的元素。开发者可以通过替换这些索引处的颜色值来自定义主题。
```cpp
enum GuiCol_ {
    // 基础背景
    GuiCol_WindowBg,             // 窗口主背景色
    GuiCol_TitleBarBg,           // 窗口标题栏背景色
    GuiCol_PopupBg,              // 弹出层(下拉框、颜色选择器)背景色
    GuiCol_PopupBorder,          // 弹出层边框色
    GuiCol_FrameBg,              // 控件(输入框、滑动条背景等)基础背景色
    GuiCol_FrameBgHovered,       // 控件鼠标悬停时的背景色

    // 文本颜色
    GuiCol_Text,                 // 基础文本颜色
    GuiCol_TextDisabled,         // 禁用状态或提示文本颜色
    GuiCol_TextHighlight,        // 高亮文本颜色(如当前选中的Tab或下拉项)
    GuiCol_ErrorText,            // 报错信息文本颜色
    GuiCol_TextShadow,           // 文本阴影颜色 (底层引擎使用)
    GuiCol_TextOutline,          // 文本描边颜色 (底层引擎使用)

    // 交互控件
    GuiCol_Button,               // 按钮基础颜色
    GuiCol_ButtonHovered,        // 按钮鼠标悬停颜色
    GuiCol_SliderGrab,           // 滑动条拖拽滑块/选中填充颜色
    GuiCol_CheckMark,            // 复选框打勾标识的颜色
    GuiCol_ActiveIndicator,      // 热键状态开启时的指示灯颜色
    GuiCol_InactiveIndicator,    // 热键状态关闭时的指示灯颜色
    GuiCol_ControlDisabled,      // 被禁用控件的覆盖背景色

    // 边界与手柄
    GuiCol_Separator,            // 分割线/标签栏底线颜色
    GuiCol_Border,               // 基础边框线颜色
    GuiCol_ResizeGrip,           // 右下角缩放手柄默认颜色
    GuiCol_ResizeGripHovered,    // 缩放手柄鼠标悬停颜色
    GuiCol_ResizeGripActive,     // 缩放手柄鼠标按下拖拽时的颜色

    // 标签页 (Tab)
    GuiCol_Tab,                  // 未选中的标签页颜色
    GuiCol_TabHovered,           // 未选中但鼠标悬停的标签页颜色
    GuiCol_TabActive,            // 当前选中的标签页颜色

    // 颜色选择器 (ColorPicker) 专属
    GuiCol_ColorPickerDark,      // 颜色选择器内部深色游标/边框
    GuiCol_ColorPickerLight,     // 颜色选择器内部浅色游标/边框(及全局Alpha参考)
    GuiCol_CheckerboardLight,    // 颜色选择器透明度棋盘格-浅色块
    GuiCol_CheckerboardDark,     // 颜色选择器透明度棋盘格-深色块
    GuiCol_ColorPickerShadow,    // 颜色选择器明度(V)渐变遮罩色

    GuiCol_COUNT                 // 枚举最大值（用于数组大小分配）
};
```

---

### 📌 全局样式配置 (GuiStyle)
存放所有关于界面尺寸、间距及颜色的设定。可以通过 `Shadow::GetStyle()` 获取它的引用并动态修改。
```cpp
struct GuiStyle {
    Color Colors[GuiCol_COUNT];          // 所有界面元素的颜色数组

    // 尺寸与间距标志配置
    Vec2 WindowPadding = { 16.f, 16.f }; // 窗口内部四周的留白边距 (X=水平, Y=垂直)
    Vec2 FramePadding = { 8.f, 2.f };    // 控件内部边缘与文本内容的间距 (X为水平边距, Y为垂直居中补偿)
    Vec2 ItemSpacing = { 10.f, 8.f };    // 相邻控件之间的水平与垂直间距
    float ScrollbarSize = 10.f;          // 滚动条的宽度
    float ScrollbarMargin = 4.f;         // 滚动条与右侧边缘/其他元素的间距
    float ResizeGripSize = 16.f;         // 右下角缩放窗口手柄的尺寸
    float TabExtraWidth = 20.f;          // Tab标签在文字宽度之外额外预留的宽度

    // 控件特定对齐配置 (右侧对齐布局用)
    float ControlOffsetMin = 120.f;      // 右侧互动控件(Slider, Hotkey等)对齐的最小左侧偏移绝对值
    float ControlOffsetRatio = 0.4f;     // 右侧互动控件偏移量占整体窗口宽度的比例

    // ColorPicker 弹出层专属尺寸配置
    float CPPadding = 8.f;               // 颜色选择器面板的内边距
    float CPSVSize = 200.f;              // 色彩(SV)方形选取面板的尺寸(宽高相同)
    float CPHueWidth = 20.f;             // 色相(Hue)竖条的宽度
    float CPAlphaWidth = 20.f;           // 透明度(Alpha)竖条的宽度
    float CPSpacing = 8.f;               // 面板内各个区块之间的间距

    // 窗口与全局缩放
    Vec2 WindowMinSize = { 200.f, 150.f };// 窗口允许被缩放至的最小尺寸限制
    float FontScaleDpi = 1.0f;           // 默认 1.0，全局控制文本与菜单自适应渲染缩放比
};
```
