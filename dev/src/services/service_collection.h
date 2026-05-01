#pragma once

#include <memory>
#include "service_interfaces.h"
#include "event_bus.h"

struct ServiceOverrides {
	// Ownership is explicit: if you pass an override here, the ServiceCollection will keep it alive.
	std::shared_ptr<IActivityBarService> activity_bar{};
	std::shared_ptr<IPrimarySidebarService> primary_sidebar{};
	std::shared_ptr<IEditorAreaService> editor_area{};
	std::shared_ptr<ITitleBarService> title_bar{};
	std::shared_ptr<INotificationService> notification{};
	std::shared_ptr<ILayoutService> layout{};
	std::shared_ptr<IViewRegistry> view_registry{};
	std::shared_ptr<IEventBus> event_bus{};
};

class ServiceCollection {
public:
	ServiceCollection(const ServiceOverrides& overrides = {});

	IActivityBarService& GetActivityBarService() { return *activity_bar_; }
	const IActivityBarService& GetActivityBarService() const { return *activity_bar_; }

	IPrimarySidebarService& GetPrimarySidebarService() { return *primary_sidebar_; }
	const IPrimarySidebarService& GetPrimarySidebarService() const { return *primary_sidebar_; }

	IEditorAreaService& GetEditorAreaService() { return *editor_area_; }
	const IEditorAreaService& GetEditorAreaService() const { return *editor_area_; }

	ITitleBarService& GetTitleBarService() { return *title_bar_; }
	const ITitleBarService& GetTitleBarService() const { return *title_bar_; }

	INotificationService& GetNotificationService() { return *notification_; }
	const INotificationService& GetNotificationService() const { return *notification_; }

	ILayoutService& GetLayoutService() { return *layout_; }
	const ILayoutService& GetLayoutService() const { return *layout_; }

	IViewRegistry& GetViewRegistry() { return *view_registry_; }
	const IViewRegistry& GetViewRegistry() const { return *view_registry_; }

	IEventBus& GetEventBus() { return *event_bus_; }
	const IEventBus& GetEventBus() const { return *event_bus_; }

private:
	ServiceOverrides overrides_{};

	std::shared_ptr<IActivityBarService> activity_bar_{};
	std::shared_ptr<IPrimarySidebarService> primary_sidebar_{};
	std::shared_ptr<IEditorAreaService> editor_area_{};
	std::shared_ptr<ITitleBarService> title_bar_{};
	std::shared_ptr<INotificationService> notification_{};
	std::shared_ptr<ILayoutService> layout_{};
	std::shared_ptr<IViewRegistry> view_registry_{};
	std::shared_ptr<IEventBus> event_bus_{};
};
