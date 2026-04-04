 #pragma once
 #include <SDL3/SDL.h>

#include "../../services/service_interfaces.h"
#include "../../core/title_bar_extension.h"

 class TitleBarPart {
 public:
	explicit TitleBarPart(ITitleBarService& service);

	 void Render(
		 SDL_Window* window,
		 float title_h,
		 bool primary_sidebar_visible,
		 EditorTab* active_tab,
		 const SceneTitleBarExtensionRenderer* scene_titlebar_extension,
		 const std::function<void(EditorTab&, const std::string&)>* scene_titlebar_action_handler,
		 float scene_titlebar_extension_width
	 );

	ITitleBarService& GetService();
	const ITitleBarService& GetService() const;

 private:
	ITitleBarService& service_;
 };
