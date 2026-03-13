/**
 * @file workbench_config.cpp
 * @brief 工作台的配置管理
 * @author Your Name
 * @date 2026-02-05
 */

#include "workbench_config.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

WorkbenchTheme MakeDefaultTheme()
{
    WorkbenchTheme theme{};

    theme.colors.clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    theme.colors.activity_bar_bg = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    theme.colors.activity_bar_hover = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
    theme.colors.activity_bar_active = ImVec4(0.25f, 0.25f, 0.50f, 1.0f);
    theme.colors.activity_bar_icon = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);
    theme.colors.activity_bar_indicator = ImVec4(0.0f, 0.48f, 0.80f, 1.0f);
    theme.colors.activity_bar_border = ImVec4(0.24f, 0.24f, 0.26f, 1.0f);

    theme.colors.title_bar_bg = ImVec4(0.173f, 0.173f, 0.188f, 1.0f);
    theme.colors.title_bar_menu_hover = ImVec4(0.282f, 0.282f, 0.298f, 1.0f);
    theme.colors.title_bar_text = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);
    theme.colors.title_bar_menu_border = ImVec4(0.50f, 0.50f, 0.50f, 1.0f);
    theme.colors.title_bar_menu_item_hover = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    theme.colors.title_bar_border = ImVec4(0.24f, 0.24f, 0.26f, 1.0f);
    theme.colors.title_bar_button_hover = ImVec4(1.0f, 1.0f, 1.0f, 0.10f);
    theme.colors.title_bar_button_active = ImVec4(1.0f, 1.0f, 1.0f, 0.15f);
    theme.colors.title_bar_close_hover = ImVec4(0.81f, 0.26f, 0.26f, 1.0f);
    theme.colors.title_bar_close_active = ImVec4(0.91f, 0.36f, 0.36f, 1.0f);
    theme.colors.title_bar_layout_icon = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    theme.colors.title_bar_control_icon = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    theme.colors.title_bar_icon_light = ImVec4(0.39f, 0.59f, 1.0f, 1.0f);
    theme.colors.title_bar_icon_dark = ImVec4(0.24f, 0.39f, 0.78f, 1.0f);
    theme.colors.title_bar_icon_edge = ImVec4(0.59f, 0.71f, 1.0f, 1.0f);

    theme.colors.status_bar_bg = ImVec4(0.18f, 0.18f, 0.20f, 1.0f);
    theme.colors.status_bar_text = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);

    theme.colors.panel_bg = ImVec4(0.118f, 0.118f, 0.118f, 1.0f);
    theme.colors.panel_tab_bg = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    theme.colors.panel_tab_active = ImVec4(0.118f, 0.118f, 0.118f, 1.0f);
    theme.colors.panel_tab_hover = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    theme.colors.panel_text = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);
    theme.colors.panel_active_indicator = ImVec4(0.0f, 0.48f, 0.80f, 1.0f);
    theme.colors.panel_border = ImVec4(0.24f, 0.24f, 0.26f, 1.0f);
    theme.colors.panel_terminal_prompt = ImVec4(0.5f, 0.8f, 0.5f, 1.0f);

    theme.colors.primary_sidebar_bg = ImVec4(0.149f, 0.149f, 0.149f, 1.0f);
    theme.colors.primary_sidebar_text_dim = ImVec4(0.60f, 0.60f, 0.60f, 1.0f);
    theme.colors.primary_sidebar_border = ImVec4(0.24f, 0.24f, 0.26f, 1.0f);

    theme.colors.secondary_sidebar_bg = ImVec4(0.149f, 0.149f, 0.149f, 1.0f);
    theme.colors.secondary_sidebar_text = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
    theme.colors.secondary_sidebar_hover = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
    theme.colors.secondary_sidebar_border = ImVec4(0.24f, 0.24f, 0.26f, 1.0f);
    theme.colors.secondary_sidebar_close_border = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
    theme.colors.secondary_sidebar_close_icon = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);

    theme.colors.editor_welcome_bg = ImVec4(0.118f, 0.118f, 0.118f, 1.0f);
    theme.colors.editor_welcome_text = ImVec4(0.59f, 0.59f, 0.59f, 1.0f);
    theme.colors.editor_welcome_hint = ImVec4(0.39f, 0.39f, 0.39f, 1.0f);

    theme.colors.editor_tab_bar_bg = ImVec4(0.145f, 0.145f, 0.149f, 1.0f);
    theme.colors.editor_tab_active_bg = ImVec4(0.118f, 0.118f, 0.118f, 1.0f);
    theme.colors.editor_tab_hover_bg = ImVec4(0.176f, 0.176f, 0.176f, 1.0f);
    theme.colors.editor_tab_inactive_bg = ImVec4(0.145f, 0.145f, 0.149f, 1.0f);
    theme.colors.editor_tab_text_active = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    theme.colors.editor_tab_text_inactive = ImVec4(0.67f, 0.67f, 0.67f, 1.0f);
    theme.colors.editor_tab_modified_dot = ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
    theme.colors.editor_tab_close_hover_bg = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
    theme.colors.editor_tab_close_icon = ImVec4(0.90f, 0.90f, 0.90f, 1.0f);
    theme.colors.editor_tab_border = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    theme.colors.editor_tab_active_indicator = ImVec4(0.0f, 0.48f, 0.80f, 1.0f);

    theme.sizes.activity_bar_padding_y = 8.0f;
    theme.sizes.activity_bar_item_spacing = 4.0f;
    theme.sizes.activity_bar_active_indicator_w = 2.0f;
    theme.sizes.activity_bar_icon_size = 20.0f;
    theme.sizes.activity_bar_icon_stroke = 2.0f;
    theme.sizes.activity_bar_border_thickness = 1.0f;

    theme.sizes.title_icon_start_x = 6.0f;
    theme.sizes.title_icon_layout_w_ratio = 0.85f;
    theme.sizes.title_icon_draw_ratio = 1.80f;
    theme.sizes.title_menu_spacing_x = 8.0f;
    theme.sizes.title_menu_btn_w = 60.0f;
    theme.sizes.title_dropdown_w = 150.0f;
    theme.sizes.title_menu_item_h = 24.0f;
    theme.sizes.title_menu_text_padding_x = 8.0f;
    theme.sizes.title_layout_btn_w = 32.0f;
    theme.sizes.title_control_btn_w = 46.0f;
    theme.sizes.title_button_icon_stroke = 1.0f;
    theme.sizes.title_close_icon_size = 5.0f;
    theme.sizes.title_layout_icon_size = 8.0f;
    theme.sizes.title_layout_icon_stroke = 1.5f;
    theme.sizes.title_layout_icon_rounding = 2.0f;
    theme.sizes.title_dropdown_border_thickness = 1.0f;
    theme.sizes.title_menu_arrow_w = 5.0f;
    theme.sizes.title_menu_arrow_h = 8.0f;
    theme.sizes.title_bar_border_thickness = 2.0f;

    theme.sizes.status_bar_padding_x = 8.0f;
    theme.sizes.status_bar_progress_w = 100.0f;
    theme.sizes.status_bar_progress_h = 16.0f;
    theme.sizes.status_bar_progress_right_margin = 120.0f;

    theme.sizes.panel_tab_height = 35.0f;
    theme.sizes.panel_tab_width = 120.0f;
    theme.sizes.panel_content_padding_x = 8.0f;
    theme.sizes.panel_content_padding_y = 8.0f;
    theme.sizes.panel_active_indicator_h = 2.0f;
    theme.sizes.panel_border_thickness = 1.0f;

    theme.sizes.sidebar_padding_x = 12.0f;
    theme.sizes.sidebar_padding_y = 8.0f;
    theme.sizes.sidebar_border_thickness = 1.0f;
    theme.sizes.secondary_close_btn_size = 18.0f;
    theme.sizes.secondary_close_btn_padding = 6.0f;
    theme.sizes.secondary_close_btn_rounding = 3.0f;
    theme.sizes.secondary_close_btn_border_thickness = 1.0f;
    theme.sizes.secondary_close_icon_stroke = 1.8f;

    theme.sizes.editor_tab_width = 120.0f;
    theme.sizes.editor_tab_bar_height = 35.0f;
    theme.sizes.editor_tab_close_size = 16.0f;
    theme.sizes.editor_tab_text_padding_x = 10.0f;
    theme.sizes.editor_tab_close_padding_x = 8.0f;
    theme.sizes.editor_tab_modified_dot_radius = 3.0f;
    theme.sizes.editor_tab_close_rounding = 2.0f;
    theme.sizes.editor_tab_border_thickness = 1.0f;
    theme.sizes.editor_tab_active_indicator_h = 2.0f;

    theme.sizes.editor_welcome_title_offset_y = -30.0f;
    theme.sizes.editor_welcome_hint_offset_y = 10.0f;

    return theme;
}

WorkbenchTheme g_theme = MakeDefaultTheme();
WorkbenchMetrics g_metrics{};

std::string Trim(std::string value)
{
    auto not_space = [](unsigned char ch) { return !std::isspace(ch); };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), not_space));
    value.erase(std::find_if(value.rbegin(), value.rend(), not_space).base(), value.end());
    return value;
}

std::string ToLower(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return (char)std::tolower(c); });
    return value;
}

bool ParseFloatList(const std::string& value, std::vector<float>& out)
{
    out.clear();
    std::stringstream ss(value);
    std::string item;
    while (std::getline(ss, item, ',')) {
        item = Trim(item);
        if (item.empty())
            return false;
        try {
            out.push_back(std::stof(item));
        } catch (...) {
            return false;
        }
    }
    return !out.empty();
}

ImVec4 ParseColor(const std::string& value, const ImVec4& fallback)
{
    std::vector<float> components;
    if (!ParseFloatList(value, components))
        return fallback;
    if (components.size() < 3)
        return fallback;

    float r = components[0];
    float g = components[1];
    float b = components[2];
    float a = (components.size() >= 4) ? components[3] : 1.0f;

    float max_component = std::max(std::max(r, g), std::max(b, a));
    if (max_component > 1.0f) {
        r /= 255.0f;
        g /= 255.0f;
        b /= 255.0f;
        a /= 255.0f;
    }
    return ImVec4(r, g, b, a);
}

bool ParseFloatValue(const std::string& value, float& out)
{
    try {
        out = std::stof(value);
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace

const WorkbenchTheme& GetWorkbenchTheme()
{
    return g_theme;
}

void SetWorkbenchTheme(const WorkbenchTheme& theme)
{
    g_theme = theme;
}

const WorkbenchMetrics& GetWorkbenchMetrics()
{
    return g_metrics;
}

void SetWorkbenchMetrics(const WorkbenchMetrics& metrics)
{
    g_metrics = metrics;
}

void WorkbenchConfig::ApplyStyle(float main_scale) const
{
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;
}

bool WorkbenchConfig::LoadFromFile(const char* path)
{
    std::ifstream file(path);
    if (!file.is_open())
        return false;

    WorkbenchTheme theme = GetWorkbenchTheme();
    WorkbenchMetrics metrics = GetWorkbenchMetrics();

    std::unordered_map<std::string, ImVec4*> color_map = {
        {"clear_color", &theme.colors.clear_color},
        {"activity_bar_bg", &theme.colors.activity_bar_bg},
        {"activity_bar_hover", &theme.colors.activity_bar_hover},
        {"activity_bar_active", &theme.colors.activity_bar_active},
        {"activity_bar_icon", &theme.colors.activity_bar_icon},
        {"activity_bar_indicator", &theme.colors.activity_bar_indicator},
        {"activity_bar_border", &theme.colors.activity_bar_border},

        {"title_bar_bg", &theme.colors.title_bar_bg},
        {"title_bar_menu_hover", &theme.colors.title_bar_menu_hover},
        {"title_bar_text", &theme.colors.title_bar_text},
        {"title_bar_menu_border", &theme.colors.title_bar_menu_border},
        {"title_bar_menu_item_hover", &theme.colors.title_bar_menu_item_hover},
        {"title_bar_border", &theme.colors.title_bar_border},
        {"title_bar_button_hover", &theme.colors.title_bar_button_hover},
        {"title_bar_button_active", &theme.colors.title_bar_button_active},
        {"title_bar_close_hover", &theme.colors.title_bar_close_hover},
        {"title_bar_close_active", &theme.colors.title_bar_close_active},
        {"title_bar_layout_icon", &theme.colors.title_bar_layout_icon},
        {"title_bar_control_icon", &theme.colors.title_bar_control_icon},
        {"title_bar_icon_light", &theme.colors.title_bar_icon_light},
        {"title_bar_icon_dark", &theme.colors.title_bar_icon_dark},
        {"title_bar_icon_edge", &theme.colors.title_bar_icon_edge},

        {"status_bar_bg", &theme.colors.status_bar_bg},
        {"status_bar_text", &theme.colors.status_bar_text},

        {"panel_bg", &theme.colors.panel_bg},
        {"panel_tab_bg", &theme.colors.panel_tab_bg},
        {"panel_tab_active", &theme.colors.panel_tab_active},
        {"panel_tab_hover", &theme.colors.panel_tab_hover},
        {"panel_text", &theme.colors.panel_text},
        {"panel_active_indicator", &theme.colors.panel_active_indicator},
        {"panel_border", &theme.colors.panel_border},
        {"panel_terminal_prompt", &theme.colors.panel_terminal_prompt},

        {"primary_sidebar_bg", &theme.colors.primary_sidebar_bg},
        {"primary_sidebar_text_dim", &theme.colors.primary_sidebar_text_dim},
        {"primary_sidebar_border", &theme.colors.primary_sidebar_border},

        {"secondary_sidebar_bg", &theme.colors.secondary_sidebar_bg},
        {"secondary_sidebar_text", &theme.colors.secondary_sidebar_text},
        {"secondary_sidebar_hover", &theme.colors.secondary_sidebar_hover},
        {"secondary_sidebar_border", &theme.colors.secondary_sidebar_border},
        {"secondary_sidebar_close_border", &theme.colors.secondary_sidebar_close_border},
        {"secondary_sidebar_close_icon", &theme.colors.secondary_sidebar_close_icon},

        {"editor_welcome_bg", &theme.colors.editor_welcome_bg},
        {"editor_welcome_text", &theme.colors.editor_welcome_text},
        {"editor_welcome_hint", &theme.colors.editor_welcome_hint},

        {"editor_tab_bar_bg", &theme.colors.editor_tab_bar_bg},
        {"editor_tab_active_bg", &theme.colors.editor_tab_active_bg},
        {"editor_tab_hover_bg", &theme.colors.editor_tab_hover_bg},
        {"editor_tab_inactive_bg", &theme.colors.editor_tab_inactive_bg},
        {"editor_tab_text_active", &theme.colors.editor_tab_text_active},
        {"editor_tab_text_inactive", &theme.colors.editor_tab_text_inactive},
        {"editor_tab_modified_dot", &theme.colors.editor_tab_modified_dot},
        {"editor_tab_close_hover_bg", &theme.colors.editor_tab_close_hover_bg},
        {"editor_tab_close_icon", &theme.colors.editor_tab_close_icon},
        {"editor_tab_border", &theme.colors.editor_tab_border},
        {"editor_tab_active_indicator", &theme.colors.editor_tab_active_indicator},
    };

    std::unordered_map<std::string, float*> size_map = {
        {"title_h", &metrics.title_h},
        {"status_bar_h", &metrics.status_bar_h},
        {"activity_bar_w", &metrics.activity_bar_w},
        {"primary_sidebar_w", &metrics.primary_sidebar_w},
        {"panel_h", &metrics.panel_h},
        {"secondary_sidebar_w", &metrics.secondary_sidebar_w},

        {"activity_bar_padding_y", &theme.sizes.activity_bar_padding_y},
        {"activity_bar_item_spacing", &theme.sizes.activity_bar_item_spacing},
        {"activity_bar_active_indicator_w", &theme.sizes.activity_bar_active_indicator_w},
        {"activity_bar_icon_size", &theme.sizes.activity_bar_icon_size},
        {"activity_bar_icon_stroke", &theme.sizes.activity_bar_icon_stroke},
        {"activity_bar_border_thickness", &theme.sizes.activity_bar_border_thickness},

        {"title_icon_start_x", &theme.sizes.title_icon_start_x},
        {"title_icon_layout_w_ratio", &theme.sizes.title_icon_layout_w_ratio},
        {"title_icon_draw_ratio", &theme.sizes.title_icon_draw_ratio},
        {"title_menu_spacing_x", &theme.sizes.title_menu_spacing_x},
        {"title_menu_btn_w", &theme.sizes.title_menu_btn_w},
        {"title_dropdown_w", &theme.sizes.title_dropdown_w},
        {"title_menu_item_h", &theme.sizes.title_menu_item_h},
        {"title_menu_text_padding_x", &theme.sizes.title_menu_text_padding_x},
        {"title_layout_btn_w", &theme.sizes.title_layout_btn_w},
        {"title_control_btn_w", &theme.sizes.title_control_btn_w},
        {"title_button_icon_stroke", &theme.sizes.title_button_icon_stroke},
        {"title_close_icon_size", &theme.sizes.title_close_icon_size},
        {"title_layout_icon_size", &theme.sizes.title_layout_icon_size},
        {"title_layout_icon_stroke", &theme.sizes.title_layout_icon_stroke},
        {"title_layout_icon_rounding", &theme.sizes.title_layout_icon_rounding},
        {"title_dropdown_border_thickness", &theme.sizes.title_dropdown_border_thickness},
        {"title_menu_arrow_w", &theme.sizes.title_menu_arrow_w},
        {"title_menu_arrow_h", &theme.sizes.title_menu_arrow_h},
        {"title_bar_border_thickness", &theme.sizes.title_bar_border_thickness},

        {"status_bar_padding_x", &theme.sizes.status_bar_padding_x},
        {"status_bar_progress_w", &theme.sizes.status_bar_progress_w},
        {"status_bar_progress_h", &theme.sizes.status_bar_progress_h},
        {"status_bar_progress_right_margin", &theme.sizes.status_bar_progress_right_margin},

        {"panel_tab_height", &theme.sizes.panel_tab_height},
        {"panel_tab_width", &theme.sizes.panel_tab_width},
        {"panel_content_padding_x", &theme.sizes.panel_content_padding_x},
        {"panel_content_padding_y", &theme.sizes.panel_content_padding_y},
        {"panel_active_indicator_h", &theme.sizes.panel_active_indicator_h},
        {"panel_border_thickness", &theme.sizes.panel_border_thickness},

        {"sidebar_padding_x", &theme.sizes.sidebar_padding_x},
        {"sidebar_padding_y", &theme.sizes.sidebar_padding_y},
        {"sidebar_border_thickness", &theme.sizes.sidebar_border_thickness},
        {"secondary_close_btn_size", &theme.sizes.secondary_close_btn_size},
        {"secondary_close_btn_padding", &theme.sizes.secondary_close_btn_padding},
        {"secondary_close_btn_rounding", &theme.sizes.secondary_close_btn_rounding},
        {"secondary_close_btn_border_thickness", &theme.sizes.secondary_close_btn_border_thickness},
        {"secondary_close_icon_stroke", &theme.sizes.secondary_close_icon_stroke},

        {"editor_tab_width", &theme.sizes.editor_tab_width},
        {"editor_tab_bar_height", &theme.sizes.editor_tab_bar_height},
        {"editor_tab_close_size", &theme.sizes.editor_tab_close_size},
        {"editor_tab_text_padding_x", &theme.sizes.editor_tab_text_padding_x},
        {"editor_tab_close_padding_x", &theme.sizes.editor_tab_close_padding_x},
        {"editor_tab_modified_dot_radius", &theme.sizes.editor_tab_modified_dot_radius},
        {"editor_tab_close_rounding", &theme.sizes.editor_tab_close_rounding},
        {"editor_tab_border_thickness", &theme.sizes.editor_tab_border_thickness},
        {"editor_tab_active_indicator_h", &theme.sizes.editor_tab_active_indicator_h},

        {"editor_welcome_title_offset_y", &theme.sizes.editor_welcome_title_offset_y},
        {"editor_welcome_hint_offset_y", &theme.sizes.editor_welcome_hint_offset_y},
    };

    std::string section;
    std::string line;
    while (std::getline(file, line)) {
        line = Trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';')
            continue;

        if (line.front() == '[' && line.back() == ']') {
            section = ToLower(Trim(line.substr(1, line.size() - 2)));
            continue;
        }

        size_t eq = line.find('=');
        if (eq == std::string::npos)
            continue;

        std::string key = ToLower(Trim(line.substr(0, eq)));
        std::string value = Trim(line.substr(eq + 1));

        if (section == "colors") {
            auto it = color_map.find(key);
            if (it != color_map.end())
                *it->second = ParseColor(value, *it->second);
        } else if (section == "sizes") {
            auto it = size_map.find(key);
            if (it != size_map.end())
                ParseFloatValue(value, *it->second);
        }
    }

    SetWorkbenchTheme(theme);
    SetWorkbenchMetrics(metrics);
    return true;
}
