 #pragma once
 #include <SDL3/SDL.h>

 #include "../../services/title_bar_service.h"

 class TitleBarPart {
 public:
	explicit TitleBarPart(TitleBarService& service);

	 void Render(
		 SDL_Window* window,
		 float title_h,
		 bool primary_sidebar_visible,
		 bool panel_visible,
		 bool secondary_sidebar_visible
	 );

	TitleBarService& GetService();
	const TitleBarService& GetService() const;

 private:
	TitleBarService& service_;
 };
