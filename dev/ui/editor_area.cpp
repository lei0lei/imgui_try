#include "editor_area.h"
#include "imgui.h"
#include <cmath>

// 绘制空白欢迎界面
static void DrawWelcomeScreen(ImVec2 area_min, ImVec2 area_max)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 center = ImVec2((area_min.x + area_max.x) * 0.5f, (area_min.y + area_max.y) * 0.5f);
    
    // VS Code 风格的背景色
    ImU32 bg_color = IM_COL32(30, 30, 30, 255);  // #1E1E1E
    draw_list->AddRectFilled(area_min, area_max, bg_color);
    
    // 显示欢迎文本
    ImGui::PushFont(ImGui::GetFont());
    const char* welcome_text = "No editor open";
    const char* hint_text = "Open a file to start editing";
    
    ImVec2 welcome_size = ImGui::CalcTextSize(welcome_text);
    ImVec2 hint_size = ImGui::CalcTextSize(hint_text);
    
    // 绘制欢迎文本（居中）
    ImVec2 welcome_pos = ImVec2(center.x - welcome_size.x * 0.5f, center.y - 30);
    ImVec2 hint_pos = ImVec2(center.x - hint_size.x * 0.5f, center.y + 10);
    
    draw_list->AddText(welcome_pos, IM_COL32(150, 150, 150, 255), welcome_text);
    draw_list->AddText(hint_pos, IM_COL32(100, 100, 100, 255), hint_text);
    
    ImGui::PopFont();
}

// 绘制标签栏
static void DrawTabBar(ImVec2 tab_bar_min, ImVec2 tab_bar_max, std::vector<EditorTab>& tabs, EditorAreaResult& result)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    // 标签栏背景色
    ImU32 tab_bar_bg = IM_COL32(37, 37, 38, 255);  // #252526
    draw_list->AddRectFilled(tab_bar_min, tab_bar_max, tab_bar_bg);
    
    float tab_height = tab_bar_max.y - tab_bar_min.y;
    float tab_width = 120.0f;  // 固定标签宽度
    float close_btn_size = 16.0f;
    
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
            tab_bg = IM_COL32(30, 30, 30, 255);  // 激活标签 - 编辑器背景色
        } else if (is_hovered) {
            tab_bg = IM_COL32(45, 45, 45, 255);  // 悬停
        } else {
            tab_bg = IM_COL32(37, 37, 38, 255);  // 非激活
        }
        
        draw_list->AddRectFilled(tab_min, tab_max, tab_bg);
        
        // 激活标签的顶部高亮线
        if (tab.active) {
            draw_list->AddLine(tab_min, ImVec2(tab_max.x, tab_min.y), 
                             IM_COL32(0, 122, 204, 255), 2.0f);  // VS Code 蓝色
        }
        
        // 标签文本
        float text_x = tab_min.x + 10;
        float text_y = tab_min.y + (tab_height - ImGui::GetTextLineHeight()) * 0.5f;
        
        // 修改标记（圆点）
        if (tab.modified) {
            draw_list->AddCircleFilled(ImVec2(text_x + 4, text_y + ImGui::GetTextLineHeight() * 0.5f), 
                                      3.0f, IM_COL32(200, 200, 200, 255));
            text_x += 12;
        }
        
        // 文件名
        ImU32 text_color = tab.active ? IM_COL32(255, 255, 255, 255) : IM_COL32(170, 170, 170, 255);
        
        // 截断过长的文件名
        std::string display_name = tab.name;
        ImVec2 text_size = ImGui::CalcTextSize(display_name.c_str());
        float max_text_width = tab_width - (tab.modified ? 30 : 20) - close_btn_size - 10;
        
        if (text_size.x > max_text_width) {
            while (text_size.x > max_text_width && display_name.length() > 3) {
                display_name = display_name.substr(0, display_name.length() - 1);
                text_size = ImGui::CalcTextSize((display_name + "...").c_str());
            }
            display_name += "...";
        }
        
        draw_list->AddText(ImVec2(text_x, text_y), text_color, display_name.c_str());
        
        // 关闭按钮
        ImVec2 close_center = ImVec2(tab_max.x - close_btn_size - 8, tab_min.y + tab_height * 0.5f);
        ImVec2 close_min = ImVec2(close_center.x - close_btn_size * 0.5f, close_center.y - close_btn_size * 0.5f);
        ImVec2 close_max = ImVec2(close_center.x + close_btn_size * 0.5f, close_center.y + close_btn_size * 0.5f);
        
        bool close_hovered = (mouse_pos.x >= close_min.x && mouse_pos.x <= close_max.x &&
                             mouse_pos.y >= close_min.y && mouse_pos.y <= close_max.y);
        
        if (close_hovered) {
            draw_list->AddRectFilled(close_min, close_max, IM_COL32(80, 80, 80, 255), 2.0f);
        }
        
        // X 图标
        float x_size = 5.0f;
        ImU32 x_color = close_hovered ? IM_COL32(230, 230, 230, 255) : IM_COL32(150, 150, 150, 255);
        draw_list->AddLine(ImVec2(close_center.x - x_size, close_center.y - x_size),
                          ImVec2(close_center.x + x_size, close_center.y + x_size), x_color, 1.5f);
        draw_list->AddLine(ImVec2(close_center.x - x_size, close_center.y + x_size),
                          ImVec2(close_center.x + x_size, close_center.y - x_size), x_color, 1.5f);
        
        // 处理点击事件
        if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            if (close_hovered) {
                // 点击关闭按钮
                result.closed_tab = (int)i;
            } else {
                // 点击标签切换激活
                result.active_tab = (int)i;
            }
        }
        
        // 标签右边框
        draw_list->AddLine(ImVec2(tab_max.x, tab_min.y), tab_max, 
                          IM_COL32(50, 50, 50, 255), 1.0f);
    }
    
    // 标签栏底部边框
    draw_list->AddLine(ImVec2(tab_bar_min.x, tab_bar_max.y - 1), 
                      ImVec2(tab_bar_max.x, tab_bar_max.y - 1),
                      IM_COL32(50, 50, 50, 255), 1.0f);
}

// 绘制编辑器内容区域
static void DrawEditorContent(ImVec2 content_min, ImVec2 content_max, const EditorTab* active_tab)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    if (!active_tab) return;
    
    // 根据场景类型绘制不同内容
    if (active_tab->scene_type == SceneType::Scene2D) {
        // 2D 场景 - 简单的图形设计界面
        ImU32 bg_color = IM_COL32(40, 40, 45, 255);
        draw_list->AddRectFilled(content_min, content_max, bg_color);
        
        // 绘制网格背景
        float grid_size = 30.0f;
        ImU32 grid_color = IM_COL32(60, 60, 65, 255);
        
        for (float x = content_min.x; x < content_max.x; x += grid_size) {
            draw_list->AddLine(ImVec2(x, content_min.y), ImVec2(x, content_max.y), grid_color, 1.0f);
        }
        for (float y = content_min.y; y < content_max.y; y += grid_size) {
            draw_list->AddLine(ImVec2(content_min.x, y), ImVec2(content_max.x, y), grid_color, 1.0f);
        }
        
        // 绘制一些 2D 图形示例
        ImVec2 center = ImVec2((content_min.x + content_max.x) * 0.5f, (content_min.y + content_max.y) * 0.5f);
        float time = (float)ImGui::GetTime();
        
        // 旋转的矩形
        float angle = time * 0.5f;
        float rect_size = 80.0f;
        ImVec2 rect_points[4];
        for (int i = 0; i < 4; i++) {
            float a = angle + i * 3.14159f * 0.5f;
            rect_points[i] = ImVec2(
                center.x + cosf(a) * rect_size - 100,
                center.y + sinf(a) * rect_size
            );
        }
        draw_list->AddQuadFilled(rect_points[0], rect_points[1], rect_points[2], rect_points[3], 
                                IM_COL32(100, 150, 255, 180));
        draw_list->AddQuad(rect_points[0], rect_points[1], rect_points[2], rect_points[3], 
                          IM_COL32(150, 200, 255, 255), 2.5f);
        
        // 跳动的圆形
        float circle_y = center.y + sinf(time * 2.0f) * 30.0f;
        draw_list->AddCircleFilled(ImVec2(center.x + 100, circle_y), 50.0f, 
                                  IM_COL32(255, 100, 150, 180), 32);
        draw_list->AddCircle(ImVec2(center.x + 100, circle_y), 50.0f, 
                            IM_COL32(255, 150, 200, 255), 32, 2.5f);
        
        // 波浪线
        int wave_points = 100;
        for (int i = 0; i < wave_points - 1; i++) {
            float t1 = (float)i / wave_points;
            float t2 = (float)(i + 1) / wave_points;
            
            float x1 = content_min.x + (content_max.x - content_min.x) * t1;
            float x2 = content_min.x + (content_max.x - content_min.x) * t2;
            
            float y1 = center.y + 150 + sinf(time * 2.0f + t1 * 10.0f) * 30.0f;
            float y2 = center.y + 150 + sinf(time * 2.0f + t2 * 10.0f) * 30.0f;
            
            draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), 
                             IM_COL32(100, 255, 150, 255), 3.0f);
        }
        
        // 标题信息
        float padding = 20.0f;
        ImVec2 title_pos = ImVec2(content_min.x + padding, content_min.y + padding);
        std::string title_text = "2D Canvas: " + active_tab->name;
        draw_list->AddText(title_pos, IM_COL32(220, 220, 230, 255), title_text.c_str());
        
        ImVec2 hint_pos = ImVec2(content_min.x + padding, content_min.y + padding + 25);
        draw_list->AddText(hint_pos, IM_COL32(140, 140, 150, 200), "2D graphics scene with animations");
        
    } else {
        // 3D 场景 - 原有的 3D 网格场景
        ImU32 bg_top = IM_COL32(25, 25, 30, 255);
        ImU32 bg_bottom = IM_COL32(15, 15, 20, 255);
        draw_list->AddRectFilledMultiColor(content_min, content_max, bg_top, bg_top, bg_bottom, bg_bottom);
        
        // 绘制3D透视网格场景
        ImVec2 center = ImVec2((content_min.x + content_max.x) * 0.5f, (content_min.y + content_max.y) * 0.5f);
        float width = content_max.x - content_min.x;
        float height = content_max.y - content_min.y;
        
        // 动画时间
        float time = (float)ImGui::GetTime();
        
        // 地平线位置
        float horizon_y = center.y + height * 0.1f;
        
        // 绘制透视网格地面
        int grid_lines = 20;
        float grid_spacing = 40.0f;
        
        // 消失点（透视中心）
        ImVec2 vanishing_point = ImVec2(center.x, horizon_y);
        
        // 绘制纵向网格线（向远处延伸）
        for (int i = -grid_lines / 2; i <= grid_lines / 2; i++) {
            float x_offset = i * grid_spacing;
            
            // 近端点
            ImVec2 near_point = ImVec2(center.x + x_offset, content_max.y);
            
            // 远端点（向消失点收缩）
            float far_x = center.x + x_offset * 0.2f;
            ImVec2 far_point = ImVec2(far_x, horizon_y);
            
            // 根据距离中心的位置调整透明度
            float alpha = 1.0f - (fabsf((float)i) / (grid_lines / 2.0f)) * 0.5f;
            ImU32 line_color = IM_COL32(60, 120, 180, (int)(alpha * 120));
            
            draw_list->AddLine(near_point, far_point, line_color, 1.5f);
        }
        
        // 绘制横向网格线（由近到远）
        int horizontal_lines = 15;
        for (int i = 0; i < horizontal_lines; i++) {
            float t = (float)i / (horizontal_lines - 1);
            
            // 指数插值实现透视效果
            float ease_t = t * t;
            float y = content_max.y + (horizon_y - content_max.y) * ease_t;
            
            // 根据深度计算宽度（近宽远窄）
            float scale = 1.0f - ease_t * 0.8f;
            float half_width = width * 0.5f * scale;
            
            ImVec2 left = ImVec2(center.x - half_width, y);
            ImVec2 right = ImVec2(center.x + half_width, y);
            
            // 距离越远，透明度越低
            float alpha = 1.0f - ease_t * 0.7f;
            ImU32 line_color = IM_COL32(60, 120, 180, (int)(alpha * 120));
            
            draw_list->AddLine(left, right, line_color, 1.5f);
        }
        
        // 绘制一些漂浮的几何体装饰
        float orbit_radius = 80.0f;
        
        // 立方体轮廓
        for (int cube_idx = 0; cube_idx < 3; cube_idx++) {
            float angle = time * 0.3f + cube_idx * 2.0f;
            float cube_x = center.x + cosf(angle) * orbit_radius * (1.0f + cube_idx * 0.5f);
            float cube_y = horizon_y - 100.0f - cube_idx * 40.0f + sinf(time * 0.5f + cube_idx) * 20.0f;
            float size = 25.0f - cube_idx * 5.0f;
            
            ImVec2 cube_center = ImVec2(cube_x, cube_y);
            
            // 旋转角度
            float rot = time * 0.5f + cube_idx * 1.0f;
            
            // 绘制简化的3D立方体（等距投影）
            float cos_r = cosf(rot);
            float sin_r = sinf(rot);
            
            // 8个顶点的2D投影
            ImVec2 vertices[8];
            float offsets[8][3] = {
                {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
                {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}
            };
            
            for (int v = 0; v < 8; v++) {
                float x = offsets[v][0] * size;
                float y = offsets[v][1] * size;
                float z = offsets[v][2] * size;
                
                // Y轴旋转
                float rx = x * cos_r - z * sin_r;
                float rz = x * sin_r + z * cos_r;
                
                // 等距投影
                vertices[v].x = cube_center.x + rx - rz * 0.5f;
                vertices[v].y = cube_center.y + y + rz * 0.3f;
            }
            
            // 绘制立方体的边
            int edges[12][2] = {
                {0,1}, {1,2}, {2,3}, {3,0},  // 后面
                {4,5}, {5,6}, {6,7}, {7,4},  // 前面
                {0,4}, {1,5}, {2,6}, {3,7}   // 连接边
            };
            
            ImU32 cube_color = IM_COL32(80, 160, 220, 200);
            for (int e = 0; e < 12; e++) {
                draw_list->AddLine(vertices[edges[e][0]], vertices[edges[e][1]], cube_color, 2.0f);
            }
        }
        
        // 标题信息
        float padding = 20.0f;
        ImVec2 title_pos = ImVec2(content_min.x + padding, content_min.y + padding);
        
        std::string title_text = "3D View: " + active_tab->name;
        draw_list->AddText(title_pos, IM_COL32(200, 220, 240, 255), title_text.c_str());
        
        // 提示信息
        ImVec2 hint_pos = ImVec2(content_min.x + padding, content_min.y + padding + 25);
        draw_list->AddText(hint_pos, IM_COL32(120, 140, 160, 200), "Interactive 3D scene with animated grid");
    }
}

EditorAreaResult DrawEditorArea(
    float left_offset,
    float right_offset,
    float title_h,
    float status_bar_h,
    float panel_h,
    bool panel_visible,
    std::vector<EditorTab>& tabs)
{
    EditorAreaResult result{};
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
    
    if (tabs.empty()) {
        // 没有打开的文件 - 显示欢迎界面
        DrawWelcomeScreen(area_min, area_max);
    } else {
        // 有文件打开 - 显示标签栏和编辑器内容
        float tab_bar_height = 35.0f;
        
        ImVec2 tab_bar_min = area_min;
        ImVec2 tab_bar_max = ImVec2(area_max.x, area_min.y + tab_bar_height);
        
        DrawTabBar(tab_bar_min, tab_bar_max, tabs, result);
        
        // 查找激活的标签
        const EditorTab* active_tab = nullptr;
        for (const auto& tab : tabs) {
            if (tab.active) {
                active_tab = &tab;
                break;
            }
        }
        
        // 绘制编辑器内容
        ImVec2 content_min = ImVec2(area_min.x, tab_bar_max.y);
        ImVec2 content_max = area_max;
        DrawEditorContent(content_min, content_max, active_tab);
    }
    
    ImGui::End();
    ImGui::PopStyleVar(3);
    
    return result;
}
