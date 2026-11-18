#include <vcl.h>
#pragma hdrstop

#include "Fonts.h"
#include "Tmainform.h"

#pragma package(smart_init)

Fonts::Fonts(char enabled)
{
    is_enabled = enabled;
    is_forced = false;
    canvas = new TCanvas();
    canvas->Handle = 0;
    dc = 0;
    canvas->Pen->Style = psSolid;
    canvas->Brush->Style = bsClear;
}

Fonts::~Fonts()
{
    delete canvas;
}

char Fonts::BeginOffscreen()
{
    Begin(false, false, MainForm->OffscreenSurface);
}

char Fonts::BeginOn(TDx_DrawSurface *target)
{
    Begin(false, false, target);
}

char Fonts::BeginOffscreenIf(unsigned char enabled)
{
    Begin(enabled, false, MainForm->OffscreenSurface);
}

// Takes the surface's DC for drawing text when fonts are on or forced; false when it cannot be taken.
bool Fonts::Begin(char enabled, char forced, TDx_DrawSurface *target)
{
    is_forced = forced;
    if (is_enabled || is_forced)
    {
        if (target->GetDC(&dc))
        {
            surface = target;
            canvas->Handle = dc;
            return true;
        }
        dc = 0;
        return false;
    }
    return true;
}

// Draws the text at the point in the set colour, size, face and underline while drawing is on; the underline is for
// this text only.
bool Fonts::DrawString(short x, short y, String text)
{
    if (is_enabled || is_forced)
    {
        if (canvas->Handle == NULL)
            return false;
        canvas->Font->Color = (TColor)color;
        canvas->Font->Size = size;
        if (!face)
            canvas->Font->Name = "Arial";
        else if (face == 1)
            canvas->Font->Name = "MS Sans Serif";
        else if (face == 2)
            canvas->Font->Name = "Forte";
        if (is_underlined)
            canvas->Font->Style = TFontStyles() << fsUnderline;
        canvas->TextOut(x, y, text);
        if (is_underlined)
            canvas->Font->Style = TFontStyles();
    }
    is_underlined = false;
    unused_flag = false;
    return true;
}

// Ends a run of text drawing and releases the surface it drew on; false when none was held.
bool Fonts::End()
{
    if (is_enabled || is_forced)
    {
        canvas->Handle = 0;
        is_forced = false;
        HDC handle = dc;
        if (handle)
        {
            surface->ReleaseDC(handle);
            dc = 0;
            surface = 0;
            return true;
        }
        return false;
    }
    return true;
}

// The width of the text in the font's face and size; 0 when fonts are off.
int Fonts::TextWidth(String text)
{
    if (is_enabled || is_forced)
    {
        canvas->Font->Size = size;
        if (!face)
            canvas->Font->Name = "Arial";
        else if (face == 1)
            canvas->Font->Name = "MS Sans Serif";
        else if (face == 2)
            canvas->Font->Name = "Forte";
        return canvas->TextWidth(text);
    }
    return 0;
}

// The height of the text in the font's face and size; 0 when fonts are off.
int Fonts::TextHeight(String text)
{
    if (is_enabled || is_forced)
    {
        canvas->Font->Size = size;
        if (!face)
            canvas->Font->Name = "Arial";
        else if (face == 1)
            canvas->Font->Name = "MS Sans Serif";
        else if (face == 2)
            canvas->Font->Name = "Forte";
        return canvas->TextHeight(text);
    }
    return 0;
}

TCanvas *Fonts::GetCanvas()
{
    return canvas;
}

// Halves each part of the text colour.
void Fonts::DarkenColor()
{
    unsigned char red = (unsigned char)color / 2;
    unsigned char green = (((unsigned short)color >> 8) & 0xFF) / 2;
    unsigned char blue = (((int)color >> 16) & 0xFF) / 2;
    color = red | ((unsigned short)green << 8) | (blue << 16);
}
