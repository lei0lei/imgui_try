#include "plugin.h"

namespace Scenes::Scene2DPlugin {

ScenePluginDescriptor Provider::GetDescriptor() const
{
    return {
        "scene.2d",
        "2D Scene Tools",
        "Create and inspect 2D scene workflows for experiments.",
        "imgui-try team",
        "2D",
        SceneType::Scene2D,
        "2DScene"
    };
}

} // namespace Scenes::Scene2DPlugin
