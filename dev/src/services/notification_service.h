#pragma once
#include <string>
#include "service_interfaces.h"

// Notification state
struct NotificationState {
    std::string message;
    float progress = -1.0f; // <0 means no progress
};

// Notification service for user-facing messages
class NotificationService : public INotificationService {
public:
    NotificationService();
    void SetMessage(const std::string& msg) override;
    void SetProgress(float value); // 0~1
    void Clear();
    const NotificationState& GetState() const override;
private:
    NotificationState state_;
};
