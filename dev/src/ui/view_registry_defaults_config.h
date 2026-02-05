#pragma once

#include <vector>
#include "../services/view_registry.h"
#include "../core/workbench_types.h"

namespace UI {

struct DefaultViewConfig {
    SceneType mode;
    ViewContainer container;
    const char* id;
    const char* title;
    ViewRenderer renderer;
};

struct EditorViewPlugin {
    SceneType mode;
    std::vector<ViewDefinition> primary_views;
    std::vector<ViewDefinition> secondary_views;
    std::vector<ViewDefinition> panel_views;
    std::vector<std::pair<ActivityBarItem, std::string>> primary_bindings;
    std::string default_secondary_id;
    std::string default_panel_id;
};

const std::vector<DefaultViewConfig>& GetDefaultViewConfigs();
void RegisterViewPlugin(const EditorViewPlugin& plugin);
const EditorViewPlugin* GetViewPlugin(SceneType mode);
ViewDefinition GetDefaultPrimaryView(SceneType mode, ActivityBarItem item);
ViewDefinition GetDefaultSecondaryView(SceneType mode);
ViewDefinition GetDefaultPanelView(SceneType mode);

} // namespace UI
