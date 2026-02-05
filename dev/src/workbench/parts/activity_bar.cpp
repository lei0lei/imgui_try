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
	 ActivityBarViewModel vm{};
	 vm.selected_index = service_.GetSelectedItem();
	 vm.on_select = [this](int index) { service_.SetSelectedItem(index); };
	 return DrawActivityBarUI(title_h, status_bar_h, width, vm);
 }

 IActivityBarService& ActivityBarPart::GetService()
 {
	 return service_;
 }

 const IActivityBarService& ActivityBarPart::GetService() const
 {
	 return service_;
 }
