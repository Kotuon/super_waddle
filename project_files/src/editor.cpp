
// System includes
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Local includes
#include "editor.hpp"
#include "graphics.hpp"

Editor::Editor() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setting up ImGui flags
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Setting style for ImGui
    ImGui::StyleColorsDark();
    if ( ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable ) {
        ImGui::GetStyle().WindowRounding = 0.f;
        ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 1.f;
    }

    // Setting up ImGui
    ImGui_ImplGlfw_InitForOpenGL( Graphics::Instance().GetWindow(), true );
    ImGui_ImplOpenGL3_Init( "#version 330" );
}

Editor& Editor::Instance() {
    static Editor editorInstance;
    return editorInstance;
}
