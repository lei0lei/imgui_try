#include "plugin.h"

namespace Scenes::NodeEditorScenePlugin {

ScenePluginDescriptor Provider::GetDescriptor() const
{
    return {
        "scene.node",
        "Node Editor Toolkit",
        "Build node graphs and execute scripts in output panel.",
        "imgui-try team",
        "NG",
        SceneType::NodeEditor,
        "NodeEditorScene"
    };
}

} // namespace Scenes::NodeEditorScenePlugin
