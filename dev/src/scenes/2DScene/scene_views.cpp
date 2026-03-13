#include "scene_views.h"
#include "scene_theme.h"

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
const bool kRegistered2DRenderer = []() {
    Scenes::ScenePluginRegistry::RegisterRenderer("scene.2d", [](ImVec2 content_min, ImVec2 content_max, EditorTab& tab) {
        Scenes::Scene2DViews::RenderCanvas(content_min, content_max, tab);
    });
    return true;
}();
}

void RenderCanvas(ImVec2 content_min, ImVec2 content_max, EditorTab& tab)
{
    (void)kRegistered2DRenderer;
    const WorkbenchThemeColors& wb_colors = GetWorkbenchTheme().colors;
    const Scene2DTheme::Theme& scene_theme = Scene2DTheme::Get();
    const Scene2DTheme::Colors& scene_colors = scene_theme.colors;
    const Scene2DTheme::Sizes& scene_sizes = scene_theme.sizes;

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

    draw_list->AddRectFilled(content_min, content_max, ImGui::GetColorU32(scene_colors.bg));

    float grid_size = scene_sizes.grid_size;
    ImU32 grid_color = ImGui::GetColorU32(scene_colors.grid);

    for (float x = content_min.x; x < content_max.x; x += grid_size) {
        draw_list->AddLine(ImVec2(x, content_min.y), ImVec2(x, content_max.y), grid_color, scene_sizes.grid_line_thickness);
    }
    for (float y = content_min.y; y < content_max.y; y += grid_size) {
        draw_list->AddLine(ImVec2(content_min.x, y), ImVec2(content_max.x, y), grid_color, scene_sizes.grid_line_thickness);
    }

    ImVec2 center = ImVec2((content_min.x + content_max.x) * 0.5f, (content_min.y + content_max.y) * 0.5f);
    float time = (float)ImGui::GetTime();

    float angle = time * 0.5f;
    float rect_size = scene_sizes.rect_size;
    ImVec2 rect_points[4];
    for (int i = 0; i < 4; i++) {
        float a = angle + i * 3.14159f * 0.5f;
        rect_points[i] = ImVec2(
            center.x + cosf(a) * rect_size - 100,
            center.y + sinf(a) * rect_size
        );
    }
    draw_list->AddQuadFilled(rect_points[0], rect_points[1], rect_points[2], rect_points[3],
        ImGui::GetColorU32(scene_colors.rect_fill));
    draw_list->AddQuad(rect_points[0], rect_points[1], rect_points[2], rect_points[3],
        ImGui::GetColorU32(scene_colors.rect_border), scene_sizes.rect_outline);

    float circle_y = center.y + sinf(time * 2.0f) * 30.0f;
    draw_list->AddCircleFilled(ImVec2(center.x + 100, circle_y), scene_sizes.circle_radius,
        ImGui::GetColorU32(scene_colors.circle_fill), 32);
    draw_list->AddCircle(ImVec2(center.x + 100, circle_y), scene_sizes.circle_radius,
        ImGui::GetColorU32(scene_colors.circle_border), 32, scene_sizes.circle_outline);

    int wave_points = 100;
    for (int i = 0; i < wave_points - 1; i++) {
        float t1 = (float)i / wave_points;
        float t2 = (float)(i + 1) / wave_points;

        float x1 = content_min.x + (content_max.x - content_min.x) * t1;
        float x2 = content_min.x + (content_max.x - content_min.x) * t2;

        float y1 = center.y + 150 + sinf(time * 2.0f + t1 * 10.0f) * 30.0f;
        float y2 = center.y + 150 + sinf(time * 2.0f + t2 * 10.0f) * 30.0f;

        draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2),
            ImGui::GetColorU32(scene_colors.wave), scene_sizes.wave_thickness);
    }

    float padding = scene_sizes.content_padding;
    ImVec2 title_pos = ImVec2(content_min.x + padding, content_min.y + padding);
    std::string title_text = "2D Canvas: " + tab.name;
    draw_list->AddText(title_pos, ImGui::GetColorU32(scene_colors.title), title_text.c_str());

    ImVec2 hint_pos = ImVec2(content_min.x + padding, content_min.y + padding + 25);
    draw_list->AddText(hint_pos, ImGui::GetColorU32(scene_colors.hint), "2D graphics scene with animations");

    if (show_secondary) {
        std::string& secondary_view = tab.scene_ui_state["scene2d.secondary.active_view"];
        if (secondary_view.empty()) {
            secondary_view = "outline";
        }
        ImGui::SetCursorScreenPos(ImVec2(content_max.x, content_min.y));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, wb_colors.secondary_sidebar_bg);
        ImGui::PushStyleColor(ImGuiCol_Border, wb_colors.secondary_sidebar_border);
        ImGui::PushStyleColor(ImGuiCol_Text, wb_colors.secondary_sidebar_text);
        ImGui::BeginChild("scene2d_secondary", ImVec2(full_max.x - content_max.x, full_max.y - content_min.y), true);
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
        std::string& panel_view = tab.scene_ui_state["scene2d.panel.active_view"];
        if (panel_view.empty()) {
            panel_view = "output";
        }
        ImGui::SetCursorScreenPos(ImVec2(content_min.x, content_max.y));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, wb_colors.panel_bg);
        ImGui::PushStyleColor(ImGuiCol_Border, wb_colors.panel_border);
        ImGui::PushStyleColor(ImGuiCol_Text, wb_colors.panel_text);
        ImGui::BeginChild("scene2d_panel", ImVec2(content_max.x - content_min.x, full_max.y - content_max.y), true);
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
