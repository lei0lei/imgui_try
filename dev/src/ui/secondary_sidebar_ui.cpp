/**
 * @file secondary_sidebar_ui.cpp
 * @brief 副侧边栏组件的UI渲染
 * @author Your Name
 * @date 2026-02-05
 */

#include "secondary_sidebar_ui.h"
#include "imgui.h"
#include "../workbench/workbench_config.h"

SecondarySidebarResult DrawSecondarySidebarUI(float title_h,
                                              float status_bar_h,
                                              float panel_h,
                                              float width,
                                              const SecondarySidebarViewModel& view_model,
                                              EditorTab* active_tab)
{
    SecondarySidebarResult result{};
    if (!view_model.is_visible)
        return result;

    ImGuiIO& io = ImGui::GetIO();
    const WorkbenchTheme& theme = GetWorkbenchTheme();
    const WorkbenchThemeColors& colors = theme.colors;
    const WorkbenchThemeSizes& sizes = theme.sizes;

    ImVec4 bg_color = colors.secondary_sidebar_bg;
    ImVec4 text_color = colors.secondary_sidebar_text;
    ImVec4 hover_color = colors.secondary_sidebar_hover;

    float sidebar_x = io.DisplaySize.x - width;
    float sidebar_start_y = title_h;
    float sidebar_end_y = io.DisplaySize.y - status_bar_h - panel_h;

    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    bg->AddRectFilled(ImVec2(sidebar_x, sidebar_start_y),
                      ImVec2(io.DisplaySize.x, io.DisplaySize.y - status_bar_h),
                      ImGui::GetColorU32(bg_color));

    ImGui::SetNextWindowPos(ImVec2(sidebar_x, sidebar_start_y));
    ImGui::SetNextWindowSize(ImVec2(width, sidebar_end_y - sidebar_start_y));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(sizes.sidebar_padding_x, sizes.sidebar_padding_y));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                              ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground;

    ImGui::Begin("SecondarySidebar", nullptr, flags);

    // Draw close button at top-right
    {
        ImVec2 win_pos = ImGui::GetWindowPos();
        ImVec2 win_size = ImGui::GetWindowSize();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        float btn_size = sizes.secondary_close_btn_size;
        ImVec2 btn_min = ImVec2(win_pos.x + win_size.x - btn_size - sizes.secondary_close_btn_padding, win_pos.y + sizes.secondary_close_btn_padding);
        ImVec2 btn_max = ImVec2(btn_min.x + btn_size, btn_min.y + btn_size);
        ImVec2 mouse = ImGui::GetMousePos();
        bool hovered = (mouse.x >= btn_min.x && mouse.x <= btn_max.x && mouse.y >= btn_min.y && mouse.y <= btn_max.y);
        if (hovered)
            dl->AddRectFilled(btn_min, btn_max, ImGui::GetColorU32(hover_color), sizes.secondary_close_btn_rounding);
        dl->AddRect(btn_min, btn_max, ImGui::GetColorU32(colors.secondary_sidebar_close_border), sizes.secondary_close_btn_rounding, 0, sizes.secondary_close_btn_border_thickness);
        ImU32 xcol = ImGui::GetColorU32(ImVec4(colors.secondary_sidebar_close_icon.x, colors.secondary_sidebar_close_icon.y, colors.secondary_sidebar_close_icon.z, hovered ? 1.0f : 0.8f));
        float inset = sizes.secondary_close_btn_padding - 2.0f;
        dl->AddLine(ImVec2(btn_min.x + inset, btn_min.y + inset), ImVec2(btn_max.x - inset, btn_max.y - inset), xcol, sizes.secondary_close_icon_stroke);
        dl->AddLine(ImVec2(btn_min.x + inset, btn_max.y - inset), ImVec2(btn_max.x - inset, btn_min.y + inset), xcol, sizes.secondary_close_icon_stroke);
        if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            result.request_close = true;
    }

    const ViewDefinition* active_view = view_model.active_view;
    const std::vector<ViewDefinition>* views = view_model.views;
    int active_index = view_model.active_index;
    bool show_tabs = view_model.show_tabs && views && views->size() > 1;

    if (!active_tab) {
        ImGui::TextColored(text_color, "SECONDARY SIDEBAR");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextWrapped("No editor open. Select an editor to show its outline and properties.");
    } else if (!active_view || !active_view->renderer) {
        ImGui::TextColored(text_color, "NO VIEW");
        ImGui::Separator();
        ImGui::Text("No secondary sidebar view for this editor.");
    } else {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 window_pos = ImGui::GetWindowPos();

        float tab_height = sizes.panel_tab_height;
        float tab_width = sizes.panel_tab_width;

        if (show_tabs) {
            for (int i = 0; i < static_cast<int>(views->size()); ++i) {
                ImVec2 tab_min = ImVec2(window_pos.x + i * tab_width, window_pos.y);
                ImVec2 tab_max = ImVec2(tab_min.x + tab_width, tab_min.y + tab_height);
                ImVec2 mouse_pos = ImGui::GetMousePos();
                bool is_hovered = (mouse_pos.x >= tab_min.x && mouse_pos.x <= tab_max.x &&
                                   mouse_pos.y >= tab_min.y && mouse_pos.y <= tab_max.y);
                bool is_active = (active_index == i);

                ImU32 tab_color = is_active ? ImGui::GetColorU32(colors.panel_tab_active) : ImGui::GetColorU32(colors.panel_tab_bg);
                if (!is_active && is_hovered) {
                    tab_color = ImGui::GetColorU32(colors.panel_tab_hover);
                }
                draw_list->AddRectFilled(tab_min, tab_max, tab_color);

                ImVec2 text_size = ImGui::CalcTextSize((*views)[i].title.c_str());
                ImVec2 text_pos = ImVec2(tab_min.x + (tab_width - text_size.x) * 0.5f,
                                         tab_min.y + (tab_height - text_size.y) * 0.5f);
                draw_list->AddText(text_pos, ImGui::GetColorU32(colors.secondary_sidebar_text), (*views)[i].title.c_str());

                if (is_active) {
                    draw_list->AddRectFilled(ImVec2(tab_min.x, tab_min.y),
                                             ImVec2(tab_max.x, tab_min.y + sizes.panel_active_indicator_h),
                                             ImGui::GetColorU32(colors.panel_active_indicator));
                }

                if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    if (view_model.on_select_tab)
                        view_model.on_select_tab(i);
                    active_index = i;
                }
            }
        }

        float content_y = show_tabs ? tab_height : 0.0f;
        ImGui::SetCursorPos(ImVec2(sizes.panel_content_padding_x, content_y + sizes.panel_content_padding_y));
        ImGui::BeginChild("SecondarySidebarContent",
                          ImVec2(width - sizes.panel_content_padding_x * 2.0f,
                                 (sidebar_end_y - sidebar_start_y) - content_y - sizes.panel_content_padding_y * 2.0f),
                          false);

        ImVec2 content_min = ImGui::GetCursorScreenPos();
        ImVec2 content_max = ImVec2(content_min.x + ImGui::GetContentRegionAvail().x,
                                    content_min.y + ImGui::GetContentRegionAvail().y);
        active_view->renderer(content_min, content_max, active_tab);
        ImGui::EndChild();
    }

    ImGui::End();
    ImGui::PopStyleVar(3);
    ImDrawList* _bg = ImGui::GetBackgroundDrawList();
    _bg->AddLine(ImVec2(sidebar_x, sidebar_start_y),
                ImVec2(sidebar_x, sidebar_end_y),
                ImGui::GetColorU32(colors.secondary_sidebar_border), sizes.sidebar_border_thickness);
    return result;
}
