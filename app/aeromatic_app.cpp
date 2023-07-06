#include <string>
#include <iostream>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <Systems/Systems.h>
#include "Aircraft.h"
#include "types.h"


#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}


void draw(Aeromatic::Param* param)
{
    const char * name = param->name().c_str();
    char *label = new char[strlen("##")+strlen(name)+1];
    strcpy(label,"##");
    strcat(label, name);
    
    ImGui::Text((param->name() + " [" + param->get_units() + "]" + " (" + param->get() + ")").c_str());
    ImGui::SameLine();
    HelpMarker(param->help().c_str());
    unsigned options = param->no_options();
    if (options)
    {
        if (param->get() == "no")
            param->current_value = param->get_option(0);
        else if((param->get() == "yes"))
            param->current_value = param->get_option(1);
        else
            param->current_value = param->get_option(std::stoi(param->get()));

        if (ImGui::BeginCombo(label, param->current_value.c_str())) // The second parameter is the label previewed before opening the combo.
        {
            for (unsigned j=0; j<options; ++j)
            {
                bool is_selected = (param->current_value.c_str() == param->get_option(j).c_str()); // You can store your selection however you want, outside or inside your objects
                if (ImGui::Selectable(param->get_option(j).c_str(), is_selected))
                {
                    param->current_value = param->get_option(j);
                    auto input = std::to_string(j);
                    param->set(input);
                }
            }
            ImGui::EndCombo();
        }
    }
    else
    {
        std::string input = "";
        switch(param->get_type())
        {
            case Aeromatic::ParamType::PARAM_BOOL:
                if (param->get() == "yes")
                    param->current_value_i = 1;
                else if (param->get() == "no")
                    param->current_value_i = 0;

                ImGui::Text("no"); ImGui::SameLine();
                ImGui::SliderInt(label, &param->current_value_i , 0, 1); ImGui::SameLine();
                ImGui::Text("yes");


                if (param->current_value_i == 0)
                    input = "no";
                else if (param->current_value_i == 1)
                    input = "yes";
                param->set(input);
                break;
            case Aeromatic::ParamType::PARAM_INT:
                param->current_value_i = std::stoi(param->get());
                ImGui::InputInt(label, &(param->current_value_i));
                input = std::to_string(param->current_value_i);
                param->set(input);
                break;
            case Aeromatic::ParamType::PARAM_FLOAT:
                param->current_value_f = std::stof(param->get());
                ImGui::InputFloat(label, &(param->current_value_f));
                input = std::to_string(param->current_value_f);
                param->set(input);
                break;
            case Aeromatic::ParamType::PARAM_STRING:
                sprintf(param->current_value_c, param->get().c_str());
                ImGui::InputText(label, param->current_value_c, Aeromatic::ParamType::PARAM_MAX_STRING);
                input = param->current_value_c;
                param->set(input);
                break;
            case Aeromatic::ParamType::PARAM_UNSUPPORTED:
            default:
                break;
        }
    }

    ImGui::Dummy(ImVec2(5,5));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(5,5));
}

// Main code
int main(int, char**)
{

    Aeromatic::Aeromatic aeromatic;

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1330, 520, "NiceAeromatic++", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

#ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::StyleColorsClassic();

        // menu
        {
            bool successful = false;
            bool failed = false;
            if(ImGui::BeginMainMenuBar())
            {
                if(ImGui::MenuItem("Generate!"))
                {
                    if (aeromatic.fdm())
                    {
                        successful = true;
                    }
                    else {
                        failed = false;
                    }
                }

            ImGui::EndMainMenuBar();
            }

            if(successful)
                ImGui::OpenPopup("Successful");

            if(failed)
                ImGui::OpenPopup("Failed");
        }

        // modal windows
        {

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            
            if (ImGui::BeginPopupModal("Successful", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("We're finished, the files have been written.");

                if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }
            if (ImGui::BeginPopupModal("Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Error: Unable to write files");

                if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }
        }

        // all param windows
        {
            // General Information window
            ImGui::Begin("General Information");                          
            for (auto it : aeromatic._general_order) {
                draw(aeromatic._general[it]);
            }
            ImGui::End();

            // Weight and Balance wondiw
            ImGui::Begin("Weight and Balance");                          
            for (auto it : aeromatic._weight_balance_order) {
                draw(aeromatic._weight_balance[it]);
            }
            ImGui::End();

            // Geometry
            ImGui::Begin("Geometry");                          
            for (auto it : aeromatic._geometry_order) {
                draw(aeromatic._geometry[it]);
            }
            ImGui::End();

            // Systems
            ImGui::Begin("Systems");                          
            const std::vector<Aeromatic::System*> systems = aeromatic.get_systems();
            for (auto it : systems)
            {
                Aeromatic::System* system = it;

                Aeromatic::Param* param;
                system->param_reset();
                while ((param = system->param_next()) != 0) {
                    draw(param);
                }
            }
            ImGui::End();           

        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
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