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
