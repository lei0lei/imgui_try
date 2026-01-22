#include "status_bar.h"
#include "imgui.h"
#include <stdio.h>

StatusBarResult DrawStatusBar(float status_bar_h, float title_h)
{
    StatusBarResult result{};
    ImGuiIO& io = ImGui::GetIO();

    // VS Code style colors
    ImVec4 bg_color = ImVec4(0.117f, 0.117f, 0.117f, 1.0f);      // #1E1E1E darker than title
    ImVec4 text_color = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);       // #CCCCCC

    // Background
    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    float status_bar_y = io.DisplaySize.y - status_bar_h;
    bg->AddRectFilled(ImVec2(0, status_bar_y), ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGui::GetColorU32(bg_color));

    // Status bar window
    ImGui::SetNextWindowPos(ImVec2(0, status_bar_y));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, status_bar_h));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                              ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
                              ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav;

    ImGui::Begin("StatusBar", nullptr, flags);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 window_pos = ImGui::GetWindowPos();

    // FPS display
    char fps_text[32];
    snprintf(fps_text, sizeof(fps_text), "%.1f FPS", io.Framerate);
    
    ImVec2 text_size = ImGui::CalcTextSize(fps_text);
    ImVec2 text_pos = ImVec2(window_pos.x + 12, 
                              window_pos.y + (status_bar_h - ImGui::GetTextLineHeight()) * 0.5f);
    
    draw_list->AddText(text_pos, ImGui::GetColorU32(text_color), fps_text);

    ImGui::End();
    ImGui::PopStyleVar(3);

    // Draw border line at top of status bar
    ImDrawList* fg = ImGui::GetForegroundDrawList();
    ImU32 border_color = IM_COL32(62, 62, 66, 255);  // VS Code style #3E3E42
    ImVec2 line_start = ImVec2(0, status_bar_y - 1);
    ImVec2 line_end = ImVec2(io.DisplaySize.x, status_bar_y - 1);
    fg->AddLine(line_start, line_end, border_color, 2.0f);
    
    // Draw left and right borders (complete window frame)
    fg->AddLine(ImVec2(0, title_h), 
                ImVec2(0, io.DisplaySize.y - status_bar_h), 
                border_color, 2.0f);
    fg->AddLine(ImVec2(io.DisplaySize.x - 1, title_h), 
                ImVec2(io.DisplaySize.x - 1, io.DisplaySize.y - status_bar_h), 
                border_color, 2.0f);

    return result;
}
