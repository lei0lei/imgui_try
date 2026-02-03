#include "editor_area_service.h"

int EditorAreaService::GetActiveTabIndex() const {
    for (size_t i = 0; i < tabs_.size(); ++i) {
        if (tabs_[i].active) return static_cast<int>(i);
    }
    return -1;
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
