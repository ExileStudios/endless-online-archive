#ifndef Gfx_captchaH
#define Gfx_captchaH

#include <TDDColorKey.H>
#include <TDDSCaps.H>
#include <TDDSurfaceDesc.H>
#include <TDx_DrawSurface.H>

struct CaptchaGraphics
{
    TDx_DrawSurface *letters_surface;
    TDx_DrawSurface *noise_surface;
    String data;
    TDDSurfaceDesc *surface_desc;
    TDDSCaps *surface_caps;
    TDDColorKey *color_key;

    CaptchaGraphics();
    ~CaptchaGraphics();
    TRect GetLetterRect(unsigned char letter);
    TRect GetNoiseRect(unsigned char row);
    char Load(int unused);
    bool RenderNoise(int unused);
    bool RenderLetters(int unused);
};

#endif
