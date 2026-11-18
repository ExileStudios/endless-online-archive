#include <vcl.h>
#pragma hdrstop

#include "Gfx_captcha.h"
#include "Tmainform.h"
#include <stdlib.h>

#pragma package(smart_init)

CaptchaGraphics::CaptchaGraphics()
{
    letters_surface = new TDx_DrawSurface(NULL);
    noise_surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    surface_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    surface_desc->Width = 220;
    surface_desc->Height = 201;
    letters_surface->GetSurfaceDesc(surface_desc);
    letters_surface->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 883;
    surface_desc->Height = 37;
    noise_surface->GetSurfaceDesc(surface_desc);
    noise_surface->Create(surface_desc, MainForm->Dx_Draw1);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
}

CaptchaGraphics::~CaptchaGraphics()
{
    noise_surface->Destroy();
    delete noise_surface;
    letters_surface->Destroy();
    delete letters_surface;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    delete color_key;
}

// The place of a capital letter's picture on the letter sheet; any other character takes the first one.
TRect CaptchaGraphics::GetLetterRect(unsigned char letter)
{
    TRect rect;
    char index = 0;
    if ((letter >= 'A'))
    {
        if ((letter <= 'Z'))
            index = letter - 'A';
    }
    rect.left = 34 * (unsigned char)index;
    rect.top = 0;
    rect.right = 34 * (unsigned char)index + 33;
    rect.bottom = 37;
    return rect;
}

// A 191 by 37 part of the noise picture at the row, starting 0 to 29 across at random.
TRect CaptchaGraphics::GetNoiseRect(unsigned char row)
{
    TRect rect;
    int shift = random(30);
    rect.left = shift;
    rect.top = row;
    rect.right = shift + 191;
    rect.bottom = row + 37;
    return rect;
}

// Loads the captcha data file and checks its size, sum and mark, then draws the letters and the noise; false when a
// check or a drawing fails.
char CaptchaGraphics::Load(int unused)
{
    int handle;
    int file_length;
    char *buffer;
    String bytes;
    String file_name = MainForm->files->GetDataFilePath(DataFile_Captcha);
    try
    {
        handle = FileOpen(file_name.c_str(), fmOpenRead);
        file_length = FileSeek(handle, 0, 2);
        FileSeek(handle, 0, 0);
        buffer = new char[file_length + 1];
        FileRead(handle, buffer, file_length);
        FileClose(handle);
        bytes = buffer;
        bytes.SetLength(file_length);
        delete[] buffer;
        if (bytes.Length() != 0x1597B)
            return 0;
        int checksum = 0;
        for (int i = 4; i <= bytes.Length(); i++)
        {
            bytes[i] -= i % 70;
            bytes[i] -= 20;
            checksum += bytes[i];
        }
        if (checksum != 0x1326A0)
            return 0;
        if (bytes[43440] != 26)
            return 0;
        data = bytes;
    }
    catch (...)
    {
        return 0;
    }
    if (!RenderLetters(unused))
        return 0;
    if (!RenderNoise(unused))
        return 0;
    return 1;
}

// Draws the noise strip in grey from the captcha data, read backwards; false when the surface cannot be drawn on.
bool CaptchaGraphics::RenderNoise(int unused)
{
    unsigned char shade;
    if (MainForm->gfx_transfers->BeginDraw(noise_surface))
    {
        int pos = 65345;
        for (int y = 0; y < 37; y++)
        {
            for (int x = 0; x < 883; x++)
            {
                shade = data[pos] - 2;
                pos -= 2;
                MainForm->gfx_transfers->PutPixel(x, y, shade, shade, shade);
            }
        }
        MainForm->gfx_transfers->EndDraw(noise_surface);
        noise_surface->SetColorKey(DDCKEY_SRCBLT, color_key);
        return true;
    }
    noise_surface->SetColorKey(DDCKEY_SRCBLT, color_key);
    return false;
}

// Draws the letter sheet in grey from the captcha data, two bytes per pixel.
bool CaptchaGraphics::RenderLetters(int unused)
{
    unsigned char shade;
    if (MainForm->gfx_transfers->BeginDraw(letters_surface))
    {
        int pos = 4;
        for (int y = 0; y < 201; y++)
        {
            for (int x = 0; x < 220; x++)
            {
                shade = data[pos] - 2;
                pos += 2;
                MainForm->gfx_transfers->PutPixel(x, y, shade, shade, shade);
            }
        }
        MainForm->gfx_transfers->EndDraw(letters_surface);
        letters_surface->SetColorKey(DDCKEY_SRCBLT, color_key);
        return true;
    }
    letters_surface->SetColorKey(DDCKEY_SRCBLT, color_key);
    return true;
}
