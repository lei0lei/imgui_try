#include "scene_views.h"

#include "node_graph_executor.h"
#include "../../workbench/workbench_config.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <tuple>

namespace Scenes::NodeEditorViews {

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

} // namespace

void RenderCanvas(ImVec2 content_min, ImVec2 content_max, EditorTab& tab)
{
    const WorkbenchTheme& theme = GetWorkbenchTheme();
    const WorkbenchThemeColors& colors = theme.colors;
    const WorkbenchThemeSizes& sizes = theme.sizes;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 canvas_size = ImVec2(content_max.x - content_min.x, content_max.y - content_min.y);
    draw_list->AddRectFilled(content_min, content_max, ImGui::GetColorU32(colors.editor_node_bg));

    ImGui::SetCursorScreenPos(content_min);
    ImGui::SetNextItemAllowOverlap();
    ImGui::InvisibleButton("node_canvas", canvas_size,
        ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle);

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 mouse_pos = io.MousePos;
    bool in_canvas = (mouse_pos.x >= content_min.x && mouse_pos.x <= content_max.x &&
                      mouse_pos.y >= content_min.y && mouse_pos.y <= content_max.y);
    bool canvas_hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlappedByItem);

    const float overlay_padding = sizes.editor_node_overlay_padding;
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
        float old_zoom = tab.node_canvas_zoom;
        float new_zoom = std::clamp(old_zoom * zoom_scale, sizes.editor_node_zoom_min, sizes.editor_node_zoom_max);
        if (new_zoom == old_zoom) {
            return;
        }
        ImVec2 pivot_to_canvas = ImVec2((pivot.x - content_min.x) / old_zoom - tab.node_canvas_pan.x,
                                        (pivot.y - content_min.y) / old_zoom - tab.node_canvas_pan.y);
        tab.node_canvas_zoom = new_zoom;
        tab.node_canvas_pan.x = (pivot.x - content_min.x) / new_zoom - pivot_to_canvas.x;
        tab.node_canvas_pan.y = (pivot.y - content_min.y) / new_zoom - pivot_to_canvas.y;
    };

    bool allow_input = in_canvas && canvas_hovered && !over_overlay;
    bool drag_enabled = allow_input && (middle_down || right_down || (left_down && space_down));
    if (drag_enabled) {
        float inv_zoom = (tab.node_canvas_zoom > 0.0f) ? (1.0f / tab.node_canvas_zoom) : 1.0f;
        tab.node_canvas_pan.x += io.MouseDelta.x * inv_zoom;
        tab.node_canvas_pan.y += io.MouseDelta.y * inv_zoom;
    }

    if (allow_input && io.MouseWheel != 0.0f) {
        float wheel = std::clamp(io.MouseWheel, -3.0f, 3.0f);
        apply_zoom(1.0f + wheel * sizes.editor_node_zoom_step, io.MousePos);
    }

    auto screen_to_canvas = [&](ImVec2 screen_pos) {
        return ImVec2((screen_pos.x - content_min.x) / tab.node_canvas_zoom - tab.node_canvas_pan.x,
                      (screen_pos.y - content_min.y) / tab.node_canvas_zoom - tab.node_canvas_pan.y);
    };
    auto canvas_to_screen = [&](ImVec2 canvas_pos) {
        return ImVec2(content_min.x + (canvas_pos.x + tab.node_canvas_pan.x) * tab.node_canvas_zoom,
                      content_min.y + (canvas_pos.y + tab.node_canvas_pan.y) * tab.node_canvas_zoom);
    };

    auto find_node_by_id = [&](int id) -> EditorTab::Node* {
        for (auto& node : tab.nodes) {
            if (node.id == id) {
                return &node;
            }
        }
        return nullptr;
    };

    float base_font_size = ImGui::GetFontSize();
    float font_size = base_font_size * tab.node_canvas_zoom;
    float title_h = font_size * 1.8f;
    float port_h = font_size * 1.25f;
    float vert_top = font_size * 0.5f;
    float vert_bottom = font_size * 0.5f;
    float horiz_pad = font_size * 1.7f;
    float default_width = 160.0f;
    float port_radius = 0.6f * font_size * 0.5f;

    auto port_center_screen = [&](const EditorTab::Node& node, bool is_output, int port_index, float local_port_radius) -> ImVec2 {
        ImVec2 node_pos = canvas_to_screen(node.pos);
        ImVec2 node_size = ImVec2(node.size.x * tab.node_canvas_zoom, node.size.y * tab.node_canvas_zoom);
        float x = is_output ? (node_pos.x + node_size.x - local_port_radius) : (node_pos.x + local_port_radius);
        float y = node_pos.y + title_h + vert_top + port_h * 0.5f + port_index * port_h;
        return ImVec2(x, y);
    };

    auto hit_test_port = [&](ImVec2 mouse, float local_port_radius) -> std::tuple<int, int, bool> {
        for (auto it = tab.nodes.rbegin(); it != tab.nodes.rend(); ++it) {
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
        for (auto it = tab.nodes.rbegin(); it != tab.nodes.rend(); ++it) {
            const auto& node = *it;
            ImVec2 node_pos = canvas_to_screen(node.pos);
            ImVec2 node_size = ImVec2(node.size.x * tab.node_canvas_zoom, node.size.y * tab.node_canvas_zoom);
            ImVec2 node_max = ImVec2(node_pos.x + node_size.x, node_pos.y + node_size.y);
            if (mouse.x >= node_pos.x && mouse.x <= node_max.x && mouse.y >= node_pos.y && mouse.y <= node_max.y) {
                return node.id;
            }
        }
        return 0;
    };

    auto update_node_layout = [&]() {
        for (auto& node : tab.nodes) {
            int max_ports = static_cast<int>(std::max(node.inputs.size(), node.outputs.size()));
            float body_h = std::max(1, max_ports) * port_h + vert_top + vert_bottom;
            float total_h = title_h + body_h;
            if (node.size.y * tab.node_canvas_zoom < total_h) {
                node.size.y = total_h / tab.node_canvas_zoom;
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
        for (const auto& link : tab.links) {
            if (link.to_node == node_id && link.to_port == local_port_index) {
                return true;
            }
        }
        return false;
    };

    auto is_output_connected = [&](int node_id, int local_port_index) {
        for (const auto& link : tab.links) {
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
        for (int i = static_cast<int>(tab.links.size()) - 1; i >= 0; --i) {
            const auto& link = tab.links[i];
            const EditorTab::Node* from_node = nullptr;
            const EditorTab::Node* to_node = nullptr;
            for (const auto& node : tab.nodes) {
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
            float local_font_size = ImGui::GetFontSize() * tab.node_canvas_zoom;
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

    if (tab.request_add_node_from_library && !tab.pending_node_type.empty()) {
        request_add_node = true;
        request_node_pos = screen_to_canvas(ImVec2((content_min.x + content_max.x) * 0.5f,
                                                   (content_min.y + content_max.y) * 0.5f));
    }

    if (request_add_node) {
        auto configure_ports = [](EditorTab::Node& node, const std::string& type) {
            node.inputs.clear();
            node.outputs.clear();
            if (type == "Gain") {
                node.inputs.push_back({ "In", EditorTab::NodePortType::Double });
                node.outputs.push_back({ "Out", EditorTab::NodePortType::Double });
            } else if (type == "Test") {
                node.inputs.push_back({ "In1", EditorTab::NodePortType::Float });
                node.inputs.push_back({ "In2", EditorTab::NodePortType::Double });
                node.inputs.push_back({ "In3", EditorTab::NodePortType::Int });
                node.inputs.push_back({ "In4", EditorTab::NodePortType::Double });
                node.outputs.push_back({ "Out1", EditorTab::NodePortType::Int });
                node.outputs.push_back({ "Out2", EditorTab::NodePortType::Double });
                node.outputs.push_back({ "Out3", EditorTab::NodePortType::Generic });
            } else if (type == "If") {
                node.inputs.push_back({ "Cond", EditorTab::NodePortType::Bool });
                node.inputs.push_back({ "Value", EditorTab::NodePortType::Generic });
                node.outputs.push_back({ "Out", EditorTab::NodePortType::Generic });
            } else if (type == "Switch") {
                node.inputs.push_back({ "A", EditorTab::NodePortType::Generic });
                node.inputs.push_back({ "B", EditorTab::NodePortType::Generic });
                node.inputs.push_back({ "Sel", EditorTab::NodePortType::Bool });
                node.outputs.push_back({ "Out", EditorTab::NodePortType::Generic });
            } else {
                node.inputs.push_back({ "In", EditorTab::NodePortType::Generic });
                node.outputs.push_back({ "Out", EditorTab::NodePortType::Generic });
            }
        };

        EditorTab::Node node;
        node.id = tab.node_next_id++;
        if (!tab.pending_node_type.empty()) {
            node.title = tab.pending_node_type + " " + std::to_string(node.id);
        } else {
            node.title = "Node " + std::to_string(node.id);
        }
        node.type = tab.pending_node_type.empty() ? "Generic" : tab.pending_node_type;
        node.pos = request_node_pos;
        configure_ports(node, tab.pending_node_type);
        tab.nodes.push_back(node);
        tab.request_add_node_from_library = false;
        tab.pending_node_type.clear();
    }

    if (allow_input && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !space_down) {
        float local_port_radius = 0.6f * font_size * 0.5f;
        auto [port_node_id, port_index, is_output] = hit_test_port(mouse_pos, local_port_radius);
        if (port_node_id != 0 && is_output) {
            tab.linking = true;
            tab.link_from_node_id = port_node_id;
            tab.link_from_port = port_index;
        } else {
            int node_id = hit_test_node(mouse_pos);
            if (node_id != 0) {
                tab.selected_node_id = node_id;
                tab.dragging_node_id = node_id;
                if (auto node = find_node_by_id(node_id)) {
                    ImVec2 node_pos = canvas_to_screen(node->pos);
                    tab.dragging_node_offset = ImVec2(mouse_pos.x - node_pos.x, mouse_pos.y - node_pos.y);
                } else {
                    tab.dragging_node_offset = ImVec2(0.0f, 0.0f);
                }
            } else {
                tab.selected_node_id = 0;
            }
        }
    }

    if (allow_input && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        float local_port_radius = 0.6f * font_size * 0.5f;
        auto [port_node_id, port_index, is_output] = hit_test_port(mouse_pos, local_port_radius);
        if (port_node_id != 0) {
            if (is_output) {
                tab.links.erase(std::remove_if(tab.links.begin(), tab.links.end(), [&](const EditorTab::Link& link) {
                    return link.from_node == port_node_id && link.from_port == port_index;
                }), tab.links.end());
            } else {
                tab.links.erase(std::remove_if(tab.links.begin(), tab.links.end(), [&](const EditorTab::Link& link) {
                    return link.to_node == port_node_id && link.to_port == port_index;
                }), tab.links.end());
            }
        } else {
            int link_index = hit_test_link(mouse_pos);
            if (link_index >= 0) {
                tab.links.erase(tab.links.begin() + link_index);
            }
        }
    }

    if (tab.dragging_node_id != 0 && ImGui::IsMouseDown(ImGuiMouseButton_Left) && !space_down) {
        if (auto node = find_node_by_id(tab.dragging_node_id)) {
            ImVec2 new_screen = ImVec2(mouse_pos.x - tab.dragging_node_offset.x, mouse_pos.y - tab.dragging_node_offset.y);
            node->pos = screen_to_canvas(new_screen);
        }
    }
    if (tab.dragging_node_id != 0 && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        tab.dragging_node_id = 0;
    }

    if (tab.linking && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        float local_port_radius = 0.6f * font_size * 0.5f;
        auto [port_node_id, port_index, is_output] = hit_test_port(mouse_pos, local_port_radius);
        if (port_node_id != 0 && !is_output && tab.link_from_node_id != 0 && tab.link_from_port >= 0) {
            tab.links.erase(std::remove_if(tab.links.begin(), tab.links.end(), [&](const EditorTab::Link& link) {
                return link.to_node == port_node_id && link.to_port == port_index;
            }), tab.links.end());

            EditorTab::Link link;
            link.from_node = tab.link_from_node_id;
            link.from_port = tab.link_from_port;
            link.to_node = port_node_id;
            link.to_port = port_index;
            tab.links.push_back(link);
        }
        tab.linking = false;
        tab.link_from_node_id = 0;
        tab.link_from_port = -1;
    }

    if (allow_input && ImGui::IsKeyPressed(ImGuiKey_Delete) && tab.selected_node_id != 0) {
        int node_id = tab.selected_node_id;
        tab.links.erase(std::remove_if(tab.links.begin(), tab.links.end(), [&](const EditorTab::Link& link) {
            return link.from_node == node_id || link.to_node == node_id;
        }), tab.links.end());
        tab.nodes.erase(std::remove_if(tab.nodes.begin(), tab.nodes.end(), [&](const EditorTab::Node& node) {
            return node.id == node_id;
        }), tab.nodes.end());
        tab.selected_node_id = 0;
        tab.dragging_node_id = 0;
        tab.linking = false;
        tab.link_from_node_id = 0;
        tab.link_from_port = -1;
    }

    if (!std::isfinite(tab.node_canvas_pan.x) || !std::isfinite(tab.node_canvas_pan.y) ||
        !std::isfinite(tab.node_canvas_zoom)) {
        tab.node_canvas_pan = ImVec2(0.0f, 0.0f);
        tab.node_canvas_zoom = 1.0f;
    }

    ImVec2 grid_pan = ImVec2(tab.node_canvas_pan.x * tab.node_canvas_zoom,
                             tab.node_canvas_pan.y * tab.node_canvas_zoom);
    const float pan_wrap = sizes.editor_node_grid_size * 1024.0f;
    if (pan_wrap > 0.0f) {
        grid_pan.x = std::fmod(grid_pan.x, pan_wrap);
        grid_pan.y = std::fmod(grid_pan.y, pan_wrap);
    }

    draw_list->PushClipRect(content_min, content_max, true);
    const float base_grid = sizes.editor_node_grid_size;
    const float grid_size = base_grid * tab.node_canvas_zoom;
    const float grid_draw_min = sizes.editor_node_grid_draw_min;
    const float grid_draw_max = sizes.editor_node_grid_draw_max;
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

    ImU32 minor_col = ImGui::GetColorU32(colors.editor_node_grid_minor);
    ImU32 major_col = ImGui::GetColorU32(colors.editor_node_grid_major);

    bool draw_minor = draw_grid_size >= (grid_draw_min * sizes.editor_node_grid_minor_threshold);
    if (draw_minor) {
        float start_x = grid_start(content_min.x, grid_pan.x, draw_grid_size);
        for (float x = start_x; x <= content_max.x; x += draw_grid_size) {
            draw_list->AddLine(ImVec2(x, content_min.y), ImVec2(x, content_max.y), minor_col, sizes.editor_node_grid_thickness);
        }
        float start_y = grid_start(content_min.y, grid_pan.y, draw_grid_size);
        for (float y = start_y; y <= content_max.y; y += draw_grid_size) {
            draw_list->AddLine(ImVec2(content_min.x, y), ImVec2(content_max.x, y), minor_col, sizes.editor_node_grid_thickness);
        }
    }

    {
        float start_x = grid_start(content_min.x, grid_pan.x, draw_major_grid_size);
        for (float x = start_x; x <= content_max.x; x += draw_major_grid_size) {
            draw_list->AddLine(ImVec2(x, content_min.y), ImVec2(x, content_max.y), major_col, sizes.editor_node_grid_major_thickness);
        }
        float start_y = grid_start(content_min.y, grid_pan.y, draw_major_grid_size);
        for (float y = start_y; y <= content_max.y; y += draw_major_grid_size) {
            draw_list->AddLine(ImVec2(content_min.x, y), ImVec2(content_max.x, y), major_col, sizes.editor_node_grid_major_thickness);
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

    for (const auto& link : tab.links) {
        const EditorTab::Node* from_node = nullptr;
        const EditorTab::Node* to_node = nullptr;
        for (const auto& node : tab.nodes) {
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
        float link_font_size = ImGui::GetFontSize() * tab.node_canvas_zoom;
        float local_port_radius = 0.6f * link_font_size * 0.5f;
        ImVec2 p1 = port_center_screen(*from_node, true, link.from_port, local_port_radius);
        ImVec2 p4 = port_center_screen(*to_node, false, link.to_port, local_port_radius);
        float dx = std::abs(p4.x - p1.x) * 0.5f;
        ImVec2 p2 = ImVec2(p1.x + dx, p1.y);
        ImVec2 p3 = ImVec2(p4.x - dx, p4.y);
        draw_list->AddBezierCubic(p1, p2, p3, p4, link_col, 2.0f);
    }

    if (tab.linking && tab.link_from_node_id != 0 && tab.link_from_port >= 0) {
        if (auto from_node = find_node_by_id(tab.link_from_node_id)) {
            if (tab.link_from_port < static_cast<int>(from_node->outputs.size())) {
                float local_font_size = ImGui::GetFontSize() * tab.node_canvas_zoom;
                float local_port_radius = 0.6f * local_font_size * 0.5f;
                ImVec2 p1 = port_center_screen(*from_node, true, tab.link_from_port, local_port_radius);
                ImVec2 p4 = mouse_pos;
                float dx = std::abs(p4.x - p1.x) * 0.5f;
                ImVec2 p2 = ImVec2(p1.x + dx, p1.y);
                ImVec2 p3 = ImVec2(p4.x - dx, p4.y);
                draw_list->AddBezierCubic(p1, p2, p3, p4, link_col, 2.0f);
            }
        }
    }

    ImGui::SetWindowFontScale(tab.node_canvas_zoom);
    for (auto& node : tab.nodes) {
        float port_thickness = 0.1f * font_size;

        ImVec2 node_pos = canvas_to_screen(node.pos);
        ImVec2 node_size = ImVec2(node.size.x * tab.node_canvas_zoom, node.size.y * tab.node_canvas_zoom);
        ImVec2 node_max = ImVec2(node_pos.x + node_size.x, node_pos.y + node_size.y);
        float rounding = title_h * 0.3f;

        draw_list->AddRectFilled(node_pos, node_max, node_body, rounding);
        ImVec2 head_br = ImVec2(node_max.x, node_pos.y + title_h);
        draw_list->AddRectFilled(node_pos, head_br, node_head, rounding);
        draw_list->AddLine(ImVec2(node_pos.x, head_br.y), ImVec2(head_br.x - 1.0f, head_br.y), node_line, 2.0f);

        bool selected = (node.id == tab.selected_node_id);
        draw_list->AddRect(node_pos, node_max, selected ? node_outline_active : node_outline, rounding, 0, selected ? 2.0f : 1.5f);

        ImVec2 text_pos = ImVec2(node_pos.x + title_pad * tab.node_canvas_zoom, node_pos.y + title_pad * tab.node_canvas_zoom);
        draw_list->AddText(ImVec2(text_pos.x + 2.0f, text_pos.y + 2.0f), IM_COL32(0, 0, 0, 255), node.title.c_str());
        draw_list->AddText(text_pos, node_text, node.title.c_str());

        for (int i = 0; i < static_cast<int>(node.inputs.size()); ++i) {
            ImVec2 c = port_center_screen(node, false, i, port_radius);
            bool connected = is_input_connected(node.id, i);
            bool hovered = (hover_port_node == node.id && hover_port_index == i && !hover_is_output);
            bool should_connect = tab.linking && hovered;
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
            bool should_connect = tab.linking && hovered;
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
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, sizes.editor_node_overlay_rounding);
    ImGui::PushStyleColor(ImGuiCol_Button, colors.editor_node_overlay_bg);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors.editor_node_overlay_hover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors.editor_node_overlay_active);
    ImGui::PushStyleColor(ImGuiCol_Text, colors.editor_node_overlay_text);

    ImDrawList* overlay_draw = ImGui::GetWindowDrawList();
    ImVec4 overlay_bg = colors.editor_node_overlay_bg;
    overlay_bg.w = std::min(1.0f, overlay_bg.w + 0.1f);

    auto draw_icon_plus = [&](ImVec2 p0, ImVec2 p1) {
        ImVec2 c = ImVec2((p0.x + p1.x) * 0.5f, (p0.y + p1.y) * 0.5f);
        float r = (p1.x - p0.x) * 0.25f;
        ImU32 col = ImGui::GetColorU32(colors.editor_node_overlay_text);
        overlay_draw->AddLine(ImVec2(c.x - r, c.y), ImVec2(c.x + r, c.y), col, 2.0f);
        overlay_draw->AddLine(ImVec2(c.x, c.y - r), ImVec2(c.x, c.y + r), col, 2.0f);
    };
    auto draw_icon_minus = [&](ImVec2 p0, ImVec2 p1) {
        ImVec2 c = ImVec2((p0.x + p1.x) * 0.5f, (p0.y + p1.y) * 0.5f);
        float r = (p1.x - p0.x) * 0.25f;
        ImU32 col = ImGui::GetColorU32(colors.editor_node_overlay_text);
        overlay_draw->AddLine(ImVec2(c.x - r, c.y), ImVec2(c.x + r, c.y), col, 2.0f);
    };
    auto draw_icon_reset = [&](ImVec2 p0, ImVec2 p1) {
        ImVec2 c = ImVec2((p0.x + p1.x) * 0.5f, (p0.y + p1.y) * 0.5f);
        float r = (p1.x - p0.x) * 0.25f;
        ImU32 col = ImGui::GetColorU32(colors.editor_node_overlay_text);
        overlay_draw->AddCircle(ImVec2(c.x, c.y), r, col, 24, 2.0f);
        overlay_draw->AddLine(ImVec2(c.x, c.y - r * 0.9f), ImVec2(c.x + r * 0.6f, c.y - r * 0.6f), col, 2.0f);
    };

    auto icon_button = [&](const char* id, ImVec2 pos, auto draw_icon) -> bool {
        ImGui::SetCursorScreenPos(pos);
        bool pressed = ImGui::Button(id, button_size);
        ImVec2 p0 = ImGui::GetItemRectMin();
        ImVec2 p1 = ImGui::GetItemRectMax();
        ImU32 bg_col = ImGui::GetColorU32(ImGui::IsItemActive() ? colors.editor_node_overlay_active
            : (ImGui::IsItemHovered() ? colors.editor_node_overlay_hover : overlay_bg));
        overlay_draw->AddRectFilled(p0, p1, bg_col, sizes.editor_node_overlay_rounding);
        draw_icon(p0, p1);
        return pressed;
    };

    ImVec2 zoom_pivot = ImVec2((content_min.x + content_max.x) * 0.5f, (content_min.y + content_max.y) * 0.5f);

    ImVec2 button_pos = ImVec2(stack_pos.x + overlay_padding, stack_pos.y + overlay_padding);
    if (icon_button("##zoom_in", button_pos, draw_icon_plus)) {
        apply_zoom(1.0f + sizes.editor_node_zoom_step, zoom_pivot);
    }
    button_pos.y += button_size.y + spacing;
    if (icon_button("##zoom_out", button_pos, draw_icon_minus)) {
        apply_zoom(1.0f - sizes.editor_node_zoom_step, zoom_pivot);
    }
    button_pos.y += button_size.y + spacing;
    if (icon_button("##zoom_reset", button_pos, draw_icon_reset)) {
        tab.node_canvas_pan = ImVec2(0.0f, 0.0f);
        tab.node_canvas_zoom = 1.0f;
    }
    button_pos.y += button_size.y + spacing;
    ImVec2 percent_pos = ImVec2(button_pos.x, button_pos.y);
    ImVec2 percent_max = ImVec2(button_pos.x + button_size.x, button_pos.y + button_size.y);
    overlay_draw->AddRectFilled(percent_pos, percent_max, ImGui::GetColorU32(overlay_bg), sizes.editor_node_overlay_rounding);
    const float zoom_percent = tab.node_canvas_zoom * 100.0f;
    std::string percent_text = std::to_string((int)zoom_percent);
    ImVec2 percent_text_size = ImGui::CalcTextSize(percent_text.c_str());
    ImVec2 percent_text_pos = ImVec2(
        percent_pos.x + (button_size.x - percent_text_size.x) * 0.5f,
        percent_pos.y + (button_size.y - percent_text_size.y) * 0.5f
    );
    overlay_draw->AddText(percent_text_pos, ImGui::GetColorU32(colors.editor_node_overlay_text), percent_text.c_str());

    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar();
}

void RenderLibrary(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawHeader("NODE LIBRARY", colors.primary_sidebar_text_dim);
    const bool can_add = active_tab && active_tab->scene_type == SceneType::NodeEditor;

    auto draw_item = [&](const char* label) {
        if (ImGui::Selectable(label, false) && can_add) {
            active_tab->pending_node_type = label;
            active_tab->request_add_node_from_library = true;
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

    if (!active_tab || active_tab->scene_type != SceneType::NodeEditor) {
        return;
    }

    EditorTab::Node* selected = nullptr;
    if (active_tab->selected_node_id != 0) {
        for (auto& node : active_tab->nodes) {
            if (node.id == active_tab->selected_node_id) {
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
    if (active_tab && active_tab->scene_type == SceneType::NodeEditor) {
        if (ImGui::Button("Run")) {
            auto result = NodeGraph::ExecuteGraph(*active_tab, false);
            active_tab->node_exec_last_ok = result.success;
            active_tab->node_exec_last_parallel = result.parallel;
            active_tab->node_exec_last_ms = result.duration_ms;
            active_tab->node_exec_last_error = result.error;
            active_tab->node_exec_log = std::move(result.log);
            active_tab->node_exec_outputs = std::move(result.outputs);
            if (active_tab->node_exec_log.empty()) {
                active_tab->node_exec_log.push_back(result.success ? "Execution finished." : "Execution failed.");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Run Parallel")) {
            auto result = NodeGraph::ExecuteGraph(*active_tab, true);
            active_tab->node_exec_last_ok = result.success;
            active_tab->node_exec_last_parallel = result.parallel;
            active_tab->node_exec_last_ms = result.duration_ms;
            active_tab->node_exec_last_error = result.error;
            active_tab->node_exec_log = std::move(result.log);
            active_tab->node_exec_outputs = std::move(result.outputs);
            if (active_tab->node_exec_log.empty()) {
                active_tab->node_exec_log.push_back(result.success ? "Execution finished." : "Execution failed.");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Log")) {
            active_tab->node_exec_log.clear();
            active_tab->node_exec_outputs.clear();
            active_tab->node_exec_last_error.clear();
        }

        ImGui::Separator();
        ImGui::Text("Last run: %s", active_tab->node_exec_last_ok ? "OK" : "Error");
        ImGui::Text("Mode: %s", active_tab->node_exec_last_parallel ? "Parallel" : "Single");
        ImGui::Text("Duration: %.2f ms", active_tab->node_exec_last_ms);
        if (!active_tab->node_exec_last_ok && !active_tab->node_exec_last_error.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "%s", active_tab->node_exec_last_error.c_str());
        }

        if (!active_tab->node_exec_outputs.empty()) {
            ImGui::Spacing();
            ImGui::Text("Outputs:");
            for (const auto& entry : active_tab->node_exec_outputs) {
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
        if (active_tab->node_exec_log.empty()) {
            ImGui::TextColored(colors.panel_text, "No logs yet.");
        } else {
            for (const auto& line : active_tab->node_exec_log) {
                ImGui::TextWrapped("%s", line.c_str());
            }
        }
        ImGui::EndChild();
        return;
    }

    ImGui::TextColored(colors.panel_text, "Build completed successfully");
}

} // namespace Scenes::NodeEditorViews
