#include <vcl.h>
#pragma hdrstop

#include "Ctrl_stats.h"
#include "Tmainform.h"
#pragma package(smart_init)

StatController::StatController()
{
    window_x = 15;
    window_y = 53;
    is_visible = false;
}

StatController::~StatController()
{
}

// Loads the window's graphics and shows it.
void StatController::Open()
{
    MainForm->gfx->CreateStatGraphics();
    MainForm->gfx->LoadStatGraphics();
    is_visible = true;
    MainForm->redraw_needed = true;
}

// Hides the window and frees its graphics.
void StatController::Close()
{
    is_visible = false;
    MainForm->gfx->FreeStatGraphics();
    MainForm->redraw_needed = true;
}

// A click inside the window starts moving it.
void StatController::MouseDown(int x, int y, int button)
{
    if (x >= window_x && x <= window_x + 284 && y >= window_y && y <= window_y + 250 && !is_dragging)
    {
        is_dragging = true;
        drag_offset_x = x - window_x;
        drag_offset_y = y - window_y;
    }
}

// Moves a dragged window, kept on screen, and tracks the close button.
void StatController::MouseMove(int x, int y)
{
    if (is_dragging)
    {
        window_x = x - drag_offset_x;
        window_y = y - drag_offset_y;
        if (window_x <= 10)
            window_x = 10;
        if (window_y <= 8)
            window_y = 8;
        if (window_x >= 346)
            window_x = 346;
        if (window_y >= 59)
            window_y = 59;
        MainForm->redraw_needed = true;
    }
    if (x >= window_x + 92 && x <= window_x + 182 && y >= window_y + 213 && y <= window_y + 239)
    {
        if (!close_hovered)
        {
            close_hovered = true;
            MainForm->redraw_needed = true;
        }
    }
    else if (close_hovered)
    {
        close_hovered = false;
        MainForm->redraw_needed = true;
    }
}

// Ends moving the window.
void StatController::MouseUp()
{
    is_dragging = false;
}
