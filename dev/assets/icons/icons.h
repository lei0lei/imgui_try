#pragma once

#include "imgui.h"

// VS Code style icon library - Hand-drawn for maximum performance
// All icons are programmatically rendered, no external assets needed

namespace Icons {

// File operations
void DrawFileIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);
void DrawFolderIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);
void DrawFolderOpenIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);
void DrawSaveIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);

// Edit operations
void DrawUndoIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);
void DrawRedoIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);
void DrawCopyIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);
void DrawPasteIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);
void DrawCutIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);

// View operations
void DrawSearchIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);
void DrawSettingsIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);
void DrawInfoIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);

// Navigation
void DrawChevronLeftIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);
void DrawChevronRightIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);
void DrawChevronUpIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);
void DrawChevronDownIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);

// Status
void DrawCheckIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);
void DrawCrossIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);
void DrawWarningIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);
void DrawErrorIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);

// Code editor
void DrawCodeIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);
void DrawTerminalIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);
void DrawDebugIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color);

} // namespace Icons
