#pragma once

#include <string>
#include <vector>

#include "../services/editor_area_service.h"

namespace Scenes {

struct ScenePluginDescriptor {
    std::string id;
    std::string name;
    std::string info;
    std::string author;
    std::string icon_text;
    SceneType mode = SceneType::Scene3D;
    std::string folder;
};

class IScenePluginProvider {
public:
    virtual ~IScenePluginProvider() = default;
    virtual ScenePluginDescriptor GetDescriptor() const = 0;
};

} // namespace Scenes
