#include "scene_views.h"

#include "../scene_plugin_registry.h"
#include "../../workbench/workbench_config.h"

#include <cmath>

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

namespace {
const bool kRegistered2DRenderer = []() {
    Scenes::ScenePluginRegistry::RegisterRenderer("scene.2d", [](ImVec2 content_min, ImVec2 content_max, EditorTab& tab) {
        Scenes::Scene2DViews::RenderCanvas(content_min, content_max, tab);
    });
    Scenes::SceneViewContributions views{};
    views.primary_view_id = "scene";
    views.primary_view_title = "Scene";
    views.primary_view_renderer = &Scenes::Scene2DViews::RenderHierarchy;
    views.secondary_views.push_back({ "outline", "Outline", &Scenes::Scene2DViews::RenderOutline });
    views.secondary_views.push_back({ "properties", "Properties", &Scenes::Scene2DViews::RenderProperties });
    views.default_secondary_id = "outline";
    views.panel_views.push_back({ "output", "OUTPUT", &Scenes::Scene2DViews::RenderPanelOutput });
    views.default_panel_id = "output";
    Scenes::ScenePluginRegistry::RegisterViews("scene.2d", views);
    return true;
}();
}

void RenderCanvas(ImVec2 content_min, ImVec2 content_max, EditorTab& tab)
{
    (void)kRegistered2DRenderer;
    const WorkbenchTheme& theme = GetWorkbenchTheme();
    const WorkbenchThemeColors& colors = theme.colors;
    const WorkbenchThemeSizes& sizes = theme.sizes;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRectFilled(content_min, content_max, ImGui::GetColorU32(colors.editor_2d_bg));

    float grid_size = sizes.editor_2d_grid_size;
    ImU32 grid_color = ImGui::GetColorU32(colors.editor_2d_grid);

    for (float x = content_min.x; x < content_max.x; x += grid_size) {
        draw_list->AddLine(ImVec2(x, content_min.y), ImVec2(x, content_max.y), grid_color, sizes.editor_2d_grid_line_thickness);
    }
    for (float y = content_min.y; y < content_max.y; y += grid_size) {
        draw_list->AddLine(ImVec2(content_min.x, y), ImVec2(content_max.x, y), grid_color, sizes.editor_2d_grid_line_thickness);
    }

    ImVec2 center = ImVec2((content_min.x + content_max.x) * 0.5f, (content_min.y + content_max.y) * 0.5f);
    float time = (float)ImGui::GetTime();

    float angle = time * 0.5f;
    float rect_size = sizes.editor_2d_rect_size;
    ImVec2 rect_points[4];
    for (int i = 0; i < 4; i++) {
        float a = angle + i * 3.14159f * 0.5f;
        rect_points[i] = ImVec2(
            center.x + cosf(a) * rect_size - 100,
            center.y + sinf(a) * rect_size
        );
    }
    draw_list->AddQuadFilled(rect_points[0], rect_points[1], rect_points[2], rect_points[3],
        ImGui::GetColorU32(colors.editor_2d_rect_fill));
    draw_list->AddQuad(rect_points[0], rect_points[1], rect_points[2], rect_points[3],
        ImGui::GetColorU32(colors.editor_2d_rect_border), sizes.editor_2d_rect_outline);

    float circle_y = center.y + sinf(time * 2.0f) * 30.0f;
    draw_list->AddCircleFilled(ImVec2(center.x + 100, circle_y), sizes.editor_2d_circle_radius,
        ImGui::GetColorU32(colors.editor_2d_circle_fill), 32);
    draw_list->AddCircle(ImVec2(center.x + 100, circle_y), sizes.editor_2d_circle_radius,
        ImGui::GetColorU32(colors.editor_2d_circle_border), 32, sizes.editor_2d_circle_outline);

    int wave_points = 100;
    for (int i = 0; i < wave_points - 1; i++) {
        float t1 = (float)i / wave_points;
        float t2 = (float)(i + 1) / wave_points;

        float x1 = content_min.x + (content_max.x - content_min.x) * t1;
        float x2 = content_min.x + (content_max.x - content_min.x) * t2;

        float y1 = center.y + 150 + sinf(time * 2.0f + t1 * 10.0f) * 30.0f;
        float y2 = center.y + 150 + sinf(time * 2.0f + t2 * 10.0f) * 30.0f;

        draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2),
            ImGui::GetColorU32(colors.editor_2d_wave), sizes.editor_2d_wave_thickness);
    }

    float padding = sizes.editor_content_padding;
    ImVec2 title_pos = ImVec2(content_min.x + padding, content_min.y + padding);
    std::string title_text = "2D Canvas: " + tab.name;
    draw_list->AddText(title_pos, ImGui::GetColorU32(colors.editor_2d_title), title_text.c_str());

    ImVec2 hint_pos = ImVec2(content_min.x + padding, content_min.y + padding + 25);
    draw_list->AddText(hint_pos, ImGui::GetColorU32(colors.editor_2d_hint), "2D graphics scene with animations");
}

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
