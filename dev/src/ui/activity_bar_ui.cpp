/**
 * @file activity_bar_ui.cpp
 * @brief 活动栏组件的UI渲染
 * @author Your Name
 * @date 2026-02-05
 */

#include "activity_bar_ui.h"
#include "imgui.h"
#include "icon_texture_manager.h"
#include "../workbench/workbench_config.h"
#include <SDL3/SDL.h>
#include <cmath>

static ImTextureID g_activity_bar_icon_textures[7] = {
    (ImTextureID)0,
    (ImTextureID)0,
    (ImTextureID)0,
    (ImTextureID)0,
    (ImTextureID)0,
    (ImTextureID)0,
    (ImTextureID)0
};
static ImTextureID g_activity_bar_settings_texture = (ImTextureID)0;

static int ToActivityBarIconIndex(ActivityBarItem item)
{
    const int index = static_cast<int>(item);
    if (index <= 0 || index >= 7)
        return 0;
    return index;
}

void SetActivityBarIconTexture(ActivityBarItem item, ImTextureID texture_id)
{
    const int index = ToActivityBarIconIndex(item);
    if (index != 0)
        g_activity_bar_icon_textures[index] = texture_id;
}

void SetActivityBarSettingsIconTexture(ImTextureID texture_id)
{
    g_activity_bar_settings_texture = texture_id;
}

void ConfigureActivityBarIcons()
{
    struct ActivityIconBinding {
        ActivityBarItem item;
        const char* key;
        const char* file_name;
    };

    const ActivityIconBinding bindings[] = {
        { ActivityBarItem::Explorer, "activity.explorer", "folder.png" },
        { ActivityBarItem::Search, "activity.search", "paper.png" },
        { ActivityBarItem::Debug, "activity.debug", "play.png" },
        { ActivityBarItem::Editor, "activity.editor", "area.png" },
        { ActivityBarItem::Extensions, "activity.extensions", "extension.png" }
    };

    for (const ActivityIconBinding& binding : bindings)
        SetActivityBarIconTexture(binding.item, IconTextureManagerLoadPng(binding.key, binding.file_name));

    SetActivityBarSettingsIconTexture(IconTextureManagerLoadPng("activity.settings", "setting.png"));
}

void ClearActivityBarIcons()
{
    SetActivityBarIconTexture(ActivityBarItem::Explorer, (ImTextureID)0);
    SetActivityBarIconTexture(ActivityBarItem::Search, (ImTextureID)0);
    SetActivityBarIconTexture(ActivityBarItem::Debug, (ImTextureID)0);
    SetActivityBarIconTexture(ActivityBarItem::Editor, (ImTextureID)0);
    SetActivityBarIconTexture(ActivityBarItem::Extensions, (ImTextureID)0);
    SetActivityBarSettingsIconTexture((ImTextureID)0);
}

bool InitializeActivityBarIconSystem(
    VkPhysicalDevice physical_device,
    VkDevice device,
    uint32_t queue_family,
    VkQueue queue,
    VkAllocationCallbacks* allocator)
{
    IconTextureManagerInitInfo icon_init{};
    icon_init.physical_device = physical_device;
    icon_init.device = device;
    icon_init.queue_family = queue_family;
    icon_init.queue = queue;
    icon_init.allocator = allocator;
    icon_init.base_path = SDL_GetBasePath();

    if (!IconTextureManagerInitialize(icon_init))
        return false;

    ConfigureActivityBarIcons();
    return true;
}

void ShutdownActivityBarIconSystem()
{
    ClearActivityBarIcons();
    IconTextureManagerShutdown();
}

ActivityBarResult DrawActivityBarUI(float title_h, float status_bar_h, float width, const ActivityBarViewModel& view_model)
{
    ActivityBarResult result{};
    ImGuiIO& io = ImGui::GetIO();

    const WorkbenchTheme& theme = GetWorkbenchTheme();
    const WorkbenchThemeColors& colors = theme.colors;
    const WorkbenchThemeSizes& sizes = theme.sizes;

    ImVec4 bg_color = colors.activity_bar_bg;
    ImVec4 hover_color = colors.activity_bar_hover;
    ImVec4 active_color = colors.activity_bar_active;
    ImVec4 icon_color = colors.activity_bar_icon;

    // Background
    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    float bar_start_y = title_h;
    float bar_end_y = io.DisplaySize.y - status_bar_h;
    bg->AddRectFilled(ImVec2(0, bar_start_y), ImVec2(width, bar_end_y), ImGui::GetColorU32(bg_color));

    ImGui::SetNextWindowPos(ImVec2(0, bar_start_y));
    ImGui::SetNextWindowSize(ImVec2(width, bar_end_y - bar_start_y));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, sizes.activity_bar_padding_y));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, sizes.activity_bar_item_spacing));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                              ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
                              ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav;

    ImGui::Begin("ActivityBar", NULL, flags);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 window_pos = ImGui::GetWindowPos();

    struct ActivityItem {
        ActivityBarItem id;
        const char* tooltip;
    };
    ActivityItem items[] = {
        { ActivityBarItem::Explorer, "Explorer" },
        { ActivityBarItem::Search, "Search" },
        { ActivityBarItem::Debug, "Debug" },
        { ActivityBarItem::Editor, "Editor" },
        { ActivityBarItem::Extensions, "Extensions" }
    };
    constexpr int item_count = sizeof(items) / sizeof(items[0]);
    float item_size = width;
    float item_y = window_pos.y + sizes.activity_bar_padding_y;
    int selected = view_model.selected_index;
    for (int i = 0; i < item_count; ++i) {
        ImVec2 item_min = ImVec2(window_pos.x, item_y);
        ImVec2 item_max = ImVec2(window_pos.x + width, item_y + item_size);
        ImVec2 mouse_pos = ImGui::GetMousePos();
        bool is_hovered = (mouse_pos.x >= item_min.x && mouse_pos.x <= item_max.x && mouse_pos.y >= item_min.y && mouse_pos.y <= item_max.y);
        bool is_selected = (selected == static_cast<int>(items[i].id));
        if (is_selected) {
            draw_list->AddRectFilled(item_min, item_max, ImGui::GetColorU32(active_color));
            draw_list->AddRectFilled(ImVec2(item_min.x, item_min.y), ImVec2(item_min.x + sizes.activity_bar_active_indicator_w, item_max.y), ImGui::GetColorU32(colors.activity_bar_indicator));
        } else if (is_hovered) {
            draw_list->AddRectFilled(item_min, item_max, ImGui::GetColorU32(hover_color));
        }
        ImVec2 icon_center = ImVec2(item_min.x + width * 0.5f, item_min.y + item_size * 0.5f);
        float icon_size = sizes.activity_bar_icon_size;
        ImU32 icon_col = ImGui::GetColorU32(icon_color);
        ImTextureID icon_texture = g_activity_bar_icon_textures[ToActivityBarIconIndex(items[i].id)];
        if (icon_texture != (ImTextureID)0) {
            ImVec2 img_min = ImVec2(icon_center.x - icon_size * 0.5f, icon_center.y - icon_size * 0.5f);
            ImVec2 img_max = ImVec2(icon_center.x + icon_size * 0.5f, icon_center.y + icon_size * 0.5f);
            draw_list->AddImage(icon_texture, img_min, img_max, ImVec2(0, 0), ImVec2(1, 1), icon_col);
        } else {
            switch (items[i].id) {
                case ActivityBarItem::Explorer:
                    draw_list->AddRect(ImVec2(icon_center.x - icon_size * 0.4f, icon_center.y - icon_size * 0.5f),
                                     ImVec2(icon_center.x + icon_size * 0.4f, icon_center.y + icon_size * 0.5f), icon_col, 0, 0, sizes.activity_bar_icon_stroke);
                    draw_list->AddLine(ImVec2(icon_center.x - icon_size * 0.2f, icon_center.y - icon_size * 0.5f),
                                     ImVec2(icon_center.x - icon_size * 0.2f, icon_center.y - icon_size * 0.3f), icon_col, sizes.activity_bar_icon_stroke);
                    break;
                case ActivityBarItem::Search:
                    draw_list->AddCircle(ImVec2(icon_center.x - icon_size * 0.1f, icon_center.y - icon_size * 0.1f), icon_size * 0.3f, icon_col, 12, sizes.activity_bar_icon_stroke);
                    draw_list->AddLine(ImVec2(icon_center.x + icon_size * 0.15f, icon_center.y + icon_size * 0.15f),
                                     ImVec2(icon_center.x + icon_size * 0.4f, icon_center.y + icon_size * 0.4f), icon_col, sizes.activity_bar_icon_stroke);
                    break;
                case ActivityBarItem::Debug:
                    draw_list->AddTriangleFilled(
                        ImVec2(icon_center.x - icon_size * 0.3f, icon_center.y - icon_size * 0.4f),
                        ImVec2(icon_center.x - icon_size * 0.3f, icon_center.y + icon_size * 0.4f),
                        ImVec2(icon_center.x + icon_size * 0.3f, icon_center.y), icon_col);
                    break;
                case ActivityBarItem::Extensions:
                    draw_list->AddRectFilled(ImVec2(icon_center.x - icon_size * 0.4f, icon_center.y - icon_size * 0.4f),
                                           ImVec2(icon_center.x - icon_size * 0.1f, icon_center.y - icon_size * 0.1f), icon_col);
                    draw_list->AddRectFilled(ImVec2(icon_center.x + icon_size * 0.1f, icon_center.y - icon_size * 0.4f),
                                           ImVec2(icon_center.x + icon_size * 0.4f, icon_center.y - icon_size * 0.1f), icon_col);
                    draw_list->AddRectFilled(ImVec2(icon_center.x - icon_size * 0.4f, icon_center.y + icon_size * 0.1f),
                                           ImVec2(icon_center.x - icon_size * 0.1f, icon_center.y + icon_size * 0.4f), icon_col);
                    draw_list->AddRectFilled(ImVec2(icon_center.x + icon_size * 0.1f, icon_center.y + icon_size * 0.1f),
                                           ImVec2(icon_center.x + icon_size * 0.4f, icon_center.y + icon_size * 0.4f), icon_col);
                    break;
                default:
                    break;
            }
        }
        if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            selected = static_cast<int>(items[i].id);
            if (view_model.on_select)
                view_model.on_select(selected);
            result.selected_item = items[i].id;
            result.item_clicked = true;
        }
        if (is_hovered) {
            ImGui::SetTooltip("%s", items[i].tooltip);
        }
        item_y += item_size + sizes.activity_bar_item_spacing;
    }

    float window_height = ImGui::GetWindowSize().y;
    ImVec2 settings_min = ImVec2(window_pos.x, window_pos.y + window_height - sizes.activity_bar_padding_y - item_size);
    ImVec2 settings_max = ImVec2(window_pos.x + width, settings_min.y + item_size);
    ImVec2 mouse_pos = ImGui::GetMousePos();
    bool settings_hovered = (mouse_pos.x >= settings_min.x && mouse_pos.x <= settings_max.x && mouse_pos.y >= settings_min.y && mouse_pos.y <= settings_max.y);

    if (settings_hovered) {
        draw_list->AddRectFilled(settings_min, settings_max, ImGui::GetColorU32(hover_color));
    }

    ImVec2 settings_center = ImVec2(settings_min.x + width * 0.5f, settings_min.y + item_size * 0.5f);
    float settings_icon_size = sizes.activity_bar_icon_size;
    ImU32 settings_icon_col = ImGui::GetColorU32(icon_color);
    if (g_activity_bar_settings_texture != (ImTextureID)0) {
        ImVec2 img_min = ImVec2(settings_center.x - settings_icon_size * 0.5f, settings_center.y - settings_icon_size * 0.5f);
        ImVec2 img_max = ImVec2(settings_center.x + settings_icon_size * 0.5f, settings_center.y + settings_icon_size * 0.5f);
        draw_list->AddImage(g_activity_bar_settings_texture, img_min, img_max, ImVec2(0, 0), ImVec2(1, 1), settings_icon_col);
    } else {
        float gear_outer_r = settings_icon_size * 0.34f;
        float gear_inner_r = settings_icon_size * 0.13f;
        float tooth_inner_r = gear_outer_r * 0.90f;
        float tooth_outer_r = gear_outer_r * 1.32f;
        constexpr float kPi = 3.14159265358979323846f;
        draw_list->AddCircle(settings_center, gear_outer_r, settings_icon_col, 24, sizes.activity_bar_icon_stroke);
        draw_list->AddCircle(settings_center, gear_inner_r, settings_icon_col, 18, sizes.activity_bar_icon_stroke);
        for (int spoke = 0; spoke < 8; ++spoke) {
            float a = kPi * 0.25f * spoke;
            float c = cosf(a);
            float s = sinf(a);
            ImVec2 p0 = ImVec2(settings_center.x + c * tooth_inner_r, settings_center.y + s * tooth_inner_r);
            ImVec2 p1 = ImVec2(settings_center.x + c * tooth_outer_r, settings_center.y + s * tooth_outer_r);
            draw_list->AddLine(p0, p1, settings_icon_col, sizes.activity_bar_icon_stroke);
        }
    }

    if (settings_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        ImGui::OpenPopup("ActivityBarSettingsMenu");
    }
    if (settings_hovered) {
        ImGui::SetTooltip("Settings");
    }

    ImGui::SetNextWindowPos(ImVec2(settings_max.x + 4.0f, settings_min.y), ImGuiCond_Appearing);
    if (ImGui::BeginPopup("ActivityBarSettingsMenu")) {
        ImGui::MenuItem("Workbench Settings", NULL, false, false);
        ImGui::Separator();
        ImGui::MenuItem("Theme", NULL, false, false);
        ImGui::MenuItem("Keyboard Shortcuts", NULL, false, false);
        ImGui::EndPopup();
    }

    ImGui::End();
    ImGui::PopStyleVar(4);
    // 右边框
    ImDrawList* _bg = ImGui::GetBackgroundDrawList();
    _bg->AddLine(ImVec2(width - sizes.activity_bar_border_thickness, bar_start_y), ImVec2(width - sizes.activity_bar_border_thickness, bar_end_y), ImGui::GetColorU32(colors.activity_bar_border), sizes.activity_bar_border_thickness);
    result.selected_item = (ActivityBarItem)selected;
    return result;
}
