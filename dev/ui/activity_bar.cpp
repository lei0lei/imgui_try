#include "activity_bar.h"
#include "imgui.h"

// Global state for activity bar
static ActivityBarItem g_selected = ActivityBarItem::Explorer;
static bool g_sidebar_visible = false;

ActivityBarResult DrawActivityBar(float title_h, float status_bar_h, float width)
{
    ActivityBarResult result{};
    ImGuiIO& io = ImGui::GetIO();

    // VS Code activity bar color
    ImVec4 bg_color = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);  // #333333
    ImVec4 hover_color = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    ImVec4 active_color = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    ImVec4 icon_color = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);

    // Background
    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    float bar_start_y = title_h;
    float bar_end_y = io.DisplaySize.y - status_bar_h;
    bg->AddRectFilled(ImVec2(0, bar_start_y), ImVec2(width, bar_end_y), ImGui::GetColorU32(bg_color));

    // Activity bar window
    ImGui::SetNextWindowPos(ImVec2(0, bar_start_y));
    ImGui::SetNextWindowSize(ImVec2(width, bar_end_y - bar_start_y));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 4));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                              ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
                              ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav;

    ImGui::Begin("ActivityBar", nullptr, flags);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 window_pos = ImGui::GetWindowPos();
    
    // Item data
    struct ActivityItem {
        ActivityBarItem id;
        const char* tooltip;
        // We'll draw simple icons with shapes
    };

    ActivityItem items[] = {
        { ActivityBarItem::Explorer, "Explorer" },
        { ActivityBarItem::Search, "Search" },
        { ActivityBarItem::SourceControl, "Source Control" },
        { ActivityBarItem::Debug, "Debug" },
        { ActivityBarItem::Extensions, "Extensions" }
    };

    float item_size = width;
    float item_y = window_pos.y + 8;

    for (const auto& item : items)
    {
        ImVec2 item_min = ImVec2(window_pos.x, item_y);
        ImVec2 item_max = ImVec2(window_pos.x + width, item_y + item_size);
        ImVec2 mouse_pos = ImGui::GetMousePos();
        
        bool is_hovered = (mouse_pos.x >= item_min.x && mouse_pos.x <= item_max.x &&
                          mouse_pos.y >= item_min.y && mouse_pos.y <= item_max.y);
        bool is_selected = (g_selected == item.id);

        // Background for hover/active
        if (is_selected)
        {
            draw_list->AddRectFilled(item_min, item_max, ImGui::GetColorU32(active_color));
            // Active indicator (left border)
            draw_list->AddRectFilled(ImVec2(item_min.x, item_min.y), 
                                   ImVec2(item_min.x + 2, item_max.y), 
                                   IM_COL32(0, 122, 204, 255));  // VS Code blue
        }
        else if (is_hovered)
        {
            draw_list->AddRectFilled(item_min, item_max, ImGui::GetColorU32(hover_color));
        }

        // Draw simple icon in center
        ImVec2 icon_center = ImVec2(item_min.x + width * 0.5f, item_min.y + item_size * 0.5f);
        float icon_size = 20.0f;
        ImU32 icon_col = ImGui::GetColorU32(icon_color);

        switch (item.id)
        {
            case ActivityBarItem::Explorer:
                // File icon (document with folded corner)
                draw_list->AddRect(ImVec2(icon_center.x - 8, icon_center.y - 10),
                                 ImVec2(icon_center.x + 8, icon_center.y + 10), icon_col, 0, 0, 2.0f);
                draw_list->AddLine(ImVec2(icon_center.x - 4, icon_center.y - 10),
                                 ImVec2(icon_center.x - 4, icon_center.y - 6), icon_col, 2.0f);
                break;
                
            case ActivityBarItem::Search:
                // Magnifying glass
                draw_list->AddCircle(ImVec2(icon_center.x - 2, icon_center.y - 2), 6, icon_col, 12, 2.0f);
                draw_list->AddLine(ImVec2(icon_center.x + 3, icon_center.y + 3),
                                 ImVec2(icon_center.x + 8, icon_center.y + 8), icon_col, 2.0f);
                break;
                
            case ActivityBarItem::SourceControl:
                // Branch icon
                draw_list->AddCircle(ImVec2(icon_center.x - 4, icon_center.y - 6), 3, icon_col, 12, 2.0f);
                draw_list->AddCircle(ImVec2(icon_center.x - 4, icon_center.y + 6), 3, icon_col, 12, 2.0f);
                draw_list->AddCircle(ImVec2(icon_center.x + 4, icon_center.y + 6), 3, icon_col, 12, 2.0f);
                draw_list->AddLine(ImVec2(icon_center.x - 4, icon_center.y - 3),
                                 ImVec2(icon_center.x - 4, icon_center.y + 3), icon_col, 2.0f);
                break;
                
            case ActivityBarItem::Debug:
                // Play/bug icon
                draw_list->AddTriangleFilled(
                    ImVec2(icon_center.x - 6, icon_center.y - 8),
                    ImVec2(icon_center.x - 6, icon_center.y + 8),
                    ImVec2(icon_center.x + 6, icon_center.y), icon_col);
                break;
                
            case ActivityBarItem::Extensions:
                // Grid/blocks icon
                draw_list->AddRectFilled(ImVec2(icon_center.x - 8, icon_center.y - 8),
                                       ImVec2(icon_center.x - 2, icon_center.y - 2), icon_col);
                draw_list->AddRectFilled(ImVec2(icon_center.x + 2, icon_center.y - 8),
                                       ImVec2(icon_center.x + 8, icon_center.y - 2), icon_col);
                draw_list->AddRectFilled(ImVec2(icon_center.x - 8, icon_center.y + 2),
                                       ImVec2(icon_center.x - 2, icon_center.y + 8), icon_col);
                draw_list->AddRectFilled(ImVec2(icon_center.x + 2, icon_center.y + 2),
                                       ImVec2(icon_center.x + 8, icon_center.y + 8), icon_col);
                break;
        }

        // Handle click
        if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            // Toggle: if clicking the same item, hide sidebar; otherwise show and switch
            if (g_selected == item.id && g_sidebar_visible)
            {
                g_sidebar_visible = false;  // Hide sidebar
            }
            else
            {
                g_selected = item.id;
                g_sidebar_visible = true;   // Show sidebar with new selection
            }
            result.selected_item = g_selected;
            result.item_clicked = true;
        }

        // Tooltip
        if (is_hovered)
        {
            ImGui::SetTooltip("%s", item.tooltip);
        }

        item_y += item_size + 4;  // Move to next item
    }

    ImGui::End();
    ImGui::PopStyleVar(4);

    // Right border
    ImDrawList* fg = ImGui::GetForegroundDrawList();
    fg->AddLine(ImVec2(width - 1, bar_start_y), 
                ImVec2(width - 1, bar_end_y), 
                IM_COL32(62, 62, 66, 255), 1.0f);

    result.selected_item = g_selected;
    result.sidebar_visible = g_sidebar_visible;
    return result;
}

// Toggle the primary sidebar visibility
void TogglePrimarySidebar()
{
    g_sidebar_visible = !g_sidebar_visible;
}
