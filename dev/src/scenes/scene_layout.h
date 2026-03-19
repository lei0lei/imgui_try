#pragma once

#include "scene_context.h"

namespace Scenes {

struct SceneCanvasLayout {
    ImVec2 full_max;
    ImVec2 canvas_max;
    bool show_secondary;
    bool show_panel;
};

inline SceneCanvasLayout ComputeCanvasLayout(
    const SceneContext& ctx,
    float min_canvas_width,
    float min_canvas_height,
    float secondary_width,
    float panel_height)
{
    SceneCanvasLayout out{};
    out.full_max = ctx.content_max;
    out.canvas_max = ctx.content_max;

    const float canvas_width = ctx.content_max.x - ctx.content_min.x;
    const float canvas_height = ctx.content_max.y - ctx.content_min.y;

    out.show_secondary = canvas_width >= min_canvas_width;
    out.show_panel = canvas_height >= min_canvas_height;

    if (out.show_secondary) {
        out.canvas_max.x -= secondary_width;
    }
    if (out.show_panel) {
        out.canvas_max.y -= panel_height;
    }

    return out;
}

} // namespace Scenes
