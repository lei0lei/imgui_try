/**
 * @file view_registry.cpp
 * @brief 视图注册表的实现，用于管理UI组件
 * @author Your Name
 * @date 2026-02-05
 */

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

void ViewRegistry::RegisterView(SceneType mode, ViewContainer container, const ViewDefinition& view)
{
    const int s = SceneIndex(mode);
    const int c = ContainerIndex(container);
    const int index = static_cast<int>(views_[s][c].size());
    views_[s][c].push_back(view);
    id_to_index_[s][c][view.id] = index;
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
    const auto it = id_to_index_[s][c].find(id);
    if (it != id_to_index_[s][c].end()) {
        active_indices_[s][c] = it->second;
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