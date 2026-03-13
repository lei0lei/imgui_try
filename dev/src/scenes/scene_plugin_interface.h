#pragma once

#include <string>
#include <vector>

#include "../services/editor_area_service.h"

namespace Scenes {

using SceneCanvasRenderer = void(*)(ImVec2 content_min, ImVec2 content_max, EditorTab& tab);
using SceneViewRenderer = void(*)(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab);

struct SceneContributedView {
    std::string id;
    std::string title;
    SceneViewRenderer renderer = nullptr;
};

struct SceneViewContributions {
    std::vector<SceneContributedView> secondary_views;
    std::string default_secondary_id;

    std::vector<SceneContributedView> panel_views;
    std::string default_panel_id;
};

struct ScenePluginDescriptor {
    std::string id;
    std::string name;
    std::string info;
    std::string author;
    std::string icon_text;
    std::string folder;
};

class IScenePluginProvider {
public:
    virtual ~IScenePluginProvider() = default;
    virtual ScenePluginDescriptor GetDescriptor() const = 0;
};

} // namespace Scenes
