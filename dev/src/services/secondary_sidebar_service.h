#pragma once
#include <string>
#include <vector>
#include "service_interfaces.h"

// SecondarySidebarService: manages the state and logic of the right secondary sidebar
class SecondarySidebarService : public ISecondarySidebarService {
public:
    SecondarySidebarService();

    // Visibility
    bool IsVisible() const override;
    void SetVisible(bool visible) override;
    void ToggleVisible();

    // Optionally: manage sidebar content type, etc.
    // void SetActiveTab(int tab);
    // int GetActiveTab() const;

private:
    bool visible_ = false;
};
