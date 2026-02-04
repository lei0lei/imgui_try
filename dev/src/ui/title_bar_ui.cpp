
#include "title_bar_ui.h"
#include "../services/title_bar_service.h"
#include "../workbench/workbench_config.h"
#include "imgui.h"
#include <SDL3/SDL.h>
#include <math.h>

// DrawPhageIcon 在 title_bar.cpp 中实现，这里声明以便链接
void DrawPhageIcon(ImDrawList* draw_list, ImVec2 center, float size, ImU32 color_light, ImU32 color_dark, ImU32 color_edge)
{
    const float radius = size * 0.5f;

    // Rotation animation
    float time = (float)ImGui::GetTime();
    float rot_y = time * 1.2f;
    float rot_x = time * 0.8f;

    // Pulsing effect
    float pulse = 1.0f + sinf(time * 2.0f) * 0.05f;
    float r = radius * pulse;

    // 6 vertices of an octahedron
    float vertices[6][3] = {
        {0, 1, 0},   // top
        {0, -1, 0},  // bottom
        {1, 0, 0},   // right
        {-1, 0, 0},  // left
        {0, 0, 1},   // front
        {0, 0, -1}   // back
    };

    // 8 triangular faces (each side of the two pyramids)
    int faces[8][3] = {
        {0, 2, 4}, {0, 4, 3}, {0, 3, 5}, {0, 5, 2}, // upper
        {1, 4, 2}, {1, 3, 4}, {1, 5, 3}, {1, 2, 5}  // lower
    };

    // Rotate and project vertices
    ImVec2 projected[6];
    float z_depth[6];
    for (int i = 0; i < 6; i++) {
        float x = vertices[i][0];
        float y = vertices[i][1];
        float z = vertices[i][2];

        // Rotate around Y axis
        float x1 = x * cosf(rot_y) - z * sinf(rot_y);
        float z1 = x * sinf(rot_y) + z * cosf(rot_y);

        // Rotate around X axis
        float y2 = y * cosf(rot_x) - z1 * sinf(rot_x);
        float z2 = y * sinf(rot_x) + z1 * cosf(rot_x);

        // Perspective projection
        float scale = r / (2.5f - z2 * 0.5f);
        projected[i] = ImVec2(center.x + x1 * scale, center.y - y2 * scale);
        z_depth[i] = z2;
    }

    // Calculate face depths and sort (back to front)
    struct FaceDepth { int idx; float depth; };
    FaceDepth face_depths[8];
    for (int i = 0; i < 8; i++) {
        float avg_z = (z_depth[faces[i][0]] + z_depth[faces[i][1]] + z_depth[faces[i][2]]) / 3.0f;
        face_depths[i] = {i, avg_z};
    }
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7 - i; j++) {
            if (face_depths[j].depth > face_depths[j + 1].depth) {
                FaceDepth temp = face_depths[j];
                face_depths[j] = face_depths[j + 1];
                face_depths[j + 1] = temp;
            }
        }
    }

    // Draw faces with gradient (back to front)
    for (int f = 0; f < 8; f++) {
        int face_idx = face_depths[f].idx;
        ImVec2 p0 = projected[faces[face_idx][0]];
        ImVec2 p1 = projected[faces[face_idx][1]];
        ImVec2 p2 = projected[faces[face_idx][2]];

        float avg_z = face_depths[f].depth;
        float brightness = 0.55f + avg_z * 0.35f + 0.1f * sinf(time * 3.0f + face_idx);

        float hue = fmodf(time * 0.2f + face_idx * 0.08f, 1.0f);
        ImU32 face_color;
        if (hue < 0.33f) {
            face_color = IM_COL32((int)(120 * brightness), (int)(170 * brightness), (int)(255 * brightness), 200);
        } else if (hue < 0.66f) {
            face_color = IM_COL32((int)(90 * brightness), (int)(200 * brightness), (int)(255 * brightness), 200);
        } else {
            face_color = IM_COL32((int)(140 * brightness), (int)(130 * brightness), (int)(255 * brightness), 200);
        }

        draw_list->AddTriangleFilled(p0, p1, p2, face_color);
    }

    // Draw edges (thin with subtle glow)
    ImU32 edge_glow = IM_COL32(150, 200, 255, 50);
    ImU32 edge_main = IM_COL32(220, 240, 255, 255);
    bool drawn_edges[6][6] = {};
    for (int f = 0; f < 8; f++) {
        for (int e = 0; e < 3; e++) {
            int v0 = faces[f][e];
            int v1 = faces[f][(e + 1) % 3];
            if (v0 > v1) { int temp = v0; v0 = v1; v1 = temp; }

            if (!drawn_edges[v0][v1]) {
                draw_list->AddLine(projected[v0], projected[v1], edge_glow, 1.6f);
                draw_list->AddLine(projected[v0], projected[v1], edge_main, 1.0f);
                drawn_edges[v0][v1] = true;
            }
        }
    }
}

void RenderTitleBarUI(SDL_Window* window, TitleBarService& service, float title_h, 
                      bool primary_sidebar_visible, bool panel_visible, bool secondary_sidebar_visible)
{
    ImGuiIO& io = ImGui::GetIO();
    const WorkbenchTheme& theme = GetWorkbenchTheme();
    const WorkbenchThemeColors& colors = theme.colors;
    const WorkbenchThemeSizes& sizes = theme.sizes;

    ImVec4 bg_color = colors.title_bar_bg;
    ImVec4 menu_hover = colors.title_bar_menu_hover;
    ImVec4 text_color = colors.title_bar_text;

    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    bg->AddRectFilled(ImVec2(0, 0), ImVec2(io.DisplaySize.x, title_h), ImGui::GetColorU32(bg_color));

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, title_h));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                              ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
                              ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav;
    ImGui::Begin("TitleBar", nullptr, flags);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImDrawList* fg_list = ImGui::GetForegroundDrawList();
    ImVec2 window_pos = ImGui::GetWindowPos();

    // === Left side: Icon ===
    float start_x = sizes.title_icon_start_x;
    float icon_draw_size = title_h * sizes.title_icon_draw_ratio;
    float icon_layout_w = title_h * sizes.title_icon_layout_w_ratio;
    float icon_offset_x = start_x;
    ImVec2 icon_center = ImVec2(window_pos.x + icon_offset_x + icon_layout_w * 0.5f, window_pos.y + title_h * 0.5f);
    ImU32 color_light = ImGui::GetColorU32(colors.title_bar_icon_light);
    ImU32 color_dark = ImGui::GetColorU32(colors.title_bar_icon_dark);
    ImU32 color_edge = ImGui::GetColorU32(colors.title_bar_icon_edge);
    // extern void DrawPhageIcon(ImDrawList*, ImVec2, float, ImU32, ImU32, ImU32);
    DrawPhageIcon(draw_list, icon_center, icon_draw_size, color_light, color_dark, color_edge);

    // === Menu bar ===
    float menu_x = icon_offset_x + icon_layout_w + sizes.title_menu_spacing_x;
    float menu_btn_w = sizes.title_menu_btn_w;
    float dropdown_w = sizes.title_dropdown_w;
    const char* menu_names[] = { "File", "Edit", "View", "Help" };
    const int menu_count = 4;
    int active_menu = (int)service.GetActiveMenu() - 1;
        bool menu_click_handled = false;
    for (int i = 0; i < menu_count; i++) {
        ImVec2 btn_pos = ImVec2(window_pos.x + menu_x + i * menu_btn_w, window_pos.y);
        ImVec2 btn_size = ImVec2(menu_btn_w, title_h);
        ImVec2 btn_max = ImVec2(btn_pos.x + btn_size.x, btn_pos.y + btn_size.y);
        ImVec2 mouse_pos = ImGui::GetMousePos();
        bool is_hovered = (mouse_pos.x >= btn_pos.x && mouse_pos.x <= btn_max.x && mouse_pos.y >= btn_pos.y && mouse_pos.y <= btn_max.y);
            if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            active_menu = (active_menu == i) ? -1 : i;
            service.SetActiveMenu(active_menu >= 0 ? (TitleBarMenu)(active_menu + 1) : TitleBarMenu::None);
                menu_click_handled = true;
        }
        ImU32 btn_bg = (is_hovered || active_menu == i) ? ImGui::GetColorU32(menu_hover) : ImGui::GetColorU32(bg_color);
        draw_list->AddRectFilled(btn_pos, btn_max, btn_bg);
        ImVec2 text_pos = ImVec2(btn_pos.x + sizes.title_menu_text_padding_x, btn_pos.y + (title_h - ImGui::GetTextLineHeight()) * 0.5f);
        draw_list->AddText(text_pos, ImGui::GetColorU32(text_color), menu_names[i]);
        if (active_menu == i) {
            const char* items[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
            bool has_submenu[] = { false, false, false, false, false };
            if (i == 0) { items[0] = "New"; has_submenu[0] = true; items[1] = "Open"; items[2] = "Save"; items[3] = "Exit"; }
            else if (i == 1) { items[0] = "Undo"; items[1] = "Redo"; }
            else if (i == 2) { items[0] = "Explorer"; items[1] = "Console"; }
            else if (i == 3) { items[0] = "About"; }
            float menu_item_h = sizes.title_menu_item_h;
            int item_count = 0; for (int j = 0; j < 5; j++) if (items[j]) item_count++;
            ImVec2 dropdown_pos = ImVec2(btn_pos.x, btn_max.y);
            ImVec2 dropdown_size = ImVec2(dropdown_w, menu_item_h * item_count);
            ImVec2 dropdown_max = ImVec2(dropdown_pos.x + dropdown_size.x, dropdown_pos.y + dropdown_size.y);
            fg_list->AddRectFilled(dropdown_pos, dropdown_max, ImGui::GetColorU32(bg_color));
            fg_list->AddRect(dropdown_pos, dropdown_max, ImGui::GetColorU32(colors.title_bar_menu_border), 0.0f, 0, sizes.title_dropdown_border_thickness);
            static int hovered_submenu_item = -1; int current_hovered_item = -1;
            bool mouse_in_new_submenu = false;
            if (i == 0) {
                ImVec2 submenu_pos = ImVec2(dropdown_max.x, dropdown_pos.y);
                ImVec2 submenu_size = ImVec2(dropdown_w, menu_item_h * 2);
                ImVec2 submenu_max = ImVec2(submenu_pos.x + submenu_size.x, submenu_pos.y + submenu_size.y);
                mouse_in_new_submenu = (mouse_pos.x >= submenu_pos.x && mouse_pos.x <= submenu_max.x && mouse_pos.y >= submenu_pos.y && mouse_pos.y <= submenu_max.y);
            }
            for (int j = 0; j < item_count; j++) {
                if (!items[j]) continue;
                ImVec2 item_pos = ImVec2(dropdown_pos.x, dropdown_pos.y + j * menu_item_h);
                ImVec2 item_max = ImVec2(item_pos.x + dropdown_size.x, item_pos.y + menu_item_h);
                bool item_hovered = (mouse_pos.x >= item_pos.x && mouse_pos.x <= item_max.x && mouse_pos.y >= item_pos.y && mouse_pos.y <= item_max.y);
                if (i == 0 && j == 0 && mouse_in_new_submenu) item_hovered = true;
                if (item_hovered) current_hovered_item = j;
                ImU32 item_bg = item_hovered ? ImGui::GetColorU32(colors.title_bar_menu_item_hover) : ImGui::GetColorU32(bg_color);
                fg_list->AddRectFilled(item_pos, item_max, item_bg);
                float text_height = ImGui::GetTextLineHeight();
                float centered_y = item_pos.y + (menu_item_h - text_height) * 0.5f;
                fg_list->AddText(ImVec2(item_pos.x + sizes.title_menu_text_padding_x, centered_y), ImGui::GetColorU32(text_color), items[j]);
                if (has_submenu[j]) {
                    float arrow_x = item_max.x - sizes.title_menu_text_padding_x - sizes.title_menu_arrow_w;
                    float arrow_y = item_pos.y + menu_item_h * 0.5f;
                    fg_list->AddTriangleFilled(ImVec2(arrow_x - sizes.title_menu_arrow_w * 0.6f, arrow_y - sizes.title_menu_arrow_h * 0.5f),
                                              ImVec2(arrow_x - sizes.title_menu_arrow_w * 0.6f, arrow_y + sizes.title_menu_arrow_h * 0.5f),
                                              ImVec2(arrow_x + sizes.title_menu_arrow_w * 0.4f, arrow_y), ImGui::GetColorU32(text_color));
                }
                // 菜单点击事件
                if (item_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !has_submenu[j]) {
                    active_menu = -1;
                    service.SetActiveMenu(TitleBarMenu::None);
                    if (i == 0) {
                        if (j == 1) service.TriggerCommand(CommandId::FileOpen);
                        else if (j == 2) service.TriggerCommand(CommandId::FileSave);
                        else if (j == 3) service.TriggerCommand(CommandId::FileExit);
                    } else if (i == 1) {
                        if (j == 0) service.TriggerCommand(CommandId::EditUndo);
                        else if (j == 1) service.TriggerCommand(CommandId::EditRedo);
                    } else if (i == 2) {
                        if (j == 0) service.TriggerCommand(CommandId::ViewExplorer);
                        else if (j == 1) service.TriggerCommand(CommandId::ViewConsole);
                    } else if (i == 3) {
                        if (j == 0) service.TriggerCommand(CommandId::HelpAbout);
                    }
                        menu_click_handled = true;
                }
            }
            hovered_submenu_item = current_hovered_item;
            if (i == 0) {
                bool show_submenu = false;
                const char* submenu_items[] = { "2D Scene", "3D Scene", "Node Graph" };
                int submenu_count = 3;
                ImVec2 submenu_pos = ImVec2(dropdown_max.x, dropdown_pos.y);
                ImVec2 submenu_size = ImVec2(dropdown_w, menu_item_h * submenu_count);
                ImVec2 submenu_max = ImVec2(submenu_pos.x + submenu_size.x, submenu_pos.y + submenu_size.y);
                bool mouse_in_submenu = (mouse_pos.x >= submenu_pos.x && mouse_pos.x <= submenu_max.x && mouse_pos.y >= submenu_pos.y && mouse_pos.y <= submenu_max.y);
                if (hovered_submenu_item == 0 || mouse_in_submenu) {
                    show_submenu = true;
                    hovered_submenu_item = 0;
                }
                if (show_submenu) {
                    fg_list->AddRectFilled(submenu_pos, submenu_max, ImGui::GetColorU32(bg_color));
                    fg_list->AddRect(submenu_pos, submenu_max, ImGui::GetColorU32(colors.title_bar_menu_border), 0.0f, 0, sizes.title_dropdown_border_thickness);
                    for (int k = 0; k < submenu_count; k++) {
                        ImVec2 sub_item_pos = ImVec2(submenu_pos.x, submenu_pos.y + k * menu_item_h);
                        ImVec2 sub_item_max = ImVec2(sub_item_pos.x + submenu_size.x, sub_item_pos.y + menu_item_h);
                        bool sub_item_hovered = (mouse_pos.x >= sub_item_pos.x && mouse_pos.x <= sub_item_max.x && mouse_pos.y >= sub_item_pos.y && mouse_pos.y <= sub_item_max.y);
                        ImU32 sub_item_bg = sub_item_hovered ? ImGui::GetColorU32(colors.title_bar_menu_item_hover) : ImGui::GetColorU32(bg_color);
                        fg_list->AddRectFilled(sub_item_pos, sub_item_max, sub_item_bg);
                        float text_height = ImGui::GetTextLineHeight();
                        float centered_y = sub_item_pos.y + (menu_item_h - text_height) * 0.5f;
                        fg_list->AddText(ImVec2(sub_item_pos.x + sizes.title_menu_text_padding_x, centered_y), ImGui::GetColorU32(text_color), submenu_items[k]);
                        if (sub_item_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                            active_menu = -1;
                            if (k == 0) service.TriggerCommand(CommandId::FileNew2D);
                            else if (k == 1) service.TriggerCommand(CommandId::FileNew3D);
                            else if (k == 2) service.TriggerCommand(CommandId::FileNewNodeGraph);
                            service.SetActiveMenu(TitleBarMenu::None);
                            menu_click_handled = true;
                        }
                    }
                }
            }
        }
    }
        if (menu_click_handled) {
            service.RequestBlockTabClicksOnce();
        }
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !menu_click_handled) {
        ImVec2 mouse_pos = ImGui::GetMousePos();
        bool in_menu_buttons = (mouse_pos.x >= window_pos.x + menu_x && mouse_pos.x <= window_pos.x + menu_x + menu_count * menu_btn_w && mouse_pos.y >= window_pos.y && mouse_pos.y <= window_pos.y + title_h);
        ImVec2 dropdown_btn_pos = ImVec2(window_pos.x + menu_x + active_menu * menu_btn_w, window_pos.y + title_h);
        bool in_dropdown = (mouse_pos.x >= dropdown_btn_pos.x && mouse_pos.x <= dropdown_btn_pos.x + dropdown_w && mouse_pos.y >= dropdown_btn_pos.y && mouse_pos.y <= dropdown_btn_pos.y + 200);
        if (!in_menu_buttons && !in_dropdown) active_menu = -1;
            active_menu = -1;
            service.SetActiveMenu(TitleBarMenu::None);
    }

    // === Right side: Layout buttons + Control buttons ===
    const float btn_w = sizes.title_control_btn_w;
    const float btn_h = title_h;
    const float layout_btn_w = sizes.title_layout_btn_w;
    const float layout_btn_h = title_h;
    float btn_start_x = io.DisplaySize.x - btn_w * 3;
    float layout_start_x = btn_start_x - layout_btn_w * 3;
    ImVec4 btn_normal = ImVec4(0, 0, 0, 0);
    ImVec4 btn_hover = colors.title_bar_button_hover;
    ImVec4 btn_active = colors.title_bar_button_active;
    ImVec4 close_hover = colors.title_bar_close_hover;
    ImVec4 close_active = colors.title_bar_close_active;
    auto draw_square_btn = [&](float x_pos, float w, float h, const char* id, ImVec4 normal, ImVec4 hover, ImVec4 active, auto draw_icon) -> bool {
        ImGui::SetCursorPos(ImVec2(x_pos, 0));
        ImGui::PushID(id);
        ImGui::PushStyleColor(ImGuiCol_Button, normal);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, active);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
        bool pressed = ImGui::Button("##btn", ImVec2(w, h));
        bool hovered = ImGui::IsItemHovered();
        ImVec2 p0 = ImGui::GetItemRectMin();
        ImVec2 p1 = ImGui::GetItemRectMax();
        ImVec2 center = ImVec2((p0.x + p1.x) * 0.5f, (p0.y + p1.y) * 0.5f);
        draw_icon(center, hovered);
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);
        ImGui::PopID();
        return pressed;
    };
    auto draw_control_btn = [&](float x_pos, const char* id, ImVec4 normal, ImVec4 hover, ImVec4 active, auto draw_icon) -> bool {
        return draw_square_btn(x_pos, btn_w, btn_h, id, normal, hover, active, draw_icon);
    };
    auto draw_layout_btn = [&](float x_pos, const char* id, auto draw_icon) -> bool {
        return draw_square_btn(x_pos, layout_btn_w, layout_btn_h, id, btn_normal, btn_hover, btn_active, draw_icon);
    };
    bool btn_layout_left = draw_layout_btn(layout_start_x + layout_btn_w * 0, "layout_left", [&](ImVec2 c, bool h){
        float alpha = primary_sidebar_visible ? (h ? 1.0f : 0.9f) : (h ? 0.7f : 0.5f);
        ImU32 col = ImGui::GetColorU32(ImVec4(colors.title_bar_layout_icon.x, colors.title_bar_layout_icon.y, colors.title_bar_layout_icon.z, alpha));
        float half = sizes.title_layout_icon_size;
        draw_list->AddRect(ImVec2(c.x-half, c.y-(half-1.0f)), ImVec2(c.x+half, c.y+(half-1.0f)), col, sizes.title_layout_icon_rounding, 0, sizes.title_layout_icon_stroke);
        if (primary_sidebar_visible) draw_list->AddRectFilled(ImVec2(c.x-(half-1.0f), c.y-(half-2.0f)), ImVec2(c.x-(half*0.25f), c.y+(half-2.0f)), col);
        else draw_list->AddRect(ImVec2(c.x-half, c.y-(half-1.0f)), ImVec2(c.x-(half*0.25f), c.y+(half-1.0f)), col, sizes.title_layout_icon_rounding, 0, sizes.title_layout_icon_stroke);
    });
    bool btn_layout_bottom = draw_layout_btn(layout_start_x + layout_btn_w * 1, "layout_bottom", [&](ImVec2 c, bool h){
        float alpha = panel_visible ? (h ? 1.0f : 0.9f) : (h ? 0.7f : 0.5f);
        ImU32 col = ImGui::GetColorU32(ImVec4(colors.title_bar_layout_icon.x, colors.title_bar_layout_icon.y, colors.title_bar_layout_icon.z, alpha));
        float half = sizes.title_layout_icon_size;
        draw_list->AddRect(ImVec2(c.x-half, c.y-(half-1.0f)), ImVec2(c.x+half, c.y+(half-1.0f)), col, sizes.title_layout_icon_rounding, 0, sizes.title_layout_icon_stroke);
        if (panel_visible) draw_list->AddRectFilled(ImVec2(c.x-(half-1.0f), c.y+(half*0.12f)), ImVec2(c.x+(half-1.0f), c.y+(half*0.75f)), col);
        else draw_list->AddRect(ImVec2(c.x-half, c.y+(half*0.12f)), ImVec2(c.x+half, c.y+(half-1.0f)), col, sizes.title_layout_icon_rounding, 0, sizes.title_layout_icon_stroke);
    });
    bool btn_layout_right = draw_layout_btn(layout_start_x + layout_btn_w * 2, "layout_right", [&](ImVec2 c, bool h){
        float alpha = secondary_sidebar_visible ? (h ? 1.0f : 0.9f) : (h ? 0.7f : 0.5f);
        ImU32 col = ImGui::GetColorU32(ImVec4(colors.title_bar_layout_icon.x, colors.title_bar_layout_icon.y, colors.title_bar_layout_icon.z, alpha));
        float half = sizes.title_layout_icon_size;
        draw_list->AddRect(ImVec2(c.x-half, c.y-(half-1.0f)), ImVec2(c.x+half, c.y+(half-1.0f)), col, sizes.title_layout_icon_rounding, 0, sizes.title_layout_icon_stroke);
        if (secondary_sidebar_visible) draw_list->AddRectFilled(ImVec2(c.x+(half*0.25f), c.y-(half-2.0f)), ImVec2(c.x+(half-1.0f), c.y+(half-2.0f)), col);
        else draw_list->AddRect(ImVec2(c.x+(half*0.25f), c.y-(half-1.0f)), ImVec2(c.x+half, c.y+(half-1.0f)), col, sizes.title_layout_icon_rounding, 0, sizes.title_layout_icon_stroke);
    });
    bool btn_min = draw_control_btn(btn_start_x, "min", btn_normal, btn_hover, btn_active, [&](ImVec2 center, bool hovered) {
        ImU32 col = ImGui::GetColorU32(ImVec4(colors.title_bar_control_icon.x, colors.title_bar_control_icon.y, colors.title_bar_control_icon.z, hovered ? 1.0f : 0.8f));
        draw_list->AddLine(ImVec2(center.x - sizes.title_close_icon_size, center.y), ImVec2(center.x + sizes.title_close_icon_size, center.y), col, sizes.title_button_icon_stroke);
    });
    bool is_maximized = (SDL_GetWindowFlags(window) & SDL_WINDOW_MAXIMIZED) != 0;
    bool btn_max = draw_control_btn(btn_start_x + btn_w, "max", btn_normal, btn_hover, btn_active, [&](ImVec2 center, bool hovered) {
        ImU32 col = ImGui::GetColorU32(ImVec4(colors.title_bar_control_icon.x, colors.title_bar_control_icon.y, colors.title_bar_control_icon.z, hovered ? 1.0f : 0.8f));
        if (is_maximized) {
            draw_list->AddRect(ImVec2(center.x - sizes.title_close_icon_size * 0.6f, center.y - sizes.title_close_icon_size), ImVec2(center.x + sizes.title_close_icon_size, center.y + sizes.title_close_icon_size * 0.6f), col, 0, 0, sizes.title_button_icon_stroke);
            draw_list->AddRect(ImVec2(center.x - sizes.title_close_icon_size, center.y - sizes.title_close_icon_size * 0.6f), ImVec2(center.x + sizes.title_close_icon_size * 0.6f, center.y + sizes.title_close_icon_size), col, 0, 0, sizes.title_button_icon_stroke);
        } else {
            draw_list->AddRect(ImVec2(center.x - sizes.title_close_icon_size, center.y - sizes.title_close_icon_size), ImVec2(center.x + sizes.title_close_icon_size, center.y + sizes.title_close_icon_size), col, 0, 0, sizes.title_button_icon_stroke);
        }
    });
    bool btn_close = draw_control_btn(btn_start_x + btn_w * 2, "close", btn_normal, close_hover, close_active, [&](ImVec2 center, bool hovered) {
        ImU32 col = ImGui::GetColorU32(ImVec4(colors.title_bar_control_icon.x, colors.title_bar_control_icon.y, colors.title_bar_control_icon.z, hovered ? 1.0f : 0.8f));
        float size = sizes.title_close_icon_size;
        draw_list->AddLine(ImVec2(center.x - size, center.y - size), ImVec2(center.x + size, center.y + size), col, 1.0f);
        draw_list->AddLine(ImVec2(center.x - size, center.y + size), ImVec2(center.x + size, center.y - size), col, 1.0f);
    });
    // 事件写回 service (commands)
    if (btn_layout_left) service.TriggerCommand(CommandId::TogglePrimarySidebar);
    if (btn_layout_bottom) service.TriggerCommand(CommandId::TogglePanel);
    if (btn_layout_right) service.TriggerCommand(CommandId::ToggleSecondarySidebar);
    if (btn_min) service.TriggerCommand(CommandId::WindowMinimize);
    if (btn_max) service.TriggerCommand(CommandId::WindowMaximize);
    if (btn_close) service.TriggerCommand(CommandId::WindowClose);

    // 拖动窗口逻辑（原样迁移）
    static bool title_dragging = false;
    static bool pending_restore = false;
    static float drag_offset_x = 0, drag_offset_y = 0;
    static float click_global_x = 0, click_global_y = 0;
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        ImVec2 mouse_pos = ImGui::GetMousePos();
        float button_block_x = layout_start_x;
        if (mouse_pos.x < button_block_x) {
            float global_x, global_y;
            SDL_GetGlobalMouseState(&global_x, &global_y);
            click_global_x = global_x;
            click_global_y = global_y;
            bool is_maximized = (SDL_GetWindowFlags(window) & SDL_WINDOW_MAXIMIZED) != 0;
            if (is_maximized) { pending_restore = true; title_dragging = false; }
            else {
                int win_x, win_y;
                SDL_GetWindowPosition(window, &win_x, &win_y);
                drag_offset_x = global_x - win_x;
                drag_offset_y = global_y - win_y;
                title_dragging = true;
            }
        }
    }
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        if (pending_restore) {
            float cur_x, cur_y;
            SDL_GetGlobalMouseState(&cur_x, &cur_y);
            bool moved = (fabsf(cur_x - click_global_x) > 0.1f) || (fabsf(cur_y - click_global_y) > 0.1f);
            if (moved) {
                pending_restore = false;
                SDL_RestoreWindow(window);
                int win_w, win_h;
                SDL_GetWindowSize(window, &win_w, &win_h);
                float ratio = cur_x / io.DisplaySize.x;
                if (ratio < 0.05f) ratio = 0.05f;
                if (ratio > 0.95f) ratio = 0.95f;
                int new_x = (int)(cur_x - win_w * ratio);
                int new_y = (int)(cur_y - title_h * 0.5f);
                SDL_SetWindowPosition(window, new_x, new_y);
                drag_offset_x = win_w * ratio;
                drag_offset_y = title_h * 0.5f;
                title_dragging = true;
            }
        }
        if (title_dragging) {
            float global_x, global_y;
            SDL_GetGlobalMouseState(&global_x, &global_y);
            SDL_SetWindowPosition(window, (int)(global_x - drag_offset_x), (int)(global_y - drag_offset_y));
        }
    } else { title_dragging = false; pending_restore = false; }
    ImGui::End();
    ImGui::PopStyleVar(4);
    ImDrawList* fg = ImGui::GetForegroundDrawList();
    ImU32 border_color = ImGui::GetColorU32(colors.title_bar_border);
    ImVec2 line_start = ImVec2(0, title_h - sizes.title_bar_border_thickness);
    ImVec2 line_end = ImVec2(io.DisplaySize.x, title_h - sizes.title_bar_border_thickness);
    fg->AddLine(line_start, line_end, border_color, sizes.title_bar_border_thickness);
}
