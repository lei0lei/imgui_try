#include "scene_views.h"
#include "scene_theme.h"

#include "../scene_plugin_registry.h"
#include "../scene_layout.h"
#include "../scene_state.h"
#include "../../command/command_ids.h"
#include "../../core/workbench_theme.h"
#include "../../ui/title_bar_extension_widgets.h"

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
    Scenes::ScenePluginRegistry::RegisterRenderer("scene.3d", [](const SceneContext& ctx, EditorTab& tab) {
        Scenes::Scene3DViews::RenderCanvas(ctx, tab);
    });
    return true;
}();

const bool kRegistered3DTitleBarExtension = []() {
    Scenes::ScenePluginRegistry::RegisterTitleBarExtension("scene.3d", [](const TitleBarExtensionContext& ctx) {
        if (ctx.region != TitleBarExtensionRegion::Right || !ctx.active_tab) {
            return;
        }

        auto& tab = *ctx.active_tab;
        const bool secondary_visible = SceneState::GetOrInit(tab, "scene.layout.secondary.visible", "true") == "true";
        const bool panel_visible = SceneState::GetOrInit(tab, "scene.layout.panel.visible", "true") == "true";

        ImGui::PushID(static_cast<int>(tab.id));
        const float btn_w = GetWorkbenchTheme().sizes.title_layout_btn_w;
        const float btn_h = ctx.title_h;

        if (DrawTitleBarIconButton("secondary", btn_w, btn_h, secondary_visible, [&](ImDrawList* draw_list, ImVec2 c, float half, ImU32 col) {
            draw_list->AddRect(ImVec2(c.x - half, c.y - (half - 1.0f)), ImVec2(c.x + half, c.y + (half - 1.0f)), col, 2.0f, 0, 1.5f);
            if (secondary_visible) {
                draw_list->AddRectFilled(ImVec2(c.x + (half * 0.25f), c.y - (half - 2.0f)), ImVec2(c.x + (half - 1.0f), c.y + (half - 2.0f)), col);
            } else {
                draw_list->AddRect(ImVec2(c.x + (half * 0.25f), c.y - (half - 1.0f)), ImVec2(c.x + half, c.y + (half - 1.0f)), col, 2.0f, 0, 1.0f);
            }
        })) {
            if (ctx.trigger_scene_action) {
                ctx.trigger_scene_action("scene.toggle.secondary");
            }
        }
        ImGui::SameLine(0.0f, 0.0f);
        if (DrawTitleBarIconButton("panel", btn_w, btn_h, panel_visible, [&](ImDrawList* draw_list, ImVec2 c, float half, ImU32 col) {
            draw_list->AddRect(ImVec2(c.x - half, c.y - (half - 1.0f)), ImVec2(c.x + half, c.y + (half - 1.0f)), col, 2.0f, 0, 1.5f);
            if (panel_visible) {
                draw_list->AddRectFilled(ImVec2(c.x - (half - 1.0f), c.y + (half * 0.15f)), ImVec2(c.x + (half - 1.0f), c.y + (half - 1.0f)), col);
            } else {
                draw_list->AddRect(ImVec2(c.x - (half - 1.0f), c.y + (half * 0.15f)), ImVec2(c.x + (half - 1.0f), c.y + (half - 1.0f)), col, 2.0f, 0, 1.0f);
            }
        })) {
            if (ctx.trigger_scene_action) {
                ctx.trigger_scene_action("scene.toggle.panel");
            }
        }
        ImGui::PopID();
    });
    return true;
}();

const bool kRegistered3DStatusBarExtension = []() {
    Scenes::ScenePluginRegistry::RegisterStatusBarExtension("scene.3d", [](const StatusBarExtensionContext& ctx) {
        if (ctx.region != StatusBarExtensionRegion::Right || !ctx.active_tab) {
            return;
        }

        const auto& colors = GetWorkbenchTheme().colors;
        const bool secondary_visible = SceneState::GetOrInit(*ctx.active_tab, "scene.layout.secondary.visible", "true") == "true";
        const bool panel_visible = SceneState::GetOrInit(*ctx.active_tab, "scene.layout.panel.visible", "true") == "true";

        auto draw_chip = [&](const char* label) {
            const ImVec2 text_size = ImGui::CalcTextSize(label);
            const float chip_h = ctx.status_bar_h - 2.0f;
            const ImVec2 chip_pos = ImGui::GetCursorScreenPos();
            const ImVec2 chip_size(text_size.x + 16.0f, chip_h);
            const ImVec2 chip_max(chip_pos.x + chip_size.x, chip_pos.y + chip_size.y);
            const bool hovered = ImGui::IsMouseHoveringRect(chip_pos, chip_max, true);
            const float hover_darkening = hovered ? 0.82f : 1.0f;
            const ImVec4 bg(
                colors.status_bar_bg.x * hover_darkening,
                colors.status_bar_bg.y * hover_darkening,
                colors.status_bar_bg.z * hover_darkening,
                colors.status_bar_bg.w);

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddRectFilled(chip_pos, chip_max, ImGui::GetColorU32(bg), 0.0f);
            const ImVec2 text_pos(chip_pos.x + 8.0f, chip_pos.y + (chip_h - text_size.y) * 0.5f);
            draw_list->AddText(text_pos, ImGui::GetColorU32(colors.status_bar_text), label);
            ImGui::Dummy(chip_size);
        };

        draw_chip("3D");
        ImGui::SameLine();
        draw_chip(secondary_visible ? "Secondary:On" : "Secondary:Off");
        ImGui::SameLine();
        draw_chip(panel_visible ? "Panel:On" : "Panel:Off");
    });
    return true;
}();

const bool kRegistered3DTitleBarActions = []() {
    Scenes::ScenePluginRegistry::RegisterTitleBarActionHandler("scene.3d", [](EditorTab& tab, const std::string& action_id) {
        if (action_id == "scene.toggle.secondary") {
            std::string& raw = SceneState::GetOrInit(tab, "scene.layout.secondary.visible", "true");
            raw = (raw == "true") ? "false" : "true";
            return;
        }
        if (action_id == "scene.toggle.panel") {
            std::string& raw = SceneState::GetOrInit(tab, "scene.layout.panel.visible", "true");
            raw = (raw == "true") ? "false" : "true";
            return;
        }
    });
    return true;
}();

const bool kRegistered3DTitleBarExtensionWidth = []() {
    Scenes::ScenePluginRegistry::RegisterTitleBarExtensionWidthResolver("scene.3d", [](float title_h, const EditorTab*) {
        (void)title_h;
        const float btn_w = GetWorkbenchTheme().sizes.title_layout_btn_w;
        return btn_w * 2.0f;
    });
    return true;
}();

const bool kRegistered3DPrimarySidebarDebugProvider = []() {
    Scenes::ScenePluginRegistry::RegisterPrimarySidebarDebugDataProvider("scene.3d", [](const EditorTab* active_tab) {
        Scenes::ScenePrimarySidebarDebugData data;
        if (!active_tab || active_tab->scene_plugin_id != "scene.3d") {
            return data;
        }

        const auto read_flag = [&](const char* key) {
            const auto it = active_tab->scene_ui_state.find(key);
            return it == active_tab->scene_ui_state.end() ? true : (it->second == "true");
        };

        const bool secondary_visible = read_flag("scene.layout.secondary.visible");
        const bool panel_visible = read_flag("scene.layout.panel.visible");

        data.variables.push_back({ "scene", active_tab->scene_plugin_id });
        data.variables.push_back({ "tab", active_tab->name });
        data.variables.push_back({ "secondaryVisible", secondary_visible ? "true" : "false" });
        data.variables.push_back({ "panelVisible", panel_visible ? "true" : "false" });

        std::vector<std::string> watch_exprs;
        const auto watch_it = active_tab->scene_ui_state.find("debug.watch.list");
        if (watch_it != active_tab->scene_ui_state.end() && !watch_it->second.empty()) {
            const std::string& raw = watch_it->second;
            size_t start = 0;
            while (start <= raw.size()) {
                const size_t sep = raw.find(';', start);
                const size_t end = (sep == std::string::npos) ? raw.size() : sep;
                std::string expr = raw.substr(start, end - start);
                if (!expr.empty()) {
                    watch_exprs.push_back(std::move(expr));
                }
                if (sep == std::string::npos) {
                    break;
                }
                start = sep + 1;
            }
        }
        if (watch_exprs.empty()) {
            watch_exprs = { "camera.orbit", "horizon.offset", "grid.lines" };
        }

        for (const std::string& expr : watch_exprs) {
            std::string value = "<unknown>";
            if (expr == "camera.orbit") value = "time * 0.3";
            else if (expr == "horizon.offset") value = "height * 0.1";
            else if (expr == "grid.lines") value = "dynamic alpha";
            else if (expr == "secondary.visible") value = secondary_visible ? "true" : "false";
            else if (expr == "panel.visible") value = panel_visible ? "true" : "false";
            data.watches.push_back({ expr, value });
        }

        data.callstack.push_back({ "Scene3DViews::RenderCanvas", active_tab->name, true });
        data.callstack.push_back({ "EditorArea::DrawEditorContent", "workbench editor", false });
        data.callstack.push_back({ "WorkbenchRenderer::RenderEditorArea", "workbench", false });
        return data;
    });
    return true;
}();
}

void RenderCanvas(const SceneContext& ctx, EditorTab& tab)
{
    (void)kRegistered3DRenderer;
    ImVec2 content_min = ctx.content_min;
    ImVec2 content_max = ctx.content_max;
    const WorkbenchThemeColors& wb_colors = ctx.theme.colors;
    const Scene3DTheme::Theme& scene_theme = Scene3DTheme::Get();
    const Scene3DTheme::Colors& scene_colors = scene_theme.colors;
    const Scene3DTheme::Sizes& scene_sizes = scene_theme.sizes;

    const float secondary_w = 300.0f;
    const float panel_h = 180.0f;
    const SceneCanvasLayout layout = ComputeCanvasLayout(ctx, 640.0f, 420.0f, secondary_w, panel_h);
    const ImVec2 full_max = layout.full_max;
    const bool secondary_visible = SceneState::GetOrInit(tab, "scene.layout.secondary.visible", "true") == "true";
    const bool panel_visible = SceneState::GetOrInit(tab, "scene.layout.panel.visible", "true") == "true";
    ImVec2 effective_canvas_max = layout.canvas_max;
    if (layout.show_secondary && !secondary_visible) {
        effective_canvas_max.x += secondary_w;
    }
    if (layout.show_panel && !panel_visible) {
        effective_canvas_max.y += panel_h;
    }
    content_max = effective_canvas_max;

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

    if (layout.show_secondary && secondary_visible) {
        std::string& secondary_view = SceneState::GetOrInit(tab, "scene3d.secondary.active_view", "outline");
        const float tab_bar_h = GetWorkbenchTheme().sizes.editor_tab_bar_height;
        const float secondary_top_y = content_min.y - tab_bar_h;
        ImGui::SetCursorScreenPos(ImVec2(content_max.x, secondary_top_y));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, wb_colors.secondary_sidebar_bg);
        ImGui::PushStyleColor(ImGuiCol_Border, wb_colors.secondary_sidebar_border);
        ImGui::PushStyleColor(ImGuiCol_Text, wb_colors.secondary_sidebar_text);
        ImGui::BeginChild("scene3d_secondary", ImVec2(full_max.x - content_max.x, full_max.y - secondary_top_y), true);
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

    if (layout.show_panel && panel_visible) {
        std::string& panel_view = SceneState::GetOrInit(tab, "scene3d.panel.active_view", "output");
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
