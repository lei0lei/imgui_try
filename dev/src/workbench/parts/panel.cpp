 #include "panel.h"

PanelPart::PanelPart(PanelService& service)
	: service_(service)
{
}

 PanelResult PanelPart::Render(
	 float left_offset,
	 float right_offset,
	 float status_bar_h,
	 float panel_h,
	 ViewRegistry& view_registry,
	 SceneType mode,
	 EditorTab* active_tab)
 {
	 return DrawPanelUI(left_offset, right_offset, status_bar_h, panel_h, service_, view_registry, mode, active_tab);
 }

 PanelService& PanelPart::GetService()
 {
	 return service_;
 }

 const PanelService& PanelPart::GetService() const
 {
	 return service_;
 }
