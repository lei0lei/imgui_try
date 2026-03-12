#pragma once

#include "../scene_plugin_interface.h"

namespace Scenes::Scene2DPlugin {

class Provider final : public IScenePluginProvider {
public:
    ScenePluginDescriptor GetDescriptor() const override;
};

} // namespace Scenes::Scene2DPlugin
