/**
 * @file view_registry_defaults_config.cpp
 * @brief 默认视图注册表组件的配置和设置
 * @author Your Name
 * @date 2026-02-05
 */

#include "view_registry_defaults_config.h"
#include "imgui.h"
#include "../workbench/workbench_config.h"
#include "../scenes/scene_plugin_registry.h"
#include <algorithm>
#include <deque>
#include <string>
#include <utility>

namespace UI {

namespace {
std::deque<std::string> g_pending_scene_tab_requests;
const std::string kPrimarySidebarGlobalPluginId = "__project_primary__";
}

void RequestCreateSceneTab(const std::string& plugin_id)
{
    if (!plugin_id.empty()) {
        g_pending_scene_tab_requests.push_back(plugin_id);
    }
}

bool ConsumeCreateSceneTabRequest(std::string& plugin_id)
{
    if (g_pending_scene_tab_requests.empty()) {
        return false;
    }
    plugin_id = g_pending_scene_tab_requests.front();
    g_pending_scene_tab_requests.pop_front();
    return true;
}

namespace {

void DrawPlaceholderHeader(const char* title, const ImVec4& color)
{
    ImGui::TextColored(color, "%s", title);
    ImGui::Separator();
    ImGui::Spacing();
}

void DrawEmptyState(const char* message)
{
    ImGui::TextWrapped("%s", message);
}

std::string TruncateTextToWidth(const char* text, float max_width)
{
    if (!text) {
        return "";
    }
    const std::string full(text);
    if (ImGui::CalcTextSize(full.c_str()).x <= max_width) {
        return full;
    }

    static const char* ellipsis = "...";
    std::string out = full;
    while (!out.empty()) {
        out.pop_back();
        std::string candidate = out + ellipsis;
        if (ImGui::CalcTextSize(candidate.c_str()).x <= max_width) {
            return candidate;
        }
    }
    return ellipsis;
}

void RenderExplorer(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("EXPLORER", colors.primary_sidebar_text_dim);
    ImGui::Text("No folder opened");
    ImGui::Spacing();
    ImGui::Button("Open Folder");
}

void RenderSearch(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("SEARCH", colors.primary_sidebar_text_dim);
    static char search_buf[256] = "";
    ImGui::InputTextWithHint("##search", "Search...", search_buf, sizeof(search_buf));
    ImGui::Spacing();
    ImGui::Text("Search across files");
}

void RenderDebug(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("RUN AND DEBUG", colors.primary_sidebar_text_dim);
    DrawEmptyState("No configurations");
}

void RenderEditor(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("EDITOR", colors.primary_sidebar_text_dim);

    Scenes::ScenePluginRegistry::Instance().EnsureLoaded();
    const auto& plugins = Scenes::ScenePluginRegistry::Instance().GetPlugins();

    ImGui::TextColored(colors.primary_sidebar_text_dim, "Scene Plugins");
    ImGui::Spacing();

    const float item_h = ImGui::GetTextLineHeight() * 3.35f;
    const float icon_size = ImGui::GetTextLineHeight() * 2.2f;
    const float outer_pad_x = 10.0f;
    const float inner_gap = 10.0f;

    for (const auto& plugin : plugins) {
        ImGui::PushID(plugin.id.c_str());
        const ImVec2 cursor = ImGui::GetCursorScreenPos();
        const float width = ImGui::GetContentRegionAvail().x;

        ImGui::InvisibleButton("extension_item", ImVec2(width, item_h));
        const bool hovered = ImGui::IsItemHovered();

        ImDrawList* draw = ImGui::GetWindowDrawList();
        const ImVec2 min = cursor;
        const ImVec2 max = ImVec2(cursor.x + width, cursor.y + item_h);
        const ImU32 row_bg = hovered
            ? ImGui::GetColorU32(colors.title_bar_menu_item_hover)
            : ImGui::GetColorU32(colors.primary_sidebar_bg);
        const ImU32 row_border = ImGui::GetColorU32(colors.primary_sidebar_border);
        draw->AddRectFilled(min, max, row_bg, 4.0f);
        draw->AddRect(min, max, row_border, 4.0f, 0, 1.0f);

        const ImVec2 icon_min = ImVec2(min.x + outer_pad_x, min.y + (item_h - icon_size) * 0.5f);
        const ImVec2 icon_max = ImVec2(icon_min.x + icon_size, icon_min.y + icon_size);
        draw->AddRectFilled(icon_min, icon_max, ImGui::GetColorU32(colors.activity_bar_active), 4.0f);
        const ImVec2 icon_text_size = ImGui::CalcTextSize(plugin.icon_text.c_str());
        const ImVec2 icon_text_pos = ImVec2(
            icon_min.x + (icon_size - icon_text_size.x) * 0.5f,
            icon_min.y + (icon_size - icon_text_size.y) * 0.5f
        );
        draw->AddText(icon_text_pos, ImGui::GetColorU32(colors.activity_bar_icon), plugin.icon_text.c_str());

        const float text_x = icon_max.x + inner_gap;
        const float text_w = max.x - text_x - outer_pad_x;
        const float line_h = ImGui::GetTextLineHeight();
        const float text_y = min.y + 6.0f;

        draw->AddText(ImVec2(text_x, text_y), ImGui::GetColorU32(colors.title_bar_text), plugin.name.c_str());

        const std::string info_line = TruncateTextToWidth(plugin.info.c_str(), text_w);
        draw->AddText(ImVec2(text_x, text_y + line_h), ImGui::GetColorU32(colors.primary_sidebar_text_dim), info_line.c_str());

        const std::string author_line = std::string("by ") + plugin.author;
        draw->AddText(ImVec2(text_x, text_y + line_h * 2.0f), ImGui::GetColorU32(colors.primary_sidebar_text_dim), author_line.c_str());

        if (ImGui::BeginPopupContextItem("extension_scene_context")) {
            if (ImGui::MenuItem("New Tab")) {
                RequestCreateSceneTab(plugin.id);
            }
            ImGui::EndPopup();
        }

        ImGui::Dummy(ImVec2(0.0f, 6.0f));
        ImGui::PopID();
    }

    ImGui::Spacing();
    ImGui::Separator();
    if (plugins.empty()) {
        DrawEmptyState("No scene plugins found under src/scenes. Add a scene.plugin file to each scene folder.");
    } else {
        DrawEmptyState("Right-click a scene plugin and choose New Tab.");
    }
}

void RenderExtensions(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("EXTENSIONS", colors.primary_sidebar_text_dim);
    DrawEmptyState("Extension marketplace");
}

void RenderPanelProblems(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "No problems detected");
}

void RenderPanelTerminal(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_terminal_prompt, "$ ");
    ImGui::SameLine();
    ImGui::TextColored(colors.panel_text, "Ready");
}

void RenderPanelConsole(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "Console output goes here.");
}

void RenderPanelDebug(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "Debug console");
}

void RenderPanelTimeline(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "Timeline is empty.");
}

void RenderPanelProfiler(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    ImGui::TextColored(colors.panel_text, "Profiler idle.");
}

} // namespace

namespace {

std::vector<EditorViewPlugin>& PluginRegistry()
{
    static std::vector<EditorViewPlugin> plugins;
    return plugins;
}

void EnsureDefaultPlugins()
{
    auto& plugins = PluginRegistry();
    if (!plugins.empty())
        return;

    EditorViewPlugin global_primary_plugin{};
    global_primary_plugin.plugin_id = kPrimarySidebarGlobalPluginId;
    global_primary_plugin.primary_views.push_back({ "explorer", "Explorer", RenderExplorer });
    global_primary_plugin.primary_views.push_back({ "search", "Search", RenderSearch });
    global_primary_plugin.primary_views.push_back({ "debug", "Debug", RenderDebug });
    global_primary_plugin.primary_views.push_back({ "editor", "Editor", RenderEditor });
    global_primary_plugin.primary_views.push_back({ "extensions", "Extensions", RenderExtensions });
    global_primary_plugin.primary_bindings = {
        { ActivityBarItem::Explorer, "explorer" },
        { ActivityBarItem::Search, "search" },
        { ActivityBarItem::Debug, "debug" },
        { ActivityBarItem::Editor, "editor" },
        { ActivityBarItem::Extensions, "extensions" }
    };
    plugins.push_back(std::move(global_primary_plugin));

    Scenes::ScenePluginRegistry::Instance().EnsureLoaded();
    const auto& scene_plugins = Scenes::ScenePluginRegistry::Instance().GetPlugins();

    for (const auto& scene_plugin : scene_plugins) {
        const auto* scene_views = Scenes::ScenePluginRegistry::Instance().GetViews(scene_plugin.id);

        EditorViewPlugin plugin{};
        plugin.plugin_id = scene_plugin.id;

        if (scene_views) {
            for (const auto& view : scene_views->secondary_views) {
                if (!view.renderer) {
                    continue;
                }
                plugin.secondary_views.push_back({ view.id, view.title, view.renderer });
            }

            for (const auto& view : scene_views->panel_views) {
                if (!view.renderer) {
                    continue;
                }
                plugin.panel_views.push_back({ view.id, view.title, view.renderer });
            }

            plugin.default_secondary_id = !scene_views->default_secondary_id.empty()
                ? scene_views->default_secondary_id
                : (!plugin.secondary_views.empty() ? plugin.secondary_views.front().id : "");
            plugin.default_panel_id = !scene_views->default_panel_id.empty()
                ? scene_views->default_panel_id
                : (!plugin.panel_views.empty() ? plugin.panel_views.front().id : "");
        }

        plugins.push_back(std::move(plugin));
    }
}

const EditorViewPlugin* FindPluginById(const std::string& plugin_id)
{
    EnsureDefaultPlugins();
    const auto& plugins = PluginRegistry();
    for (const auto& plugin : plugins) {
        if (plugin.plugin_id == plugin_id) {
            return &plugin;
        }
    }
    return nullptr;
}

const ViewDefinition* FindViewById(const std::vector<ViewDefinition>& list, const std::string& id)
{
    for (const auto& view : list) {
        if (view.id == id) {
            return &view;
        }
    }
    return nullptr;
}

std::string GetPrimaryIdForActivity(const EditorViewPlugin& plugin, ActivityBarItem item)
{
    for (const auto& binding : plugin.primary_bindings) {
        if (binding.first == item) {
            return binding.second;
        }
    }
    return {};
}

}

const std::vector<DefaultViewConfig>& GetDefaultViewConfigs()
{
    static std::vector<DefaultViewConfig> configs;
    if (!configs.empty())
        return configs;

    EnsureDefaultPlugins();
    for (const auto& plugin : PluginRegistry()) {
        for (const auto& view : plugin.primary_views) {
            configs.push_back({ plugin.plugin_id, ViewContainer::PrimarySidebar, view.id, view.title, view.renderer });
        }
        for (const auto& view : plugin.secondary_views) {
            configs.push_back({ plugin.plugin_id, ViewContainer::SecondarySidebar, view.id, view.title, view.renderer });
        }
        for (const auto& view : plugin.panel_views) {
            configs.push_back({ plugin.plugin_id, ViewContainer::Panel, view.id, view.title, view.renderer });
        }
    }

    return configs;
}

void RegisterViewPlugin(const EditorViewPlugin& plugin)
{
    EnsureDefaultPlugins();
    auto& plugins = PluginRegistry();
    for (auto& existing : plugins) {
        if (existing.plugin_id == plugin.plugin_id) {
            existing = plugin;
            return;
        }
    }
    plugins.push_back(plugin);
}

std::string GetDefaultScenePluginId()
{
    EnsureDefaultPlugins();
    const auto& plugins = PluginRegistry();
    for (const auto& plugin : plugins) {
        if (plugin.plugin_id != kPrimarySidebarGlobalPluginId) {
            return plugin.plugin_id;
        }
    }
    return {};
}

const std::string& GetPrimarySidebarGlobalPluginId()
{
    return kPrimarySidebarGlobalPluginId;
}

const EditorViewPlugin* GetViewPluginByPluginId(const std::string& plugin_id)
{
    return FindPluginById(plugin_id);
}

ViewDefinition GetDefaultPrimaryViewForPlugin(const std::string& plugin_id, ActivityBarItem item)
{
    (void)plugin_id;
    const auto* plugin = FindPluginById(kPrimarySidebarGlobalPluginId);
    if (!plugin) {
        return { "empty", "Empty", nullptr };
    }
    const std::string id = GetPrimaryIdForActivity(*plugin, item);
    if (id.empty()) {
        return { "empty", "Empty", nullptr };
    }
    const auto* view = FindViewById(plugin->primary_views, id);
    if (!view) {
        return { "empty", "Empty", nullptr };
    }
    return { view->id, view->title, view->renderer };
}

ViewDefinition GetDefaultSecondaryViewForPlugin(const std::string& plugin_id)
{
    const auto* plugin = FindPluginById(plugin_id);
    if (!plugin) {
        return { "empty", "Empty", nullptr };
    }
    const auto* view = FindViewById(plugin->secondary_views, plugin->default_secondary_id);
    if (!view) {
        return { "empty", "Empty", nullptr };
    }
    return { view->id, view->title, view->renderer };
}

ViewDefinition GetDefaultPanelViewForPlugin(const std::string& plugin_id)
{
    const auto* plugin = FindPluginById(plugin_id);
    if (!plugin) {
        return { "empty", "Empty", nullptr };
    }
    const auto* view = FindViewById(plugin->panel_views, plugin->default_panel_id);
    if (!view) {
        return { "empty", "Empty", nullptr };
    }
    return { view->id, view->title, view->renderer };
}

} // namespace UI
