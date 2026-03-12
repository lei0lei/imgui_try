#pragma once

#include "../scene_plugin_interface.h"

namespace Scenes::NodeEditorScenePlugin {

class Provider final : public IScenePluginProvider {
public:
    ScenePluginDescriptor GetDescriptor() const override;
};

} // namespace Scenes::NodeEditorScenePlugin
