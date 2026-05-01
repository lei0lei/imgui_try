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
	bool primary_sidebar_visible,
	EditorTab* active_tab,
	const SceneTitleBarExtensionRenderer* scene_titlebar_extension,
	const std::function<void(EditorTab&, const std::string&)>* scene_titlebar_action_handler,
	float scene_titlebar_extension_width)
{
	TitleBarProps props{};
	props.active_menu = service_.GetActiveMenu();
	props.primary_sidebar_visible = primary_sidebar_visible;
	props.active_tab = active_tab;
	props.scene_titlebar_extension = scene_titlebar_extension;
	props.scene_titlebar_extension_width = scene_titlebar_extension_width;

	const TitleBarResult result = RenderTitleBarUI(window, props, title_h);

	if (result.menu != props.active_menu) {
		service_.SetActiveMenu(result.menu);
	}
	for (CommandId cmd : result.commands) {
		service_.TriggerCommand(cmd);
	}
	for (const std::string& action_id : result.scene_actions) {
		if (!active_tab || !scene_titlebar_action_handler || !(*scene_titlebar_action_handler) || action_id.empty()) {
			continue;
		}
		(*scene_titlebar_action_handler)(*active_tab, action_id);
	}
	if (result.block_tab_clicks_once) {
		service_.RequestBlockTabClicksOnce();
	}
}

ITitleBarService& TitleBarPart::GetService()
{
	return service_;
}

const ITitleBarService& TitleBarPart::GetService() const
{
	return service_;
}
