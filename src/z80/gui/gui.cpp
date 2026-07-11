#include "../memory/memory.h"
#include "../cpu/registers.h"
#include "../cpu/cpu.h"
#include "../instructions/instruction_set.h"
#include "log.cpp"
#include "gui.h"
#include "imgui.h"
#include "imgui_memory_editor.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <string>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> 


#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

#include "gui.h"
static MemoryEditor mem_edit;
static AppLog log;


// Codigo de Fato
int GUI::run_interface(Memory &mem, CPU &cpu)
{

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Definições do ImGui
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char *glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char *glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char *glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Janela
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    GLFWwindow *window = glfwCreateWindow((int)(1360 * main_scale), (int)(768 * main_scale), "Z80 Emulator", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); 

    // Setup ImGui 
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    //  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    //  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
    //  io.ConfigViewportsNoAutoMerge = true;
    //  io.ConfigViewportsNoTaskBarIcon = true;
   
    ImGui::StyleColorsDark();

    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale); 
    style.FontScaleDpi = main_scale; 
#if GLFW_VERSION_MAJOR >= 3 && GLFW_VERSION_MINOR >= 3
    io.ConfigDpiScaleFonts = true;     
    io.ConfigDpiScaleViewports = true; 
#endif

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);


    bool show_demo_window = false;
    static AppLog log;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static bool mostrar_editor = false;
    static bool mostrar_macro = false;
    static bool mostrar_montador = false;
    static bool mostrar_ligador = false;
    static bool mostrar_debug = false;

    ImGuiWindowFlags flags_travadas = ImGuiWindowFlags_NoMove |
                                      ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoCollapse |
                                      ImGuiWindowFlags_NoTitleBar |
                                      ImGuiWindowFlags_NoScrollbar;

    
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    static bool running = false;
    while (!glfwWindowShouldClose(window))

#endif
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
        

        if (running && !cpu.isHalted())
        {
        cpu.step();
        }
        // Pega dimensões atuais para garantir que as janelas ocupem todo o espaço (tiling)
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        float total_X = viewport->WorkSize.x;
        float total_Y = viewport->WorkSize.y;
        float inicio_X = viewport->WorkPos.x;
        float inicio_Y = viewport->WorkPos.y;

        // Define as proporções dinamicamente baseadas no tamanho atual
        float largura_esquerda = total_X * 0.58f;
        float largura_direita = total_X * 0.42f;
        float altura_topo = total_Y * 0.70f;
        float altura_fundo = total_Y * 0.30f;

        // Começa o frame do ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Janelas
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // Janela Principal (Editor e Menus)
        ImGui::SetNextWindowPos(ImVec2(inicio_X, inicio_Y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(largura_esquerda, altura_topo), ImGuiCond_Always);
        ImGui::Begin("Principal", nullptr, flags_travadas);

        ImGui::Checkbox("Demo Window", &show_demo_window);

        if (ImGui::BeginTabBar("MinhasAbas", ImGuiTabBarFlags_None))
        {

            if (ImGui::BeginTabItem("Menu"))
            {
                ImGui::Text("Esta é a aba de Menu.");
                if (ImGui::Button("Abrir arquivo"))
                {
                }

                if (ImGui::Button("Novo arquivo"))
                {
                    mostrar_editor = true;
                }

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Editor", &mostrar_editor))
            {
                ImGui::Text("Esta é a aba de Edicão de arquivo!.");

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Macro", &mostrar_macro))
            {
                ImGui::Text("Esta é a aba Macro.");

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Montador", &mostrar_montador))
            {
                ImGui::Text("Esta é a aba Montador.");

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Ligador", &mostrar_ligador))
            {
                ImGui::Text("Esta é a aba Ligador.");

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Rodando", &mostrar_debug))
            {
                ImGui::Text("Esta é a aba de debug.");

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::End();

        // Janela de Log (Abaixo da Principal)
        ImGui::SetNextWindowPos(ImVec2(inicio_X, altura_topo + inicio_Y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(largura_esquerda, altura_fundo), ImGuiCond_Always);
        ImGui::Begin("Example: Log", nullptr, flags_travadas);
        ImGui::End(); // Truque de append para definir flags
        log.Draw("Example: Log");

        // Janela de Registradores (Lado Direito Superior)
        ImGui::SetNextWindowPos(ImVec2(inicio_X + largura_esquerda, inicio_Y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(largura_direita, total_Y * 0.50f), ImGuiCond_Always);
        ImGui::Begin("Registradores Z80", nullptr, flags_travadas);

        Registers& regs = cpu.getRegisters();

        ImGui::SeparatorText("Registradores");

        ImGui::Columns(2, nullptr, false);

        ImGui::Text("A: %02X    B: %02X", regs.A, regs.B);
        ImGui::Text("C: %02X    D: %02X", regs.C, regs.D);
        ImGui::Text("E: %02X    H: %02X", regs.E, regs.H);
        ImGui::Text("L: %02X", regs.L);

        ImGui::NextColumn();

        ImGui::Text("16 bit:");
        ImGui::Text("AF: %04X", regs.AF());
        ImGui::Text("BC: %04X", regs.BC());
        ImGui::Text("DE: %04X", regs.DE());
        ImGui::Text("HL: %04X", regs.HL());

        ImGui::Columns(1);
        ImGui::Separator();

        ImGui::Text("PC: %04X", regs.PC);
        ImGui::Text("SP: %04X", regs.SP);
        ImGui::Text("IX: %04X", regs.IX);
        ImGui::Text("IY: %04X", regs.IY);

        ImGui::SeparatorText("Flags");

        ImGui::Text(
            "S:%d  Z:%d  H:%d  PV:%d  N:%d  C:%d",
            regs.F.S,
            regs.F.Z,
            regs.F.H,
            regs.F.PV,
            regs.F.N,
            regs.F.C
        );

        ImGui::SeparatorText("Opções");

        if (ImGui::Button("Run"))
        {
            running = true;
        }

        ImGui::SameLine();

        if (ImGui::Button("Pause"))
        {
            running = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Avançar p/ proxima instrução"))
        {
            running = false;
            cpu.mem.write(0x1000, 0x0F); 
            if (!cpu.isHalted())
            {
                cpu.step();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset"))
        {
            running = false;
            cpu.reset_Gui();
        }
        


        ImGui::SeparatorText("Proxima instrução");

        uint16_t pc = regs.PC;
        uint8_t opcode = mem.read(pc);

        ImGui::Text("PC: %04X", pc);

        std::string instrL = NomeUnicoDeFunçaoQNExiste(mem, pc);

        ImGui::Text("Opcode: %02X", mem.read(pc));
        ImGui::Text("%s", instrL.c_str());


        ImGui::End();

        // Editor de Memória (Lado Direito Inferior)
        ImGui::SetNextWindowPos(ImVec2(inicio_X + largura_esquerda, inicio_Y + (total_Y * 0.50f)), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(largura_direita, total_Y * 0.50f), ImGuiCond_Always);
        ImGui::Begin("Memory Editor", nullptr, flags_travadas);
        mem_edit.DrawContents(mem.Outro_get_array(), 0x10000); 
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    };
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}