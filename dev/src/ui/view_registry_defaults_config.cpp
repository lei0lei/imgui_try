/**
 * @file view_registry_defaults_config.cpp
 * @brief 默认视图注册表组件的配置和设置
 * @author Your Name
 * @date 2026-02-05
 */

#include "view_registry_defaults_config.h"
#include "folder_dialog.h"
#include "imgui.h"
#include "../workbench/workbench_config.h"
#include "../scenes/scene_plugin_registry.h"
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <deque>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>

namespace UI {

namespace {
std::deque<std::string> g_pending_scene_tab_requests;
const std::string kPrimarySidebarGlobalPluginId = "__project_primary__";
const char* kDebugWatchListKey = "debug.watch.list";

struct SearchHit {
    std::string file;
    int line = 0;
    std::string snippet;
};

std::string g_explorer_root_path;
std::string g_explorer_selected_path;
std::vector<SearchHit> g_search_hits;
std::string g_last_search_query;
std::string g_last_search_root;
bool g_search_hits_truncated = false;
bool g_search_scan_truncated = false;
int g_search_files_scanned = 0;
double g_search_last_input_change_time = 0.0;
std::string g_search_last_input_snapshot;

constexpr int kSearchMaxHits = 200;
constexpr int kSearchMaxFiles = 1200;
constexpr int kSearchMaxLinesPerFile = 5000;
constexpr std::uintmax_t kSearchMaxFileBytes = 1024 * 1024;
constexpr double kSearchDebounceSeconds = 0.35;

// Training sidebar: paginated subfolder list + configurable project path
std::string g_training_selected_data_path;
char g_training_project_path_buf[2048] = {};
int g_training_data_page = 0;
constexpr int kTrainingFoldersPerPage = 10;
std::string g_training_cached_root;
std::vector<std::string> g_training_subdirs_sorted;
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

void SetExplorerRootPath(const std::string& root_path)
{
    g_explorer_root_path = root_path;
    g_training_cached_root.clear();
}

const std::string& GetExplorerRootPath()
{
    return g_explorer_root_path;
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

std::vector<std::string> ParseWatchList(const std::string& raw)
{
    std::vector<std::string> out;
    size_t start = 0;
    while (start <= raw.size()) {
        const size_t sep = raw.find(';', start);
        const size_t end = (sep == std::string::npos) ? raw.size() : sep;
        std::string token = raw.substr(start, end - start);

        size_t left = 0;
        while (left < token.size() && (token[left] == ' ' || token[left] == '\t')) {
            ++left;
        }
        size_t right = token.size();
        while (right > left && (token[right - 1] == ' ' || token[right - 1] == '\t')) {
            --right;
        }
        token = token.substr(left, right - left);

        if (!token.empty()) {
            out.push_back(std::move(token));
        }

        if (sep == std::string::npos) {
            break;
        }
        start = sep + 1;
    }
    return out;
}

std::string JoinWatchList(const std::vector<std::string>& list)
{
    std::string raw;
    for (size_t i = 0; i < list.size(); ++i) {
        if (i > 0) {
            raw += ';';
        }
        raw += list[i];
    }
    return raw;
}

std::string ToLowerAscii(const std::string& value)
{
    std::string out = value;
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return out;
}

bool IsLikelyBinaryExtension(const std::filesystem::path& path)
{
    const std::string ext = ToLowerAscii(path.extension().string());
    static const std::vector<std::string> binary_exts = {
        ".exe", ".dll", ".lib", ".obj", ".pdb", ".png", ".jpg", ".jpeg", ".gif", ".bmp", ".ico", ".ttf", ".otf", ".wav", ".mp3", ".mp4", ".zip"
    };
    return std::find(binary_exts.begin(), binary_exts.end(), ext) != binary_exts.end();
}

void EnsureExplorerRoot()
{
    if (!g_explorer_root_path.empty()) {
        return;
    }
    std::error_code ec;
    const std::filesystem::path cwd = std::filesystem::current_path(ec);
    if (!ec) {
        g_explorer_root_path = cwd.string();
    }
}

void DrawExplorerTree(const std::filesystem::path& root, const std::filesystem::path& path, int depth)
{
    if (depth > 6) {
        return;
    }

    std::error_code ec;
    std::vector<std::filesystem::directory_entry> dirs;
    std::vector<std::filesystem::directory_entry> files;
    for (const auto& entry : std::filesystem::directory_iterator(path, ec)) {
        if (ec) {
            break;
        }
        if (entry.is_directory(ec)) {
            dirs.push_back(entry);
        } else if (entry.is_regular_file(ec)) {
            files.push_back(entry);
        }
    }

    auto by_name = [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
        return a.path().filename().string() < b.path().filename().string();
    };
    std::sort(dirs.begin(), dirs.end(), by_name);
    std::sort(files.begin(), files.end(), by_name);

    int shown = 0;
    for (const auto& dir : dirs) {
        if (++shown > 200) {
            break;
        }
        const std::string label = dir.path().filename().string();
        const bool open = ImGui::TreeNode(label.c_str());
        if (ImGui::IsItemClicked()) {
            g_explorer_selected_path = dir.path().string();
        }
        if (open) {
            DrawExplorerTree(root, dir.path(), depth + 1);
            ImGui::TreePop();
        }
    }

    for (const auto& file : files) {
        if (++shown > 200) {
            break;
        }
        (void)root;
        const std::string display = file.path().filename().string();
        const bool selected = (g_explorer_selected_path == file.path().string());
        if (ImGui::Selectable(display.c_str(), selected)) {
            g_explorer_selected_path = file.path().string();
        }
    }
}

void RunSearch(const std::string& root, const std::string& query)
{
    g_search_hits.clear();
    g_search_hits_truncated = false;
    g_search_scan_truncated = false;
    g_search_files_scanned = 0;
    if (root.empty() || query.empty()) {
        return;
    }

    const std::string q = ToLowerAscii(query);
    std::error_code ec;
    int total_hits = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root, ec)) {
        if (ec || total_hits >= kSearchMaxHits || g_search_files_scanned >= kSearchMaxFiles) {
            if (total_hits >= kSearchMaxHits) g_search_hits_truncated = true;
            if (g_search_files_scanned >= kSearchMaxFiles) g_search_scan_truncated = true;
            break;
        }
        if (!entry.is_regular_file(ec) || IsLikelyBinaryExtension(entry.path())) {
            continue;
        }

        const std::uintmax_t file_size = entry.file_size(ec);
        if (!ec && file_size > kSearchMaxFileBytes) {
            continue;
        }

        ++g_search_files_scanned;

        std::ifstream in(entry.path());
        if (!in.is_open()) {
            continue;
        }

        std::string line;
        int line_no = 0;
        while (std::getline(in, line)) {
            ++line_no;
            if (line_no > kSearchMaxLinesPerFile) {
                break;
            }
            if (ToLowerAscii(line).find(q) != std::string::npos) {
                SearchHit hit;
                hit.file = entry.path().string();
                hit.line = line_no;
                hit.snippet = line;
                g_search_hits.push_back(std::move(hit));
                ++total_hits;
                if (total_hits >= kSearchMaxHits) {
                    g_search_hits_truncated = true;
                    break;
                }
            }
        }
    }
}

void RenderExplorer(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("EXPLORER", colors.primary_sidebar_text_dim);
    if (g_explorer_root_path.empty()) {
        ImGui::TextColored(colors.primary_sidebar_text_dim, "No folder opened");
        return;
    }

    if (g_explorer_root_path.empty() || !std::filesystem::exists(g_explorer_root_path)) {
        ImGui::TextColored(colors.primary_sidebar_text_dim, "Root path not found");
        return;
    }

    std::filesystem::path root_path(g_explorer_root_path);
    std::string folder_name = root_path.filename().string();
    if (folder_name.empty()) {
        folder_name = g_explorer_root_path;
    }
    ImGui::Text("%s", folder_name.c_str());
    ImGui::TextColored(colors.primary_sidebar_text_dim, "%s", g_explorer_root_path.c_str());
    ImGui::Separator();

    DrawExplorerTree(root_path, root_path, 0);
}

void RenderSearch(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("SEARCH", colors.primary_sidebar_text_dim);

    static char search_buf[256] = "";
    ImGui::InputTextWithHint("##search", "Search...", search_buf, sizeof(search_buf));
    ImGui::SameLine();
    const bool run_now = ImGui::Button("Search") || ImGui::IsKeyPressed(ImGuiKey_Enter);
    ImGui::Spacing();
    ImGui::TextColored(colors.primary_sidebar_text_dim, "FILES TO INCLUDE");
    ImGui::TextUnformatted(g_explorer_root_path.empty() ? "<open a folder from File menu>" : g_explorer_root_path.c_str());
    ImGui::Separator();

    const std::string query = search_buf;

    if (g_explorer_root_path.empty()) {
        ImGui::TextColored(colors.primary_sidebar_text_dim, "Open a folder from title bar File menu to search.");
        return;
    }

    if (query.empty()) {
        ImGui::TextColored(colors.primary_sidebar_text_dim, "Type to search in project files");
        return;
    }

    const double now = ImGui::GetTime();
    if (query != g_search_last_input_snapshot) {
        g_search_last_input_snapshot = query;
        g_search_last_input_change_time = now;
    }

    const bool root_changed = (g_last_search_root != g_explorer_root_path);
    const bool query_changed = (g_last_search_query != query);
    const bool debounced_ready = (now - g_search_last_input_change_time) >= kSearchDebounceSeconds;
    if ((run_now || debounced_ready || root_changed) && (query_changed || root_changed)) {
        g_last_search_query = query;
        g_last_search_root = g_explorer_root_path;
        RunSearch(g_explorer_root_path, query);
    }

    ImGui::Text("%d results (scanned %d files)", static_cast<int>(g_search_hits.size()), g_search_files_scanned);
    if (g_search_hits_truncated) {
        ImGui::TextColored(colors.primary_sidebar_text_dim, "Result limit reached. Refine search to see more.");
    }
    if (g_search_scan_truncated) {
        ImGui::TextColored(colors.primary_sidebar_text_dim, "Scan budget reached. Showing partial results.");
    }
    for (const auto& hit : g_search_hits) {
        std::string label = hit.file + ":" + std::to_string(hit.line);
        if (ImGui::TreeNode(label.c_str())) {
            ImGui::TextWrapped("%s", hit.snippet.c_str());
            ImGui::TreePop();
        }
    }
}

void RenderDebug(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("RUN AND DEBUG", colors.primary_sidebar_text_dim);

    Scenes::ScenePrimarySidebarDebugData debug_data;
    if (active_tab && !active_tab->scene_plugin_id.empty()) {
        Scenes::ScenePluginRegistry::Instance().EnsureLoaded();
        if (const auto* provider = Scenes::ScenePluginRegistry::Instance().GetPrimarySidebarDebugDataProvider(active_tab->scene_plugin_id)) {
            if (*provider) {
                debug_data = (*provider)(active_tab);
            }
        }
    }

    if (ImGui::CollapsingHeader("VARIABLES", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (debug_data.variables.empty()) {
            ImGui::TextColored(colors.primary_sidebar_text_dim, "No variables available");
        } else {
            for (const auto& variable : debug_data.variables) {
                ImGui::BulletText("%s = %s", variable.name.c_str(), variable.value.c_str());
            }
        }
    }

    if (ImGui::CollapsingHeader("WATCH", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (active_tab) {
            auto& raw_watch_list = active_tab->scene_ui_state[kDebugWatchListKey];
            std::vector<std::string> watch_list = ParseWatchList(raw_watch_list);

            static std::unordered_map<uint64_t, std::array<char, 128>> add_watch_inputs;
            auto& add_input = add_watch_inputs[active_tab->id];

            ImGui::PushID(static_cast<int>(active_tab->id));
            ImGui::SetNextItemWidth(-70.0f);
            ImGui::InputTextWithHint("##watch_expr", "Add watch expression...", add_input.data(), add_input.size());
            ImGui::SameLine();
            if (ImGui::Button("Add") && add_input[0] != '\0') {
                watch_list.push_back(add_input.data());
                raw_watch_list = JoinWatchList(watch_list);
                add_input[0] = '\0';
            }
            ImGui::PopID();
            ImGui::Spacing();

            for (size_t i = 0; i < watch_list.size(); ++i) {
                ImGui::PushID(static_cast<int>(i));
                ImGui::TextColored(colors.primary_sidebar_text_dim, "%s", watch_list[i].c_str());
                ImGui::SameLine();
                if (ImGui::SmallButton("x")) {
                    watch_list.erase(watch_list.begin() + i);
                    raw_watch_list = JoinWatchList(watch_list);
                    ImGui::PopID();
                    break;
                }
                ImGui::PopID();
            }

            if (!watch_list.empty()) {
                ImGui::Separator();
            }
        }

        if (debug_data.watches.empty()) {
            ImGui::TextColored(colors.primary_sidebar_text_dim, "No watch expressions");
        } else {
            for (const auto& watch : debug_data.watches) {
                ImGui::BulletText("%s: %s", watch.expression.c_str(), watch.value.c_str());
            }
        }
    }

    if (ImGui::CollapsingHeader("CALL STACK", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (debug_data.callstack.empty()) {
            ImGui::TextColored(colors.primary_sidebar_text_dim, "No call stack");
        } else {
            for (const auto& frame : debug_data.callstack) {
                if (frame.is_active) {
                    ImGui::TextColored(colors.title_bar_text, "> %s", frame.label.c_str());
                } else {
                    ImGui::Text("  %s", frame.label.c_str());
                }
                if (!frame.location.empty()) {
                    ImGui::TextColored(colors.primary_sidebar_text_dim, "    %s", frame.location.c_str());
                }
            }
        }
    }

    // Optional scene contribution to the Debug view (does not replace the default layout).
    if (active_tab && !active_tab->scene_plugin_id.empty()) {
        Scenes::ScenePluginRegistry::Instance().EnsureLoaded();
        if (const auto* renderer = Scenes::ScenePluginRegistry::Instance().GetPrimarySidebarContributionRenderer(active_tab->scene_plugin_id, ActivityBarItem::Debug)) {
            if (*renderer) {
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                (*renderer)(ImVec2(0, 0), ImVec2(0, 0), active_tab);
            }
        }
    }
}

void RenderEditor(ImVec2, ImVec2, EditorTab* active_tab)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("EDITOR", colors.primary_sidebar_text_dim);

    // Optional scene contribution to the Editor view (e.g. inspector, library).
    if (active_tab && !active_tab->scene_plugin_id.empty()) {
        Scenes::ScenePluginRegistry::Instance().EnsureLoaded();
        if (const auto* renderer = Scenes::ScenePluginRegistry::Instance().GetPrimarySidebarContributionRenderer(active_tab->scene_plugin_id, ActivityBarItem::Editor)) {
            if (*renderer) {
                (*renderer)(ImVec2(0, 0), ImVec2(0, 0), active_tab);
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
            }
        }
    }

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

void RefreshTrainingSubdirList(const std::string& project_root)
{
    if (project_root == g_training_cached_root && !g_training_subdirs_sorted.empty()) {
        return;
    }
    g_training_cached_root = project_root;
    g_training_subdirs_sorted.clear();
    g_training_data_page = 0;
    if (project_root.empty()) {
        return;
    }
    std::error_code ec;
    if (!std::filesystem::exists(project_root, ec) || ec) {
        return;
    }
    for (const auto& entry : std::filesystem::directory_iterator(project_root, ec)) {
        if (ec) {
            break;
        }
        if (entry.is_directory()) {
            g_training_subdirs_sorted.push_back(entry.path().string());
        }
    }
    std::sort(g_training_subdirs_sorted.begin(), g_training_subdirs_sorted.end(), [](const std::string& a, const std::string& b) {
        return std::filesystem::path(a).filename().string() < std::filesystem::path(b).filename().string();
    });
}

void RenderTraining(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("TRAINING", colors.primary_sidebar_text_dim);

    const std::string& project_root = GetExplorerRootPath();
    RefreshTrainingSubdirList(project_root);

    if (ImGui::CollapsingHeader("Training data", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (project_root.empty()) {
            ImGui::TextColored(colors.primary_sidebar_text_dim, "Open a project (File -> Open Project) first.");
        } else {
            ImGui::TextColored(colors.primary_sidebar_text_dim, "Project root:");
            ImGui::TextWrapped("%s", project_root.c_str());

            const int n = static_cast<int>(g_training_subdirs_sorted.size());
            const int ipp = kTrainingFoldersPerPage;
            const int total_pages = (n == 0) ? 1 : (n + ipp - 1) / ipp;
            if (g_training_data_page >= total_pages) {
                g_training_data_page = total_pages - 1;
            }
            if (g_training_data_page < 0) {
                g_training_data_page = 0;
            }

            ImGui::Text("Subfolders: %d   Page %d / %d", n, g_training_data_page + 1, total_pages);
            ImGui::BeginDisabled(n == 0);
            if (ImGui::SmallButton("Prev##training_page") && g_training_data_page > 0) {
                --g_training_data_page;
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("Next##training_page") && g_training_data_page < total_pages - 1) {
                ++g_training_data_page;
            }
            ImGui::EndDisabled();

            const float line_h = ImGui::GetTextLineHeightWithSpacing();
            const float list_h = line_h * static_cast<float>(ipp) + ImGui::GetStyle().FramePadding.y * 2.0f;
            ImGui::BeginChild("training_data_folder_list", ImVec2(0.0f, list_h), ImGuiChildFlags_Borders);
            if (n == 0) {
                ImGui::TextColored(colors.primary_sidebar_text_dim, "No subfolders in the project root.");
            } else {
                const int start = g_training_data_page * ipp;
                const int end = std::min(n, start + ipp);
                for (int i = start; i < end; ++i) {
                    const std::string& full = g_training_subdirs_sorted[static_cast<size_t>(i)];
                    const std::string name = std::filesystem::path(full).filename().string();
                    ImGui::PushID(i);
                    const bool sel = (g_training_selected_data_path == full);
                    if (ImGui::Selectable(name.c_str(), sel)) {
                        g_training_selected_data_path = full;
                    }
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("%s", full.c_str());
                    }
                    ImGui::PopID();
                }
            }
            ImGui::EndChild();
        }
    }

    if (ImGui::CollapsingHeader("Training config", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (g_training_project_path_buf[0] == '\0' && !project_root.empty()) {
            std::snprintf(g_training_project_path_buf, sizeof(g_training_project_path_buf), "%s", project_root.c_str());
        }
        ImGui::TextUnformatted("Project");
        ImGui::SetNextItemWidth(-100.0f);
        ImGui::InputText("##training_project_path", g_training_project_path_buf, sizeof(g_training_project_path_buf));
        ImGui::SameLine();
        if (ImGui::Button("Browse##training_proj")) {
            const std::string picked = UI::PickFolderPathFromDialog();
            if (!picked.empty()) {
                std::snprintf(g_training_project_path_buf, sizeof(g_training_project_path_buf), "%s", picked.c_str());
            }
        }
        ImGui::TextColored(colors.primary_sidebar_text_dim, "Browse opens a folder dialog; you can also edit the path directly.");
    }
}

void RenderExtensions(ImVec2, ImVec2, EditorTab*)
{
    const auto& colors = GetWorkbenchTheme().colors;
    DrawPlaceholderHeader("EXTENSIONS", colors.primary_sidebar_text_dim);
    DrawEmptyState("Extension marketplace");
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
    global_primary_plugin.primary_views.push_back({ "training", "Training", RenderTraining });
    global_primary_plugin.primary_bindings = {
        { ActivityBarItem::Explorer, "explorer" },
        { ActivityBarItem::Search, "search" },
        { ActivityBarItem::Debug, "debug" },
        { ActivityBarItem::Editor, "editor" },
        { ActivityBarItem::Extensions, "extensions" },
        { ActivityBarItem::Training, "training" }
    };
    plugins.push_back(std::move(global_primary_plugin));

    Scenes::ScenePluginRegistry::Instance().EnsureLoaded();
    const auto& scene_plugins = Scenes::ScenePluginRegistry::Instance().GetPlugins();

    for (const auto& scene_plugin : scene_plugins) {
        EditorViewPlugin plugin{};
        plugin.plugin_id = scene_plugin.id;

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
            configs.push_back({ plugin.plugin_id, view.id, view.title, view.renderer });
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

std::string GetTrainingSelectedDataFolderPath()
{
    return g_training_selected_data_path;
}

std::string GetTrainingConfigProjectPath()
{
    return std::string(g_training_project_path_buf);
}

} // namespace UI
