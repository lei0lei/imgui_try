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

void EditorAreaService::AddTab(const EditorTab& tab) {
    EditorTab new_tab = tab;
    if (new_tab.id == 0) {
        new_tab.id = next_tab_id_++;
    } else if (new_tab.id >= next_tab_id_) {
        next_tab_id_ = new_tab.id + 1;
    }

    for (auto& t : tabs_) t.active = false;
    tabs_.push_back(std::move(new_tab));
    active_tab_index_ = static_cast<int>(tabs_.size()) - 1;
    tabs_[active_tab_index_].active = true;
}

void EditorAreaService::CloseTab(int index) {
    if (index < 0 || index >= static_cast<int>(tabs_.size())) return;
    const bool was_active = tabs_[index].active;
    tabs_.erase(tabs_.begin() + index);

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
    tabs_.erase(tabs_.begin() + from_index);
    tabs_.insert(tabs_.begin() + to_index, std::move(moved));

    if (active_tab_index_ == from_index) {
        active_tab_index_ = to_index;
    } else if (from_index < active_tab_index_ && active_tab_index_ <= to_index) {
        --active_tab_index_;
    } else if (to_index <= active_tab_index_ && active_tab_index_ < from_index) {
        ++active_tab_index_;
    }
}

void EditorAreaService::CloseActiveTab() {
    int idx = active_tab_index_;
    if (idx >= 0) CloseTab(idx);
}

void EditorAreaService::SetTabs(const std::vector<EditorTab>& tabs) {
    tabs_ = tabs;
    uint64_t max_id = 0;
    for (auto& tab : tabs_) {
        if (tab.id == 0) {
            tab.id = ++max_id;
        }
        if (tab.id > max_id) {
            max_id = tab.id;
        }
    }
    next_tab_id_ = max_id + 1;
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
