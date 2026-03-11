/**
 * @file editor_area_service.cpp
 * @brief 编辑器区域服务的实现，用于管理打开的文件和标签
 * @author Your Name
 * @date 2026-02-05
 */

#include "editor_area_service.h"

int EditorAreaService::GetActiveTabIndex() const {
    return active_tab_index_;
}

EditorTab* EditorAreaService::GetActiveTab() {
    const int idx = active_tab_index_;
    if (idx < 0 || idx >= static_cast<int>(tabs_.size())) return nullptr;
    return &tabs_[idx];
}

const EditorTab* EditorAreaService::GetActiveTab() const {
    const int idx = active_tab_index_;
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
    tab_layout_states_.push_back({});
    active_tab_index_ = static_cast<int>(tabs_.size()) - 1;
    tabs_[active_tab_index_].active = true;
}

void EditorAreaService::CloseTab(int index) {
    if (index < 0 || index >= static_cast<int>(tabs_.size())) return;
    const bool was_active = tabs_[index].active;
    tabs_.erase(tabs_.begin() + index);
    tab_layout_states_.erase(tab_layout_states_.begin() + index);

    if (tabs_.empty()) {
        active_tab_index_ = -1;
        return;
    }

    if (was_active) {
        int new_active = index;
        if (new_active >= static_cast<int>(tabs_.size())) new_active = static_cast<int>(tabs_.size()) - 1;
        for (auto& t : tabs_) t.active = false;
        tabs_[new_active].active = true;
        active_tab_index_ = new_active;
        return;
    }

    if (active_tab_index_ > index) {
        --active_tab_index_;
    }
}

void EditorAreaService::ActivateTab(int index) {
    if (index < 0 || index >= static_cast<int>(tabs_.size())) return;
    for (auto& t : tabs_) t.active = false;
    tabs_[index].active = true;
    active_tab_index_ = index;
}

void EditorAreaService::MoveTab(int from_index, int to_index) {
    if (from_index < 0 || to_index < 0) return;
    if (from_index >= static_cast<int>(tabs_.size()) || to_index >= static_cast<int>(tabs_.size())) return;
    if (from_index == to_index) return;
    EditorTab moved = tabs_[from_index];
    TabLayoutState moved_layout = tab_layout_states_[from_index];
    tabs_.erase(tabs_.begin() + from_index);
    tab_layout_states_.erase(tab_layout_states_.begin() + from_index);
    tabs_.insert(tabs_.begin() + to_index, std::move(moved));
    tab_layout_states_.insert(tab_layout_states_.begin() + to_index, std::move(moved_layout));

    if (active_tab_index_ == from_index) {
        active_tab_index_ = to_index;
    } else if (from_index < active_tab_index_ && active_tab_index_ <= to_index) {
        --active_tab_index_;
    } else if (to_index <= active_tab_index_ && active_tab_index_ < from_index) {
        ++active_tab_index_;
    }
}

bool EditorAreaService::GetPanelVisibleForActiveTab(bool fallback) const {
    const int idx = active_tab_index_;
    if (idx < 0 || idx >= static_cast<int>(tab_layout_states_.size())) {
        return fallback;
    }
    return tab_layout_states_[idx].panel_visible;
}

bool EditorAreaService::GetSecondaryVisibleForActiveTab(bool fallback) const {
    const int idx = active_tab_index_;
    if (idx < 0 || idx >= static_cast<int>(tab_layout_states_.size())) {
        return fallback;
    }
    return tab_layout_states_[idx].secondary_sidebar_visible;
}

bool EditorAreaService::SetPanelVisibleForActiveTab(bool visible) {
    const int idx = active_tab_index_;
    if (idx < 0 || idx >= static_cast<int>(tab_layout_states_.size())) {
        return false;
    }
    tab_layout_states_[idx].panel_visible = visible;
    return true;
}

bool EditorAreaService::SetSecondaryVisibleForActiveTab(bool visible) {
    const int idx = active_tab_index_;
    if (idx < 0 || idx >= static_cast<int>(tab_layout_states_.size())) {
        return false;
    }
    tab_layout_states_[idx].secondary_sidebar_visible = visible;
    return true;
}

void EditorAreaService::CloseActiveTab() {
    int idx = active_tab_index_;
    if (idx >= 0) CloseTab(idx);
}

void EditorAreaService::SetTabs(const std::vector<EditorTab>& tabs) {
    tabs_ = tabs;
    tab_layout_states_.assign(tabs_.size(), TabLayoutState{});
    RebuildActiveTabIndex();
}

void EditorAreaService::RebuildActiveTabIndex() {
    active_tab_index_ = -1;
    for (size_t i = 0; i < tabs_.size(); ++i) {
        if (tabs_[i].active) {
            active_tab_index_ = static_cast<int>(i);
            break;
        }
    }

    if (active_tab_index_ < 0 && !tabs_.empty()) {
        active_tab_index_ = 0;
    }

    for (size_t i = 0; i < tabs_.size(); ++i) {
        tabs_[i].active = (static_cast<int>(i) == active_tab_index_);
    }
}
