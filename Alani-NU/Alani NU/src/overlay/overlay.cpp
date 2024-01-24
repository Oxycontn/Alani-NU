#pragma warning( disable : 4244 4267 4305 )

#include <Windows.h>
#include <dwmapi.h>
#include <windowsx.h>
#include <iostream>
#include "fstream"
#include <vector>

#include "../classes/global.hpp"
#include "overlay.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "..\..\ext\ImGui\stb_image.h"
#include "..\..\ext\ImGui\imgui_plot.hpp"
#include "..\..\ext\Images\playerModel.h"
#include "..\..\ext\Images\logo.h"
#include "..\..\ext\Font\font.h"
#include "..\..\ext\Font\fonticons.h"
#include "..\..\ext\Font\fonticonsdata.h"

ImU32 Color(RGB color, float alpha)
{
    return IM_COL32(color.R * 255, color.G * 255, color.B * 255, alpha);
}

namespace Render
{
    void Rect(float x, float y, float w, float h, RGB color, float alpha, float thickness)
    {
        ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), Color(color, alpha), 0, 0, thickness);
    }
    void RectFilled(float x, float y, float w, float h, RGB color, float alpha)
    {
        ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), Color(color, alpha), 0, 0);
    }
    void Circle(float x, float y, float radius, RGB color, float alpha, bool filled)
    {
        if (!filled)
            ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(x, y), radius, Color(color, alpha), 0, 1);
        else
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2(x, y), radius, Color(color, alpha));
    }
    void Text(float fontSize, float x, float y, RGB color, float alpha, std::string text)
    {
        ImGui::GetBackgroundDrawList()->AddText(NULL, fontSize ,ImVec2(x, y), Color(color, alpha), text.c_str());
    }
    void Line(float x1, float y1, float x2, float y2, RGB color, float alpha, float thickness)
    {
        ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), Color(color, alpha), thickness);
    }
    void Triangle(ImVec2 p1, ImVec2 p2, ImVec2 p3, RGB color, float alpha, bool isFilled, int thickness)
    {
        if (isFilled)
            ImGui::GetBackgroundDrawList()->AddTriangleFilled(p1, p2, p3, Color(color, alpha));
        else
            ImGui::GetBackgroundDrawList()->AddTriangle(p1, p2, p3, Color(color, alpha), thickness);
    }
    void Image(ImTextureID user_texture, ImVec2 p_min, ImVec2 p_max)
    {
        ImGui::GetBackgroundDrawList()->AddImage(user_texture, p_min, p_max);
    }
}

namespace RenderPrev
{
    void Rect(float x, float y, float w, float h, RGB color, float alpha, float thickness)
    {
        ImGui::GetWindowDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), Color(color, alpha), 0, 0, thickness);
    }
    void RectFilled(float x, float y, float w, float h, RGB color, float alpha)
    {
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), Color(color, alpha), 0, 0);
    }
    void Circle(float x, float y, float radius, RGB color, float alpha)
    {
        ImGui::GetWindowDrawList()->AddCircle(ImVec2(x, y), radius, Color(color, alpha), 0, 1);
    }
    void FilledCicrle(float x, float y, float radius, RGB color, float alpha)
    {
        ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(x, y), radius, Color(color, alpha));
    }
    void Text(float fontSize, float x, float y, RGB color, float alpha, std::string text)
    {
        ImGui::GetWindowDrawList()->AddText(NULL, fontSize, ImVec2(x, y), Color(color, alpha), text.c_str());
    }
    void Line(float x1, float y1, float x2, float y2, RGB color, float alpha, float thickness)
    {
        ImGui::GetWindowDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), Color(color, alpha), thickness);
    }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param) 
{
    if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param))
    {
        return 0L;
    }

    if (message == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0L;
    }

    return DefWindowProc(window, message, w_param, l_param);
}

void Overlay::CreateOverlay()
{
    WNDCLASSEXA wc{};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = window_procedure;
    wc.hInstance = GetModuleHandleA(0);
    wc.lpszClassName = "overlay";

    RegisterClassExA(&wc);

    overlay = CreateWindowExA(WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_LAYERED, wc.lpszClassName, "overlaywindow", WS_POPUP, 0, 0, screenWidth, screenHeight, nullptr, nullptr, wc.hInstance, nullptr);

    if (overlay == WM_NULL)
        printf("[OV]Failed to create Overlay\n");

    SetLayeredWindowAttributes(overlay, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

    {
        RECT client_area{};
        GetClientRect(overlay, &client_area);

        RECT window_area{};
        GetWindowRect(overlay, &window_area);

        POINT diff{};
        ClientToScreen(overlay, &diff);

        const MARGINS margins{
            window_area.left + (diff.x - window_area.left),
            window_area.top + (diff.y - window_area.top),
            client_area.right,
            client_area.bottom
        };

        DwmExtendFrameIntoClientArea(overlay, &margins);
    }

    printf("[OV]Created Overlay\n");
}

bool Overlay::CreateDevice()
{
    DXGI_SWAP_CHAIN_DESC sd{};
    ZeroMemory(&sd, sizeof(sd));

    sd.BufferDesc.RefreshRate.Numerator = 60U; // fps
    sd.BufferDesc.RefreshRate.Denominator = 1U;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.SampleDesc.Count = 1U;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2U;
    sd.OutputWindow = overlay;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    constexpr D3D_FEATURE_LEVEL levels[2]{
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };

    ID3D11RenderTargetView* render_target_view{ nullptr };
    D3D_FEATURE_LEVEL level{};

    //create device and that
    HRESULT d3d11Create = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0U, levels, 2U, D3D11_SDK_VERSION,
        &sd, &swap_chain, &device, &level,
        &device_context);

    if (d3d11Create == S_OK)
        printf("[OV]Created Device and Swap Chain\n");

    ID3D11Texture2D* back_buffer{ nullptr };
    swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

    if (back_buffer) {
        device->CreateRenderTargetView(back_buffer, nullptr, &target_view);
        back_buffer->Release();
        ShowWindow(overlay, TRUE);
        printf("[OV]Created Device\n");
        return true;
    }

    printf("[OV]Failed to create Device\n");
    return false;
}

void Overlay::CreateImGui()
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(overlay);
    ImGui_ImplDX11_Init(device, device_context);

    printf("[OV]Created ImGui\n");
}

void Overlay::StartRender()
{
    //set up font
    ImGuiIO& io = ImGui::GetIO();
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    font = io.Fonts->AddFontFromMemoryTTF(fontData, sizeof(fontData), 14.0f);
    fontIcons = io.Fonts->AddFontFromMemoryCompressedTTF(FA_compressed_data, FA_compressed_size, 16.0f, &icons_config, icons_ranges);

    MSG msg;
    while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Overlay::EndRender()
{
    ImGui::Render();

    float color[4]{ 0, 0, 0, 0 };
    device_context->OMSetRenderTargets(1U, &target_view, nullptr);
    device_context->ClearRenderTargetView(target_view, color);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    swap_chain->Present(1U, 0U);
}

void Overlay::DestroyOverlay()
{
    DestroyWindow(overlay);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    printf("[OV]Destroyed Overlay\n");
}

void Overlay::DestroyDevice()
{
    if (swap_chain) {
        swap_chain->Release();
    }

    if (device_context) {
        device_context->Release();
    }

    if (device) {
        device->Release();
    }

    if (target_view) {
        target_view->Release();
    }
    printf("[OV]Destroyed Device\n");
}

void Overlay::DestroyImGui()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();
    printf("[OV]Destroyed ImGui\n");
}

void Debug::ShowConsole()
{
    ShowWindow(GetConsoleWindow(), SW_SHOW);
}

void Debug::HideConsole()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);
}

//from imgui wiki
bool Overlay::LoadTextureFromMemory(const unsigned char* buffer, size_t size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
    // Load from disk into a raw RGBA buffer
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load_from_memory(buffer, size, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create texture
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    device->CreateTexture2D(&desc, &subResource, &pTexture);

    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    device->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();

    *out_width = image_width;
    *out_height = image_height;
    stbi_image_free(image_data);

    return true;
}

bool Menu::ButtonCenteredOnLine(const char* label, float alignment)
{
    ImGuiStyle& style = ImGui::GetStyle();

    float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

    return ImGui::Button(label);
}

void Menu::PushStyle()
{
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
    colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

    // Border
    colors[ImGuiCol_Border] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
    colors[ImGuiCol_BorderShadow] = ImVec4{ 0.0f, 0.0f, 0.0f, 0.24f };

    // Text
    colors[ImGuiCol_Text] = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    colors[ImGuiCol_TextDisabled] = ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = ImVec4{ 0.13f, 0.13f, 0.17, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{ 0.13f, 0.13f, 0.17, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_CheckMark] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };

    // Popups
    colors[ImGuiCol_PopupBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 0.92f };

    // Slider
    colors[ImGuiCol_SliderGrab] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.54f };
    colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.54f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.13f, 0.13, 0.17, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.24, 0.24f, 0.32f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.2f, 0.22f, 0.27f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
    colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f };

    // Seperator
    colors[ImGuiCol_Separator] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };
    colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };
    colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 1.0f };

    // Resize Grip
    colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
    colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.29f };
    colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 0.29f };

    // Plot
    colors[ImGuiCol_PlotLines] = ImVec4{ 1,1,1,1 };

    auto& style = ImGui::GetStyle();
    style.TabRounding = 4;
    style.ScrollbarRounding = 9;
    style.WindowRounding = 7;
    style.GrabRounding = 3;
    style.FrameRounding = 3;
    style.PopupRounding = 4;
    style.ChildRounding = 4;
}

void Menu::TextCentered(std::string text)
{
    float win_width = ImGui::GetWindowSize().x;
    float text_width = ImGui::CalcTextSize(text.c_str()).x;

    // calculate the indentation that centers the text on one line, relative
    // to window left, regardless of the `ImGuiStyleVar_WindowPadding` value
    float text_indentation = (win_width - text_width) * 0.5f;

    // if text is too long to be drawn on one line, `text_indentation` can
    // become too small or even negative, so we check a minimum indentation
    float min_indentation = 20.0f;
    if (text_indentation <= min_indentation) {
        text_indentation = min_indentation;
    }

    ImGui::SameLine(text_indentation);
    ImGui::PushTextWrapPos(win_width - text_indentation);
    ImGui::TextWrapped(text.c_str());
    ImGui::PopTextWrapPos();
}

void Menu::NewMenu()
{
    //set up image
    int logoWidth = 1;
    int logoHeight = 1;
    ID3D11ShaderResourceView* logoTexture = NULL;
    if (!logoTexture)
        overlay.LoadTextureFromMemory(logoData, sizeof(logoData), &logoTexture, &logoWidth, &logoHeight);

    //call config menu
    ConfigMenu();

    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize({200, 250});

    ImGui::Begin("menu", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

    PushStyle();

    if (ImGui::BeginChild("##menu", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar))
    {
        ImGui::Image((void*)logoTexture, ImVec2(logoWidth, logoHeight));

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        if (ImGui::Button(ICON_FA_EYE_LOW_VISION "  Visual", ImVec2(165, 40)))
            tab = 1;

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        if (ImGui::Button(ICON_FA_GUN "  Aimbot", ImVec2(165, 40)))
            tab = 2;

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        if (ImGui::Button(ICON_FA_CIRCLE_DOT "  Misc", ImVec2(165, 40)))
            tab = 3;

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        if (tab == 1)
            EspMenu();

        if (tab == 2)
            AimbotMenu();

        if (tab == 3)
            MiscMenu();
            
    }
    ImGui::EndChild();

    ImGui::End();
}

void Menu::ConfigMenu()
{
    ImGui::SetNextWindowPos({ 0, 255 });
    ImGui::SetNextWindowSize({ 200, 70 });

    ImGui::Begin("config", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

    PushStyle();

    if (ImGui::BeginChild("##configback", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar))
    {
        if (ImGui::Button("Save", ImVec2(79, 40)))
        {
            std::fstream fs;

            //create a file to save config, will overwrite any config.txt file.
            fs.open("config.txt");

            //create config file if file isnt open!
            if (!fs.is_open())
            {
                fs.open("config.txt", std::fstream::in | std::fstream::out | std::fstream::trunc);
                fs.close();
            }
            //now write settings to config file

            //esp

            //frame
            fs << global.features.teamenable << std::endl;
            fs << global.features.teamcombo << std::endl;
            fs << global.features.teamalpha << std::endl;
            //bone
            fs << global.features.teamskel << std::endl;
            fs << global.features.teamJoint << std::endl;
            fs << global.features.teamBoneAlpha << std::endl;
            //head
            fs << global.features.teamhead << std::endl;
            fs << global.features.teamHeadAlpha << std::endl;
            //snap
            fs << global.features.teamsnap << std::endl;
            fs << global.features.teamSnapAlpha << std::endl;
            //misc
            fs << global.features.teamhealth << std::endl;
            fs << global.features.teamarmor << std::endl;
            fs << global.features.teamweapon << std::endl;
            fs << global.features.teamname << std::endl;

            //frame
            fs << global.features.enemyenable << std::endl;
            fs << global.features.enemycombo << std::endl;
            fs << global.features.enemyalpha << std::endl;
            //bone
            fs << global.features.enemyskel << std::endl;
            fs << global.features.enemyJoint << std::endl;
            fs << global.features.enemyBoneAlpha << std::endl;
            //head
            fs << global.features.enemyhead << std::endl;
            fs << global.features.enemyBoneAlpha << std::endl;
            //snap
            fs << global.features.enemysnap << std::endl;
            fs << global.features.enemySnapAlpha << std::endl;
            //misc
            fs << global.features.enemyhealth << std::endl;
            fs << global.features.enemyarmor << std::endl;
            fs << global.features.enemyweapon << std::endl;
            fs << global.features.enemyname << std::endl;

            //world

            fs << global.features.weapon << std::endl;
            fs << global.features.molotov << std::endl;
            fs << global.features.incgrenade << std::endl;
            fs << global.features.smoke << std::endl;
            fs << global.features.flash << std::endl;
            fs << global.features.grenade << std::endl;
            fs << global.features.decoy << std::endl;
            fs << global.features.c4 << std::endl;

            //aimbot

            //AR
            fs << global.features.ARaimbotenable << std::endl;
            fs << global.features.ARaimbotautoshot << std::endl;
            fs << global.features.ARaimbotvisable << std::endl;
            fs << global.features.ARaimbotcombobone << std::endl;
            fs << global.features.ARaimbotcombokey << std::endl;
            fs << global.features.ARaimbotfov << std::endl;
            fs << global.features.ARaimbotsmooth << std::endl;
            fs << global.features.ARaimbotfovcircle << std::endl;
            fs << global.features.ARaimbotdistance << std::endl;

            //Shotguns
            fs << global.features.SGaimbotenable << std::endl;
            fs << global.features.SGaimbotautoshot << std::endl;
            fs << global.features.SGaimbotvisable << std::endl;
            fs << global.features.SGaimbotcombobone << std::endl;
            fs << global.features.SGaimbotcombokey << std::endl;
            fs << global.features.SGaimbotfov << std::endl;
            fs << global.features.SGaimbotsmooth << std::endl;
            fs << global.features.SGaimbotfovcircle << std::endl;
            fs << global.features.SGaimbotdistance << std::endl;

            //Pistols
            fs << global.features.PSaimbotenable << std::endl;
            fs << global.features.PSaimbotautoshot << std::endl;
            fs << global.features.PSaimbotvisable << std::endl;
            fs << global.features.PSaimbotcombobone << std::endl;
            fs << global.features.PSaimbotcombokey << std::endl;
            fs << global.features.PSaimbotfov << std::endl;
            fs << global.features.PSaimbotsmooth << std::endl;
            fs << global.features.PSaimbotfovcircle << std::endl;
            fs << global.features.PSaimbotdistance << std::endl;

            //Snipers
            fs << global.features.SRaimbotenable << std::endl;
            fs << global.features.SRaimbotautoshot << std::endl;
            fs << global.features.SRaimbotvisable << std::endl;
            fs << global.features.SRaimbotcombobone << std::endl;
            fs << global.features.SRaimbotcombokey << std::endl;
            fs << global.features.SRaimbotfov << std::endl;
            fs << global.features.SRaimbotsmooth << std::endl;
            fs << global.features.SRaimbotfovcircle << std::endl;
            fs << global.features.SRaimbotdistance << std::endl;

            //SMG
            fs << global.features.SMGaimbotenable << std::endl;
            fs << global.features.SMGaimbotautoshot << std::endl;
            fs << global.features.SMGaimbotvisable << std::endl;
            fs << global.features.SMGaimbotcombobone << std::endl;
            fs << global.features.SMGaimbotcombokey << std::endl;
            fs << global.features.SMGaimbotfov << std::endl;
            fs << global.features.SMGaimbotsmooth << std::endl;
            fs << global.features.SMGaimbotfovcircle << std::endl;
            fs << global.features.SMGaimbotdistance << std::endl;

            //rcs
            fs << global.features.rcsenable << std::endl;
            fs << global.features.rcsscaleX << std::endl;
            fs << global.features.rcsscaleY << std::endl;

            //misc
        }

        ImGui::SameLine();

        if (ImGui::Button("Load", ImVec2(79, 40)))
        {
            std::ifstream fs("config.txt");

            if (fs.is_open())
            {
                //esp
                
                //frame
                fs >> global.features.teamenable;
                fs >> global.features.teamcombo;
                fs >> global.features.teamalpha;
                //bone
                fs >> global.features.teamskel;
                fs >> global.features.teamJoint;
                fs >> global.features.teamBoneAlpha;
                //head
                fs >> global.features.teamhead;
                fs >> global.features.teamHeadAlpha;
                //snap
                fs >> global.features.teamsnap;
                fs >> global.features.teamSnapAlpha;
                //misc
                fs >> global.features.teamhealth;
                fs >> global.features.teamarmor;
                fs >> global.features.teamweapon;
                fs >> global.features.teamname;

                //frame
                fs >> global.features.enemyenable;
                fs >> global.features.enemycombo;
                fs >> global.features.enemyalpha;
                //bone
                fs >> global.features.enemyskel;
                fs >> global.features.enemyJoint;
                fs >> global.features.enemyBoneAlpha;
                //head
                fs >> global.features.enemyhead;
                fs >> global.features.enemyBoneAlpha;
                //snap
                fs >> global.features.enemysnap;
                fs >> global.features.enemySnapAlpha;
                //misc
                fs >> global.features.enemyhealth;
                fs >> global.features.enemyarmor;
                fs >> global.features.enemyweapon;
                fs >> global.features.enemyname;

                //world

                fs >> global.features.weapon;
                fs >> global.features.molotov;
                fs >> global.features.incgrenade;
                fs >> global.features.smoke;
                fs >> global.features.flash;
                fs >> global.features.grenade;
                fs >> global.features.decoy;
                fs >> global.features.c4;

                //aimbot

                //AR
                fs >> global.features.ARaimbotenable;
                fs >> global.features.ARaimbotautoshot;
                fs >> global.features.ARaimbotvisable;
                fs >> global.features.ARaimbotcombobone;
                fs >> global.features.ARaimbotcombokey;
                fs >> global.features.ARaimbotfov;
                fs >> global.features.ARaimbotsmooth;
                fs >> global.features.ARaimbotfovcircle;
                fs >> global.features.ARaimbotdistance;

                //Shotguns
                fs >> global.features.SGaimbotenable;
                fs >> global.features.SGaimbotautoshot;
                fs >> global.features.SGaimbotvisable;
                fs >> global.features.SGaimbotcombobone;
                fs >> global.features.SGaimbotcombokey;
                fs >> global.features.SGaimbotfov;
                fs >> global.features.SGaimbotsmooth;
                fs >> global.features.SGaimbotfovcircle;
                fs >> global.features.SGaimbotdistance;

                //Pistols
                fs >> global.features.PSaimbotenable;
                fs >> global.features.PSaimbotautoshot;
                fs >> global.features.PSaimbotvisable;
                fs >> global.features.PSaimbotcombobone;
                fs >> global.features.PSaimbotcombokey;
                fs >> global.features.PSaimbotfov;
                fs >> global.features.PSaimbotsmooth;
                fs >> global.features.PSaimbotfovcircle;
                fs >> global.features.PSaimbotdistance;

                //Snipers
                fs >> global.features.SRaimbotenable;
                fs >> global.features.SRaimbotautoshot;
                fs >> global.features.SRaimbotvisable;
                fs >> global.features.SRaimbotcombobone;
                fs >> global.features.SRaimbotcombokey;
                fs >> global.features.SRaimbotfov;
                fs >> global.features.SRaimbotsmooth;
                fs >> global.features.SRaimbotfovcircle;
                fs >> global.features.SRaimbotdistance;

                //SMG
                fs >> global.features.SMGaimbotenable;
                fs >> global.features.SMGaimbotautoshot;
                fs >> global.features.SMGaimbotvisable;
                fs >> global.features.SMGaimbotcombobone;
                fs >> global.features.SMGaimbotcombokey;
                fs >> global.features.SMGaimbotfov;
                fs >> global.features.SMGaimbotsmooth;
                fs >> global.features.SMGaimbotfovcircle;
                fs >> global.features.SMGaimbotdistance;

                //rcs
                fs >> global.features.rcsenable;
                fs >> global.features.rcsscaleX;
                fs >> global.features.rcsscaleY;

                //misc

                fs.close();
            }
        }
    }
    ImGui::EndChild();

    ImGui::End();

}

void Menu::EspMenu()
{
    ImGui::SetNextWindowPos({ 220, 0 });
    ImGui::SetNextWindowSize({ 240, 675 });

    ImGui::Begin("espmenu", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

    PushStyle();

    if (ImGui::BeginChild("##eps1", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar))
    {
        TextCentered("Visuals");

        if (ImGui::BeginChild("##settings", ImVec2(205, 600), true, ImGuiWindowFlags_NoScrollbar))
        {
            if (ImGui::BeginTabBar("##settings", NULL))
            {
                if (ImGui::BeginTabItem("  Team  ", NULL, NULL))
                {
                    esptab = 1;

                    ImGui::NewLine();
                    TextCentered("Frame");
                    if (ImGui::BeginChild("##teamframe", ImVec2(190, 90), true, ImGuiWindowFlags_NoScrollbar))
                    {
                        ImGui::Checkbox("enable", &global.features.teamenable);
                        ImGui::SameLine();
                        ImGui::ColorEdit3("##teamframecolor", (float*)&global.features.teaamcolor, ImGuiColorEditFlags_NoInputs);
                        ImGui::Combo("style", &global.features.teamcombo, global.features.comboSelections, 4);
                        if(global.features.teamcombo == 1)
                            ImGui::SliderFloat("alpha", &global.features.teamalpha, 0, 100);
                        else if(global.features.teamcombo == 3)
                            ImGui::SliderFloat("alpha", &global.features.teamalpha, 0, 255);
                    }
                    ImGui::EndChild();

                    ImGui::NewLine();
                    TextCentered("Bones");
                    if (ImGui::BeginChild("##teambones", ImVec2(190, 90), true, ImGuiWindowFlags_NoScrollbar))
                    {
                        ImGui::Checkbox("enable", &global.features.teamskel);
                        ImGui::SameLine();
                        ImGui::ColorEdit3("##teambonescolor", (float*)&global.features.teamBoneColor, ImGuiColorEditFlags_NoInputs);
                        ImGui::Checkbox("joint circles", &global.features.teamJoint);
                        ImGui::SliderFloat("alpha", &global.features.teamBoneAlpha, 0, 255);
                    }
                    ImGui::EndChild();

                    ImGui::NewLine();
                    TextCentered("Head");
                    if (ImGui::BeginChild("##teamhead", ImVec2(190, 70), true, ImGuiWindowFlags_NoScrollbar))
                    {
                        ImGui::Checkbox("enable", &global.features.teamhead);
                        ImGui::SameLine();
                        ImGui::ColorEdit3("##teamheadcolor", (float*)&global.features.teamHeaadColor, ImGuiColorEditFlags_NoInputs);
                        ImGui::SliderFloat("alpha", &global.features.teamHeadAlpha, 0, 255);
                    }
                    ImGui::EndChild();

                    ImGui::NewLine();
                    TextCentered("Snap Lines");
                    if (ImGui::BeginChild("##teamsnap", ImVec2(190, 70), true, ImGuiWindowFlags_NoScrollbar))
                    {
                        ImGui::Checkbox("enable", &global.features.teamsnap);
                        ImGui::SameLine();
                        ImGui::ColorEdit3("##teamsnapcolor", (float*)&global.features.teamSnapColor, ImGuiColorEditFlags_NoInputs);
                        ImGui::SliderFloat("alpha", &global.features.teamSnapAlpha, 0, 255);
                    }
                    ImGui::EndChild();

                    ImGui::NewLine();
                    TextCentered("Misc");
                    if(ImGui::BeginChild("##teammisc", ImVec2(190, 135), false, ImGuiWindowFlags_NoScrollbar))
                    {
                        ImGui::Checkbox("health", &global.features.teamhealth);

                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();

                        ImGui::Checkbox("armor", &global.features.teamarmor);

                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();

                        ImGui::Checkbox("weapon", &global.features.teamweapon);

                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();

                        ImGui::Checkbox("name", &global.features.teamname);
                    }
                    ImGui::EndChild();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("  Enemy  ", NULL, NULL))
                {
                    esptab = 2;

                    ImGui::NewLine();
                    TextCentered("Frame");
                    if (ImGui::BeginChild("##enemyframe", ImVec2(190, 90), true, ImGuiWindowFlags_NoScrollbar))
                    {
                        ImGui::Checkbox("enable", &global.features.enemyenable);
                        ImGui::SameLine();
                        ImGui::ColorEdit3("##enemyframecolor", (float*)&global.features.enemycolor, ImGuiColorEditFlags_NoInputs);
                        ImGui::Combo("style", &global.features.enemycombo, global.features.comboSelections, 4);
                        if (global.features.enemycombo == 1)
                            ImGui::SliderFloat("alpha", &global.features.enemyalpha, 0, 100);
                        else if (global.features.enemycombo == 3)
                            ImGui::SliderFloat("alpha", &global.features.enemyalpha, 0, 255);
                    }
                    ImGui::EndChild();

                    ImGui::NewLine();
                    TextCentered("Bones");
                    if (ImGui::BeginChild("##enemybones", ImVec2(190, 90), true, ImGuiWindowFlags_NoScrollbar))
                    {
                        ImGui::Checkbox("enable", &global.features.enemyskel);
                        ImGui::SameLine();
                        ImGui::ColorEdit3("##enemybonescolor", (float*)&global.features.enemyBoneColor, ImGuiColorEditFlags_NoInputs);
                        ImGui::Checkbox("joint circles", &global.features.enemyJoint);
                        ImGui::SliderFloat("alpha", &global.features.enemyBoneAlpha, 0, 255);
                    }
                    ImGui::EndChild();

                    ImGui::NewLine();
                    TextCentered("Head");
                    if (ImGui::BeginChild("##enemyhead", ImVec2(190, 70), true, ImGuiWindowFlags_NoScrollbar))
                    {
                        ImGui::Checkbox("enable", &global.features.enemyhead);
                        ImGui::SameLine();
                        ImGui::ColorEdit3("##teamheadcolor", (float*)&global.features.enemyHeaadColor, ImGuiColorEditFlags_NoInputs);
                        ImGui::SliderFloat("alpha", &global.features.enemyHeadAlpha, 0, 255);
                    }
                    ImGui::EndChild();

                    ImGui::NewLine();
                    TextCentered("Snap Lines");
                    if (ImGui::BeginChild("##enemysnap", ImVec2(190, 70), true, ImGuiWindowFlags_NoScrollbar))
                    {
                        ImGui::Checkbox("enable", &global.features.enemysnap);
                        ImGui::SameLine();
                        ImGui::ColorEdit3("##teamsnapcolor", (float*)&global.features.enemySnapColor, ImGuiColorEditFlags_NoInputs);
                        ImGui::SliderFloat("alpha", &global.features.enemySnapAlpha, 0, 255);
                    }
                    ImGui::EndChild();

                    ImGui::NewLine();
                    TextCentered("Misc");
                    if (ImGui::BeginChild("##enemymisc", ImVec2(190, 135), false, ImGuiWindowFlags_NoScrollbar))
                    {
                        ImGui::Checkbox("health", &global.features.enemyhealth);

                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();

                        ImGui::Checkbox("armor", &global.features.enemyarmor);

                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();

                        ImGui::Checkbox("weapon", &global.features.enemyweapon);

                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();
                        ImGui::Spacing();

                        ImGui::Checkbox("name", &global.features.enemyname);
                    }
                    ImGui::EndChild();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("  World  ", NULL, NULL))
                {
                    ImGui::NewLine();
                    TextCentered("Dropped Entites");

                    ImGui::Checkbox("weapons", &global.features.weapon);

                    ImGui::Spacing();
                    ImGui::Spacing();
                    ImGui::Spacing();

                    ImGui::Checkbox("molotov", &global.features.molotov);

                    ImGui::Spacing();
                    ImGui::Spacing();
                    ImGui::Spacing();

                    ImGui::Checkbox("inc grende", &global.features.incgrenade);

                    ImGui::Spacing();
                    ImGui::Spacing();
                    ImGui::Spacing();

                    ImGui::Checkbox("smoke", &global.features.smoke);

                    ImGui::Spacing();
                    ImGui::Spacing();
                    ImGui::Spacing();

                    ImGui::Checkbox("flash", &global.features.flash);

                    ImGui::Spacing();
                    ImGui::Spacing();
                    ImGui::Spacing();

                    ImGui::Checkbox("grenade", &global.features.grenade);

                    ImGui::Spacing();
                    ImGui::Spacing();
                    ImGui::Spacing();

                    ImGui::Checkbox("decoy", &global.features.decoy);

                    ImGui::Spacing();
                    ImGui::Spacing();
                    ImGui::Spacing();

                    ImGui::Checkbox("c4", &global.features.c4);

                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
        ImGui::EndChild();

        if (ButtonCenteredOnLine("Preview", 0.5f))
            previewWindow = !previewWindow;

        if (previewWindow)
            PreviewMenu();
    }
    ImGui::EndChild();

    ImGui::End();
}

void Menu::PreviewMenu()
{
    //set up image
    int csWidth = 1;
    int csHeight = 1;
    ID3D11ShaderResourceView* csTexture = NULL;
    if(!csTexture)
        overlay.LoadTextureFromMemory(rawData, sizeof(rawData), &csTexture, &csWidth, &csHeight);

    ImGui::SetNextWindowPos({ 480, 0 });
    ImGui::SetNextWindowSize({ 415, 605 });

    ImGui::Begin("previewmenu", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

    PushStyle();

    if (ImGui::BeginChild("##eps1", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar))
    {
        TextCentered("Preview");
        ImGui::Image((void*)csTexture, ImVec2(csWidth, csHeight));

        //now we do a check for each feture then display it on screen lul
        
        //team
        if (esptab == 1)
        {
            //2d boxes
            if (global.features.teamenable && global.features.teamcombo == 1)
            {
                RenderPrev::Rect(550, 50, 250, 520, white, 255, 1.5);
                RenderPrev::RectFilled(550, 50, 250, 520, global.features.teaamcolor, global.features.teamalpha);
            }

            //corners
            if (global.features.teamenable && global.features.teamcombo == 3)
            {
                RenderPrev::Line(550, 50, 600, 50, global.features.teaamcolor, global.features.teamalpha, 1.5);
                RenderPrev::Line(550, 50, 550, 100, global.features.teaamcolor, global.features.teamalpha, 1.5);

                RenderPrev::Line(747, 50, 797, 50, global.features.teaamcolor, global.features.teamalpha, 1.5);
                RenderPrev::Line(797, 50, 797, 100, global.features.teaamcolor, global.features.teamalpha, 1.5);

                RenderPrev::Line(550, 570, 600, 570, global.features.teaamcolor, global.features.teamalpha, 1.5);
                RenderPrev::Line(550, 520, 550, 570, global.features.teaamcolor, global.features.teamalpha, 1.5);

                RenderPrev::Line(747, 570, 797, 570, global.features.teaamcolor, global.features.teamalpha, 1.5);
                RenderPrev::Line(797, 520, 797, 570, global.features.teaamcolor, global.features.teamalpha, 1.5);
            }

            //bone
            if (global.features.teamskel)
            {
                //spine
                RenderPrev::Line(680, 140, 680, 300, global.features.teamBoneColor, global.features.teamBoneAlpha, 1.5);

                //left shoulder
                RenderPrev::Line(680, 160, 630, 160, global.features.teamBoneColor, global.features.teamBoneAlpha, 1.5);
                //right shoulder
                RenderPrev::Line(680, 160, 730, 160, global.features.teamBoneColor, global.features.teamBoneAlpha, 1.5);

                //left arm
                RenderPrev::Line(630, 160, 590, 220, global.features.teamBoneColor, global.features.teamBoneAlpha, 1.5);
                //right arm
                RenderPrev::Line(730, 160, 750, 220, global.features.teamBoneColor, global.features.teamBoneAlpha, 1.5);

                //left forearm
                RenderPrev::Line(590, 220, 650, 240, global.features.teamBoneColor, global.features.teamBoneAlpha, 1.5);
                //right forearm
                RenderPrev::Line(750, 220, 750, 260, global.features.teamBoneColor, global.features.teamBoneAlpha, 1.5);

                //left torso
                RenderPrev::Line(680, 300, 650, 340, global.features.teamBoneColor, global.features.teamBoneAlpha, 1.5);
                //right torso
                RenderPrev::Line(680, 300, 730, 350, global.features.teamBoneColor, global.features.teamBoneAlpha, 1.5);

                //left leg
                RenderPrev::Line(650, 340, 650, 410, global.features.teamBoneColor, global.features.teamBoneAlpha, 1.5);
                //right leg
                RenderPrev::Line(730, 350, 740, 410, global.features.teamBoneColor, global.features.teamBoneAlpha, 1.5);

                //left foot
                RenderPrev::Line(650, 410, 660, 530, global.features.teamBoneColor, global.features.teamBoneAlpha, 1.5);
                //right foot
                RenderPrev::Line(740, 410, 750, 530, global.features.teamBoneColor, global.features.teamBoneAlpha, 1.5);
            }

            //joints
            if (global.features.teamJoint)
            {
                //spine
                RenderPrev::FilledCicrle(680, 160, 4.5, white, 255);

                //left shoulder
                RenderPrev::FilledCicrle(630, 161, 4.5, white, 255);
                //right shoulder
                RenderPrev::FilledCicrle(730, 161, 4.5, white, 255);

                //left arm
                RenderPrev::FilledCicrle(590, 220, 4.5, white, 255);
                //right arm
                RenderPrev::FilledCicrle(750, 220, 4.5, white, 255);

                //dick
                RenderPrev::FilledCicrle(680, 302, 4.5, white, 255);

                //left torso
                RenderPrev::FilledCicrle(650, 340, 4.5, white, 255);
                //right torso
                RenderPrev::FilledCicrle(730, 350, 4.5, white, 255);

                //left knee
                RenderPrev::FilledCicrle(650, 410, 4.5, white, 255);
                //right knee
                RenderPrev::FilledCicrle(740, 410, 4.5, white, 255);

                //left foot
                RenderPrev::FilledCicrle(660, 530, 4.5, white, 255);
                //right foot
                RenderPrev::FilledCicrle(750, 530, 4.5, white, 255);
            }

            //head
            if (global.features.teamhead)
                RenderPrev::Circle(670, 100, 35, global.features.teamHeaadColor, global.features.teamHeadAlpha);

            //health bar
            if (global.features.teamhealth)
            {
                RenderPrev::RectFilled(535, 50, 13, 520, white, 255);
                RenderPrev::RectFilled(537, 52, 10, 516, green, 200);
            }

            //armor bar
            if (global.features.teamarmor)
            {
                RenderPrev::RectFilled(802, 50, 13, 520, white, 255);
                RenderPrev::RectFilled(804, 52, 10, 516, blue, 200);
            }

            //weapon name
            if (global.features.teamweapon)
                RenderPrev::Text(15.0f, 550, 570, white, 255, std::string("Weapon Name"));

            //player name
            if (global.features.teamname)
                RenderPrev::Text(15.0f, 550, 35, white, 255, std::string("Player Name"));
        }

        //enemy
        if (esptab == 2)
        {
            //2d boxes
            if (global.features.enemyenable && global.features.enemycombo == 1)
            {
                RenderPrev::Rect(550, 50, 250, 520, white, 255, 1.5);
                RenderPrev::RectFilled(550, 50, 250, 520, global.features.enemycolor, global.features.enemyalpha);
            }

            //corners
            if (global.features.enemyenable && global.features.enemycombo == 3)
            {
                RenderPrev::Line(550, 50, 600, 50, global.features.enemycolor, global.features.enemyalpha, 1.5);
                RenderPrev::Line(550, 50, 550, 100, global.features.enemycolor, global.features.enemyalpha, 1.5);

                RenderPrev::Line(747, 50, 797, 50, global.features.enemycolor, global.features.enemyalpha, 1.5);
                RenderPrev::Line(797, 50, 797, 100, global.features.enemycolor, global.features.enemyalpha, 1.5);

                RenderPrev::Line(550, 570, 600, 570, global.features.enemycolor, global.features.enemyalpha, 1.5);
                RenderPrev::Line(550, 520, 550, 570, global.features.enemycolor, global.features.enemyalpha, 1.5);

                RenderPrev::Line(747, 570, 797, 570, global.features.enemycolor, global.features.enemyalpha, 1.5);
                RenderPrev::Line(797, 520, 797, 570, global.features.enemycolor, global.features.enemyalpha, 1.5);
            }

            //bone
            if (global.features.enemyskel)
            {
                //spine
                RenderPrev::Line(680, 140, 680, 300, global.features.enemyBoneColor, global.features.enemyBoneAlpha, 1.5);

                //left shoulder
                RenderPrev::Line(680, 160, 630, 160, global.features.enemyBoneColor, global.features.enemyBoneAlpha, 1.5);
                //right shoulder
                RenderPrev::Line(680, 160, 730, 160, global.features.enemyBoneColor, global.features.enemyBoneAlpha, 1.5);

                //left arm
                RenderPrev::Line(630, 160, 590, 220, global.features.enemyBoneColor, global.features.enemyBoneAlpha, 1.5);
                //right arm
                RenderPrev::Line(730, 160, 750, 220, global.features.enemyBoneColor, global.features.enemyBoneAlpha, 1.5);

                //left forearm
                RenderPrev::Line(590, 220, 650, 240, global.features.enemyBoneColor, global.features.enemyBoneAlpha, 1.5);
                //right forearm
                RenderPrev::Line(750, 220, 750, 260, global.features.enemyBoneColor, global.features.enemyBoneAlpha, 1.5);

                //left torso
                RenderPrev::Line(680, 300, 650, 340, global.features.enemyBoneColor, global.features.enemyBoneAlpha, 1.5);
                //right torso
                RenderPrev::Line(680, 300, 730, 350, global.features.enemyBoneColor, global.features.enemyBoneAlpha, 1.5);

                //left leg
                RenderPrev::Line(650, 340, 650, 410, global.features.enemyBoneColor, global.features.enemyBoneAlpha, 1.5);
                //right leg
                RenderPrev::Line(730, 350, 740, 410, global.features.enemyBoneColor, global.features.enemyBoneAlpha, 1.5);

                //left foot
                RenderPrev::Line(650, 410, 660, 530, global.features.enemyBoneColor, global.features.enemyBoneAlpha, 1.5);
                //right foot
                RenderPrev::Line(740, 410, 750, 530, global.features.enemyBoneColor, global.features.enemyBoneAlpha, 1.5);
            }

            //joints
            if (global.features.enemyJoint)
            {
                //spine
                RenderPrev::FilledCicrle(680, 160, 4.5, white, 255);

                //left shoulder
                RenderPrev::FilledCicrle(630, 161, 4.5, white, 255);
                //right shoulder
                RenderPrev::FilledCicrle(730, 161, 4.5, white, 255);

                //left arm
                RenderPrev::FilledCicrle(590, 220, 4.5, white, 255);
                //right arm
                RenderPrev::FilledCicrle(750, 220, 4.5, white, 255);

                //dick
                RenderPrev::FilledCicrle(680, 302, 4.5, white, 255);

                //left torso
                RenderPrev::FilledCicrle(650, 340, 4.5, white, 255);
                //right torso
                RenderPrev::FilledCicrle(730, 350, 4.5, white, 255);

                //left knee
                RenderPrev::FilledCicrle(650, 410, 4.5, white, 255);
                //right knee
                RenderPrev::FilledCicrle(740, 410, 4.5, white, 255);

                //left foot
                RenderPrev::FilledCicrle(660, 530, 4.5, white, 255);
                //right foot
                RenderPrev::FilledCicrle(750, 530, 4.5, white, 255);
            }

            //head
            if (global.features.enemyhead)
                RenderPrev::Circle(670, 100, 35, global.features.enemyHeaadColor, global.features.enemyHeadAlpha);

            //health bar
            if (global.features.enemyhealth)
            {
                RenderPrev::RectFilled(535, 50, 13, 520, white, 255);
                RenderPrev::RectFilled(537, 52, 10, 516, green, 200);
            }

            //armor bar
            if (global.features.enemyarmor)
            {
                RenderPrev::RectFilled(802, 50, 13, 520, white, 255);
                RenderPrev::RectFilled(804, 52, 10, 516, blue, 200);
            }

            //weapon name
            if (global.features.enemyweapon)
                RenderPrev::Text(15.0f, 550, 570, white, 255, std::string("Weapon Name"));

            //player name
            if (global.features.enemyname)
                RenderPrev::Text(15.0f, 550, 35, white, 255, std::string("Player Name"));
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void Menu::AimbotMenu()
{
    ImGui::SetNextWindowPos({ 220, 0 });
    ImGui::SetNextWindowSize({ 260, 620 });

    ImGui::Begin("aimbotmenu", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

    PushStyle();

    if (ImGui::BeginChild("##background", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar))
    {
        TextCentered("Aimbot");

        ImGui::SetNextItemWidth(225);
        ImGui::Combo("##Guns", &global.features.aimbotGunCombo, global.features.aimbotGunSelection, 5);

        if (ImGui::BeginChild("##aimbotAR", ImVec2(225, 370), true, ImGuiWindowFlags_NoScrollbar))
        {
            //AR
            if (global.features.aimbotGunCombo == 0)
            {
                TextCentered("Assault Riffles");

                ImGui::Checkbox("enable", &global.features.ARaimbotenable);
                ImGui::SameLine();
                ButtonCenteredOnLine(ICON_FA_QUESTION "##tooltip1", 2.f);
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                {
                    ImGui::SetTooltip("Autoshot : Aimbot will automatically shoot when on the selected bone.\nVisable : Aimbot will only activate on visable players\nFov Circle : When a player is inside the circle, aimbot will target them\nBone : Select what bone to target for aimbot\nHotKey: Select what key to press to activate aimbot\nFOV : Size of the FOV circle\nSmooth : The movement of the aimbot. Higher the value the faster it will move, lower the value slower the slower it will move.\nDistance : Max distance to a player for the aimbot to see them.");
                }
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::SliderFloat("smooth", &global.features.ARaimbotsmooth, 0.3, 1.1);
                ImGui::SliderInt("distance", &global.features.ARaimbotdistance, 100, 2000);
                ImGui::Checkbox("autoshot", &global.features.ARaimbotautoshot);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::SliderInt("delay ms", &global.features.ARautosleep, 0, 2000);
                ImGui::Checkbox("visable", &global.features.ARaimbotvisable);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Checkbox("fov circle", &global.features.ARaimbotfovcircle);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::SliderFloat("size", &global.features.ARaimbotfov, 1, 80);
                ImGui::Combo("bone", &global.features.ARaimbotcombobone, global.features.aimbotBoneSelections, 4);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Combo("hotkey", &global.features.ARaimbotcombokey, global.features.aimbotKeySelections, 4);
            }

            //Shotguns
            if (global.features.aimbotGunCombo == 1)
            {
                TextCentered("Shotguns");

                ImGui::Checkbox("enable", &global.features.SGaimbotenable);
                ImGui::SameLine();
                ButtonCenteredOnLine(ICON_FA_QUESTION "##tooltip1", 2.f);
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                {
                    ImGui::SetTooltip("Autoshot : Aimbot will automatically shoot when on the selected bone.\nVisable : Aimbot will only activate on visable players\nFov Circle : When a player is inside the circle, aimbot will target them\nBone : Select what bone to target for aimbot\nHotKey: Select what key to press to activate aimbot\nFOV : Size of the FOV circle\nSmooth : The movement of the aimbot. Higher the value the faster it will move, lower the value slower the slower it will move.\nDistance : Max distance to a player for the aimbot to see them.");
                }
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::SliderFloat("smooth", &global.features.SGaimbotsmooth, 0.3, 1.1);
                ImGui::SliderInt("distance", &global.features.SGaimbotdistance, 100, 2000);
                ImGui::Checkbox("autoshot", &global.features.SGaimbotautoshot);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::SliderInt("delay ms", &global.features.SGautosleep, 0, 2000);
                ImGui::Checkbox("visable", &global.features.SGaimbotvisable);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Checkbox("fov circle", &global.features.SGaimbotfovcircle);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::SliderFloat("size", &global.features.SGaimbotfov, 1, 80);
                ImGui::Combo("bone", &global.features.SGaimbotcombobone, global.features.aimbotBoneSelections, 4);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Combo("hotkey", &global.features.SGaimbotcombokey, global.features.aimbotKeySelections, 4);
            }

            //Pistols
            if (global.features.aimbotGunCombo == 2)
            {
                TextCentered("Pistols");

                ImGui::Checkbox("enable", &global.features.PSaimbotenable);
                ImGui::SameLine();
                ButtonCenteredOnLine(ICON_FA_QUESTION "##tooltip1", 2.f);
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                {
                    ImGui::SetTooltip("Autoshot : Aimbot will automatically shoot when on the selected bone.\nVisable : Aimbot will only activate on visable players\nFov Circle : When a player is inside the circle, aimbot will target them\nBone : Select what bone to target for aimbot\nHotKey: Select what key to press to activate aimbot\nFOV : Size of the FOV circle\nSmooth : The movement of the aimbot. Higher the value the faster it will move, lower the value slower the slower it will move.\nDistance : Max distance to a player for the aimbot to see them.");
                }
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::SliderFloat("smooth", &global.features.PSaimbotsmooth, 0.3, 1.1);
                ImGui::SliderInt("distance", &global.features.PSaimbotdistance, 100, 2000);
                ImGui::Checkbox("autoshot", &global.features.PSaimbotautoshot);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::SliderInt("delay ms", &global.features.PSautosleep, 0, 2000);
                ImGui::Checkbox("visable", &global.features.PSaimbotvisable);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Checkbox("fov circle", &global.features.PSaimbotfovcircle);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::SliderFloat("size", &global.features.PSaimbotfov, 1, 80);
                ImGui::Combo("bone", &global.features.PSaimbotcombobone, global.features.aimbotBoneSelections, 4);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Combo("hotkey", &global.features.PSaimbotcombokey, global.features.aimbotKeySelections, 4);
            }

            //Snipers
            if (global.features.aimbotGunCombo == 3)
            {
                TextCentered("Snipers");

                ImGui::Checkbox("enable", &global.features.SRaimbotenable);
                ImGui::SameLine();
                ButtonCenteredOnLine(ICON_FA_QUESTION "##tooltip1", 2.f);
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                {
                    ImGui::SetTooltip("Autoshot : Aimbot will automatically shoot when on the selected bone.\nVisable : Aimbot will only activate on visable players\nFov Circle : When a player is inside the circle, aimbot will target them\nBone : Select what bone to target for aimbot\nHotKey: Select what key to press to activate aimbot\nFOV : Size of the FOV circle\nSmooth : The movement of the aimbot. Higher the value the faster it will move, lower the value slower the slower it will move.\nDistance : Max distance to a player for the aimbot to see them.");
                }
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::SliderFloat("smooth", &global.features.SRaimbotsmooth, 0.3, 1.1);
                ImGui::SliderInt("distance", &global.features.SRaimbotdistance, 100, 2000);
                ImGui::Checkbox("autoshot", &global.features.SRaimbotautoshot);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::SliderInt("delay ms", &global.features.SRautosleep, 0, 2000);
                ImGui::Checkbox("visable", &global.features.SRaimbotvisable);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Checkbox("fov circle", &global.features.SRaimbotfovcircle);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::SliderFloat("size", &global.features.SRaimbotfov, 1, 80);
                ImGui::Combo("bone", &global.features.SRaimbotcombobone, global.features.aimbotBoneSelections, 4);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Combo("hotkey", &global.features.SRaimbotcombokey, global.features.aimbotKeySelections, 4);
            }

            //SMGs
            if (global.features.aimbotGunCombo == 4)
            {
                TextCentered("SMGs");

                ImGui::Checkbox("enable", &global.features.SMGaimbotenable);
                ImGui::SameLine();
                ButtonCenteredOnLine(ICON_FA_QUESTION "##tooltip1", 2.f);
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                {
                    ImGui::SetTooltip("Autoshot : Aimbot will automatically shoot when on the selected bone.\nVisable : Aimbot will only activate on visable players\nFov Circle : When a player is inside the circle, aimbot will target them\nBone : Select what bone to target for aimbot\nHotKey: Select what key to press to activate aimbot\nFOV : Size of the FOV circle\nSmooth : The movement of the aimbot. Higher the value the faster it will move, lower the value slower the slower it will move.\nDistance : Max distance to a player for the aimbot to see them.");
                }
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::SliderFloat("smooth", &global.features.SMGaimbotsmooth, 0.3, 1.1);
                ImGui::SliderInt("distance", &global.features.SMGaimbotdistance, 100, 2000);
                ImGui::Checkbox("autoshot", &global.features.SMGaimbotautoshot);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::SliderInt("delay ms", &global.features.SMGautosleep, 0, 2000);
                ImGui::Checkbox("visable", &global.features.SMGaimbotvisable);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Checkbox("fov circle", &global.features.SMGaimbotfovcircle);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::SliderFloat("size", &global.features.SMGaimbotfov, 1, 80);
                ImGui::Combo("bone", &global.features.SMGaimbotcombobone, global.features.aimbotBoneSelections, 4);
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Combo("hotkey", &global.features.SMGaimbotcombokey, global.features.aimbotKeySelections, 4);
            }
        }
        ImGui::EndChild();

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::NewLine();
        TextCentered("RCS");

        if (ImGui::BeginChild("##rcs", ImVec2(225, 120), true, ImGuiWindowFlags_NoScrollbar))
        {
            ImGui::Checkbox("enable", &global.features.rcsenable);
            ImGui::SameLine();
            ButtonCenteredOnLine(ICON_FA_QUESTION "##tooltip1", 2.f);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImGui::SetTooltip("Scale X : Horizontal efficiency of the RCS\nScale Y: Vertical efficiency of the RCS");
            }

            ImGui::Spacing();
            ImGui::SliderFloat("scale X", &global.features.rcsscaleX, 0, 2);
            ImGui::SliderFloat("scale Y", &global.features.rcsscaleY, 0, 2);
        }
        ImGui::EndChild();

        if (ButtonCenteredOnLine("Graph", 0.5f))
            aimbotGrpah = !aimbotGrpah;

        if (aimbotGrpah)
            AimbotGraph();
    }
    ImGui::EndChild();

    ImGui::PopStyleColor();

    ImGui::End();
}

void Menu::AimbotGraph()
{
    ImGui::SetNextWindowPos({ 500, 0 });
    ImGui::SetNextWindowSize({ 315, 620 });

    ImGui::Begin("graphmenu", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

    PushStyle();

    //define
    ImGui::PlotInterface plot1, plot2;
    std::vector<ImGui::PlotItem> items1(1), items2(1);
    float aimbotX, aimbotY;

    //define what page the user is on for aimbot!
    if (global.features.aimbotGunCombo == 0)
    {
        aimbotX = global.features.ARaimbotsmooth;
        aimbotY = global.features.ARaimbotdistance;
    }
    else if (global.features.aimbotGunCombo == 1)
    {
        aimbotX = global.features.SGaimbotsmooth;
        aimbotY = global.features.SGaimbotdistance;
    }
    else if (global.features.aimbotGunCombo == 2)
    {
        aimbotX = global.features.PSaimbotsmooth;
        aimbotY = global.features.PSaimbotdistance;
    }
    else if (global.features.aimbotGunCombo == 3)
    {
        aimbotX = global.features.SRaimbotsmooth;
        aimbotY = global.features.SRaimbotdistance;
    }
    else if (global.features.aimbotGunCombo == 4)
    {
        aimbotX = global.features.SMGaimbotsmooth;
        aimbotY = global.features.SMGaimbotdistance;
    }

    //setup aimbot plot
    plot1.title = "Aimbot Effect";
    plot1.x_axis.label = "Smooth";
    plot1.y_axis.label = "Distance";
    plot1.y_axis.maximum = 2000;
    plot1.x_axis.maximum = 1.001;

    //set up aimbot line
    items1[0].label = "Effect";
    items1[0].type = ImGui::PlotItem::Line;
    items1[0].color = ImVec4{1,1,1,1};
    items1[0].size = 2;
    items1[0].data.resize(10000);
    for (int i = 0; i < 10000; ++i)
    {
        float x = i * aimbotX;
        float y = std::sin(1.5 * x) * aimbotY;
        items1[0].data[i] = { x, y };
    }

    //setup rcs plot
    plot2.title = "RCS Effect";
    plot2.x_axis.label = "Horizontal";
    plot2.y_axis.label = "Vertical";
    plot2.y_axis.maximum = 2;
    plot2.x_axis.maximum = 2;

    //set up rcs line
    items2[0].label = "Effect";
    items2[0].type = ImGui::PlotItem::Line;
    items2[0].color = ImVec4{ 1,1,1,1 };
    items2[0].size = 2;
    items2[0].data.resize(10000);
    for (int i = 0; i < 10000; ++i)
    {
        float x = i * 0.00011f * global.features.rcsscaleX;
        float y = 0.5f * std::sin(1.5 * x) * global.features.rcsscaleY;
        items2[0].data[i] = { x, y };
    }

    //call plots
    ImGui::Plot("##aimbot", plot1, items1, ImVec2(300, 300));
    ImGui::Plot("##rcs", plot2, items2, ImVec2(300, 300));

    ImGui::NewLine();

    ImGui::End();
}

void Menu::MiscMenu()
{

}