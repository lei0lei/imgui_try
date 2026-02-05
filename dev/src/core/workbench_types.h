#pragma once

// Shared UI/workbench enums to decouple UI from service implementations.

enum class ActivityBarItem {
    None = 0,
    Explorer,
    Search,
    NodeEditor,
    Debug,
    Extensions
};

enum class TitleBarMenu {
    None = 0,
    File,
    Edit,
    View,
    Help
};
