#include "scene_views.h"

#include "../scene_plugin_registry.h"
#include "../../workbench/workbench_config.h"

#include <cmath>

namespace Scenes::Scene3DViews {

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
const bool kRegistered3DRenderer = []() {
    Scenes::ScenePluginRegistry::RegisterRenderer("scene.3d", [](ImVec2 content_min, ImVec2 content_max, EditorTab& tab) {
        Scenes::Scene3DViews::RenderCanvas(content_min, content_max, tab);
    });
    Scenes::SceneViewContributions views{};
    views.primary_view_id = "scene";
    views.primary_view_title = "Scene";
    views.primary_view_renderer = &Scenes::Scene3DViews::RenderHierarchy;
    views.secondary_views.push_back({ "outline", "Outline", &Scenes::Scene3DViews::RenderOutline });
    views.secondary_views.push_back({ "properties", "Properties", &Scenes::Scene3DViews::RenderProperties });
    views.default_secondary_id = "outline";
    views.panel_views.push_back({ "output", "OUTPUT", &Scenes::Scene3DViews::RenderPanelOutput });
    views.default_panel_id = "output";
    Scenes::ScenePluginRegistry::RegisterViews("scene.3d", views);
    return true;
}();
}

void RenderCanvas(ImVec2 content_min, ImVec2 content_max, EditorTab& tab)
{
    (void)kRegistered3DRenderer;
    const WorkbenchTheme& theme = GetWorkbenchTheme();
    const WorkbenchThemeColors& colors = theme.colors;
    const WorkbenchThemeSizes& sizes = theme.sizes;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImU32 bg_top = ImGui::GetColorU32(colors.editor_3d_bg_top);
    ImU32 bg_bottom = ImGui::GetColorU32(colors.editor_3d_bg_bottom);
    draw_list->AddRectFilledMultiColor(content_min, content_max, bg_top, bg_top, bg_bottom, bg_bottom);

    ImVec2 center = ImVec2((content_min.x + content_max.x) * 0.5f, (content_min.y + content_max.y) * 0.5f);
    float width = content_max.x - content_min.x;
    float height = content_max.y - content_min.y;

    float time = (float)ImGui::GetTime();
    float horizon_y = center.y + height * 0.1f;

    int grid_lines = static_cast<int>(sizes.editor_3d_grid_lines);
    float grid_spacing = sizes.editor_3d_grid_spacing;

    for (int i = -grid_lines / 2; i <= grid_lines / 2; i++) {
        float x_offset = i * grid_spacing;
        ImVec2 near_point = ImVec2(center.x + x_offset, content_max.y);
        float far_x = center.x + x_offset * 0.2f;
        ImVec2 far_point = ImVec2(far_x, horizon_y);
        float alpha = 1.0f - (fabsf((float)i) / (grid_lines / 2.0f)) * 0.5f;
        ImVec4 grid = colors.editor_3d_grid;
        ImU32 line_color = ImGui::GetColorU32(ImVec4(grid.x, grid.y, grid.z, grid.w * alpha));
        draw_list->AddLine(near_point, far_point, line_color, sizes.editor_3d_grid_line_thickness);
    }

    int horizontal_lines = static_cast<int>(sizes.editor_3d_horizontal_lines);
    for (int i = 0; i < horizontal_lines; i++) {
        float t = (float)i / (horizontal_lines - 1);
        float ease_t = t * t;
        float y = content_max.y + (horizon_y - content_max.y) * ease_t;
        float scale = 1.0f - ease_t * 0.8f;
        float half_width = width * 0.5f * scale;
        ImVec2 left = ImVec2(center.x - half_width, y);
        ImVec2 right = ImVec2(center.x + half_width, y);
        float alpha = 1.0f - ease_t * 0.7f;
        ImVec4 grid = colors.editor_3d_grid;
        ImU32 line_color = ImGui::GetColorU32(ImVec4(grid.x, grid.y, grid.z, grid.w * alpha));
        draw_list->AddLine(left, right, line_color, sizes.editor_3d_horizontal_line_thickness);
    }

    float orbit_radius = sizes.editor_3d_orbit_radius;
    for (int cube_idx = 0; cube_idx < 3; cube_idx++) {
        float angle = time * 0.3f + cube_idx * 2.0f;
        float cube_x = center.x + cosf(angle) * orbit_radius * (1.0f + cube_idx * 0.5f);
        float cube_y = horizon_y - 100.0f - cube_idx * 40.0f + sinf(time * 0.5f + cube_idx) * 20.0f;
        float size = sizes.editor_3d_cube_size - cube_idx * 5.0f;

        ImVec2 cube_center = ImVec2(cube_x, cube_y);
        float rot = time * 0.5f + cube_idx * 1.0f;
        float cos_r = cosf(rot);
        float sin_r = sinf(rot);

        ImVec2 vertices[8];
        float offsets[8][3] = {
            {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
            {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}
        };

        for (int v = 0; v < 8; v++) {
            float x = offsets[v][0] * size;
            float y = offsets[v][1] * size;
            float z = offsets[v][2] * size;

            float rx = x * cos_r - z * sin_r;
            float rz = x * sin_r + z * cos_r;

            vertices[v].x = cube_center.x + rx - rz * 0.5f;
            vertices[v].y = cube_center.y + y + rz * 0.3f;
        }

        int edges[12][2] = {
            {0,1}, {1,2}, {2,3}, {3,0},
            {4,5}, {5,6}, {6,7}, {7,4},
            {0,4}, {1,5}, {2,6}, {3,7}
        };

        ImU32 cube_color = ImGui::GetColorU32(colors.editor_3d_cube);
        for (int e = 0; e < 12; e++) {
            draw_list->AddLine(vertices[edges[e][0]], vertices[edges[e][1]], cube_color, sizes.editor_3d_cube_outline);
        }
    }

    float padding = sizes.editor_content_padding;
    ImVec2 title_pos = ImVec2(content_min.x + padding, content_min.y + padding);

    std::string title_text = "3D View: " + tab.name;
    draw_list->AddText(title_pos, ImGui::GetColorU32(colors.editor_3d_title), title_text.c_str());

    ImVec2 hint_pos = ImVec2(content_min.x + padding, content_min.y + padding + 25);
    draw_list->AddText(hint_pos, ImGui::GetColorU32(colors.editor_3d_hint), "Interactive 3D scene with animated grid");
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
    ImGui::Text("Scene 3D Outline");
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
    ImGui::Text("Scene 3D Properties");
}

void RenderPanelOutput(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "Build completed successfully");
}

} // namespace Scenes::Scene3DViews
