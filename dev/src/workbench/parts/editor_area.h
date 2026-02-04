 #pragma once

 #include <memory>
 #include "../../services/editor_area_service.h"
 #include "../../ui/editor_area_ui.h"

 class EditorAreaPart {
 public:
	explicit EditorAreaPart(EditorAreaService& service);

	 void Render(
		 float left_offset,
		 float right_offset,
		 float title_h,
		 float status_bar_h,
		 float panel_h,
		 bool panel_visible,
		 bool block_tab_clicks
	 );

	EditorAreaService& GetService();
	const EditorAreaService& GetService() const;

 private:
	EditorAreaService& service_;
	 std::unique_ptr<UI::EditorArea> ui_;
 };
