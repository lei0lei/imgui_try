#include "scene_views.h"
#include "scene_theme.h"

#include "../scene_plugin_registry.h"
#include "../scene_layout.h"
#include "../scene_state.h"
#include "../../command/command_ids.h"
#include "../../core/workbench_theme.h"
#include "../../ui/title_bar_extension_widgets.h"

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
    Scenes::ScenePluginRegistry::RegisterRenderer("scene.2d", [](const SceneContext& ctx, EditorTab& tab) {
        Scenes::Scene2DViews::RenderCanvas(ctx, tab);
    });
    return true;
}();

const bool kRegistered2DTitleBarExtension = []() {
    Scenes::ScenePluginRegistry::RegisterTitleBarExtension("scene.2d", [](const TitleBarExtensionContext& ctx) {
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

const bool kRegistered2DStatusBarExtension = []() {
    Scenes::ScenePluginRegistry::RegisterStatusBarExtension("scene.2d", [](const StatusBarExtensionContext& ctx) {
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

        draw_chip("2D");
        ImGui::SameLine();
        draw_chip(secondary_visible ? "Secondary:On" : "Secondary:Off");
        ImGui::SameLine();
        draw_chip(panel_visible ? "Panel:On" : "Panel:Off");
    });
    return true;
}();

const bool kRegistered2DTitleBarActions = []() {
    Scenes::ScenePluginRegistry::RegisterTitleBarActionHandler("scene.2d", [](EditorTab& tab, const std::string& action_id) {
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

const bool kRegistered2DTitleBarExtensionWidth = []() {
    Scenes::ScenePluginRegistry::RegisterTitleBarExtensionWidthResolver("scene.2d", [](float title_h, const EditorTab*) {
        (void)title_h;
        const float btn_w = GetWorkbenchTheme().sizes.title_layout_btn_w;
        return btn_w * 2.0f;
    });
    return true;
}();

const bool kRegistered2DPrimarySidebarDebugProvider = []() {
    Scenes::ScenePluginRegistry::RegisterPrimarySidebarDebugDataProvider("scene.2d", [](const EditorTab* active_tab) {
        Scenes::ScenePrimarySidebarDebugData data;
        if (!active_tab || active_tab->scene_plugin_id != "scene.2d") {
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
            watch_exprs = { "rect.rotation", "circle.bounce", "wave.points" };
        }

        for (const std::string& expr : watch_exprs) {
            std::string value = "<unknown>";
            if (expr == "rect.rotation") value = "time * 0.5";
            else if (expr == "circle.bounce") value = "sin(time * 2.0)";
            else if (expr == "wave.points") value = "100";
            else if (expr == "secondary.visible") value = secondary_visible ? "true" : "false";
            else if (expr == "panel.visible") value = panel_visible ? "true" : "false";
            data.watches.push_back({ expr, value });
        }

        data.callstack.push_back({ "Scene2DViews::RenderCanvas", active_tab->name, true });
        data.callstack.push_back({ "EditorArea::DrawEditorContent", "workbench editor", false });
        data.callstack.push_back({ "WorkbenchRenderer::RenderEditorArea", "workbench", false });
        return data;
    });
    return true;
}();
}

void RenderCanvas(const SceneContext& ctx, EditorTab& tab)
{
    (void)kRegistered2DRenderer;
    ImVec2 content_min = ctx.content_min;
    ImVec2 content_max = ctx.content_max;
    const WorkbenchThemeColors& wb_colors = ctx.theme.colors;
    const Scene2DTheme::Theme& scene_theme = Scene2DTheme::Get();
    const Scene2DTheme::Colors& scene_colors = scene_theme.colors;
    const Scene2DTheme::Sizes& scene_sizes = scene_theme.sizes;

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

    if (layout.show_secondary && secondary_visible) {
        std::string& secondary_view = SceneState::GetOrInit(tab, "scene2d.secondary.active_view", "outline");
        const float tab_bar_h = GetWorkbenchTheme().sizes.editor_tab_bar_height;
        const float secondary_top_y = content_min.y - tab_bar_h;
        ImGui::SetCursorScreenPos(ImVec2(content_max.x, secondary_top_y));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, wb_colors.secondary_sidebar_bg);
        ImGui::PushStyleColor(ImGuiCol_Border, wb_colors.secondary_sidebar_border);
        ImGui::PushStyleColor(ImGuiCol_Text, wb_colors.secondary_sidebar_text);
        ImGui::BeginChild("scene2d_secondary", ImVec2(full_max.x - content_max.x, full_max.y - secondary_top_y), true);
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
        std::string& panel_view = SceneState::GetOrInit(tab, "scene2d.panel.active_view", "output");
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
