/**
 * @file status_bar.cpp
 * @brief 状态栏工作台部件实现
 * @author Your Name
 * @date 2026-02-05
 */

#include "status_bar.h"
#include "../../ui/status_bar_ui.h"

StatusBarPart::StatusBarPart(INotificationService& service)
	: service_(service)
{
}

void StatusBarPart::Render(
	SDL_Window* window,
	float status_bar_h,
	float title_h,
	EditorTab* active_tab,
	const SceneStatusBarExtensionRenderer* scene_status_bar_extension,
	const std::function<void(EditorTab&, const std::string&)>* scene_status_bar_action_handler)
{
	StatusBarProps props{};
	const NotificationState& state = service_.GetState();
	props.message = state.message;
	props.progress = state.progress;
	props.active_tab = active_tab;
	props.scene_status_bar_extension = scene_status_bar_extension;

	const StatusBarResult result = RenderStatusBarUI(window, props, status_bar_h, title_h);
	for (const std::string& action_id : result.scene_actions) {
		if (!active_tab || !scene_status_bar_action_handler || !(*scene_status_bar_action_handler) || action_id.empty()) {
			continue;
		}
		(*scene_status_bar_action_handler)(*active_tab, action_id);
	}
}

INotificationService& StatusBarPart::GetService()
{
	return service_;
}

const INotificationService& StatusBarPart::GetService() const
{
	return service_;
}
