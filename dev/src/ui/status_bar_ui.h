#pragma once
#include <SDL3/SDL.h>
#include "../services/notification_service.h"

// 只负责渲染和交互收集，所有状态通过 NotificationService 管理
void RenderStatusBarUI(SDL_Window* window, NotificationService& service, float status_bar_h, float title_h);
