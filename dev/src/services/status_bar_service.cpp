/**
 * @file status_bar_service.cpp
 * @brief 状态栏服务的实现，用于底部状态信息
 * @author Your Name
 * @date 2026-02-05
 */

#include "status_bar_service.h"

StatusBarService::StatusBarService() {
    state_.message = "Ready";
    state_.progress = -1.0f;
}

void StatusBarService::SetMessage(const std::string& msg) {
    state_.message = msg;
}

void StatusBarService::SetProgress(float value) {
    state_.progress = value;
}

void StatusBarService::Clear() {
    state_.message.clear();
    state_.progress = -1.0f;
}

const StatusBarState& StatusBarService::GetState() const {
    return state_;
}
