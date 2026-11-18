#ifndef GfxtransfersH
#define GfxtransfersH

#include <Graphics.hpp>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

struct GfxTransfers
{
    int width;
    int height;
    char key_red;
    char key_green;
    unsigned char key_blue;
    bool is_loaded;
    bool has_dc;
    Graphics::TBitmap *bitmap;
    TCanvas *canvas;
    HDC dc;
    HDC bitmap_dc;

    GfxTransfers();
    ~GfxTransfers();
    bool LoadResourceBitmap(int library, int resource_id);
    void SetTransparentPixel(int x, int y);
    void SetTransparentColor(int color);
    bool BeginDraw(TDx_DrawSurface *surface);
    void EndDraw(TDx_DrawSurface *surface);
    void Draw(TDx_DrawSurface *surface);
    bool DrawAt(TDx_DrawSurface *surface, int x, int y);
    void PutPixel(int x, int y, unsigned char red, char green, unsigned char blue);
    void CreateSurface(TDx_DrawSurface *surface, TDDSurfaceDesc *desc, int surface_width, int surface_height);
};

#endif
