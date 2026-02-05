/**
 * @file panel_ui.cpp
 * @brief 底部面板组件的UI渲染
 * @author Your Name
 * @date 2026-02-05
 */

#include "panel_ui.h"
#include <imgui.h>
#include "../workbench/workbench_config.h"

PanelResult DrawPanelUI(float left_offset,
                        float right_offset,
                        float status_bar_h,
                        float panel_h,
                        const PanelViewModel& view_model,
                        EditorTab* active_tab) {
    PanelResult result{};
    ImGuiIO& io = ImGui::GetIO();

    // VS Code panel colors
    const WorkbenchTheme& theme = GetWorkbenchTheme();
    const WorkbenchThemeColors& colors = theme.colors;
    const WorkbenchThemeSizes& sizes = theme.sizes;

    ImVec4 bg_color = colors.panel_bg;
    ImVec4 tab_bg = colors.panel_tab_bg;
    ImVec4 tab_active = colors.panel_tab_active;
    ImVec4 text_color = colors.panel_text;

    float panel_x = left_offset;
    float panel_y = io.DisplaySize.y - status_bar_h - panel_h;
    float panel_width = io.DisplaySize.x - left_offset - right_offset;

    // Background
    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    bg->AddRectFilled(ImVec2(panel_x, panel_y), 
                     ImVec2(panel_x + panel_width, io.DisplaySize.y - status_bar_h), 
                     ImGui::GetColorU32(bg_color));

    // Panel window
    ImGui::SetNextWindowPos(ImVec2(panel_x, panel_y));
    ImGui::SetNextWindowSize(ImVec2(panel_width, panel_h));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                              ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
                              ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Panel", nullptr, flags);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 window_pos = ImGui::GetWindowPos();

    const std::vector<ViewDefinition>* views = view_model.views;
    float tab_height = sizes.panel_tab_height;
    float tab_width = sizes.panel_tab_width;
    bool show_tabs = view_model.show_tabs && views && views->size() > 1;

    if (views && !views->empty()) {
        int active_index = view_model.active_index;
        if (show_tabs) {
            for (int i = 0; i < static_cast<int>(views->size()); ++i)
            {
                ImVec2 tab_min = ImVec2(window_pos.x + i * tab_width, window_pos.y);
                ImVec2 tab_max = ImVec2(tab_min.x + tab_width, tab_min.y + tab_height);
                ImVec2 mouse_pos = ImGui::GetMousePos();
                bool is_hovered = (mouse_pos.x >= tab_min.x && mouse_pos.x <= tab_max.x &&
                                  mouse_pos.y >= tab_min.y && mouse_pos.y <= tab_max.y);
                bool is_active = (active_index == i);

                ImU32 tab_color = is_active ? ImGui::GetColorU32(tab_active) : ImGui::GetColorU32(tab_bg);
                if (!is_active && is_hovered)
                {
                    tab_color = ImGui::GetColorU32(colors.panel_tab_hover);
                }
                draw_list->AddRectFilled(tab_min, tab_max, tab_color);

                ImVec2 text_size = ImGui::CalcTextSize((*views)[i].title.c_str());
                ImVec2 text_pos = ImVec2(tab_min.x + (tab_width - text_size.x) * 0.5f,
                                        tab_min.y + (tab_height - text_size.y) * 0.5f);
                draw_list->AddText(text_pos, ImGui::GetColorU32(text_color), (*views)[i].title.c_str());

                if (is_active)
                {
                    draw_list->AddRectFilled(ImVec2(tab_min.x, tab_min.y),
                                           ImVec2(tab_max.x, tab_min.y + sizes.panel_active_indicator_h),
                                           ImGui::GetColorU32(colors.panel_active_indicator));
                }

                if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    if (view_model.on_select_tab)
                        view_model.on_select_tab(i);
                    active_index = i;
                }
            }
        }

        float content_y = show_tabs ? tab_height : 0.0f;
        ImGui::SetCursorPos(ImVec2(sizes.panel_content_padding_x, content_y + sizes.panel_content_padding_y));
        ImGui::BeginChild("PanelContent", ImVec2(panel_width - sizes.panel_content_padding_x * 2.0f, panel_h - content_y - sizes.panel_content_padding_y * 2.0f), false);
        if (!active_tab) {
            ImGui::Text("No editor open. Panel is idle.");
        } else {
            const ViewDefinition* active_view = view_model.active_view;
            if (active_view && active_view->renderer) {
                ImVec2 content_min = ImGui::GetCursorScreenPos();
                ImVec2 content_max = ImVec2(content_min.x + ImGui::GetContentRegionAvail().x,
                                            content_min.y + ImGui::GetContentRegionAvail().y);
                active_view->renderer(content_min, content_max, active_tab);
            } else {
                ImGui::Text("No panel view for this editor.");
            }
        }
        ImGui::EndChild();
    } else {
        ImGui::SetCursorPos(ImVec2(sizes.panel_content_padding_x, tab_height + sizes.panel_content_padding_y));
        ImGui::Text("No panel views for this editor.");
    }

    ImGui::End();
    ImGui::PopStyleVar(4);

    // Top border
    ImDrawList* _fg = ImGui::GetBackgroundDrawList();
    _fg->AddLine(ImVec2(panel_x, panel_y),
                ImVec2(panel_x + panel_width, panel_y),
                ImGui::GetColorU32(colors.panel_border), sizes.panel_border_thickness);

    return result;
}
