 #include "editor_area.h"

EditorAreaPart::EditorAreaPart(IEditorAreaService& service)
	: service_(service),
	  ui_(std::make_unique<UI::EditorArea>())
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
	 UI::EditorArea::ViewModel vm{};
	 vm.get_tabs = [this]() -> const std::vector<EditorTab>& { return service_.GetTabs(); };
	 vm.get_active_tab = [this]() -> EditorTab* { return service_.GetActiveTab(); };
	 vm.close_tab = [this](int index) { service_.CloseTab(index); };
	 vm.activate_tab = [this](int index) { service_.ActivateTab(index); };
	 ui_->Draw(left_offset, right_offset, title_h, status_bar_h, panel_h, panel_visible, block_tab_clicks, vm);
 }

 IEditorAreaService& EditorAreaPart::GetService()
 {
	 return service_;
 }

 const IEditorAreaService& EditorAreaPart::GetService() const
 {
	 return service_;
 }
