#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include "editor_tab.h"
#include "service_interfaces.h"

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
    void CloseActiveTab();
    void SetTabs(const std::vector<EditorTab>& tabs);
private:
    void RebuildActiveTabIndex();

    std::vector<EditorTab> tabs_;
    int active_tab_index_ = -1;
    uint64_t next_tab_id_ = 1;
};
