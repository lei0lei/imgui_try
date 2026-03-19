#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "node_editor_state.h"

namespace NodeGraph {

struct ExecutionResult {
    bool success = true;
    bool parallel = false;
    double duration_ms = 0.0;
    std::string error;
    std::vector<std::string> log;
    std::unordered_map<int, std::vector<std::string>> outputs;
};

// Get default Lua script for a node type.
std::string GetDefaultNodeScript(const std::string& type);

// Execute node graph for the given node scene state.
ExecutionResult ExecuteGraph(const Scenes::NodeEditor::NodeEditorState& state, bool parallel);

} // namespace NodeGraph
