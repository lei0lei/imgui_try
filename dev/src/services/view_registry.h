#pragma once

#include <functional>
#include <string>
#include <array>
#include <unordered_map>
#include <vector>
#include "editor_area_service.h"
#include "service_interfaces.h"

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

class ViewRegistry : public IViewRegistry {
public:
    void RegisterView(const std::string& scene_key, ViewContainer container, const ViewDefinition& view) override;
    const std::vector<ViewDefinition>& GetViews(const std::string& scene_key, ViewContainer container) const override;
    int GetActiveViewIndex(const std::string& scene_key, ViewContainer container) const override;
    void SetActiveViewIndex(const std::string& scene_key, ViewContainer container, int index) override;
    void SetActiveViewById(const std::string& scene_key, ViewContainer container, const std::string& id) override;
    const ViewDefinition* GetActiveView(const std::string& scene_key, ViewContainer container) const override;

private:
    static int ContainerIndex(ViewContainer container);
    static std::string NormalizeSceneKey(const std::string& scene_key);

    struct SceneBucket {
        std::array<std::vector<ViewDefinition>, 3> views{};
        std::array<int, 3> active_indices{};
        std::array<std::unordered_map<std::string, int>, 3> id_to_index{};
    };

    mutable std::unordered_map<std::string, SceneBucket> scene_buckets_{};
};