#include "title_bar.h"
#include "imgui.h"
#define _USE_MATH_DEFINES
#include <math.h>

// Draw a 3D octahedron (正八面体) with rotation
static void DrawPhageIcon(ImDrawList* draw_list, ImVec2 center, float size, ImU32 color_light, ImU32 color_dark, ImU32 color_edge)
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
    bool drawn_edges[6][6] = {};
    for (int f = 0; f < 8; f++) {
        for (int e = 0; e < 3; e++) {
            int v0 = faces[f][e];
            int v1 = faces[f][(e + 1) % 3];
            if (v0 > v1) { int temp = v0; v0 = v1; v1 = temp; }

            if (!drawn_edges[v0][v1]) {
                draw_list->AddLine(projected[v0], projected[v1], IM_COL32(150, 200, 255, 50), 1.6f);
                draw_list->AddLine(projected[v0], projected[v1], IM_COL32(220, 240, 255, 255), 1.0f);
                drawn_edges[v0][v1] = true;
            }
        }
    }
}

TitleBarResult DrawTitleBar(SDL_Window* window, float title_h, 
                            bool primary_sidebar_visible, 
                            bool panel_visible, 
                            bool secondary_sidebar_visible)
{
    TitleBarResult result{};
    ImGuiIO& io = ImGui::GetIO();

    // VS Code style colors
    ImVec4 bg_color = ImVec4(0.173f, 0.173f, 0.188f, 1.0f);  // #2C2C30
    ImVec4 menu_hover = ImVec4(0.282f, 0.282f, 0.298f, 1.0f); // #48484C
    ImVec4 text_color = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);

    // Background
    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    bg->AddRectFilled(ImVec2(0, 0), ImVec2(io.DisplaySize.x, title_h), ImGui::GetColorU32(bg_color));

    // Title bar window
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
    ImDrawList* fg_list = ImGui::GetForegroundDrawList();  // For dropdown menus (always on top)
    ImVec2 window_pos = ImGui::GetWindowPos();
    
    // === Left side: Icon ===
    float start_x = 6.0f;
    float icon_draw_size = title_h * 1.80f;   // larger visual size
    float icon_layout_w = title_h * 0.85f;    // keep compact horizontal footprint
    float icon_offset_x = start_x;
    
    // Draw phage icon (visual size decoupled from layout width)
    ImVec2 icon_center = ImVec2(window_pos.x + icon_offset_x + icon_layout_w * 0.5f, window_pos.y + title_h * 0.5f);
    ImU32 color_light = IM_COL32(100, 150, 255, 255);
    ImU32 color_dark = IM_COL32(60, 100, 200, 255);
    ImU32 color_edge = IM_COL32(150, 180, 255, 255);
    DrawPhageIcon(draw_list, icon_center, icon_draw_size, color_light, color_dark, color_edge);
    
    // === Menu bar area - pure manual implementation ===
    float menu_x = icon_offset_x + icon_layout_w + 8;
    float menu_btn_w = 60.0f;      // Menu button width
    float dropdown_w = 150.0f;     // Fixed dropdown width
    
    // Menu items data
    const char* menu_names[] = { "File", "Edit", "View", "Help" };
    const int menu_count = 4;
    
    static int active_menu = -1;  // -1 means no menu open
    
    // Draw menu buttons
    for (int i = 0; i < menu_count; i++)
    {
        ImVec2 btn_pos = ImVec2(window_pos.x + menu_x + i * menu_btn_w, window_pos.y);
        ImVec2 btn_size = ImVec2(menu_btn_w, title_h);
        ImVec2 btn_max = ImVec2(btn_pos.x + btn_size.x, btn_pos.y + btn_size.y);
        
        // Check hover
        ImVec2 mouse_pos = ImGui::GetMousePos();
        bool is_hovered = (mouse_pos.x >= btn_pos.x && mouse_pos.x <= btn_max.x &&
                          mouse_pos.y >= btn_pos.y && mouse_pos.y <= btn_max.y);
        
        // Check click
        if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            active_menu = (active_menu == i) ? -1 : i;
        }
        
        // Draw button background
        ImU32 btn_bg = (is_hovered || active_menu == i) 
            ? ImGui::GetColorU32(menu_hover) 
            : ImGui::GetColorU32(bg_color);
        draw_list->AddRectFilled(btn_pos, btn_max, btn_bg);
        
        // Draw button text (vertically centered)
        ImVec2 text_pos = ImVec2(btn_pos.x + 8, btn_pos.y + (title_h - ImGui::GetTextLineHeight()) * 0.5f);
        draw_list->AddText(text_pos, ImGui::GetColorU32(text_color), menu_names[i]);
        
        // Draw dropdown menu if this menu is active
        if (active_menu == i)
        {
            // Menu items
            const char* items[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
            bool has_submenu[] = { false, false, false, false, false };  // Track which items have submenus
            
            if (i == 0) { // File
                items[0] = "New";
                has_submenu[0] = true;  // "New" has a submenu
                items[1] = "Open";
                items[2] = "Save";
                items[3] = "Exit";
            }
            else if (i == 1) { // Edit
                items[0] = "Undo";
                items[1] = "Redo";
            }
            else if (i == 2) { // View
                items[0] = "Explorer";
                items[1] = "Console";
            }
            else if (i == 3) { // Help
                items[0] = "About";
            }
            
            // Draw dropdown menu box with fixed width
            float menu_item_h = 24.0f;
            int item_count = 0;
            for (int j = 0; j < 5; j++)
                if (items[j]) item_count++;
            
            ImVec2 dropdown_pos = ImVec2(btn_pos.x, btn_max.y);
            ImVec2 dropdown_size = ImVec2(dropdown_w, menu_item_h * item_count);
            ImVec2 dropdown_max = ImVec2(dropdown_pos.x + dropdown_size.x, dropdown_pos.y + dropdown_size.y);
            
            // Use foreground DrawList so dropdown appears on top
            fg_list->AddRectFilled(dropdown_pos, dropdown_max, ImGui::GetColorU32(bg_color));
            fg_list->AddRect(dropdown_pos, dropdown_max, ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.5f, 1.0f)));
            
            // Track submenu state
            static int hovered_submenu_item = -1;
            int current_hovered_item = -1;
            
            // Pre-check if mouse is in submenu area (for File menu "New" item)
            bool mouse_in_new_submenu = false;
            if (i == 0) {
                ImVec2 submenu_pos = ImVec2(dropdown_max.x, dropdown_pos.y);
                ImVec2 submenu_size = ImVec2(dropdown_w, menu_item_h * 2);  // 2 submenu items
                ImVec2 submenu_max = ImVec2(submenu_pos.x + submenu_size.x, submenu_pos.y + submenu_size.y);
                mouse_in_new_submenu = (mouse_pos.x >= submenu_pos.x && mouse_pos.x <= submenu_max.x &&
                                       mouse_pos.y >= submenu_pos.y && mouse_pos.y <= submenu_max.y);
            }
            
            // Draw menu items
            for (int j = 0; j < item_count; j++)
            {
                if (!items[j]) continue;
                
                ImVec2 item_pos = ImVec2(dropdown_pos.x, dropdown_pos.y + j * menu_item_h);
                ImVec2 item_max = ImVec2(item_pos.x + dropdown_size.x, item_pos.y + menu_item_h);
                
                bool item_hovered = (mouse_pos.x >= item_pos.x && mouse_pos.x <= item_max.x &&
                                    mouse_pos.y >= item_pos.y && mouse_pos.y <= item_max.y);
                
                // If this is "New" item and mouse is in submenu, keep it highlighted
                if (i == 0 && j == 0 && mouse_in_new_submenu) {
                    item_hovered = true;
                }
                
                if (item_hovered) current_hovered_item = j;
                
                ImU32 item_bg = item_hovered ? ImGui::GetColorU32(ImVec4(0.25f, 0.25f, 0.25f, 1.0f)) : 
                                               ImGui::GetColorU32(bg_color);
                fg_list->AddRectFilled(item_pos, item_max, item_bg);
                
                // Vertically center the text
                float text_height = ImGui::GetTextLineHeight();
                float centered_y = item_pos.y + (menu_item_h - text_height) * 0.5f;
                fg_list->AddText(ImVec2(item_pos.x + 8, centered_y), 
                    ImGui::GetColorU32(text_color), items[j]);
                
                // Draw arrow for submenu
                if (has_submenu[j]) {
                    float arrow_x = item_max.x - 12;
                    float arrow_y = item_pos.y + menu_item_h * 0.5f;
                    fg_list->AddTriangleFilled(
                        ImVec2(arrow_x - 3, arrow_y - 4),
                        ImVec2(arrow_x - 3, arrow_y + 4),
                        ImVec2(arrow_x + 2, arrow_y),
                        ImGui::GetColorU32(text_color)
                    );
                }
                
                // Handle item click (only for items without submenu)
                if (item_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !has_submenu[j])
                {
                    active_menu = -1;  // Close menu
                    
                    // Map menu item to action
                    if (i == 0) { // File menu
                        if (j == 1) result.menu_action = MenuAction::FileOpen;
                        else if (j == 2) result.menu_action = MenuAction::FileSave;
                        else if (j == 3) result.menu_action = MenuAction::FileExit;
                    }
                    else if (i == 1) { // Edit menu
                        if (j == 0) result.menu_action = MenuAction::EditUndo;
                        else if (j == 1) result.menu_action = MenuAction::EditRedo;
                    }
                    else if (i == 2) { // View menu
                        if (j == 0) result.menu_action = MenuAction::ViewExplorer;
                        else if (j == 1) result.menu_action = MenuAction::ViewConsole;
                    }
                    else if (i == 3) { // Help menu
                        if (j == 0) result.menu_action = MenuAction::HelpAbout;
                    }
                }
            }
            
            // Update hovered submenu item
            hovered_submenu_item = current_hovered_item;
            
            // Draw submenu for "New" option
            if (i == 0) {
                // Check if we should show submenu (hovering on "New" item OR hovering in submenu area)
                bool show_submenu = false;
                
                // Calculate submenu area
                const char* submenu_items[] = { "2D Scene", "3D Scene" };
                int submenu_count = 2;
                ImVec2 submenu_pos = ImVec2(dropdown_max.x, dropdown_pos.y);
                ImVec2 submenu_size = ImVec2(dropdown_w, menu_item_h * submenu_count);
                ImVec2 submenu_max = ImVec2(submenu_pos.x + submenu_size.x, submenu_pos.y + submenu_size.y);
                
                // Check if mouse is in submenu area
                bool mouse_in_submenu = (mouse_pos.x >= submenu_pos.x && mouse_pos.x <= submenu_max.x &&
                                        mouse_pos.y >= submenu_pos.y && mouse_pos.y <= submenu_max.y);
                
                // Show submenu if hovering on "New" item or in submenu area
                if (hovered_submenu_item == 0 || mouse_in_submenu) {
                    show_submenu = true;
                    hovered_submenu_item = 0;  // Keep "New" highlighted when in submenu
                }
                
                if (show_submenu) {
                    fg_list->AddRectFilled(submenu_pos, submenu_max, ImGui::GetColorU32(bg_color));
                    fg_list->AddRect(submenu_pos, submenu_max, ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.5f, 1.0f)));
                    
                    for (int k = 0; k < submenu_count; k++) {
                        ImVec2 sub_item_pos = ImVec2(submenu_pos.x, submenu_pos.y + k * menu_item_h);
                        ImVec2 sub_item_max = ImVec2(sub_item_pos.x + submenu_size.x, sub_item_pos.y + menu_item_h);
                        
                        bool sub_item_hovered = (mouse_pos.x >= sub_item_pos.x && mouse_pos.x <= sub_item_max.x &&
                                                mouse_pos.y >= sub_item_pos.y && mouse_pos.y <= sub_item_max.y);
                        
                        ImU32 sub_item_bg = sub_item_hovered ? ImGui::GetColorU32(ImVec4(0.25f, 0.25f, 0.25f, 1.0f)) : 
                                                               ImGui::GetColorU32(bg_color);
                        fg_list->AddRectFilled(sub_item_pos, sub_item_max, sub_item_bg);
                        
                        float text_height = ImGui::GetTextLineHeight();
                        float centered_y = sub_item_pos.y + (menu_item_h - text_height) * 0.5f;
                        fg_list->AddText(ImVec2(sub_item_pos.x + 8, centered_y), 
                            ImGui::GetColorU32(text_color), submenu_items[k]);
                        
                        // Handle submenu click
                        if (sub_item_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                            active_menu = -1;
                            if (k == 0) result.menu_action = MenuAction::FileNew2D;
                            else if (k == 1) result.menu_action = MenuAction::FileNew3D;
                        }
                    }
                }
            }
        }
    }
    
    // Close menu if clicking outside menu area
    if (active_menu >= 0 && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        ImVec2 mouse_pos = ImGui::GetMousePos();
        
        // Check if click is in menu button area
        bool in_menu_buttons = (mouse_pos.x >= window_pos.x + menu_x && 
                               mouse_pos.x <= window_pos.x + menu_x + menu_count * menu_btn_w &&
                               mouse_pos.y >= window_pos.y && mouse_pos.y <= window_pos.y + title_h);
        
        // Check if click is in dropdown area
        ImVec2 dropdown_btn_pos = ImVec2(window_pos.x + menu_x + active_menu * menu_btn_w, window_pos.y + title_h);
        bool in_dropdown = (mouse_pos.x >= dropdown_btn_pos.x && 
                           mouse_pos.x <= dropdown_btn_pos.x + dropdown_w &&
                           mouse_pos.y >= dropdown_btn_pos.y && 
                           mouse_pos.y <= dropdown_btn_pos.y + 200);  // Approximate max dropdown height
        
        // Close menu if not clicking menu buttons or dropdown
        if (!in_menu_buttons && !in_dropdown)
            active_menu = -1;
    }
    
    // === Right side: Layout buttons + Control buttons ===
    const float btn_w = 46.0f;
    const float btn_h = title_h;
    const float layout_btn_w = 32.0f;
    const float layout_btn_h = title_h;
    float btn_start_x = io.DisplaySize.x - btn_w * 3;
    float layout_start_x = btn_start_x - layout_btn_w * 3;
    
    // Button colors
    ImVec4 btn_normal = ImVec4(0, 0, 0, 0);
    ImVec4 btn_hover = ImVec4(1.0f, 1.0f, 1.0f, 0.1f);
    ImVec4 btn_active = ImVec4(1.0f, 1.0f, 1.0f, 0.15f);
    ImVec4 close_hover = ImVec4(0.81f, 0.26f, 0.26f, 1.0f);
    ImVec4 close_active = ImVec4(0.91f, 0.36f, 0.36f, 1.0f);
    
    auto draw_square_btn = [&](float x_pos, float w, float h, const char* id, ImVec4 normal, ImVec4 hover, ImVec4 active, auto draw_icon) -> bool
    {
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

    auto draw_control_btn = [&](float x_pos, const char* id, ImVec4 normal, ImVec4 hover, ImVec4 active, auto draw_icon) -> bool
    {
        return draw_square_btn(x_pos, btn_w, btn_h, id, normal, hover, active, draw_icon);
    };
    
    // Layout buttons (right side, before window controls)
    auto draw_layout_btn = [&](float x_pos, const char* id, auto draw_icon) -> bool
    {
        return draw_square_btn(x_pos, layout_btn_w, layout_btn_h, id, btn_normal, btn_hover, btn_active, draw_icon);
    };

    bool btn_layout_left = draw_layout_btn(layout_start_x + layout_btn_w * 0, "layout_left", [&](ImVec2 c, bool h){
        // 左侧主面板按钮 - 根据状态显示填充或空心
        float alpha = primary_sidebar_visible ? (h ? 1.0f : 0.9f) : (h ? 0.7f : 0.5f);
        ImU32 col = ImGui::GetColorU32(ImVec4(1,1,1,alpha));
        draw_list->AddRect(ImVec2(c.x-8, c.y-7), ImVec2(c.x+8, c.y+7), col, 2.0f, 0, 1.5f);
        if (primary_sidebar_visible) {
            // 打开状态：左侧面板填充
            draw_list->AddRectFilled(ImVec2(c.x-7, c.y-6), ImVec2(c.x-2, c.y+6), col);
        } else {
            // 关闭状态：左侧面板空心
            draw_list->AddRect(ImVec2(c.x-8, c.y-7), ImVec2(c.x-2, c.y+7), col, 1.5f, 0, 1.5f);
        }
    });
    bool btn_layout_bottom = draw_layout_btn(layout_start_x + layout_btn_w * 1, "layout_bottom", [&](ImVec2 c, bool h){
        // 底部面板按钮 - 根据状态显示填充或空心
        float alpha = panel_visible ? (h ? 1.0f : 0.9f) : (h ? 0.7f : 0.5f);
        ImU32 col = ImGui::GetColorU32(ImVec4(1,1,1,alpha));
        draw_list->AddRect(ImVec2(c.x-8, c.y-7), ImVec2(c.x+8, c.y+7), col, 2.0f, 0, 1.5f);
        if (panel_visible) {
            // 打开状态：底部面板填充
            draw_list->AddRectFilled(ImVec2(c.x-7, c.y+1), ImVec2(c.x+7, c.y+6), col);
        } else {
            // 关闭状态：底部面板空心
            draw_list->AddRect(ImVec2(c.x-8, c.y+1), ImVec2(c.x+8, c.y+7), col, 1.5f, 0, 1.5f);
        }
    });
    bool btn_layout_right = draw_layout_btn(layout_start_x + layout_btn_w * 2, "layout_right", [&](ImVec2 c, bool h){
        // 右侧次要面板按钮 - 根据状态显示填充或空心
        float alpha = secondary_sidebar_visible ? (h ? 1.0f : 0.9f) : (h ? 0.7f : 0.5f);
        ImU32 col = ImGui::GetColorU32(ImVec4(1,1,1,alpha));
        draw_list->AddRect(ImVec2(c.x-8, c.y-7), ImVec2(c.x+8, c.y+7), col, 2.0f, 0, 1.5f);
        if (secondary_sidebar_visible) {
            // 打开状态：右侧面板填充
            draw_list->AddRectFilled(ImVec2(c.x+2, c.y-6), ImVec2(c.x+7, c.y+6), col);
        } else {
            // 关闭状态：右侧面板空心
            draw_list->AddRect(ImVec2(c.x+2, c.y-7), ImVec2(c.x+8, c.y+7), col, 1.5f, 0, 1.5f);
        }
    });

    // Minimize button
    bool btn_min = draw_control_btn(btn_start_x, "min", btn_normal, btn_hover, btn_active, 
        [&](ImVec2 center, bool hovered) {
            ImU32 col = ImGui::GetColorU32(ImVec4(1, 1, 1, hovered ? 1.0f : 0.8f));
            draw_list->AddLine(ImVec2(center.x - 5, center.y), ImVec2(center.x + 5, center.y), col, 1.0f);
        });

    // Maximize/Restore button
    bool is_maximized = (SDL_GetWindowFlags(window) & SDL_WINDOW_MAXIMIZED) != 0;
    bool btn_max = draw_control_btn(btn_start_x + btn_w, "max", btn_normal, btn_hover, btn_active,
        [&](ImVec2 center, bool hovered) {
            ImU32 col = ImGui::GetColorU32(ImVec4(1, 1, 1, hovered ? 1.0f : 0.8f));
            if (is_maximized) {
                // Restore icon: two overlapping squares
                draw_list->AddRect(ImVec2(center.x - 3, center.y - 5), ImVec2(center.x + 5, center.y + 3), col, 0, 0, 1.0f);
                draw_list->AddRect(ImVec2(center.x - 5, center.y - 3), ImVec2(center.x + 3, center.y + 5), col, 0, 0, 1.0f);
            } else {
                // Maximize icon: single square
                draw_list->AddRect(ImVec2(center.x - 5, center.y - 5), ImVec2(center.x + 5, center.y + 5), col, 0, 0, 1.0f);
            }
        });

    // Close button
    bool btn_close = draw_control_btn(btn_start_x + btn_w * 2, "close", btn_normal, close_hover, close_active,
        [&](ImVec2 center, bool hovered) {
            ImU32 col = ImGui::GetColorU32(ImVec4(1, 1, 1, hovered ? 1.0f : 0.8f));
            float size = 5.0f;
            draw_list->AddLine(ImVec2(center.x - size, center.y - size), ImVec2(center.x + size, center.y + size), col, 1.0f);
            draw_list->AddLine(ImVec2(center.x - size, center.y + size), ImVec2(center.x + size, center.y - size), col, 1.0f);
        });


    // Window dragging (outside button area)
    static bool title_dragging = false;
    static bool pending_restore = false; // click happened on maximized window, wait for movement
    static float drag_offset_x = 0, drag_offset_y = 0;
    static float click_global_x = 0, click_global_y = 0;

    // Mouse down: record state, but don't restore yet
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        ImVec2 mouse_pos = ImGui::GetMousePos();
        float button_block_x = layout_start_x; // layout buttons + control buttons occupy the right side
        if (mouse_pos.x < button_block_x) // Not in button area
        {
            float global_x, global_y;
            SDL_GetGlobalMouseState(&global_x, &global_y);
            click_global_x = global_x;
            click_global_y = global_y;

            bool is_maximized = (SDL_GetWindowFlags(window) & SDL_WINDOW_MAXIMIZED) != 0;
            if (is_maximized)
            {
                pending_restore = true;  // wait until cursor moves to restore
                title_dragging = false;
            }
            else
            {
                // Start normal drag immediately
                int win_x, win_y;
                SDL_GetWindowPosition(window, &win_x, &win_y);
                drag_offset_x = global_x - win_x;
                drag_offset_y = global_y - win_y;
                title_dragging = true;
            }
        }
    }

    // While mouse held: if pending_restore, restore on first movement; else drag
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        if (pending_restore)
        {
            // Only restore when cursor actually moves
            float cur_x, cur_y;
            SDL_GetGlobalMouseState(&cur_x, &cur_y);
            bool moved = (fabsf(cur_x - click_global_x) > 0.1f) || (fabsf(cur_y - click_global_y) > 0.1f);
            if (moved)
            {
                pending_restore = false;

                // Restore window to normal size
                SDL_RestoreWindow(window);

                // Get restored size
                int win_w, win_h;
                SDL_GetWindowSize(window, &win_w, &win_h);

                // Keep cursor at same relative X position (ratio on maximized window width)
                float ratio = cur_x / io.DisplaySize.x;
                if (ratio < 0.05f) ratio = 0.05f;
                if (ratio > 0.95f) ratio = 0.95f;

                int new_x = (int)(cur_x - win_w * ratio);
                int new_y = (int)(cur_y - title_h * 0.5f);
                SDL_SetWindowPosition(window, new_x, new_y);

                // Set drag offsets so dragging continues smoothly
                drag_offset_x = win_w * ratio;
                drag_offset_y = title_h * 0.5f;
                title_dragging = true;
            }
        }

        if (title_dragging)
        {
            float global_x, global_y;
            SDL_GetGlobalMouseState(&global_x, &global_y);
            SDL_SetWindowPosition(window, (int)(global_x - drag_offset_x), (int)(global_y - drag_offset_y));
        }
    }
    else
    {
        // Mouse released
        title_dragging = false;
        pending_restore = false;
    }

    ImGui::End();
    ImGui::PopStyleVar(4);

    // Draw border line at bottom of title bar
    ImDrawList* fg = ImGui::GetForegroundDrawList();
    ImU32 border_color = IM_COL32(62, 62, 66, 255);  // VS Code style #3E3E42
    ImVec2 line_start = ImVec2(0, title_h - 1);
    ImVec2 line_end = ImVec2(io.DisplaySize.x, title_h - 1);
    fg->AddLine(line_start, line_end, border_color, 2.0f);

    result.minimize = btn_min;
    result.maximize = btn_max;
    result.close = btn_close;
    result.toggle_primary_sidebar = btn_layout_left;     // 左边按钮控制主面板
    result.toggle_panel = btn_layout_bottom;             // 中间按钮控制底部面板
    result.toggle_secondary_sidebar = btn_layout_right;  // 右边按钮控制次要面板
    return result;
}
