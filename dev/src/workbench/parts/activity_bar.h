 #pragma once

 #include "../../services/activity_bar_service.h"
 #include "../../ui/activity_bar_ui.h"

 class ActivityBarPart {
 public:
	explicit ActivityBarPart(ActivityBarService& service);

	 ActivityBarResult Render(float title_h, float status_bar_h, float width);

	ActivityBarService& GetService();
	const ActivityBarService& GetService() const;

 private:
	ActivityBarService& service_;
 };
