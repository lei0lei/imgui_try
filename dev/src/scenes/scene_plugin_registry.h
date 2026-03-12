#pragma once

#include <vector>

#include "scene_plugin_interface.h"

namespace Scenes {

class ScenePluginRegistry {
public:
    static ScenePluginRegistry& Instance();

    void EnsureLoaded();
    const std::vector<ScenePluginDescriptor>& GetPlugins() const { return plugins_; }

private:
    void Load();

    bool loaded_ = false;
    std::vector<ScenePluginDescriptor> plugins_;
};

} // namespace Scenes
