/**
 * @file view_registry_defaults_config.cpp
 * @brief 默认视图注册表组件的配置和设置
 * @author Your Name
 * @date 2026-02-05
 */

#include "view_registry_defaults_config.h"
#include "imgui.h"
#include "../workbench/workbench_config.h"
#include "../algorithms/node_graph_executor.h"
#include <algorithm>
#include <utility>

namespace UI {

namespace {

void DrawPlaceholderHeader(const char* title, const ImVec4& color)
{
    ImGui::TextColored(color, "%s", title);
    ImGui::Separator();
    ImGui::Spacing();
}

void DrawEmptyState(const char* message)
{
    ImGui::TextWrapped("%s", message);
}

void RenderExplorer(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("EXPLORER", colors.primary_sidebar_text_dim);
    ImGui::Text("No folder opened");
    ImGui::Spacing();
    ImGui::Button("Open Folder");
}

void RenderSearch(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("SEARCH", colors.primary_sidebar_text_dim);
    static char search_buf[256] = "";
    ImGui::InputTextWithHint("##search", "Search...", search_buf, sizeof(search_buf));
    ImGui::Spacing();
    ImGui::Text("Search across files");
}

void RenderDebug(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("RUN AND DEBUG", colors.primary_sidebar_text_dim);
    DrawEmptyState("No configurations");
}

void RenderExtensions(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("EXTENSIONS", colors.primary_sidebar_text_dim);
    DrawEmptyState("Extension marketplace");
}

void RenderNodeLibrary(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("NODE LIBRARY", colors.primary_sidebar_text_dim);
    bool can_add = active_tab && active_tab->scene_type == SceneType::NodeEditor;

    auto draw_item = [&](const char* label) {
        if (ImGui::Selectable(label, false)) {
            if (can_add) {
                active_tab->pending_node_type = label;
                active_tab->request_add_node_from_library = true;
            }
        }
    };

    ImGui::Text("Math");
    draw_item("Gain");
    draw_item("Test");
    ImGui::Spacing();
    ImGui::Text("Logic");
    draw_item("If");
    draw_item("Switch");
}

void RenderNodeAssets(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("NODE ASSETS", colors.primary_sidebar_text_dim);
    DrawEmptyState("No node assets available");
}

void RenderSceneHierarchy(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("HIERARCHY", colors.primary_sidebar_text_dim);
    if (active_tab) {
        ImGui::Text("Scene: %s", active_tab->name.c_str());
    }
    ImGui::BulletText("Camera");
    ImGui::BulletText("Light");
    ImGui::BulletText("Root");
}

void RenderSceneAssets(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("ASSETS", colors.primary_sidebar_text_dim);
    DrawEmptyState("No assets loaded");
}

void RenderOutline(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("OUTLINE", colors.secondary_sidebar_text);
    if (active_tab) {
        ImGui::Text("Active: %s", active_tab->name.c_str());
        ImGui::BulletText("Root");
        ImGui::BulletText("Node A");
        ImGui::BulletText("Node B");
    } else {
        DrawEmptyState("No outline available");
    }
}

void RenderNodeOutline(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab)
{
    RenderOutline(content_min, content_max, active_tab);
    ImGui::Spacing();
    ImGui::Text("Node Editor Outline");
}

void RenderSceneOutline(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab)
{
    RenderOutline(content_min, content_max, active_tab);
    ImGui::Spacing();
    ImGui::Text("Scene Outline");
}

void RenderProperties(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("PROPERTIES", colors.secondary_sidebar_text);
    if (active_tab) {
        ImGui::Text("Selection: %s", active_tab->name.c_str());
    }
    DrawEmptyState("Select an element to inspect its properties.");
}

void RenderNodeProperties(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab)
{
    RenderProperties(content_min, content_max, active_tab);
    ImGui::Spacing();
    ImGui::Text("Node Properties");

    if (!active_tab || active_tab->scene_type != SceneType::NodeEditor) {
        return;
    }

    EditorTab::Node* selected = nullptr;
    if (active_tab->selected_node_id != 0) {
        for (auto& node : active_tab->nodes) {
            if (node.id == active_tab->selected_node_id) {
                selected = &node;
                break;
            }
        }
    }

    if (!selected) {
        ImGui::TextColored(GetWorkbenchTheme().colors.secondary_sidebar_text, "No node selected");
        return;
    }

    ImGui::Separator();
    ImGui::Text("Name: %s", selected->title.c_str());
    ImGui::Text("Type: %s", selected->type.empty() ? "Generic" : selected->type.c_str());

    ImGui::Spacing();
    ImGui::Text("Inputs:");
    for (const auto& input : selected->inputs) {
        ImGui::BulletText("%s", input.name.c_str());
    }
    ImGui::Text("Outputs:");
    for (const auto& output : selected->outputs) {
        ImGui::BulletText("%s", output.name.c_str());
    }

    ImGui::Spacing();
    ImGui::Text("Lua Script (run(inputs))");

    auto input_text_multiline = [](const char* label, std::string* str, const ImVec2& size) -> bool {
        if (str->capacity() < 1024) {
            str->reserve(1024);
        }
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize;
        auto callback = [](ImGuiInputTextCallbackData* data) -> int {
            if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
                auto* s = static_cast<std::string*>(data->UserData);
                s->resize(static_cast<size_t>(data->BufTextLen));
                data->Buf = s->data();
            }
            return 0;
        };
        if (str->empty()) {
            str->resize(1);
            (*str)[0] = '\0';
        }
        return ImGui::InputTextMultiline(label, str->data(), str->capacity() + 1, size, flags, callback, str);
    };

    const std::string default_script = NodeGraph::GetDefaultNodeScript(selected->type.empty() ? "Generic" : selected->type);
    if (selected->script.empty()) {
        selected->script = default_script;
    }
    input_text_multiline("##node_script", &selected->script, ImVec2(0, 140));
    if (ImGui::Button("Reset to Default")) {
        selected->script = default_script;
    }
}

void RenderSceneProperties(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab)
{
    RenderProperties(content_min, content_max, active_tab);
    ImGui::Spacing();
    ImGui::Text("Scene Properties");
}

void RenderPanelOutput(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    if (active_tab && active_tab->scene_type == SceneType::NodeEditor) {
        if (ImGui::Button("Run")) {
            auto result = NodeGraph::ExecuteGraph(*active_tab, false);
            active_tab->node_exec_last_ok = result.success;
            active_tab->node_exec_last_parallel = result.parallel;
            active_tab->node_exec_last_ms = result.duration_ms;
            active_tab->node_exec_last_error = result.error;
            active_tab->node_exec_log = std::move(result.log);
            active_tab->node_exec_outputs = std::move(result.outputs);
            if (active_tab->node_exec_log.empty()) {
                active_tab->node_exec_log.push_back(result.success ? "Execution finished." : "Execution failed.");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Run Parallel")) {
            auto result = NodeGraph::ExecuteGraph(*active_tab, true);
            active_tab->node_exec_last_ok = result.success;
            active_tab->node_exec_last_parallel = result.parallel;
            active_tab->node_exec_last_ms = result.duration_ms;
            active_tab->node_exec_last_error = result.error;
            active_tab->node_exec_log = std::move(result.log);
            active_tab->node_exec_outputs = std::move(result.outputs);
            if (active_tab->node_exec_log.empty()) {
                active_tab->node_exec_log.push_back(result.success ? "Execution finished." : "Execution failed.");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Log")) {
            active_tab->node_exec_log.clear();
            active_tab->node_exec_outputs.clear();
            active_tab->node_exec_last_error.clear();
        }

        ImGui::Separator();
        ImGui::Text("Last run: %s", active_tab->node_exec_last_ok ? "OK" : "Error");
        ImGui::Text("Mode: %s", active_tab->node_exec_last_parallel ? "Parallel" : "Single");
        ImGui::Text("Duration: %.2f ms", active_tab->node_exec_last_ms);
        if (!active_tab->node_exec_last_ok && !active_tab->node_exec_last_error.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "%s", active_tab->node_exec_last_error.c_str());
        }

        if (!active_tab->node_exec_outputs.empty()) {
            ImGui::Spacing();
            ImGui::Text("Outputs:");
            for (const auto& entry : active_tab->node_exec_outputs) {
                const int node_id = entry.first;
                const auto& outputs = entry.second;
                std::string label = "Node " + std::to_string(node_id);
                if (ImGui::TreeNode(label.c_str())) {
                    for (size_t i = 0; i < outputs.size(); ++i) {
                        ImGui::BulletText("Out%zu = %s", i + 1, outputs[i].c_str());
                    }
                    ImGui::TreePop();
                }
            }
        }

        ImGui::Spacing();
        ImGui::Text("Log:");
        ImGui::BeginChild("node_exec_log", ImVec2(0, 0), true);
        if (active_tab->node_exec_log.empty()) {
            ImGui::TextColored(colors.panel_text, "No logs yet.");
        } else {
            for (const auto& line : active_tab->node_exec_log) {
                ImGui::TextWrapped("%s", line.c_str());
            }
        }
        ImGui::EndChild();
        return;
    }

    ImGui::TextColored(colors.panel_text, "Build completed successfully");
}

void RenderPanelProblems(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "No problems detected");
}

void RenderPanelTerminal(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_terminal_prompt, "$ ");
    ImGui::SameLine();
    ImGui::TextColored(colors.panel_text, "Ready");
}

void RenderPanelConsole(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "Console output goes here.");
}

void RenderPanelDebug(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "Debug console");
}

void RenderPanelTimeline(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "Timeline is empty.");
}

void RenderPanelProfiler(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "Profiler idle.");
}

} // namespace

namespace {

std::vector<EditorViewPlugin>& PluginRegistry()
{
    static std::vector<EditorViewPlugin> plugins;
    return plugins;
}

void EnsureDefaultPlugins()
{
    auto& plugins = PluginRegistry();
    if (!plugins.empty())
        return;

    plugins.push_back({
        SceneType::Scene2D,
        {
            { "explorer", "Explorer", RenderExplorer },
            { "search", "Search", RenderSearch },
            { "node", "Node", RenderSceneHierarchy },
            { "debug", "Debug", RenderDebug },
            { "extensions", "Extensions", RenderExtensions }
        },
        {
            { "outline", "Outline", RenderSceneOutline },
            { "properties", "Properties", RenderSceneProperties }
        },
        {
            { "problems", "PROBLEMS", RenderPanelProblems },
            { "output", "OUTPUT", RenderPanelOutput },
            { "debug", "DEBUG", RenderPanelDebug },
            { "terminal", "TERMINAL", RenderPanelTerminal }
        },
        {
            { ActivityBarItem::Explorer, "explorer" },
            { ActivityBarItem::Search, "search" },
            { ActivityBarItem::NodeEditor, "node" },
            { ActivityBarItem::Debug, "debug" },
            { ActivityBarItem::Extensions, "extensions" }
        },
        "outline",
        "output"
    });

    plugins.push_back({
        SceneType::Scene3D,
        {
            { "explorer", "Explorer", RenderExplorer },
            { "search", "Search", RenderSearch },
            { "node", "Node", RenderSceneHierarchy },
            { "debug", "Debug", RenderDebug },
            { "extensions", "Extensions", RenderExtensions }
        },
        {
            { "outline", "Outline", RenderSceneOutline },
            { "properties", "Properties", RenderSceneProperties }
        },
        {
            { "problems", "PROBLEMS", RenderPanelProblems },
            { "output", "OUTPUT", RenderPanelOutput },
            { "debug", "DEBUG", RenderPanelDebug },
            { "terminal", "TERMINAL", RenderPanelTerminal }
        },
        {
            { ActivityBarItem::Explorer, "explorer" },
            { ActivityBarItem::Search, "search" },
            { ActivityBarItem::NodeEditor, "node" },
            { ActivityBarItem::Debug, "debug" },
            { ActivityBarItem::Extensions, "extensions" }
        },
        "outline",
        "output"
    });

    plugins.push_back({
        SceneType::NodeEditor,
        {
            { "explorer", "Explorer", RenderExplorer },
            { "search", "Search", RenderSearch },
            { "node", "Node", RenderNodeLibrary },
            { "debug", "Debug", RenderDebug },
            { "extensions", "Extensions", RenderExtensions }
        },
        {
            { "outline", "Outline", RenderNodeOutline },
            { "properties", "Properties", RenderNodeProperties }
        },
        {
            { "output", "OUTPUT", RenderPanelOutput }
        },
        {
            { ActivityBarItem::Explorer, "explorer" },
            { ActivityBarItem::Search, "search" },
            { ActivityBarItem::NodeEditor, "node" },
            { ActivityBarItem::Debug, "debug" },
            { ActivityBarItem::Extensions, "extensions" }
        },
        "outline",
        "output"
    });
}

const EditorViewPlugin* FindPlugin(SceneType mode)
{
    EnsureDefaultPlugins();
    const auto& plugins = PluginRegistry();
    for (const auto& plugin : plugins) {
        if (plugin.mode == mode) {
            return &plugin;
        }
    }
    return nullptr;
}

const ViewDefinition* FindViewById(const std::vector<ViewDefinition>& list, const std::string& id)
{
    for (const auto& view : list) {
        if (view.id == id) {
            return &view;
        }
    }
    return nullptr;
}

std::string GetPrimaryIdForActivity(const EditorViewPlugin& plugin, ActivityBarItem item)
{
    for (const auto& binding : plugin.primary_bindings) {
        if (binding.first == item) {
            return binding.second;
        }
    }
    return {};
}

}

const std::vector<DefaultViewConfig>& GetDefaultViewConfigs()
{
    static std::vector<DefaultViewConfig> configs;
    if (!configs.empty())
        return configs;

    EnsureDefaultPlugins();
    for (const auto& plugin : PluginRegistry()) {
        for (const auto& view : plugin.primary_views) {
            configs.push_back({ plugin.mode, ViewContainer::PrimarySidebar, view.id.c_str(), view.title.c_str(), view.renderer });
        }
        for (const auto& view : plugin.secondary_views) {
            configs.push_back({ plugin.mode, ViewContainer::SecondarySidebar, view.id.c_str(), view.title.c_str(), view.renderer });
        }
        for (const auto& view : plugin.panel_views) {
            configs.push_back({ plugin.mode, ViewContainer::Panel, view.id.c_str(), view.title.c_str(), view.renderer });
        }
    }

    return configs;
}

void RegisterViewPlugin(const EditorViewPlugin& plugin)
{
    EnsureDefaultPlugins();
    auto& plugins = PluginRegistry();
    for (auto& existing : plugins) {
        if (existing.mode == plugin.mode) {
            existing = plugin;
            return;
        }
    }
    plugins.push_back(plugin);
}

const EditorViewPlugin* GetViewPlugin(SceneType mode)
{
    return FindPlugin(mode);
}

ViewDefinition GetDefaultPrimaryView(SceneType mode, ActivityBarItem item)
{
    const auto* plugin = FindPlugin(mode);
    if (!plugin) {
        return { "empty", "Empty", nullptr };
    }
    const std::string id = GetPrimaryIdForActivity(*plugin, item);
    if (id.empty()) {
        return { "empty", "Empty", nullptr };
    }
    const auto* view = FindViewById(plugin->primary_views, id);
    if (!view) {
        return { "empty", "Empty", nullptr };
    }
    return { view->id, view->title, view->renderer };
}

ViewDefinition GetDefaultSecondaryView(SceneType mode)
{
    const auto* plugin = FindPlugin(mode);
    if (!plugin) {
        return { "empty", "Empty", nullptr };
    }
    const auto* view = FindViewById(plugin->secondary_views, plugin->default_secondary_id);
    if (!view) {
        return { "empty", "Empty", nullptr };
    }
    return { view->id, view->title, view->renderer };
}

ViewDefinition GetDefaultPanelView(SceneType mode)
{
    const auto* plugin = FindPlugin(mode);
    if (!plugin) {
        return { "empty", "Empty", nullptr };
    }
    const auto* view = FindViewById(plugin->panel_views, plugin->default_panel_id);
    if (!view) {
        return { "empty", "Empty", nullptr };
    }
    return { view->id, view->title, view->renderer };
}

} // namespace UI
