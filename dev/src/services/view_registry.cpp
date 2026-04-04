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

std::string ViewRegistry::NormalizeSceneKey(const std::string& scene_key)
{
    return scene_key.empty() ? kDefaultSceneKey : scene_key;
}

void ViewRegistry::RegisterView(const std::string& scene_key, const ViewDefinition& view)
{
    SceneBucket& bucket = scene_buckets_[NormalizeSceneKey(scene_key)];
    const auto existing = bucket.id_to_index.find(view.id);
    if (existing != bucket.id_to_index.end()) {
        const int index = existing->second;
        if (index >= 0 && index < static_cast<int>(bucket.views.size())) {
            bucket.views[index] = view; // overwrite existing definition
        } else {
            // Defensive: map says it exists but vector is out-of-sync. Rebuild by appending.
            const int new_index = static_cast<int>(bucket.views.size());
            bucket.views.push_back(view);
            bucket.id_to_index[view.id] = new_index;
        }
    } else {
        const int index = static_cast<int>(bucket.views.size());
        bucket.views.push_back(view);
        bucket.id_to_index[view.id] = index;
    }
    if (bucket.active_index < 0) {
        bucket.active_index = 0;
    }
}

const std::vector<ViewDefinition>& ViewRegistry::GetViews(const std::string& scene_key) const
{
    static const std::vector<ViewDefinition> kEmptyViews;
    const auto it = scene_buckets_.find(NormalizeSceneKey(scene_key));
    if (it == scene_buckets_.end()) {
        return kEmptyViews;
    }
    return it->second.views;
}

int ViewRegistry::GetActiveViewIndex(const std::string& scene_key) const
{
    const auto it = scene_buckets_.find(NormalizeSceneKey(scene_key));
    if (it == scene_buckets_.end()) {
        return -1;
    }
    int idx = it->second.active_index;
    const int size = static_cast<int>(it->second.views.size());
    return ClampIndex(idx, size);
}

void ViewRegistry::SetActiveViewIndex(const std::string& scene_key, int index)
{
    SceneBucket& bucket = scene_buckets_[NormalizeSceneKey(scene_key)];
    const int size = static_cast<int>(bucket.views.size());
    bucket.active_index = ClampIndex(index, size);
}

void ViewRegistry::SetActiveViewById(const std::string& scene_key, const std::string& id)
{
    SceneBucket& bucket = scene_buckets_[NormalizeSceneKey(scene_key)];
    const auto it = bucket.id_to_index.find(id);
    if (it != bucket.id_to_index.end()) {
        bucket.active_index = it->second;
    }
}

const ViewDefinition* ViewRegistry::GetActiveView(const std::string& scene_key) const
{
    const auto it = scene_buckets_.find(NormalizeSceneKey(scene_key));
    if (it == scene_buckets_.end()) {
        return nullptr;
    }

    const SceneBucket& bucket = it->second;
    const auto& list = bucket.views;
    int idx = ClampIndex(bucket.active_index, static_cast<int>(list.size()));
    if (idx < 0 || idx >= static_cast<int>(list.size())) {
        return nullptr;
    }
    return &list[idx];
}