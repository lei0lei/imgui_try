#include "scene_views.h"
#include "scene_theme.h"

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

bool DrawSceneTabButton(const char* label, bool active, const WorkbenchThemeColors& colors)
{
    ImGui::PushStyleColor(ImGuiCol_Button, active ? colors.panel_tab_active : colors.panel_tab_bg);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors.panel_tab_hover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors.panel_tab_active);
    const bool pressed = ImGui::Button(label);
    ImGui::PopStyleColor(3);
    return pressed;
}

} // namespace

namespace {
const bool kRegistered3DRenderer = []() {
    Scenes::ScenePluginRegistry::RegisterRenderer("scene.3d", [](ImVec2 content_min, ImVec2 content_max, EditorTab& tab) {
        Scenes::Scene3DViews::RenderCanvas(content_min, content_max, tab);
    });
    return true;
}();
}

void RenderCanvas(ImVec2 content_min, ImVec2 content_max, EditorTab& tab)
{
    (void)kRegistered3DRenderer;
    const WorkbenchThemeColors& wb_colors = GetWorkbenchTheme().colors;
    const Scene3DTheme::Theme& scene_theme = Scene3DTheme::Get();
    const Scene3DTheme::Colors& scene_colors = scene_theme.colors;
    const Scene3DTheme::Sizes& scene_sizes = scene_theme.sizes;

    const ImVec2 full_max = content_max;
    bool show_secondary = true;
    bool show_panel = true;
    const float secondary_w = 300.0f;
    const float panel_h = 180.0f;
    if ((content_max.x - content_min.x) < 640.0f) {
        show_secondary = false;
    }
    if ((content_max.y - content_min.y) < 420.0f) {
        show_panel = false;
    }
    content_max.x -= show_secondary ? secondary_w : 0.0f;
    content_max.y -= show_panel ? panel_h : 0.0f;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImU32 bg_top = ImGui::GetColorU32(scene_colors.bg_top);
    ImU32 bg_bottom = ImGui::GetColorU32(scene_colors.bg_bottom);
    draw_list->AddRectFilledMultiColor(content_min, content_max, bg_top, bg_top, bg_bottom, bg_bottom);

    ImVec2 center = ImVec2((content_min.x + content_max.x) * 0.5f, (content_min.y + content_max.y) * 0.5f);
    float width = content_max.x - content_min.x;
    float height = content_max.y - content_min.y;

    float time = (float)ImGui::GetTime();
    float horizon_y = center.y + height * 0.1f;

    int grid_lines = static_cast<int>(scene_sizes.grid_lines);
    float grid_spacing = scene_sizes.grid_spacing;

    for (int i = -grid_lines / 2; i <= grid_lines / 2; i++) {
        float x_offset = i * grid_spacing;
        ImVec2 near_point = ImVec2(center.x + x_offset, content_max.y);
        float far_x = center.x + x_offset * 0.2f;
        ImVec2 far_point = ImVec2(far_x, horizon_y);
        float alpha = 1.0f - (fabsf((float)i) / (grid_lines / 2.0f)) * 0.5f;
        ImVec4 grid = scene_colors.grid;
        ImU32 line_color = ImGui::GetColorU32(ImVec4(grid.x, grid.y, grid.z, grid.w * alpha));
        draw_list->AddLine(near_point, far_point, line_color, scene_sizes.grid_line_thickness);
    }

    int horizontal_lines = static_cast<int>(scene_sizes.horizontal_lines);
    for (int i = 0; i < horizontal_lines; i++) {
        float t = (float)i / (horizontal_lines - 1);
        float ease_t = t * t;
        float y = content_max.y + (horizon_y - content_max.y) * ease_t;
        float scale = 1.0f - ease_t * 0.8f;
        float half_width = width * 0.5f * scale;
        ImVec2 left = ImVec2(center.x - half_width, y);
        ImVec2 right = ImVec2(center.x + half_width, y);
        float alpha = 1.0f - ease_t * 0.7f;
        ImVec4 grid = scene_colors.grid;
        ImU32 line_color = ImGui::GetColorU32(ImVec4(grid.x, grid.y, grid.z, grid.w * alpha));
        draw_list->AddLine(left, right, line_color, scene_sizes.horizontal_line_thickness);
    }

    float orbit_radius = scene_sizes.orbit_radius;
    for (int cube_idx = 0; cube_idx < 3; cube_idx++) {
        float angle = time * 0.3f + cube_idx * 2.0f;
        float cube_x = center.x + cosf(angle) * orbit_radius * (1.0f + cube_idx * 0.5f);
        float cube_y = horizon_y - 100.0f - cube_idx * 40.0f + sinf(time * 0.5f + cube_idx) * 20.0f;
        float size = scene_sizes.cube_size - cube_idx * 5.0f;

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

        ImU32 cube_color = ImGui::GetColorU32(scene_colors.cube);
        for (int e = 0; e < 12; e++) {
            draw_list->AddLine(vertices[edges[e][0]], vertices[edges[e][1]], cube_color, scene_sizes.cube_outline);
        }
    }

    float padding = scene_sizes.content_padding;
    ImVec2 title_pos = ImVec2(content_min.x + padding, content_min.y + padding);

    std::string title_text = "3D View: " + tab.name;
    draw_list->AddText(title_pos, ImGui::GetColorU32(scene_colors.title), title_text.c_str());

    ImVec2 hint_pos = ImVec2(content_min.x + padding, content_min.y + padding + 25);
    draw_list->AddText(hint_pos, ImGui::GetColorU32(scene_colors.hint), "Interactive 3D scene with animated grid");

    if (show_secondary) {
        std::string& secondary_view = tab.scene_ui_state["scene3d.secondary.active_view"];
        if (secondary_view.empty()) {
            secondary_view = "outline";
        }
        ImGui::SetCursorScreenPos(ImVec2(content_max.x, content_min.y));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, wb_colors.secondary_sidebar_bg);
        ImGui::PushStyleColor(ImGuiCol_Border, wb_colors.secondary_sidebar_border);
        ImGui::PushStyleColor(ImGuiCol_Text, wb_colors.secondary_sidebar_text);
        ImGui::BeginChild("scene3d_secondary", ImVec2(full_max.x - content_max.x, full_max.y - content_min.y), true);
        if (DrawSceneTabButton("Outline", secondary_view == "outline", wb_colors)) {
            secondary_view = "outline";
        }
        ImGui::SameLine();
        if (DrawSceneTabButton("Properties", secondary_view == "properties", wb_colors)) {
            secondary_view = "properties";
        }
        ImGui::Separator();
        if (secondary_view == "properties") {
            RenderProperties(ImVec2(0, 0), ImVec2(0, 0), &tab);
        } else {
            RenderOutline(ImVec2(0, 0), ImVec2(0, 0), &tab);
        }
        ImGui::EndChild();
        ImGui::PopStyleColor(3);
    }

    if (show_panel) {
        std::string& panel_view = tab.scene_ui_state["scene3d.panel.active_view"];
        if (panel_view.empty()) {
            panel_view = "output";
        }
        ImGui::SetCursorScreenPos(ImVec2(content_min.x, content_max.y));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, wb_colors.panel_bg);
        ImGui::PushStyleColor(ImGuiCol_Border, wb_colors.panel_border);
        ImGui::PushStyleColor(ImGuiCol_Text, wb_colors.panel_text);
        ImGui::BeginChild("scene3d_panel", ImVec2(content_max.x - content_min.x, full_max.y - content_max.y), true);
        RenderPanelOutput(ImVec2(0, 0), ImVec2(0, 0), &tab);
        ImGui::EndChild();
        ImGui::PopStyleColor(3);
    }
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
