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
	 PrimarySidebarProps props{};
	 props.activity_bar_w = activity_bar_w;
	 props.title_h = title_h;
	 props.status_bar_h = status_bar_h;
	 props.width = width;
	 props.active_view = view_registry.GetActiveView(scene_plugin_id);
	 props.fallback_view = UI::GetDefaultPrimaryViewForPlugin(scene_plugin_id, active_item);
	 return DrawPrimarySidebarUI(props, active_tab);
 }

 IPrimarySidebarService& PrimarySidebarPart::GetService()
 {
	 return service_;
 }

 const IPrimarySidebarService& PrimarySidebarPart::GetService() const
 {
	 return service_;
 }
