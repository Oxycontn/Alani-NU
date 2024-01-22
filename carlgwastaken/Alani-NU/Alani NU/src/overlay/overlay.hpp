#pragma once
#include "../../ext/ImGui/imgui.h"
#include "../../ext/ImGui/imgui_impl_win32.h"
#include "../../ext/ImGui/imgui_impl_dx11.h"
#include <d3d11.h>
#include <string>


// this is way to fucking ghetto but whatever honestly, it works for now :shrug:
typedef struct
{
    float R;
    float G;
    float B;
} RGB;


ImU32 Color(RGB color, float alpha);

inline RGB green = { 0, 1, 0 };
inline RGB blue = { 0, 0, 1 };
inline RGB white = { 1, 1, 1 };
inline RGB black = { 0, 0, 0 };

namespace Render
{
    void Rect(float x, float y, float w, float h, RGB color, float alpha, float thickness);

    void RectFilled(float x, float y, float w, float h, RGB color, float alpha);

    void Circle(float x, float y, float radius, RGB color, float alpha, bool filled);

    void Text(float fontSize, float x, float y, RGB color, float alpha, std::string text);

    void Line(float x1, float y1, float x2, float y2, RGB color, float alpha, float thickness);

    void Triangle(ImVec2 p1, ImVec2 p2, ImVec2 p3, RGB color, float alpha, bool isFilled, int thickness);

    void Image(ImTextureID user_texture, ImVec2 p_min, ImVec2 p_max);
}

namespace RenderPrev
{
    void Rect(float x, float y, float w, float h, RGB color, float alpha, float thickness);

    void RectFilled(float x, float y, float w, float h, RGB color, float alpha);

    void Circle(float x, float y, float radius, RGB color, float alpha);

    void FilledCicrle(float x, float y, float radius, RGB color, float alpha);

    void Text(float fontSize, float x, float y, RGB color, float alpha, std::string text);

    void Line(float x1, float y1, float x2, float y2, RGB color, float alpha, float thickness);
}

class Overlay
{
public:
    HWND overlay;
    ImVec2 Pos; 
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    ImFont* font = NULL;
    ImFont* fontIcons = NULL;

    ID3D11Device* device{ nullptr };
    ID3D11DeviceContext* device_context{ nullptr };

    IDXGISwapChain* swap_chain{ nullptr };
    DXGI_SWAP_CHAIN_DESC sd{};

    UINT  g_ResizeWidth = 0, g_ResizeHeight = 0;
    ID3D11RenderTargetView* target_view{ nullptr };
    D3D_FEATURE_LEVEL level{};

    void CreateOverlay();
    bool CreateDevice();
    void CreateImGui();

    void StartRender();
    void EndRender();

    void DestroyOverlay();
    void DestroyDevice();
    void DestroyImGui();

    //from imgui wiki
    bool LoadTextureFromMemory(const unsigned char* buffer, size_t size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);

    bool RenderMenu = false; 

    WNDCLASSEXW wc{};
};
inline Overlay overlay;

class Menu
{
public:
    int tab, esptab;
    bool previewWindow, aimbotGrpah;

    void PushStyle();
    void TextCentered(std::string text);
    bool ButtonCenteredOnLine(const char* label, float alignment);
    void NewMenu();
    void EspMenu();
    void AimbotMenu();
    void MiscMenu();
    void ConfigMenu();
    void PreviewMenu();
    void AimbotGraph();
};
inline Menu menu;

class Debug
{
public:
    void ShowConsole();
    void HideConsole();
};
inline Debug debug;