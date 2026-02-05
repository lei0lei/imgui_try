#include "view_registry_defaults_config.h"
#include "imgui.h"
#include "../workbench/workbench_config.h"

namespace UI {

namespace {

void DrawPlaceholderHeader(const char* title, const ImVec4& color)
{
    ImGui::TextColored(color, "%s", title);
    ImGui::Separator();
    ImGui::Spacing();
}

void DrawEmptyState(const char* message)
{
    ImGui::TextWrapped("%s", message);
}

void RenderExplorer(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("EXPLORER", colors.primary_sidebar_text_dim);
    ImGui::Text("No folder opened");
    ImGui::Spacing();
    ImGui::Button("Open Folder");
}

void RenderSearch(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("SEARCH", colors.primary_sidebar_text_dim);
    static char search_buf[256] = "";
    ImGui::InputTextWithHint("##search", "Search...", search_buf, sizeof(search_buf));
    ImGui::Spacing();
    ImGui::Text("Search across files");
}

void RenderDebug(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("RUN AND DEBUG", colors.primary_sidebar_text_dim);
    DrawEmptyState("No configurations");
}

void RenderExtensions(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("EXTENSIONS", colors.primary_sidebar_text_dim);
    DrawEmptyState("Extension marketplace");
}

void RenderNodeLibrary(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("NODE LIBRARY", colors.primary_sidebar_text_dim);
    ImGui::Text("Math");
    ImGui::BulletText("Gain");
    ImGui::BulletText("Test");
    ImGui::Spacing();
    ImGui::Text("Logic");
    ImGui::BulletText("If");
    ImGui::BulletText("Switch");
}

void RenderNodeAssets(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("NODE ASSETS", colors.primary_sidebar_text_dim);
    DrawEmptyState("No node assets available");
}

void RenderSceneHierarchy(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("HIERARCHY", colors.primary_sidebar_text_dim);
    if (active_tab) {
        ImGui::Text("Scene: %s", active_tab->name.c_str());
    }
    ImGui::BulletText("Camera");
    ImGui::BulletText("Light");
    ImGui::BulletText("Root");
}

void RenderSceneAssets(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("ASSETS", colors.primary_sidebar_text_dim);
    DrawEmptyState("No assets loaded");
}

void RenderOutline(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("OUTLINE", colors.secondary_sidebar_text);
    if (active_tab) {
        ImGui::Text("Active: %s", active_tab->name.c_str());
        ImGui::BulletText("Root");
        ImGui::BulletText("Node A");
        ImGui::BulletText("Node B");
    } else {
        DrawEmptyState("No outline available");
    }
}

void RenderNodeOutline(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab)
{
    RenderOutline(content_min, content_max, active_tab);
    ImGui::Spacing();
    ImGui::Text("Node Editor Outline");
}

void RenderSceneOutline(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab)
{
    RenderOutline(content_min, content_max, active_tab);
    ImGui::Spacing();
    ImGui::Text("Scene Outline");
}

void RenderProperties(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("PROPERTIES", colors.secondary_sidebar_text);
    if (active_tab) {
        ImGui::Text("Selection: %s", active_tab->name.c_str());
    }
    DrawEmptyState("Select an element to inspect its properties.");
}

void RenderNodeProperties(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab)
{
    RenderProperties(content_min, content_max, active_tab);
    ImGui::Spacing();
    ImGui::Text("Node Properties");
}

void RenderSceneProperties(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab)
{
    RenderProperties(content_min, content_max, active_tab);
    ImGui::Spacing();
    ImGui::Text("Scene Properties");
}

void RenderPanelOutput(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "Build completed successfully");
}

void RenderPanelProblems(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "No problems detected");
}

void RenderPanelTerminal(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_terminal_prompt, "$ ");
    ImGui::SameLine();
    ImGui::TextColored(colors.panel_text, "Ready");
}

void RenderPanelConsole(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "Console output goes here.");
}

void RenderPanelDebug(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "Debug console");
}

void RenderPanelTimeline(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "Timeline is empty.");
}

void RenderPanelProfiler(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "Profiler idle.");
}

} // namespace

const std::vector<DefaultViewConfig>& GetDefaultViewConfigs()
{
    static std::vector<DefaultViewConfig> configs;
    if (!configs.empty())
        return configs;

    for (SceneType mode : { SceneType::Scene2D, SceneType::Scene3D, SceneType::NodeEditor }) {
        configs.push_back({ mode, ViewContainer::PrimarySidebar, "explorer", "Explorer", RenderExplorer });
        configs.push_back({ mode, ViewContainer::PrimarySidebar, "search", "Search", RenderSearch });
        configs.push_back({ mode, ViewContainer::PrimarySidebar, "node", "Node", (mode == SceneType::NodeEditor) ? RenderNodeLibrary : RenderSceneHierarchy });
        configs.push_back({ mode, ViewContainer::PrimarySidebar, "debug", "Debug", RenderDebug });
        configs.push_back({ mode, ViewContainer::PrimarySidebar, "extensions", "Extensions", RenderExtensions });

        if (mode == SceneType::NodeEditor) {
            configs.push_back({ mode, ViewContainer::SecondarySidebar, "outline", "Outline", RenderNodeOutline });
            configs.push_back({ mode, ViewContainer::SecondarySidebar, "properties", "Properties", RenderNodeProperties });
        } else {
            configs.push_back({ mode, ViewContainer::SecondarySidebar, "outline", "Outline", RenderSceneOutline });
            configs.push_back({ mode, ViewContainer::SecondarySidebar, "properties", "Properties", RenderSceneProperties });
        }

        configs.push_back({ mode, ViewContainer::Panel, "problems", "PROBLEMS", RenderPanelProblems });
        configs.push_back({ mode, ViewContainer::Panel, "output", "OUTPUT", RenderPanelOutput });
        configs.push_back({ mode, ViewContainer::Panel, "debug", "DEBUG", RenderPanelDebug });
        configs.push_back({ mode, ViewContainer::Panel, "terminal", "TERMINAL", RenderPanelTerminal });
    }

    return configs;
}

ViewDefinition GetDefaultPrimaryView(ActivityBarItem item)
{
    switch (item) {
        case ActivityBarItem::Explorer:
            return { "explorer", "Explorer", RenderExplorer };
        case ActivityBarItem::Search:
            return { "search", "Search", RenderSearch };
        case ActivityBarItem::NodeEditor:
            return { "node", "Node", RenderNodeLibrary };
        case ActivityBarItem::Debug:
            return { "debug", "Debug", RenderDebug };
        case ActivityBarItem::Extensions:
            return { "extensions", "Extensions", RenderExtensions };
        default:
            return { "empty", "Empty", nullptr };
    }
}

} // namespace UI
