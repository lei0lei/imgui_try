#pragma once

#include "imgui.h"
#include "../services/editor_area_service.h"
#include <unordered_map>

namespace UI {

using EditorSceneRenderer = void(*)(ImVec2 content_min, ImVec2 content_max, EditorTab& tab);

class EditorSceneRegistry {
public:
    void RegisterRenderer(SceneType type, EditorSceneRenderer renderer);
    EditorSceneRenderer GetRenderer(SceneType type) const;

private:
    struct SceneTypeHash {
        size_t operator()(SceneType type) const noexcept {
            return static_cast<size_t>(type);
        }
    };

    std::unordered_map<SceneType, EditorSceneRenderer, SceneTypeHash> renderers_;
};

} // namespace UI
