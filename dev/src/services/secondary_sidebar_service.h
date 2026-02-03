#pragma once
#include <string>
#include <vector>

// SecondarySidebarService: manages the state and logic of the right secondary sidebar
class SecondarySidebarService {
public:
    SecondarySidebarService();

    // Visibility
    bool IsVisible() const;
    void SetVisible(bool visible);
    void ToggleVisible();

    // Optionally: manage sidebar content type, etc.
    // void SetActiveTab(int tab);
    // int GetActiveTab() const;

private:
    bool visible_ = false;
};
