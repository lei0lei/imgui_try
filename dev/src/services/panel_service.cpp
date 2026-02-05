/**
 * @file panel_service.cpp
 * @brief 面板服务的实现，用于管理底部面板可见性
 * @author Your Name
 * @date 2026-02-05
 */

#include "panel_service.h"

PanelService::PanelService() {
    Reset();
}

void PanelService::Reset() {
    state_ = PanelState();
}

void PanelService::SetVisible(bool v) {
    state_.visible = v;
}

bool PanelService::IsVisible() const {
    return state_.visible;
}

void PanelService::ToggleVisible() {
    state_.visible = !state_.visible;
}

void PanelService::SetActiveTab(PanelTab tab) {
    state_.active_tab = tab;
}

PanelTab PanelService::GetActiveTab() const {
    return state_.active_tab;
}

const std::vector<PanelTab>& PanelService::GetTabs() const {
    return state_.tabs;
}

PanelState& PanelService::GetState() {
    return state_;
}

const PanelState& PanelService::GetState() const {
    return state_;
}
