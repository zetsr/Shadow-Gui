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

namespace Example_Nav {
    void DrawGUI() {
    // 默认尺寸 { 480.f, 760.f }
        if (Shadow::Nav::Begin("Shadow Menu", "v1.6.1", { 100.f, 100.f }, { 480.f, 760.f })) {
            if (Shadow::Nav::BeginTabBar("MainTabs")) {

                // --- TAB 1: Local (包含状态显示和 Disabled 控件演示) ---
                if (Shadow::Nav::BeginTabItem("Local")) {
                    static bool bGodMode = false;
                    static bool bNeverWanted = true;
                    static float fRunSpeed = 1.0f;
                    static int keyTeleport = VK_F4;
                    static bool bTeleportActive = false;
                    static Shadow::HotkeyMode hkTeleportMode = Shadow::HotkeyMode::ToggleOn;
                    static int healClickCount = 0;

                    // 1. Checkbox 及状态
                    Shadow::Nav::Checkbox("God Mode", &bGodMode);
                    Shadow::Nav::TextColored(bGodMode ? Shadow::Color{ 0.2f, 0.9f, 0.2f, 1.f } : Shadow::Color{ 0.9f, 0.2f, 0.2f, 1.f },
                        std::format("[Status] God Mode: {}", bGodMode ? "ENABLED" : "DISABLED"));

                    Shadow::Nav::Checkbox("Never Wanted", &bNeverWanted);
                    Shadow::Nav::TextColored(bNeverWanted ? Shadow::Color{ 0.2f, 0.9f, 0.2f, 1.f } : Shadow::Color{ 0.9f, 0.2f, 0.2f, 1.f },
                        std::format("[Status] Never Wanted: {}", bNeverWanted ? "ENABLED" : "DISABLED"));

                    // 2. Slider
                    Shadow::Nav::Slider("Run Speed Multiplier", &fRunSpeed, 1.0f, 10.0f, 0.5f);

                    // 3. Hotkey 及状态
                    Shadow::Nav::HotKey("Teleport Key", &keyTeleport, &bTeleportActive, &hkTeleportMode);
                    Shadow::Nav::TextColored(bTeleportActive ? Shadow::Color{ 0.2f, 0.9f, 0.2f, 1.f } : Shadow::Color{ 0.7f, 0.7f, 0.7f, 1.f },
                        std::format("[Status] Teleport Hotkey: {}", bTeleportActive ? "ACTIVE" : "INACTIVE"));

                    // 4. Button 及状态
                    if (Shadow::Nav::Button("Heal Player")) {
                        healClickCount++;
                    }
                    Shadow::Nav::Text(std::format("[Status] Heal Player Click Count: {}", healClickCount));

                    // 5. BeginDisabled 演示（禁用状态，光标会自动跨过它们）
                    Shadow::BeginDisabled(true);
                    static bool bDisabledOption = false;
                    static float fDisabledVal = 50.0f;
                    Shadow::Nav::Checkbox("Disabled Locked Option", &bDisabledOption);
                    Shadow::Nav::Slider("Disabled Locked Slider", &fDisabledVal, 0.f, 100.f);
                    Shadow::Nav::Button("Disabled Locked Button");
                    Shadow::EndDisabled();

                    Shadow::Nav::EndTabItem();
                }

                // --- TAB 2: Online ---
                if (Shadow::Nav::BeginTabItem("Online")) {
                    static float fPlayerESPColor[4] = { 1.0f, 0.2f, 0.2f, 1.0f };
                    static bool bESPBoxes = true;
                    static float fMoneyDropDelay = 5.0f;

                    Shadow::Nav::Checkbox("Player ESP Boxes", &bESPBoxes);
                    Shadow::Nav::ColorPicker("ESP Box Color", &fPlayerESPColor[0], &fPlayerESPColor[1], &fPlayerESPColor[2], &fPlayerESPColor[3]);
                    Shadow::Nav::Slider("Money Drop Delay (s)", &fMoneyDropDelay, 0.1f, 10.0f, 0.1f);
                    Shadow::Nav::Button("Give All Weapons");
                    Shadow::Nav::Button("Teleport To Waypoint");

                    Shadow::Nav::EndTabItem();
                }

                // --- TAB 3: Peds (包含 10 个非 Text 占位控件、30个占位 Text 以及三层嵌套 TreeNode) ---
                if (Shadow::Nav::BeginTabItem("Peds")) {
                    static bool bRandomPeds = false;
                    static float pType = 1.0f;
                    static float pWeapon = 2.0f;
                    static float pedTintColor[3] = { 0.2f, 0.8f, 0.4f };

                    Shadow::Nav::Checkbox("Randomize Peds", &bRandomPeds);
                    Shadow::Nav::Slider("Ped Type", &pType, 0.f, 10.f, 1.0f);
                    Shadow::Nav::Slider("Ped Weapon", &pWeapon, 0.f, 20.f, 1.0f);
                    Shadow::Nav::ColorPicker("Ped Tint Color", &pedTintColor[0], &pedTintColor[1], &pedTintColor[2]);

                    // 10 个可交互的非 Text 占位符控件
                    static bool bPedGodMode = false;
                    static bool bPedNoRagdoll = true;
                    static bool bPedFreeze = false;
                    static float fPedScale = 1.0f;
                    static float fPedHealth = 200.0f;
                    static float fPedArmor = 50.0f;
                    static int kPedHotkey = VK_F5;
                    static bool bPedHotActive = false;
                    static Shadow::HotkeyMode hkPedHotMode = Shadow::HotkeyMode::HoldOn;
                    static float pedGlowColor[4] = { 0.1f, 0.5f, 1.0f, 0.8f };

                    Shadow::Nav::Checkbox("Ped God Mode", &bPedGodMode);
                    Shadow::Nav::Checkbox("Ped No Ragdoll", &bPedNoRagdoll);
                    Shadow::Nav::Checkbox("Ped Freeze Position", &bPedFreeze);
                    Shadow::Nav::Slider("Ped Scale Multiplier", &fPedScale, 0.1f, 5.0f, 0.1f);
                    Shadow::Nav::Slider("Ped Health Points", &fPedHealth, 100.f, 1000.f, 50.f);
                    Shadow::Nav::Slider("Ped Armor Value", &fPedArmor, 0.f, 100.f, 10.f);
                    Shadow::Nav::HotKey("Ped Action Hotkey", &kPedHotkey, &bPedHotActive, &hkPedHotMode);
                    Shadow::Nav::ColorPicker("Ped Outline Glow Color", &pedGlowColor[0], &pedGlowColor[1], &pedGlowColor[2], &pedGlowColor[3]);

                    if (Shadow::Nav::Button("Explode All Peds")) {}
                    if (Shadow::Nav::Button("Revive All Peds")) {}

                    // === [第一层: 分支 A] Bodyguard Options ===
                    if (Shadow::Nav::TreeNode("Bodyguard Options", "VIP Bodyguards")) {
                        Shadow::Nav::Button("Spawn Bodyguard (Normal)");

                        // --- [第二层: 分支 A1] Male Types ---
                        if (Shadow::Nav::TreeNode("Male Types")) {
                            Shadow::Nav::Button("Male Young");
                            Shadow::Nav::Button("Male Mid");
                            Shadow::Nav::Button("Male Old");

                            // ... [第三层: 分支 A1a] Combat Styles ...
                            if (Shadow::Nav::TreeNode("Combat Styles")) {
                                Shadow::Nav::Button("Aggressive");
                                Shadow::Nav::Button("Defensive");
                                Shadow::Nav::Button("Stealth");
                                Shadow::Nav::TreePop();
                            }

                            // ... [第三层: 分支 A1b] Weapon Loadout ...
                            if (Shadow::Nav::TreeNode("Weapon Loadout")) {
                                Shadow::Nav::Button("Pistol Loadout");
                                Shadow::Nav::Button("Rifle Loadout");
                                Shadow::Nav::Button("Heavy Loadout");
                                Shadow::Nav::TreePop();
                            }

                            Shadow::Nav::TreePop();
                        }

                        // --- [第二层: 分支 A2] Female Types ---
                        if (Shadow::Nav::TreeNode("Female Types")) {
                            Shadow::Nav::Button("Female Young");
                            Shadow::Nav::Button("Female Mid/Old");

                            // ... [第三层: 分支 A2a] Female Outfits ...
                            if (Shadow::Nav::TreeNode("Female Outfits")) {
                                Shadow::Nav::Button("Casual Outfit");
                                Shadow::Nav::Button("Formal Outfit");
                                Shadow::Nav::Button("Tactical Outfit");
                                Shadow::Nav::TreePop();
                            }

                            // ... [第三层: 分支 A2b] Voice Options ...
                            if (Shadow::Nav::TreeNode("Voice Options")) {
                                Shadow::Nav::Button("Voice 1");
                                Shadow::Nav::Button("Voice 2");
                                Shadow::Nav::TreePop();
                            }

                            Shadow::Nav::TreePop();
                        }

                        Shadow::Nav::TreePop();
                    }

                    // === [第一层: 分支 B] Special Ped Spawner ===
                    if (Shadow::Nav::TreeNode("Special Ped Spawner")) {
                        Shadow::Nav::Button("Spawn Gang Member");
                        Shadow::Nav::Button("Spawn Security Guard");

                        // 30 个 Text 占位测试子菜单滚动条
                        for (int j = 1; j <= 30; ++j) {
                            Shadow::Nav::Text(std::format("--- Spawner Item Slot #{:02d} ---", j));
                        }

                        // --- [第二层: 分支 B1] Animals ---
                        if (Shadow::Nav::TreeNode("Animals")) {
                            Shadow::Nav::Button("Dog (Chop)");
                            Shadow::Nav::Button("Cat");
                            Shadow::Nav::TreePop();
                        }

                        Shadow::Nav::TreePop();
                    }

                    // 30 个 Text 占位测试 Peds 主菜单滚动条
                    for (int i = 1; i <= 30; ++i) {
                        Shadow::Nav::Text(std::format("--- Peds Menu Item Slot #{:02d} ---", i));
                    }

                    Shadow::Nav::Button("Kill All Spawned Peds");
                    Shadow::Nav::EndTabItem();
                }

                Shadow::Nav::EndTabBar();
            }
            Shadow::Nav::End();
        }
    }
}