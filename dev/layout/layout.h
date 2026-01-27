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
public:
    void render() override;
};

} // namespace layout

#endif // LAYOUT_H