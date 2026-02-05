/**
 * @file activity_bar_service.cpp
 * @brief 活动栏服务的实现，用于管理侧边栏导航
 * @author Your Name
 * @date 2026-02-05
 */

#include "activity_bar_service.h"

ActivityBarService::ActivityBarService()
    : selected_item_(1) // 1 = Explorer
{
    items_ = { "Explorer", "Search", "Node editor", "Debug", "Extensions" };
}

int ActivityBarService::GetSelectedItem() const {
    return selected_item_;
}

void ActivityBarService::SetSelectedItem(int index) {
    if (index >= 1 && index <= (int)items_.size())
        selected_item_ = index;
}

const std::vector<std::string>& ActivityBarService::GetItems() const {
    return items_;
}

void ActivityBarService::SetItems(const std::vector<std::string>& items) {
    items_ = items;
    if (selected_item_ >= (int)items_.size())
        selected_item_ = 0;
}
