 #include "activity_bar.h"

ActivityBarPart::ActivityBarPart(ActivityBarService& service)
	: service_(service)
{
}

 ActivityBarResult ActivityBarPart::Render(float title_h, float status_bar_h, float width)
 {
	 return DrawActivityBarUI(title_h, status_bar_h, width, service_);
 }

 ActivityBarService& ActivityBarPart::GetService()
 {
	 return service_;
 }

 const ActivityBarService& ActivityBarPart::GetService() const
 {
	 return service_;
 }
