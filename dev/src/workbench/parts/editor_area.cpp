/**
 * @file editor_area.cpp
 * @brief 编辑器区域工作台部件实现
 * @author Your Name
 * @date 2026-02-05
 */

 #include "editor_area.h"

EditorAreaPart::EditorAreaPart(IEditorAreaService& service, IEventBus& event_bus)
	: service_(service),
	  event_bus_(event_bus),
	  ui_(std::make_unique<UI::EditorArea>())
 {
 }

 void EditorAreaPart::Render(
	 float left_offset,
	 float right_offset,
	 float title_h,
	 float status_bar_h,
	 bool block_tab_clicks)
 {
	 UI::EditorArea::Props props{};
	 props.left_offset = left_offset;
	 props.right_offset = right_offset;
	 props.title_h = title_h;
	 props.status_bar_h = status_bar_h;
	 props.block_tab_clicks = block_tab_clicks;
	 props.tabs = &service_.GetTabs();
	 props.active_tab = service_.GetActiveTab();
	 props.event_bus = &event_bus_;

	 const UI::EditorArea::Result result = ui_->Draw(props);
	 if (result.closed_tab >= 0) {
		 service_.CloseTab(result.closed_tab);
	 }
	 if (result.active_tab >= 0) {
		 service_.ActivateTab(result.active_tab);
	 }
	 if (result.move_from >= 0 && result.move_to >= 0 && result.move_from != result.move_to) {
		 service_.MoveTab(result.move_from, result.move_to);
	 }
 }

 IEditorAreaService& EditorAreaPart::GetService()
 {
	 return service_;
 }

 const IEditorAreaService& EditorAreaPart::GetService() const
 {
	 return service_;
 }
