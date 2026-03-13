#pragma once

#include "imgui.h"

namespace Scenes::Scene3DTheme {

struct Colors {
    ImVec4 bg_top;
    ImVec4 bg_bottom;
    ImVec4 grid;
    ImVec4 cube;
    ImVec4 title;
    ImVec4 hint;
};

struct Sizes {
    float grid_spacing;
    float grid_line_thickness;
    float horizontal_line_thickness;
    float grid_lines;
    float horizontal_lines;
    float orbit_radius;
    float cube_size;
    float cube_outline;
    float content_padding;
};

struct Theme {
    Colors colors;
    Sizes sizes;
};

inline const Theme& Get()
{
    static const Theme theme = {
        {
            ImVec4(0.098f, 0.098f, 0.118f, 1.0f),
            ImVec4(0.059f, 0.059f, 0.078f, 1.0f),
            ImVec4(0.24f, 0.47f, 0.71f, 0.47f),
            ImVec4(0.31f, 0.63f, 0.86f, 0.78f),
            ImVec4(0.78f, 0.86f, 0.94f, 1.0f),
            ImVec4(0.47f, 0.55f, 0.63f, 0.80f),
        },
        {
            40.0f,
            1.5f,
            1.5f,
            20.0f,
            15.0f,
            80.0f,
            25.0f,
            2.0f,
            20.0f,
        }
    };
    return theme;
}

} // namespace Scenes::Scene3DTheme
