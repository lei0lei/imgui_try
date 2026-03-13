/**
 * @file title_bar.cpp
 * @brief 标题栏工作台部件实现
 * @author Your Name
 * @date 2026-02-05
 */

 #include "title_bar.h"
 #include "../../ui/title_bar_ui.h"

TitleBarPart::TitleBarPart(ITitleBarService& service)
	: service_(service)
{
}

 void TitleBarPart::Render(
	 SDL_Window* window,
	 float title_h,
	 bool primary_sidebar_visible)
 {
	 TitleBarViewModel vm{};
	 vm.active_menu = service_.GetActiveMenu();
	 vm.set_active_menu = [this](TitleBarMenu menu) { service_.SetActiveMenu(menu); };
	 vm.trigger_command = [this](CommandId cmd) { service_.TriggerCommand(cmd); };
	 vm.request_block_tab_clicks_once = [this]() { service_.RequestBlockTabClicksOnce(); };
	 vm.primary_sidebar_visible = primary_sidebar_visible;
	 RenderTitleBarUI(window, vm, title_h);
 }

 ITitleBarService& TitleBarPart::GetService()
 {
	 return service_;
 }

 const ITitleBarService& TitleBarPart::GetService() const
 {
	 return service_;
 }
