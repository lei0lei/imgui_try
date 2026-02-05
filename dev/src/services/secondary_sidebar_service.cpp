/**
 * @file secondary_sidebar_service.cpp
 * @brief 副侧边栏服务的实现，用于附加面板
 * @author Your Name
 * @date 2026-02-05
 */

#include "secondary_sidebar_service.h"

SecondarySidebarService::SecondarySidebarService()
    : visible_(false)
{
}

bool SecondarySidebarService::IsVisible() const {
    return visible_;
}

void SecondarySidebarService::SetVisible(bool visible) {
    visible_ = visible;
}

void SecondarySidebarService::ToggleVisible() {
    visible_ = !visible_;
}
