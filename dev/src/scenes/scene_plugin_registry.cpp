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

bool TryParseMode(const std::string& text, SceneType& mode)
{
    const std::string value = ToLower(Trim(text));
    if (value == "scene2d" || value == "2d" || value == "2dscene") {
        mode = SceneType::Scene2D;
        return true;
    }
    if (value == "scene3d" || value == "3d" || value == "3dscene") {
        mode = SceneType::Scene3D;
        return true;
    }
    if (value == "nodeeditor" || value == "node" || value == "nodegraph") {
        mode = SceneType::NodeEditor;
        return true;
    }
    return false;
}

void AddBuiltInFallback(std::vector<ScenePluginDescriptor>& plugins)
{
    plugins.push_back({"scene.2d", "2D Scene Tools", "Create and inspect 2D scene workflows for experiments.", "imgui-try team", "2D", SceneType::Scene2D, "fallback"});
    plugins.push_back({"scene.3d", "3D Scene Tools", "Open a realtime 3D workspace with rendering playground.", "imgui-try team", "3D", SceneType::Scene3D, "fallback"});
    plugins.push_back({"scene.node", "Node Editor Toolkit", "Build node graphs and execute scripts in output panel.", "imgui-try team", "NG", SceneType::NodeEditor, "fallback"});
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

    const std::string mode_text = kv.count("mode") ? kv["mode"] : "";
    if (!TryParseMode(mode_text, out.mode)) {
        return false;
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
        Load();
        loaded_ = true;
    }
}

void ScenePluginRegistry::Load()
{
    plugins_.clear();

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
                plugins_.push_back(std::move(plugin));
            }
        }
    }

    if (plugins_.empty()) {
        AddBuiltInFallback(plugins_);
    }

    std::sort(plugins_.begin(), plugins_.end(), [](const ScenePluginDescriptor& a, const ScenePluginDescriptor& b) {
        return a.name < b.name;
    });
}

} // namespace Scenes
