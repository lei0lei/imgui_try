#include "window_border.h"
#include "imgui.h"

void DrawWindowBorders(const WindowBorderConfig& config)
{
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* fg = ImGui::GetForegroundDrawList();
    ImU32 border_color = IM_COL32(62, 62, 66, 255);  // VS Code style #3E3E42
    
    // Left border
    fg->AddLine(ImVec2(0, config.title_h), 
                ImVec2(0, io.DisplaySize.y - config.status_bar_h), 
                border_color, 2.0f);
    
    // Right border
    fg->AddLine(ImVec2(io.DisplaySize.x - 1, config.title_h), 
                ImVec2(io.DisplaySize.x - 1, io.DisplaySize.y - config.status_bar_h), 
                border_color, 2.0f);
}
