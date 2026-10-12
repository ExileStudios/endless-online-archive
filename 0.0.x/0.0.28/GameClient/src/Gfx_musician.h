#ifndef Gfx_musicianH
#define Gfx_musicianH

struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The musician window surfaces: the opaque background and a sheet of square cells.
struct MusicianGraphics
{
    TDx_DrawSurface *background_surface;
    TDx_DrawSurface *cell_sheet_surface;
    Windows::TRect background_rect;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    MusicianGraphics();
    ~MusicianGraphics();
    static Windows::TRect GetCellRect(MusicianGraphics *self, int column, int row);
    GraphicSize LoadBackground(HMODULE library, int resource_id);
    GraphicSize LoadCellSheet(HMODULE library, int resource_id);
};

#endif
