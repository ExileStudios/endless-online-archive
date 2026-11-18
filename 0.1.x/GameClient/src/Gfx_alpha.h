#ifndef Gfx_alphaH
#define Gfx_alphaH

enum GreenMask
{
    GreenMask_Rgb555 = 992,
    GreenMask_Rgb565 = 2016
};

enum RedMask
{
    RedMask_Rgb555 = 31744,
    RedMask_Rgb565 = 63488
};

struct TDx_DrawSurface;

// Blits done by hand in surface memory, alpha blending and half blending, in the display's pixel format.
struct ExtBlts
{
    bool is_supported;
    unsigned short red_bits;
    unsigned short green_bits;
    unsigned short blue_bits;
    unsigned short red_shift;
    unsigned short green_shift;
    unsigned short blue_shift;
    int pixel_format;

    ExtBlts(TDx_DrawSurface *surface);
    ~ExtBlts();
    unsigned int MakeColor(unsigned int red, unsigned int green, unsigned int blue);
    void GetMaskShiftAndWidth(int mask, unsigned short *shift, unsigned short *width);
    int AlphaBlt(int dest_x, int dest_y, TDx_DrawSurface *dest_surface, TDx_DrawSurface *source_surface,
        Windows::TRect *source_rect, int alpha);
    int HalfBlendBlt(int dest_x, int dest_y, TDx_DrawSurface *dest_surface, TDx_DrawSurface *source_surface,
        Windows::TRect *source_rect);
};

#endif
