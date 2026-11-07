#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>

#include "external/MinHook/include/MinHook.h"
#include "src/Shadow.h"
#include "src/Backend_DX11.h"
#include "src/Shadow_Demo.h"
#include "misc/fonts/CangErShuYuanTi_W03.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

namespace DX11Hook {
    // 菜单开闭与快捷键配置
    constexpr int TOGGLE_MENU_KEY = VK_F1;
    constexpr float MENU_FADE_SPEED = 5.0f;
    constexpr float MIN_VISIBLE_ALPHA = 0.001f;
    constexpr float FONT_SIZE_DEFAULT = 20.0f;

    bool g_ShowMenu = true;
    float g_CurrentAlpha = 0.0f;
    HWND g_Hwnd = nullptr;
    WNDPROC g_OriginalWndProc = nullptr;

    ID3D11Device* g_Device = nullptr;
    ID3D11DeviceContext* g_Context = nullptr;
    ID3D11RenderTargetView* g_RenderTargetView = nullptr;

    typedef HRESULT(__fastcall* tPresent)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    tPresent oPresent = nullptr;

    typedef HRESULT(__fastcall* tResizeBuffers)(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    tResizeBuffers oResizeBuffers = nullptr;

    void CreateRenderTarget(IDXGISwapChain* pSwapChain) {
        ID3D11Texture2D* pBackBuffer = nullptr;
        if (SUCCEEDED(pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)))) {
            g_Device->CreateRenderTargetView(pBackBuffer, nullptr, &g_RenderTargetView);
            pBackBuffer->Release();
        }
    }

    void CleanupRenderTarget() {
        if (g_RenderTargetView) {
            g_RenderTargetView->Release();
            g_RenderTargetView = nullptr;
        }
    }

    LRESULT APIENTRY WndProcHook(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        Shadow::ProcessGlobalHotkeys(hwnd, uMsg, wParam, lParam);

        if (uMsg == WM_KEYDOWN && wParam == TOGGLE_MENU_KEY && !Shadow::g_Ctx.AssigningHotkey) {
            bool isFirstPress = ((lParam & (1 << 30)) == 0);
            if (isFirstPress) {
                g_ShowMenu = !g_ShowMenu;
            }
        }

        if (g_ShowMenu || g_CurrentAlpha > MIN_VISIBLE_ALPHA) {
            if (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP || uMsg == WM_CHAR) {
                if (wParam == TOGGLE_MENU_KEY) {
                    return CallWindowProc(g_OriginalWndProc, hwnd, uMsg, wParam, lParam);
                }
                if (Shadow::IsHotkeyRegistered(static_cast<int>(wParam))) {
                    return CallWindowProc(g_OriginalWndProc, hwnd, uMsg, wParam, lParam);
                }

                Shadow::Input(hwnd, uMsg, wParam, lParam);

                if (Shadow::IsKeyAllowed(static_cast<int>(wParam))) {
                    return CallWindowProc(g_OriginalWndProc, hwnd, uMsg, wParam, lParam);
                }
                return 1;
            }

            if (!Shadow::IsMouseMsgAllowed(uMsg)) {
                Shadow::Input(hwnd, uMsg, wParam, lParam);
                return 1;
            }

            Shadow::Input(hwnd, uMsg, wParam, lParam);
            return CallWindowProc(g_OriginalWndProc, hwnd, uMsg, wParam, lParam);
        }

        Shadow::Input(hwnd, uMsg, wParam, lParam);
        return CallWindowProc(g_OriginalWndProc, hwnd, uMsg, wParam, lParam);
    }

    HRESULT __fastcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
        if (!g_Device) {
            if (FAILED(pSwapChain->GetDevice(IID_PPV_ARGS(&g_Device)))) {
                return oPresent(pSwapChain, SyncInterval, Flags);
            }
            g_Device->GetImmediateContext(&g_Context);

            DXGI_SWAP_CHAIN_DESC desc;
            pSwapChain->GetDesc(&desc);
            g_Hwnd = desc.OutputWindow;

            g_OriginalWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_Hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook)));

            Shadow::DX11_Init(g_Hwnd, g_Device, g_Context);
            CreateRenderTarget(pSwapChain);
        }

        if (!g_RenderTargetView) {
            CreateRenderTarget(pSwapChain);
        }

        if (g_RenderTargetView) {
            g_Context->OMSetRenderTargets(1, &g_RenderTargetView, nullptr);

            Shadow::SetAllowedKeys({ 'W', 'A', 'S', 'D', VK_SPACE });
            Shadow::DX11_NewFrame();
            Shadow::NewFrame();
            Shadow::UpdateAllHotkeyStates();

            float dt = Shadow::GetIO().DeltaTime;
            if (g_ShowMenu) {
                g_CurrentAlpha += MENU_FADE_SPEED * dt;
                if (g_CurrentAlpha > 1.0f) g_CurrentAlpha = 1.0f;
            }
            else {
                g_CurrentAlpha -= MENU_FADE_SPEED * dt;
                if (g_CurrentAlpha < 0.0f) g_CurrentAlpha = 0.0f;
            }

            Shadow::PushStyleVar(Shadow::GuiStyleVar_Alpha, g_CurrentAlpha);

            if (g_CurrentAlpha > MIN_VISIBLE_ALPHA) {
                Shadow::ShadowFont font = Shadow::LoadFontFromBuffer(
                    g_Fonts::CangErShuYuanTi_W03,
                    sizeof(g_Fonts::CangErShuYuanTi_W03),
                    static_cast<int32_t>(FONT_SIZE_DEFAULT)
                );
                Shadow::PushFont(font);
                Shadow::PushTextPixelSnap();

                Shadow::ShowDemoWindow();

                Shadow::PopTextPixelSnap();
                Shadow::PopFont();
            }

            Shadow::PopStyleVar();

            Shadow::Render();
            Shadow::DX11_RenderDrawData(Shadow::GetDrawData());
        }

        return oPresent(pSwapChain, SyncInterval, Flags);
    }

    HRESULT __fastcall hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
        // 在 DXGI 释放旧缓冲区调整大小前，彻底释放 RenderTargetView
        CleanupRenderTarget();

        HRESULT hr = oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

        // 重新创建与新分辨率对齐的 RenderTargetView
        CreateRenderTarget(pSwapChain);

        return hr;
    }

    void HookDX11() {
        // 构造虚假窗口与临时设备，用于稳定抓取 IDXGISwapChain 虚函数表指针
        WNDCLASSEX wc{ sizeof(WNDCLASSEX), CS_CLASSDC, DefWindowProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"TempDX11Window", nullptr };
        RegisterClassEx(&wc);
        HWND hWnd = CreateWindow(wc.lpszClassName, L"", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, nullptr, nullptr, wc.hInstance, nullptr);

        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

        DXGI_SWAP_CHAIN_DESC scd{};
        scd.BufferCount = 1;
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.OutputWindow = hWnd;
        scd.SampleDesc.Count = 1;
        scd.Windowed = TRUE;

        IDXGISwapChain* pDummySwapChain = nullptr;
        ID3D11Device* pDummyDevice = nullptr;
        ID3D11DeviceContext* pDummyContext = nullptr;

        if (SUCCEEDED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevels, 2, D3D11_SDK_VERSION, &scd, &pDummySwapChain, &pDummyDevice, &featureLevel, &pDummyContext))) {
            void** pVMT = *reinterpret_cast<void***>(pDummySwapChain);

            void* targetPresent = pVMT[8];       // IDXGISwapChain::Present
            void* targetResizeBuffers = pVMT[13]; // IDXGISwapChain::ResizeBuffers

            if (MH_CreateHook(targetPresent, reinterpret_cast<void*>(&hkPresent), reinterpret_cast<void**>(&oPresent)) == MH_OK) {
                MH_EnableHook(targetPresent);
            }

            if (MH_CreateHook(targetResizeBuffers, reinterpret_cast<void*>(&hkResizeBuffers), reinterpret_cast<void**>(&oResizeBuffers)) == MH_OK) {
                MH_EnableHook(targetResizeBuffers);
            }

            pDummySwapChain->Release();
            pDummyDevice->Release();
            pDummyContext->Release();
        }

        DestroyWindow(hWnd);
        UnregisterClass(wc.lpszClassName, wc.hInstance);
    }
} // namespace DX11Hook

void InitializationThread(LPVOID lpParam) {
    if (MH_Initialize() != MH_OK) return;
    DX11Hook::HookDX11();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        if (HANDLE hThread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(InitializationThread), hModule, 0, nullptr)) {
            CloseHandle(hThread);
        }
        break;
    case DLL_PROCESS_DETACH:
        if (DX11Hook::g_Hwnd && DX11Hook::g_OriginalWndProc) {
            SetWindowLongPtr(DX11Hook::g_Hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(DX11Hook::g_OriginalWndProc));
        }
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        DX11Hook::CleanupRenderTarget();
        Shadow::DX11_Shutdown();
        if (DX11Hook::g_Context) DX11Hook::g_Context->Release();
        if (DX11Hook::g_Device) DX11Hook::g_Device->Release();
        break;
    }
    return TRUE;
}