#include "editor_scene_registry.h"

namespace UI {

void EditorSceneRegistry::RegisterRenderer(SceneType type, EditorSceneRenderer renderer)
{
    renderers_[type] = renderer;
}

EditorSceneRenderer EditorSceneRegistry::GetRenderer(SceneType type) const
{
    auto it = renderers_.find(type);
    if (it == renderers_.end()) {
        return nullptr;
    }
    return it->second;
}

} // namespace UI
