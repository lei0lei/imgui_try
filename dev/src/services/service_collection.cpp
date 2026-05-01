#include "service_collection.h"

#include "activity_bar_service.h"
#include "editor_area_service.h"
#include "layout_service.h"
#include "notification_service.h"
#include "primary_sidebar_service.h"
#include "title_bar_service.h"
#include "view_registry.h"

ServiceCollection::ServiceCollection(const ServiceOverrides& overrides)
    : overrides_(overrides)
{
    activity_bar_ = overrides_.activity_bar
        ? overrides_.activity_bar
        : std::static_pointer_cast<IActivityBarService>(std::make_shared<ActivityBarService>());
    primary_sidebar_ = overrides_.primary_sidebar
        ? overrides_.primary_sidebar
        : std::static_pointer_cast<IPrimarySidebarService>(std::make_shared<PrimarySidebarService>());
    editor_area_ = overrides_.editor_area
        ? overrides_.editor_area
        : std::static_pointer_cast<IEditorAreaService>(std::make_shared<EditorAreaService>());
    title_bar_ = overrides_.title_bar
        ? overrides_.title_bar
        : std::shared_ptr<ITitleBarService>(std::make_shared<TitleBarService>());
    notification_ = overrides_.notification
        ? overrides_.notification
        : std::shared_ptr<INotificationService>(std::make_shared<NotificationService>());
    layout_ = overrides_.layout
        ? overrides_.layout
        : std::shared_ptr<ILayoutService>(std::make_shared<LayoutService>());
    view_registry_ = overrides_.view_registry
        ? overrides_.view_registry
        : std::static_pointer_cast<IViewRegistry>(std::make_shared<ViewRegistry>());
    event_bus_ = overrides_.event_bus
        ? overrides_.event_bus
        : std::static_pointer_cast<IEventBus>(std::make_shared<EventBus>());
}
