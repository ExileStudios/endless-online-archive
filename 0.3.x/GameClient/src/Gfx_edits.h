#ifndef Gfx_editsH
#define Gfx_editsH

#include <Classes.hpp>
#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

// Draws the game's own edit boxes onto the text canvas; it keeps the wrapped lines of a multi-line box.
struct EditCanvas
{
    TDx_DrawSurface *surface;
    bool caret_visible;
    int caret_x;
    int caret_y;
    TRect end_rect;
    TRect caret_rect;
    TRect meter_rect;
    TRect meter_back_rect;
    TStringList *lines;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    EditCanvas();
    ~EditCanvas();
    TRect GetBoxRect(short width);
    TRect GetFillRect(int level);
    void LoadResource();
    void DrawEditBoxes(int screen_id);
    void DrawCaret();
    void DrawBoxes(int screen_id);
    void DrawTexts(int screen_id);
};

#endif
