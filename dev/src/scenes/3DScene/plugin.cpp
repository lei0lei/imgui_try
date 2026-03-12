#include "plugin.h"

namespace Scenes::Scene3DPlugin {

ScenePluginDescriptor Provider::GetDescriptor() const
{
    return {
        "scene.3d",
        "3D Scene Tools",
        "Open a realtime 3D workspace with rendering playground.",
        "imgui-try team",
        "3D",
        SceneType::Scene3D,
        "3DScene"
    };
}

} // namespace Scenes::Scene3DPlugin
