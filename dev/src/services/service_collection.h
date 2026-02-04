#pragma once

#include "activity_bar_service.h"
#include "primary_sidebar_service.h"
#include "secondary_sidebar_service.h"
#include "panel_service.h"
#include "editor_area_service.h"
#include "title_bar_service.h"
#include "notification_service.h"
#include "layout_service.h"
#include "view_registry.h"

class ServiceCollection {
public:
	ActivityBarService& GetActivityBarService() { return activity_bar_service_; }
	const ActivityBarService& GetActivityBarService() const { return activity_bar_service_; }

	PrimarySidebarService& GetPrimarySidebarService() { return primary_sidebar_service_; }
	const PrimarySidebarService& GetPrimarySidebarService() const { return primary_sidebar_service_; }

	SecondarySidebarService& GetSecondarySidebarService() { return secondary_sidebar_service_; }
	const SecondarySidebarService& GetSecondarySidebarService() const { return secondary_sidebar_service_; }

	PanelService& GetPanelService() { return panel_service_; }
	const PanelService& GetPanelService() const { return panel_service_; }

	EditorAreaService& GetEditorAreaService() { return editor_area_service_; }
	const EditorAreaService& GetEditorAreaService() const { return editor_area_service_; }

	TitleBarService& GetTitleBarService() { return title_bar_service_; }
	const TitleBarService& GetTitleBarService() const { return title_bar_service_; }

	NotificationService& GetNotificationService() { return notification_service_; }
	const NotificationService& GetNotificationService() const { return notification_service_; }

	LayoutService& GetLayoutService() { return layout_service_; }
	const LayoutService& GetLayoutService() const { return layout_service_; }

	ViewRegistry& GetViewRegistry() { return view_registry_; }
	const ViewRegistry& GetViewRegistry() const { return view_registry_; }

private:
	ActivityBarService activity_bar_service_;
	PrimarySidebarService primary_sidebar_service_;
	SecondarySidebarService secondary_sidebar_service_;
	PanelService panel_service_;
	EditorAreaService editor_area_service_;
	TitleBarService title_bar_service_;
	NotificationService notification_service_;
	LayoutService layout_service_;
	ViewRegistry view_registry_;
};
