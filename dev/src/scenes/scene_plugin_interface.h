#pragma once

#include <string>
#include <vector>

#include "../services/editor_area_service.h"

namespace Scenes {

using SceneCanvasRenderer = void(*)(ImVec2 content_min, ImVec2 content_max, EditorTab& tab);

struct ScenePluginDescriptor {
    std::string id;
    std::string name;
    std::string info;
    std::string author;
    std::string icon_text;
    std::string folder;
};

class IScenePluginProvider {
public:
    virtual ~IScenePluginProvider() = default;
    virtual ScenePluginDescriptor GetDescriptor() const = 0;
};

} // namespace Scenes
