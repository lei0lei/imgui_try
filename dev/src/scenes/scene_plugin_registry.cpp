#include "scene_plugin_registry.h"

#include <algorithm>
#include <cctype>
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

std::unordered_map<std::string, SceneCanvasRenderer>& GlobalRenderers()
{
    static std::unordered_map<std::string, SceneCanvasRenderer> renderers;
    return renderers;
}

std::unordered_map<std::string, SceneViewContributions>& GlobalViews()
{
    static std::unordered_map<std::string, SceneViewContributions> views;
    return views;
}

bool LoadPluginFromManifest(const std::filesystem::path& manifest_path, ScenePluginDescriptor& out)
{
    std::ifstream file(manifest_path);
    if (!file.is_open()) {
        return false;
    }

    std::unordered_map<std::string, std::string> kv;
    std::string line;
    while (std::getline(file, line)) {
        const std::string trimmed = Trim(line);
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        const size_t eq = trimmed.find('=');
        if (eq == std::string::npos) {
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
        Load();
        loaded_ = true;
    }
}

void ScenePluginRegistry::RegisterRenderer(const std::string& plugin_id, SceneCanvasRenderer renderer)
{
    if (plugin_id.empty() || renderer == nullptr) {
        return;
    }
    GlobalRenderers()[plugin_id] = renderer;
}

void ScenePluginRegistry::RegisterViews(const std::string& plugin_id, const SceneViewContributions& views)
{
    if (plugin_id.empty()) {
        return;
    }
    GlobalViews()[plugin_id] = views;
}

void ScenePluginRegistry::Load()
{
    plugins_.clear();
    renderers_.clear();
    views_.clear();

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
            if (LoadPluginFromManifest(manifest, plugin)) {
                auto it = GlobalRenderers().find(plugin.id);
                if (it != GlobalRenderers().end()) {
                    renderers_[plugin.id] = it->second;
                }
                auto vit = GlobalViews().find(plugin.id);
                if (vit != GlobalViews().end()) {
                    views_[plugin.id] = vit->second;
                }
                plugins_.push_back(std::move(plugin));
            }
        }
    }

    for (const auto& plugin : plugins_) {
        auto it = GlobalRenderers().find(plugin.id);
        if (it != GlobalRenderers().end()) {
            renderers_[plugin.id] = it->second;
        }
        auto vit = GlobalViews().find(plugin.id);
        if (vit != GlobalViews().end()) {
            views_[plugin.id] = vit->second;
        }
    }

    std::sort(plugins_.begin(), plugins_.end(), [](const ScenePluginDescriptor& a, const ScenePluginDescriptor& b) {
        return a.name < b.name;
    });
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

const SceneViewContributions* ScenePluginRegistry::GetViews(const std::string& plugin_id) const
{
    auto it = views_.find(plugin_id);
    if (it == views_.end()) {
        return nullptr;
    }
    return &it->second;
}

} // namespace Scenes
