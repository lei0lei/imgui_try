#pragma once

#include "../services/view_registry.h"
#include "../core/workbench_types.h"

namespace UI {

void RegisterDefaultViews(IViewRegistry& registry);
std::string GetDefaultScenePluginId();

ViewDefinition GetDefaultPrimaryViewForPlugin(const std::string& plugin_id, ActivityBarItem item);
ViewDefinition GetDefaultSecondaryViewForPlugin(const std::string& plugin_id);
ViewDefinition GetDefaultPanelViewForPlugin(const std::string& plugin_id);

} // namespace UI