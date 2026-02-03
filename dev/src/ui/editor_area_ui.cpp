#include "editor_area_ui.h"
#include "editor_scene_renderers.h"
#include "imgui.h"
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
                       std::vector<EditorTab>& tabs, 
                       int& closed_tab, int& active_tab)
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
    
    for (size_t i = 0; i < tabs.size(); i++)
    {
        EditorTab& tab = tabs[i];
        
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
        if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            if (close_hovered) {
                // 点击关闭按钮
                closed_tab = (int)i;
            } else {
                // 点击标签切换激活
                active_tab = (int)i;
            }
        }
        
        // 标签右边框
        draw_list->AddLine(ImVec2(tab_max.x, tab_min.y), tab_max, 
                  ImGui::GetColorU32(colors.editor_tab_border), sizes.editor_tab_border_thickness);
    }
    
    // 标签栏底部边框
    draw_list->AddLine(ImVec2(tab_bar_min.x, tab_bar_max.y - sizes.editor_tab_border_thickness), 
                      ImVec2(tab_bar_max.x, tab_bar_max.y - sizes.editor_tab_border_thickness),
                      ImGui::GetColorU32(colors.editor_tab_border), sizes.editor_tab_border_thickness);
}

// 绘制编辑器内容区域
static void DrawEditorContent(ImVec2 content_min, ImVec2 content_max, EditorTab* active_tab, EditorSceneRegistry& registry)
{
    if (!active_tab) return;
    
    if (auto renderer = registry.GetRenderer(active_tab->scene_type)) {
        renderer(content_min, content_max, *active_tab);
    } else {
        DrawScene3D(content_min, content_max, *active_tab);
    }
}

EditorArea::EditorArea(EditorAreaService& service) 
    : service_(service) 
{
    RegisterDefaultEditorSceneRenderers(scene_registry_);
}

void EditorArea::Draw(
    float left_offset,
    float right_offset,
    float title_h,
    float status_bar_h,
    float panel_h,
    bool panel_visible)
{
    ImGuiIO& io = ImGui::GetIO();
    
    // 计算编辑器区域
    float bottom_offset = status_bar_h + (panel_visible ? panel_h : 0.0f);
    
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
    
    auto& tabs = service_.GetTabs();
    
    if (tabs.empty()) {
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
        DrawTabBar(tab_bar_min, tab_bar_max, tabs, closed_tab, active_tab);
        
        // 处理用户操作（通过 service）
        if (closed_tab >= 0) {
            service_.CloseTab(closed_tab);
            // 重新获取tabs引用，因为CloseTab可能修改了数组
            tabs = service_.GetTabs();
        }
        if (active_tab >= 0) {
            service_.ActivateTab(active_tab);
        }
        
        // 只有在还有tabs的情况下才绘制编辑器内容
        if (!tabs.empty()) {
            // 查找激活的标签
            EditorTab* active_tab_ptr = nullptr;
            for (auto& tab : tabs) {
                if (tab.active) {
                    active_tab_ptr = &tab;
                    break;
                }
            }
            
            // 绘制编辑器内容
            ImVec2 content_min = ImVec2(area_min.x, tab_bar_max.y);
            ImVec2 content_max = area_max;
            DrawEditorContent(content_min, content_max, active_tab_ptr, scene_registry_);
        } else {
            // 关闭最后一个tab后，绘制欢迎界面
            DrawWelcomeScreen(area_min, area_max);
        }
    }
    
    ImGui::End();
    ImGui::PopStyleVar(3);
}

} // namespace UI
