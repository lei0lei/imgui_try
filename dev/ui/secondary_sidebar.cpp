#include "secondary_sidebar.h"
#include "imgui.h"

SecondarySidebarResult DrawSecondarySidebar(float title_h, 
                                            float status_bar_h, 
                                            float panel_h,
                                            float width)
{
    SecondarySidebarResult result{};
    
    ImGuiIO& io = ImGui::GetIO();

    // VS Code sidebar colors
    ImVec4 bg_color = ImVec4(0.149f, 0.149f, 0.149f, 1.0f);  // #252525
    ImVec4 text_color = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
    ImVec4 hover_color = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);

    float sidebar_x = io.DisplaySize.x - width;
    float sidebar_start_y = title_h;
    float sidebar_end_y = io.DisplaySize.y - status_bar_h - panel_h;

    // Background (slightly overlap borders to avoid 1px gaps)
    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    // Restore original background fill logic
    bg->AddRectFilled(ImVec2(sidebar_x, sidebar_start_y), 
                      ImVec2(io.DisplaySize.x, io.DisplaySize.y - status_bar_h), 
                      ImGui::GetColorU32(bg_color));

    // Sidebar window
    ImGui::SetNextWindowPos(ImVec2(sidebar_x, sidebar_start_y));
    ImGui::SetNextWindowSize(ImVec2(width, sidebar_end_y - sidebar_start_y));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                              ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
                              ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("SecondarySidebar", nullptr, flags);

    // Draw close button at top-right
    {
        ImVec2 win_pos = ImGui::GetWindowPos();
        ImVec2 win_size = ImGui::GetWindowSize();
        ImDrawList* dl = ImGui::GetWindowDrawList();

        float btn_size = 18.0f;
        ImVec2 btn_min = ImVec2(win_pos.x + win_size.x - btn_size - 6.0f, win_pos.y + 6.0f);
        ImVec2 btn_max = ImVec2(btn_min.x + btn_size, btn_min.y + btn_size);
        
        ImVec2 mouse = ImGui::GetMousePos();
        bool hovered = (mouse.x >= btn_min.x && mouse.x <= btn_max.x && mouse.y >= btn_min.y && mouse.y <= btn_max.y);
        if (hovered)
            dl->AddRectFilled(btn_min, btn_max, ImGui::GetColorU32(hover_color), 3.0f);
        dl->AddRect(btn_min, btn_max, IM_COL32(90,90,90,255), 3.0f, 0, 1.0f);
        // X icon
        ImU32 xcol = IM_COL32(220,220,220, hovered ? 255 : 200);
        dl->AddLine(ImVec2(btn_min.x + 4, btn_min.y + 4), ImVec2(btn_max.x - 4, btn_max.y - 4), xcol, 1.8f);
        dl->AddLine(ImVec2(btn_min.x + 4, btn_max.y - 4), ImVec2(btn_max.x - 4, btn_min.y + 4), xcol, 1.8f);
        
        if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            result.request_close = true;
    }

    // Header
    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "OUTLINE");
    ImGui::Separator();
    ImGui::Spacing();

    // Content
    ImGui::Text("No outline available");
    ImGui::Spacing();
    ImGui::TextWrapped("The outline view shows the symbol tree of the currently active editor.");

    ImGui::End();
    ImGui::PopStyleVar(3);

    // Left border
    ImDrawList* fg = ImGui::GetForegroundDrawList();
    fg->AddLine(ImVec2(sidebar_x, sidebar_start_y), 
                ImVec2(sidebar_x, sidebar_end_y), 
                IM_COL32(62, 62, 66, 255), 1.0f);

    return result;
}
