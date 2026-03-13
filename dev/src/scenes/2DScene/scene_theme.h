#pragma once

#include "imgui.h"

namespace Scenes::Scene2DTheme {

struct Colors {
    ImVec4 bg;
    ImVec4 grid;
    ImVec4 rect_fill;
    ImVec4 rect_border;
    ImVec4 circle_fill;
    ImVec4 circle_border;
    ImVec4 wave;
    ImVec4 title;
    ImVec4 hint;
};

struct Sizes {
    float grid_size;
    float grid_line_thickness;
    float rect_size;
    float rect_outline;
    float circle_radius;
    float circle_outline;
    float wave_thickness;
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
            ImVec4(0.157f, 0.157f, 0.176f, 1.0f),
            ImVec4(0.235f, 0.235f, 0.255f, 1.0f),
            ImVec4(0.39f, 0.59f, 1.0f, 0.70f),
            ImVec4(0.59f, 0.78f, 1.0f, 1.0f),
            ImVec4(1.0f, 0.39f, 0.59f, 0.70f),
            ImVec4(1.0f, 0.59f, 0.78f, 1.0f),
            ImVec4(0.39f, 1.0f, 0.59f, 1.0f),
            ImVec4(0.86f, 0.86f, 0.90f, 1.0f),
            ImVec4(0.55f, 0.55f, 0.59f, 0.80f),
        },
        {
            30.0f,
            1.0f,
            80.0f,
            2.5f,
            50.0f,
            2.5f,
            3.0f,
            20.0f,
        }
    };
    return theme;
}

} // namespace Scenes::Scene2DTheme
