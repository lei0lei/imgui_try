/**
 * @file editor_area_ui.cpp
 * @brief 主编辑器区域的UI渲染，包含标签和内容
 * @author Your Name
 * @date 2026-02-05
 */

#include "editor_area_ui.h"
#include "imgui.h"
#include "../scenes/scene_plugin_registry.h"
#include "../workbench/workbench_config.h"
#include <algorithm>

namespace UI {

// 绘制空白欢迎界面
static void DrawWelcomeScreen(ImVec2 area_min, ImVec2 area_max)
{
    const WorkbenchTheme& theme = GetWorkbenchTheme();
    const WorkbenchThemeColors& colors = theme.colors;
    const WorkbenchThemeSizes& sizes = theme.sizes;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 center = ImVec2((area_min.x + area_max.x) * 0.5f, (area_min.y + area_max.y) * 0.5f);
    
    // 背景色
    draw_list->AddRectFilled(area_min, area_max, ImGui::GetColorU32(colors.editor_welcome_bg));
    
    // 显示欢迎文本
    ImGui::PushFont(ImGui::GetFont());
    const char* welcome_text = "No editor open";
    const char* hint_text = "Open a file to start editing";
    
    ImVec2 welcome_size = ImGui::CalcTextSize(welcome_text);
    ImVec2 hint_size = ImGui::CalcTextSize(hint_text);
    
    // 绘制欢迎文本（居中）
    ImVec2 welcome_pos = ImVec2(center.x - welcome_size.x * 0.5f, center.y + sizes.editor_welcome_title_offset_y);
    ImVec2 hint_pos = ImVec2(center.x - hint_size.x * 0.5f, center.y + sizes.editor_welcome_hint_offset_y);
    
    draw_list->AddText(welcome_pos, ImGui::GetColorU32(colors.editor_welcome_text), welcome_text);
    draw_list->AddText(hint_pos, ImGui::GetColorU32(colors.editor_welcome_hint), hint_text);
    
    ImGui::PopFont();
}

// 绘制标签栏
static void DrawTabBar(ImVec2 tab_bar_min, ImVec2 tab_bar_max, 
                       const std::vector<EditorTab>& tabs, 
                       int& closed_tab, int& active_tab,
                       int& move_from, int& move_to,
                       bool block_tab_clicks)
{
    const WorkbenchTheme& theme = GetWorkbenchTheme();
    const WorkbenchThemeColors& colors = theme.colors;
    const WorkbenchThemeSizes& sizes = theme.sizes;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    // 标签栏背景色
    draw_list->AddRectFilled(tab_bar_min, tab_bar_max, ImGui::GetColorU32(colors.editor_tab_bar_bg));
    
    float tab_height = tab_bar_max.y - tab_bar_min.y;
    float tab_width = sizes.editor_tab_width;
    float close_btn_size = sizes.editor_tab_close_size;
    
    ImVec2 mouse_pos = ImGui::GetMousePos();
    
    static int drag_index = -1;
    static int pending_click = -1;
    static bool dragging = false;
    static int drag_target = -1;

    int visible_count = static_cast<int>(tabs.size());
    float max_visible = (tab_width > 0.0f) ? ((tab_bar_max.x - tab_bar_min.x) / tab_width) : 0.0f;
    if (max_visible > 0.0f) {
        int max_vis = static_cast<int>(max_visible);
        if (max_vis < visible_count) visible_count = max_vis;
    }

    for (int i = 0; i < static_cast<int>(tabs.size()); i++)
    {
        const EditorTab& tab = tabs[i];
        
        // 计算标签位置
        ImVec2 tab_min = ImVec2(tab_bar_min.x + i * tab_width, tab_bar_min.y);
        ImVec2 tab_max = ImVec2(tab_min.x + tab_width, tab_bar_max.y);
        
        bool is_hovered = (mouse_pos.x >= tab_min.x && mouse_pos.x <= tab_max.x &&
                          mouse_pos.y >= tab_min.y && mouse_pos.y <= tab_max.y);
        
        // 标签背景色
        ImU32 tab_bg;
        if (tab.active) {
            tab_bg = ImGui::GetColorU32(colors.editor_tab_active_bg);
        } else if (is_hovered) {
            tab_bg = ImGui::GetColorU32(colors.editor_tab_hover_bg);
        } else {
            tab_bg = ImGui::GetColorU32(colors.editor_tab_inactive_bg);
        }
        
        draw_list->AddRectFilled(tab_min, tab_max, tab_bg);

        // Drag placeholder highlight
        if (dragging && i == drag_target) {
            draw_list->AddRectFilled(tab_min, tab_max, ImGui::GetColorU32(colors.editor_tab_hover_bg), 0.0f);
            draw_list->AddRect(tab_min, tab_max, ImGui::GetColorU32(colors.editor_tab_active_indicator), 0.0f, 0, 2.0f);
        }
        
        // 激活标签的顶部高亮线
        if (tab.active) {
            draw_list->AddLine(tab_min, ImVec2(tab_max.x, tab_min.y), 
                             ImGui::GetColorU32(colors.editor_tab_active_indicator), sizes.editor_tab_active_indicator_h);
        }
        
        // 标签文本
        float text_x = tab_min.x + sizes.editor_tab_text_padding_x;
        float text_y = tab_min.y + (tab_height - ImGui::GetTextLineHeight()) * 0.5f;
        
        // 修改标记（圆点）
        if (tab.modified) {
            draw_list->AddCircleFilled(ImVec2(text_x + sizes.editor_tab_modified_dot_radius + 1.0f, text_y + ImGui::GetTextLineHeight() * 0.5f), 
                                      sizes.editor_tab_modified_dot_radius, ImGui::GetColorU32(colors.editor_tab_modified_dot));
            text_x += sizes.editor_tab_modified_dot_radius * 4.0f;
        }
        
        // 文件名
        ImU32 text_color = tab.active ? ImGui::GetColorU32(colors.editor_tab_text_active) : ImGui::GetColorU32(colors.editor_tab_text_inactive);
        
        // 截断过长的文件名
        std::string display_name = tab.name;
        ImVec2 text_size = ImGui::CalcTextSize(display_name.c_str());
        float max_text_width = tab_width - (tab.modified ? 30.0f : 20.0f) - close_btn_size - sizes.editor_tab_text_padding_x;
        
        if (text_size.x > max_text_width) {
            while (text_size.x > max_text_width && display_name.length() > 3) {
                display_name = display_name.substr(0, display_name.length() - 1);
                text_size = ImGui::CalcTextSize((display_name + "...").c_str());
            }
            display_name += "...";
        }
        
        draw_list->AddText(ImVec2(text_x, text_y), text_color, display_name.c_str());
        
        // 关闭按钮
        ImVec2 close_center = ImVec2(tab_max.x - close_btn_size - sizes.editor_tab_close_padding_x, tab_min.y + tab_height * 0.5f);
        ImVec2 close_min = ImVec2(close_center.x - close_btn_size * 0.5f, close_center.y - close_btn_size * 0.5f);
        ImVec2 close_max = ImVec2(close_center.x + close_btn_size * 0.5f, close_center.y + close_btn_size * 0.5f);
        
        bool close_hovered = (mouse_pos.x >= close_min.x && mouse_pos.x <= close_max.x &&
                             mouse_pos.y >= close_min.y && mouse_pos.y <= close_max.y);
        
        if (close_hovered) {
            draw_list->AddRectFilled(close_min, close_max, ImGui::GetColorU32(colors.editor_tab_close_hover_bg), sizes.editor_tab_close_rounding);
        }
        
        // X 图标
        float x_size = sizes.title_close_icon_size;
        ImU32 x_color = close_hovered ? ImGui::GetColorU32(colors.editor_tab_close_icon) : ImGui::GetColorU32(colors.editor_tab_text_inactive);
        draw_list->AddLine(ImVec2(close_center.x - x_size, close_center.y - x_size),
                  ImVec2(close_center.x + x_size, close_center.y + x_size), x_color, sizes.title_button_icon_stroke);
        draw_list->AddLine(ImVec2(close_center.x - x_size, close_center.y + x_size),
                  ImVec2(close_center.x + x_size, close_center.y - x_size), x_color, sizes.title_button_icon_stroke);
        
        // 处理点击事件
        if (!block_tab_clicks && is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            if (close_hovered) {
                closed_tab = (int)i;
            } else {
                pending_click = (int)i;
                drag_index = (int)i;
                dragging = false;
                drag_target = (int)i;
            }
        }
        
        // 标签右边框
        draw_list->AddLine(ImVec2(tab_max.x, tab_min.y), tab_max, 
                  ImGui::GetColorU32(colors.editor_tab_border), sizes.editor_tab_border_thickness);
    }

    if (!block_tab_clicks && drag_index >= 0 && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        dragging = true;
        float x = mouse_pos.x - tab_bar_min.x;
        int target = static_cast<int>(x / tab_width);
        if (target < 0) target = 0;
        if (target >= visible_count) target = visible_count - 1;
        drag_target = target;
        move_from = drag_index;
        move_to = target;
    }

    if (!block_tab_clicks && drag_index >= 0 && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        if (dragging) {
            if (move_from >= 0 && move_to >= 0 && move_from != move_to) {
                // move request already set
            }
        } else if (pending_click >= 0) {
            active_tab = pending_click;
        }
        drag_index = -1;
        pending_click = -1;
        dragging = false;
        drag_target = -1;
    }

    // Draw drag ghost with easing
    static float ghost_x_smooth = 0.0f;
    if (dragging && drag_index >= 0 && drag_index < static_cast<int>(tabs.size())) {
        const EditorTab& tab = tabs[drag_index];
        float target_x = mouse_pos.x - tab_width * 0.5f;
        float min_x = tab_bar_min.x;
        float max_x = tab_bar_max.x - tab_width;
        if (target_x < min_x) target_x = min_x;
        if (target_x > max_x) target_x = max_x;

        float dt = ImGui::GetIO().DeltaTime;
        float smooth = 1.0f - std::exp(-12.0f * dt);
        if (ghost_x_smooth == 0.0f) {
            ghost_x_smooth = target_x;
        }
        ghost_x_smooth = ghost_x_smooth + (target_x - ghost_x_smooth) * smooth;

        ImVec2 ghost_min = ImVec2(ghost_x_smooth, tab_bar_min.y);
        ImVec2 ghost_max = ImVec2(ghost_x_smooth + tab_width, tab_bar_max.y);
        ImU32 ghost_bg = ImGui::GetColorU32(ImVec4(colors.editor_tab_active_bg.x, colors.editor_tab_active_bg.y, colors.editor_tab_active_bg.z, 0.45f));
        draw_list->AddRectFilled(ghost_min, ghost_max, ghost_bg);
        draw_list->AddRect(ghost_min, ghost_max, ImGui::GetColorU32(colors.editor_tab_active_indicator), 0.0f, 0, 2.0f);
        float text_x = ghost_min.x + sizes.editor_tab_text_padding_x;
        float text_y = ghost_min.y + (tab_height - ImGui::GetTextLineHeight()) * 0.5f;
        draw_list->AddText(ImVec2(text_x, text_y), ImGui::GetColorU32(colors.editor_tab_text_active), tab.name.c_str());
    } else {
        ghost_x_smooth = 0.0f;
    }
    
    // 标签栏底部边框
    draw_list->AddLine(ImVec2(tab_bar_min.x, tab_bar_max.y - sizes.editor_tab_border_thickness), 
                      ImVec2(tab_bar_max.x, tab_bar_max.y - sizes.editor_tab_border_thickness),
                      ImGui::GetColorU32(colors.editor_tab_border), sizes.editor_tab_border_thickness);
}

// 绘制编辑器内容区域
static void DrawEditorContent(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab)
{
    if (!active_tab) return;

    Scenes::ScenePluginRegistry::Instance().EnsureLoaded();
    if (auto renderer = Scenes::ScenePluginRegistry::Instance().GetRenderer(active_tab->scene_plugin_id)) {
        const Scenes::SceneContext ctx{ content_min, content_max, GetWorkbenchTheme(), GetWorkbenchMetrics() };
        renderer(ctx, *active_tab);
    } else {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRectFilled(content_min, content_max, IM_COL32(30, 30, 32, 255));
        draw_list->AddText(ImVec2(content_min.x + 16.0f, content_min.y + 16.0f), IM_COL32(220, 220, 220, 255), "No renderer registered for this scene.");
    }
}

EditorArea::EditorArea() = default;

void EditorArea::Draw(
    float left_offset,
    float right_offset,
    float title_h,
    float status_bar_h,
    bool block_tab_clicks,
    const ViewModel& view_model)
{
    ImGuiIO& io = ImGui::GetIO();
    
    // 计算编辑器区域
    float bottom_offset = status_bar_h;
    
    ImVec2 area_min = ImVec2(left_offset, title_h);
    ImVec2 area_max = ImVec2(io.DisplaySize.x - right_offset, io.DisplaySize.y - bottom_offset);
    
    // 创建编辑器窗口
    ImGui::SetNextWindowPos(area_min);
    ImGui::SetNextWindowSize(ImVec2(area_max.x - area_min.x, area_max.y - area_min.y));
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
                             ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav;
    
    ImGui::Begin("EditorArea", nullptr, flags);
    
    const std::vector<EditorTab>* tabs_ptr = view_model.get_tabs ? &view_model.get_tabs() : nullptr;
    
    if (!tabs_ptr || tabs_ptr->empty()) {
        const WorkbenchThemeColors& colors = GetWorkbenchTheme().colors;
        ImDrawList* bg = ImGui::GetBackgroundDrawList();
        ImVec2 full_max = ImVec2(io.DisplaySize.x, area_max.y);
        bg->AddRectFilled(area_min, full_max, ImGui::GetColorU32(colors.editor_welcome_bg));
        area_max.x = io.DisplaySize.x;
        // 没有打开的文件 - 显示欢迎界面
        DrawWelcomeScreen(area_min, area_max);
    } else {
        // 有文件打开 - 显示标签栏和编辑器内容
        const WorkbenchThemeSizes& sizes = GetWorkbenchTheme().sizes;
        float tab_bar_height = sizes.editor_tab_bar_height;
        
        ImVec2 tab_bar_min = area_min;
        ImVec2 tab_bar_max = ImVec2(area_max.x, area_min.y + tab_bar_height);
        
        // 处理标签栏交互
        int closed_tab = -1;
        int active_tab = -1;
        int move_from = -1;
        int move_to = -1;
        DrawTabBar(tab_bar_min, tab_bar_max, *tabs_ptr, closed_tab, active_tab, move_from, move_to, block_tab_clicks);
        
        // 处理用户操作（通过 service）
        if (closed_tab >= 0 && view_model.close_tab) {
            view_model.close_tab(closed_tab);
        }
        if (active_tab >= 0 && view_model.activate_tab) {
            view_model.activate_tab(active_tab);
        }
        if (move_from >= 0 && move_to >= 0 && move_from != move_to && view_model.move_tab) {
            view_model.move_tab(move_from, move_to);
        }

        // 重新获取 tabs 指针（可能已更新）
        tabs_ptr = view_model.get_tabs ? &view_model.get_tabs() : nullptr;
        
        // 只有在还有tabs的情况下才绘制编辑器内容
        if (tabs_ptr && !tabs_ptr->empty()) {
            // 查找激活的标签
            EditorTab* active_tab_ptr = view_model.get_active_tab ? view_model.get_active_tab() : nullptr;
            
            // 绘制编辑器内容
            ImVec2 content_min = ImVec2(area_min.x, tab_bar_max.y);
            ImVec2 content_max = area_max;
            DrawEditorContent(content_min, content_max, active_tab_ptr);
        } else {
            // 关闭最后一个tab后，绘制欢迎界面
            const WorkbenchThemeColors& colors = GetWorkbenchTheme().colors;
            ImDrawList* bg = ImGui::GetBackgroundDrawList();
            ImVec2 full_max = ImVec2(io.DisplaySize.x, area_max.y);
            bg->AddRectFilled(area_min, full_max, ImGui::GetColorU32(colors.editor_welcome_bg));
            area_max.x = io.DisplaySize.x;
            DrawWelcomeScreen(area_min, area_max);
        }
    }
    
    ImGui::End();
    ImGui::PopStyleVar(3);
}

} // namespace UI
