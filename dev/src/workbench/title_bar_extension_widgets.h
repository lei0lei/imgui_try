#pragma once

#include <functional>

#include "imgui.h"
#include "workbench_config.h"

inline bool DrawTitleBarIconButton(const char* id,
                                   float button_w,
                                   float button_h,
                                   bool active,
                                   const std::function<void(ImDrawList*, ImVec2, float, ImU32)>& draw_icon)
{
    const auto& colors = GetWorkbenchTheme().colors;
    const auto& sizes = GetWorkbenchTheme().sizes;

    ImGui::PushID(id);
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetColorU32(colors.title_bar_button_hover));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetColorU32(colors.title_bar_button_active));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

    const bool pressed = ImGui::Button("##btn", ImVec2(button_w, button_h));
    const bool hovered = ImGui::IsItemHovered();

    ImVec2 p0 = ImGui::GetItemRectMin();
    ImVec2 p1 = ImGui::GetItemRectMax();
    ImVec2 c = ImVec2((p0.x + p1.x) * 0.5f, (p0.y + p1.y) * 0.5f);
    const float half = sizes.title_layout_icon_size;

    float alpha = active ? (hovered ? 1.0f : 0.9f) : (hovered ? 0.7f : 0.5f);
    ImU32 col = ImGui::GetColorU32(ImVec4(colors.title_bar_layout_icon.x,
                                          colors.title_bar_layout_icon.y,
                                          colors.title_bar_layout_icon.z,
                                          alpha));

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_icon(draw_list, c, half, col);

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);
    ImGui::PopID();

    return pressed;
}
