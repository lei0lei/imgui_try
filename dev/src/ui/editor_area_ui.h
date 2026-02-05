#pragma once

#include <functional>
#include <vector>
#include "../services/editor_area_service.h"
#include "editor_scene_registry.h"

namespace UI {

class EditorArea {
public:
    EditorArea();

    struct ViewModel {
        std::function<const std::vector<EditorTab>&()> get_tabs;
        std::function<EditorTab*()> get_active_tab;
        std::function<void(int)> close_tab;
        std::function<void(int)> activate_tab;
    };

    void Draw(
        float left_offset,
        float right_offset,
        float title_h,
        float status_bar_h,
        float panel_h,
        bool panel_visible,
        bool block_tab_clicks,
        const ViewModel& view_model
    );

private:
    EditorSceneRegistry scene_registry_;
};

} // namespace UI
