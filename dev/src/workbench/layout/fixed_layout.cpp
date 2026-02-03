#include "fixed_layout.h"
#include <iostream>

namespace layout {

void ActivityBar::render() {
    std::cout << "Rendering ActivityBar" << std::endl;
}

void Panel::render() {
    std::cout << "Rendering Panel" << std::endl;
}

void PrimarySidebar::render() {
    std::cout << "Rendering PrimarySidebar" << std::endl;
}

void SecondaryBar::render() {
    std::cout << "Rendering SecondaryBar" << std::endl;
}

void StatusBar::render() {
    std::cout << "Rendering StatusBar" << std::endl;
}

TitleBar::TitleBar()
    : primarySidebarVisible(true), panelVisible(false), secondarySidebarVisible(false) {}

void TitleBar::render() {
    std::cout << "Rendering TitleBar" << std::endl;
}


void TitleBar::handleEvents() {
    // 处理 TitleBar 的交互逻辑，例如切换侧边栏
    if (true) {
        primarySidebarVisible = !primarySidebarVisible;
    }
    if (true) {
        panelVisible = !panelVisible;
    }
    if (true) {
        secondarySidebarVisible = !secondarySidebarVisible;
    }
}

} // namespace layout