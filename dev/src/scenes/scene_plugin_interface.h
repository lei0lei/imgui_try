#pragma once

#include <functional>
#include <string>
#include <vector>

#include "../core/workbench_types.h"
#include "../services/editor_area_service.h"
#include "../services/view_types.h"
#include "scene_context.h"

namespace Scenes {

using SceneCanvasRenderer = void(*)(const SceneContext& ctx, EditorTab& tab);

struct ScenePluginDescriptor {
    std::string id;
    std::string name;
    std::string info;
    std::string author;
    std::string icon_text;
    std::string folder;
};

struct SceneDebugVariable {
    std::string name;
    std::string value;
};

struct SceneDebugWatch {
    std::string expression;
    std::string value;
};

struct SceneDebugCallStackFrame {
    std::string label;
    std::string location;
    bool is_active = false;
};

struct ScenePrimarySidebarDebugData {
    std::vector<SceneDebugVariable> variables;
    std::vector<SceneDebugWatch> watches;
    std::vector<SceneDebugCallStackFrame> callstack;
};

using ScenePrimarySidebarDebugDataProvider = std::function<ScenePrimarySidebarDebugData(const EditorTab*)>;

// Scene contributions to the global primary sidebar views.
// Workbench remains the owner of the primary sidebar layout; scenes can only render optional content
// inside the selected ActivityBarItem's default view (e.g. Debug / Editor).
using ScenePrimarySidebarContributionRenderer = ViewRenderer;

} // namespace Scenes
