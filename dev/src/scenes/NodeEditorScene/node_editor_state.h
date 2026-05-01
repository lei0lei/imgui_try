#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>
#include <vector>

#include "imgui.h"
#include "../../services/editor_area_service.h"

namespace Scenes::NodeEditor {

enum class NodePortType {
    Generic,
    Bool,
    Int,
    Float,
    Double,
    Vector,
    Image,
    Text
};

struct NodePort {
    std::string name;
    NodePortType type = NodePortType::Generic;
};

struct Node {
    int id = 0;
    std::string type;
    std::string title;
    ImVec2 pos = ImVec2(0.0f, 0.0f);
    ImVec2 size = ImVec2(160.0f, 80.0f);
    std::vector<NodePort> inputs;
    std::vector<NodePort> outputs;
    std::string script;
};

struct Link {
    int from_node = 0;
    int from_port = -1;
    int to_node = 0;
    int to_port = -1;
};

struct NodeEditorState {
    ImVec2 node_canvas_pan = ImVec2(0.0f, 0.0f);
    float node_canvas_zoom = 1.0f;

    bool node_exec_last_ok = true;
    bool node_exec_last_parallel = false;
    double node_exec_last_ms = 0.0;
    std::string node_exec_last_error;
    std::vector<std::string> node_exec_log;
    std::unordered_map<int, std::vector<std::string>> node_exec_outputs;

    std::vector<Node> nodes;
    std::vector<Link> links;
    int node_next_id = 1;
    int selected_node_id = 0;
    int dragging_node_id = 0;
    ImVec2 dragging_node_offset = ImVec2(0.0f, 0.0f);
    bool linking = false;
    int link_from_node_id = 0;
    int link_from_port = -1;
    bool request_add_node_from_library = false;
    std::string pending_node_type;
};

NodeEditorState& GetState(EditorTab& tab);
const NodeEditorState& GetState(const EditorTab& tab);

inline std::unordered_map<uint64_t, NodeEditorState>& StateStore()
{
    static std::unordered_map<uint64_t, NodeEditorState> store;
    return store;
}

inline NodeEditorState& GetState(EditorTab& tab)
{
    return StateStore()[tab.id];
}

inline const NodeEditorState& GetState(const EditorTab& tab)
{
    auto& store = StateStore();
    auto it = store.find(tab.id);
    if (it == store.end()) {
        it = store.emplace(tab.id, NodeEditorState{}).first;
    }
    return it->second;
}

} // namespace Scenes::NodeEditor
