#include "editor_scene_renderers.h"
#include "../workbench/workbench_config.h"
#include <SDL3/SDL.h>
#include <cmath>
#include <algorithm>

namespace UI {

namespace {

void DrawScene2DAdapter(ImVec2 content_min, ImVec2 content_max, EditorTab& tab)
{
    DrawScene2D(content_min, content_max, tab);
}

void DrawScene3DAdapter(ImVec2 content_min, ImVec2 content_max, EditorTab& tab)
{
    DrawScene3D(content_min, content_max, tab);
}

} // namespace

void RegisterDefaultEditorSceneRenderers(EditorSceneRegistry& registry)
{
    registry.RegisterRenderer(SceneType::Scene2D, DrawScene2DAdapter);
    registry.RegisterRenderer(SceneType::Scene3D, DrawScene3DAdapter);
    registry.RegisterRenderer(SceneType::NodeEditor, DrawNodeEditorCanvas);
}

void DrawScene2D(ImVec2 content_min, ImVec2 content_max, const EditorTab& tab)
{
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

void DrawScene3D(ImVec2 content_min, ImVec2 content_max, const EditorTab& tab)
{
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

    ImVec2 vanishing_point = ImVec2(center.x, horizon_y);
    (void)vanishing_point;

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

void DrawNodeEditorCanvas(ImVec2 content_min, ImVec2 content_max, EditorTab& tab)
{
    const WorkbenchTheme& theme = GetWorkbenchTheme();
    const WorkbenchThemeColors& colors = theme.colors;
    const WorkbenchThemeSizes& sizes = theme.sizes;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 canvas_size = ImVec2(content_max.x - content_min.x, content_max.y - content_min.y);
    draw_list->AddRectFilled(content_min, content_max, ImGui::GetColorU32(colors.editor_node_bg));

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 mouse_pos = io.MousePos;
    bool in_canvas = (mouse_pos.x >= content_min.x && mouse_pos.x <= content_max.x &&
                      mouse_pos.y >= content_min.y && mouse_pos.y <= content_max.y);

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

    float local_x = 0.0f;
    float local_y = 0.0f;
    SDL_MouseButtonFlags buttons = SDL_GetMouseState(&local_x, &local_y);
    const bool* keys = SDL_GetKeyboardState(nullptr);
    bool space_down = keys[SDL_SCANCODE_SPACE] != 0 || ImGui::IsKeyDown(ImGuiKey_Space);
    bool left_down = (buttons & SDL_BUTTON_LMASK) != 0;
    bool middle_down = (buttons & SDL_BUTTON_MMASK) != 0;
    bool right_down = (buttons & SDL_BUTTON_RMASK) != 0;

    static int drag_mode = 0; // 0: none, 1: left+space, 2: middle, 3: right
    static ImVec2 last_mouse_pos = ImVec2(0.0f, 0.0f);

    bool started_drag = false;
    if (drag_mode == 0) {
        if (in_canvas && !over_overlay && middle_down) {
            drag_mode = 2;
            started_drag = true;
        } else if (in_canvas && !over_overlay && right_down) {
            drag_mode = 3;
            started_drag = true;
        } else if (in_canvas && !over_overlay && left_down && space_down) {
            drag_mode = 1;
            started_drag = true;
        }
    }

    if (drag_mode == 1 && !left_down) drag_mode = 0;
    if (drag_mode == 2 && !middle_down) drag_mode = 0;
    if (drag_mode == 3 && !right_down) drag_mode = 0;

    float global_x = 0.0f;
    float global_y = 0.0f;
    SDL_GetGlobalMouseState(&global_x, &global_y);

    if (drag_mode != 0) {
        ImVec2 drag_pos = ImVec2(global_x, global_y);
        if (started_drag) {
            last_mouse_pos = drag_pos;
        }
        ImVec2 delta = ImVec2(drag_pos.x - last_mouse_pos.x, drag_pos.y - last_mouse_pos.y);
        tab.node_canvas_pan.x += delta.x;
        tab.node_canvas_pan.y += delta.y;
        last_mouse_pos = drag_pos;
    }

    if (in_canvas && io.MouseWheel != 0.0f) {
        float wheel = std::clamp(io.MouseWheel, -3.0f, 3.0f);
        float old_zoom = tab.node_canvas_zoom;
        float zoom = old_zoom * (1.0f + wheel * sizes.editor_node_zoom_step);
        zoom = std::clamp(zoom, sizes.editor_node_zoom_min, sizes.editor_node_zoom_max);

        if (zoom != old_zoom) {
            ImVec2 mouse = io.MousePos;
            ImVec2 before = ImVec2((mouse.x - content_min.x - tab.node_canvas_pan.x) / old_zoom,
                                   (mouse.y - content_min.y - tab.node_canvas_pan.y) / old_zoom);
            tab.node_canvas_zoom = zoom;
            ImVec2 after = ImVec2(before.x * zoom, before.y * zoom);
            tab.node_canvas_pan.x += (mouse.x - content_min.x) - after.x;
            tab.node_canvas_pan.y += (mouse.y - content_min.y) - after.y;
        }
    }

    if (!std::isfinite(tab.node_canvas_pan.x) || !std::isfinite(tab.node_canvas_pan.y) ||
        !std::isfinite(tab.node_canvas_zoom)) {
        tab.node_canvas_pan = ImVec2(0.0f, 0.0f);
        tab.node_canvas_zoom = 1.0f;
    }

    const float pan_wrap = sizes.editor_node_grid_size * 1024.0f;
    if (pan_wrap > 0.0f) {
        tab.node_canvas_pan.x = std::fmod(tab.node_canvas_pan.x, pan_wrap);
        tab.node_canvas_pan.y = std::fmod(tab.node_canvas_pan.y, pan_wrap);
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

    {
        float start_x = grid_start(content_min.x, tab.node_canvas_pan.x, draw_grid_size);
        for (float x = start_x; x <= content_max.x; x += draw_grid_size) {
            draw_list->AddLine(ImVec2(x, content_min.y), ImVec2(x, content_max.y), minor_col, sizes.editor_node_grid_thickness);
        }
        float start_y = grid_start(content_min.y, tab.node_canvas_pan.y, draw_grid_size);
        for (float y = start_y; y <= content_max.y; y += draw_grid_size) {
            draw_list->AddLine(ImVec2(content_min.x, y), ImVec2(content_max.x, y), minor_col, sizes.editor_node_grid_thickness);
        }
    }

    {
        float start_x = grid_start(content_min.x, tab.node_canvas_pan.x, draw_major_grid_size);
        for (float x = start_x; x <= content_max.x; x += draw_major_grid_size) {
            draw_list->AddLine(ImVec2(x, content_min.y), ImVec2(x, content_max.y), major_col, sizes.editor_node_grid_major_thickness);
        }
        float start_y = grid_start(content_min.y, tab.node_canvas_pan.y, draw_major_grid_size);
        for (float y = start_y; y <= content_max.y; y += draw_major_grid_size) {
            draw_list->AddLine(ImVec2(content_min.x, y), ImVec2(content_max.x, y), major_col, sizes.editor_node_grid_major_thickness);
        }
    }

    draw_list->PopClipRect();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, sizes.editor_node_overlay_rounding);
    ImGui::PushStyleColor(ImGuiCol_Button, colors.editor_node_overlay_bg);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors.editor_node_overlay_bg);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors.editor_node_overlay_bg);
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
        overlay_draw->AddRectFilled(p0, p1, ImGui::GetColorU32(overlay_bg), sizes.editor_node_overlay_rounding);
        draw_icon(p0, p1);
        return pressed;
    };

    auto apply_zoom = [&](float zoom_scale, ImVec2 pivot) {
        float old_zoom = tab.node_canvas_zoom;
        float new_zoom = std::clamp(old_zoom * zoom_scale, sizes.editor_node_zoom_min, sizes.editor_node_zoom_max);
        if (new_zoom == old_zoom) {
            return;
        }
        ImVec2 before = ImVec2((pivot.x - content_min.x - tab.node_canvas_pan.x) / old_zoom,
                               (pivot.y - content_min.y - tab.node_canvas_pan.y) / old_zoom);
        tab.node_canvas_zoom = new_zoom;
        ImVec2 after = ImVec2(before.x * new_zoom, before.y * new_zoom);
        tab.node_canvas_pan.x += (pivot.x - content_min.x) - after.x;
        tab.node_canvas_pan.y += (pivot.y - content_min.y) - after.y;
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
    ImVec2 percent_text_size = ImGui::CalcTextSize("100");
    ImVec2 percent_text_pos = ImVec2(
        percent_pos.x + (button_size.x - percent_text_size.x) * 0.5f,
        percent_pos.y + (button_size.y - percent_text_size.y) * 0.5f
    );
    std::string percent_text = std::to_string((int)zoom_percent);
    overlay_draw->AddText(percent_text_pos, ImGui::GetColorU32(colors.editor_node_overlay_text), percent_text.c_str());

    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar();
}

} // namespace UI
