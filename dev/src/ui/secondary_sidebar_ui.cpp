#include "secondary_sidebar_ui.h"
#include "imgui.h"
#include "../workbench/workbench_config.h"

SecondarySidebarResult DrawSecondarySidebarUI(float title_h,
                                              float status_bar_h,
                                              float panel_h,
                                              float width,
                                              SecondarySidebarService& service,
                                              ViewRegistry& view_registry,
                                              SceneType mode,
                                              EditorTab* active_tab)
{
    SecondarySidebarResult result{};
    if (!service.IsVisible())
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
                              ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
                              ImGuiWindowFlags_NoBringToFrontOnFocus;

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

    const ViewDefinition* active_view = view_registry.GetActiveView(mode, ViewContainer::SecondarySidebar);
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
        ImVec2 content_min = ImGui::GetCursorScreenPos();
        ImVec2 content_max = ImVec2(content_min.x + ImGui::GetContentRegionAvail().x,
                                    content_min.y + ImGui::GetContentRegionAvail().y);
        active_view->renderer(content_min, content_max, active_tab);
    }

    ImGui::End();
    ImGui::PopStyleVar(3);
    ImDrawList* _bg = ImGui::GetBackgroundDrawList();
    _bg->AddLine(ImVec2(sidebar_x, sidebar_start_y),
                ImVec2(sidebar_x, sidebar_end_y),
                ImGui::GetColorU32(colors.secondary_sidebar_border), sizes.sidebar_border_thickness);
    return result;
}
