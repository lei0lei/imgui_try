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
    if (view_model.progress >= 0.0f && view_model.progress <= 1.0f) {
        ImGui::SameLine();
        ImGui::SetCursorPosX(win_size.x - sizes.status_bar_progress_right_margin);
        ImGui::ProgressBar(view_model.progress, ImVec2(sizes.status_bar_progress_w, sizes.status_bar_progress_h));
    }
    ImGui::End();
    ImGui::PopStyleVar(3);
}
