#include "view_registry.h"
#include <algorithm>

namespace {
int ClampIndex(int index, int size)
{
    if (size <= 0) return -1;
    if (index < 0) return 0;
    if (index >= size) return size - 1;
    return index;
}
}

int ViewRegistry::SceneIndex(SceneType type)
{
    switch (type) {
        case SceneType::Scene2D: return 0;
        case SceneType::Scene3D: return 1;
        case SceneType::NodeEditor: return 2;
        default: return 1;
    }
}

int ViewRegistry::ContainerIndex(ViewContainer container)
{
    switch (container) {
        case ViewContainer::PrimarySidebar: return 0;
        case ViewContainer::SecondarySidebar: return 1;
        case ViewContainer::Panel: return 2;
        default: return 0;
    }
}

void ViewRegistry::RegisterView(SceneType mode, ViewContainer container, ViewDefinition view)
{
    const int s = SceneIndex(mode);
    const int c = ContainerIndex(container);
    views_[s][c].push_back(std::move(view));
    if (active_indices_[s][c] <= 0) {
        active_indices_[s][c] = 0;
    }
}

const std::vector<ViewDefinition>& ViewRegistry::GetViews(SceneType mode, ViewContainer container) const
{
    const int s = SceneIndex(mode);
    const int c = ContainerIndex(container);
    return views_[s][c];
}

int ViewRegistry::GetActiveViewIndex(SceneType mode, ViewContainer container) const
{
    const int s = SceneIndex(mode);
    const int c = ContainerIndex(container);
    int idx = active_indices_[s][c];
    const int size = static_cast<int>(views_[s][c].size());
    return ClampIndex(idx, size);
}

void ViewRegistry::SetActiveViewIndex(SceneType mode, ViewContainer container, int index)
{
    const int s = SceneIndex(mode);
    const int c = ContainerIndex(container);
    const int size = static_cast<int>(views_[s][c].size());
    active_indices_[s][c] = ClampIndex(index, size);
}

void ViewRegistry::SetActiveViewById(SceneType mode, ViewContainer container, const std::string& id)
{
    const int s = SceneIndex(mode);
    const int c = ContainerIndex(container);
    const auto& list = views_[s][c];
    for (size_t i = 0; i < list.size(); ++i) {
        if (list[i].id == id) {
            active_indices_[s][c] = static_cast<int>(i);
            return;
        }
    }
}

const ViewDefinition* ViewRegistry::GetActiveView(SceneType mode, ViewContainer container) const
{
    const int s = SceneIndex(mode);
    const int c = ContainerIndex(container);
    const auto& list = views_[s][c];
    int idx = ClampIndex(active_indices_[s][c], static_cast<int>(list.size()));
    if (idx < 0 || idx >= static_cast<int>(list.size())) {
        return nullptr;
    }
    return &list[idx];
}