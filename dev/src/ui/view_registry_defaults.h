#pragma once

#include "../services/view_registry.h"
#include "../core/workbench_types.h"

namespace UI {

void RegisterDefaultViews(IViewRegistry& registry);
std::string GetDefaultScenePluginId();
const std::string& GetPrimarySidebarGlobalPluginId();

ViewDefinition GetDefaultPrimaryViewForPlugin(const std::string& plugin_id, ActivityBarItem item);

} // namespace UI