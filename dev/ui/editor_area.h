#pragma once

#include <vector>
#include <string>

enum class SceneType {
    Scene2D,    // 2D 场景
    Scene3D     // 3D 场景
};

// 编辑器标签页
struct EditorTab {
    std::string name;           // 文件名或标题
    std::string path;           // 文件完整路径
    bool modified = false;      // 是否已修改
    bool active = false;        // 是否为当前激活标签
    SceneType scene_type = SceneType::Scene3D;  // 场景类型
};

struct EditorAreaResult {
    int closed_tab = -1;        // 关闭的标签索引，-1表示无
    int active_tab = -1;        // 切换到的标签索引，-1表示无变化
};

// 绘制主编辑器区域（类似 VSCode 的编辑器窗口）
// 当没有打开文件时显示欢迎界面
EditorAreaResult DrawEditorArea(
    float left_offset,          // 左侧偏移（activity bar + sidebar）
    float right_offset,         // 右侧偏移（secondary sidebar）
    float title_h,              // 标题栏高度
    float status_bar_h,         // 状态栏高度
    float panel_h,              // 底部面板高度
    bool panel_visible,         // 底部面板是否可见
    std::vector<EditorTab>& tabs  // 编辑器标签页列表
);

void HandleEditorAreaEvents(std::vector<EditorTab>& editor_tabs, int closed_tab, int active_tab);