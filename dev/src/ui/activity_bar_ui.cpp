/**
 * @file activity_bar_ui.cpp
 * @brief 活动栏组件的UI渲染
 * @author Your Name
 * @date 2026-02-05
 */

#include "activity_bar_ui.h"
#include "imgui.h"
#include "../workbench/workbench_config.h"

ActivityBarResult DrawActivityBarUI(float title_h, float status_bar_h, float width, const ActivityBarViewModel& view_model)
{
    ActivityBarResult result{};
    ImGuiIO& io = ImGui::GetIO();

    const WorkbenchTheme& theme = GetWorkbenchTheme();
    const WorkbenchThemeColors& colors = theme.colors;
    const WorkbenchThemeSizes& sizes = theme.sizes;

    ImVec4 bg_color = colors.activity_bar_bg;
    ImVec4 hover_color = colors.activity_bar_hover;
    ImVec4 active_color = colors.activity_bar_active;
    ImVec4 icon_color = colors.activity_bar_icon;

    // Background
    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    float bar_start_y = title_h;
    float bar_end_y = io.DisplaySize.y - status_bar_h;
    bg->AddRectFilled(ImVec2(0, bar_start_y), ImVec2(width, bar_end_y), ImGui::GetColorU32(bg_color));

    ImGui::SetNextWindowPos(ImVec2(0, bar_start_y));
    ImGui::SetNextWindowSize(ImVec2(width, bar_end_y - bar_start_y));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, sizes.activity_bar_padding_y));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, sizes.activity_bar_item_spacing));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                              ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
                              ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav;

    ImGui::Begin("ActivityBar", nullptr, flags);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 window_pos = ImGui::GetWindowPos();

    struct ActivityItem {
        ActivityBarItem id;
        const char* tooltip;
    };
    ActivityItem items[] = {
        { ActivityBarItem::Explorer, "Explorer" },
        { ActivityBarItem::Search, "Search" },
        { ActivityBarItem::NodeEditor, "Node editor" },
        { ActivityBarItem::Debug, "Debug" },
        { ActivityBarItem::Extensions, "Extensions" }
    };
    float item_size = width;
    float item_y = window_pos.y + sizes.activity_bar_padding_y;
    int selected = view_model.selected_index;
    for (int i = 0; i < 5; ++i) {
        ImVec2 item_min = ImVec2(window_pos.x, item_y);
        ImVec2 item_max = ImVec2(window_pos.x + width, item_y + item_size);
        ImVec2 mouse_pos = ImGui::GetMousePos();
        bool is_hovered = (mouse_pos.x >= item_min.x && mouse_pos.x <= item_max.x && mouse_pos.y >= item_min.y && mouse_pos.y <= item_max.y);
        bool is_selected = (selected == i + 1); // ActivityBarItem::Explorer == 1
        if (is_selected) {
            draw_list->AddRectFilled(item_min, item_max, ImGui::GetColorU32(active_color));
            draw_list->AddRectFilled(ImVec2(item_min.x, item_min.y), ImVec2(item_min.x + sizes.activity_bar_active_indicator_w, item_max.y), ImGui::GetColorU32(colors.activity_bar_indicator));
        } else if (is_hovered) {
            draw_list->AddRectFilled(item_min, item_max, ImGui::GetColorU32(hover_color));
        }
        ImVec2 icon_center = ImVec2(item_min.x + width * 0.5f, item_min.y + item_size * 0.5f);
        float icon_size = sizes.activity_bar_icon_size;
        ImU32 icon_col = ImGui::GetColorU32(icon_color);
        switch (items[i].id) {
            case ActivityBarItem::Explorer:
                draw_list->AddRect(ImVec2(icon_center.x - icon_size * 0.4f, icon_center.y - icon_size * 0.5f),
                                 ImVec2(icon_center.x + icon_size * 0.4f, icon_center.y + icon_size * 0.5f), icon_col, 0, 0, sizes.activity_bar_icon_stroke);
                draw_list->AddLine(ImVec2(icon_center.x - icon_size * 0.2f, icon_center.y - icon_size * 0.5f),
                                 ImVec2(icon_center.x - icon_size * 0.2f, icon_center.y - icon_size * 0.3f), icon_col, sizes.activity_bar_icon_stroke);
                break;
            case ActivityBarItem::Search:
                draw_list->AddCircle(ImVec2(icon_center.x - icon_size * 0.1f, icon_center.y - icon_size * 0.1f), icon_size * 0.3f, icon_col, 12, sizes.activity_bar_icon_stroke);
                draw_list->AddLine(ImVec2(icon_center.x + icon_size * 0.15f, icon_center.y + icon_size * 0.15f),
                                 ImVec2(icon_center.x + icon_size * 0.4f, icon_center.y + icon_size * 0.4f), icon_col, sizes.activity_bar_icon_stroke);
                break;
            case ActivityBarItem::NodeEditor:
                draw_list->AddCircle(ImVec2(icon_center.x - icon_size * 0.2f, icon_center.y - icon_size * 0.3f), icon_size * 0.15f, icon_col, 12, sizes.activity_bar_icon_stroke);
                draw_list->AddCircle(ImVec2(icon_center.x - icon_size * 0.2f, icon_center.y + icon_size * 0.3f), icon_size * 0.15f, icon_col, 12, sizes.activity_bar_icon_stroke);
                draw_list->AddCircle(ImVec2(icon_center.x + icon_size * 0.2f, icon_center.y + icon_size * 0.3f), icon_size * 0.15f, icon_col, 12, sizes.activity_bar_icon_stroke);
                draw_list->AddLine(ImVec2(icon_center.x - icon_size * 0.2f, icon_center.y - icon_size * 0.15f),
                                 ImVec2(icon_center.x - icon_size * 0.2f, icon_center.y + icon_size * 0.15f), icon_col, sizes.activity_bar_icon_stroke);
                break;
            case ActivityBarItem::Debug:
                draw_list->AddTriangleFilled(
                    ImVec2(icon_center.x - icon_size * 0.3f, icon_center.y - icon_size * 0.4f),
                    ImVec2(icon_center.x - icon_size * 0.3f, icon_center.y + icon_size * 0.4f),
                    ImVec2(icon_center.x + icon_size * 0.3f, icon_center.y), icon_col);
                break;
            case ActivityBarItem::Extensions:
                draw_list->AddRectFilled(ImVec2(icon_center.x - icon_size * 0.4f, icon_center.y - icon_size * 0.4f),
                                       ImVec2(icon_center.x - icon_size * 0.1f, icon_center.y - icon_size * 0.1f), icon_col);
                draw_list->AddRectFilled(ImVec2(icon_center.x + icon_size * 0.1f, icon_center.y - icon_size * 0.4f),
                                       ImVec2(icon_center.x + icon_size * 0.4f, icon_center.y - icon_size * 0.1f), icon_col);
                draw_list->AddRectFilled(ImVec2(icon_center.x - icon_size * 0.4f, icon_center.y + icon_size * 0.1f),
                                       ImVec2(icon_center.x - icon_size * 0.1f, icon_center.y + icon_size * 0.4f), icon_col);
                draw_list->AddRectFilled(ImVec2(icon_center.x + icon_size * 0.1f, icon_center.y + icon_size * 0.1f),
                                       ImVec2(icon_center.x + icon_size * 0.4f, icon_center.y + icon_size * 0.4f), icon_col);
                break;
        }
        if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            selected = i + 1;
            if (view_model.on_select)
                view_model.on_select(i + 1);
            result.selected_item = items[i].id;
            result.item_clicked = true;
        }
        if (is_hovered) {
            ImGui::SetTooltip("%s", items[i].tooltip);
        }
        item_y += item_size + sizes.activity_bar_item_spacing;
    }
    ImGui::End();
    ImGui::PopStyleVar(4);
    // 右边框
    ImDrawList* _bg = ImGui::GetBackgroundDrawList();
    _bg->AddLine(ImVec2(width - sizes.activity_bar_border_thickness, bar_start_y), ImVec2(width - sizes.activity_bar_border_thickness, bar_end_y), ImGui::GetColorU32(colors.activity_bar_border), sizes.activity_bar_border_thickness);
    result.selected_item = (ActivityBarItem)selected;
    return result;
}
