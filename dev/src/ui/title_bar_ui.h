#pragma once
#include <SDL3/SDL.h>
class TitleBarService;

// 只负责渲染和交互收集，不返回业务状态，所有状态通过 TitleBarService 管理
void RenderTitleBarUI(SDL_Window* window, TitleBarService& service, float title_h, 
                      bool primary_sidebar_visible = true, bool panel_visible = true, bool secondary_sidebar_visible = true);
