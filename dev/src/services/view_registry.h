#pragma once

#include <array>
#include <functional>
#include <string>
#include <vector>
#include "editor_area_service.h"

enum class ViewContainer {
    PrimarySidebar = 0,
    SecondarySidebar = 1,
    Panel = 2
};

using ViewRenderer = std::function<void(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab)>;

struct ViewDefinition {
    std::string id;
    std::string title;
    ViewRenderer renderer;
};

class ViewRegistry {
public:
    void RegisterView(SceneType mode, ViewContainer container, ViewDefinition view);
    const std::vector<ViewDefinition>& GetViews(SceneType mode, ViewContainer container) const;
    int GetActiveViewIndex(SceneType mode, ViewContainer container) const;
    void SetActiveViewIndex(SceneType mode, ViewContainer container, int index);
    void SetActiveViewById(SceneType mode, ViewContainer container, const std::string& id);
    const ViewDefinition* GetActiveView(SceneType mode, ViewContainer container) const;

private:
    static int SceneIndex(SceneType type);
    static int ContainerIndex(ViewContainer container);

    std::array<std::array<std::vector<ViewDefinition>, 3>, 3> views_{};
    std::array<std::array<int, 3>, 3> active_indices_{};
};