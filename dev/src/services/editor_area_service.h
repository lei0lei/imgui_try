#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "imgui.h"
#include "service_interfaces.h"

// 编辑器标签页
struct EditorTab {
    std::string name;
    std::string path;
    bool modified = false;
    bool active = false;
    std::string scene_plugin_id;
    std::string secondary_active_view_id;
    std::string panel_active_view_id;
    ImVec2 node_canvas_pan = ImVec2(0.0f, 0.0f);
    float node_canvas_zoom = 1.0f;
    // Node graph execution status
    bool node_exec_last_ok = true;
    bool node_exec_last_parallel = false;
    double node_exec_last_ms = 0.0;
    std::string node_exec_last_error;
    std::vector<std::string> node_exec_log;
    std::unordered_map<int, std::vector<std::string>> node_exec_outputs;
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

// 编辑器区域服务，负责标签页和状态管理
class EditorAreaService : public IEditorAreaService {
public:
    EditorAreaService() = default;
    const std::vector<EditorTab>& GetTabs() const override { return tabs_; }
    std::vector<EditorTab>& GetTabs() override { return tabs_; }
    int GetActiveTabIndex() const override;
    EditorTab* GetActiveTab() override;
    const EditorTab* GetActiveTab() const override;
    void AddTab(const EditorTab& tab) override;
    void CloseTab(int index) override;
    void ActivateTab(int index) override;
    void MoveTab(int from_index, int to_index) override;
    bool GetPanelVisibleForActiveTab(bool fallback) const override;
    bool GetSecondaryVisibleForActiveTab(bool fallback) const override;
    bool SetPanelVisibleForActiveTab(bool visible) override;
    bool SetSecondaryVisibleForActiveTab(bool visible) override;
    void CloseActiveTab();
    void SetTabs(const std::vector<EditorTab>& tabs);
private:
    struct TabLayoutState {
        bool panel_visible = false;
        bool secondary_sidebar_visible = false;
    };

    void RebuildActiveTabIndex();

    std::vector<EditorTab> tabs_;
    std::vector<TabLayoutState> tab_layout_states_;
    int active_tab_index_ = -1;
};
