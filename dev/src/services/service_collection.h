#pragma once

#include <memory>
#include "activity_bar_service.h"
#include "primary_sidebar_service.h"
#include "secondary_sidebar_service.h"
#include "panel_service.h"
#include "editor_area_service.h"
#include "title_bar_service.h"
#include "notification_service.h"
#include "layout_service.h"
#include "view_registry.h"

struct ServiceOverrides {
	IActivityBarService* activity_bar = nullptr;
	IPrimarySidebarService* primary_sidebar = nullptr;
	ISecondarySidebarService* secondary_sidebar = nullptr;
	IPanelService* panel = nullptr;
	IEditorAreaService* editor_area = nullptr;
	ITitleBarService* title_bar = nullptr;
	INotificationService* notification = nullptr;
	ILayoutService* layout = nullptr;
	IViewRegistry* view_registry = nullptr;
};

class ServiceCollection {
public:
	ServiceCollection(const ServiceOverrides& overrides = {}) : overrides_(overrides) {
		// Eagerly initialize core services so dependencies are explicit and deterministic.
		(void)EnsureActivityBar();
		(void)EnsurePrimarySidebar();
		(void)EnsureSecondarySidebar();
		(void)EnsurePanel();
		(void)EnsureEditorArea();
		(void)EnsureTitleBar();
		(void)EnsureNotification();
		(void)EnsureLayout();
		(void)EnsureViewRegistry();
	}

	IActivityBarService& GetActivityBarService() { return *EnsureActivityBar(); }
	const IActivityBarService& GetActivityBarService() const { return *EnsureActivityBar(); }

	IPrimarySidebarService& GetPrimarySidebarService() { return *EnsurePrimarySidebar(); }
	const IPrimarySidebarService& GetPrimarySidebarService() const { return *EnsurePrimarySidebar(); }

	ISecondarySidebarService& GetSecondarySidebarService() { return *EnsureSecondarySidebar(); }
	const ISecondarySidebarService& GetSecondarySidebarService() const { return *EnsureSecondarySidebar(); }

	IPanelService& GetPanelService() { return *EnsurePanel(); }
	const IPanelService& GetPanelService() const { return *EnsurePanel(); }

	IEditorAreaService& GetEditorAreaService() { return *EnsureEditorArea(); }
	const IEditorAreaService& GetEditorAreaService() const { return *EnsureEditorArea(); }

	ITitleBarService& GetTitleBarService() { return *EnsureTitleBar(); }
	const ITitleBarService& GetTitleBarService() const { return *EnsureTitleBar(); }

	INotificationService& GetNotificationService() { return *EnsureNotification(); }
	const INotificationService& GetNotificationService() const { return *EnsureNotification(); }

	ILayoutService& GetLayoutService() { return *EnsureLayout(); }
	const ILayoutService& GetLayoutService() const { return *EnsureLayout(); }

	IViewRegistry& GetViewRegistry() { return *EnsureViewRegistry(); }
	const IViewRegistry& GetViewRegistry() const { return *EnsureViewRegistry(); }

private:
	IActivityBarService* EnsureActivityBar() const {
		if (!activity_bar_service_) {
			if (overrides_.activity_bar) {
				activity_bar_service_ = overrides_.activity_bar;
			} else {
				activity_bar_owned_ = std::make_unique<ActivityBarService>();
				activity_bar_service_ = activity_bar_owned_.get();
			}
		}
		return activity_bar_service_;
	}

	IPrimarySidebarService* EnsurePrimarySidebar() const {
		if (!primary_sidebar_service_) {
			if (overrides_.primary_sidebar) {
				primary_sidebar_service_ = overrides_.primary_sidebar;
			} else {
				primary_sidebar_owned_ = std::make_unique<PrimarySidebarService>();
				primary_sidebar_service_ = primary_sidebar_owned_.get();
			}
		}
		return primary_sidebar_service_;
	}

	ISecondarySidebarService* EnsureSecondarySidebar() const {
		if (!secondary_sidebar_service_) {
			if (overrides_.secondary_sidebar) {
				secondary_sidebar_service_ = overrides_.secondary_sidebar;
			} else {
				secondary_sidebar_owned_ = std::make_unique<SecondarySidebarService>();
				secondary_sidebar_service_ = secondary_sidebar_owned_.get();
			}
		}
		return secondary_sidebar_service_;
	}

	IPanelService* EnsurePanel() const {
		if (!panel_service_) {
			if (overrides_.panel) {
				panel_service_ = overrides_.panel;
			} else {
				panel_owned_ = std::make_unique<PanelService>();
				panel_service_ = panel_owned_.get();
			}
		}
		return panel_service_;
	}

	IEditorAreaService* EnsureEditorArea() const {
		if (!editor_area_service_) {
			if (overrides_.editor_area) {
				editor_area_service_ = overrides_.editor_area;
			} else {
				editor_area_owned_ = std::make_unique<EditorAreaService>();
				editor_area_service_ = editor_area_owned_.get();
			}
		}
		return editor_area_service_;
	}

	ITitleBarService* EnsureTitleBar() const {
		if (!title_bar_service_) {
			if (overrides_.title_bar) {
				title_bar_service_ = overrides_.title_bar;
			} else {
				title_bar_owned_ = std::make_unique<TitleBarService>();
				title_bar_service_ = title_bar_owned_.get();
			}
		}
		return title_bar_service_;
	}

	INotificationService* EnsureNotification() const {
		if (!notification_service_) {
			if (overrides_.notification) {
				notification_service_ = overrides_.notification;
			} else {
				notification_owned_ = std::make_unique<NotificationService>();
				notification_service_ = notification_owned_.get();
			}
		}
		return notification_service_;
	}

	ILayoutService* EnsureLayout() const {
		if (!layout_service_) {
			if (overrides_.layout) {
				layout_service_ = overrides_.layout;
			} else {
				layout_owned_ = std::make_unique<LayoutService>();
				layout_service_ = layout_owned_.get();
			}
		}
		return layout_service_;
	}

	IViewRegistry* EnsureViewRegistry() const {
		if (!view_registry_) {
			if (overrides_.view_registry) {
				view_registry_ = overrides_.view_registry;
			} else {
				view_registry_owned_ = std::make_unique<ViewRegistry>();
				view_registry_ = view_registry_owned_.get();
			}
		}
		return view_registry_;
	}

	ServiceOverrides overrides_{};
	mutable std::unique_ptr<ActivityBarService> activity_bar_owned_;
	mutable std::unique_ptr<PrimarySidebarService> primary_sidebar_owned_;
	mutable std::unique_ptr<SecondarySidebarService> secondary_sidebar_owned_;
	mutable std::unique_ptr<PanelService> panel_owned_;
	mutable std::unique_ptr<EditorAreaService> editor_area_owned_;
	mutable std::unique_ptr<TitleBarService> title_bar_owned_;
	mutable std::unique_ptr<NotificationService> notification_owned_;
	mutable std::unique_ptr<LayoutService> layout_owned_;
	mutable std::unique_ptr<ViewRegistry> view_registry_owned_;

	mutable IActivityBarService* activity_bar_service_ = nullptr;
	mutable IPrimarySidebarService* primary_sidebar_service_ = nullptr;
	mutable ISecondarySidebarService* secondary_sidebar_service_ = nullptr;
	mutable IPanelService* panel_service_ = nullptr;
	mutable IEditorAreaService* editor_area_service_ = nullptr;
	mutable ITitleBarService* title_bar_service_ = nullptr;
	mutable INotificationService* notification_service_ = nullptr;
	mutable ILayoutService* layout_service_ = nullptr;
	mutable IViewRegistry* view_registry_ = nullptr;
};
