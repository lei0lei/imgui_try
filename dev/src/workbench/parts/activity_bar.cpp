/**
 * @file activity_bar.cpp
 * @brief 活动栏工作台部件实现
 * @author Your Name
 * @date 2026-02-05
 */

 #include "activity_bar.h"

ActivityBarPart::ActivityBarPart(IActivityBarService& service)
	: service_(service)
{
}

 ActivityBarResult ActivityBarPart::Render(float title_h, float status_bar_h, float width)
 {
	 ActivityBarProps props{};
	 props.selected_index = service_.GetSelectedItem();
	 ActivityBarResult result = DrawActivityBarUI(title_h, status_bar_h, width, props);
	 if (result.item_clicked) {
		 service_.SetSelectedItem(static_cast<int>(result.selected_item));
	 }
	 return result;
 }

 IActivityBarService& ActivityBarPart::GetService()
 {
	 return service_;
 }

 const IActivityBarService& ActivityBarPart::GetService() const
 {
	 return service_;
 }
