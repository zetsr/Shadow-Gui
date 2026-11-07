#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <algorithm>
#include <memory>
#include <cstdint>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "../misc/stb/stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "../misc/stb/stb_truetype.h"

#include "Shadow.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace Shadow {
    // 渲染管线与资源配置常量
    constexpr size_t INITIAL_VERTEX_BUFFER_SIZE = 8192;
    constexpr size_t INITIAL_INDEX_BUFFER_SIZE = 16384;
    constexpr int ATLAS_DEFAULT_WIDTH = 2048;
    constexpr int ATLAS_DEFAULT_HEIGHT = 2048;
    constexpr int ATLAS_PADDING = 1;
    constexpr float MATH_PI = 3.14159265358979323846f;
    constexpr int CIRCLE_NUM_SEGMENTS = 32;
    constexpr int CORNER_ROUNDING_SEGMENTS = 8;
    constexpr float THICKNESS_HALF_RATIO = 0.5f;
    constexpr float TEXTURE_DEFAULT_UV_MIN = 0.0f;
    constexpr float TEXTURE_DEFAULT_UV_MAX = 1.0f;
    constexpr int RGBA_BYTES_PER_PIXEL = 4;
    constexpr unsigned char COLOR_CHANNEL_MAX = 255;
    constexpr int MIN_TARGET_PIXEL_HEIGHT = 1;
    constexpr int DEFAULT_FALLBACK_FONT_SIZE = 14;

    // DX11 顶点格式
    struct DX11Vertex {
        Vec2 pos;
        Vec2 uv;
        Color col;
    };

    // 正交投影常量缓冲
    struct alignas(16) DX11ConstantBuffer {
        float mvp[4][4];
    };

    // 字形度量缓存项
    struct DX11Glyph {
        float width;       // 烘焙位图宽度
        float height;      // 烘焙位图高度
        float xOff;        // X 轴基线偏移
        float yOff;        // Y 轴基线偏移
        float xAdvance;    // 字符水平推进量
        Vec2 uv0;          // 图集左上角 UV
        Vec2 uv1;          // 图集右下角 UV
    };

    // 字符与字号双维度复合键
    struct GlyphKey {
        uint32_t Codepoint;
        int PixelHeight;

        bool operator==(const GlyphKey& other) const noexcept {
            return Codepoint == other.Codepoint && PixelHeight == other.PixelHeight;
        }
    };

    struct GlyphKeyHasher {
        size_t operator()(const GlyphKey& k) const noexcept {
            return std::hash<uint32_t>()(k.Codepoint) ^ (std::hash<int>()(k.PixelHeight) << 16);
        }
    };

    // 字体资源封装结构体 (支持全字号动态按需矢量光栅化)
    struct DX11FontResource {
        stbtt_fontinfo FontInfo{};
        std::vector<unsigned char> FontBuffer;
        int BaseFontSize = DEFAULT_FALLBACK_FONT_SIZE;
        int Ascent = 0;
        int Descent = 0;
        int LineGap = 0;

        int AtlasWidth = ATLAS_DEFAULT_WIDTH;
        int AtlasHeight = ATLAS_DEFAULT_HEIGHT;
        std::vector<unsigned char> AtlasRgbaPixels;
        int CurrentShelfX = ATLAS_PADDING;
        int CurrentShelfY = ATLAS_PADDING;
        int CurrentShelfH = 0;
        bool AtlasDirty = false;

        ID3D11Texture2D* AtlasTexture = nullptr;
        ID3D11ShaderResourceView* AtlasSRV = nullptr;
        std::unordered_map<GlyphKey, DX11Glyph, GlyphKeyHasher> Glyphs;

        ~DX11FontResource() {
            if (AtlasSRV) { AtlasSRV->Release(); AtlasSRV = nullptr; }
            if (AtlasTexture) { AtlasTexture->Release(); AtlasTexture = nullptr; }
        }
    };

    // 字体复合缓存键
    struct FontBufferCacheKey {
        const void* BufferPtr;
        int32_t FontSize;
        bool operator==(const FontBufferCacheKey& o) const {
            return BufferPtr == o.BufferPtr && FontSize == o.FontSize;
        }
    };

    struct FontBufferCacheHasher {
        size_t operator()(const FontBufferCacheKey& k) const noexcept {
            return std::hash<const void*>()(k.BufferPtr) ^ (std::hash<int32_t>()(k.FontSize) << 1);
        }
    };

    struct FontFileCacheKey {
        std::wstring FilePath;
        int32_t FontSize;
        bool operator==(const FontFileCacheKey& o) const {
            return FilePath == o.FilePath && FontSize == o.FontSize;
        }
    };

    struct FontFileCacheHasher {
        size_t operator()(const FontFileCacheKey& k) const noexcept {
            return std::hash<std::wstring>()(k.FilePath) ^ (std::hash<int32_t>()(k.FontSize) << 1);
        }
    };

    // 后端全局状态
    inline HWND g_DX11_Hwnd = nullptr;
    inline ID3D11Device* g_DX11_Device = nullptr;
    inline ID3D11DeviceContext* g_DX11_Context = nullptr;

    inline ID3D11Buffer* g_DX11_VB = nullptr;
    inline ID3D11Buffer* g_DX11_IB = nullptr;
    inline size_t g_DX11_VertexBufferSize = INITIAL_VERTEX_BUFFER_SIZE;
    inline size_t g_DX11_IndexBufferSize = INITIAL_INDEX_BUFFER_SIZE;

    inline ID3D11VertexShader* g_DX11_VertexShader = nullptr;
    inline ID3D11PixelShader* g_DX11_PixelShader = nullptr;
    inline ID3D11InputLayout* g_DX11_InputLayout = nullptr;
    inline ID3D11Buffer* g_DX11_ConstantBuffer = nullptr;
    inline ID3D11BlendState* g_DX11_BlendState = nullptr;
    inline ID3D11RasterizerState* g_DX11_RasterizerState = nullptr;
    inline ID3D11DepthStencilState* g_DX11_DepthStencilState = nullptr;
    inline ID3D11SamplerState* g_DX11_SamplerState = nullptr;

    inline ID3D11Texture2D* g_DX11_WhiteTexture = nullptr;
    inline ID3D11ShaderResourceView* g_DX11_WhiteSRV = nullptr;

    inline std::vector<std::unique_ptr<DX11FontResource>> g_DX11_FontPool;
    inline std::unordered_map<FontFileCacheKey, ShadowFont, FontFileCacheHasher> g_DX11_FileFontCache;
    inline std::unordered_map<FontBufferCacheKey, ShadowFont, FontBufferCacheHasher> g_DX11_BufferFontCache;

    inline std::vector<ID3D11ShaderResourceView*> g_DX11_TexturePool;
    inline std::unordered_map<std::wstring, ShadowTexture> g_DX11_FileTextureCache;
    inline std::unordered_map<const void*, ShadowTexture> g_DX11_BufferTextureCache;

    inline double g_DX11_Time = 0.0;
    inline LARGE_INTEGER g_DX11_PerfFrequency = { 0 };
    inline LARGE_INTEGER g_DX11_PerfCounter = { 0 };

    // UTF-8 解码为 UTF-32 Codepoint
    inline uint32_t DX11_DecodeUTF8(const char*& text, const char* end) {
        if (text >= end) return 0;
        unsigned char c = static_cast<unsigned char>(*text++);
        if (c < 0x80) return c;
        if ((c & 0xE0) == 0xC0) {
            if (text >= end) return 0;
            return ((c & 0x1F) << 6) | (static_cast<unsigned char>(*text++) & 0x3F);
        }
        if ((c & 0xF0) == 0xE0) {
            if (text + 1 >= end) return 0;
            uint32_t cp = ((c & 0x0F) << 12);
            cp |= (static_cast<unsigned char>(*text++) & 0x3F) << 6;
            cp |= (static_cast<unsigned char>(*text++) & 0x3F);
            return cp;
        }
        if ((c & 0xF8) == 0xF0) {
            if (text + 2 >= end) return 0;
            uint32_t cp = ((c & 0x07) << 18);
            cp |= (static_cast<unsigned char>(*text++) & 0x3F) << 12;
            cp |= (static_cast<unsigned char>(*text++) & 0x3F) << 6;
            cp |= (static_cast<unsigned char>(*text++) & 0x3F);
            return cp;
        }
        return c;
    }

    // 根据目标物理像素高度按需光栅化矢量字形 (无损高清，杜绝位图缩放模糊)
    inline const DX11Glyph* DX11_GetOrBakeGlyph(DX11FontResource* font, uint32_t codepoint, int pixelHeight) {
        if (!font) return nullptr;
        if (pixelHeight < MIN_TARGET_PIXEL_HEIGHT) {
            pixelHeight = MIN_TARGET_PIXEL_HEIGHT;
        }

        GlyphKey key{ codepoint, pixelHeight };
        auto it = font->Glyphs.find(key);
        if (it != font->Glyphs.end()) {
            return &it->second;
        }

        float scale = stbtt_ScaleForPixelHeight(&font->FontInfo, static_cast<float>(pixelHeight));

        int glyphIndex = stbtt_FindGlyphIndex(&font->FontInfo, static_cast<int>(codepoint));
        if (glyphIndex == 0 && codepoint != ' ') {
            glyphIndex = stbtt_FindGlyphIndex(&font->FontInfo, '?');
        }

        int advanceWidth = 0, leftSideBearing = 0;
        stbtt_GetGlyphHMetrics(&font->FontInfo, glyphIndex, &advanceWidth, &leftSideBearing);

        int ix0 = 0, iy0 = 0, ix1 = 0, iy1 = 0;
        stbtt_GetGlyphBitmapBox(&font->FontInfo, glyphIndex, scale, scale, &ix0, &iy0, &ix1, &iy1);

        int glyphWidth = ix1 - ix0;
        int glyphHeight = iy1 - iy0;

        if (font->CurrentShelfX + glyphWidth + ATLAS_PADDING >= font->AtlasWidth) {
            font->CurrentShelfY += font->CurrentShelfH + ATLAS_PADDING;
            font->CurrentShelfX = ATLAS_PADDING;
            font->CurrentShelfH = 0;
        }

        if (font->CurrentShelfY + glyphHeight + ATLAS_PADDING >= font->AtlasHeight) {
            // 图集满时重置并清空已缓存字形，触发全量重建
            font->Glyphs.clear();
            std::fill(font->AtlasRgbaPixels.begin(), font->AtlasRgbaPixels.end(), static_cast<unsigned char>(0));
            font->CurrentShelfX = ATLAS_PADDING;
            font->CurrentShelfY = ATLAS_PADDING;
            font->CurrentShelfH = 0;
            font->AtlasDirty = true;
        }

        int atlasX = font->CurrentShelfX;
        int atlasY = font->CurrentShelfY;

        if (glyphWidth > 0 && glyphHeight > 0) {
            std::vector<unsigned char> tempAlpha(glyphWidth * glyphHeight, 0);
            stbtt_MakeGlyphBitmap(&font->FontInfo, tempAlpha.data(), glyphWidth, glyphHeight, glyphWidth, scale, scale, glyphIndex);

            for (int r = 0; r < glyphHeight; ++r) {
                for (int c = 0; c < glyphWidth; ++c) {
                    unsigned char alpha = tempAlpha[r * glyphWidth + c];
                    size_t dstIdx = static_cast<size_t>(((atlasY + r) * font->AtlasWidth + (atlasX + c)) * RGBA_BYTES_PER_PIXEL);
                    font->AtlasRgbaPixels[dstIdx + 0] = COLOR_CHANNEL_MAX;
                    font->AtlasRgbaPixels[dstIdx + 1] = COLOR_CHANNEL_MAX;
                    font->AtlasRgbaPixels[dstIdx + 2] = COLOR_CHANNEL_MAX;
                    font->AtlasRgbaPixels[dstIdx + 3] = alpha;
                }
            }
        }

        font->CurrentShelfX += glyphWidth + ATLAS_PADDING;
        if (glyphHeight > font->CurrentShelfH) {
            font->CurrentShelfH = glyphHeight;
        }

        DX11Glyph glyph{};
        glyph.width = static_cast<float>(glyphWidth);
        glyph.height = static_cast<float>(glyphHeight);
        glyph.xAdvance = static_cast<float>(advanceWidth) * scale;
        glyph.xOff = static_cast<float>(ix0);
        glyph.yOff = static_cast<float>(iy0) + static_cast<float>(font->Ascent) * scale;
        glyph.uv0 = { static_cast<float>(atlasX) / static_cast<float>(font->AtlasWidth), static_cast<float>(atlasY) / static_cast<float>(font->AtlasHeight) };
        glyph.uv1 = { static_cast<float>(atlasX + glyphWidth) / static_cast<float>(font->AtlasWidth), static_cast<float>(atlasY + glyphHeight) / static_cast<float>(font->AtlasHeight) };

        font->Glyphs[key] = glyph;
        font->AtlasDirty = true;
        return &font->Glyphs[key];
    }

    // 提交 RGBA 图集到 GPU 显存
    inline void DX11_UpdateAtlasTexture(DX11FontResource* font) {
        if (!font || !font->AtlasDirty || !g_DX11_Device || !g_DX11_Context) return;

        if (!font->AtlasTexture) {
            D3D11_TEXTURE2D_DESC desc{};
            desc.Width = font->AtlasWidth;
            desc.Height = font->AtlasHeight;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

            D3D11_SUBRESOURCE_DATA subData{};
            subData.pSysMem = font->AtlasRgbaPixels.data();
            subData.SysMemPitch = font->AtlasWidth * RGBA_BYTES_PER_PIXEL;

            if (FAILED(g_DX11_Device->CreateTexture2D(&desc, &subData, &font->AtlasTexture))) {
                return;
            }

            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
            srvDesc.Format = desc.Format;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;

            if (FAILED(g_DX11_Device->CreateShaderResourceView(font->AtlasTexture, &srvDesc, &font->AtlasSRV))) {
                font->AtlasTexture->Release();
                font->AtlasTexture = nullptr;
                return;
            }
        }
        else {
            g_DX11_Context->UpdateSubresource(font->AtlasTexture, 0, nullptr, font->AtlasRgbaPixels.data(), font->AtlasWidth * RGBA_BYTES_PER_PIXEL, 0);
        }

        font->AtlasDirty = false;
    }

    // 准确文本尺寸测量（根据动态物理字号精确换算）
    inline Vec2 DX11_MeasureText(ShadowFont shadowFont, float fontScale, bool noSDF, std::string_view text) {
        if (!shadowFont.NativePtr || text.empty()) {
            return { 0.f, 0.f };
        }

        auto* font = static_cast<DX11FontResource*>(shadowFont.NativePtr);
        const auto& style = Shadow::GetStyle();
        int targetPixelHeight = std::max(MIN_TARGET_PIXEL_HEIGHT, static_cast<int>(std::round(font->BaseFontSize * fontScale * style.FontScaleDpi)));

        float scale = stbtt_ScaleForPixelHeight(&font->FontInfo, static_cast<float>(targetPixelHeight));
        float fontLineHeight = static_cast<float>(font->Ascent - font->Descent) * scale;
        if (fontLineHeight <= 0.0f) {
            fontLineHeight = static_cast<float>(targetPixelHeight);
        }

        float width = 0.0f;
        const char* p = text.data();
        const char* end = p + text.size();

        while (p < end) {
            uint32_t cp = DX11_DecodeUTF8(p, end);
            if (cp == 0) break;
            const DX11Glyph* g = DX11_GetOrBakeGlyph(font, cp, targetPixelHeight);
            if (g) {
                width += g->xAdvance;
            }
        }

        return { width, fontLineHeight };
    }

    inline ShadowTexture DX11_LoadTextureFromBuffer(const unsigned char* BufferData, size_t BufferSize) {
        if (!BufferData || BufferSize == 0 || !g_DX11_Device) return ShadowTexture{};

        auto it = g_DX11_BufferTextureCache.find(BufferData);
        if (it != g_DX11_BufferTextureCache.end()) return it->second;

        int width = 0, height = 0, channels = 0;
        unsigned char* image = stbi_load_from_memory(BufferData, static_cast<int>(BufferSize), &width, &height, &channels, 4);
        if (!image) return ShadowTexture{};

        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA subData{};
        subData.pSysMem = image;
        subData.SysMemPitch = width * RGBA_BYTES_PER_PIXEL;

        ID3D11Texture2D* pTexture = nullptr;
        ID3D11ShaderResourceView* pSRV = nullptr;

        HRESULT hr = g_DX11_Device->CreateTexture2D(&desc, &subData, &pTexture);
        stbi_image_free(image);

        if (FAILED(hr)) return ShadowTexture{};

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        hr = g_DX11_Device->CreateShaderResourceView(pTexture, &srvDesc, &pSRV);
        pTexture->Release();

        if (FAILED(hr)) return ShadowTexture{};

        g_DX11_TexturePool.push_back(pSRV);
        ShadowTexture tex{ pSRV };
        g_DX11_BufferTextureCache[BufferData] = tex;
        return tex;
    }

    inline ShadowTexture DX11_LoadTextureFromFile(const wchar_t* FilePath) {
        if (!FilePath || !g_DX11_Device) return ShadowTexture{};

        auto it = g_DX11_FileTextureCache.find(FilePath);
        if (it != g_DX11_FileTextureCache.end()) return it->second;

        FILE* f = nullptr;
        _wfopen_s(&f, FilePath, L"rb");
        if (!f) return ShadowTexture{};

        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fseek(f, 0, SEEK_SET);

        if (size <= 0) {
            fclose(f);
            return ShadowTexture{};
        }

        std::vector<unsigned char> buffer(size);
        fread(buffer.data(), 1, size, f);
        fclose(f);

        ShadowTexture tex = DX11_LoadTextureFromBuffer(buffer.data(), buffer.size());
        if (tex) {
            g_DX11_FileTextureCache[FilePath] = tex;
        }
        return tex;
    }

    // 字体装载实现（结合 TTC 偏移检测与矢量轮廓初始化）
    inline ShadowFont DX11_LoadFontFromBuffer(const unsigned char* BufferData, size_t BufferSize, int32_t FontSize = DEFAULT_FONT_SIZE) {
        if (!BufferData || BufferSize == 0) return ShadowFont{};

        const auto& style = Shadow::GetStyle();
        int targetFontSize = (FontSize >= MIN_VALID_FONT_SIZE) ? FontSize : static_cast<int>(style.FallbackFontSize);
        if (targetFontSize <= 0) targetFontSize = DEFAULT_FALLBACK_FONT_SIZE;

        FontBufferCacheKey key{ BufferData, targetFontSize };
        auto it = g_DX11_BufferFontCache.find(key);
        if (it != g_DX11_BufferFontCache.end()) return it->second;

        auto fontRes = std::make_unique<DX11FontResource>();
        fontRes->FontBuffer.assign(BufferData, BufferData + BufferSize);

        int fontOffset = stbtt_GetFontOffsetForIndex(fontRes->FontBuffer.data(), 0);
        if (fontOffset < 0) {
            fontOffset = 0;
        }

        if (!stbtt_InitFont(&fontRes->FontInfo, fontRes->FontBuffer.data(), fontOffset)) {
            return ShadowFont{};
        }

        fontRes->BaseFontSize = targetFontSize;
        stbtt_GetFontVMetrics(&fontRes->FontInfo, &fontRes->Ascent, &fontRes->Descent, &fontRes->LineGap);

        fontRes->AtlasRgbaPixels.resize(static_cast<size_t>(fontRes->AtlasWidth * fontRes->AtlasHeight * RGBA_BYTES_PER_PIXEL), 0);

        // 预热基础 ASCII 字符
        for (uint32_t c = 32; c <= 126; ++c) {
            DX11_GetOrBakeGlyph(fontRes.get(), c, targetFontSize);
        }

        ShadowFont shadowFont{ fontRes.get() };
        g_DX11_BufferFontCache[key] = shadowFont;
        g_DX11_FontPool.push_back(std::move(fontRes));
        return shadowFont;
    }

    inline ShadowFont DX11_LoadFontFromFile(const wchar_t* FilePath, int32_t FontSize = DEFAULT_FONT_SIZE) {
        if (!FilePath) return ShadowFont{};

        const auto& style = Shadow::GetStyle();
        int targetFontSize = (FontSize >= MIN_VALID_FONT_SIZE) ? FontSize : static_cast<int>(style.FallbackFontSize);
        if (targetFontSize <= 0) targetFontSize = DEFAULT_FALLBACK_FONT_SIZE;

        FontFileCacheKey key{ FilePath, targetFontSize };
        auto it = g_DX11_FileFontCache.find(key);
        if (it != g_DX11_FileFontCache.end()) return it->second;

        FILE* f = nullptr;
        _wfopen_s(&f, FilePath, L"rb");
        if (!f) return ShadowFont{};

        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fseek(f, 0, SEEK_SET);

        if (size <= 0) {
            fclose(f);
            return ShadowFont{};
        }

        std::vector<unsigned char> buffer(size);
        fread(buffer.data(), 1, size, f);
        fclose(f);

        auto fontRes = std::make_unique<DX11FontResource>();
        fontRes->FontBuffer = std::move(buffer);

        int fontOffset = stbtt_GetFontOffsetForIndex(fontRes->FontBuffer.data(), 0);
        if (fontOffset < 0) {
            fontOffset = 0;
        }

        if (!stbtt_InitFont(&fontRes->FontInfo, fontRes->FontBuffer.data(), fontOffset)) {
            return ShadowFont{};
        }

        fontRes->BaseFontSize = targetFontSize;
        stbtt_GetFontVMetrics(&fontRes->FontInfo, &fontRes->Ascent, &fontRes->Descent, &fontRes->LineGap);

        fontRes->AtlasRgbaPixels.resize(static_cast<size_t>(fontRes->AtlasWidth * fontRes->AtlasHeight * RGBA_BYTES_PER_PIXEL), 0);

        for (uint32_t c = 32; c <= 126; ++c) {
            DX11_GetOrBakeGlyph(fontRes.get(), c, targetFontSize);
        }

        ShadowFont shadowFont{ fontRes.get() };
        g_DX11_FileFontCache[key] = shadowFont;
        g_DX11_FontPool.push_back(std::move(fontRes));
        return shadowFont;
    }

    inline void DX11_RegisterBackendCallbacks() {
        Shadow::g_Ctx.TextSizeFn = &DX11_MeasureText;
        Shadow::g_Ctx.LoadTextureFromBufferCallback = &DX11_LoadTextureFromBuffer;
        Shadow::g_Ctx.LoadTextureFromFileCallback = &DX11_LoadTextureFromFile;
        Shadow::g_Ctx.LoadFontFromFileCallback = &DX11_LoadFontFromFile;
        Shadow::g_Ctx.LoadFontFromBufferCallback = &DX11_LoadFontFromBuffer;
    }

    inline bool DX11_CreateDeviceObjects() {
        if (!g_DX11_Device) return false;

        const char* vertexShaderHLSL =
            "cbuffer ProjectionBuffer : register(b0) {\n"
            "    float4x4 ProjectionMatrix;\n"
            "};\n"
            "struct VS_INPUT {\n"
            "    float2 pos : POSITION;\n"
            "    float2 uv  : TEXCOORD0;\n"
            "    float4 col : COLOR0;\n"
            "};\n"
            "struct PS_INPUT {\n"
            "    float4 pos : SV_POSITION;\n"
            "    float2 uv  : TEXCOORD0;\n"
            "    float4 col : COLOR0;\n"
            "};\n"
            "PS_INPUT main(VS_INPUT input) {\n"
            "    PS_INPUT output;\n"
            "    output.pos = mul(ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\n"
            "    output.col = input.col;\n"
            "    output.uv  = input.uv;\n"
            "    return output;\n"
            "}\n";

        const char* pixelShaderHLSL =
            "struct PS_INPUT {\n"
            "    float4 pos : SV_POSITION;\n"
            "    float2 uv  : TEXCOORD0;\n"
            "    float4 col : COLOR0;\n"
            "};\n"
            "SamplerState Sampler : register(s0);\n"
            "Texture2D Texture : register(t0);\n"
            "float4 main(PS_INPUT input) : SV_Target {\n"
            "    float4 texColor = Texture.Sample(Sampler, input.uv);\n"
            "    return input.col * texColor;\n"
            "}\n";

        ID3DBlob* vsBlob = nullptr;
        if (FAILED(D3DCompile(vertexShaderHLSL, strlen(vertexShaderHLSL), nullptr, nullptr, nullptr, "main", "vs_4_0", 0, 0, &vsBlob, nullptr))) {
            return false;
        }
        if (FAILED(g_DX11_Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &g_DX11_VertexShader))) {
            vsBlob->Release();
            return false;
        }

        D3D11_INPUT_ELEMENT_DESC localLayout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,       0, offsetof(DX11Vertex, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, offsetof(DX11Vertex, uv),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(DX11Vertex, col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        if (FAILED(g_DX11_Device->CreateInputLayout(localLayout, 3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &g_DX11_InputLayout))) {
            vsBlob->Release();
            return false;
        }
        vsBlob->Release();

        ID3DBlob* psBlob = nullptr;
        if (FAILED(D3DCompile(pixelShaderHLSL, strlen(pixelShaderHLSL), nullptr, nullptr, nullptr, "main", "ps_4_0", 0, 0, &psBlob, nullptr))) {
            return false;
        }
        if (FAILED(g_DX11_Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &g_DX11_PixelShader))) {
            psBlob->Release();
            return false;
        }
        psBlob->Release();

        D3D11_BUFFER_DESC cbDesc{};
        cbDesc.ByteWidth = sizeof(DX11ConstantBuffer);
        cbDesc.Usage = D3D11_USAGE_DYNAMIC;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (FAILED(g_DX11_Device->CreateBuffer(&cbDesc, nullptr, &g_DX11_ConstantBuffer))) {
            return false;
        }

        D3D11_BLEND_DESC blendDesc{};
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        if (FAILED(g_DX11_Device->CreateBlendState(&blendDesc, &g_DX11_BlendState))) {
            return false;
        }

        D3D11_RASTERIZER_DESC rasterDesc{};
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.ScissorEnable = TRUE;
        rasterDesc.DepthClipEnable = TRUE;
        if (FAILED(g_DX11_Device->CreateRasterizerState(&rasterDesc, &g_DX11_RasterizerState))) {
            return false;
        }

        D3D11_DEPTH_STENCIL_DESC depthDesc{};
        depthDesc.DepthEnable = FALSE;
        depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        depthDesc.StencilEnable = FALSE;
        if (FAILED(g_DX11_Device->CreateDepthStencilState(&depthDesc, &g_DX11_DepthStencilState))) {
            return false;
        }

        D3D11_SAMPLER_DESC samplerDesc{};
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        if (FAILED(g_DX11_Device->CreateSamplerState(&samplerDesc, &g_DX11_SamplerState))) {
            return false;
        }

        uint32_t whitePixel = 0xFFFFFFFF;
        D3D11_SUBRESOURCE_DATA initData{};
        initData.pSysMem = &whitePixel;
        initData.SysMemPitch = 4;

        D3D11_TEXTURE2D_DESC texDesc{};
        texDesc.Width = 1;
        texDesc.Height = 1;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_IMMUTABLE;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        if (FAILED(g_DX11_Device->CreateTexture2D(&texDesc, &initData, &g_DX11_WhiteTexture))) {
            return false;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        if (FAILED(g_DX11_Device->CreateShaderResourceView(g_DX11_WhiteTexture, &srvDesc, &g_DX11_WhiteSRV))) {
            return false;
        }

        return true;
    }

    inline void DX11_InvalidateDeviceObjects() {
        if (g_DX11_VB) { g_DX11_VB->Release(); g_DX11_VB = nullptr; }
        if (g_DX11_IB) { g_DX11_IB->Release(); g_DX11_IB = nullptr; }
        if (g_DX11_VertexShader) { g_DX11_VertexShader->Release(); g_DX11_VertexShader = nullptr; }
        if (g_DX11_InputLayout) { g_DX11_InputLayout->Release(); g_DX11_InputLayout = nullptr; }
        if (g_DX11_PixelShader) { g_DX11_PixelShader->Release(); g_DX11_PixelShader = nullptr; }
        if (g_DX11_ConstantBuffer) { g_DX11_ConstantBuffer->Release(); g_DX11_ConstantBuffer = nullptr; }
        if (g_DX11_BlendState) { g_DX11_BlendState->Release(); g_DX11_BlendState = nullptr; }
        if (g_DX11_RasterizerState) { g_DX11_RasterizerState->Release(); g_DX11_RasterizerState = nullptr; }
        if (g_DX11_DepthStencilState) { g_DX11_DepthStencilState->Release(); g_DX11_DepthStencilState = nullptr; }
        if (g_DX11_SamplerState) { g_DX11_SamplerState->Release(); g_DX11_SamplerState = nullptr; }
        if (g_DX11_WhiteSRV) { g_DX11_WhiteSRV->Release(); g_DX11_WhiteSRV = nullptr; }
        if (g_DX11_WhiteTexture) { g_DX11_WhiteTexture->Release(); g_DX11_WhiteTexture = nullptr; }

        for (auto& f : g_DX11_FontPool) {
            if (f->AtlasSRV) { f->AtlasSRV->Release(); f->AtlasSRV = nullptr; }
            if (f->AtlasTexture) { f->AtlasTexture->Release(); f->AtlasTexture = nullptr; }
            f->AtlasDirty = true;
        }

        for (auto* srv : g_DX11_TexturePool) {
            if (srv) srv->Release();
        }
        g_DX11_TexturePool.clear();
        g_DX11_FileTextureCache.clear();
        g_DX11_BufferTextureCache.clear();
    }

    inline bool DX11_Init(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context) {
        g_DX11_Hwnd = hWnd;
        g_DX11_Device = device;
        g_DX11_Context = context;

        QueryPerformanceFrequency(&g_DX11_PerfFrequency);
        QueryPerformanceCounter(&g_DX11_PerfCounter);

        DX11_RegisterBackendCallbacks();
        return DX11_CreateDeviceObjects();
    }

    inline void DX11_Shutdown() {
        DX11_InvalidateDeviceObjects();
        g_DX11_FontPool.clear();
        g_DX11_FileFontCache.clear();
        g_DX11_BufferFontCache.clear();
        g_DX11_Device = nullptr;
        g_DX11_Context = nullptr;
        g_DX11_Hwnd = nullptr;
    }

    inline void DX11_NewFrame() {
        if (!g_DX11_VertexShader) {
            DX11_CreateDeviceObjects();
        }

        DX11_RegisterBackendCallbacks();

        LARGE_INTEGER currentCounter;
        QueryPerformanceCounter(&currentCounter);
        double dt = static_cast<double>(currentCounter.QuadPart - g_DX11_PerfCounter.QuadPart) / static_cast<double>(g_DX11_PerfFrequency.QuadPart);
        g_DX11_PerfCounter = currentCounter;
        g_DX11_Time += dt;

        RECT rect{ 0, 0, 0, 0 };
        if (g_DX11_Hwnd) {
            GetClientRect(g_DX11_Hwnd, &rect);
        }

        ShadowIO& io = Shadow::GetIO();
        io.DisplaySize = { static_cast<float>(rect.right - rect.left), static_cast<float>(rect.bottom - rect.top) };
        io.DeltaTime = static_cast<float>(dt);

        Shadow::g_Ctx.RealTimeSeconds = g_DX11_Time;
        Shadow::g_Ctx.DeltaTime = dt;

        if (!Shadow::g_Ctx.DefaultFont) {
            Shadow::g_Ctx.DefaultFont = DX11_LoadFontFromFile(L"C:\\Windows\\Fonts\\msyh.ttc", DEFAULT_FALLBACK_FONT_SIZE);
            if (!Shadow::g_Ctx.DefaultFont) {
                Shadow::g_Ctx.DefaultFont = DX11_LoadFontFromFile(L"C:\\Windows\\Fonts\\simhei.ttf", DEFAULT_FALLBACK_FONT_SIZE);
            }
            if (!Shadow::g_Ctx.DefaultFont) {
                Shadow::g_Ctx.DefaultFont = DX11_LoadFontFromFile(L"C:\\Windows\\Fonts\\arial.ttf", DEFAULT_FALLBACK_FONT_SIZE);
            }
        }
    }

    // 内部几何生成器
    struct DX11MeshBuilder {
        std::vector<DX11Vertex> Vertices;
        std::vector<uint32_t> Indices;

        void AddVertex(Vec2 pos, Vec2 uv, Color col) {
            Vertices.push_back({ pos, uv, col });
        }

        void AddTriangleIndices(uint32_t i0, uint32_t i1, uint32_t i2) {
            Indices.push_back(i0);
            Indices.push_back(i1);
            Indices.push_back(i2);
        }

        void BuildThickLine(Vec2 p0, Vec2 p1, Color col, float thickness) {
            float dx = p1.x - p0.x;
            float dy = p1.y - p0.y;
            float len = std::sqrt(dx * dx + dy * dy);
            if (len <= 0.0f) return;

            float nx = -dy / len * (thickness * THICKNESS_HALF_RATIO);
            float ny = dx / len * (thickness * THICKNESS_HALF_RATIO);

            uint32_t idx = static_cast<uint32_t>(Vertices.size());
            AddVertex({ p0.x + nx, p0.y + ny }, { 0.f, 0.f }, col);
            AddVertex({ p0.x - nx, p0.y - ny }, { 0.f, 0.f }, col);
            AddVertex({ p1.x - nx, p1.y - ny }, { 0.f, 0.f }, col);
            AddVertex({ p1.x + nx, p1.y + ny }, { 0.f, 0.f }, col);

            AddTriangleIndices(idx, idx + 1, idx + 2);
            AddTriangleIndices(idx, idx + 2, idx + 3);
        }

        void BuildRectFilled(Vec2 pos, Vec2 size, Color col, float rounding, ShadowDrawFlags flags) {
            if (size.x <= 0.f || size.y <= 0.f) return;

            bool roundTL = (flags & ShadowDrawFlags_RoundCornersTopLeft) && !(flags & ShadowDrawFlags_RoundCornersNone);
            bool roundTR = (flags & ShadowDrawFlags_RoundCornersTopRight) && !(flags & ShadowDrawFlags_RoundCornersNone);
            bool roundBL = (flags & ShadowDrawFlags_RoundCornersBottomLeft) && !(flags & ShadowDrawFlags_RoundCornersNone);
            bool roundBR = (flags & ShadowDrawFlags_RoundCornersBottomRight) && !(flags & ShadowDrawFlags_RoundCornersNone);

            if (rounding <= 0.0f || (!roundTL && !roundTR && !roundBL && !roundBR)) {
                uint32_t idx = static_cast<uint32_t>(Vertices.size());
                AddVertex(pos, { 0.f, 0.f }, col);
                AddVertex({ pos.x + size.x, pos.y }, { 1.f, 0.f }, col);
                AddVertex({ pos.x + size.x, pos.y + size.y }, { 1.f, 1.f }, col);
                AddVertex({ pos.x, pos.y + size.y }, { 0.f, 1.f }, col);

                AddTriangleIndices(idx, idx + 1, idx + 2);
                AddTriangleIndices(idx, idx + 2, idx + 3);
                return;
            }

            float r = std::min(rounding, std::min(size.x, size.y) * 0.5f);
            uint32_t centerIdx = static_cast<uint32_t>(Vertices.size());
            AddVertex({ pos.x + size.x * 0.5f, pos.y + size.y * 0.5f }, { 0.5f, 0.5f }, col);

            auto AppendArc = [&](Vec2 center, float radius, float startAngle, float endAngle) {
                int segs = CORNER_ROUNDING_SEGMENTS;
                for (int i = 0; i <= segs; ++i) {
                    float t = static_cast<float>(i) / static_cast<float>(segs);
                    float a = startAngle + t * (endAngle - startAngle);
                    AddVertex({ center.x + radius * std::cos(a), center.y - radius * std::sin(a) }, { 0.f, 0.f }, col);
                }
                };

            if (roundTL) AppendArc({ pos.x + r, pos.y + r }, r, MATH_PI, MATH_PI * 0.5f);
            else AddVertex(pos, { 0.f, 0.f }, col);

            if (roundTR) AppendArc({ pos.x + size.x - r, pos.y + r }, r, MATH_PI * 0.5f, 0.0f);
            else AddVertex({ pos.x + size.x, pos.y }, { 1.f, 0.f }, col);

            if (roundBR) AppendArc({ pos.x + size.x - r, pos.y + size.y - r }, r, 0.0f, -MATH_PI * 0.5f);
            else AddVertex({ pos.x + size.x, pos.y + size.y }, { 1.f, 1.f }, col);

            if (roundBL) AppendArc({ pos.x + r, pos.y + size.y - r }, r, -MATH_PI * 0.5f, -MATH_PI);
            else AddVertex({ pos.x, pos.y + size.y }, { 0.f, 1.f }, col);

            uint32_t totalPerimeter = static_cast<uint32_t>(Vertices.size() - centerIdx - 1);
            for (uint32_t i = 0; i < totalPerimeter; ++i) {
                uint32_t current = centerIdx + 1 + i;
                uint32_t next = (i + 1 < totalPerimeter) ? (current + 1) : (centerIdx + 1);
                AddTriangleIndices(centerIdx, current, next);
            }
        }

        void BuildCircleFilled(Vec2 center, float radius, Color col) {
            if (radius <= 0.0f) return;
            uint32_t centerIdx = static_cast<uint32_t>(Vertices.size());
            AddVertex(center, { 0.5f, 0.5f }, col);

            int segs = CIRCLE_NUM_SEGMENTS;
            for (int i = 0; i <= segs; ++i) {
                float a = (static_cast<float>(i) / static_cast<float>(segs)) * 2.0f * MATH_PI;
                AddVertex({ center.x + radius * std::cos(a), center.y + radius * std::sin(a) }, { 0.5f, 0.5f }, col);
            }

            for (int i = 0; i < segs; ++i) {
                AddTriangleIndices(centerIdx, centerIdx + 1 + i, centerIdx + 2 + i);
            }
        }
    };

    // 渲染后端 DrawData 核心执行逻辑（集成预扫描按需矢量光栅化与 1:1 像素映射）
    inline void DX11_RenderDrawData(const ShadowDrawData* drawData) {
        if (!drawData || !drawData->Valid || !g_DX11_Context || !g_DX11_Device) return;
        if (drawData->DisplaySize.x <= 0.f || drawData->DisplaySize.y <= 0.f) return;

        const auto& globalStyle = Shadow::GetStyle();

        // 第一阶段：预扫描所有文本命令，对当前帧使用到的所有 (字符, 实际物理字号) 进行原生矢量光栅化
        for (const ShadowDrawList* cmdList : drawData->CmdLists) {
            if (!cmdList) continue;
            for (const auto& cmd : cmdList->CmdBuffer) {
                if (cmd.type == ShadowDrawCmdType::Text && cmd.font.NativePtr && !cmd.text.empty()) {
                    auto* font = static_cast<DX11FontResource*>(cmd.font.NativePtr);
                    int targetPixelHeight = std::max(MIN_TARGET_PIXEL_HEIGHT, static_cast<int>(std::round(font->BaseFontSize * cmd.fontScale * globalStyle.FontScaleDpi)));

                    const char* p = cmd.text.data();
                    const char* end = p + cmd.text.size();
                    while (p < end) {
                        uint32_t cp = DX11_DecodeUTF8(p, end);
                        if (cp == 0) break;
                        DX11_GetOrBakeGlyph(font, cp, targetPixelHeight);
                    }
                }
            }
        }

        // 第二阶段：一次性将所有发生变动的 Font Atlas 提交上传至 GPU 显存
        for (auto& font : g_DX11_FontPool) {
            if (font->AtlasDirty || !font->AtlasTexture) {
                DX11_UpdateAtlasTexture(font.get());
            }
        }

        // 设置视口
        D3D11_VIEWPORT vp{};
        vp.Width = drawData->DisplaySize.x;
        vp.Height = drawData->DisplaySize.y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        g_DX11_Context->RSSetViewports(1, &vp);

        // 更新正交投影矩阵
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        if (SUCCEEDED(g_DX11_Context->Map(g_DX11_ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
            auto* cb = static_cast<DX11ConstantBuffer*>(mappedResource.pData);
            float L = 0.0f;
            float R = drawData->DisplaySize.x;
            float T = 0.0f;
            float B = drawData->DisplaySize.y;
            float mvp[4][4] = {
                { 2.0f / (R - L),    0.0f,              0.0f, 0.0f },
                { 0.0f,              2.0f / (T - B),    0.0f, 0.0f },
                { 0.0f,              0.0f,              0.5f, 0.0f },
                { (R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f },
            };
            memcpy(cb->mvp, mvp, sizeof(mvp));
            g_DX11_Context->Unmap(g_DX11_ConstantBuffer, 0);
        }

        // 绑定 DX11 管线状态
        UINT stride = sizeof(DX11Vertex);
        UINT offset = 0;
        g_DX11_Context->IASetInputLayout(g_DX11_InputLayout);
        g_DX11_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        g_DX11_Context->VSSetShader(g_DX11_VertexShader, nullptr, 0);
        g_DX11_Context->VSSetConstantBuffers(0, 1, &g_DX11_ConstantBuffer);
        g_DX11_Context->PSSetShader(g_DX11_PixelShader, nullptr, 0);
        g_DX11_Context->PSSetSamplers(0, 1, &g_DX11_SamplerState);
        g_DX11_Context->RSSetState(g_DX11_RasterizerState);
        g_DX11_Context->OMSetDepthStencilState(g_DX11_DepthStencilState, 0);

        float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
        g_DX11_Context->OMSetBlendState(g_DX11_BlendState, blendFactor, 0xFFFFFFFF);

        auto RenderBatch = [&](const DX11MeshBuilder& mesh, ID3D11ShaderResourceView* srv, const D3D11_RECT& scissor) {
            if (mesh.Vertices.empty() || mesh.Indices.empty()) return;

            if (!g_DX11_VB || g_DX11_VertexBufferSize < mesh.Vertices.size()) {
                if (g_DX11_VB) { g_DX11_VB->Release(); g_DX11_VB = nullptr; }
                g_DX11_VertexBufferSize = mesh.Vertices.size() + INITIAL_VERTEX_BUFFER_SIZE;
                D3D11_BUFFER_DESC desc{};
                desc.ByteWidth = static_cast<UINT>(g_DX11_VertexBufferSize * sizeof(DX11Vertex));
                desc.Usage = D3D11_USAGE_DYNAMIC;
                desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                g_DX11_Device->CreateBuffer(&desc, nullptr, &g_DX11_VB);
            }

            if (!g_DX11_IB || g_DX11_IndexBufferSize < mesh.Indices.size()) {
                if (g_DX11_IB) { g_DX11_IB->Release(); g_DX11_IB = nullptr; }
                g_DX11_IndexBufferSize = mesh.Indices.size() + INITIAL_INDEX_BUFFER_SIZE;
                D3D11_BUFFER_DESC desc{};
                desc.ByteWidth = static_cast<UINT>(g_DX11_IndexBufferSize * sizeof(uint32_t));
                desc.Usage = D3D11_USAGE_DYNAMIC;
                desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                g_DX11_Device->CreateBuffer(&desc, nullptr, &g_DX11_IB);
            }

            D3D11_MAPPED_SUBRESOURCE vMap;
            if (SUCCEEDED(g_DX11_Context->Map(g_DX11_VB, 0, D3D11_MAP_WRITE_DISCARD, 0, &vMap))) {
                memcpy(vMap.pData, mesh.Vertices.data(), mesh.Vertices.size() * sizeof(DX11Vertex));
                g_DX11_Context->Unmap(g_DX11_VB, 0);
            }

            D3D11_MAPPED_SUBRESOURCE iMap;
            if (SUCCEEDED(g_DX11_Context->Map(g_DX11_IB, 0, D3D11_MAP_WRITE_DISCARD, 0, &iMap))) {
                memcpy(iMap.pData, mesh.Indices.data(), mesh.Indices.size() * sizeof(uint32_t));
                g_DX11_Context->Unmap(g_DX11_IB, 0);
            }

            g_DX11_Context->IASetVertexBuffers(0, 1, &g_DX11_VB, &stride, &offset);
            g_DX11_Context->IASetIndexBuffer(g_DX11_IB, DXGI_FORMAT_R32_UINT, 0);
            g_DX11_Context->RSSetScissorRects(1, &scissor);

            ID3D11ShaderResourceView* bindSRV = srv ? srv : g_DX11_WhiteSRV;
            g_DX11_Context->PSSetShaderResources(0, 1, &bindSRV);

            g_DX11_Context->DrawIndexed(static_cast<UINT>(mesh.Indices.size()), 0, 0);
            };

        for (const ShadowDrawList* cmdList : drawData->CmdLists) {
            if (!cmdList) continue;
            for (const auto& cmd : cmdList->CmdBuffer) {
                D3D11_RECT scissor{ 0, 0, static_cast<LONG>(drawData->DisplaySize.x), static_cast<LONG>(drawData->DisplaySize.y) };
                if (cmd.clippingEnabled) {
                    scissor.left = std::max(0L, static_cast<LONG>(cmd.clipMin.x));
                    scissor.top = std::max(0L, static_cast<LONG>(cmd.clipMin.y));
                    scissor.right = std::min(static_cast<LONG>(drawData->DisplaySize.x), static_cast<LONG>(cmd.clipMax.x));
                    scissor.bottom = std::min(static_cast<LONG>(drawData->DisplaySize.y), static_cast<LONG>(cmd.clipMax.y));
                    if (scissor.right <= scissor.left || scissor.bottom <= scissor.top) continue;
                }

                DX11MeshBuilder builder;
                ID3D11ShaderResourceView* texSRV = static_cast<ID3D11ShaderResourceView*>(cmd.texture.NativePtr);

                switch (cmd.type) {
                case ShadowDrawCmdType::Line:
                    builder.BuildThickLine(cmd.pos, cmd.size, cmd.color, cmd.thickness);
                    RenderBatch(builder, texSRV, scissor);
                    break;
                case ShadowDrawCmdType::Rect: {
                    float halfThick = cmd.thickness * THICKNESS_HALF_RATIO;
                    builder.BuildThickLine({ cmd.pos.x - halfThick, cmd.pos.y }, { cmd.pos.x + cmd.size.x + halfThick, cmd.pos.y }, cmd.color, cmd.thickness);
                    builder.BuildThickLine({ cmd.pos.x - halfThick, cmd.pos.y + cmd.size.y }, { cmd.pos.x + cmd.size.x + halfThick, cmd.pos.y + cmd.size.y }, cmd.color, cmd.thickness);
                    builder.BuildThickLine({ cmd.pos.x, cmd.pos.y + halfThick }, { cmd.pos.x, cmd.pos.y + cmd.size.y - halfThick }, cmd.color, cmd.thickness);
                    builder.BuildThickLine({ cmd.pos.x + cmd.size.x, cmd.pos.y + halfThick }, { cmd.pos.x + cmd.size.x, cmd.pos.y + cmd.size.y - halfThick }, cmd.color, cmd.thickness);
                    RenderBatch(builder, texSRV, scissor);
                    break;
                }
                case ShadowDrawCmdType::RectFilled:
                    builder.BuildRectFilled(cmd.pos, cmd.size, cmd.color, cmd.rounding, cmd.flags);
                    RenderBatch(builder, texSRV, scissor);
                    break;
                case ShadowDrawCmdType::Texture: {
                    uint32_t idx = static_cast<uint32_t>(builder.Vertices.size());
                    builder.AddVertex(cmd.pos, { TEXTURE_DEFAULT_UV_MIN, TEXTURE_DEFAULT_UV_MIN }, cmd.color);
                    builder.AddVertex({ cmd.pos.x + cmd.size.x, cmd.pos.y }, { TEXTURE_DEFAULT_UV_MAX, TEXTURE_DEFAULT_UV_MIN }, cmd.color);
                    builder.AddVertex({ cmd.pos.x + cmd.size.x, cmd.pos.y + cmd.size.y }, { TEXTURE_DEFAULT_UV_MAX, TEXTURE_DEFAULT_UV_MAX }, cmd.color);
                    builder.AddVertex({ cmd.pos.x, cmd.pos.y + cmd.size.y }, { TEXTURE_DEFAULT_UV_MIN, TEXTURE_DEFAULT_UV_MAX }, cmd.color);
                    builder.AddTriangleIndices(idx, idx + 1, idx + 2);
                    builder.AddTriangleIndices(idx, idx + 2, idx + 3);
                    RenderBatch(builder, texSRV, scissor);
                    break;
                }
                case ShadowDrawCmdType::CircleFilled:
                    builder.BuildCircleFilled(cmd.pos, cmd.size.x, cmd.color);
                    RenderBatch(builder, texSRV, scissor);
                    break;
                case ShadowDrawCmdType::Triangle:
                    builder.BuildThickLine(cmd.p1, cmd.p2, cmd.color, cmd.thickness);
                    builder.BuildThickLine(cmd.p2, cmd.p3, cmd.color, cmd.thickness);
                    builder.BuildThickLine(cmd.p3, cmd.p1, cmd.color, cmd.thickness);
                    RenderBatch(builder, texSRV, scissor);
                    break;
                case ShadowDrawCmdType::TriangleFilled: {
                    uint32_t idx = static_cast<uint32_t>(builder.Vertices.size());
                    builder.AddVertex(cmd.p1, { 0.f, 0.f }, cmd.color);
                    builder.AddVertex(cmd.p2, { 0.f, 0.f }, cmd.color);
                    builder.AddVertex(cmd.p3, { 0.f, 0.f }, cmd.color);
                    builder.AddTriangleIndices(idx, idx + 1, idx + 2);
                    RenderBatch(builder, texSRV, scissor);
                    break;
                }
                case ShadowDrawCmdType::Text: {
                    if (!cmd.font.NativePtr || cmd.text.empty()) break;
                    auto* font = static_cast<DX11FontResource*>(cmd.font.NativePtr);
                    int targetPixelHeight = std::max(MIN_TARGET_PIXEL_HEIGHT, static_cast<int>(std::round(font->BaseFontSize * cmd.fontScale * globalStyle.FontScaleDpi)));

                    auto DrawString = [&](Vec2 pos, Color col) {
                        const char* p = cmd.text.data();
                        const char* end = p + cmd.text.size();
                        float curX = pos.x;
                        float curY = pos.y;

                        while (p < end) {
                            uint32_t cp = DX11_DecodeUTF8(p, end);
                            if (cp == 0) break;

                            const DX11Glyph* g = DX11_GetOrBakeGlyph(font, cp, targetPixelHeight);
                            if (g) {
                                float x0 = std::floor(curX + g->xOff);
                                float y0 = std::floor(curY + g->yOff);
                                float x1 = x0 + g->width;
                                float y1 = y0 + g->height;

                                uint32_t vidx = static_cast<uint32_t>(builder.Vertices.size());
                                builder.AddVertex({ x0, y0 }, { g->uv0.x, g->uv0.y }, col);
                                builder.AddVertex({ x1, y0 }, { g->uv1.x, g->uv0.y }, col);
                                builder.AddVertex({ x1, y1 }, { g->uv1.x, g->uv1.y }, col);
                                builder.AddVertex({ x0, y1 }, { g->uv0.x, g->uv1.y }, col);

                                builder.AddTriangleIndices(vidx, vidx + 1, vidx + 2);
                                builder.AddTriangleIndices(vidx, vidx + 2, vidx + 3);

                                curX += g->xAdvance;
                            }
                        }
                        };

                    if (cmd.textShadowColor.a > 0.001f) {
                        DrawString({ cmd.pos.x + globalStyle.TextShadowOffsetX, cmd.pos.y + globalStyle.TextShadowOffsetY }, cmd.textShadowColor);
                    }
                    if (cmd.textOutline && cmd.textOutlineColor.a > 0.001f) {
                        DrawString({ cmd.pos.x - 1.f, cmd.pos.y }, cmd.textOutlineColor);
                        DrawString({ cmd.pos.x + 1.f, cmd.pos.y }, cmd.textOutlineColor);
                        DrawString({ cmd.pos.x, cmd.pos.y - 1.f }, cmd.textOutlineColor);
                        DrawString({ cmd.pos.x, cmd.pos.y + 1.f }, cmd.textOutlineColor);
                    }
                    DrawString(cmd.pos, cmd.color);

                    RenderBatch(builder, font->AtlasSRV, scissor);
                    break;
                }
                }
            }
        }
    }
} // namespace Shadow