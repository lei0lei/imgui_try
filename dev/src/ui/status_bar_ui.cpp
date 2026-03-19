/**
 * @file status_bar_ui.cpp
 * @brief 底部状态栏的UI渲染
 * @author Your Name
 * @date 2026-02-05
 */

#include "status_bar_ui.h"
#include "imgui.h"
#include "../workbench/workbench_config.h"

void RenderStatusBarUI(SDL_Window* window, const StatusBarViewModel& view_model, float status_bar_h, float title_h) {
    ImGuiIO& io = ImGui::GetIO();
    const WorkbenchTheme& theme = GetWorkbenchTheme();
    const WorkbenchThemeColors& colors = theme.colors;
    const WorkbenchThemeSizes& sizes = theme.sizes;

    ImVec4 bg_color = colors.status_bar_bg;
    ImVec4 text_color = colors.status_bar_text;

    ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y - status_bar_h));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, status_bar_h));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(sizes.status_bar_padding_x, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav;
    ImGui::Begin("StatusBar", nullptr, flags);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 win_pos = ImGui::GetWindowPos();
    ImVec2 win_size = ImGui::GetWindowSize();
    draw_list->AddRectFilled(win_pos, ImVec2(win_pos.x + win_size.x, win_pos.y + win_size.y), ImGui::GetColorU32(bg_color));
    ImGui::PushStyleColor(ImGuiCol_Text, text_color);
    ImGui::SetCursorPosY((status_bar_h - ImGui::GetTextLineHeight()) * 0.5f);
    ImGui::TextUnformatted(view_model.message.c_str());
    ImGui::PopStyleColor();

    const float right_region_w = 420.0f;
    const float separator_x = win_size.x - right_region_w;
    draw_list->AddLine(
        ImVec2(win_pos.x + separator_x, win_pos.y + 2.0f),
        ImVec2(win_pos.x + separator_x, win_pos.y + win_size.y - 2.0f),
        ImGui::GetColorU32(colors.panel_border),
        1.0f);

    if (view_model.progress >= 0.0f && view_model.progress <= 1.0f) {
        ImGui::SameLine();
        ImGui::SetCursorPosX(separator_x - sizes.status_bar_progress_right_margin);
        ImGui::ProgressBar(view_model.progress, ImVec2(sizes.status_bar_progress_w, sizes.status_bar_progress_h));
    }

    if (view_model.scene_status_bar_extension && *view_model.scene_status_bar_extension) {
        const ImVec2 region_min = ImVec2(win_pos.x + separator_x + 8.0f, win_pos.y);
        const ImVec2 region_max = ImVec2(win_pos.x + win_size.x - 6.0f, win_pos.y + win_size.y);
        ImGui::PushClipRect(region_min, region_max, true);
        ImGui::SetCursorScreenPos(ImVec2(region_min.x, region_min.y + 1.0f));

        StatusBarExtensionContext ctx{};
        ctx.region = StatusBarExtensionRegion::Right;
        ctx.region_min = region_min;
        ctx.region_max = region_max;
        ctx.status_bar_h = status_bar_h;
        ctx.active_tab = view_model.active_tab;
        ctx.trigger_scene_action = [
            active_tab = view_model.active_tab,
            scene_status_bar_action_handler = view_model.scene_status_bar_action_handler
        ](const std::string& action_id) {
            if (!active_tab || !scene_status_bar_action_handler || !(*scene_status_bar_action_handler) || action_id.empty()) {
                return;
            }
            (*scene_status_bar_action_handler)(*active_tab, action_id);
        };
        (*view_model.scene_status_bar_extension)(ctx);
        ImGui::PopClipRect();
    }

    ImGui::End();
    ImGui::PopStyleVar(3);
}
