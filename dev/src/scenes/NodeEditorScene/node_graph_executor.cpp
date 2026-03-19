#include "node_graph_executor.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <future>
#include <optional>
#include <queue>
#include <sstream>
#include <unordered_set>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

namespace NodeGraph {

using Scenes::NodeEditor::Node;
using Scenes::NodeEditor::NodeEditorState;
using Scenes::NodeEditor::NodePortType;

namespace {

struct NodeValue {
    enum class Type {
        None,
        Bool,
        Int,
        Double,
        String
    };

    Type type = Type::None;
    bool b = false;
    int i = 0;
    double d = 0.0;
    std::string s;
};

NodeValue DefaultValueForPort(NodePortType type)
{
    NodeValue v;
    switch (type) {
        case NodePortType::Bool:
            v.type = NodeValue::Type::Bool;
            v.b = false;
            break;
        case NodePortType::Int:
            v.type = NodeValue::Type::Int;
            v.i = 0;
            break;
        case NodePortType::Float:
        case NodePortType::Double:
        case NodePortType::Vector:
        case NodePortType::Image:
            v.type = NodeValue::Type::Double;
            v.d = 0.0;
            break;
        case NodePortType::Text:
            v.type = NodeValue::Type::String;
            v.s = "";
            break;
        default:
            v.type = NodeValue::Type::Double;
            v.d = 0.0;
            break;
    }
    return v;
}

std::string NodeValueToString(const NodeValue& v)
{
    switch (v.type) {
        case NodeValue::Type::Bool:
            return v.b ? "true" : "false";
        case NodeValue::Type::Int:
            return std::to_string(v.i);
        case NodeValue::Type::Double: {
            std::ostringstream ss;
            ss << v.d;
            return ss.str();
        }
        case NodeValue::Type::String:
            return v.s;
        default:
            return "nil";
    }
}

void PushNodeValue(lua_State* L, const NodeValue& v)
{
    switch (v.type) {
        case NodeValue::Type::Bool:
            lua_pushboolean(L, v.b);
            break;
        case NodeValue::Type::Int:
            lua_pushinteger(L, v.i);
            break;
        case NodeValue::Type::Double:
            lua_pushnumber(L, v.d);
            break;
        case NodeValue::Type::String:
            lua_pushstring(L, v.s.c_str());
            break;
        default:
            lua_pushnil(L);
            break;
    }
}

NodeValue ReadNodeValue(lua_State* L, int idx)
{
    NodeValue v;
    if (lua_isboolean(L, idx)) {
        v.type = NodeValue::Type::Bool;
        v.b = lua_toboolean(L, idx) != 0;
    } else if (lua_isinteger(L, idx)) {
        v.type = NodeValue::Type::Int;
        v.i = static_cast<int>(lua_tointeger(L, idx));
    } else if (lua_isnumber(L, idx)) {
        v.type = NodeValue::Type::Double;
        v.d = lua_tonumber(L, idx);
    } else if (lua_isstring(L, idx)) {
        v.type = NodeValue::Type::String;
        v.s = lua_tostring(L, idx);
    } else {
        v.type = NodeValue::Type::None;
    }
    return v;
}

int LuaLog(lua_State* L)
{
    auto* log = static_cast<std::vector<std::string>*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!log) {
        return 0;
    }
    int n = lua_gettop(L);
    std::ostringstream ss;
    for (int i = 1; i <= n; ++i) {
        if (i > 1) ss << " ";
        if (lua_isstring(L, i)) {
            ss << lua_tostring(L, i);
        } else if (lua_isnumber(L, i)) {
            ss << lua_tonumber(L, i);
        } else if (lua_isboolean(L, i)) {
            ss << (lua_toboolean(L, i) ? "true" : "false");
        } else {
            ss << "<value>";
        }
    }
    log->push_back(ss.str());
    return 0;
}

std::string NormalizeNodeType(const Node& node)
{
    if (!node.type.empty()) {
        return node.type;
    }
    std::string type;
    for (char ch : node.title) {
        if (std::isspace(static_cast<unsigned char>(ch))) {
            break;
        }
        type.push_back(ch);
    }
    if (type.empty()) {
        type = "Generic";
    }
    return type;
}

struct InputLink {
    int from_node = 0;
    int from_port = -1;
};

struct NodeEvalResult {
    bool ok = true;
    std::string error;
    std::vector<NodeValue> outputs;
    std::vector<std::string> log;
};

NodeEvalResult EvaluateNodeLua(const Node& node,
                               const std::vector<NodeValue>& inputs,
                               const std::vector<std::string>& input_names)
{
    NodeEvalResult result;
    lua_State* L = luaL_newstate();
    if (!L) {
        result.ok = false;
        result.error = "Failed to create Lua state";
        return result;
    }
    luaL_openlibs(L);

    lua_pushlightuserdata(L, &result.log);
    lua_pushcclosure(L, LuaLog, 1);
    lua_setglobal(L, "log");

    lua_newtable(L);
    for (size_t i = 0; i < inputs.size(); ++i) {
        PushNodeValue(L, inputs[i]);
        lua_seti(L, -2, static_cast<lua_Integer>(i + 1));
        if (i < input_names.size()) {
            lua_pushstring(L, input_names[i].c_str());
            PushNodeValue(L, inputs[i]);
            lua_settable(L, -3);
        }
    }
    lua_setglobal(L, "inputs");

    std::string script = node.script.empty() ? GetDefaultNodeScript(NormalizeNodeType(node)) : node.script;
    if (luaL_dostring(L, script.c_str()) != LUA_OK) {
        result.ok = false;
        result.error = lua_tostring(L, -1);
        lua_close(L);
        return result;
    }

    lua_getglobal(L, "run");
    if (!lua_isfunction(L, -1)) {
        result.ok = false;
        result.error = "Lua script must define function run(inputs)";
        lua_close(L);
        return result;
    }

    lua_getglobal(L, "inputs");
    if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
        result.ok = false;
        result.error = lua_tostring(L, -1);
        lua_close(L);
        return result;
    }

    int output_count = static_cast<int>(node.outputs.size());
    result.outputs.resize(output_count);

    if (lua_istable(L, -1)) {
        for (int i = 0; i < output_count; ++i) {
            const std::string& name = node.outputs[i].name;
            bool got = false;
            if (!name.empty()) {
                lua_getfield(L, -1, name.c_str());
                if (!lua_isnil(L, -1)) {
                    result.outputs[i] = ReadNodeValue(L, -1);
                    got = true;
                }
                lua_pop(L, 1);
            }
            if (!got) {
                lua_geti(L, -1, i + 1);
                if (!lua_isnil(L, -1)) {
                    result.outputs[i] = ReadNodeValue(L, -1);
                    got = true;
                }
                lua_pop(L, 1);
            }
            if (!got) {
                result.outputs[i] = DefaultValueForPort(node.outputs[i].type);
            }
        }
    } else if (output_count == 1) {
        result.outputs[0] = ReadNodeValue(L, -1);
    } else {
        result.ok = false;
        result.error = "Lua run() must return a table or a single value";
        lua_close(L);
        return result;
    }

    lua_close(L);
    return result;
}

} // namespace

std::string GetDefaultNodeScript(const std::string& type)
{
    if (type == "Gain") {
        return "function run(inputs)\n  local v = inputs.In or inputs[1] or 0\n  return { Out = v }\nend\n";
    }
    if (type == "Test") {
        return "function run(inputs)\n  local a = inputs.In1 or inputs[1] or 0\n  local b = inputs.In2 or inputs[2] or 0\n  local c = inputs.In3 or inputs[3] or 0\n  local d = inputs.In4 or inputs[4] or 0\n  return { Out1 = a + b, Out2 = c + d, Out3 = a }\nend\n";
    }
    if (type == "If") {
        return "function run(inputs)\n  local cond = inputs.Cond or inputs[1]\n  local value = inputs.Value or inputs[2]\n  if cond then return { Out = value } else return { Out = nil } end\nend\n";
    }
    if (type == "Switch") {
        return "function run(inputs)\n  local a = inputs.A or inputs[1]\n  local b = inputs.B or inputs[2]\n  local sel = inputs.Sel or inputs[3]\n  if sel then return { Out = b } else return { Out = a } end\nend\n";
    }
    return "function run(inputs)\n  return { Out = inputs[1] }\nend\n";
}

ExecutionResult ExecuteGraph(const NodeEditorState& state, bool parallel)
{
    ExecutionResult result;
    result.parallel = parallel;

    auto start_time = std::chrono::high_resolution_clock::now();

    if (state.nodes.empty()) {
        result.log.push_back("No nodes to execute.");
        result.duration_ms = 0.0;
        return result;
    }

    std::unordered_map<int, const Node*> nodes;
    nodes.reserve(state.nodes.size());
    for (const auto& node : state.nodes) {
        nodes[node.id] = &node;
    }

    std::unordered_map<int, std::vector<InputLink>> input_links;
    std::unordered_map<int, std::vector<int>> adjacency;
    std::unordered_map<int, int> indegree;

    for (const auto& node : state.nodes) {
        indegree[node.id] = 0;
    }

    for (const auto& link : state.links) {
        auto from_it = nodes.find(link.from_node);
        auto to_it = nodes.find(link.to_node);
        if (from_it == nodes.end() || to_it == nodes.end()) {
            continue;
        }
        const auto& from_node = *from_it->second;
        const auto& to_node = *to_it->second;
        if (link.from_port < 0 || link.from_port >= static_cast<int>(from_node.outputs.size())) {
            continue;
        }
        if (link.to_port < 0 || link.to_port >= static_cast<int>(to_node.inputs.size())) {
            continue;
        }
        if (static_cast<int>(input_links[link.to_node].size()) <= link.to_port) {
            input_links[link.to_node].resize(link.to_port + 1);
        }
        input_links[link.to_node][link.to_port] = { link.from_node, link.from_port };
        adjacency[link.from_node].push_back(link.to_node);
    }

    for (const auto& [from, list] : adjacency) {
        for (int to : list) {
            indegree[to] += 1;
        }
    }

    std::vector<std::vector<int>> layers;
    std::vector<int> ready;
    for (const auto& [id, deg] : indegree) {
        if (deg == 0) {
            ready.push_back(id);
        }
    }

    std::unordered_map<int, int> indegree_work = indegree;
    while (!ready.empty()) {
        layers.push_back(ready);
        std::vector<int> next;
        for (int id : ready) {
            for (int to : adjacency[id]) {
                indegree_work[to] -= 1;
                if (indegree_work[to] == 0) {
                    next.push_back(to);
                }
            }
        }
        ready = std::move(next);
    }

    size_t total_nodes = 0;
    for (const auto& layer : layers) {
        total_nodes += layer.size();
    }
    if (total_nodes != state.nodes.size()) {
        result.success = false;
        result.error = "Graph contains cycles or disconnected nodes.";
        result.log.push_back(result.error);
        result.duration_ms = 0.0;
        return result;
    }

    std::unordered_map<int, std::vector<NodeValue>> outputs;

    auto eval_node = [&](int node_id) -> NodeEvalResult {
        NodeEvalResult eval;
        auto it = nodes.find(node_id);
        if (it == nodes.end()) {
            eval.ok = false;
            eval.error = "Node not found.";
            return eval;
        }
        const auto& node = *it->second;
        std::vector<NodeValue> inputs;
        std::vector<std::string> input_names;
        inputs.resize(node.inputs.size());
        input_names.reserve(node.inputs.size());

        for (size_t i = 0; i < node.inputs.size(); ++i) {
            input_names.push_back(node.inputs[i].name);
            NodeValue value = DefaultValueForPort(node.inputs[i].type);
            if (auto link_it = input_links.find(node.id); link_it != input_links.end()) {
                if (i < link_it->second.size()) {
                    const auto& link = link_it->second[i];
                    if (link.from_node != 0 && link.from_port >= 0) {
                        auto out_it = outputs.find(link.from_node);
                        if (out_it != outputs.end() && link.from_port < static_cast<int>(out_it->second.size())) {
                            value = out_it->second[link.from_port];
                        }
                    }
                }
            }
            inputs[i] = value;
        }

        eval = EvaluateNodeLua(node, inputs, input_names);
        return eval;
    };

    for (const auto& layer : layers) {
        if (parallel && layer.size() > 1) {
            std::vector<std::future<NodeEvalResult>> tasks;
            tasks.reserve(layer.size());
            for (int node_id : layer) {
                tasks.emplace_back(std::async(std::launch::async, [&, node_id]() {
                    return eval_node(node_id);
                }));
            }
            for (size_t i = 0; i < layer.size(); ++i) {
                int node_id = layer[i];
                NodeEvalResult eval = tasks[i].get();
                if (!eval.ok) {
                    result.success = false;
                    result.error = eval.error;
                    result.log.insert(result.log.end(), eval.log.begin(), eval.log.end());
                    result.log.push_back("Node " + std::to_string(node_id) + " error: " + eval.error);
                    auto end_time = std::chrono::high_resolution_clock::now();
                    result.duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
                    return result;
                }
                outputs[node_id] = eval.outputs;
                result.log.insert(result.log.end(), eval.log.begin(), eval.log.end());
            }
        } else {
            for (int node_id : layer) {
                NodeEvalResult eval = eval_node(node_id);
                if (!eval.ok) {
                    result.success = false;
                    result.error = eval.error;
                    result.log.insert(result.log.end(), eval.log.begin(), eval.log.end());
                    result.log.push_back("Node " + std::to_string(node_id) + " error: " + eval.error);
                    auto end_time = std::chrono::high_resolution_clock::now();
                    result.duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
                    return result;
                }
                outputs[node_id] = eval.outputs;
                result.log.insert(result.log.end(), eval.log.begin(), eval.log.end());
            }
        }
    }

    for (const auto& [node_id, out] : outputs) {
        std::vector<std::string> out_text;
        out_text.reserve(out.size());
        for (const auto& v : out) {
            out_text.push_back(NodeValueToString(v));
        }
        result.outputs[node_id] = std::move(out_text);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    result.duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    result.success = true;
    return result;
}

} // namespace NodeGraph
