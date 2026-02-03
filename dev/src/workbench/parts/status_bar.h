 #pragma once
 #include <SDL3/SDL.h>

#include "../../services/notification_service.h"

 class StatusBarPart {
 public:
	explicit StatusBarPart(NotificationService& service);

	 void Render(SDL_Window* window, float status_bar_h = 24.0f, float title_h = 30.0f);

	NotificationService& GetService();
	const NotificationService& GetService() const;

 private:
	NotificationService& service_;
 };
