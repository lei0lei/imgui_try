/**
 * @file panel.cpp
 * @brief 面板工作台部件实现
 * @author Your Name
 * @date 2026-02-05
 */

#include "panel.h"
#include "../../ui/view_registry_defaults.h"

PanelPart::PanelPart(IPanelService& service)
	: service_(service)
{
}

 PanelResult PanelPart::Render(
	 float left_offset,
	 float right_offset,
	 float status_bar_h,
	 float panel_h,
	 IViewRegistry& view_registry,
	 const std::string& scene_plugin_id,
	 EditorTab* active_tab)
 {
	 PanelViewModel vm{};
	 vm.views = &view_registry.GetViews(scene_plugin_id, ViewContainer::Panel);
	 const auto& views = view_registry.GetViews(scene_plugin_id, ViewContainer::Panel);
	 auto find_view_by_id = [&](const std::string& id) -> const ViewDefinition* {
		 for (const auto& view : views) {
			 if (view.id == id) {
				 return &view;
			 }
		 }
		 return nullptr;
	 };
	 auto find_index_by_id = [&](const std::string& id) -> int {
		 for (int i = 0; i < static_cast<int>(views.size()); ++i) {
			 if (views[i].id == id) {
				 return i;
			 }
		 }
		 return -1;
	 };

	 const ViewDefinition* active_view = nullptr;
	 int active_index = -1;
	 if (active_tab && !active_tab->panel_active_view_id.empty()) {
		 active_index = find_index_by_id(active_tab->panel_active_view_id);
		 active_view = (active_index >= 0) ? &views[active_index] : nullptr;
	 }
	 if (!active_view || !active_view->renderer) {
		 const ViewDefinition def = UI::GetDefaultPanelViewForPlugin(scene_plugin_id);
		 if (def.renderer) {
			 active_view = find_view_by_id(def.id);
			 active_index = find_index_by_id(def.id);
			 if (active_tab && active_view) {
				 active_tab->panel_active_view_id = active_view->id;
			 }
		 }
	 }
	 vm.active_index = (active_index >= 0) ? active_index : 0;
	 vm.on_select_tab = [active_tab, &views](int index) {
		 if (!active_tab) {
			 return;
		 }
		 if (index >= 0 && index < static_cast<int>(views.size())) {
			 active_tab->panel_active_view_id = views[index].id;
		 }
	 };
	 vm.active_view = active_view;
	 vm.has_active_tab = (active_tab != nullptr);
	 vm.show_tabs = views.size() > 1;
	 return DrawPanelUI(left_offset, right_offset, status_bar_h, panel_h, vm, active_tab);
 }

 IPanelService& PanelPart::GetService()
 {
	 return service_;
 }

 const IPanelService& PanelPart::GetService() const
 {
	 return service_;
 }
