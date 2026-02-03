 #include "title_bar.h"
 #include "../../ui/title_bar_ui.h"

TitleBarPart::TitleBarPart(TitleBarService& service)
	: service_(service)
{
}

 void TitleBarPart::Render(
	 SDL_Window* window,
	 float title_h,
	 bool primary_sidebar_visible,
	 bool panel_visible,
	 bool secondary_sidebar_visible)
 {
	 RenderTitleBarUI(window, service_, title_h, primary_sidebar_visible, panel_visible, secondary_sidebar_visible);
 }

 TitleBarService& TitleBarPart::GetService()
 {
	 return service_;
 }

 const TitleBarService& TitleBarPart::GetService() const
 {
	 return service_;
 }
