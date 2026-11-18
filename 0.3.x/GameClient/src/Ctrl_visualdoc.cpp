#include <vcl.h>
#pragma hdrstop

#include "Ctrl_visualdoc.h"
#include "Tmainform.h"
#include "Gfx_game.h"
#include "Gfx_visualdoc.h"

#pragma package(smart_init)

VisualDocController::VisualDocController()
{
    window_x = 120;
    window_y = 16;
    is_visible = false;
    close_hovered = false;
    is_dragging_image = false;
    view_left = 26;
    view_top = 26;
    view_bottom = 242;
    view_right = 378;
    view_width = view_right - view_left;
    view_height = view_bottom - view_top;
}

VisualDocController::~VisualDocController()
{
}

// The place of the arrow's picture on the sheet, lit or dimmed.
TRect VisualDocController::GetArrowRect(int arrow, int dimmed)
{
    TRect rect;
    if (arrow <= 1)
    {
        rect.left = 407;
        rect.right = 418;
        rect.top = 12 * arrow + 2;
        rect.bottom = 12 * arrow + 9;
    }
    else
    {
        rect.left = 7 * arrow + 392;
        rect.right = 7 * arrow + 398;
        rect.top = 26;
        rect.bottom = 38;
    }
    if (dimmed > 0)
    {
        rect.top += 40;
        rect.bottom += 40;
    }
    return rect;
}

// Counts down the four timers while the window is shown.
void VisualDocController::Tick()
{
    if (is_visible)
    {
        for (int i = 0; i < 4; i++)
        {
            if (arrow_blink[i] > 0)
            {
                arrow_blink[i]--;
                MainForm->redraw_needed = true;
            }
        }
    }
}

// Opens the picture document, centred in the view where it fits and scrollable where it does not; false when it
// cannot be loaded.
bool VisualDocController::Open(int document_id)
{
    MainForm->gfx->CreateVisualDocGraphics();
    for (int i = 0; i < 4; i++)
        arrow_blink[i] = 0;
    TPoint size = MainForm->gfx->LoadVisualDocGraphics(document_id);
    if (size.x > 0)
    {
        image_width = size.x;
        image_height = size.y;
        scroll_x = 0;
        scroll_y = 0;
        if (image_width <= view_width)
        {
            scroll_x = (view_width - image_width) / 2;
            scrolls_x = false;
        }
        else
        {
            scroll_x += (view_width - view_width) / 2;
            scrolls_x = true;
            arrow_blink[2] = 18;
            arrow_blink[3] = 18;
        }
        if (image_height <= view_height)
        {
            scroll_y = (view_height - image_height) / 2;
            scrolls_y = false;
        }
        else
        {
            scroll_y += (image_height - view_height) / 2;
            scrolls_y = true;
            arrow_blink[0] = 18;
            arrow_blink[1] = 18;
        }
        is_visible = true;
        MainForm->redraw_needed = true;
        return true;
    }
    return false;
}

void VisualDocController::Close()
{
    MainForm->gfx->FreeVisualDocGraphics();
    is_visible = false;
    MainForm->redraw_needed = true;
}

// A click on a scrollable picture starts dragging it; any other click in the window starts moving the window.
void VisualDocController::MouseDown(int x, int y, int button)
{
    if (scrolls_x || scrolls_y)
    {
        if (x >= window_x + view_left && x <= window_x + view_left + view_width && y >= window_y + view_top &&
            y <= window_y + view_top + view_height)
        {
            is_dragging_image = true;
            drag_scroll_x = scroll_x + x;
            drag_scroll_y = scroll_y + y;
            drag_start_x = x;
            drag_start_y = y;
            return;
        }
    }
    if (window_x <= x && x <= window_x + 405 && y >= window_y && y <= window_y + 280)
    {
        is_dragging_window = true;
        drag_offset_x = x - window_x;
        drag_offset_y = y - window_y;
    }
}

// Scrolls a dragged picture within its edges, or moves a dragged window, kept on screen, and tracks the close button.
void VisualDocController::MouseMove(int x, int y)
{
    if (is_dragging_image)
    {
        if (scrolls_x)
        {
            int dx = drag_start_x - x;
            if (dx < 0)
                arrow_blink[3] = 4;
            if (dx > 0)
                arrow_blink[2] = 4;
            scroll_x = drag_scroll_x - x + dx;
            if (scroll_x < 0)
                scroll_x = 0;
            if (image_width - view_width < scroll_x)
                scroll_x = image_width - view_width;
        }
        if (scrolls_y)
        {
            int dy = drag_start_y - y;
            if (dy < 0)
                arrow_blink[1] = 4;
            if (dy > 0)
                arrow_blink[0] = 4;
            scroll_y = drag_scroll_y - y + dy;
            if (scroll_y < 0)
                scroll_y = 0;
            if (image_height - view_height < scroll_y)
                scroll_y = image_height - view_height;
        }
        MainForm->redraw_needed = true;
    }
    else if (is_dragging_window)
    {
        window_x = x - drag_offset_x;
        window_y = y - drag_offset_y;
        if (window_x <= 10)
            window_x = 10;
        if (window_y <= 8)
            window_y = 8;
        if (window_x >= 223)
            window_x = 223;
        if (window_y >= 18)
            window_y = 18;
        MainForm->redraw_needed = true;
    }
    else if (x >= window_x + 159 && x <= window_x + 249 && y >= window_y + 255 && y <= window_y + 283)
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

void VisualDocController::MouseUp()
{
    is_dragging_window = false;
    is_dragging_image = false;
}

// Draws the visible part of the picture and an arrow on each side it can still scroll toward, fading out.
void VisualDocController::Draw()
{
    short blink;
    TRect dest;
    TRect source;
    // A picture smaller than the view is drawn whole at its offset; a larger one shows the part
    // the scroll points at.
    if (!scrolls_x)
    {
        dest.left = view_left + window_x + scroll_x;
        dest.right = view_left + window_x + scroll_x + image_width;
        source.left = 0;
        source.right = image_width;
    }
    else
    {
        dest.left = view_left + window_x;
        dest.right = view_right + window_x;
        source.left = scroll_x;
        source.right = scroll_x + view_width;
    }
    if (!scrolls_y)
    {
        dest.top = view_top + window_y + scroll_y;
        dest.bottom = view_top + window_y + scroll_y + image_height;
        source.top = 0;
        source.bottom = image_height;
    }
    else
    {
        dest.top = view_top + window_y;
        dest.bottom = view_bottom + window_y;
        source.top = scroll_y;
        source.bottom = scroll_y + view_height;
    }
    MainForm->OffscreenSurface->Blt(&dest, MainForm->gfx->visual_doc->surface, &source, DDBLT_KEYSRC | DDBLT_WAIT, 0);
    for (int arrow = 0; arrow < 4; arrow++)
    {
        if (arrow_blink[arrow] > 0)
        {
            blink = arrow_blink[arrow];
            unsigned char alpha = blink % 6;
            if (alpha > 0)
            {
                alpha = 30 * alpha;
                unsigned char dimmed = false;
                int x = view_left + window_x;
                int y = view_top + window_y;
                if (!arrow)
                {
                    if (image_height - view_height <= scroll_y)
                        dimmed = true;
                    x += view_width / 2 - 5;
                    y -= 9;
                }
                if (arrow == 1)
                {
                    if (scroll_y <= 0)
                        dimmed = true;
                    x += view_width / 2 - 5;
                    y += view_height + 2;
                }
                if (arrow == 2)
                {
                    if (image_width - view_width <= scroll_x)
                        dimmed = true;
                    y += view_height / 2;
                    x -= 9;
                }
                if (arrow == 3)
                {
                    if (scroll_x <= 0)
                        dimmed = true;
                    y += view_height / 2;
                    x += view_width + 3;
                }
                MainForm->blitter->AlphaBlt(x, y, MainForm->OffscreenSurface, MainForm->gfx->game->visual_doc_surface,
                    &GetArrowRect(arrow, dimmed), alpha);
            }
        }
    }
}
