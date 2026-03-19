#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

struct EditorTab {
    uint64_t id = 0;
    std::string name;
    std::string path;
    bool modified = false;
    bool active = false;
    std::string scene_plugin_id;
    std::unordered_map<std::string, std::string> scene_ui_state;
};
