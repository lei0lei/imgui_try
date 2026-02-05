#pragma once
#include <vector>
#include <string>
#include "imgui.h"
#include "service_interfaces.h"

// 场景类型
enum class SceneType {
    Scene2D,
    Scene3D,
    NodeEditor
};

// 编辑器标签页
struct EditorTab {
    std::string name;
    std::string path;
    bool modified = false;
    bool active = false;
    SceneType scene_type = SceneType::Scene3D;
    bool panel_visible = false;
    bool secondary_sidebar_visible = false;
    ImVec2 node_canvas_pan = ImVec2(0.0f, 0.0f);
    float node_canvas_zoom = 1.0f;
    struct Node {
        int id = 0;
        std::string title;
        ImVec2 pos = ImVec2(0.0f, 0.0f);
        ImVec2 size = ImVec2(160.0f, 80.0f);
    };
    std::vector<Node> nodes;
    int node_next_id = 1;
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
    SceneType GetActiveSceneType(SceneType fallback = SceneType::Scene3D) const override;
    void AddTab(const EditorTab& tab) override;
    void CloseTab(int index) override;
    void ActivateTab(int index) override;
    void CloseActiveTab();
    void SetTabs(const std::vector<EditorTab>& tabs) { tabs_ = tabs; }
private:
    std::vector<EditorTab> tabs_;
};
