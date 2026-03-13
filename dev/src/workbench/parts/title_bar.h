 #pragma once
 #include <SDL3/SDL.h>

#include "../../services/service_interfaces.h"

 class TitleBarPart {
 public:
	explicit TitleBarPart(ITitleBarService& service);

	 void Render(
		 SDL_Window* window,
		 float title_h,
		 bool primary_sidebar_visible
	 );

	ITitleBarService& GetService();
	const ITitleBarService& GetService() const;

 private:
	ITitleBarService& service_;
 };
