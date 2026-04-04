#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

#include "scene_plugin_interface.h"
#include "../core/title_bar_extension.h"
#include "../workbench/status_bar_extension.h"

namespace Scenes {

enum class ScenePluginDiagnosticSeverity {
    Warning,
    Error
};

struct ScenePluginLoadDiagnostic {
    ScenePluginDiagnosticSeverity severity = ScenePluginDiagnosticSeverity::Warning;
    std::string plugin_id;
    std::string manifest_path;
    std::string message;
};

class ScenePluginRegistry {
public:
    static ScenePluginRegistry& Instance();
    static void RegisterRenderer(const std::string& plugin_id, SceneCanvasRenderer renderer);
    static void RegisterTitleBarExtension(const std::string& plugin_id, SceneTitleBarExtensionRenderer renderer);
    static void RegisterStatusBarExtension(const std::string& plugin_id, SceneStatusBarExtensionRenderer renderer);
    static void RegisterTitleBarActionHandler(const std::string& plugin_id, std::function<void(EditorTab&, const std::string&)> handler);
    static void RegisterTitleBarExtensionWidthResolver(const std::string& plugin_id, std::function<float(float, const EditorTab*)> resolver);
    static void RegisterPrimarySidebarDebugDataProvider(const std::string& plugin_id, ScenePrimarySidebarDebugDataProvider provider);
    static void RegisterPrimarySidebarContributionRenderer(const std::string& plugin_id, ActivityBarItem item, ScenePrimarySidebarContributionRenderer renderer);

    void EnsureLoaded();
    void Reload();
    const std::vector<ScenePluginDescriptor>& GetPlugins() const { return plugins_; }
    const std::vector<ScenePluginLoadDiagnostic>& GetLoadDiagnostics() const { return diagnostics_; }
    const ScenePluginDescriptor* FindPluginById(const std::string& plugin_id) const;
    SceneCanvasRenderer GetRenderer(const std::string& plugin_id) const;
    const SceneTitleBarExtensionRenderer* GetTitleBarExtension(const std::string& plugin_id) const;
    const SceneStatusBarExtensionRenderer* GetStatusBarExtension(const std::string& plugin_id) const;
    const std::function<void(EditorTab&, const std::string&)>* GetTitleBarActionHandler(const std::string& plugin_id) const;
    const std::function<float(float, const EditorTab*)>* GetTitleBarExtensionWidthResolver(const std::string& plugin_id) const;
    const ScenePrimarySidebarDebugDataProvider* GetPrimarySidebarDebugDataProvider(const std::string& plugin_id) const;
    const ScenePrimarySidebarContributionRenderer* GetPrimarySidebarContributionRenderer(const std::string& plugin_id, ActivityBarItem item) const;

private:
    void Load();

    bool loaded_ = false;
    std::vector<ScenePluginDescriptor> plugins_;
    std::vector<ScenePluginLoadDiagnostic> diagnostics_;
};

} // namespace Scenes
