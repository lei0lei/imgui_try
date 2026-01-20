#include "title_bar.h"
#include "imgui.h"
#define _USE_MATH_DEFINES
#include <math.h>

// Draw a 3D phage head icon (icosahedron-like geometry)
static void DrawPhageIcon(ImDrawList* draw_list, ImVec2 center, float size, ImU32 color_light, ImU32 color_dark, ImU32 color_edge)
{
    // Create a stylized icosahedron/phage head
    const float radius = size * 0.5f;
    const int segments = 5; // Pentagon base
    
    // Draw pentagon faces with lighting
    for (int i = 0; i < segments; i++)
    {
        float angle1 = (float)i / segments * 2.0f * (float)M_PI - (float)M_PI * 0.5f;
        float angle2 = (float)(i + 1) / segments * 2.0f * (float)M_PI - (float)M_PI * 0.5f;
        
        ImVec2 p1 = ImVec2(center.x + cosf(angle1) * radius, center.y + sinf(angle1) * radius * 0.8f);
        ImVec2 p2 = ImVec2(center.x + cosf(angle2) * radius, center.y + sinf(angle2) * radius * 0.8f);
        ImVec2 top = ImVec2(center.x, center.y - radius * 0.6f);
        
        // Alternate light and dark faces for 3D effect
        ImU32 face_color = (i % 2 == 0) ? color_light : color_dark;
        draw_list->AddTriangleFilled(p1, p2, top, face_color);
    }
    
    // Draw edges for definition
    for (int i = 0; i < segments; i++)
    {
        float angle1 = (float)i / segments * 2.0f * (float)M_PI - (float)M_PI * 0.5f;
        float angle2 = (float)(i + 1) / segments * 2.0f * (float)M_PI - (float)M_PI * 0.5f;
        
        ImVec2 p1 = ImVec2(center.x + cosf(angle1) * radius, center.y + sinf(angle1) * radius * 0.8f);
        ImVec2 p2 = ImVec2(center.x + cosf(angle2) * radius, center.y + sinf(angle2) * radius * 0.8f);
        ImVec2 top = ImVec2(center.x, center.y - radius * 0.6f);
        
        draw_list->AddLine(p1, p2, color_edge, 1.2f);
        draw_list->AddLine(p1, top, color_edge, 1.2f);
    }
    
    // Base pentagon
    for (int i = 0; i < segments; i++)
    {
        float angle = (float)i / segments * 2.0f * (float)M_PI - (float)M_PI * 0.5f;
        ImVec2 p = ImVec2(center.x + cosf(angle) * radius, center.y + sinf(angle) * radius * 0.8f);
        draw_list->AddCircleFilled(p, 1.5f, color_edge, 8);
    }
}

TitleBarResult DrawTitleBar(SDL_Window* window, float title_h)
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
                              ImGuiWindowFlags_NoBringToFrontOnFocus;
    
    ImGui::Begin("TitleBar", nullptr, flags);
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 window_pos = ImGui::GetWindowPos();
    
    // === Left side: Icon ===
    float start_x = 10;
    float icon_size = title_h * 0.65f;
    
    // Draw phage icon
    ImVec2 icon_center = ImVec2(window_pos.x + start_x + icon_size * 0.5f, window_pos.y + title_h * 0.5f);
    ImU32 color_light = IM_COL32(100, 150, 255, 255);
    ImU32 color_dark = IM_COL32(60, 100, 200, 255);
    ImU32 color_edge = IM_COL32(150, 180, 255, 255);
    DrawPhageIcon(draw_list, icon_center, icon_size, color_light, color_dark, color_edge);
    
    // === Right side: Control buttons ===
    const float btn_w = 46.0f;
    const float btn_h = title_h;
    float btn_start_x = io.DisplaySize.x - btn_w * 3;
    
    // Button colors
    ImVec4 btn_normal = ImVec4(0, 0, 0, 0);
    ImVec4 btn_hover = ImVec4(1.0f, 1.0f, 1.0f, 0.1f);
    ImVec4 btn_active = ImVec4(1.0f, 1.0f, 1.0f, 0.15f);
    ImVec4 close_hover = ImVec4(0.81f, 0.26f, 0.26f, 1.0f);
    ImVec4 close_active = ImVec4(0.91f, 0.36f, 0.36f, 1.0f);
    
    auto draw_control_btn = [&](float x_pos, const char* id, ImVec4 normal, ImVec4 hover, ImVec4 active, auto draw_icon) -> bool
    {
        ImGui::SetCursorPos(ImVec2(x_pos, 0));
        
        ImGui::PushID(id);
        ImGui::PushStyleColor(ImGuiCol_Button, normal);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, active);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
        
        bool pressed = ImGui::Button("##btn", ImVec2(btn_w, btn_h));
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
    
    // Minimize button
    bool btn_min = draw_control_btn(btn_start_x, "min", btn_normal, btn_hover, btn_active, 
        [&](ImVec2 center, bool hovered) {
            ImU32 col = ImGui::GetColorU32(ImVec4(1, 1, 1, hovered ? 1.0f : 0.8f));
            draw_list->AddLine(ImVec2(center.x - 5, center.y), ImVec2(center.x + 5, center.y), col, 1.0f);
        });
    
    // Maximize button
    bool btn_max = draw_control_btn(btn_start_x + btn_w, "max", btn_normal, btn_hover, btn_active,
        [&](ImVec2 center, bool hovered) {
            ImU32 col = ImGui::GetColorU32(ImVec4(1, 1, 1, hovered ? 1.0f : 0.8f));
            draw_list->AddRect(ImVec2(center.x - 5, center.y - 5), ImVec2(center.x + 5, center.y + 5), col, 0, 0, 1.0f);
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
        if (mouse_pos.x < btn_start_x) // Not in button area
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

    result.minimize = btn_min;
    result.maximize = btn_max;
    result.close = btn_close;
    return result;
}
