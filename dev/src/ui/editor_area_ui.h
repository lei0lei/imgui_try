#pragma once

#include "../services/editor_area_service.h"
#include "editor_scene_registry.h"

namespace UI {

class EditorArea {
public:
    explicit EditorArea(EditorAreaService& service);

    void Draw(
        float left_offset,
        float right_offset,
        float title_h,
        float status_bar_h,
        float panel_h,
        bool panel_visible
    );

private:
    EditorAreaService& service_;
    EditorSceneRegistry scene_registry_;
};

} // namespace UI
