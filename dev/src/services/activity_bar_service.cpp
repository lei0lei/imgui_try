/**
 * @file activity_bar_service.cpp
 * @brief 活动栏服务的实现，用于管理侧边栏导航
 * @author Your Name
 * @date 2026-02-05
 */

#include "activity_bar_service.h"
#include "../core/workbench_types.h"

namespace {

bool IsSelectableActivityItem(int index)
{
    switch (static_cast<ActivityBarItem>(index)) {
        case ActivityBarItem::Explorer:
        case ActivityBarItem::Search:
        case ActivityBarItem::Debug:
        case ActivityBarItem::Editor:
        case ActivityBarItem::Extensions:
            return true;
        default:
            return false;
    }
}

}

ActivityBarService::ActivityBarService()
    : selected_item_(1) // 1 = Explorer
{
    items_ = { "Explorer", "Search", "Debug", "Editor", "Extensions" };
}

int ActivityBarService::GetSelectedItem() const {
    return selected_item_;
}

void ActivityBarService::SetSelectedItem(int index) {
    if (IsSelectableActivityItem(index))
        selected_item_ = index;
}

const std::vector<std::string>& ActivityBarService::GetItems() const {
    return items_;
}

void ActivityBarService::SetItems(const std::vector<std::string>& items) {
    items_ = items;
    if (!IsSelectableActivityItem(selected_item_))
        selected_item_ = static_cast<int>(ActivityBarItem::Explorer);
}
