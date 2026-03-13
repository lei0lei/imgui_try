#pragma once

#include "imgui.h"

struct WorkbenchMetrics {
    float title_h = 30.0f;
    float status_bar_h = 24.0f;
    float activity_bar_w = 48.0f;
    float primary_sidebar_w = 300.0f;
    float panel_h = 200.0f;
    float secondary_sidebar_w = 300.0f;
};

struct WorkbenchThemeColors {
    ImVec4 clear_color;

    ImVec4 activity_bar_bg;
    ImVec4 activity_bar_hover;
    ImVec4 activity_bar_active;
    ImVec4 activity_bar_icon;
    ImVec4 activity_bar_indicator;
    ImVec4 activity_bar_border;

    ImVec4 title_bar_bg;
    ImVec4 title_bar_menu_hover;
    ImVec4 title_bar_text;
    ImVec4 title_bar_menu_border;
    ImVec4 title_bar_menu_item_hover;
    ImVec4 title_bar_border;
    ImVec4 title_bar_button_hover;
    ImVec4 title_bar_button_active;
    ImVec4 title_bar_close_hover;
    ImVec4 title_bar_close_active;
    ImVec4 title_bar_layout_icon;
    ImVec4 title_bar_control_icon;
    ImVec4 title_bar_icon_light;
    ImVec4 title_bar_icon_dark;
    ImVec4 title_bar_icon_edge;

    ImVec4 status_bar_bg;
    ImVec4 status_bar_text;

    ImVec4 panel_bg;
    ImVec4 panel_tab_bg;
    ImVec4 panel_tab_active;
    ImVec4 panel_tab_hover;
    ImVec4 panel_text;
    ImVec4 panel_active_indicator;
    ImVec4 panel_border;
    ImVec4 panel_terminal_prompt;

    ImVec4 primary_sidebar_bg;
    ImVec4 primary_sidebar_text_dim;
    ImVec4 primary_sidebar_border;

    ImVec4 secondary_sidebar_bg;
    ImVec4 secondary_sidebar_text;
    ImVec4 secondary_sidebar_hover;
    ImVec4 secondary_sidebar_border;
    ImVec4 secondary_sidebar_close_border;
    ImVec4 secondary_sidebar_close_icon;

    ImVec4 editor_welcome_bg;
    ImVec4 editor_welcome_text;
    ImVec4 editor_welcome_hint;

    ImVec4 editor_tab_bar_bg;
    ImVec4 editor_tab_active_bg;
    ImVec4 editor_tab_hover_bg;
    ImVec4 editor_tab_inactive_bg;
    ImVec4 editor_tab_text_active;
    ImVec4 editor_tab_text_inactive;
    ImVec4 editor_tab_modified_dot;
    ImVec4 editor_tab_close_hover_bg;
    ImVec4 editor_tab_close_icon;
    ImVec4 editor_tab_border;
    ImVec4 editor_tab_active_indicator;
};

struct WorkbenchThemeSizes {
    float activity_bar_padding_y;
    float activity_bar_item_spacing;
    float activity_bar_active_indicator_w;
    float activity_bar_icon_size;
    float activity_bar_icon_stroke;
    float activity_bar_border_thickness;

    float title_icon_start_x;
    float title_icon_layout_w_ratio;
    float title_icon_draw_ratio;
    float title_menu_spacing_x;
    float title_menu_btn_w;
    float title_dropdown_w;
    float title_menu_item_h;
    float title_menu_text_padding_x;
    float title_layout_btn_w;
    float title_control_btn_w;
    float title_button_icon_stroke;
    float title_close_icon_size;
    float title_layout_icon_size;
    float title_layout_icon_stroke;
    float title_layout_icon_rounding;
    float title_dropdown_border_thickness;
    float title_menu_arrow_w;
    float title_menu_arrow_h;
    float title_bar_border_thickness;

    float status_bar_padding_x;
    float status_bar_progress_w;
    float status_bar_progress_h;
    float status_bar_progress_right_margin;

    float panel_tab_height;
    float panel_tab_width;
    float panel_content_padding_x;
    float panel_content_padding_y;
    float panel_active_indicator_h;
    float panel_border_thickness;

    float sidebar_padding_x;
    float sidebar_padding_y;
    float sidebar_border_thickness;
    float secondary_close_btn_size;
    float secondary_close_btn_padding;
    float secondary_close_btn_rounding;
    float secondary_close_btn_border_thickness;
    float secondary_close_icon_stroke;

    float editor_tab_width;
    float editor_tab_bar_height;
    float editor_tab_close_size;
    float editor_tab_text_padding_x;
    float editor_tab_close_padding_x;
    float editor_tab_modified_dot_radius;
    float editor_tab_close_rounding;
    float editor_tab_border_thickness;
    float editor_tab_active_indicator_h;

    float editor_welcome_title_offset_y;
    float editor_welcome_hint_offset_y;
};

struct WorkbenchTheme {
    WorkbenchThemeColors colors;
    WorkbenchThemeSizes sizes;
};

const WorkbenchTheme& GetWorkbenchTheme();
void SetWorkbenchTheme(const WorkbenchTheme& theme);
const WorkbenchMetrics& GetWorkbenchMetrics();
void SetWorkbenchMetrics(const WorkbenchMetrics& metrics);

class WorkbenchConfig {
public:
    const WorkbenchMetrics& Metrics() const { return GetWorkbenchMetrics(); }

    void ApplyStyle(float main_scale) const;
    bool LoadFromFile(const char* path);

private:
    // metrics stored globally for shared UI layout
};
