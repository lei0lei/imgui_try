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
