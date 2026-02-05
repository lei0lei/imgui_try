#pragma once

#include <string>
#include <vector>
#include "../core/workbench_types.h"
#include "../command/command_ids.h"

struct LayoutState;
struct NotificationState;
struct EditorTab;
struct ViewDefinition;
enum class SceneType;
enum class ViewContainer;
enum class PanelTab;

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

class ISecondarySidebarService {
public:
    virtual ~ISecondarySidebarService() = default;
    virtual bool IsVisible() const = 0;
    virtual void SetVisible(bool visible) = 0;
};

class IPanelService {
public:
    virtual ~IPanelService() = default;
    virtual void Reset() = 0;
    virtual void SetVisible(bool v) = 0;
    virtual bool IsVisible() const = 0;
    virtual void ToggleVisible() = 0;
    virtual void SetActiveTab(PanelTab tab) = 0;
    virtual PanelTab GetActiveTab() const = 0;
};

class ILayoutService {
public:
    virtual ~ILayoutService() = default;
    virtual bool IsPrimarySidebarVisible() const = 0;
    virtual void SetPrimarySidebarVisible(bool visible) = 0;
    virtual void TogglePrimarySidebar() = 0;
    virtual bool IsSecondarySidebarVisible() const = 0;
    virtual void SetSecondarySidebarVisible(bool visible) = 0;
    virtual void ToggleSecondarySidebar() = 0;
    virtual bool IsPanelVisible() const = 0;
    virtual void SetPanelVisible(bool visible) = 0;
    virtual void TogglePanel() = 0;
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
    virtual SceneType GetActiveSceneType(SceneType fallback) const = 0;
    virtual void AddTab(const EditorTab& tab) = 0;
    virtual void CloseTab(int index) = 0;
    virtual void ActivateTab(int index) = 0;
    virtual void MoveTab(int from_index, int to_index) = 0;
};

class IViewRegistry {
public:
    virtual ~IViewRegistry() = default;
    virtual void RegisterView(SceneType mode, ViewContainer container, const ViewDefinition& view) = 0;
    virtual const std::vector<ViewDefinition>& GetViews(SceneType mode, ViewContainer container) const = 0;
    virtual int GetActiveViewIndex(SceneType mode, ViewContainer container) const = 0;
    virtual void SetActiveViewIndex(SceneType mode, ViewContainer container, int index) = 0;
    virtual void SetActiveViewById(SceneType mode, ViewContainer container, const std::string& id) = 0;
    virtual const ViewDefinition* GetActiveView(SceneType mode, ViewContainer container) const = 0;
};
