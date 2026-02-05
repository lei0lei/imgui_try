/**
 * @file primary_sidebar_service.cpp
 * @brief 主侧边栏服务的实现，用于资源管理器和导航
 * @author Your Name
 * @date 2026-02-05
 */

#include "primary_sidebar_service.h"

PrimarySidebarService::PrimarySidebarService()
    : selected_item_(0), visible_(true)
{
    items_ = { "Explorer", "Search", "Source Control", "Debug", "Extensions" };
}

int PrimarySidebarService::GetSelectedItem() const {
    return selected_item_;
}

void PrimarySidebarService::SetSelectedItem(int index) {
    if (index >= 0 && index < (int)items_.size())
        selected_item_ = index;
}

bool PrimarySidebarService::IsVisible() const {
    return visible_;
}

void PrimarySidebarService::SetVisible(bool visible) {
    visible_ = visible;
}

void PrimarySidebarService::ToggleVisible() {
    visible_ = !visible_;
}

const std::vector<std::string>& PrimarySidebarService::GetItems() const {
    return items_;
}

void PrimarySidebarService::SetItems(const std::vector<std::string>& items) {
    items_ = items;
    if (selected_item_ >= (int)items_.size())
        selected_item_ = 0;
}
