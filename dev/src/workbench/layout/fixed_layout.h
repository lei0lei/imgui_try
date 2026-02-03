#ifndef LAYOUT_H
#define LAYOUT_H

#include <string>
#include <memory>

namespace layout {

class FixedLayout {
public:
    virtual void render() = 0;
    virtual ~FixedLayout() = default;
};

class ActivityBar : public FixedLayout {
public:
    void render() override;
};

class Panel : public FixedLayout {
public:
    void render() override;
};

class PrimarySidebar : public FixedLayout {
public:
    void render() override;
};

class SecondaryBar : public FixedLayout {
public:
    void render() override;
};

class StatusBar : public FixedLayout {
public:
    void render() override;
};

class TitleBar : public FixedLayout {
private:
    std::string title_;
    bool primarySidebarVisible;
    bool panelVisible;
    bool secondarySidebarVisible;
public:
    TitleBar();
    void render() override;
    void handleEvents(); // 处理交互逻辑
};

} // namespace layout

#endif // LAYOUT_H