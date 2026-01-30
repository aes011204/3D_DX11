#include "ImguiManager.h"


//CImguiManager* CImguiManager::m_pInstance = nullptr;

CImguiManager::CImguiManager()
{
}

CImguiManager::~CImguiManager()
{
    
}
void CImguiManager::ApplyEditorDarkStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.ScrollbarRounding = 6.0f;
    style.WindowPadding = ImVec2(10, 10);
    style.FramePadding = ImVec2(8, 5);
    style.ItemSpacing = ImVec2(8, 6);

    ImVec4* c = style.Colors;
    c[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.11f, 0.13f, 1.00f);
    c[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
    c[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.17f, 0.20f, 1.00f);
    c[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.23f, 0.27f, 1.00f);
    c[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.27f, 0.32f, 1.00f);
    c[ImGuiCol_Header] = ImVec4(0.20f, 0.21f, 0.25f, 1.00f);
    c[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.27f, 0.33f, 1.00f);
    c[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.32f, 0.40f, 1.00f);
    c[ImGuiCol_Button] = ImVec4(0.18f, 0.19f, 0.22f, 1.00f);
    c[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.25f, 0.30f, 1.00f);
    c[ImGuiCol_ButtonActive] = ImVec4(0.28f, 0.30f, 0.38f, 1.00f);
    c[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
    c[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.11f, 0.13f, 1.00f);
}


void CImguiManager::Initialize(HWND _hWnd, ID3D11Device* _Device, ID3D11DeviceContext* _Context)
{
    //imgui 초기화
    {

        // Make process DPI aware and obtain main monitor scale
        ImGui_ImplWin32_EnableDpiAwareness();
        float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;
        //io.ConfigDockingAlwaysTabBar = true;
        //io.ConfigDockingTransparentPayload = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup scaling
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
        style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
        io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
        io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(_hWnd);
        ImGui_ImplDX11_Init(_Device, _Context);
    }
    ApplyEditorDarkStyle();
}
void CImguiManager::Begin()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void CImguiManager::Example()
{
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        // io 는 ImGui 내부에 존재하는 단일 글로벌 상태 객체 // 매번 지역 참조로 받기 (읽기만 할거면 상관없음) // 맴버변수로 캐싱하면 나중에 위험(ImGuiContext를 여러 개 운영하려는 경우일떄는 ㄱㄴ)
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }
}

//int CImguiManager::Update()
//{
//    //imgui
//    {
//        // Start the Dear ImGui frame
//        ImGui_ImplDX11_NewFrame();
//        ImGui_ImplWin32_NewFrame();
//        ImGui::NewFrame();
//
//        // 예시
//
//        {
//
//            // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
//            if (show_demo_window)
//                ImGui::ShowDemoWindow(&show_demo_window);
//
//            // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
//            {
//                static float f = 0.0f;
//                static int counter = 0;
//
//                ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
//
//                ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
//                ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
//                ImGui::Checkbox("Another Window", &show_another_window);
//
//                ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
//                ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
//
//                if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
//                    counter++;
//                ImGui::SameLine();
//                ImGui::Text("counter = %d", counter);
//
//                // io 는 ImGui 내부에 존재하는 단일 글로벌 상태 객체 // 매번 지역 참조로 받기 (읽기만 할거면 상관없음) // 맴버변수로 캐싱하면 나중에 위험(ImGuiContext를 여러 개 운영하려는 경우일떄는 ㄱㄴ)
//                ImGuiIO& io = ImGui::GetIO(); (void)io;
//
//                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
//                ImGui::End();
//            }
//
//            // 3. Show another simple window.
//            if (show_another_window)
//            {
//                ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
//                ImGui::Text("Hello from another window!");
//                if (ImGui::Button("Close Me"))
//                    show_another_window = false;
//                ImGui::End();
//            }
//        }
//    }
//    return 0;
//}



void CImguiManager::Render()
{

    //imgui
    {

        // Rendering
        ImGui::Render(); //(데이터 준비 단계)

        //const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        //g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr); // (바인딩) -> 외부에서 MainApp (BindingRenderTarget 이랑 같음)
        //g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha); // (지우기) -> 외부에서 MainApp
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // io 는 ImGui 내부에 존재하는 단일 글로벌 상태 객체 // 매번 지역 참조로 받기 (읽기만 할거면 상관없음) // 맴버변수로 캐싱하면 나중에 위험(ImGuiContext를 여러 개 운영하려는 경우일떄는 ㄱㄴ)
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows(); // (실제 imgui 랜더링)
            ImGui::RenderPlatformWindowsDefault(); // (멀티 뷰포트 처리)
        }



    }


}

CImguiManager* CImguiManager::Create()
{
    return new CImguiManager;
}

void CImguiManager::Free()
{
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    delete this;

}
