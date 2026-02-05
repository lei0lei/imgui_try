/**
 * @file notification_service.cpp
 * @brief 通知服务的实现，用于状态消息和进度
 * @author Your Name
 * @date 2026-02-05
 */

#include "notification_service.h"

NotificationService::NotificationService() {
    state_.message = "Ready";
    state_.progress = -1.0f;
}

void NotificationService::SetMessage(const std::string& msg) {
    state_.message = msg;
}

void NotificationService::SetProgress(float value) {
    state_.progress = value;
}

void NotificationService::Clear() {
    state_.message.clear();
    state_.progress = -1.0f;
}

const NotificationState& NotificationService::GetState() const {
    return state_;
}
