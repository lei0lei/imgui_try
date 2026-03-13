/**
 * @file view_registry.cpp
 * @brief 视图注册表的实现，用于管理UI组件
 * @author Your Name
 * @date 2026-02-05
 */

#include "view_registry.h"
#include <algorithm>

namespace {
constexpr const char* kDefaultSceneKey = "__default_scene__";

int ClampIndex(int index, int size)
{
    if (size <= 0) return -1;
    if (index < 0) return 0;
    if (index >= size) return size - 1;
    return index;
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

std::string ViewRegistry::NormalizeSceneKey(const std::string& scene_key)
{
    return scene_key.empty() ? kDefaultSceneKey : scene_key;
}

void ViewRegistry::RegisterView(const std::string& scene_key, ViewContainer container, const ViewDefinition& view)
{
    SceneBucket& bucket = scene_buckets_[NormalizeSceneKey(scene_key)];
    const int c = ContainerIndex(container);
    const int index = static_cast<int>(bucket.views[c].size());
    bucket.views[c].push_back(view);
    bucket.id_to_index[c][view.id] = index;
    if (bucket.active_indices[c] <= 0) {
        bucket.active_indices[c] = 0;
    }
}

const std::vector<ViewDefinition>& ViewRegistry::GetViews(const std::string& scene_key, ViewContainer container) const
{
    static const std::vector<ViewDefinition> kEmptyViews;
    const int c = ContainerIndex(container);
    const auto it = scene_buckets_.find(NormalizeSceneKey(scene_key));
    if (it == scene_buckets_.end()) {
        return kEmptyViews;
    }
    return it->second.views[c];
}

int ViewRegistry::GetActiveViewIndex(const std::string& scene_key, ViewContainer container) const
{
    const auto it = scene_buckets_.find(NormalizeSceneKey(scene_key));
    if (it == scene_buckets_.end()) {
        return -1;
    }
    const int c = ContainerIndex(container);
    int idx = it->second.active_indices[c];
    const int size = static_cast<int>(it->second.views[c].size());
    return ClampIndex(idx, size);
}

void ViewRegistry::SetActiveViewIndex(const std::string& scene_key, ViewContainer container, int index)
{
    SceneBucket& bucket = scene_buckets_[NormalizeSceneKey(scene_key)];
    const int c = ContainerIndex(container);
    const int size = static_cast<int>(bucket.views[c].size());
    bucket.active_indices[c] = ClampIndex(index, size);
}

void ViewRegistry::SetActiveViewById(const std::string& scene_key, ViewContainer container, const std::string& id)
{
    SceneBucket& bucket = scene_buckets_[NormalizeSceneKey(scene_key)];
    const int c = ContainerIndex(container);
    const auto it = bucket.id_to_index[c].find(id);
    if (it != bucket.id_to_index[c].end()) {
        bucket.active_indices[c] = it->second;
    }
}

const ViewDefinition* ViewRegistry::GetActiveView(const std::string& scene_key, ViewContainer container) const
{
    const auto it = scene_buckets_.find(NormalizeSceneKey(scene_key));
    if (it == scene_buckets_.end()) {
        return nullptr;
    }

    const SceneBucket& bucket = it->second;
    const int c = ContainerIndex(container);
    const auto& list = bucket.views[c];
    int idx = ClampIndex(bucket.active_indices[c], static_cast<int>(list.size()));
    if (idx < 0 || idx >= static_cast<int>(list.size())) {
        return nullptr;
    }
    return &list[idx];
}