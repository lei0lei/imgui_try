#pragma once

// Deprecated legacy layout placeholder.
// Kept only for compatibility with stale/generated build files.
namespace layout {

class FixedLayout {
public:
    virtual ~FixedLayout() = default;
    virtual void render() {}
};

} // namespace layout
