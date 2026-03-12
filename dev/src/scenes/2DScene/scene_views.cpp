#include "scene_views.h"

#include "../../workbench/workbench_config.h"

namespace Scenes::Scene2DViews {

namespace {

void DrawHeader(const char* title, const ImVec4& color)
{
    ImGui::TextColored(color, "%s", title);
    ImGui::Separator();
    ImGui::Spacing();
}

void DrawEmpty(const char* text)
{
    ImGui::TextWrapped("%s", text);
}

} // namespace

void RenderHierarchy(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawHeader("HIERARCHY", colors.primary_sidebar_text_dim);
    if (active_tab) {
        ImGui::Text("Scene: %s", active_tab->name.c_str());
    }
    ImGui::BulletText("Camera");
    ImGui::BulletText("Light");
    ImGui::BulletText("Root");
}

void RenderOutline(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawHeader("OUTLINE", colors.secondary_sidebar_text);
    if (active_tab) {
        ImGui::Text("Active: %s", active_tab->name.c_str());
        ImGui::BulletText("Root");
        ImGui::BulletText("Node A");
        ImGui::BulletText("Node B");
    } else {
        DrawEmpty("No outline available");
    }
    ImGui::Spacing();
    ImGui::Text("Scene 2D Outline");
}

void RenderProperties(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawHeader("PROPERTIES", colors.secondary_sidebar_text);
    if (active_tab) {
        ImGui::Text("Selection: %s", active_tab->name.c_str());
    }
    DrawEmpty("Select an element to inspect its properties.");
    ImGui::Spacing();
    ImGui::Text("Scene 2D Properties");
}

void RenderPanelOutput(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "Build completed successfully");
}

} // namespace Scenes::Scene2DViews
