#include "primary_sidebar.h"
#include "imgui.h"

PrimarySidebarResult DrawPrimarySidebar(ActivityBarItem active_item, 
                                        float activity_bar_w, 
                                        float title_h, 
                                        float status_bar_h, 
                                        float width)
{
    PrimarySidebarResult result{};
    ImGuiIO& io = ImGui::GetIO();

    // VS Code sidebar colors
    ImVec4 bg_color = ImVec4(0.149f, 0.149f, 0.149f, 1.0f);  // #252525
    ImVec4 text_color = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
    ImVec4 header_color = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    float sidebar_x = activity_bar_w;
    float sidebar_start_y = title_h;
    float sidebar_end_y = io.DisplaySize.y - status_bar_h;

    // Background
    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    bg->AddRectFilled(ImVec2(sidebar_x, sidebar_start_y), 
                     ImVec2(sidebar_x + width, sidebar_end_y), 
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

    ImGui::Begin("PrimarySidebar", nullptr, flags);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Header based on active item
    const char* header_text = "SIDEBAR";
    switch (active_item)
    {
        case ActivityBarItem::Explorer:
            header_text = "EXPLORER";
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "EXPLORER");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("No folder opened");
            ImGui::Spacing();
            if (ImGui::Button("Open Folder"))
            {
                // TODO: Open folder dialog
            }
            break;
            
        case ActivityBarItem::Search:
            header_text = "SEARCH";
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "SEARCH");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("Search across files");
            ImGui::Spacing();
            {
                static char search_buf[256] = "";
                ImGui::InputTextWithHint("##search", "Search...", search_buf, sizeof(search_buf));
            }
            break;
            
        case ActivityBarItem::SourceControl:
            header_text = "SOURCE CONTROL";
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "SOURCE CONTROL");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("No source control providers");
            break;
            
        case ActivityBarItem::Debug:
            header_text = "RUN AND DEBUG";
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "RUN AND DEBUG");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("No configurations");
            break;
            
        case ActivityBarItem::Extensions:
            header_text = "EXTENSIONS";
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "EXTENSIONS");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("Extension marketplace");
            break;
            
        default:
            break;
    }

    ImGui::End();
    ImGui::PopStyleVar(3);

    // Right border
    ImDrawList* fg = ImGui::GetForegroundDrawList();
    fg->AddLine(ImVec2(sidebar_x + width - 1, sidebar_start_y), 
                ImVec2(sidebar_x + width - 1, sidebar_end_y), 
                IM_COL32(62, 62, 66, 255), 1.0f);

    return result;
}
