#pragma once

#include "../scene_plugin_interface.h"

namespace Scenes::Scene3DPlugin {

class Provider final : public IScenePluginProvider {
public:
    ScenePluginDescriptor GetDescriptor() const override;
};

} // namespace Scenes::Scene3DPlugin
