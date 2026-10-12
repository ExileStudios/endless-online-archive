#ifndef Gfx_balloonH
#define Gfx_balloonH

class TDDBltFX;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// A speech balloon: a fill surface with the bubble and its tail, and a see-through text surface with the outline.
struct Balloon
{
    TDx_DrawSurface *fill_surface;
    TDx_DrawSurface *text_surface;
    Windows::TRect bubble_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;
    TDDBltFX *fill_fx;

    Balloon(int text_width, int text_height, String *lines, int style);
    ~Balloon();
};

#endif
