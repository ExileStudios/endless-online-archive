#ifndef Gfx_alphaH
#define Gfx_alphaH

#include <TDx_DrawSurface.H>

struct MapGlow;

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
    unsigned char is_supported;
    unsigned char light_table[250][250];
    unsigned char light_table_size;
    char unused_bytes[20];
    unsigned char darkness_map[620][300];
    bool flicker;
    float pulse;
    unsigned char pulse_step;
    unsigned char darkness;
    float light_scale;
    unsigned short red_bits;
    unsigned short green_bits;
    unsigned short blue_bits;
    unsigned short red_shift;
    unsigned short green_shift;
    unsigned short blue_shift;
    int pixel_format;
    short lights_added;
    short effects_drawn;
    TRect lock_rect;
    int pitch_gap;
    int source_gap;
    int pixel;
    int tint_color;
    int tint_red;
    int tint_green;
    int tint_blue;
    unsigned int pixel_red;
    unsigned int pixel_green;
    unsigned int pixel_blue;
    int blend_red;
    int blend_green;
    int blend_blue;
    int blend_bias;
    int alpha_pair;
    char odd_width;

    ExtBlts(TDx_DrawSurface *surface);
    ~ExtBlts();
    int MakeColor(int red, int green, int blue);
    int ReadPixel(int x, int y, TDx_DrawSurface *surface);
    int GetMaskShiftAndWidth(int mask, unsigned short *shift, unsigned short *width);
    void ResetDarkness(int amount, unsigned char brightest);
    void AddLight(short x, short y, short size, unsigned char strength);
    int ApplyDarkness(TDx_DrawSurface *surface);
    void AddMapLights();
    int DrawMapEffects(TDx_DrawSurface *surface);
    int DrawGlow(TDx_DrawSurface *surface, MapGlow *source);
    int BlendGlow(TDx_DrawSurface *surface, short x, short y, unsigned char strength, short size, int color);
    int FlickerValue(char x, unsigned char y, short center, unsigned char strength);
    void BuildLightTable(short size, unsigned char strength);
    float Falloff(short center, short x, short y, short radius);
    int DrawBeam(TDx_DrawSurface *surface, MapGlow *source);
    int GetLightColor(unsigned char index);
    int BlendBeam(TDx_DrawSurface *surface, short x, short y, unsigned char strength, int color);
    int DrawRay(TDx_DrawSurface *surface, MapGlow *source);
    int BlendRay(TDx_DrawSurface *surface, short x, short y, unsigned char strength, int color);
    int TintScreen(TDx_DrawSurface *surface, unsigned char side, int percent, int color);
    int AlphaBlt(int dest_x, int dest_y, TDx_DrawSurface *dest_surface, TDx_DrawSurface *source_surface,
    TRect *source_rect, int alpha);
    int HalfBlendBlt(int dest_x, int dest_y, TDx_DrawSurface *dest_surface, TDx_DrawSurface *source_surface,
    TRect *source_rect);
};

#endif
