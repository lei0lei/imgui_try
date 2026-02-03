 #include "status_bar.h"
 #include "../../ui/status_bar_ui.h"

StatusBarPart::StatusBarPart(NotificationService& service)
	: service_(service)
{
}

 void StatusBarPart::Render(SDL_Window* window, float status_bar_h, float title_h)
 {
	 RenderStatusBarUI(window, service_, status_bar_h, title_h);
 }

NotificationService& StatusBarPart::GetService()
 {
	 return service_;
 }

const NotificationService& StatusBarPart::GetService() const
 {
	 return service_;
 }
