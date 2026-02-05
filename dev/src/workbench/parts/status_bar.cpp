 #include "status_bar.h"
 #include "../../ui/status_bar_ui.h"

StatusBarPart::StatusBarPart(INotificationService& service)
	: service_(service)
{
}

 void StatusBarPart::Render(SDL_Window* window, float status_bar_h, float title_h)
 {
	 StatusBarViewModel vm{};
	 const NotificationState& state = service_.GetState();
	 vm.message = state.message;
	 vm.progress = state.progress;
	 RenderStatusBarUI(window, vm, status_bar_h, title_h);
 }

 INotificationService& StatusBarPart::GetService()
 {
	 return service_;
 }

 const INotificationService& StatusBarPart::GetService() const
 {
	 return service_;
 }
