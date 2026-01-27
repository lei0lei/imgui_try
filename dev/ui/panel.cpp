#include "panel.h"
#include "imgui.h"

PanelResult DrawPanel(float left_offset, float right_offset, float status_bar_h, float height)
{
    PanelResult result{};
    result.is_visible = true;
    result.height = height;
    
    ImGuiIO& io = ImGui::GetIO();

    // VS Code panel colors
    ImVec4 bg_color = ImVec4(0.118f, 0.118f, 0.118f, 1.0f);  // #1E1E1E
    ImVec4 tab_bg = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    ImVec4 tab_active = ImVec4(0.118f, 0.118f, 0.118f, 1.0f);
    ImVec4 text_color = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);

    float panel_x = left_offset;
    float panel_y = io.DisplaySize.y - status_bar_h - height;
    float panel_width = io.DisplaySize.x - left_offset - right_offset;

    // Background
    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    // Restore original background fill logic
    bg->AddRectFilled(ImVec2(panel_x, panel_y), 
                     ImVec2(panel_x + panel_width, io.DisplaySize.y - status_bar_h), 
                     ImGui::GetColorU32(bg_color));

    // Panel window
    ImGui::SetNextWindowPos(ImVec2(panel_x, panel_y));
    ImGui::SetNextWindowSize(ImVec2(panel_width, height));
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

    // Tab bar
    const char* tabs[] = { "PROBLEMS", "OUTPUT", "DEBUG", "TERMINAL" };
    const int tab_count = 4;
    static int active_tab = 3;  // Default to TERMINAL
    
    float tab_height = 35.0f;
    float tab_width = 120.0f;
    
    // Draw tabs
    for (int i = 0; i < tab_count; i++)
    {
        ImVec2 tab_min = ImVec2(window_pos.x + i * tab_width, window_pos.y);
        ImVec2 tab_max = ImVec2(tab_min.x + tab_width, tab_min.y + tab_height);
        ImVec2 mouse_pos = ImGui::GetMousePos();
        
        bool is_hovered = (mouse_pos.x >= tab_min.x && mouse_pos.x <= tab_max.x &&
                          mouse_pos.y >= tab_min.y && mouse_pos.y <= tab_max.y);
        bool is_active = (active_tab == i);

        // Tab background
        ImU32 tab_color = is_active ? ImGui::GetColorU32(tab_active) : ImGui::GetColorU32(tab_bg);
        if (!is_active && is_hovered)
        {
            tab_color = ImGui::GetColorU32(ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
        }
        draw_list->AddRectFilled(tab_min, tab_max, tab_color);

        // Tab text
        ImVec2 text_size = ImGui::CalcTextSize(tabs[i]);
        ImVec2 text_pos = ImVec2(tab_min.x + (tab_width - text_size.x) * 0.5f,
                                tab_min.y + (tab_height - text_size.y) * 0.5f);
        draw_list->AddText(text_pos, ImGui::GetColorU32(text_color), tabs[i]);

        // Active indicator (top border)
        if (is_active)
        {
            draw_list->AddRectFilled(ImVec2(tab_min.x, tab_min.y), 
                                   ImVec2(tab_max.x, tab_min.y + 2), 
                                   IM_COL32(0, 122, 204, 255));
        }

        // Handle click
        if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            active_tab = i;
        }
    }

    // Content area
    ImGui::SetCursorPos(ImVec2(8, tab_height + 8));
    ImGui::BeginChild("PanelContent", ImVec2(panel_width - 16, height - tab_height - 16), false);
    
    switch (active_tab)
    {
        case 0: // PROBLEMS
            ImGui::Text("No problems detected");
            break;
        case 1: // OUTPUT
            ImGui::Text("Output channel:");
            ImGui::Text("Build completed successfully");
            break;
        case 2: // DEBUG CONSOLE
            ImGui::Text("Debug console");
            break;
        case 3: // TERMINAL
            ImGui::TextColored(ImVec4(0.5f, 0.8f, 0.5f, 1.0f), "$ ");
            ImGui::SameLine();
            ImGui::Text("Ready");
            break;
    }
    
    ImGui::EndChild();

    ImGui::End();
    ImGui::PopStyleVar(4);

    // Top border
    ImDrawList* _fg = ImGui::GetBackgroundDrawList();
    _fg->AddLine(ImVec2(panel_x, panel_y), 
                ImVec2(panel_x + panel_width, panel_y), 
                IM_COL32(62, 62, 66, 255), 1.0f);

    return result;
}
