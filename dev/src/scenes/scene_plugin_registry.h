#pragma once

#include <unordered_map>
#include <vector>

#include "scene_plugin_interface.h"

namespace Scenes {

class ScenePluginRegistry {
public:
    static ScenePluginRegistry& Instance();
    static void RegisterRenderer(const std::string& plugin_id, SceneCanvasRenderer renderer);

    void EnsureLoaded();
    const std::vector<ScenePluginDescriptor>& GetPlugins() const { return plugins_; }
    const ScenePluginDescriptor* FindPluginById(const std::string& plugin_id) const;
    SceneCanvasRenderer GetRenderer(const std::string& plugin_id) const;

private:
    void Load();

    bool loaded_ = false;
    std::vector<ScenePluginDescriptor> plugins_;
    std::unordered_map<std::string, SceneCanvasRenderer> renderers_;
};

} // namespace Scenes
