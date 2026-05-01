#pragma once

#include <string>
#include <vector>
#include "../services/view_registry.h"
#include "../core/workbench_types.h"

namespace UI {

struct DefaultViewConfig {
    std::string plugin_id;
    std::string id;
    std::string title;
    ViewRenderer renderer;
};

struct EditorViewPlugin {
    std::string plugin_id;
    std::vector<ViewDefinition> primary_views;
    std::vector<std::pair<ActivityBarItem, std::string>> primary_bindings;
};

const std::vector<DefaultViewConfig>& GetDefaultViewConfigs();
void RegisterViewPlugin(const EditorViewPlugin& plugin);
std::string GetDefaultScenePluginId();
const std::string& GetPrimarySidebarGlobalPluginId();
const EditorViewPlugin* GetViewPluginByPluginId(const std::string& plugin_id);
ViewDefinition GetDefaultPrimaryViewForPlugin(const std::string& plugin_id, ActivityBarItem item);

void RequestCreateSceneTab(const std::string& plugin_id);
bool ConsumeCreateSceneTabRequest(std::string& plugin_id);

void SetExplorerRootPath(const std::string& root_path);
const std::string& GetExplorerRootPath();

// Training panel (sidebar "Training" activity): selected dataset folder + configured project path
std::string GetTrainingSelectedDataFolderPath();
std::string GetTrainingConfigProjectPath();

} // namespace UI
