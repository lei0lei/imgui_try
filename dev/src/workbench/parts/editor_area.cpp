 #include "editor_area.h"

EditorAreaPart::EditorAreaPart(EditorAreaService& service)
	: service_(service),
	  ui_(std::make_unique<UI::EditorArea>(service_))
 {
 }

 void EditorAreaPart::Render(
	 float left_offset,
	 float right_offset,
	 float title_h,
	 float status_bar_h,
	 float panel_h,
	 bool panel_visible,
	 bool block_tab_clicks)
 {
	 ui_->Draw(left_offset, right_offset, title_h, status_bar_h, panel_h, panel_visible, block_tab_clicks);
 }

 EditorAreaService& EditorAreaPart::GetService()
 {
	 return service_;
 }

 const EditorAreaService& EditorAreaPart::GetService() const
 {
	 return service_;
 }
