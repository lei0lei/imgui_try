#pragma once

#include <functional>
#include <string>

// These define SceneTitleBarExtensionRenderer / SceneStatusBarExtensionRenderer
#include "../core/title_bar_extension.h"
#include "status_bar_extension.h"

struct EditorTab;

namespace WorkbenchSceneExtensionResolver {

struct TitleBarSceneExtensions {
    const SceneTitleBarExtensionRenderer* extension = nullptr;
    const std::function<void(EditorTab&, const std::string&)>* action_handler = nullptr;
    float extension_width = 0.0f;
};

struct StatusBarSceneExtensions {
    const SceneStatusBarExtensionRenderer* extension = nullptr;
    const std::function<void(EditorTab&, const std::string&)>* action_handler = nullptr;
};

TitleBarSceneExtensions ResolveTitleBar(EditorTab* active_tab, float title_h);
StatusBarSceneExtensions ResolveStatusBar(EditorTab* active_tab);

} // namespace WorkbenchSceneExtensionResolver

