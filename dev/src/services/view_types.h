#pragma once

#include <functional>
#include <string>

#include "imgui.h"

struct EditorTab;

using ViewRenderer = std::function<void(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab)>;

struct ViewDefinition {
    std::string id;
    std::string title;
    ViewRenderer renderer;
};
