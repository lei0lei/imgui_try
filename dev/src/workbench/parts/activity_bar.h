 #pragma once

#include "../../services/service_interfaces.h"
 #include "../../ui/activity_bar_ui.h"

 class ActivityBarPart {
 public:
	explicit ActivityBarPart(IActivityBarService& service);

	 ActivityBarResult Render(float title_h, float status_bar_h, float width);

	IActivityBarService& GetService();
	const IActivityBarService& GetService() const;

 private:
	IActivityBarService& service_;
 };
