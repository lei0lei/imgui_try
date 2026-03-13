#pragma once

#include "imgui.h"

namespace Scenes::NodeEditorTheme {

struct Colors {
    ImVec4 bg;
    ImVec4 grid_minor;
    ImVec4 grid_major;
    ImVec4 axis;
    ImVec4 overlay_bg;
    ImVec4 overlay_hover;
    ImVec4 overlay_active;
    ImVec4 overlay_text;
};

struct Sizes {
    float grid_size;
    float grid_thickness;
    float grid_major_thickness;
    float axis_thickness;
    float grid_draw_min;
    float grid_draw_max;
    float grid_minor_threshold;
    float zoom_min;
    float zoom_max;
    float zoom_step;
    float overlay_padding;
    float overlay_rounding;
};

struct Theme {
    Colors colors;
    Sizes sizes;
};

inline const Theme& Get()
{
    static const Theme theme = {
        {
            ImVec4(0.118f, 0.118f, 0.129f, 1.0f),
            ImVec4(0.20f, 0.20f, 0.22f, 1.0f),
            ImVec4(0.26f, 0.26f, 0.30f, 1.0f),
            ImVec4(0.35f, 0.35f, 0.42f, 1.0f),
            ImVec4(0.20f, 0.20f, 0.24f, 0.95f),
            ImVec4(0.30f, 0.30f, 0.36f, 0.98f),
            ImVec4(0.38f, 0.38f, 0.45f, 1.0f),
            ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        },
        {
            64.0f,
            1.0f,
            1.5f,
            2.0f,
            12.0f,
            72.0f,
            1.25f,
            0.3f,
            1.5f,
            0.1f,
            6.0f,
            4.0f,
        }
    };
    return theme;
}

} // namespace Scenes::NodeEditorTheme
