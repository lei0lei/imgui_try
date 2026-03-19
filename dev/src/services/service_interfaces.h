#pragma once

#include <string>
#include <vector>
#include "../core/workbench_types.h"
#include "../command/command_ids.h"
#include "editor_tab.h"
#include "view_types.h"

struct LayoutState;
struct NotificationState;

class IActivityBarService {
public:
    virtual ~IActivityBarService() = default;
    virtual int GetSelectedItem() const = 0;
    virtual void SetSelectedItem(int index) = 0;
    virtual const std::vector<std::string>& GetItems() const = 0;
    virtual void SetItems(const std::vector<std::string>& items) = 0;
};

class IPrimarySidebarService {
public:
    virtual ~IPrimarySidebarService() = default;
    virtual int GetSelectedItem() const = 0;
    virtual void SetSelectedItem(int index) = 0;
    virtual bool IsVisible() const = 0;
    virtual void SetVisible(bool visible) = 0;
    virtual void ToggleVisible() = 0;
    virtual const std::vector<std::string>& GetItems() const = 0;
    virtual void SetItems(const std::vector<std::string>& items) = 0;
};

class ILayoutService {
public:
    virtual ~ILayoutService() = default;
    virtual bool IsPrimarySidebarVisible() const = 0;
    virtual void SetPrimarySidebarVisible(bool visible) = 0;
    virtual void TogglePrimarySidebar() = 0;
    virtual const LayoutState& GetState() const = 0;
};

class IEditorAreaService;
class ITitleBarService;
class INotificationService;
class IViewRegistry;

class ITitleBarService {
public:
    virtual ~ITitleBarService() = default;
    virtual void SetActiveMenu(TitleBarMenu menu) = 0;
    virtual TitleBarMenu GetActiveMenu() const = 0;
    virtual void TriggerCommand(CommandId cmd) = 0;
    virtual CommandId ConsumeLastCommand() = 0;
    virtual void RequestBlockTabClicksOnce() = 0;
    virtual bool ConsumeBlockTabClicksOnce() = 0;
};

class INotificationService {
public:
    virtual ~INotificationService() = default;
    virtual void SetMessage(const std::string& message) = 0;
    virtual const NotificationState& GetState() const = 0;
};

class IEditorAreaService {
public:
    virtual ~IEditorAreaService() = default;
    virtual const std::vector<EditorTab>& GetTabs() const = 0;
    virtual std::vector<EditorTab>& GetTabs() = 0;
    virtual int GetActiveTabIndex() const = 0;
    virtual EditorTab* GetActiveTab() = 0;
    virtual const EditorTab* GetActiveTab() const = 0;
    virtual void AddTab(const EditorTab& tab) = 0;
    virtual void CloseTab(int index) = 0;
    virtual void ActivateTab(int index) = 0;
    virtual void MoveTab(int from_index, int to_index) = 0;
};

class IViewRegistry {
public:
    virtual ~IViewRegistry() = default;
    virtual void RegisterView(const std::string& scene_key, const ViewDefinition& view) = 0;
    virtual const std::vector<ViewDefinition>& GetViews(const std::string& scene_key) const = 0;
    virtual int GetActiveViewIndex(const std::string& scene_key) const = 0;
    virtual void SetActiveViewIndex(const std::string& scene_key, int index) = 0;
    virtual void SetActiveViewById(const std::string& scene_key, const std::string& id) = 0;
    virtual const ViewDefinition* GetActiveView(const std::string& scene_key) const = 0;
};
