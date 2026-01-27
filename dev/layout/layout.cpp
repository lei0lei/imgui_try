#include "layout.h"
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

void TitleBar::render() {
    std::cout << "Rendering TitleBar" << std::endl;
}

} // namespace layout