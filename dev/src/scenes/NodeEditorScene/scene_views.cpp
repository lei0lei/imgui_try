#include "scene_views.h"
#include "scene_theme.h"

#include "node_graph_executor.h"
#include "node_editor_state.h"
#include "../scene_plugin_registry.h"
#include "../scene_layout.h"
#include "../scene_state.h"
#include "../../command/command_ids.h"
#include "../../workbench/title_bar_extension_widgets.h"
#include "../../workbench/workbench_config.h"

#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <string>
#include <tuple>

namespace Scenes::NodeEditorViews {

void RenderOutline(ImVec2, ImVec2, EditorTab* active_tab);
void RenderProperties(ImVec2, ImVec2, EditorTab* active_tab);
void RenderPanelOutput(ImVec2, ImVec2, EditorTab* active_tab);

namespace {

void DrawHeader(const char* title, const ImVec4& color)
{
    ImGui::TextColored(color, "%s", title);
    ImGui::Separator();
    ImGui::Spacing();
}

void DrawOutlineBase(EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawHeader("OUTLINE", colors.secondary_sidebar_text);
    if (active_tab) {
        ImGui::Text("Active: %s", active_tab->name.c_str());
        ImGui::BulletText("Root");
        ImGui::BulletText("Node A");
        ImGui::BulletText("Node B");
    } else {
        ImGui::TextWrapped("No outline available");
    }
}

bool InputTextMultilineStdString(const char* label, std::string* str, const ImVec2& size)
{
    if (str->capacity() < 1024) {
        str->reserve(1024);
    }
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize;
    auto callback = [](ImGuiInputTextCallbackData* data) -> int {
        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
            auto* s = static_cast<std::string*>(data->UserData);
            s->resize(static_cast<size_t>(data->BufTextLen));
            data->Buf = s->data();
        }
        return 0;
    };
    if (str->empty()) {
        str->resize(1);
        (*str)[0] = '\0';
    }
    return ImGui::InputTextMultiline(label, str->data(), str->capacity() + 1, size, flags, callback, str);
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

void ExecuteNodeGraph(EditorTab& tab, bool parallel)
{
    auto& node_state = NodeEditor::GetState(tab);
    auto result = NodeGraph::ExecuteGraph(node_state, parallel);
    node_state.node_exec_last_ok = result.success;
    node_state.node_exec_last_parallel = result.parallel;
    node_state.node_exec_last_ms = result.duration_ms;
    node_state.node_exec_last_error = result.error;
    node_state.node_exec_log = std::move(result.log);
    node_state.node_exec_outputs = std::move(result.outputs);
    if (node_state.node_exec_log.empty()) {
        node_state.node_exec_log.push_back(result.success ? "Execution finished." : "Execution failed.");
    }
}

void RenderSecondarySidebarPanel(const SceneCanvasLayout& layout,
    const WorkbenchThemeColors& wb_colors,
    const ImVec2& content_max,
    const ImVec2& content_min,
    const ImVec2& full_max,
    EditorTab& tab)
{
    if (!layout.show_secondary) {
        return;
    }

    std::string& secondary_view = SceneState::GetOrInit(tab, "scene_node.secondary.active_view", "library");
    const float tab_bar_h = GetWorkbenchTheme().sizes.editor_tab_bar_height;
    const float secondary_top_y = content_min.y - tab_bar_h;
    ImGui::SetCursorScreenPos(ImVec2(content_max.x, secondary_top_y));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, wb_colors.secondary_sidebar_bg);
    ImGui::PushStyleColor(ImGuiCol_Border, wb_colors.secondary_sidebar_border);
    ImGui::PushStyleColor(ImGuiCol_Text, wb_colors.secondary_sidebar_text);
    ImGui::BeginChild("scene_node_secondary", ImVec2(full_max.x - content_max.x, full_max.y - secondary_top_y), true);
    if (DrawSceneTabButton("Outline", secondary_view == "outline", wb_colors)) {
        secondary_view = "outline";
    }
    ImGui::SameLine();
    if (DrawSceneTabButton("Properties", secondary_view == "properties", wb_colors)) {
        secondary_view = "properties";
    }
    ImGui::SameLine();
    if (DrawSceneTabButton("Library", secondary_view == "library", wb_colors)) {
        secondary_view = "library";
    }
    ImGui::Separator();
    if (secondary_view == "properties") {
        RenderProperties(ImVec2(0, 0), ImVec2(0, 0), &tab);
    } else if (secondary_view == "library") {
        RenderLibrary(ImVec2(0, 0), ImVec2(0, 0), &tab);
    } else {
        RenderOutline(ImVec2(0, 0), ImVec2(0, 0), &tab);
    }
    ImGui::EndChild();
    ImGui::PopStyleColor(3);
}

void RenderBottomPanel(const SceneCanvasLayout& layout,
    const WorkbenchThemeColors& wb_colors,
    const ImVec2& content_min,
    const ImVec2& content_max,
    const ImVec2& full_max,
    EditorTab& tab)
{
    if (!layout.show_panel) {
        return;
    }

    std::string& panel_view = SceneState::GetOrInit(tab, "scene_node.panel.active_view", "output");
    (void)panel_view;
    ImGui::SetCursorScreenPos(ImVec2(content_min.x, content_max.y));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, wb_colors.panel_bg);
    ImGui::PushStyleColor(ImGuiCol_Border, wb_colors.panel_border);
    ImGui::PushStyleColor(ImGuiCol_Text, wb_colors.panel_text);
    ImGui::BeginChild("scene_node_panel", ImVec2(content_max.x - content_min.x, full_max.y - content_max.y), true);
    RenderPanelOutput(ImVec2(0, 0), ImVec2(0, 0), &tab);
    ImGui::EndChild();
    ImGui::PopStyleColor(3);
}

} // namespace

namespace {
const bool kRegisteredNodeRenderer = []() {
    Scenes::ScenePluginRegistry::RegisterRenderer("scene.node", [](const SceneContext& ctx, EditorTab& tab) {
        Scenes::NodeEditorViews::RenderCanvas(ctx, tab);
    });
    return true;
}();

const bool kRegisteredNodeTitleBarExtension = []() {
    Scenes::ScenePluginRegistry::RegisterTitleBarExtension("scene.node", [](const TitleBarExtensionContext& ctx) {
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

const bool kRegisteredNodeStatusBarExtension = []() {
    Scenes::ScenePluginRegistry::RegisterStatusBarExtension("scene.node", [](const StatusBarExtensionContext& ctx) {
        if (ctx.region != StatusBarExtensionRegion::Right || !ctx.active_tab) {
            return;
        }

        const auto& colors = GetWorkbenchTheme().colors;
        const bool secondary_visible = SceneState::GetOrInit(*ctx.active_tab, "scene.layout.secondary.visible", "true") == "true";
        const bool panel_visible = SceneState::GetOrInit(*ctx.active_tab, "scene.layout.panel.visible", "true") == "true";
        const auto& state = NodeEditor::GetState(*ctx.active_tab);

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

        draw_chip("NODE");
        ImGui::SameLine();
        draw_chip(secondary_visible ? "Secondary:On" : "Secondary:Off");
        ImGui::SameLine();
        draw_chip(panel_visible ? "Panel:On" : "Panel:Off");
        ImGui::SameLine();
        draw_chip("Run");
        ImGui::SameLine();
        ImGui::Text("N:%d L:%d", static_cast<int>(state.nodes.size()), static_cast<int>(state.links.size()));
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "%.2f ms", static_cast<float>(state.node_exec_last_ms));
    });
    return true;
}();

const bool kRegisteredNodeTitleBarActions = []() {
    Scenes::ScenePluginRegistry::RegisterTitleBarActionHandler("scene.node", [](EditorTab& tab, const std::string& action_id) {
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
        if (action_id == "scene.execute.single") {
            ExecuteNodeGraph(tab, false);
            return;
        }
        if (action_id == "scene.execute.parallel") {
            ExecuteNodeGraph(tab, true);
            return;
        }
    });
    return true;
}();

const bool kRegisteredNodeTitleBarExtensionWidth = []() {
    Scenes::ScenePluginRegistry::RegisterTitleBarExtensionWidthResolver("scene.node", [](float title_h, const EditorTab*) {
        (void)title_h;
        const float btn_w = GetWorkbenchTheme().sizes.title_layout_btn_w;
        return btn_w * 2.0f;
    });
    return true;
}();

const bool kRegisteredNodePrimarySidebarDebugProvider = []() {
    Scenes::ScenePluginRegistry::RegisterPrimarySidebarDebugDataProvider("scene.node", [](const EditorTab* active_tab) {
        Scenes::ScenePrimarySidebarDebugData data;
        if (!active_tab || active_tab->scene_plugin_id != "scene.node") {
            return data;
        }

        const auto& node_state = NodeEditor::GetState(*active_tab);
        data.variables.push_back({ "scene", active_tab->scene_plugin_id });
        data.variables.push_back({ "tab", active_tab->name });
        data.variables.push_back({ "nodes", std::to_string(node_state.nodes.size()) });
        data.variables.push_back({ "links", std::to_string(node_state.links.size()) });
        data.variables.push_back({ "selectedNodeId", std::to_string(node_state.selected_node_id) });
        data.variables.push_back({ "zoom", std::to_string(node_state.node_canvas_zoom) });
        data.variables.push_back({ "lastExecMs", std::to_string(node_state.node_exec_last_ms) });

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
            int watch_count = 0;
            for (const auto& it : node_state.node_exec_outputs) {
                if (watch_count >= 8) {
                    break;
                }
                watch_exprs.push_back("node[" + std::to_string(it.first) + "]");
                ++watch_count;
            }
            if (watch_exprs.empty()) {
                watch_exprs = { "nodes", "links", "selectedNodeId", "lastExecMs" };
            }
        }

        for (const std::string& expr : watch_exprs) {
            std::string value = "<unknown>";
            if (expr == "nodes") value = std::to_string(node_state.nodes.size());
            else if (expr == "links") value = std::to_string(node_state.links.size());
            else if (expr == "selectedNodeId") value = std::to_string(node_state.selected_node_id);
            else if (expr == "zoom") value = std::to_string(node_state.node_canvas_zoom);
            else if (expr == "lastExecMs") value = std::to_string(node_state.node_exec_last_ms);
            else if (expr.size() > 6 && expr.rfind("node[", 0) == 0 && expr.back() == ']') {
                const std::string id_str = expr.substr(5, expr.size() - 6);
                const int node_id = std::atoi(id_str.c_str());
                const auto out_it = node_state.node_exec_outputs.find(node_id);
                if (out_it == node_state.node_exec_outputs.end()) {
                    value = "<missing>";
                } else if (out_it->second.empty()) {
                    value = "<empty>";
                } else {
                    value = out_it->second.front();
                }
            }
            data.watches.push_back({ expr, value });
        }

        data.callstack.push_back({ "NodeGraph::ExecuteGraph", "scene.node", true });
        data.callstack.push_back({ "NodeEditorViews::RenderCanvas", active_tab->name, false });
        data.callstack.push_back({ "WorkbenchRenderer::RenderEditorArea", "workbench", false });
        return data;
    });
    return true;
}();

}

void RenderCanvas(const SceneContext& ctx, EditorTab& tab)
{
    (void)kRegisteredNodeRenderer;
    auto& node_state = NodeEditor::GetState(tab);
    ImVec2 content_min = ctx.content_min;
    ImVec2 content_max = ctx.content_max;
    const WorkbenchThemeColors& wb_colors = ctx.theme.colors;
    const NodeEditorTheme::Theme& scene_theme = NodeEditorTheme::Get();
    const NodeEditorTheme::Colors& scene_colors = scene_theme.colors;
    const NodeEditorTheme::Sizes& scene_sizes = scene_theme.sizes;
    const float secondary_w = 320.0f;
    const float panel_h = 190.0f;
    const SceneCanvasLayout layout = ComputeCanvasLayout(ctx, 700.0f, 460.0f, secondary_w, panel_h);
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

    ImVec2 canvas_size = ImVec2(content_max.x - content_min.x, content_max.y - content_min.y);
    draw_list->AddRectFilled(content_min, content_max, ImGui::GetColorU32(scene_colors.bg));

    ImGui::SetCursorScreenPos(content_min);
    ImGui::SetNextItemAllowOverlap();
    ImGui::InvisibleButton("node_canvas", canvas_size,
        ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle);

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 mouse_pos = io.MousePos;
    bool in_canvas = (mouse_pos.x >= content_min.x && mouse_pos.x <= content_max.x &&
                      mouse_pos.y >= content_min.y && mouse_pos.y <= content_max.y);
    bool canvas_hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlappedByItem);

    const float overlay_padding = scene_sizes.overlay_padding;
    const ImVec2 button_size = ImVec2(34.0f, 30.0f);
    const float spacing = 8.0f;
    ImVec2 percent_size = ImGui::CalcTextSize("100");
    float stack_width = std::max(button_size.x, percent_size.x) + overlay_padding * 2.0f;
    float stack_height = button_size.y * 3.0f + spacing * 2.0f + percent_size.y + overlay_padding * 2.0f;
    ImVec2 stack_pos = ImVec2(content_max.x - stack_width - overlay_padding, content_min.y + overlay_padding);
    ImVec2 stack_max = ImVec2(stack_pos.x + stack_width, stack_pos.y + stack_height);
    bool over_overlay = (mouse_pos.x >= stack_pos.x && mouse_pos.x <= stack_max.x &&
                         mouse_pos.y >= stack_pos.y && mouse_pos.y <= stack_max.y);

    bool space_down = ImGui::IsKeyDown(ImGuiKey_Space);
    bool left_down = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    bool middle_down = ImGui::IsMouseDown(ImGuiMouseButton_Middle);
    bool right_down = ImGui::IsMouseDown(ImGuiMouseButton_Right);

    auto apply_zoom = [&](float zoom_scale, ImVec2 pivot) {
        float old_zoom = node_state.node_canvas_zoom;
        float new_zoom = std::clamp(old_zoom * zoom_scale, scene_sizes.zoom_min, scene_sizes.zoom_max);
        if (new_zoom == old_zoom) {
            return;
        }
        ImVec2 pivot_to_canvas = ImVec2((pivot.x - content_min.x) / old_zoom - node_state.node_canvas_pan.x,
                                        (pivot.y - content_min.y) / old_zoom - node_state.node_canvas_pan.y);
        node_state.node_canvas_zoom = new_zoom;
        node_state.node_canvas_pan.x = (pivot.x - content_min.x) / new_zoom - pivot_to_canvas.x;
        node_state.node_canvas_pan.y = (pivot.y - content_min.y) / new_zoom - pivot_to_canvas.y;
    };

    bool allow_input = in_canvas && canvas_hovered && !over_overlay;
    bool drag_enabled = allow_input && (middle_down || right_down || (left_down && space_down));
    if (drag_enabled) {
        float inv_zoom = (node_state.node_canvas_zoom > 0.0f) ? (1.0f / node_state.node_canvas_zoom) : 1.0f;
        node_state.node_canvas_pan.x += io.MouseDelta.x * inv_zoom;
        node_state.node_canvas_pan.y += io.MouseDelta.y * inv_zoom;
    }

    if (allow_input && io.MouseWheel != 0.0f) {
        float wheel = std::clamp(io.MouseWheel, -3.0f, 3.0f);
        apply_zoom(1.0f + wheel * scene_sizes.zoom_step, io.MousePos);
    }

    auto screen_to_canvas = [&](ImVec2 screen_pos) {
        return ImVec2((screen_pos.x - content_min.x) / node_state.node_canvas_zoom - node_state.node_canvas_pan.x,
                      (screen_pos.y - content_min.y) / node_state.node_canvas_zoom - node_state.node_canvas_pan.y);
    };
    auto canvas_to_screen = [&](ImVec2 canvas_pos) {
        return ImVec2(content_min.x + (canvas_pos.x + node_state.node_canvas_pan.x) * node_state.node_canvas_zoom,
                      content_min.y + (canvas_pos.y + node_state.node_canvas_pan.y) * node_state.node_canvas_zoom);
    };

    auto find_node_by_id = [&](int id) -> NodeEditor::Node* {
        for (auto& node : node_state.nodes) {
            if (node.id == id) {
                return &node;
            }
        }
        return nullptr;
    };

    float base_font_size = ImGui::GetFontSize();
    float font_size = base_font_size * node_state.node_canvas_zoom;
    float title_h = font_size * 1.8f;
    float port_h = font_size * 1.25f;
    float vert_top = font_size * 0.5f;
    float vert_bottom = font_size * 0.5f;
    float horiz_pad = font_size * 1.7f;
    float default_width = 160.0f;
    float port_radius = 0.6f * font_size * 0.5f;

    auto port_center_screen = [&](const NodeEditor::Node& node, bool is_output, int port_index, float local_port_radius) -> ImVec2 {
        ImVec2 node_pos = canvas_to_screen(node.pos);
        ImVec2 node_size = ImVec2(node.size.x * node_state.node_canvas_zoom, node.size.y * node_state.node_canvas_zoom);
        float x = is_output ? (node_pos.x + node_size.x - local_port_radius) : (node_pos.x + local_port_radius);
        float y = node_pos.y + title_h + vert_top + port_h * 0.5f + port_index * port_h;
        return ImVec2(x, y);
    };

    auto hit_test_port = [&](ImVec2 mouse, float local_port_radius) -> std::tuple<int, int, bool> {
        for (auto it = node_state.nodes.rbegin(); it != node_state.nodes.rend(); ++it) {
            const auto& node = *it;
            for (int i = 0; i < static_cast<int>(node.outputs.size()); ++i) {
                ImVec2 c = port_center_screen(node, true, i, local_port_radius);
                ImVec2 d = ImVec2(c.x - mouse.x, c.y - mouse.y);
                if ((d.x * d.x + d.y * d.y) <= local_port_radius * local_port_radius) {
                    return { node.id, i, true };
                }
            }
            for (int i = 0; i < static_cast<int>(node.inputs.size()); ++i) {
                ImVec2 c = port_center_screen(node, false, i, local_port_radius);
                ImVec2 d = ImVec2(c.x - mouse.x, c.y - mouse.y);
                if ((d.x * d.x + d.y * d.y) <= local_port_radius * local_port_radius) {
                    return { node.id, i, false };
                }
            }
        }
        return { 0, -1, false };
    };

    auto hit_test_node = [&](ImVec2 mouse) -> int {
        for (auto it = node_state.nodes.rbegin(); it != node_state.nodes.rend(); ++it) {
            const auto& node = *it;
            ImVec2 node_pos = canvas_to_screen(node.pos);
            ImVec2 node_size = ImVec2(node.size.x * node_state.node_canvas_zoom, node.size.y * node_state.node_canvas_zoom);
            ImVec2 node_max = ImVec2(node_pos.x + node_size.x, node_pos.y + node_size.y);
            if (mouse.x >= node_pos.x && mouse.x <= node_max.x && mouse.y >= node_pos.y && mouse.y <= node_max.y) {
                return node.id;
            }
        }
        return 0;
    };

    auto update_node_layout = [&]() {
        for (auto& node : node_state.nodes) {
            int max_ports = static_cast<int>(std::max(node.inputs.size(), node.outputs.size()));
            float body_h = std::max(1, max_ports) * port_h + vert_top + vert_bottom;
            float total_h = title_h + body_h;
            if (node.size.y * node_state.node_canvas_zoom < total_h) {
                node.size.y = total_h / node_state.node_canvas_zoom;
            }
            float text_w = ImGui::CalcTextSize(node.title.c_str()).x;
            float min_w = std::max(default_width, text_w + horiz_pad * 2.0f);
            if (node.size.x < min_w) {
                node.size.x = min_w;
            }
        }
    };

    update_node_layout();

    auto is_input_connected = [&](int node_id, int local_port_index) {
        for (const auto& link : node_state.links) {
            if (link.to_node == node_id && link.to_port == local_port_index) {
                return true;
            }
        }
        return false;
    };

    auto is_output_connected = [&](int node_id, int local_port_index) {
        for (const auto& link : node_state.links) {
            if (link.from_node == node_id && link.from_port == local_port_index) {
                return true;
            }
        }
        return false;
    };

    auto bezier_point = [](ImVec2 p1, ImVec2 p2, ImVec2 p3, ImVec2 p4, float t) {
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * t;
        ImVec2 p = ImVec2(0.0f, 0.0f);
        p.x = uuu * p1.x + 3.0f * uu * t * p2.x + 3.0f * u * tt * p3.x + ttt * p4.x;
        p.y = uuu * p1.y + 3.0f * uu * t * p2.y + 3.0f * u * tt * p3.y + ttt * p4.y;
        return p;
    };

    auto dist_to_segment = [](ImVec2 p, ImVec2 a, ImVec2 b) {
        ImVec2 ab = ImVec2(b.x - a.x, b.y - a.y);
        ImVec2 ap = ImVec2(p.x - a.x, p.y - a.y);
        float ab_len2 = ab.x * ab.x + ab.y * ab.y;
        float t = (ab_len2 > 0.0f) ? (ap.x * ab.x + ap.y * ab.y) / ab_len2 : 0.0f;
        t = std::clamp(t, 0.0f, 1.0f);
        ImVec2 proj = ImVec2(a.x + ab.x * t, a.y + ab.y * t);
        ImVec2 d = ImVec2(p.x - proj.x, p.y - proj.y);
        return std::sqrt(d.x * d.x + d.y * d.y);
    };

    auto hit_test_link = [&](ImVec2 mouse) -> int {
        const int samples = 20;
        const float threshold = 6.0f;
        for (int i = static_cast<int>(node_state.links.size()) - 1; i >= 0; --i) {
            const auto& link = node_state.links[i];
            const NodeEditor::Node* from_node = nullptr;
            const NodeEditor::Node* to_node = nullptr;
            for (const auto& node : node_state.nodes) {
                if (node.id == link.from_node) {
                    from_node = &node;
                }
                if (node.id == link.to_node) {
                    to_node = &node;
                }
            }
            if (!from_node || !to_node) {
                continue;
            }
            if (link.from_port < 0 || link.from_port >= static_cast<int>(from_node->outputs.size()) ||
                link.to_port < 0 || link.to_port >= static_cast<int>(to_node->inputs.size())) {
                continue;
            }
            float local_font_size = ImGui::GetFontSize() * node_state.node_canvas_zoom;
            float local_port_radius = 0.6f * local_font_size * 0.5f;
            ImVec2 p1 = port_center_screen(*from_node, true, link.from_port, local_port_radius);
            ImVec2 p4 = port_center_screen(*to_node, false, link.to_port, local_port_radius);
            float dx = std::abs(p4.x - p1.x) * 0.5f;
            ImVec2 p2 = ImVec2(p1.x + dx, p1.y);
            ImVec2 p3 = ImVec2(p4.x - dx, p4.y);

            ImVec2 prev = p1;
            bool hit = false;
            for (int s = 1; s <= samples; ++s) {
                float t = static_cast<float>(s) / static_cast<float>(samples);
                ImVec2 curr = bezier_point(p1, p2, p3, p4, t);
                if (dist_to_segment(mouse, prev, curr) <= threshold) {
                    hit = true;
                    break;
                }
                prev = curr;
            }
            if (hit) {
                return i;
            }
        }
        return -1;
    };

    bool request_add_node = false;
    ImVec2 request_node_pos = ImVec2(0.0f, 0.0f);
    if (allow_input && ImGui::IsKeyPressed(ImGuiKey_N)) {
        request_add_node = true;
        request_node_pos = screen_to_canvas(io.MousePos);
    }

    if (node_state.request_add_node_from_library && !node_state.pending_node_type.empty()) {
        request_add_node = true;
        request_node_pos = screen_to_canvas(ImVec2((content_min.x + content_max.x) * 0.5f,
                                                   (content_min.y + content_max.y) * 0.5f));
    }

    if (request_add_node) {
        auto configure_ports = [](NodeEditor::Node& node, const std::string& type) {
            node.inputs.clear();
            node.outputs.clear();
            if (type == "Gain") {
                node.inputs.push_back({ "In", NodeEditor::NodePortType::Double });
                node.outputs.push_back({ "Out", NodeEditor::NodePortType::Double });
            } else if (type == "Test") {
                node.inputs.push_back({ "In1", NodeEditor::NodePortType::Float });
                node.inputs.push_back({ "In2", NodeEditor::NodePortType::Double });
                node.inputs.push_back({ "In3", NodeEditor::NodePortType::Int });
                node.inputs.push_back({ "In4", NodeEditor::NodePortType::Double });
                node.outputs.push_back({ "Out1", NodeEditor::NodePortType::Int });
                node.outputs.push_back({ "Out2", NodeEditor::NodePortType::Double });
                node.outputs.push_back({ "Out3", NodeEditor::NodePortType::Generic });
            } else if (type == "If") {
                node.inputs.push_back({ "Cond", NodeEditor::NodePortType::Bool });
                node.inputs.push_back({ "Value", NodeEditor::NodePortType::Generic });
                node.outputs.push_back({ "Out", NodeEditor::NodePortType::Generic });
            } else if (type == "Switch") {
                node.inputs.push_back({ "A", NodeEditor::NodePortType::Generic });
                node.inputs.push_back({ "B", NodeEditor::NodePortType::Generic });
                node.inputs.push_back({ "Sel", NodeEditor::NodePortType::Bool });
                node.outputs.push_back({ "Out", NodeEditor::NodePortType::Generic });
            } else {
                node.inputs.push_back({ "In", NodeEditor::NodePortType::Generic });
                node.outputs.push_back({ "Out", NodeEditor::NodePortType::Generic });
            }
        };

        NodeEditor::Node node;
        node.id = node_state.node_next_id++;
        if (!node_state.pending_node_type.empty()) {
            node.title = node_state.pending_node_type + " " + std::to_string(node.id);
        } else {
            node.title = "Node " + std::to_string(node.id);
        }
        node.type = node_state.pending_node_type.empty() ? "Generic" : node_state.pending_node_type;
        node.pos = request_node_pos;
        configure_ports(node, node_state.pending_node_type);
        node_state.nodes.push_back(node);
        node_state.request_add_node_from_library = false;
        node_state.pending_node_type.clear();
    }

    if (allow_input && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !space_down) {
        float local_port_radius = 0.6f * font_size * 0.5f;
        auto [port_node_id, port_index, is_output] = hit_test_port(mouse_pos, local_port_radius);
        if (port_node_id != 0 && is_output) {
            node_state.linking = true;
            node_state.link_from_node_id = port_node_id;
            node_state.link_from_port = port_index;
        } else {
            int node_id = hit_test_node(mouse_pos);
            if (node_id != 0) {
                node_state.selected_node_id = node_id;
                node_state.dragging_node_id = node_id;
                if (auto node = find_node_by_id(node_id)) {
                    ImVec2 node_pos = canvas_to_screen(node->pos);
                    node_state.dragging_node_offset = ImVec2(mouse_pos.x - node_pos.x, mouse_pos.y - node_pos.y);
                } else {
                    node_state.dragging_node_offset = ImVec2(0.0f, 0.0f);
                }
            } else {
                node_state.selected_node_id = 0;
            }
        }
    }

    if (allow_input && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        float local_port_radius = 0.6f * font_size * 0.5f;
        auto [port_node_id, port_index, is_output] = hit_test_port(mouse_pos, local_port_radius);
        if (port_node_id != 0) {
            if (is_output) {
                node_state.links.erase(std::remove_if(node_state.links.begin(), node_state.links.end(), [&](const NodeEditor::Link& link) {
                    return link.from_node == port_node_id && link.from_port == port_index;
                }), node_state.links.end());
            } else {
                node_state.links.erase(std::remove_if(node_state.links.begin(), node_state.links.end(), [&](const NodeEditor::Link& link) {
                    return link.to_node == port_node_id && link.to_port == port_index;
                }), node_state.links.end());
            }
        } else {
            int link_index = hit_test_link(mouse_pos);
            if (link_index >= 0) {
                node_state.links.erase(node_state.links.begin() + link_index);
            }
        }
    }

    if (node_state.dragging_node_id != 0 && ImGui::IsMouseDown(ImGuiMouseButton_Left) && !space_down) {
        if (auto node = find_node_by_id(node_state.dragging_node_id)) {
            ImVec2 new_screen = ImVec2(mouse_pos.x - node_state.dragging_node_offset.x, mouse_pos.y - node_state.dragging_node_offset.y);
            node->pos = screen_to_canvas(new_screen);
        }
    }
    if (node_state.dragging_node_id != 0 && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        node_state.dragging_node_id = 0;
    }

    if (node_state.linking && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        float local_port_radius = 0.6f * font_size * 0.5f;
        auto [port_node_id, port_index, is_output] = hit_test_port(mouse_pos, local_port_radius);
        if (port_node_id != 0 && !is_output && node_state.link_from_node_id != 0 && node_state.link_from_port >= 0) {
            node_state.links.erase(std::remove_if(node_state.links.begin(), node_state.links.end(), [&](const NodeEditor::Link& link) {
                return link.to_node == port_node_id && link.to_port == port_index;
            }), node_state.links.end());

            NodeEditor::Link link;
            link.from_node = node_state.link_from_node_id;
            link.from_port = node_state.link_from_port;
            link.to_node = port_node_id;
            link.to_port = port_index;
            node_state.links.push_back(link);
        }
        node_state.linking = false;
        node_state.link_from_node_id = 0;
        node_state.link_from_port = -1;
    }

    if (allow_input && ImGui::IsKeyPressed(ImGuiKey_Delete) && node_state.selected_node_id != 0) {
        int node_id = node_state.selected_node_id;
        node_state.links.erase(std::remove_if(node_state.links.begin(), node_state.links.end(), [&](const NodeEditor::Link& link) {
            return link.from_node == node_id || link.to_node == node_id;
        }), node_state.links.end());
        node_state.nodes.erase(std::remove_if(node_state.nodes.begin(), node_state.nodes.end(), [&](const NodeEditor::Node& node) {
            return node.id == node_id;
        }), node_state.nodes.end());
        node_state.selected_node_id = 0;
        node_state.dragging_node_id = 0;
        node_state.linking = false;
        node_state.link_from_node_id = 0;
        node_state.link_from_port = -1;
    }

    if (!std::isfinite(node_state.node_canvas_pan.x) || !std::isfinite(node_state.node_canvas_pan.y) ||
        !std::isfinite(node_state.node_canvas_zoom)) {
        node_state.node_canvas_pan = ImVec2(0.0f, 0.0f);
        node_state.node_canvas_zoom = 1.0f;
    }

    ImVec2 grid_pan = ImVec2(node_state.node_canvas_pan.x * node_state.node_canvas_zoom,
                             node_state.node_canvas_pan.y * node_state.node_canvas_zoom);
    const float pan_wrap = scene_sizes.grid_size * 1024.0f;
    if (pan_wrap > 0.0f) {
        grid_pan.x = std::fmod(grid_pan.x, pan_wrap);
        grid_pan.y = std::fmod(grid_pan.y, pan_wrap);
    }

    draw_list->PushClipRect(content_min, content_max, true);
    const float base_grid = scene_sizes.grid_size;
    const float grid_size = base_grid * node_state.node_canvas_zoom;
    const float grid_draw_min = scene_sizes.grid_draw_min;
    const float grid_draw_max = scene_sizes.grid_draw_max;
    float draw_grid_size = std::clamp(grid_size, grid_draw_min, grid_draw_max);
    if (draw_grid_size <= 0.0f) {
        draw_grid_size = 1.0f;
    }
    float draw_major_grid_size = std::clamp(draw_grid_size * 4.0f, grid_draw_min * 4.0f, grid_draw_max * 4.0f);
    if (draw_major_grid_size <= 0.0f) {
        draw_major_grid_size = draw_grid_size * 4.0f;
    }

    auto grid_start = [](float min_pos, float pan, float spacing) {
        float offset = std::fmod(pan, spacing);
        if (offset < 0.0f) offset += spacing;
        return min_pos + offset;
    };

    ImU32 minor_col = ImGui::GetColorU32(scene_colors.grid_minor);
    ImU32 major_col = ImGui::GetColorU32(scene_colors.grid_major);

    bool draw_minor = draw_grid_size >= (grid_draw_min * scene_sizes.grid_minor_threshold);
    if (draw_minor) {
        float start_x = grid_start(content_min.x, grid_pan.x, draw_grid_size);
        for (float x = start_x; x <= content_max.x; x += draw_grid_size) {
            draw_list->AddLine(ImVec2(x, content_min.y), ImVec2(x, content_max.y), minor_col, scene_sizes.grid_thickness);
        }
        float start_y = grid_start(content_min.y, grid_pan.y, draw_grid_size);
        for (float y = start_y; y <= content_max.y; y += draw_grid_size) {
            draw_list->AddLine(ImVec2(content_min.x, y), ImVec2(content_max.x, y), minor_col, scene_sizes.grid_thickness);
        }
    }

    {
        float start_x = grid_start(content_min.x, grid_pan.x, draw_major_grid_size);
        for (float x = start_x; x <= content_max.x; x += draw_major_grid_size) {
            draw_list->AddLine(ImVec2(x, content_min.y), ImVec2(x, content_max.y), major_col, scene_sizes.grid_major_thickness);
        }
        float start_y = grid_start(content_min.y, grid_pan.y, draw_major_grid_size);
        for (float y = start_y; y <= content_max.y; y += draw_major_grid_size) {
            draw_list->AddLine(ImVec2(content_min.x, y), ImVec2(content_max.x, y), major_col, scene_sizes.grid_major_thickness);
        }
    }

    ImU32 node_body = IM_COL32(0, 0, 0, 191);
    ImU32 node_head = IM_COL32(26, 38, 77, 255);
    ImU32 node_line = IM_COL32(106, 4, 15, 191);
    ImU32 node_outline = IM_COL32(51, 77, 153, 255);
    ImU32 node_outline_active = IM_COL32(80, 120, 220, 255);
    ImU32 node_text = IM_COL32(255, 255, 255, 255);
    ImU32 link_col = IM_COL32(255, 255, 255, 255);
    ImU32 port_bg = IM_COL32(0, 117, 99, 255);
    ImU32 port_ring = IM_COL32(87, 168, 196, 255);
    ImU32 port_inner = IM_COL32(255, 255, 255, 255);
    ImU32 port_active = IM_COL32(254, 110, 0, 255);
    const float title_pad = 6.0f;

    float hover_port_radius = 0.6f * font_size * 0.5f;
    auto [hover_port_node, hover_port_index, hover_is_output] = hit_test_port(mouse_pos, hover_port_radius);

    for (const auto& link : node_state.links) {
        const NodeEditor::Node* from_node = nullptr;
        const NodeEditor::Node* to_node = nullptr;
        for (const auto& node : node_state.nodes) {
            if (node.id == link.from_node) {
                from_node = &node;
            }
            if (node.id == link.to_node) {
                to_node = &node;
            }
        }
        if (!from_node || !to_node) {
            continue;
        }
        if (link.from_port < 0 || link.from_port >= static_cast<int>(from_node->outputs.size()) ||
            link.to_port < 0 || link.to_port >= static_cast<int>(to_node->inputs.size())) {
            continue;
        }
        float link_font_size = ImGui::GetFontSize() * node_state.node_canvas_zoom;
        float local_port_radius = 0.6f * link_font_size * 0.5f;
        ImVec2 p1 = port_center_screen(*from_node, true, link.from_port, local_port_radius);
        ImVec2 p4 = port_center_screen(*to_node, false, link.to_port, local_port_radius);
        float dx = std::abs(p4.x - p1.x) * 0.5f;
        ImVec2 p2 = ImVec2(p1.x + dx, p1.y);
        ImVec2 p3 = ImVec2(p4.x - dx, p4.y);
        draw_list->AddBezierCubic(p1, p2, p3, p4, link_col, 2.0f);
    }

    if (node_state.linking && node_state.link_from_node_id != 0 && node_state.link_from_port >= 0) {
        if (auto from_node = find_node_by_id(node_state.link_from_node_id)) {
            if (node_state.link_from_port < static_cast<int>(from_node->outputs.size())) {
                float local_font_size = ImGui::GetFontSize() * node_state.node_canvas_zoom;
                float local_port_radius = 0.6f * local_font_size * 0.5f;
                ImVec2 p1 = port_center_screen(*from_node, true, node_state.link_from_port, local_port_radius);
                ImVec2 p4 = mouse_pos;
                float dx = std::abs(p4.x - p1.x) * 0.5f;
                ImVec2 p2 = ImVec2(p1.x + dx, p1.y);
                ImVec2 p3 = ImVec2(p4.x - dx, p4.y);
                draw_list->AddBezierCubic(p1, p2, p3, p4, link_col, 2.0f);
            }
        }
    }

    ImGui::SetWindowFontScale(node_state.node_canvas_zoom);
    for (auto& node : node_state.nodes) {
        float port_thickness = 0.1f * font_size;

        ImVec2 node_pos = canvas_to_screen(node.pos);
        ImVec2 node_size = ImVec2(node.size.x * node_state.node_canvas_zoom, node.size.y * node_state.node_canvas_zoom);
        ImVec2 node_max = ImVec2(node_pos.x + node_size.x, node_pos.y + node_size.y);
        float rounding = title_h * 0.3f;

        draw_list->AddRectFilled(node_pos, node_max, node_body, rounding);
        ImVec2 head_br = ImVec2(node_max.x, node_pos.y + title_h);
        draw_list->AddRectFilled(node_pos, head_br, node_head, rounding);
        draw_list->AddLine(ImVec2(node_pos.x, head_br.y), ImVec2(head_br.x - 1.0f, head_br.y), node_line, 2.0f);

        bool selected = (node.id == node_state.selected_node_id);
        draw_list->AddRect(node_pos, node_max, selected ? node_outline_active : node_outline, rounding, 0, selected ? 2.0f : 1.5f);

        ImVec2 text_pos = ImVec2(node_pos.x + title_pad * node_state.node_canvas_zoom, node_pos.y + title_pad * node_state.node_canvas_zoom);
        draw_list->AddText(ImVec2(text_pos.x + 2.0f, text_pos.y + 2.0f), IM_COL32(0, 0, 0, 255), node.title.c_str());
        draw_list->AddText(text_pos, node_text, node.title.c_str());

        for (int i = 0; i < static_cast<int>(node.inputs.size()); ++i) {
            ImVec2 c = port_center_screen(node, false, i, port_radius);
            bool connected = is_input_connected(node.id, i);
            bool hovered = (hover_port_node == node.id && hover_port_index == i && !hover_is_output);
            bool should_connect = node_state.linking && hovered;
            draw_list->AddCircleFilled(c, port_radius, port_bg, 0);
            draw_list->AddCircle(c, port_radius, port_ring, 0, port_thickness);
            if (connected) {
                draw_list->AddCircleFilled(c, port_radius * 0.5f, port_inner, 0);
            }
            if (hovered || should_connect) {
                draw_list->AddCircleFilled(c, port_radius * 0.8f, port_active, 0);
            }
        }
        for (int i = 0; i < static_cast<int>(node.outputs.size()); ++i) {
            ImVec2 c = port_center_screen(node, true, i, port_radius);
            bool connected = is_output_connected(node.id, i);
            bool hovered = (hover_port_node == node.id && hover_port_index == i && hover_is_output);
            bool should_connect = node_state.linking && hovered;
            draw_list->AddCircleFilled(c, port_radius, port_bg, 0);
            draw_list->AddCircle(c, port_radius, port_ring, 0, port_thickness);
            if (connected) {
                draw_list->AddCircleFilled(c, port_radius * 0.5f, port_inner, 0);
            }
            if (hovered || should_connect) {
                draw_list->AddCircleFilled(c, port_radius * 0.8f, port_active, 0);
            }
        }
    }
    ImGui::SetWindowFontScale(1.0f);

    draw_list->PopClipRect();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, scene_sizes.overlay_rounding);
    ImGui::PushStyleColor(ImGuiCol_Button, scene_colors.overlay_bg);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, scene_colors.overlay_hover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, scene_colors.overlay_active);
    ImGui::PushStyleColor(ImGuiCol_Text, scene_colors.overlay_text);

    ImDrawList* overlay_draw = ImGui::GetWindowDrawList();
    ImVec4 overlay_bg = scene_colors.overlay_bg;
    overlay_bg.w = std::min(1.0f, overlay_bg.w + 0.1f);

    auto draw_icon_plus = [&](ImVec2 p0, ImVec2 p1) {
        ImVec2 c = ImVec2((p0.x + p1.x) * 0.5f, (p0.y + p1.y) * 0.5f);
        float r = (p1.x - p0.x) * 0.25f;
        ImU32 col = ImGui::GetColorU32(scene_colors.overlay_text);
        overlay_draw->AddLine(ImVec2(c.x - r, c.y), ImVec2(c.x + r, c.y), col, 2.0f);
        overlay_draw->AddLine(ImVec2(c.x, c.y - r), ImVec2(c.x, c.y + r), col, 2.0f);
    };
    auto draw_icon_minus = [&](ImVec2 p0, ImVec2 p1) {
        ImVec2 c = ImVec2((p0.x + p1.x) * 0.5f, (p0.y + p1.y) * 0.5f);
        float r = (p1.x - p0.x) * 0.25f;
        ImU32 col = ImGui::GetColorU32(scene_colors.overlay_text);
        overlay_draw->AddLine(ImVec2(c.x - r, c.y), ImVec2(c.x + r, c.y), col, 2.0f);
    };
    auto draw_icon_reset = [&](ImVec2 p0, ImVec2 p1) {
        ImVec2 c = ImVec2((p0.x + p1.x) * 0.5f, (p0.y + p1.y) * 0.5f);
        float r = (p1.x - p0.x) * 0.25f;
        ImU32 col = ImGui::GetColorU32(scene_colors.overlay_text);
        overlay_draw->AddCircle(ImVec2(c.x, c.y), r, col, 24, 2.0f);
        overlay_draw->AddLine(ImVec2(c.x, c.y - r * 0.9f), ImVec2(c.x + r * 0.6f, c.y - r * 0.6f), col, 2.0f);
    };

    auto icon_button = [&](const char* id, ImVec2 pos, auto draw_icon) -> bool {
        ImGui::SetCursorScreenPos(pos);
        bool pressed = ImGui::Button(id, button_size);
        ImVec2 p0 = ImGui::GetItemRectMin();
        ImVec2 p1 = ImGui::GetItemRectMax();
        ImU32 bg_col = ImGui::GetColorU32(ImGui::IsItemActive() ? scene_colors.overlay_active
            : (ImGui::IsItemHovered() ? scene_colors.overlay_hover : overlay_bg));
        overlay_draw->AddRectFilled(p0, p1, bg_col, scene_sizes.overlay_rounding);
        draw_icon(p0, p1);
        return pressed;
    };

    ImVec2 zoom_pivot = ImVec2((content_min.x + content_max.x) * 0.5f, (content_min.y + content_max.y) * 0.5f);

    ImVec2 button_pos = ImVec2(stack_pos.x + overlay_padding, stack_pos.y + overlay_padding);
    if (icon_button("##zoom_in", button_pos, draw_icon_plus)) {
        apply_zoom(1.0f + scene_sizes.zoom_step, zoom_pivot);
    }
    button_pos.y += button_size.y + spacing;
    if (icon_button("##zoom_out", button_pos, draw_icon_minus)) {
        apply_zoom(1.0f - scene_sizes.zoom_step, zoom_pivot);
    }
    button_pos.y += button_size.y + spacing;
    if (icon_button("##zoom_reset", button_pos, draw_icon_reset)) {
        node_state.node_canvas_pan = ImVec2(0.0f, 0.0f);
        node_state.node_canvas_zoom = 1.0f;
    }
    button_pos.y += button_size.y + spacing;
    ImVec2 percent_pos = ImVec2(button_pos.x, button_pos.y);
    ImVec2 percent_max = ImVec2(button_pos.x + button_size.x, button_pos.y + button_size.y);
    overlay_draw->AddRectFilled(percent_pos, percent_max, ImGui::GetColorU32(overlay_bg), scene_sizes.overlay_rounding);
    const float zoom_percent = node_state.node_canvas_zoom * 100.0f;
    std::string percent_text = std::to_string((int)zoom_percent);
    ImVec2 percent_text_size = ImGui::CalcTextSize(percent_text.c_str());
    ImVec2 percent_text_pos = ImVec2(
        percent_pos.x + (button_size.x - percent_text_size.x) * 0.5f,
        percent_pos.y + (button_size.y - percent_text_size.y) * 0.5f
    );
    overlay_draw->AddText(percent_text_pos, ImGui::GetColorU32(scene_colors.overlay_text), percent_text.c_str());

    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar();

    if (secondary_visible) {
        RenderSecondarySidebarPanel(layout, wb_colors, content_max, content_min, full_max, tab);
    }
    if (panel_visible) {
        RenderBottomPanel(layout, wb_colors, content_min, content_max, full_max, tab);
    }
}

void RenderLibrary(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawHeader("NODE LIBRARY", colors.primary_sidebar_text_dim);
    const bool can_add = active_tab && active_tab->scene_plugin_id == "scene.node";

    auto draw_item = [&](const char* label) {
        if (ImGui::Selectable(label, false) && can_add) {
            auto& node_state = NodeEditor::GetState(*active_tab);
            node_state.pending_node_type = label;
            node_state.request_add_node_from_library = true;
        }
    };

    ImGui::Text("Math");
    draw_item("Gain");
    draw_item("Test");
    ImGui::Spacing();
    ImGui::Text("Logic");
    draw_item("If");
    draw_item("Switch");
}

void RenderOutline(ImVec2, ImVec2, EditorTab* active_tab)
{
    DrawOutlineBase(active_tab);
    ImGui::Spacing();
    ImGui::Text("Node Editor Outline");
}

void RenderProperties(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawHeader("PROPERTIES", colors.secondary_sidebar_text);
    if (active_tab) {
        ImGui::Text("Selection: %s", active_tab->name.c_str());
    }
    ImGui::TextWrapped("Select an element to inspect its properties.");
    ImGui::Spacing();
    ImGui::Text("Node Properties");

    if (!active_tab || active_tab->scene_plugin_id != "scene.node") {
        return;
    }

    auto& node_state = NodeEditor::GetState(*active_tab);
    NodeEditor::Node* selected = nullptr;
    if (node_state.selected_node_id != 0) {
        for (auto& node : node_state.nodes) {
            if (node.id == node_state.selected_node_id) {
                selected = &node;
                break;
            }
        }
    }

    if (!selected) {
        ImGui::TextColored(GetWorkbenchTheme().colors.secondary_sidebar_text, "No node selected");
        return;
    }

    ImGui::Separator();
    ImGui::Text("Name: %s", selected->title.c_str());
    ImGui::Text("Type: %s", selected->type.empty() ? "Generic" : selected->type.c_str());

    ImGui::Spacing();
    ImGui::Text("Inputs:");
    for (const auto& input : selected->inputs) {
        ImGui::BulletText("%s", input.name.c_str());
    }
    ImGui::Text("Outputs:");
    for (const auto& output : selected->outputs) {
        ImGui::BulletText("%s", output.name.c_str());
    }

    ImGui::Spacing();
    ImGui::Text("Lua Script (run(inputs))");

    const std::string default_script = NodeGraph::GetDefaultNodeScript(selected->type.empty() ? "Generic" : selected->type);
    if (selected->script.empty()) {
        selected->script = default_script;
    }
    InputTextMultilineStdString("##node_script", &selected->script, ImVec2(0, 140));
    if (ImGui::Button("Reset to Default")) {
        selected->script = default_script;
    }
}

void RenderPanelOutput(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    if (active_tab && active_tab->scene_plugin_id == "scene.node") {
        auto& node_state = NodeEditor::GetState(*active_tab);
        if (ImGui::Button("Run")) {
            ExecuteNodeGraph(*active_tab, false);
        }
        ImGui::SameLine();
        if (ImGui::Button("Run Parallel")) {
            ExecuteNodeGraph(*active_tab, true);
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Log")) {
            node_state.node_exec_log.clear();
            node_state.node_exec_outputs.clear();
            node_state.node_exec_last_error.clear();
        }

        ImGui::Separator();
        ImGui::Text("Last run: %s", node_state.node_exec_last_ok ? "OK" : "Error");
        ImGui::Text("Mode: %s", node_state.node_exec_last_parallel ? "Parallel" : "Single");
        ImGui::Text("Duration: %.2f ms", node_state.node_exec_last_ms);
        if (!node_state.node_exec_last_ok && !node_state.node_exec_last_error.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "%s", node_state.node_exec_last_error.c_str());
        }

        if (!node_state.node_exec_outputs.empty()) {
            ImGui::Spacing();
            ImGui::Text("Outputs:");
            for (const auto& entry : node_state.node_exec_outputs) {
                const int node_id = entry.first;
                const auto& outputs = entry.second;
                std::string label = "Node " + std::to_string(node_id);
                if (ImGui::TreeNode(label.c_str())) {
                    for (size_t i = 0; i < outputs.size(); ++i) {
                        ImGui::BulletText("Out%zu = %s", i + 1, outputs[i].c_str());
                    }
                    ImGui::TreePop();
                }
            }
        }

        ImGui::Spacing();
        ImGui::Text("Log:");
        ImGui::BeginChild("node_exec_log", ImVec2(0, 0), true);
        if (node_state.node_exec_log.empty()) {
            ImGui::TextColored(colors.panel_text, "No logs yet.");
        } else {
            for (const auto& line : node_state.node_exec_log) {
                ImGui::TextWrapped("%s", line.c_str());
            }
        }
        ImGui::EndChild();
        return;
    }

    ImGui::TextColored(colors.panel_text, "Build completed successfully");
}

} // namespace Scenes::NodeEditorViews

