 #pragma once
 #include <SDL3/SDL.h>

#include "../../services/service_interfaces.h"
#include "../../services/notification_service.h"

 class StatusBarPart {
 public:
	explicit StatusBarPart(INotificationService& service);

	 void Render(SDL_Window* window, float status_bar_h = 24.0f, float title_h = 30.0f);

	INotificationService& GetService();
	const INotificationService& GetService() const;

 private:
	INotificationService& service_;
 };
