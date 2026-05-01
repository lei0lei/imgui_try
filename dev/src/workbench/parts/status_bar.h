 #pragma once
 #include <SDL3/SDL.h>

#include "../../services/service_interfaces.h"
#include "../../services/notification_service.h"
#include "../status_bar_extension.h"

 class StatusBarPart {
 public:
	explicit StatusBarPart(INotificationService& service);

	 void Render(
		 SDL_Window* window,
		 float status_bar_h = 24.0f,
		 float title_h = 30.0f,
		 EditorTab* active_tab = nullptr,
		 const SceneStatusBarExtensionRenderer* scene_status_bar_extension = nullptr,
		 const std::function<void(EditorTab&, const std::string&)>* scene_status_bar_action_handler = nullptr
	 );

	INotificationService& GetService();
	const INotificationService& GetService() const;

 private:
	INotificationService& service_;
 };
