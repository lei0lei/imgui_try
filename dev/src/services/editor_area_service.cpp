#include "editor_area_service.h"

int EditorAreaService::GetActiveTabIndex() const {
    for (size_t i = 0; i < tabs_.size(); ++i) {
        if (tabs_[i].active) return static_cast<int>(i);
    }
    return -1;
}

EditorTab* EditorAreaService::GetActiveTab() {
    int idx = GetActiveTabIndex();
    if (idx < 0 || idx >= static_cast<int>(tabs_.size())) return nullptr;
    return &tabs_[idx];
}

const EditorTab* EditorAreaService::GetActiveTab() const {
    int idx = GetActiveTabIndex();
    if (idx < 0 || idx >= static_cast<int>(tabs_.size())) return nullptr;
    return &tabs_[idx];
}

SceneType EditorAreaService::GetActiveSceneType(SceneType fallback) const {
    const EditorTab* tab = GetActiveTab();
    if (!tab) return fallback;
    return tab->scene_type;
}

void EditorAreaService::AddTab(const EditorTab& tab) {
    for (auto& t : tabs_) t.active = false;
    tabs_.push_back(tab);
    tabs_.back().active = true;
}

void EditorAreaService::CloseTab(int index) {
    if (index < 0 || index >= static_cast<int>(tabs_.size())) return;
    bool was_active = tabs_[index].active;
    tabs_.erase(tabs_.begin() + index);
    if (was_active && !tabs_.empty()) {
        int new_active = index;
        if (new_active >= static_cast<int>(tabs_.size())) new_active = static_cast<int>(tabs_.size()) - 1;
        for (auto& t : tabs_) t.active = false;
        tabs_[new_active].active = true;
    }
}

void EditorAreaService::ActivateTab(int index) {
    if (index < 0 || index >= static_cast<int>(tabs_.size())) return;
    for (auto& t : tabs_) t.active = false;
    tabs_[index].active = true;
}

void EditorAreaService::CloseActiveTab() {
    int idx = GetActiveTabIndex();
    if (idx >= 0) CloseTab(idx);
}
