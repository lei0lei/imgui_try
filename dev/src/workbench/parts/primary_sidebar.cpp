/**
 * @file primary_sidebar.cpp
 * @brief 主侧边栏工作台部件实现
 * @author Your Name
 * @date 2026-02-05
 */

 #include "primary_sidebar.h"

PrimarySidebarPart::PrimarySidebarPart(IPrimarySidebarService& service)
	: service_(service)
{
}

 PrimarySidebarResult PrimarySidebarPart::Render(
	 float activity_bar_w,
	 float title_h,
	 float status_bar_h,
	 float width,
	 IViewRegistry& view_registry,
	 const std::string& scene_plugin_id,
	 EditorTab* active_tab,
	 ActivityBarItem active_item)
 {
	 PrimarySidebarViewModel vm{};
	 vm.active_view = view_registry.GetActiveView(scene_plugin_id, ViewContainer::PrimarySidebar);
	 vm.fallback_view = UI::GetDefaultPrimaryViewForPlugin(scene_plugin_id, active_item);
	 vm.is_visible = service_.IsVisible();
	 vm.active_item = active_item;
	 return DrawPrimarySidebarUI(activity_bar_w, title_h, status_bar_h, width, vm, active_tab);
 }

 IPrimarySidebarService& PrimarySidebarPart::GetService()
 {
	 return service_;
 }

 const IPrimarySidebarService& PrimarySidebarPart::GetService() const
 {
	 return service_;
 }
