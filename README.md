# ✨ Shadow Gui

欢迎大家 PR 各种好看的主题配色和控件样式，或者提交你们的使用示例代码！

## 效果展示 / Showcase

### ShowDemoWindow
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/c4df028e-20f0-43d0-a1f6-d53f3280d3be" />

### Nav UI
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/02977324-df2d-4411-85c8-49345f318120" />

### AddCircleFilled
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/64b51ead-a162-4144-bece-4a8f42fef637" />

---

### ☕ 赞助 / Buy me a coffee

* 您可以通过赞助支持我们的开发，感谢大家的支持！
* If this project helps you, feel free to support my work!
* **USDT (TRC-20):** `THzBDDbBkDh3nXRkCEeG4p5r733tWeAdib`

---

## Credits

* [ocornut/imgui](https://github.com/ocornut/imgui)
* [Encryqed/Dumper-7](https://github.com/Encryqed/Dumper-7)
* [Google AI Studio](https://aistudio.google.com/)

---

## 使用方法 / Usage

从 `v1.9.0` 开始，Shadow Gui 变更为 **后端无关** 的工作模式：
* **UE 原生工程后端 (`Backend_Native.h`)**：直接编译打包进虚幻引擎项目，适用于游戏开发。
* **Dumper-7 后端 (`Backend_Dumper7.h`)**：搭配 `Dumper-7` 生成的 `CppSDK` 使用，适用于 DLL 注入、Mod 开发等场景。

---

### 方案一：UE 原生工程集成 / UE Native Integration

在你的 UE 项目中的 `UGameViewportClient` 子类引入 `Backend_Native.h` 即可直接使用。

#### 完整集成示例 (`ZClientViewport.cpp`)

<details open>
<summary>Show code</summary>

```cpp
#include "ZClientViewport.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "RHI.h"
#include "GameFramework/PlayerController.h"

// 引入 Shadow Gui Native 后端与字体文件
#include "Shadow-Gui/src/Backend_Native.h"
#include "Shadow-Gui/misc/fonts/CangErShuYuanTi_W03.h"

// 常量配置
namespace ViewportConfig {
    static const FKey MENU_TOGGLE_KEY = EKeys::F1;
    constexpr float FPS_UPDATE_INTERVAL = 1.0f;
    constexpr float DELTA_UPDATE_INTERVAL = 0.05f;
    constexpr float STATS_POS_X = 25.0f;
    constexpr float STATS_POS_Y = 25.0f;
    constexpr int32 STATS_FONT_SIZE = 20;
    constexpr float MENU_FONT_SCALE = 1.5f;
}

void UZClientViewport::SetMenuVisible(bool bVisible) {
    bShowMenu = bVisible;
    UpdateMouseCursorState();
}

void UZClientViewport::UpdateMouseCursorState() {
    UWorld* CurrentWorld = GetWorld();
    if (!CurrentWorld) {
        return;
    }

    APlayerController* PC = CurrentWorld->GetFirstPlayerController();
    if (!PC){
        return;
    }

    PC->bShowMouseCursor = bShowMenu;
    PC->bEnableClickEvents = bShowMenu;
    PC->bEnableMouseOverEvents = bShowMenu;

    if (bShowMenu) {
        FInputModeGameAndUI InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        InputMode.SetHideCursorDuringCapture(false);
        PC->SetInputMode(InputMode);
    } 
    else {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
    }
}

void UZClientViewport::PostRender(UCanvas* Canvas) {
    Super::PostRender(Canvas);

    if (!Canvas || !GEngine) {
        return;
    }

    // 1. 获取单帧 DeltaSeconds
    float RealtimeDeltaSeconds = 0.0f;
    if (UWorld* CurrentWorld = GetWorld()) {
        RealtimeDeltaSeconds = CurrentWorld->GetDeltaSeconds();
    }
    else {
        RealtimeDeltaSeconds = FApp::GetDeltaTime();
    }

    // 2. 调度计时
    FPSTimer += RealtimeDeltaSeconds;
    FPSFrames++;
    if (FPSTimer >= ViewportConfig::FPS_UPDATE_INTERVAL) {
        CachedFPS = static_cast<float>(FPSFrames) / FPSTimer;
        FPSTimer = 0.0f;
        FPSFrames = 0;
    }

    DeltaTimer += RealtimeDeltaSeconds;
    if (DeltaTimer >= ViewportConfig::DELTA_UPDATE_INTERVAL) {
        CachedDeltaTime = RealtimeDeltaSeconds * 1000.0f;
        DeltaTimer = 0.0f;
    }

    uint32 RealtimeDrawCalls = GNumDrawCallsRHI[0];

    // 5. 执行 Shadow GUI 驱动与渲染 (传入当前视口以保证光标坐标 100% 精确跟踪)
    Shadow::Native_NewFrame(Canvas, Viewport);
    Shadow::NewFrame();

    // 更新全局热键激活状态
    Shadow::UpdateAllHotkeyStates();

    // 加载自定义字体
    Shadow::ShadowFont font = Shadow::LoadFontFromBuffer(g_Fonts::CangErShuYuanTi_W03, sizeof(g_Fonts::CangErShuYuanTi_W03));

    // 3. 组装性能统计文本
    std::string PerfStatsText = std::format("FPS: {:.1f}\nDeltaTime: {:.3f} ms\nDrawCalls: {}", CachedFPS, CachedDeltaTime, RealtimeDrawCalls);

    // 4. 准备文本绘制项并渲染
    Shadow::GetBackgroundDrawList()->AddText(
        font,
        ViewportConfig::MENU_FONT_SCALE,
        { 0.f, 0.f, 0.f, 0.8f },
        { 0.f, 0.f, 0.f, 0.8f },
        { ViewportConfig::STATS_POS_X, ViewportConfig::STATS_POS_Y },
        { 1.f, 1.f, 1.f, 0.8f },
        PerfStatsText,
        true,
        false
    );

    if (bShowMenu) {
        // 保证光标在菜单打开状态下持续保持呼出
        UpdateMouseCursorState();
        Shadow::PushFont(font, ViewportConfig::MENU_FONT_SCALE);
        Shadow::PushTextPixelSnap();
        RenderShadowUI();
        Shadow::PopTextPixelSnap();
        Shadow::PopFont();
    }

    Shadow::Render();
    Shadow::Native_RenderDrawData(Shadow::GetDrawData());
}

void UZClientViewport::RenderShadowUI() {
    Shadow::ShowDemoWindow();
}

bool UZClientViewport::InputKey(const FInputKeyEventArgs& EventArgs) {
    // 菜单开闭快捷键
    if (EventArgs.Key == ViewportConfig::MENU_TOGGLE_KEY && EventArgs.Event == IE_Pressed) {
        SetMenuVisible(!bShowMenu);
        return true;
    }

    // 调用 Native 后端统一输入处理函数
    if (Shadow::Native_InputKey(EventArgs, bShowMenu)) {
        return true;
    }

    return Super::InputKey(EventArgs);
}

bool UZClientViewport::InputAxis(FViewport* InViewport, FInputDeviceId InputDevice, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad) {
    // 调用 Native 后端统一输入处理函数
    if (Shadow::Native_InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad, bShowMenu)) {
        return true;
    }

    return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
}

bool UZClientViewport::InputChar(FViewport* InViewport, int32 ControllerId, TCHAR Character) {
    // 调用 Native 后端统一输入处理函数
    if (Shadow::Native_InputChar(InViewport, ControllerId, Character, bShowMenu)) {
        return true;
    }

    return Super::InputChar(InViewport, ControllerId, Character);
}
```

</details>

---

### 方案二：DLL 注入集成 / DLL Hook Integration

#### 快速开始 / Quick Start

##### 中文
* `#include "src/Shadow.h"`
* `#include "src/Backend_Dumper7.h"`
* `#include "src/Shadow_Nav.h" // 如果需要 Nav UI`
* `#include "src/Shadow_Demo.h" // 如果需要 Demo Menu`
* 确保包含 Dumper-7 生成的 `CppSDK` 并配置好相关头文件包含路径
* 在 Hook 的 `PostRender` 中调用 `Shadow::Dumper7_NewFrame(Canvas);` 与 `Shadow::NewFrame();`
* 绘制菜单后调用 `Shadow::Render();` 与 `Shadow::Dumper7_RenderDrawData(Shadow::GetDrawData());`

##### English
* `#include "src/Shadow.h"`
* `#include "src/Backend_Dumper7.h"`
* `#include "src/Shadow_Nav.h" // if you need Nav UI`
* `#include "src/Shadow_Demo.h" // if you need Demo Menu`
* Make sure the Dumper-7 generated `CppSDK` is included in your include path
* Call `Shadow::Dumper7_NewFrame(Canvas);` and `Shadow::NewFrame();` in your hooked `PostRender`
* After drawing widgets, call `Shadow::Render();` and `Shadow::Dumper7_RenderDrawData(Shadow::GetDrawData());`

---

#### 绑定热键 / Bind Hotkeys

<details>
<summary>Show code</summary>

```cpp
// 初始化 Shadow GUI 的时候需要先 RegisterHotkey，否则在上下文执行到 Shadow::HotKey 之前无法使用对应热键
Shadow::RegisterHotkey(&g_Config::kTestKey, &g_Config::eTestKey, &g_Config::bTestKey);

// 与 RegisterHotkey 保持一致，正常声明即可
Shadow::HotKey("TestKey", &g_Config::kTestKey, &g_Config::bTestKey, &g_Config::eTestKey);
```

</details>

---

#### 输入处理 / Input Processing (WndProc Hook)

<details>
<summary>Show code</summary>

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

</details>

---

#### 渲染挂钩 / Render Hook (hkPostRender)

<details>
<summary>Show code</summary>

```cpp
void __fastcall hkPostRender(SDK::UGameViewportClient* rcx, SDK::UCanvas* canvas) {
    // 放行常用移动按键给游戏，避免菜单打开时玩家动不了
    Shadow::SetAllowedKeys({ 'W', 'A', 'S', 'D', VK_SPACE });

    // 初始化后端帧状态
    Shadow::Dumper7_NewFrame(canvas);
    Shadow::NewFrame();

    // 更新所有热键状态
    Shadow::UpdateAllHotkeyStates();

    if (Shadow::Begin("Main Menu##main_window", Shadow::ShadowWindowFlags_NoResize)) {
        if (Shadow::BeginTabBar("MainTabs##tabs", Shadow::ShadowTabBarFlags_Reorderable)) {
            if (Shadow::BeginTabItem("Misc##tab0")) {
                Shadow::TextColored({ 0.0f, 1.0f, 0.0f, 1.0f }, "你好！");
            }
            Shadow::EndTabItem();
        }
        Shadow::EndTabBar();
    }
    Shadow::End();

    // 生成绘制命令并提交给 Dumper-7 后端渲染
    Shadow::Render();
    Shadow::Dumper7_RenderDrawData(Shadow::GetDrawData());
}
```

</details>

---

### 注意事项 / Notes

> [!IMPORTANT]
> 与 Dear ImGui 不同，Shadow GUI 的所有压栈操作都必须在作用域之外出栈。

示例：
<details>
<summary>Show code</summary>

```cpp
// -> BeginTabBar
if (Shadow::BeginTabBar("TEST_BeginTabBar")) {
    // -> BeginTabItem
    if (Shadow::BeginTabItem("TEST_BeginTabItem")) {
        // -> TreeNode
        if (Shadow::TreeNode("TEST_TreeNode")) {

        }
        // -> TreePop
        Shadow::TreePop();
    }
    // -> EndTabItem
    Shadow::EndTabItem();
}
// -> EndTabBar
Shadow::EndTabBar();

```

</details>

---

> [!IMPORTANT]
> LoadTextureFromBuffer、LoadTextureFromFile、LoadFontFromBuffer、LoadFontFromFile 必须每帧调用

示例：
<details>
<summary>Show code</summary>

```cpp
void DrawLogoFromBuffer()
    {
        // 不要对 LoadTextureFromBuffer / LoadTextureFromFile 返回的指针进行跨帧保存。因为引擎GC会导致旧的指针得到一个损坏的UObject从而导致崩溃。
        // 每帧调用函数更新该指针即可，库内部会自动管理UObject的生命周期。
        Shadow::ShadowTexture Texture = Shadow::LoadTextureFromBuffer(Example_Texture::Logo, sizeof(Example_Texture::Logo));

        if (Texture)
        {
            Shadow::GetBackgroundDrawList()->AddTexture({ 100.f, 100.f }, { 480.f, 173.f }, { 1.f, 1.f, 1.f, 1.f }, Texture);
        }
    }

    void DrawLogoFromFile()
    {
        Shadow::ShadowTexture Texture = Shadow::LoadTextureFromFile(L"C:\\Example_Texture.png");

        if (Texture)
        {
            Shadow::GetBackgroundDrawList()->AddTexture({ 100.f, 100.f }, { 480.f, 173.f }, { 1.f, 1.f, 1.f, 1.f }, Texture);
        }
    }

    void DrawCustomText()
    {
        // 内存
        Shadow::ShadowFont Icon = Shadow::LoadFontFromBuffer(Example_Font::Icon, sizeof(Example_Font::Icon));

        // 文件
        Shadow::ShadowFont msyh = Shadow::LoadFontFromFile(L"C:\\Windows\\Fonts\\msyh.ttc");
		Shadow::ShadowFont tahoma_bold = Shadow::LoadFontFromFile(L"C:\\Windows\\Fonts\\tahomabd.ttf");
		Shadow::ShadowFont verdana_bold = Shadow::LoadFontFromFile(L"C:\\Windows\\Fonts\\verdanab.ttf");
    }
```

</details>

---

### 示例 / Example

#### 画一个简单的窗口 / Draw simple window

<details>
<summary>Show code</summary>

```cpp
if (Shadow::Begin(U8("测试菜单 / Demo Menu##main_window"), Shadow::ShadowWindowFlags_TextAlignCenter)) {

    if (Shadow::BeginTabBar("MainTabs##tabs", Shadow::ShadowTabBarFlags_NoScrollbar)) {

        if (Shadow::BeginTabItem(U8("设置 / Settings##tab0"))) {

            Shadow::Switch(U8("测试开关 / Test Switch"), &bTest); 
            Shadow::SameLine(); 
            Shadow::ColorPicker(U8("测试开关 / Test Switch"), 
                &cTest.r, &cTest.g, &cTest.b, &cTest.a, 
                Shadow::ShadowColorPickerFlags_NoText | Shadow::ShadowColorPickerFlags_NoRightAlign
            );

        }
        Shadow::EndTabItem();

    }
    Shadow::EndTabBar();
}
Shadow::End();
```

</details>

<img width="576" height="433" alt="1" src="https://github.com/user-attachments/assets/29bbf19e-d53d-41b3-8b64-985293be887c" />

---

#### 画一个列表窗口 / Draw listbox

<details>
<summary>Show code</summary>

```cpp
if (Shadow::Begin(U8("测试菜单 / Demo Menu##main_window"), Shadow::ShadowWindowFlags_TextAlignCenter)) {

    if (Shadow::BeginTabBar("MainTabs##tabs", Shadow::ShadowTabBarFlags_NoScrollbar)) {

        if (Shadow::BeginTabItem(U8("设置 / Settings##tab0"))) {
            static std::string input_buffer = U8("");      
            static std::vector<std::string> item_list = { 
                U8("默认项目 1"),
                U8("默认项目 2")
            };
            static int selected_index = -1;
            static std::string input_str = U8("输入要新建的项目名称...");

            Shadow::InputTextWithHint(U8("##ItemInput"), input_str, input_buffer, Shadow::ShadowInputTextFlags_NoName, { Shadow::MeasureTextSize(input_str).x + Shadow::g_Ctx.Style.WindowPadding.x });

            if (Shadow::Button(U8("创建"))) {
                if (!input_buffer.empty()) {
                    item_list.push_back(input_buffer); 
                    input_buffer.clear();              
                }
            }

            Shadow::SameLine();
            if (Shadow::Button(U8("删除"))) {
                if (selected_index >= 0 && selected_index < static_cast<int>(item_list.size())) {
                    item_list.erase(item_list.begin() + selected_index);

                    selected_index = -1;
                }
            }

            if (Shadow::BeginListBox(U8("##DynamicItemListBox"), { 300.f, 180.f })) {
                for (int i = 0; i < static_cast<int>(item_list.size()); ++i) {
                    bool is_selected = (selected_index == i);

                    if (Shadow::Selectable(item_list[i], &is_selected)) {
                        selected_index = i;
                    }
                }
            }
            Shadow::EndListBox();

            if (selected_index >= 0 && selected_index < static_cast<int>(item_list.size())) {
                Shadow::TextColored({ 0.4f, 0.8f, 1.0f, 1.0f }, U8("当前选中: ") + item_list[selected_index]);
            }
            else {
                Shadow::TextDisabled(U8("当前未选中任何项目"));
            }
        }
        Shadow::EndTabItem();

    }
    Shadow::EndTabBar();
}
Shadow::End();
```

</details>

<img width="501" height="400" alt="1" src="https://github.com/user-attachments/assets/83729fff-26a0-4f24-ab87-2c4d1462e0bd" />

---

#### 加载自定义字体 / Load Custom Font

<details>
<summary>Show code</summary>

```cpp
// 内存
Shadow::ShadowFont Icon = Shadow::LoadFontFromBuffer(Example_Font::Icon, sizeof(Example_Font::Icon));

// 文件
Shadow::ShadowFont msyh = Shadow::LoadFontFromFile(L"C:\\Windows\\Fonts\\msyh.ttc");

Shadow::ShadowFont tahoma_bold = Shadow::LoadFontFromFile(L"C:\\Windows\\Fonts\\tahomabd.ttf");

Shadow::ShadowFont verdana_bold = Shadow::LoadFontFromFile(L"C:\\Windows\\Fonts\\verdanab.ttf");

if (Icon)
{
    Shadow::Color Color = { 1.f, 1.f, 1.f, 0.75f };

    Shadow::PushFont(Icon, 2.f);
    Shadow::GetBackgroundDrawList()->AddText({ 0.f, 50.f }, Color, "A");
    Shadow::GetBackgroundDrawList()->AddText({ 0.f, 100.f }, Color, "B");
    Shadow::GetBackgroundDrawList()->AddText({ 0.f, 150.f }, Color, "C");
    Shadow::GetBackgroundDrawList()->AddText({ 0.f, 200.f }, Color, "D");
    Shadow::GetBackgroundDrawList()->AddText({ 0.f, 250.f }, Color, "E");

    Shadow::PushTextOutline(Shadow::Color(0.f, 0.f, 0.f, 0.75f));
    Shadow::GetBackgroundDrawList()->AddText({ 50.f, 50.f }, Color, "A");
    Shadow::GetBackgroundDrawList()->AddText({ 50.f, 100.f }, Color, "B");
    Shadow::GetBackgroundDrawList()->AddText({ 50.f, 150.f }, Color, "C");
    Shadow::GetBackgroundDrawList()->AddText({ 50.f, 200.f }, Color, "D");
    Shadow::GetBackgroundDrawList()->AddText({ 50.f, 250.f }, Color, "E");
    Shadow::PopTextOutline();
    Shadow::PopFont();
}

if (msyh)
{
    Shadow::Color Color = { 1.f, 1.f, 1.f, 1.f };
    Shadow::PushFont(msyh, 2.f);
    Shadow::PushTextOutline(Shadow::Color(0.f, 0.f, 0.f, 1.f));
    Shadow::GetBackgroundDrawList()->AddText({ 0.f, 300.f }, Color, "This is a test message using the msyh.ttc font.");
    Shadow::PopTextOutline();
    Shadow::PopFont();
}

if (tahoma_bold)
{
	Shadow::Color Color = { 1.f, 1.f, 0.f, 1.f };
	Shadow::PushFont(tahoma_bold, 2.f);
	Shadow::PushTextOutline(Shadow::Color(0.f, 0.f, 0.f, 1.f));
	Shadow::GetBackgroundDrawList()->AddText({ 0.f, 350.f }, Color, "This is a test message using the tahomabd.ttf font.");
	Shadow::PopTextOutline();
	Shadow::PopFont();
}

if (verdana_bold)
{
	Shadow::Color Color = { 0.f, 1.f, 1.f, 1.f };
	Shadow::PushFont(verdana_bold, 2.f);
	Shadow::PushTextOutline(Shadow::Color(0.f, 0.f, 0.f, 1.f));
	Shadow::GetBackgroundDrawList()->AddText({ 0.f, 400.f }, Color, "This is a test message using the verdanab.ttf font.");
	Shadow::PopTextOutline();
	Shadow::PopFont();
}
```

</details>

<img width="696" height="423" alt="image" src="https://github.com/user-attachments/assets/f209c716-07bf-4266-992d-0101b472b5d6" />

---