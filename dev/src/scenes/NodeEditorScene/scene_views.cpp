#include "scene_views.h"

#include "node_graph_executor.h"
#include "../../workbench/workbench_config.h"

#include <string>

namespace Scenes::NodeEditorViews {

namespace {

void DrawHeader(const char* title, const ImVec4& color)
{
    ImGui::TextColored(color, "%s", title);
    ImGui::Separator();
    ImGui::Spacing();
}

void DrawOutlineBase(EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawHeader("OUTLINE", colors.secondary_sidebar_text);
    if (active_tab) {
        ImGui::Text("Active: %s", active_tab->name.c_str());
        ImGui::BulletText("Root");
        ImGui::BulletText("Node A");
        ImGui::BulletText("Node B");
    } else {
        ImGui::TextWrapped("No outline available");
    }
}

bool InputTextMultilineStdString(const char* label, std::string* str, const ImVec2& size)
{
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
}

} // namespace

void RenderLibrary(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawHeader("NODE LIBRARY", colors.primary_sidebar_text_dim);
    const bool can_add = active_tab && active_tab->scene_type == SceneType::NodeEditor;

    auto draw_item = [&](const char* label) {
        if (ImGui::Selectable(label, false) && can_add) {
            active_tab->pending_node_type = label;
            active_tab->request_add_node_from_library = true;
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

void RenderOutline(ImVec2, ImVec2, EditorTab* active_tab)
{
    DrawOutlineBase(active_tab);
    ImGui::Spacing();
    ImGui::Text("Node Editor Outline");
}

void RenderProperties(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawHeader("PROPERTIES", colors.secondary_sidebar_text);
    if (active_tab) {
        ImGui::Text("Selection: %s", active_tab->name.c_str());
    }
    ImGui::TextWrapped("Select an element to inspect its properties.");
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

    const std::string default_script = NodeGraph::GetDefaultNodeScript(selected->type.empty() ? "Generic" : selected->type);
    if (selected->script.empty()) {
        selected->script = default_script;
    }
    InputTextMultilineStdString("##node_script", &selected->script, ImVec2(0, 140));
    if (ImGui::Button("Reset to Default")) {
        selected->script = default_script;
    }
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

} // namespace Scenes::NodeEditorViews
