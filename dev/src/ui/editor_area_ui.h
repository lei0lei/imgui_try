#pragma once

#include <vector>
#include "../services/editor_area_service.h"
#include "../services/event_bus.h"

namespace UI {

class EditorArea {
public:
    EditorArea();

    struct Props {
        float left_offset = 0.0f;
        float right_offset = 0.0f;
        float title_h = 0.0f;
        float status_bar_h = 0.0f;
        bool block_tab_clicks = false;
        const std::vector<EditorTab>* tabs = nullptr;
        EditorTab* active_tab = nullptr;
        IEventBus* event_bus = nullptr;
    };

    struct Result {
        int closed_tab = -1;
        int active_tab = -1;
        int move_from = -1;
        int move_to = -1;
    };

    Result Draw(const Props& props);

private:
    // Scene renderers are resolved via ScenePluginRegistry.
};

} // namespace UI
