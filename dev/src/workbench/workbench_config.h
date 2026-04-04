#pragma once

#include "../core/workbench_theme.h"

class WorkbenchConfig {
public:
    const WorkbenchMetrics& Metrics() const { return GetWorkbenchMetrics(); }

    void ApplyStyle(float main_scale) const;
    bool LoadFromFile(const char* path);

private:
    // metrics stored globally for shared UI layout
};
