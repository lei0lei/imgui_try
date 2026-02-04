
#include "primary_sidebar_ui.h"
#include "imgui.h"
#include "../workbench/workbench_config.h"

PrimarySidebarResult DrawPrimarySidebarUI(float activity_bar_w,
                                          float title_h,
                                          float status_bar_h,
                                          float width,
                                          PrimarySidebarService& service,
                                          ViewRegistry& view_registry,
                                          SceneType mode,
                                          EditorTab* active_tab,
                                          ActivityBarItem active_item)
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
    const ViewDefinition* active_view = view_registry.GetActiveView(mode, ViewContainer::PrimarySidebar);
    ViewDefinition fallback = UI::GetDefaultPrimaryView(active_item);
    const ViewDefinition* view_to_render = (active_view && active_view->renderer) ? active_view : &fallback;

    if (!view_to_render->renderer) {
        ImGui::TextColored(colors.primary_sidebar_text_dim, "NO VIEW");
        ImGui::Separator();
        ImGui::Text("No primary sidebar view for this activity.");
    } else {
        ImVec2 content_min = ImGui::GetCursorScreenPos();
        ImVec2 content_max = ImVec2(content_min.x + ImGui::GetContentRegionAvail().x,
                                    content_min.y + ImGui::GetContentRegionAvail().y);
        view_to_render->renderer(content_min, content_max, active_tab);
    }
    ImGui::End();
    ImGui::PopStyleVar(3);
    ImDrawList* _bg = ImGui::GetBackgroundDrawList();
    _bg->AddLine(ImVec2(sidebar_x + width - sizes.sidebar_border_thickness, sidebar_start_y), ImVec2(sidebar_x + width - sizes.sidebar_border_thickness, sidebar_end_y), ImGui::GetColorU32(colors.primary_sidebar_border), sizes.sidebar_border_thickness);
    result.is_visible = service.IsVisible();
    return result;
}
