#ifndef Gfx_msgboardH
#define Gfx_msgboardH

#define TALL_FRAME_HEIGHT 290
struct GraphicSize;
class TDDColorKey;
class TDDSCaps;
class TDDSurfaceDesc;
class TDx_DrawSurface;

// The message board sheet: two window frames stacked one above the other.
// The quest window draws from it too.
struct MsgBoardGraphics
{
    TDx_DrawSurface *surface;
    int sheet_left;
    int sheet_top;
    int sheet_width;
    int sheet_height;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    MsgBoardGraphics();
    ~MsgBoardGraphics();
    static Windows::TRect GetFrameRect(MsgBoardGraphics *self, int frame);
    GraphicSize LoadSurface(HMODULE library, int resource_id);
};

#endif
