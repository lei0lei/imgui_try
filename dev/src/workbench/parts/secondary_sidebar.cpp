/**
 * @file secondary_sidebar.cpp
 * @brief 副侧边栏工作台部件实现
 * @author Your Name
 * @date 2026-02-05
 */

#include "secondary_sidebar.h"
#include "../../ui/view_registry_defaults.h"

SecondarySidebarPart::SecondarySidebarPart(ISecondarySidebarService& service)
	: service_(service)
{
}

 SecondarySidebarResult SecondarySidebarPart::Render(
	 float title_h,
	 float status_bar_h,
	 float panel_h,
	 float width,
	 IViewRegistry& view_registry,
	 const std::string& scene_plugin_id,
	 EditorTab* active_tab)
 {
	 SecondarySidebarViewModel vm{};
	 vm.is_visible = service_.IsVisible();
	 vm.has_active_tab = (active_tab != nullptr);
	const auto& views = view_registry.GetViews(scene_plugin_id, ViewContainer::SecondarySidebar);
	 auto find_view_by_id = [&](const std::string& id) -> const ViewDefinition* {
		 for (const auto& view : views) {
			 if (view.id == id) {
				 return &view;
			 }
		 }
		 return nullptr;
	 };

	const ViewDefinition* active_view = nullptr;
	int active_index = 0;
	 if (active_tab && !active_tab->secondary_active_view_id.empty()) {
		 active_view = find_view_by_id(active_tab->secondary_active_view_id);
	 }
	 if (!active_view || !active_view->renderer) {
		 const ViewDefinition def = UI::GetDefaultSecondaryViewForPlugin(scene_plugin_id);
		 if (def.renderer) {
			 active_view = find_view_by_id(def.id);
			 if (active_tab && active_view) {
				 active_tab->secondary_active_view_id = active_view->id;
			 }
		 }
	 }
	 if (active_view) {
		 for (int i = 0; i < static_cast<int>(views.size()); ++i) {
			 if (views[i].id == active_view->id) {
				 active_index = i;
				 break;
			 }
		 }
	 }
	 vm.active_view = active_view;
	 vm.views = &views;
	 vm.active_index = active_index;
	 vm.show_tabs = views.size() > 1;
	 vm.on_select_tab = [active_tab, &views](int index) {
		 if (!active_tab) {
			 return;
		 }
		 if (index < 0 || index >= static_cast<int>(views.size())) {
			 return;
		 }
		 active_tab->secondary_active_view_id = views[index].id;
	 };
	 return DrawSecondarySidebarUI(title_h, status_bar_h, panel_h, width, vm, active_tab);
 }

 ISecondarySidebarService& SecondarySidebarPart::GetService()
 {
	 return service_;
 }

 const ISecondarySidebarService& SecondarySidebarPart::GetService() const
 {
	 return service_;
 }
