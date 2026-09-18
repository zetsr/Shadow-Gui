# ✨ Shadow Gui

欢迎大家 PR 各种好看的主题配色和控件样式，或者提交你们的使用示例代码！

## Demo Window

### Dark
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/c4df028e-20f0-43d0-a1f6-d53f3280d3be" />

### Ocean
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/5cbe32eb-1bc1-42c7-8a23-6ac04c41d6c3" />

### Nav
<img width="1920" height="1080" alt="9196d38726013468be8b69a51f05c493" src="https://github.com/user-attachments/assets/a8c0b44c-894f-44b1-afc4-c26c7dbf0dfc" />

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

### 快速开始 / Quick Start

#### 中文
* `#include "src/Shadow.h"`
* 修改 `Shadow.h` 的 `#include "../external/CppSDK/SDK.hpp"` 为实际路径
* 在 `UGameViewportClient::PostRender` 运行 `Shadow::NewFrame(Canvas);`
* 在 `Shadow::NewFrame(Canvas);` 之后添加 `Shadow::Render();`

#### English
* `#include "src/Shadow.h"`
* Modify `#include "../external/CppSDK/SDK.hpp"` in `Shadow.h` to the actual path
* Run `Shadow::NewFrame(Canvas);` in `UGameViewportClient::PostRender`
* Run `Shadow::Render();` after `Shadow::NewFrame(Canvas);`

---

### 注意事项 / Notes

> [!IMPORTANT]
> 与 Dear ImGui 不同，Shadow GUI 的所有压栈操作都必须在作用域之外出栈。

示例：
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

> [!IMPORTANT]
> LoadTextureFromBuffer、LoadTextureFromFile 必须每帧调用

示例：
```cpp
void DrawLogoFromBuffer()
    {
        SDK::UTexture2D* Texture = Shadow::LoadTextureFromBuffer(Example_Texture::Logo, sizeof(Example_Texture::Logo));

        if (Texture)
        {
            Shadow::GetBackgroundDrawList()->AddTexture({ 100.f, 100.f }, { 480.f, 173.f }, { 1.f, 1.f, 1.f, 1.f }, Texture);
        }
    }

    void DrawLogoFromFile()
    {
        SDK::UTexture2D* Texture = Shadow::LoadTextureFromFile(L"C:\\Example_Texture.png");

        if (Texture)
        {
            Shadow::GetBackgroundDrawList()->AddTexture({ 100.f, 100.f }, { 480.f, 173.f }, { 1.f, 1.f, 1.f, 1.f }, Texture);
        }
    }
```

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

#### 绑定热键 / Bind Hotkeys

```cpp
// 初始化 Shadow GUI 的时候需要先 RegisterHotkey，否则在上下文执行到 Shadow::HotKey 之前无法使用对应热键
Shadow::RegisterHotkey(&g_Config::kTestKey, &g_Config::eTestKey, &g_Config::bTestKey);

// 与 RegisterHotkey 保持一致，正常声明即可
Shadow::HotKey("TestKey", &g_Config::kTestKey, &g_Config::bTestKey, &g_Config::eTestKey);
```

---

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

---

#### 创建菜单 / Creating Menu

```cpp
void __fastcall hkPostRender(SDK::UGameViewportClient* rcx, SDK::UCanvas* canvas) {

    // 放行常用移动按键给游戏，避免菜单打开时玩家动不了
    // Pass common movement keys through to the game to prevent players from getting stuck when the menu is open
    Shadow::SetAllowedKeys({ 'W', 'A', 'S', 'D', VK_SPACE });

    // 开始新的一帧
    // Begin a new frame
    Shadow::NewFrame(canvas);

    // 如果使用 Hotkey，需要先注册热键，虽然缺少手动注册也能用
    // Shadow::RegisterHotkey(&g_Config::kTestKey, &g_Config::eTestKey, &g_Config::bTestKey);

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

    Shadow::Render();
}

```