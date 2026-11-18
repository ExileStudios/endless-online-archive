#ifndef Ctrl_popupelementH
#define Ctrl_popupelementH

#include <TDDBltFX.H>
#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

struct PopupElement
{
    TDx_DrawSurface *surface;
    TRect rect;
    bool needs_render;
    bool is_rendered;
    bool is_visible;
    int weakness;
    String text;
    int x;
    int y;
    int unused;
    int shown_at;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;
    TDDBltFX *blt_fx;

    PopupElement();
    ~PopupElement();
    void Show(int new_x, int new_y, int new_weakness, String new_text);
    void Render();
    void Tick();
    void Draw();
};

#endif
