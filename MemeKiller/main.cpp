// Made by NimaBastani (I actually edited example_win32_directx9)
// https://github.com/NimaBastani/MemeKiller
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <D3dx9tex.h>
#pragma comment(lib, "D3dx9")
#pragma comment(lib, "winmm.lib")
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
ImVec2		m_vecScale;
float ScaleX(float x) { return m_vecScale.x * x; }
float ScaleY(float y) { return m_vecScale.y * y; }
#define MULT_X	0.00052083333f	// 1/1920
#define MULT_Y	0.00092592592f 	// 1/1080
PDIRECT3DTEXTURE9 background_texture = NULL;
PDIRECT3DTEXTURE9 meme_texture[16];
PDIRECT3DTEXTURE9 ship_texture = NULL;
float ShipX;
inline bool file_exits(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}
inline void StyleColorsDarcula(ImGuiStyle* dst) //took from styles/themes thread, I don't remember who makes it sorry
{
    auto* style = (dst ? dst : &ImGui::GetStyle());
    style->WindowRounding = 5.3f;
    style->GrabRounding = style->FrameRounding = 2.3f;
    style->ScrollbarRounding = 5.0f;
    style->FrameBorderSize = 1.0f;
    style->ItemSpacing.y = 6.5f;

    style->Colors[ImGuiCol_Text] = { 0.73333335f, 0.73333335f, 0.73333335f, 1.00f };
    style->Colors[ImGuiCol_TextDisabled] = { 0.34509805f, 0.34509805f, 0.34509805f, 1.00f };
    style->Colors[ImGuiCol_WindowBg] = { 0.23529413f, 0.24705884f, 0.25490198f, 0.94f };
    style->Colors[ImGuiCol_ChildBg] = { 0.23529413f, 0.24705884f, 0.25490198f, 0.00f };
    style->Colors[ImGuiCol_PopupBg] = { 0.23529413f, 0.24705884f, 0.25490198f, 0.94f };
    style->Colors[ImGuiCol_Border] = { 0.33333334f, 0.33333334f, 0.33333334f, 0.50f };
    style->Colors[ImGuiCol_BorderShadow] = { 0.15686275f, 0.15686275f, 0.15686275f, 0.00f };
    style->Colors[ImGuiCol_FrameBg] = { 0.16862746f, 0.16862746f, 0.16862746f, 0.54f };
    style->Colors[ImGuiCol_FrameBgHovered] = { 0.453125f, 0.67578125f, 0.99609375f, 0.67f };
    style->Colors[ImGuiCol_FrameBgActive] = { 0.47058827f, 0.47058827f, 0.47058827f, 0.67f };
    style->Colors[ImGuiCol_TitleBg] = { 0.04f, 0.04f, 0.04f, 1.00f };
    style->Colors[ImGuiCol_TitleBgCollapsed] = { 0.16f, 0.29f, 0.48f, 1.00f };
    style->Colors[ImGuiCol_TitleBgActive] = { 0.00f, 0.00f, 0.00f, 0.51f };
    style->Colors[ImGuiCol_MenuBarBg] = { 0.27058825f, 0.28627452f, 0.2901961f, 0.80f };
    style->Colors[ImGuiCol_ScrollbarBg] = { 0.27058825f, 0.28627452f, 0.2901961f, 0.60f };
    style->Colors[ImGuiCol_ScrollbarGrab] = { 0.21960786f, 0.30980393f, 0.41960788f, 0.51f };
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = { 0.21960786f, 0.30980393f, 0.41960788f, 1.00f };
    style->Colors[ImGuiCol_ScrollbarGrabActive] = { 0.13725491f, 0.19215688f, 0.2627451f, 0.91f };
    // style->Colors[ImGuiCol_ComboBg]               = {0.1f, 0.1f, 0.1f, 0.99f};
    style->Colors[ImGuiCol_CheckMark] = { 0.90f, 0.90f, 0.90f, 0.83f };
    style->Colors[ImGuiCol_SliderGrab] = { 0.70f, 0.70f, 0.70f, 0.62f };
    style->Colors[ImGuiCol_SliderGrabActive] = { 0.30f, 0.30f, 0.30f, 0.84f };
    style->Colors[ImGuiCol_Button] = { 0.33333334f, 0.3529412f, 0.36078432f, 0.49f };
    style->Colors[ImGuiCol_ButtonHovered] = { 0.21960786f, 0.30980393f, 0.41960788f, 1.00f };
    style->Colors[ImGuiCol_ButtonActive] = { 0.13725491f, 0.19215688f, 0.2627451f, 1.00f };
    style->Colors[ImGuiCol_Header] = { 0.33333334f, 0.3529412f, 0.36078432f, 0.53f };
    style->Colors[ImGuiCol_HeaderHovered] = { 0.453125f, 0.67578125f, 0.99609375f, 0.67f };
    style->Colors[ImGuiCol_HeaderActive] = { 0.47058827f, 0.47058827f, 0.47058827f, 0.67f };
    style->Colors[ImGuiCol_Separator] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
    style->Colors[ImGuiCol_SeparatorHovered] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
    style->Colors[ImGuiCol_SeparatorActive] = { 0.31640625f, 0.31640625f, 0.31640625f, 1.00f };
    style->Colors[ImGuiCol_ResizeGrip] = { 1.00f, 1.00f, 1.00f, 0.85f };
    style->Colors[ImGuiCol_ResizeGripHovered] = { 1.00f, 1.00f, 1.00f, 0.60f };
    style->Colors[ImGuiCol_ResizeGripActive] = { 1.00f, 1.00f, 1.00f, 0.90f };
    style->Colors[ImGuiCol_PlotLines] = { 0.61f, 0.61f, 0.61f, 1.00f };
    style->Colors[ImGuiCol_PlotLinesHovered] = { 1.00f, 0.43f, 0.35f, 1.00f };
    style->Colors[ImGuiCol_PlotHistogram] = { 0.90f, 0.70f, 0.00f, 1.00f };
    style->Colors[ImGuiCol_PlotHistogramHovered] = { 1.00f, 0.60f, 0.00f, 1.00f };
    style->Colors[ImGuiCol_TextSelectedBg] = { 0.18431373f, 0.39607847f, 0.79215693f, 0.90f };
}
int random_number(int min_num, int max_num)
{
    int result = 0, low_num = 0, hi_num = 0;

    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1; // include max_num in output
    }
    else {
        low_num = max_num + 1; // include max_num in output
        hi_num = min_num;
    }
    //srand(50)
    srand((unsigned int)GetTickCount64());//time(NULL)
    result = (rand() % (hi_num - low_num)) + low_num;
    return result;
}
// Main code
int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Meme Killer"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Meme Killer"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    DEVMODE fullscreenSettings;
    RECT windowBoundary;
    EnumDisplaySettings(NULL, 0, &fullscreenSettings);
    HDC windowHDC = GetDC(hwnd);
    fullscreenSettings.dmPelsWidth = GetDeviceCaps(windowHDC, DESKTOPHORZRES);
    fullscreenSettings.dmPelsHeight = GetDeviceCaps(windowHDC, DESKTOPVERTRES);
    fullscreenSettings.dmBitsPerPel = GetDeviceCaps(windowHDC, BITSPIXEL);
    fullscreenSettings.dmDisplayFrequency = GetDeviceCaps(windowHDC, VREFRESH);
    fullscreenSettings.dmFields = DM_PELSWIDTH |
        DM_PELSHEIGHT |
        DM_BITSPERPEL |
        DM_DISPLAYFREQUENCY;

    SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
    SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, GetDeviceCaps(windowHDC, DESKTOPHORZRES), GetDeviceCaps(windowHDC, DESKTOPVERTRES), SWP_SHOWWINDOW);
    if (!ChangeDisplaySettings(&fullscreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL) exit(0);
    ShowWindow(hwnd, SW_MAXIMIZE);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    StyleColorsDarcula(NULL);

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Fonts
    static const ImWchar ranges[] = {
    0x0020, 0x00FF, // Basic Latin
    0x2010, 0x205E, // Punctuations
    0
    };
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\tahomabd.ttf", 15.0f, NULL, &ranges[0]);

    //load background picture
    if (file_exits("game/background.jpg"))
    {
        D3DXCreateTextureFromFileA(g_pd3dDevice, "game/background.jpg", &background_texture);
    }

    if (file_exits("game/ship.png"))
    {
        D3DXCreateTextureFromFileA(g_pd3dDevice, "game/ship.png", &ship_texture);
    }

    //Load memes
    char filename[100];
    for (int i = 1; i < sizeof(meme_texture); ++i)
    {
        sprintf(filename, "game/memes/%d.jpg", i);
        if (file_exits(filename))
        {
            D3DXCreateTextureFromFileA(g_pd3dDevice, filename, &meme_texture[i]);
        }
        else break;
    }
    bool inGame = false;
    // Main loop
    float rand1;
    float rand2;
    ImVec2 pos;
    int randi;
    double ChangeTimer = GetTickCount();
    int KilledMemes = 0;
    int ShowedMemes = 0;
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    PlaySoundA("game/music.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGuiIO& io2 = ImGui::GetIO(); (void)io2;
        // scale
        m_vecScale.x = io2.DisplaySize.x * MULT_X;
        m_vecScale.y = io2.DisplaySize.y * MULT_Y;

        //Background
        ImGui::GetBackgroundDrawList()->AddImage((void*)background_texture, ImVec2(ScaleX(0.0f), ScaleY(0.0f)), ImVec2(ScaleX(1920.0f), ScaleY(1080.0f)));
        ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(ScaleX(0.0f), ScaleY(0.0f)), ImVec2(ScaleX(1920.0f), ScaleY(1080.0f)), (ImU32)ImColor(0.0f, 0.0f, 0.0f, 0.3f));

        //FPS
        char fps[20];
        sprintf(fps, "%.f", ImGui::GetIO().Framerate);
        ImGui::GetOverlayDrawList()->AddText(ImVec2(ScaleX(1870), ScaleY(10)), (ImU32)ImColor(1.0f, 1.0f, 1.0f, 0.8f), fps);
        
        if (inGame)
        {
            char killedshowed[20];
            sprintf(killedshowed, "%d/%d", KilledMemes, ShowedMemes);
            ImGui::GetOverlayDrawList()->AddText(ImVec2(ScaleX(10), ScaleY(10)), (ImU32)ImColor(1.0f, 1.0f, 1.0f, 0.8f), killedshowed);
            if(ChangeTimer <= GetTickCount() && ChangeTimer != -1)
            {
                ShowedMemes++;
                ChangeTimer = GetTickCount() + 3000;
                rand1 = random_number(0, 1920); //x
                rand2 = random_number(0, 1080);//y
                pos = ImVec2(ScaleX(rand1), ScaleY(rand2));
                randi = random_number(1, 16); // 16 = sizeof(meme_texture)
            }

            ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::Begin("Meme", NULL, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);
            void* image = (void*)meme_texture[randi];
            ImGui::Image(image, ImVec2(50.0f, 50.0f));
            ImGui::End();
            if (GetAsyncKeyState(VK_RIGHT))
            {
                if (ShipX < io.DisplaySize.x) ShipX = ShipX + 7.0f;
            }
            if (GetAsyncKeyState(VK_LEFT))
            {
                if (ShipX > 0) ShipX = ShipX - 7.0f;
            }
            //std::cout << io2.DisplaySize.x << "  " << io2.DisplaySize.y << "   " << ShipX << std::endl;
            ImGui::SetNextWindowPos(ImVec2(ShipX, io.DisplaySize.y * 0.9f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::Begin("Ship", NULL, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);
            ImGui::Image((void*)ship_texture, ImVec2(ScaleX(100.0f), ScaleY(100.0f)));
            ImGui::Text("KILL");
            ImGui::Text("THOSE");
            ImGui::Text("MEMES");
            /*if (ImGui::Button("New"))
            {
                rand1 = random_number(0, 1920); //x
                rand2 = random_number(60, 1000);//y
                pos = ImVec2(rand1, rand2);
                randi = rand() % 16 + 1; // 16 = sizeof(meme_texture)
            }*/
            ImGui::End();
            bool SpaceDown = GetAsyncKeyState(VK_SPACE);
            if (SpaceDown)
            {
                ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(ShipX - 1.0f, io.DisplaySize.y * 0.9f), ImVec2(ShipX + 1.0f, 0.0f), (ImU32)ImColor(0.7f, 0.7f, 0.7f, 1.0f), 0.1f);
                if (ShipX - 25.0f <= pos.x && pos.x <= ShipX + 25.0f)
                {
                    ShowedMemes++;
                    KilledMemes++;
                    ChangeTimer = GetTickCount() + 2000;
                    rand1 = random_number(0, 1920); //x
                    rand2 = random_number(0, 1080);//y
                    pos = ImVec2(ScaleX(rand1), ScaleY(rand2));
                    randi = random_number(1, 16); // 16 = sizeof(meme_texture)
                }
            }
        }
        else
        {
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowBgAlpha(0.8f);
            ImGui::Begin("MENU", nullptr,
                ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoMove |
                //ImGuiWindowFlags_NoInputs |
                //ImGuiWindowFlags_NoSavedSettings |
                //ImGuiWindowFlags_NoBackground |
                //ImGuiWindowFlags_NoMouseInputs |
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize
            );
            ImVec2 buttonSize = ImVec2(ImGui::GetFontSize() * 5.5, ImGui::GetFontSize() * 2.5);
            if (ImGui::Button("Play", buttonSize)) inGame = true, KilledMemes = 0, ShowedMemes = 0;

            if (ImGui::Button("Credits", buttonSize)) ImGui::OpenPopup("Credits");
            ImGuiWindowFlags credits_flags = 0;
            credits_flags |= ImGuiWindowFlags_NoResize;
            credits_flags |= ImGuiWindowFlags_NoCollapse;
            credits_flags |= ImGuiWindowFlags_NoMove;
            credits_flags |= ImGuiWindowFlags_AlwaysAutoResize;
            if (ImGui::BeginPopupModal("Credits", NULL, credits_flags))
            {
                //ImGui::SetWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always);
                ImGui::Text("Nima Bastaniw");
                ImGui::Text("Ocornut(ImGui)");
                if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            if (ImGui::Button("Quit", buttonSize)) break;
            ImGui::End();
        }
        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*255.0f), (int)(clear_color.y*255.0f), (int)(clear_color.z*255.0f), (int)(clear_color.w*255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
