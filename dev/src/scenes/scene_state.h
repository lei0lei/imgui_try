#pragma once

#include <string>

#include "../services/editor_area_service.h"

namespace Scenes::SceneState {

inline std::string& GetOrInit(EditorTab& tab, const char* key, const char* fallback)
{
    std::string& value = tab.scene_ui_state[key];
    if (value.empty() && fallback) {
        value = fallback;
    }
    return value;
}

} // namespace Scenes::SceneState
