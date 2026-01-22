#include "icons.h"
#define _USE_MATH_DEFINES
#include <math.h>

namespace Icons {

// ========== File Operations ==========

void DrawFileIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // File body with folded corner
    ImVec2 pts[6] = {
        ImVec2(pos.x - s*0.6f, pos.y - s),       // Top left
        ImVec2(pos.x + s*0.3f, pos.y - s),       // Top right (before fold)
        ImVec2(pos.x + s*0.6f, pos.y - s*0.7f),  // Fold corner
        ImVec2(pos.x + s*0.6f, pos.y + s),       // Bottom right
        ImVec2(pos.x - s*0.6f, pos.y + s),       // Bottom left
    };
    
    draw_list->AddConvexPolyFilled(pts, 5, color);
    
    // Outline
    ImU32 outline = IM_COL32(
        (color >> 0) & 0xFF,
        (color >> 8) & 0xFF,
        (color >> 16) & 0xFF,
        100
    );
    draw_list->AddPolyline(pts, 5, outline, ImDrawFlags_Closed, 1.5f);
    
    // Fold line
    draw_list->AddLine(
        ImVec2(pos.x + s*0.3f, pos.y - s),
        ImVec2(pos.x + s*0.6f, pos.y - s*0.7f),
        outline, 1.5f);
    draw_list->AddLine(
        ImVec2(pos.x + s*0.3f, pos.y - s),
        ImVec2(pos.x + s*0.3f, pos.y - s*0.7f),
        outline, 1.5f);
}

void DrawFolderIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // Folder body with tab
    ImVec2 pts[7] = {
        ImVec2(pos.x - s*0.9f, pos.y + s*0.6f),  // Bottom left
        ImVec2(pos.x - s*0.9f, pos.y - s*0.2f),  // Left side
        ImVec2(pos.x - s*0.3f, pos.y - s*0.6f),  // Tab left
        ImVec2(pos.x + s*0.2f, pos.y - s*0.6f),  // Tab right
        ImVec2(pos.x + s*0.3f, pos.y - s*0.4f),  // Tab corner
        ImVec2(pos.x + s*0.9f, pos.y - s*0.2f),  // Right side
        ImVec2(pos.x + s*0.9f, pos.y + s*0.6f),  // Bottom right
    };
    
    draw_list->AddConvexPolyFilled(pts, 7, color);
    draw_list->AddPolyline(pts, 7, IM_COL32(0,0,0,80), ImDrawFlags_Closed, 1.5f);
}

void DrawFolderOpenIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // Open folder (trapezoid)
    ImVec2 pts[4] = {
        ImVec2(pos.x - s*0.9f, pos.y - s*0.2f),
        ImVec2(pos.x - s*0.6f, pos.y + s*0.6f),
        ImVec2(pos.x + s*0.7f, pos.y + s*0.6f),
        ImVec2(pos.x + s*0.9f, pos.y - s*0.2f),
    };
    
    draw_list->AddConvexPolyFilled(pts, 4, color);
    draw_list->AddPolyline(pts, 4, IM_COL32(0,0,0,80), ImDrawFlags_Closed, 1.5f);
    
    // Tab
    draw_list->AddRectFilled(
        ImVec2(pos.x - s*0.5f, pos.y - s*0.7f),
        ImVec2(pos.x + s*0.1f, pos.y - s*0.2f),
        color);
}

void DrawSaveIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // Floppy disk body
    draw_list->AddRectFilled(
        ImVec2(pos.x - s*0.8f, pos.y - s*0.8f),
        ImVec2(pos.x + s*0.8f, pos.y + s*0.8f),
        color, 2.0f);
    
    // Top label (darker)
    ImU32 dark = IM_COL32(
        ((color >> 0) & 0xFF) * 0.6f,
        ((color >> 8) & 0xFF) * 0.6f,
        ((color >> 16) & 0xFF) * 0.6f,
        255
    );
    draw_list->AddRectFilled(
        ImVec2(pos.x - s*0.8f, pos.y - s*0.8f),
        ImVec2(pos.x + s*0.8f, pos.y - s*0.3f),
        dark);
    
    // Label slot
    draw_list->AddRectFilled(
        ImVec2(pos.x - s*0.5f, pos.y - s*0.7f),
        ImVec2(pos.x + s*0.2f, pos.y - s*0.4f),
        IM_COL32(30, 30, 30, 255));
    
    // Write protect notch
    draw_list->AddRectFilled(
        ImVec2(pos.x + s*0.3f, pos.y + s*0.3f),
        ImVec2(pos.x + s*0.7f, pos.y + s*0.7f),
        IM_COL32(0, 0, 0, 60));
}

// ========== Edit Operations ==========

void DrawUndoIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // Arc (3/4 circle counterclockwise)
    draw_list->PathArcTo(pos, s * 0.6f, -IM_PI * 0.7f, IM_PI * 0.5f, 12);
    draw_list->PathStroke(color, ImDrawFlags_None, 2.0f);
    
    // Arrow head (pointing left)
    ImVec2 arrow_tip = ImVec2(pos.x - s*0.6f, pos.y + s*0.1f);
    draw_list->AddTriangleFilled(
        arrow_tip,
        ImVec2(arrow_tip.x + s*0.35f, arrow_tip.y - s*0.3f),
        ImVec2(arrow_tip.x + s*0.35f, arrow_tip.y + s*0.3f),
        color);
}

void DrawRedoIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // Arc (3/4 circle clockwise)
    draw_list->PathArcTo(pos, s * 0.6f, IM_PI * 0.2f, -IM_PI * 1.2f, 12);
    draw_list->PathStroke(color, ImDrawFlags_None, 2.0f);
    
    // Arrow head (pointing right)
    ImVec2 arrow_tip = ImVec2(pos.x + s*0.6f, pos.y + s*0.1f);
    draw_list->AddTriangleFilled(
        arrow_tip,
        ImVec2(arrow_tip.x - s*0.35f, arrow_tip.y - s*0.3f),
        ImVec2(arrow_tip.x - s*0.35f, arrow_tip.y + s*0.3f),
        color);
}

void DrawCopyIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // Back page
    draw_list->AddRect(
        ImVec2(pos.x - s*0.3f, pos.y - s*0.9f),
        ImVec2(pos.x + s*0.7f, pos.y + s*0.1f),
        color, 2.0f, 0, 2.0f);
    
    // Front page (solid)
    draw_list->AddRectFilled(
        ImVec2(pos.x - s*0.7f, pos.y - s*0.5f),
        ImVec2(pos.x + s*0.3f, pos.y + s*0.9f),
        color, 2.0f);
    
    // Lines on front page
    ImU32 line_col = IM_COL32(255, 255, 255, 100);
    draw_list->AddLine(
        ImVec2(pos.x - s*0.5f, pos.y - s*0.2f),
        ImVec2(pos.x + s*0.1f, pos.y - s*0.2f),
        line_col, 1.5f);
    draw_list->AddLine(
        ImVec2(pos.x - s*0.5f, pos.y + s*0.1f),
        ImVec2(pos.x + s*0.1f, pos.y + s*0.1f),
        line_col, 1.5f);
}

void DrawPasteIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // Clipboard
    draw_list->AddRectFilled(
        ImVec2(pos.x - s*0.6f, pos.y - s*0.4f),
        ImVec2(pos.x + s*0.6f, pos.y + s*0.9f),
        color, 2.0f);
    
    // Clip top
    draw_list->AddRectFilled(
        ImVec2(pos.x - s*0.3f, pos.y - s*0.8f),
        ImVec2(pos.x + s*0.3f, pos.y - s*0.3f),
        color);
    draw_list->AddCircleFilled(
        ImVec2(pos.x, pos.y - s*0.7f),
        s * 0.2f, IM_COL32(255, 255, 255, 150));
}

void DrawCutIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // Scissors - two circles connected by X
    draw_list->AddCircle(ImVec2(pos.x - s*0.4f, pos.y + s*0.5f), s*0.3f, color, 12, 2.0f);
    draw_list->AddCircle(ImVec2(pos.x + s*0.4f, pos.y + s*0.5f), s*0.3f, color, 12, 2.0f);
    
    // Blades (X shape)
    draw_list->AddLine(
        ImVec2(pos.x - s*0.2f, pos.y + s*0.3f),
        ImVec2(pos.x, pos.y - s*0.7f),
        color, 2.0f);
    draw_list->AddLine(
        ImVec2(pos.x + s*0.2f, pos.y + s*0.3f),
        ImVec2(pos.x, pos.y - s*0.7f),
        color, 2.0f);
}

// ========== View Operations ==========

void DrawSearchIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // Magnifying glass circle
    draw_list->AddCircle(
        ImVec2(pos.x - s*0.2f, pos.y - s*0.2f),
        s * 0.5f, color, 16, 2.0f);
    
    // Handle
    draw_list->AddLine(
        ImVec2(pos.x + s*0.2f, pos.y + s*0.2f),
        ImVec2(pos.x + s*0.7f, pos.y + s*0.7f),
        color, 2.5f);
}

void DrawSettingsIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // Gear - center circle + 6 teeth
    draw_list->AddCircleFilled(pos, s * 0.35f, color, 16);
    
    for (int i = 0; i < 6; i++)
    {
        float angle = i * IM_PI / 3.0f;
        float cos_a = cosf(angle);
        float sin_a = sinf(angle);
        
        ImVec2 p1 = ImVec2(pos.x + cos_a * s * 0.5f, pos.y + sin_a * s * 0.5f);
        ImVec2 p2 = ImVec2(pos.x + cos_a * s * 0.8f, pos.y + sin_a * s * 0.8f);
        draw_list->AddLine(p1, p2, color, s * 0.3f);
    }
    
    // Center hole
    draw_list->AddCircleFilled(pos, s * 0.2f, IM_COL32(0, 0, 0, 100), 12);
}

void DrawInfoIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // Circle
    draw_list->AddCircle(pos, s * 0.8f, color, 20, 2.0f);
    
    // i dot
    draw_list->AddCircleFilled(ImVec2(pos.x, pos.y - s*0.5f), s*0.15f, color);
    
    // i stem
    draw_list->AddRectFilled(
        ImVec2(pos.x - s*0.1f, pos.y - s*0.2f),
        ImVec2(pos.x + s*0.1f, pos.y + s*0.5f),
        color);
}

// ========== Navigation ==========

void DrawChevronLeftIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    ImVec2 pts[3] = {
        ImVec2(pos.x + s*0.4f, pos.y - s*0.6f),
        ImVec2(pos.x - s*0.4f, pos.y),
        ImVec2(pos.x + s*0.4f, pos.y + s*0.6f),
    };
    
    draw_list->AddPolyline(pts, 3, color, ImDrawFlags_None, 2.5f);
}

void DrawChevronRightIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    ImVec2 pts[3] = {
        ImVec2(pos.x - s*0.4f, pos.y - s*0.6f),
        ImVec2(pos.x + s*0.4f, pos.y),
        ImVec2(pos.x - s*0.4f, pos.y + s*0.6f),
    };
    
    draw_list->AddPolyline(pts, 3, color, ImDrawFlags_None, 2.5f);
}

void DrawChevronUpIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    ImVec2 pts[3] = {
        ImVec2(pos.x - s*0.6f, pos.y + s*0.4f),
        ImVec2(pos.x, pos.y - s*0.4f),
        ImVec2(pos.x + s*0.6f, pos.y + s*0.4f),
    };
    
    draw_list->AddPolyline(pts, 3, color, ImDrawFlags_None, 2.5f);
}

void DrawChevronDownIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    ImVec2 pts[3] = {
        ImVec2(pos.x - s*0.6f, pos.y - s*0.4f),
        ImVec2(pos.x, pos.y + s*0.4f),
        ImVec2(pos.x + s*0.6f, pos.y - s*0.4f),
    };
    
    draw_list->AddPolyline(pts, 3, color, ImDrawFlags_None, 2.5f);
}

// ========== Status Icons ==========

void DrawCheckIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // Checkmark
    ImVec2 pts[3] = {
        ImVec2(pos.x - s*0.6f, pos.y),
        ImVec2(pos.x - s*0.2f, pos.y + s*0.5f),
        ImVec2(pos.x + s*0.7f, pos.y - s*0.6f),
    };
    
    draw_list->AddPolyline(pts, 3, color, ImDrawFlags_None, 2.5f);
}

void DrawCrossIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.6f;
    
    // X shape
    draw_list->AddLine(
        ImVec2(pos.x - s, pos.y - s),
        ImVec2(pos.x + s, pos.y + s),
        color, 2.5f);
    draw_list->AddLine(
        ImVec2(pos.x - s, pos.y + s),
        ImVec2(pos.x + s, pos.y - s),
        color, 2.5f);
}

void DrawWarningIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // Triangle
    ImVec2 pts[3] = {
        ImVec2(pos.x, pos.y - s*0.8f),
        ImVec2(pos.x - s*0.8f, pos.y + s*0.7f),
        ImVec2(pos.x + s*0.8f, pos.y + s*0.7f),
    };
    
    draw_list->AddTriangleFilled(pts[0], pts[1], pts[2], color);
    
    // Exclamation mark
    draw_list->AddRectFilled(
        ImVec2(pos.x - s*0.1f, pos.y - s*0.4f),
        ImVec2(pos.x + s*0.1f, pos.y + s*0.2f),
        IM_COL32(0, 0, 0, 255));
    draw_list->AddCircleFilled(
        ImVec2(pos.x, pos.y + s*0.45f),
        s * 0.12f, IM_COL32(0, 0, 0, 255));
}

void DrawErrorIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // Circle
    draw_list->AddCircleFilled(pos, s * 0.8f, color, 20);
    
    // X in center (white)
    ImU32 white = IM_COL32(255, 255, 255, 255);
    float xs = s * 0.4f;
    draw_list->AddLine(
        ImVec2(pos.x - xs, pos.y - xs),
        ImVec2(pos.x + xs, pos.y + xs),
        white, 2.5f);
    draw_list->AddLine(
        ImVec2(pos.x - xs, pos.y + xs),
        ImVec2(pos.x + xs, pos.y - xs),
        white, 2.5f);
}

// ========== Code Editor Icons ==========

void DrawCodeIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // < symbol
    ImVec2 pts_left[3] = {
        ImVec2(pos.x - s*0.1f, pos.y - s*0.6f),
        ImVec2(pos.x - s*0.6f, pos.y),
        ImVec2(pos.x - s*0.1f, pos.y + s*0.6f),
    };
    draw_list->AddPolyline(pts_left, 3, color, ImDrawFlags_None, 2.0f);
    
    // > symbol
    ImVec2 pts_right[3] = {
        ImVec2(pos.x + s*0.1f, pos.y - s*0.6f),
        ImVec2(pos.x + s*0.6f, pos.y),
        ImVec2(pos.x + s*0.1f, pos.y + s*0.6f),
    };
    draw_list->AddPolyline(pts_right, 3, color, ImDrawFlags_None, 2.0f);
}

void DrawTerminalIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // Window frame
    draw_list->AddRect(
        ImVec2(pos.x - s*0.8f, pos.y - s*0.6f),
        ImVec2(pos.x + s*0.8f, pos.y + s*0.6f),
        color, 2.0f, 0, 2.0f);
    
    // > prompt
    ImVec2 pts[3] = {
        ImVec2(pos.x - s*0.5f, pos.y - s*0.3f),
        ImVec2(pos.x - s*0.1f, pos.y),
        ImVec2(pos.x - s*0.5f, pos.y + s*0.3f),
    };
    draw_list->AddPolyline(pts, 3, color, ImDrawFlags_None, 2.0f);
    
    // Cursor line
    draw_list->AddLine(
        ImVec2(pos.x + s*0.1f, pos.y - s*0.15f),
        ImVec2(pos.x + s*0.5f, pos.y - s*0.15f),
        color, 2.0f);
}

void DrawDebugIcon(ImDrawList* draw_list, ImVec2 pos, float size, ImU32 color)
{
    float s = size * 0.5f;
    
    // Bug body (circle)
    draw_list->AddCircleFilled(pos, s * 0.5f, color, 12);
    
    // Antennae
    draw_list->AddLine(
        ImVec2(pos.x - s*0.3f, pos.y - s*0.5f),
        ImVec2(pos.x - s*0.5f, pos.y - s*0.9f),
        color, 2.0f);
    draw_list->AddLine(
        ImVec2(pos.x + s*0.3f, pos.y - s*0.5f),
        ImVec2(pos.x + s*0.5f, pos.y - s*0.9f),
        color, 2.0f);
    
    // Legs (3 on each side)
    for (int i = 0; i < 3; i++)
    {
        float y_offset = (i - 1) * s * 0.3f;
        // Left legs
        draw_list->AddLine(
            ImVec2(pos.x - s*0.5f, pos.y + y_offset),
            ImVec2(pos.x - s*0.9f, pos.y + y_offset + s*0.2f),
            color, 1.5f);
        // Right legs
        draw_list->AddLine(
            ImVec2(pos.x + s*0.5f, pos.y + y_offset),
            ImVec2(pos.x + s*0.9f, pos.y + y_offset + s*0.2f),
            color, 1.5f);
    }
}

} // namespace Icons
