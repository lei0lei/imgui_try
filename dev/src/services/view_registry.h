#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "view_types.h"
#include "service_interfaces.h"

class ViewRegistry : public IViewRegistry {
public:
    void RegisterView(const std::string& scene_key, const ViewDefinition& view);
    const std::vector<ViewDefinition>& GetViews(const std::string& scene_key) const;
    int GetActiveViewIndex(const std::string& scene_key) const;
    void SetActiveViewIndex(const std::string& scene_key, int index);
    void SetActiveViewById(const std::string& scene_key, const std::string& id);
    const ViewDefinition* GetActiveView(const std::string& scene_key) const;

private:
    static std::string NormalizeSceneKey(const std::string& scene_key);

    struct SceneBucket {
        std::vector<ViewDefinition> views{};
        int active_index = -1;
        std::unordered_map<std::string, int> id_to_index{};
    };

    mutable std::unordered_map<std::string, SceneBucket> scene_buckets_{};
};