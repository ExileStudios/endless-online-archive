#ifndef FontsH
#define FontsH

#include <Graphics.hpp>
#include <TDx_DrawSurface.H>

struct Fonts
{
    short size;
    unsigned char face;
    unsigned int color;
    char unused_bytes[4];
    bool unused_flag;
    bool is_underlined;
    char is_enabled;
    char is_forced;
    TDx_DrawSurface *surface;
    TCanvas *canvas;
    HDC dc;

    Fonts(char enabled);
    ~Fonts();
    char BeginOffscreen();
    char BeginOn(TDx_DrawSurface *target);
    char BeginOffscreenIf(unsigned char enabled);
    bool Begin(char enabled, char forced, TDx_DrawSurface *target);
    bool DrawString(short x, short y, String text);
    bool End();
    int TextWidth(String text);
    int TextHeight(String text);
    TCanvas * GetCanvas();
};

#endif
