#include "scene_plugin_registry.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

namespace Scenes {
namespace {

std::string Trim(const std::string& input)
{
    size_t begin = 0;
    while (begin < input.size() && std::isspace(static_cast<unsigned char>(input[begin]))) {
        ++begin;
    }

    size_t end = input.size();
    while (end > begin && std::isspace(static_cast<unsigned char>(input[end - 1]))) {
        --end;
    }

    return input.substr(begin, end - begin);
}

std::string ToLower(std::string value)
{
    for (char& c : value) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return value;
}

void AddDiagnostic(std::vector<ScenePluginLoadDiagnostic>& diagnostics,
    ScenePluginDiagnosticSeverity severity,
    const std::string& plugin_id,
    const std::filesystem::path& manifest_path,
    const std::string& message);

std::unordered_map<std::string, SceneCanvasRenderer>& GlobalRenderers()
{
    static std::unordered_map<std::string, SceneCanvasRenderer> renderers;
    return renderers;
}

std::unordered_map<std::string, SceneTitleBarExtensionRenderer>& GlobalTitleBarExtensions()
{
    static std::unordered_map<std::string, SceneTitleBarExtensionRenderer> extensions;
    return extensions;
}

std::unordered_map<std::string, SceneStatusBarExtensionRenderer>& GlobalStatusBarExtensions()
{
    static std::unordered_map<std::string, SceneStatusBarExtensionRenderer> extensions;
    return extensions;
}

std::unordered_map<std::string, std::function<void(EditorTab&, const std::string&)>>& GlobalTitleBarActionHandlers()
{
    static std::unordered_map<std::string, std::function<void(EditorTab&, const std::string&)>> handlers;
    return handlers;
}

std::unordered_map<std::string, std::function<float(float, const EditorTab*)>>& GlobalTitleBarExtensionWidthResolvers()
{
    static std::unordered_map<std::string, std::function<float(float, const EditorTab*)>> resolvers;
    return resolvers;
}

std::unordered_map<std::string, ScenePrimarySidebarDebugDataProvider>& GlobalPrimarySidebarDebugDataProviders()
{
    static std::unordered_map<std::string, ScenePrimarySidebarDebugDataProvider> providers;
    return providers;
}

std::unordered_map<std::string, std::unordered_map<int, ScenePrimarySidebarContributionRenderer>>& GlobalPrimarySidebarContributionRenderers()
{
    static std::unordered_map<std::string, std::unordered_map<int, ScenePrimarySidebarContributionRenderer>> renderers;
    return renderers;
}

void AddDiagnostic(std::vector<ScenePluginLoadDiagnostic>& diagnostics,
    ScenePluginDiagnosticSeverity severity,
    const std::string& plugin_id,
    const std::filesystem::path& manifest_path,
    const std::string& message)
{
    ScenePluginLoadDiagnostic diag{};
    diag.severity = severity;
    diag.plugin_id = plugin_id;
    diag.manifest_path = manifest_path.string();
    diag.message = message;
    diagnostics.push_back(std::move(diag));
}

bool LoadPluginFromManifest(const std::filesystem::path& manifest_path,
    ScenePluginDescriptor& out,
    std::vector<ScenePluginLoadDiagnostic>& diagnostics)
{
    std::ifstream file(manifest_path);
    if (!file.is_open()) {
        AddDiagnostic(diagnostics,
            ScenePluginDiagnosticSeverity::Error,
            out.id,
            manifest_path,
            "Failed to open scene.plugin manifest.");
        return false;
    }

    std::unordered_map<std::string, std::string> kv;
    std::string line;
    int line_number = 0;
    while (std::getline(file, line)) {
        ++line_number;
        const std::string trimmed = Trim(line);
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        const size_t eq = trimmed.find('=');
        if (eq == std::string::npos) {
            AddDiagnostic(diagnostics,
                ScenePluginDiagnosticSeverity::Warning,
                out.id,
                manifest_path,
                "Ignoring malformed line " + std::to_string(line_number) + " (missing '=')");
            continue;
        }

        const std::string key = ToLower(Trim(trimmed.substr(0, eq)));
        const std::string value = Trim(trimmed.substr(eq + 1));
        if (!key.empty()) {
            kv[key] = value;
        }
    }

    out.folder = manifest_path.parent_path().filename().string();
    out.id = kv.count("id") ? kv["id"] : out.folder;
    out.name = kv.count("name") ? kv["name"] : out.folder;
    out.info = kv.count("info") ? kv["info"] : "No description";
    out.author = kv.count("author") ? kv["author"] : "unknown";
    out.icon_text = kv.count("icon") ? kv["icon"] : "SC";

    if (!kv.count("id")) {
        AddDiagnostic(diagnostics,
            ScenePluginDiagnosticSeverity::Warning,
            out.id,
            manifest_path,
            "Missing required key 'id'; using scene folder name as fallback.");
    }
    if (!kv.count("name")) {
        AddDiagnostic(diagnostics,
            ScenePluginDiagnosticSeverity::Warning,
            out.id,
            manifest_path,
            "Missing key 'name'; using scene folder name as fallback.");
    }
    if (!kv.count("info")) {
        AddDiagnostic(diagnostics,
            ScenePluginDiagnosticSeverity::Warning,
            out.id,
            manifest_path,
            "Missing key 'info'; using default description.");
    }
    if (!kv.count("author")) {
        AddDiagnostic(diagnostics,
            ScenePluginDiagnosticSeverity::Warning,
            out.id,
            manifest_path,
            "Missing key 'author'; using default value.");
    }
    if (!kv.count("icon")) {
        AddDiagnostic(diagnostics,
            ScenePluginDiagnosticSeverity::Warning,
            out.id,
            manifest_path,
            "Missing key 'icon'; using default value.");
    }

    return true;
}

std::filesystem::path SceneRootPath()
{
#ifdef IMGUITRY_SCENES_ROOT
    return std::filesystem::path(IMGUITRY_SCENES_ROOT);
#else
    return std::filesystem::path("src/scenes");
#endif
}

} // namespace

ScenePluginRegistry& ScenePluginRegistry::Instance()
{
    static ScenePluginRegistry instance;
    return instance;
}

void ScenePluginRegistry::EnsureLoaded()
{
    if (!loaded_) {
        Reload();
    }
}

void ScenePluginRegistry::Reload()
{
    Load();
    loaded_ = true;
}

void ScenePluginRegistry::RegisterRenderer(const std::string& plugin_id, SceneCanvasRenderer renderer)
{
    if (plugin_id.empty() || renderer == nullptr) {
        return;
    }
    GlobalRenderers()[plugin_id] = renderer;

    ScenePluginRegistry& registry = Instance();
    if (registry.loaded_) {
        registry.renderers_[plugin_id] = renderer;
    }
}

void ScenePluginRegistry::RegisterTitleBarExtension(const std::string& plugin_id, SceneTitleBarExtensionRenderer renderer)
{
    if (plugin_id.empty() || !renderer) {
        return;
    }

    GlobalTitleBarExtensions()[plugin_id] = std::move(renderer);

    ScenePluginRegistry& registry = Instance();
    if (registry.loaded_) {
        auto it = GlobalTitleBarExtensions().find(plugin_id);
        if (it != GlobalTitleBarExtensions().end()) {
            registry.title_bar_extensions_[plugin_id] = it->second;
        }
    }
}

void ScenePluginRegistry::RegisterStatusBarExtension(const std::string& plugin_id, SceneStatusBarExtensionRenderer renderer)
{
    if (plugin_id.empty() || !renderer) {
        return;
    }

    GlobalStatusBarExtensions()[plugin_id] = std::move(renderer);

    ScenePluginRegistry& registry = Instance();
    if (registry.loaded_) {
        auto it = GlobalStatusBarExtensions().find(plugin_id);
        if (it != GlobalStatusBarExtensions().end()) {
            registry.status_bar_extensions_[plugin_id] = it->second;
        }
    }
}

void ScenePluginRegistry::RegisterTitleBarActionHandler(const std::string& plugin_id, std::function<void(EditorTab&, const std::string&)> handler)
{
    if (plugin_id.empty() || !handler) {
        return;
    }

    GlobalTitleBarActionHandlers()[plugin_id] = std::move(handler);

    ScenePluginRegistry& registry = Instance();
    if (registry.loaded_) {
        auto it = GlobalTitleBarActionHandlers().find(plugin_id);
        if (it != GlobalTitleBarActionHandlers().end()) {
            registry.title_bar_action_handlers_[plugin_id] = it->second;
        }
    }
}

void ScenePluginRegistry::RegisterTitleBarExtensionWidthResolver(const std::string& plugin_id, std::function<float(float, const EditorTab*)> resolver)
{
    if (plugin_id.empty() || !resolver) {
        return;
    }

    GlobalTitleBarExtensionWidthResolvers()[plugin_id] = std::move(resolver);

    ScenePluginRegistry& registry = Instance();
    if (registry.loaded_) {
        auto it = GlobalTitleBarExtensionWidthResolvers().find(plugin_id);
        if (it != GlobalTitleBarExtensionWidthResolvers().end()) {
            registry.title_bar_extension_width_resolvers_[plugin_id] = it->second;
        }
    }
}

void ScenePluginRegistry::RegisterPrimarySidebarDebugDataProvider(const std::string& plugin_id, ScenePrimarySidebarDebugDataProvider provider)
{
    if (plugin_id.empty() || !provider) {
        return;
    }

    GlobalPrimarySidebarDebugDataProviders()[plugin_id] = std::move(provider);

    ScenePluginRegistry& registry = Instance();
    if (registry.loaded_) {
        auto it = GlobalPrimarySidebarDebugDataProviders().find(plugin_id);
        if (it != GlobalPrimarySidebarDebugDataProviders().end()) {
            registry.primary_sidebar_debug_data_providers_[plugin_id] = it->second;
        }
    }
}

void ScenePluginRegistry::RegisterPrimarySidebarContributionRenderer(const std::string& plugin_id, ActivityBarItem item, ScenePrimarySidebarContributionRenderer renderer)
{
    if (plugin_id.empty() || !renderer) {
        return;
    }

    GlobalPrimarySidebarContributionRenderers()[plugin_id][static_cast<int>(item)] = std::move(renderer);

    ScenePluginRegistry& registry = Instance();
    if (registry.loaded_) {
        auto it = GlobalPrimarySidebarContributionRenderers().find(plugin_id);
        if (it != GlobalPrimarySidebarContributionRenderers().end()) {
            registry.primary_sidebar_contribution_renderers_[plugin_id] = it->second;
        }
    }
}

void ScenePluginRegistry::Load()
{
    plugins_.clear();
    renderers_.clear();
    title_bar_extensions_.clear();
    status_bar_extensions_.clear();
    title_bar_action_handlers_.clear();
    title_bar_extension_width_resolvers_.clear();
    primary_sidebar_debug_data_providers_.clear();
    primary_sidebar_contribution_renderers_.clear();
    diagnostics_.clear();

    std::unordered_map<std::string, std::filesystem::path> seen_plugin_ids;

    std::error_code ec;
    const std::filesystem::path root = SceneRootPath();
    if (std::filesystem::exists(root, ec) && std::filesystem::is_directory(root, ec)) {
        for (const auto& dir : std::filesystem::directory_iterator(root, ec)) {
            if (ec || !dir.is_directory()) {
                continue;
            }

            const auto manifest = dir.path() / "scene.plugin";
            if (!std::filesystem::exists(manifest, ec)) {
                continue;
            }

            ScenePluginDescriptor plugin;
            if (LoadPluginFromManifest(manifest, plugin, diagnostics_)) {
                const auto duplicate = seen_plugin_ids.find(plugin.id);
                if (duplicate != seen_plugin_ids.end()) {
                    AddDiagnostic(diagnostics_,
                        ScenePluginDiagnosticSeverity::Error,
                        plugin.id,
                        manifest,
                        "Duplicate plugin id detected; first manifest: " + duplicate->second.string());
                    continue;
                }

                seen_plugin_ids[plugin.id] = manifest;
                plugins_.push_back(std::move(plugin));
            }
        }
    }

    for (const auto& plugin : plugins_) {
        auto it = GlobalRenderers().find(plugin.id);
        if (it != GlobalRenderers().end()) {
            renderers_[plugin.id] = it->second;
        } else {
            AddDiagnostic(diagnostics_,
                ScenePluginDiagnosticSeverity::Warning,
                plugin.id,
                root / plugin.folder / "scene.plugin",
                "No renderer registered for plugin id. UI will show fallback canvas.");
        }

        auto ext_it = GlobalTitleBarExtensions().find(plugin.id);
        if (ext_it != GlobalTitleBarExtensions().end()) {
            title_bar_extensions_[plugin.id] = ext_it->second;
        }

        auto status_ext_it = GlobalStatusBarExtensions().find(plugin.id);
        if (status_ext_it != GlobalStatusBarExtensions().end()) {
            status_bar_extensions_[plugin.id] = status_ext_it->second;
        }

        auto action_it = GlobalTitleBarActionHandlers().find(plugin.id);
        if (action_it != GlobalTitleBarActionHandlers().end()) {
            title_bar_action_handlers_[plugin.id] = action_it->second;
        }

        auto width_it = GlobalTitleBarExtensionWidthResolvers().find(plugin.id);
        if (width_it != GlobalTitleBarExtensionWidthResolvers().end()) {
            title_bar_extension_width_resolvers_[plugin.id] = width_it->second;
        }

        auto debug_provider_it = GlobalPrimarySidebarDebugDataProviders().find(plugin.id);
        if (debug_provider_it != GlobalPrimarySidebarDebugDataProviders().end()) {
            primary_sidebar_debug_data_providers_[plugin.id] = debug_provider_it->second;
        }

        auto contrib_it = GlobalPrimarySidebarContributionRenderers().find(plugin.id);
        if (contrib_it != GlobalPrimarySidebarContributionRenderers().end()) {
            primary_sidebar_contribution_renderers_[plugin.id] = contrib_it->second;
        }
    }

    std::sort(plugins_.begin(), plugins_.end(), [](const ScenePluginDescriptor& a, const ScenePluginDescriptor& b) {
        return a.name < b.name;
    });

    for (const auto& diag : diagnostics_) {
        const char* severity = (diag.severity == ScenePluginDiagnosticSeverity::Error) ? "ERROR" : "WARN";
        std::fprintf(stderr,
            "[scene-plugin][%s] plugin='%s' manifest='%s' %s\n",
            severity,
            diag.plugin_id.c_str(),
            diag.manifest_path.c_str(),
            diag.message.c_str());
    }
}

const ScenePluginDescriptor* ScenePluginRegistry::FindPluginById(const std::string& plugin_id) const
{
    for (const auto& plugin : plugins_) {
        if (plugin.id == plugin_id) {
            return &plugin;
        }
    }
    return nullptr;
}

SceneCanvasRenderer ScenePluginRegistry::GetRenderer(const std::string& plugin_id) const
{
    auto it = renderers_.find(plugin_id);
    if (it == renderers_.end()) {
        return nullptr;
    }
    return it->second;
}

const SceneTitleBarExtensionRenderer* ScenePluginRegistry::GetTitleBarExtension(const std::string& plugin_id) const
{
    auto it = title_bar_extensions_.find(plugin_id);
    if (it == title_bar_extensions_.end()) {
        return nullptr;
    }
    return &it->second;
}

const SceneStatusBarExtensionRenderer* ScenePluginRegistry::GetStatusBarExtension(const std::string& plugin_id) const
{
    auto it = status_bar_extensions_.find(plugin_id);
    if (it == status_bar_extensions_.end()) {
        return nullptr;
    }
    return &it->second;
}

const std::function<void(EditorTab&, const std::string&)>* ScenePluginRegistry::GetTitleBarActionHandler(const std::string& plugin_id) const
{
    auto it = title_bar_action_handlers_.find(plugin_id);
    if (it == title_bar_action_handlers_.end()) {
        return nullptr;
    }
    return &it->second;
}

const std::function<float(float, const EditorTab*)>* ScenePluginRegistry::GetTitleBarExtensionWidthResolver(const std::string& plugin_id) const
{
    auto it = title_bar_extension_width_resolvers_.find(plugin_id);
    if (it == title_bar_extension_width_resolvers_.end()) {
        return nullptr;
    }
    return &it->second;
}

const ScenePrimarySidebarDebugDataProvider* ScenePluginRegistry::GetPrimarySidebarDebugDataProvider(const std::string& plugin_id) const
{
    auto it = primary_sidebar_debug_data_providers_.find(plugin_id);
    if (it == primary_sidebar_debug_data_providers_.end()) {
        return nullptr;
    }
    return &it->second;
}

const ScenePrimarySidebarContributionRenderer* ScenePluginRegistry::GetPrimarySidebarContributionRenderer(const std::string& plugin_id, ActivityBarItem item) const
{
    auto plugin_it = primary_sidebar_contribution_renderers_.find(plugin_id);
    if (plugin_it == primary_sidebar_contribution_renderers_.end()) {
        return nullptr;
    }
    const auto item_it = plugin_it->second.find(static_cast<int>(item));
    if (item_it == plugin_it->second.end()) {
        return nullptr;
    }
    return &item_it->second;
}

} // namespace Scenes
