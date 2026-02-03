
#include "primary_sidebar_ui.h"
#include "imgui.h"
#include "../workbench/workbench_config.h"

PrimarySidebarResult DrawPrimarySidebarUI(ActivityBarItem active_item,
                                          float activity_bar_w,
                                          float title_h,
                                          float status_bar_h,
                                          float width,
                                          PrimarySidebarService& service)
{
    PrimarySidebarResult result{};
    ImGuiIO& io = ImGui::GetIO();
    const WorkbenchTheme& theme = GetWorkbenchTheme();
    const WorkbenchThemeColors& colors = theme.colors;
    const WorkbenchThemeSizes& sizes = theme.sizes;

    ImVec4 bg_color = colors.primary_sidebar_bg;
    float sidebar_x = activity_bar_w;
    float sidebar_start_y = title_h;
    float sidebar_end_y = io.DisplaySize.y - status_bar_h;
    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    bg->AddRectFilled(ImVec2(sidebar_x, sidebar_start_y), ImVec2(sidebar_x + width, sidebar_end_y), ImGui::GetColorU32(bg_color));
    ImGui::SetNextWindowPos(ImVec2(sidebar_x, sidebar_start_y));
    ImGui::SetNextWindowSize(ImVec2(width, sidebar_end_y - sidebar_start_y));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(sizes.sidebar_padding_x, sizes.sidebar_padding_y));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                              ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
                              ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::Begin("PrimarySidebar", nullptr, flags);
    // Use active_item parameter for content, matching original logic
    switch (active_item)
    {
        case ActivityBarItem::Explorer:
            ImGui::TextColored(colors.primary_sidebar_text_dim, "EXPLORER");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("No folder opened");
            ImGui::Spacing();
            if (ImGui::Button("Open Folder")) {
                // TODO: Open folder dialog
            }
            break;
        case ActivityBarItem::Search:
            ImGui::TextColored(colors.primary_sidebar_text_dim, "SEARCH");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("Search across files");
            ImGui::Spacing();
            {
                static char search_buf[256] = "";
                ImGui::InputTextWithHint("##search", "Search...", search_buf, sizeof(search_buf));
            }
            break;
        case ActivityBarItem::NodeEditor:
            ImGui::TextColored(colors.primary_sidebar_text_dim, "NODE EDITOR");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("No Node editor providers");
            break;
        case ActivityBarItem::Debug:
            ImGui::TextColored(colors.primary_sidebar_text_dim, "RUN AND DEBUG");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("No configurations");
            break;
        case ActivityBarItem::Extensions:
            ImGui::TextColored(colors.primary_sidebar_text_dim, "EXTENSIONS");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("Extension marketplace");
            break;
        default:
            break;
    }
    ImGui::End();
    ImGui::PopStyleVar(3);
    ImDrawList* _bg = ImGui::GetBackgroundDrawList();
    _bg->AddLine(ImVec2(sidebar_x + width - sizes.sidebar_border_thickness, sidebar_start_y), ImVec2(sidebar_x + width - sizes.sidebar_border_thickness, sidebar_end_y), ImGui::GetColorU32(colors.primary_sidebar_border), sizes.sidebar_border_thickness);
    result.is_visible = service.IsVisible();
    return result;
}
