#include <vcl.h>
#pragma hdrstop

#include "Gfx_alpha.h"
#include <TDx_Draw_Library_Install.H>
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

// Reads the pixel format of a surface: its bit depth, whether a 16-bit format is 565 or 555, and each colour
// mask's shift and width for MakeColor. Other bit depths, or a failed mask read, leave it unsupported.
ExtBlts::ExtBlts(TDx_DrawSurface *surface)
{
    pixel_format = 0;
    is_supported = false;
    TDDPixelFormat *pixel_format_desc = new TDDPixelFormat(NULL);
    surface->GetPixelFormat(pixel_format_desc);
    if (pixel_format_desc->RGBBitCount[0] == 32)
    {
        pixel_format = 32;
        is_supported = true;
    }
    if (pixel_format_desc->RGBBitCount[0] == 24)
    {
        pixel_format = 24;
        is_supported = true;
    }
    if (pixel_format_desc->RGBBitCount[0] == 16)
    {
        pixel_format = 16;
        is_supported = true;
        if (pixel_format_desc->RBitMask[0] == RedMask_Rgb565 && pixel_format_desc->GBitMask[0] == GreenMask_Rgb565 &&
            pixel_format_desc->BBitMask[0] == 31)
            pixel_format = 565;
        if (pixel_format_desc->RBitMask[0] == RedMask_Rgb555 && pixel_format_desc->GBitMask[0] == GreenMask_Rgb555 &&
            pixel_format_desc->BBitMask[0] == 31)
            pixel_format = 555;
    }
    try
    {
        GetMaskShiftAndWidth(pixel_format_desc->RBitMask[0], &red_shift, &red_bits);
        GetMaskShiftAndWidth(pixel_format_desc->GBitMask[0], &green_shift, &green_bits);
        GetMaskShiftAndWidth(pixel_format_desc->BBitMask[0], &blue_shift, &blue_bits);
    }
    catch (...)
    {
        is_supported = false;
    }
}

// Packs 8-bit red, green and blue into one pixel of the display format.
unsigned int ExtBlts::MakeColor(unsigned int red, unsigned int green, unsigned int blue)
{
    unsigned int part1 = red * (1 << red_bits);
    unsigned int part2 = green * (1 << green_bits);
    unsigned int part3 = blue * (1 << blue_bits);
    part1 = part1 >> 8 << red_shift;
    part2 = part2 >> 8 << green_shift;
    part3 = part3 >> 8 << blue_shift;
    return part1 | part2 | part3;
}

// The position of the lowest set bit of a colour mask, and the run of set bits from there.
void ExtBlts::GetMaskShiftAndWidth(int mask, unsigned short *shift, unsigned short *width)
{
    int bit = 1;
    *shift = 0;
    for (; *shift < 32; ++*shift)
    {
        if (mask & bit)
            break;
        bit <<= 1;
    }
    int result = 2 * bit;
    *width = 1;
    for (; *width < 32; ++*width)
    {
        if (!(mask & result))
            break;
        result <<= 1;
    }
}

ExtBlts::~ExtBlts()
{
}

// Blends a rect of a source surface onto a destination surface at an alpha from none to full, pixel by pixel.
int ExtBlts::AlphaBlt(int dest_x, int dest_y, TDx_DrawSurface *dest_surface, TDx_DrawSurface *source_surface,
    Windows::TRect *source_rect, int alpha)
{
    TDDSurfaceDesc *source_desc;
    TDDSurfaceDesc *dest_desc;
    int width;
    int rows_left;
    int dest_skip;
    int source_skip;
    unsigned int source_red;
    unsigned int source_green;
    unsigned int source_blue;
    unsigned int dest_red;
    unsigned int dest_green;
    unsigned int dest_blue;
    unsigned int blended_red;
    unsigned int blended_green;
    int packed_full;
    int packed_alpha;
    unsigned short *source;
    char odd_width;
    int result;
    int columns_left;
    TRect dest_rect;
    unsigned short *dest;
    unsigned int source_pixel;
    unsigned int dest_pixel;
    unsigned int blended_blue;
    source_desc = new TDDSurfaceDesc(NULL);
    dest_desc = new TDDSurfaceDesc(NULL);
    result = 0;
    if (alpha < 0)
        alpha = 0;
    if (alpha > 256)
        alpha = 256;
    width = source_rect->right - source_rect->left;
    rows_left = source_rect->bottom - source_rect->top;
    dest_rect.left = dest_x;
    dest_rect.top = dest_y;
    dest_rect.right = dest_x + width;
    dest_rect.bottom = dest_y + rows_left;
    source_surface->GetSurfaceDesc(source_desc);
    dest_surface->GetSurfaceDesc(dest_desc);
    dest_surface->Lock(&dest_rect, dest_desc, 1);
    source_surface->Lock(source_rect, source_desc, 1);
    try
    {
        if (pixel_format == 555)
        {
            dest_skip = dest_desc->Pitch - 2 * width;
            source_skip = source_desc->Pitch - 2 * width;
            if ((width & 1) != 0)
            {
                odd_width = 1;
                width = (width - 1) / 2;
            }
            else
            {
                odd_width = 0;
                width /= 2;
            }
            packed_full = 4194368;
            packed_alpha = alpha / 4 | alpha / 4 << 16;
            dest = (unsigned short *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            source = (unsigned short *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    source_pixel = *(int *)source;
                    if (source_pixel)
                    {
                        dest_pixel = *(int *)dest;
                        dest_red = dest_pixel >> 10 & 0x1F001F;
                        source_red = source_pixel >> 10 & 0x1F001F;
                        dest_green = dest_pixel >> 5 & 0x1F001F;
                        source_green = source_pixel >> 5 & 0x1F001F;
                        dest_blue = dest_pixel & 0x1F001F;
                        source_blue = source_pixel & 0x1F001F;
                        blended_red = (dest_red + (alpha * (source_red + packed_full - dest_red) >> 8) - packed_alpha &
                            2031647) << 10;
                        blended_green = 32 *
                            (dest_green + (alpha * (source_green + packed_full - dest_green) >> 8) - packed_alpha &
                            2031647);
                        blended_blue = dest_blue + (alpha * (source_blue + packed_full - dest_blue) >> 8) -
                            packed_alpha & 0x1F001F;
                        if (!(source_pixel >> 16))
                            *(int *)dest = (blended_red | blended_green | blended_blue) & 0xFFFF |
                                dest_pixel & 0xFFFF0000;
                        else
                        {
                            if (!(source_pixel & 0xFFFF))
                                *(int *)dest = (blended_red | blended_green | blended_blue) & 0xFFFF0000 |
                                    dest_pixel & 0xFFFF;
                            else
                                *(int *)dest = blended_red | blended_green | blended_blue;
                        }
                    }
                    dest += 2;
                    source += 2;
                }
                if (odd_width)
                {
                    source_pixel = *source;
                    if (source_pixel)
                    {
                        dest_pixel = *dest;
                        dest_red = dest_pixel >> 10 & 0x1F;
                        source_red = source_pixel >> 10 & 0x1F;
                        dest_green = dest_pixel >> 5 & 0x1F;
                        source_green = source_pixel >> 5 & 0x1F;
                        dest_blue = dest_pixel & 0x1F;
                        source_blue = source_pixel & 0x1F;
                        *dest = dest_red + (alpha * (source_red - dest_red) >> 8) << 10 |
                            32 * (dest_green + (alpha * (source_green - dest_green) >> 8)) |
                            dest_blue + (alpha * (source_blue - dest_blue) >> 8);
                    }
                    dest++;
                    source++;
                }
                dest = (unsigned short *)((char *)dest + dest_skip);
                source = (unsigned short *)((char *)source + source_skip);
            } while (--rows_left > 0);
        }
        if (pixel_format == 565)
        {
            dest_skip = dest_desc->Pitch - 2 * width;
            source_skip = source_desc->Pitch - 2 * width;
            if ((width & 1) != 0)
            {
                odd_width = 1;
                width = (width - 1) / 2;
            }
            else
            {
                odd_width = 0;
                width /= 2;
            }
            packed_full = 4194368;
            packed_alpha = alpha / 4 | alpha / 4 << 16;
            dest = (unsigned short *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            source = (unsigned short *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    source_pixel = *(int *)source;
                    if (source_pixel)
                    {
                        dest_pixel = *(int *)dest;
                        dest_red = dest_pixel >> 11 & 0x1F001F;
                        source_red = source_pixel >> 11 & 0x1F001F;
                        dest_green = dest_pixel >> 5 & 0x3F003F;
                        source_green = source_pixel >> 5 & 0x3F003F;
                        dest_blue = dest_pixel & 0x1F001F;
                        source_blue = source_pixel & 0x1F001F;
                        blended_red = (dest_red + (alpha * (source_red + packed_full - dest_red) >> 8) - packed_alpha &
                            2031647) << 11;
                        blended_green = 32 *
                            (dest_green + (alpha * (source_green + packed_full - dest_green) >> 8) - packed_alpha &
                            4128831);
                        blended_blue = dest_blue + (alpha * (source_blue + packed_full - dest_blue) >> 8) -
                            packed_alpha & 0x1F001F;
                        if (!(source_pixel >> 16))
                            *(int *)dest = (blended_red | blended_green | blended_blue) & 0xFFFF |
                                dest_pixel & 0xFFFF0000;
                        else
                        {
                            if (!(source_pixel & 0xFFFF))
                                *(int *)dest = (blended_red | blended_green | blended_blue) & 0xFFFF0000 |
                                    dest_pixel & 0xFFFF;
                            else
                                *(int *)dest = blended_red | blended_green | blended_blue;
                        }
                    }
                    dest += 2;
                    source += 2;
                }
                if (odd_width)
                {
                    source_pixel = *source;
                    if (source_pixel)
                    {
                        dest_pixel = *dest;
                        dest_red = dest_pixel >> 11 & 0x1F;
                        source_red = source_pixel >> 11 & 0x1F;
                        dest_green = dest_pixel >> 5 & 0x3F;
                        source_green = source_pixel >> 5 & 0x3F;
                        dest_blue = dest_pixel & 0x1F;
                        source_blue = source_pixel & 0x1F;
                        *dest = dest_red + (alpha * (source_red - dest_red) >> 8) << 11 |
                            32 * (dest_green + (alpha * (source_green - dest_green) >> 8)) |
                            dest_blue + (alpha * (source_blue - dest_blue) >> 8);
                    }
                    dest++;
                    source++;
                }
                dest = (unsigned short *)((char *)dest + dest_skip);
                source = (unsigned short *)((char *)source + source_skip);
            } while (--rows_left > 0);
        }
        if (pixel_format == 16)
        {
            dest_skip = dest_desc->Pitch - 2 * width;
            source_skip = source_desc->Pitch - 2 * width;
            dest = (unsigned short *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            source = (unsigned short *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    source_pixel = *source;
                    if (source_pixel)
                    {
                        dest_pixel = *dest;
                        dest_red = dest_pixel & dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwRBitMask;
                        source_red = source_pixel &
                            source_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwRBitMask;
                        dest_green = dest_pixel & dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwGBitMask;
                        source_green = source_pixel &
                            source_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwGBitMask;
                        dest_blue = dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwBBitMask & dest_pixel;
                        source_blue = source_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwBBitMask &
                            source_pixel;
                        blended_red = dest_red + (alpha * (source_red - dest_red) >> 8) &
                            dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwRBitMask;
                        blended_green = dest_green + (alpha * (source_green - dest_green) >> 8) &
                            dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwGBitMask;
                        blended_blue = dest_blue + (alpha * (source_blue - dest_blue) >> 8) &
                            dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwBBitMask;
                        *dest = blended_red | blended_green | blended_blue;
                    }
                    dest++;
                    source++;
                }
                dest = (unsigned short *)((char *)dest + dest_skip);
                source = (unsigned short *)((char *)source + source_skip);
            } while (--rows_left > 0);
        }
        if (pixel_format == 24)
        {
            dest_skip = dest_desc->Pitch - 3 * width;
            source_skip = source_desc->Pitch - 3 * width;
            dest = (unsigned short *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            source = (unsigned short *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    source_pixel = *(int *)source;
                    if ((source_pixel & 0xFFFFFF) != 0)
                    {
                        dest_pixel = *(int *)dest;
                        dest_red = dest_pixel >> 16 & 0xFF;
                        source_red = source_pixel >> 16 & 0xFF;
                        dest_green = dest_pixel >> 8 & 0xFF;
                        source_green = source_pixel >> 8 & 0xFF;
                        dest_blue = dest_pixel & 0xFF;
                        source_blue = source_pixel & 0xFF;
                        dest_pixel = dest_red + (alpha * (source_red - dest_red) >> 8) << 16 |
                            dest_green + (alpha * (source_green - dest_green) >> 8) << 8 |
                            dest_blue + (alpha * (source_blue - dest_blue) >> 8);
                        *dest = dest_pixel & 0xFFFF;
                        dest = (unsigned short *)((unsigned char *)dest + 2);
                        *(unsigned char *)dest = dest_pixel >> 16;
                        dest = (unsigned short *)((unsigned char *)dest + 1);
                    }
                    else
                        dest = (unsigned short *)((char *)dest + 3);
                    source = (unsigned short *)((char *)source + 3);
                }
                dest = (unsigned short *)((char *)dest + dest_skip);
                source = (unsigned short *)((char *)source + source_skip);
            } while (--rows_left > 0);
        }
        if (pixel_format == 32)
        {
            dest_skip = dest_desc->Pitch - 4 * width;
            source_skip = source_desc->Pitch - 4 * width;
            dest = (unsigned short *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            source = (unsigned short *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    source_pixel = *(int *)source;
                    if ((source_pixel & 0xFFFFFF) != 0)
                    {
                        dest_pixel = *(unsigned int *)dest;
                        dest_red = dest_pixel & 0xFF0000;
                        source_red = source_pixel & 0xFF0000;
                        dest_green = dest_pixel & 0xFF00;
                        source_green = source_pixel & 0xFF00;
                        dest_blue = dest_pixel & 0xFF;
                        source_blue = source_pixel & 0xFF;
                        dest_pixel = dest_red + (alpha * (source_red - dest_red) >> 8) & 0xFF0000 |
                            dest_green + (alpha * (source_green - dest_green) >> 8) & 0xFF00 |
                            dest_blue + (alpha * (source_blue - dest_blue) >> 8);
                        *(unsigned int *)dest = dest_pixel;
                    }
                    dest += 2;
                    source += 2;
                }
                dest = (unsigned short *)((char *)dest + dest_skip);
                source = (unsigned short *)((char *)source + source_skip);
            } while (--rows_left > 0);
        }
        dest_surface->Unlock(&dest_rect);
        source_surface->Unlock(source_rect);
    }
    catch (...)
    {
        dest_surface->Unlock(&dest_rect);
        source_surface->Unlock(source_rect);
    }
    source_desc->Clear();
    dest_desc->Clear();
    delete source_desc;
    delete dest_desc;
    return result;
}

// Averages a rect of a source surface with the destination surface, pixel by pixel.
int ExtBlts::HalfBlendBlt(int dest_x, int dest_y, TDx_DrawSurface *dest_surface, TDx_DrawSurface *source_surface,
    Windows::TRect *source_rect)
{
    TDDSurfaceDesc *source_desc;
    TDDSurfaceDesc *dest_desc;
    int width;
    int rows_left;
    int dest_skip;
    int source_skip;
    unsigned short half_mask;
    int packed_half_mask;
    char odd_width;
    int result;
    int columns_left;
    TRect dest_rect;
    unsigned int *dest;
    unsigned int *source;
    unsigned int source_pair_555;
    unsigned int dest_pair_555;
    source_desc = new TDDSurfaceDesc(NULL);
    dest_desc = new TDDSurfaceDesc(NULL);
    result = 0;
    width = source_rect->right - source_rect->left;
    rows_left = source_rect->bottom - source_rect->top;
    dest_rect.left = dest_x;
    dest_rect.top = dest_y;
    dest_rect.right = width + dest_x;
    dest_rect.bottom = rows_left + dest_y;
    dest_surface->Lock(&dest_rect, dest_desc, 1);
    source_surface->Lock(source_rect, source_desc, 1);
    try
    {
        if (pixel_format == 555)
        {
            dest_skip = dest_desc->Pitch - 2 * width;
            source_skip = source_desc->Pitch - 2 * width;
            if ((width & 1) != 0)
            {
                odd_width = 1;
                width = (width - 1) / 2;
            }
            else
            {
                odd_width = 0;
                width /= 2;
            }
            dest = (unsigned int *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            source = (unsigned int *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    source_pair_555 = *source;
                    if (source_pair_555)
                    {
                        dest_pair_555 = *dest;
                        if (!(source_pair_555 >> 16))
                            source_pair_555 |= dest_pair_555 & 0xFFFF0000;
                        if (!(source_pair_555 & 0xFFFF))
                            source_pair_555 |= dest_pair_555 & 0xFFFF;
                        *dest = ((dest_pair_555 & 0x7BDE7BDE) >> 1) + ((source_pair_555 & 0x7BDE7BDE) >> 1);
                    }
                    dest++;
                    source++;
                }
                if (odd_width)
                {
                    source_pair_555 = *(unsigned short *)source;
                    if (source_pair_555)
                    {
                        dest_pair_555 = *(unsigned short *)dest;
                        *(unsigned short *)dest = ((dest_pair_555 & 0x7BDE) >> 1) + ((source_pair_555 & 0x7BDE) >> 1);
                    }
                    dest = (unsigned int *)((char *)dest + 2);
                    source = (unsigned int *)((char *)source + 2);
                }
                dest = (unsigned int *)((char *)dest + dest_skip);
                source = (unsigned int *)((char *)source + source_skip);
            } while (--rows_left > 0);
        }
        if (pixel_format == 565)
        {
            dest_skip = dest_desc->Pitch - 2 * width;
            source_skip = source_desc->Pitch - 2 * width;
            if ((width & 1) != 0)
            {
                odd_width = 1;
                width = (width - 1) / 2;
            }
            else
            {
                odd_width = 0;
                width /= 2;
            }
            dest = (unsigned int *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            source = (unsigned int *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    source_pair_555 = *source;
                    if (source_pair_555)
                    {
                        dest_pair_555 = *dest;
                        if (!(source_pair_555 >> 16))
                            source_pair_555 |= dest_pair_555 & 0xFFFF0000;
                        if (!(source_pair_555 & 0xFFFF))
                            source_pair_555 |= dest_pair_555 & 0xFFFF;
                        *dest = ((dest_pair_555 & 0xF7DEF7DE) >> 1) + ((source_pair_555 & 0xF7DEF7DE) >> 1);
                    }
                    dest++;
                    source++;
                }
                if (odd_width)
                {
                    source_pair_555 = *(unsigned short *)source;
                    if (source_pair_555)
                    {
                        dest_pair_555 = *(unsigned short *)dest;
                        *(unsigned short *)dest = ((dest_pair_555 & 0xF7DE) >> 1) + ((source_pair_555 & 0xF7DE) >> 1);
                    }
                    dest = (unsigned int *)((char *)dest + 2);
                    source = (unsigned int *)((char *)source + 2);
                }
                dest = (unsigned int *)((char *)dest + dest_skip);
                source = (unsigned int *)((char *)source + source_skip);
            } while (--rows_left > 0);
        }
        if (pixel_format == 16)
        {
            dest_skip = dest_desc->Pitch - 2 * width;
            source_skip = source_desc->Pitch - 2 * width;
            if ((width & 1) != 0)
            {
                odd_width = 1;
                width = (width - 1) / 2;
            }
            else
            {
                odd_width = 0;
                width /= 2;
            }
            half_mask = (unsigned short)dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwRBitMask &
                2 * (unsigned short)dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwRBitMask |
                (unsigned short)dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwGBitMask &
                2 * (unsigned short)dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwGBitMask |
                (unsigned short)dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwBBitMask &
                2 * (unsigned short)dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwBBitMask;
            packed_half_mask = half_mask | half_mask << 16;
            dest = (unsigned int *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            source = (unsigned int *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    source_pair_555 = *source;
                    if (source_pair_555)
                    {
                        dest_pair_555 = *dest;
                        if (!(source_pair_555 >> 16))
                            source_pair_555 |= dest_pair_555 & 0xFFFF0000;
                        if (!(source_pair_555 & 0xFFFF))
                            source_pair_555 |= dest_pair_555 & 0xFFFF;
                        *dest = ((packed_half_mask & dest_pair_555) >> 1) + ((packed_half_mask & source_pair_555) >> 1);
                    }
                    dest++;
                    source++;
                }
                if (odd_width)
                {
                    source_pair_555 = *(unsigned short *)source;
                    if (source_pair_555)
                    {
                        dest_pair_555 = *(unsigned short *)dest;
                        *(unsigned short *)dest = ((half_mask & dest_pair_555) >> 1) +
                            ((half_mask & source_pair_555) >> 1);
                    }
                    dest = (unsigned int *)((char *)dest + 2);
                    source = (unsigned int *)((char *)source + 2);
                }
                dest = (unsigned int *)((char *)dest + dest_skip);
                source = (unsigned int *)((char *)source + source_skip);
            } while (--rows_left > 0);
        }
        if (pixel_format == 24)
        {
            dest_skip = dest_desc->Pitch - 3 * width;
            source_skip = source_desc->Pitch - 3 * width;
            dest = (unsigned int *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            source = (unsigned int *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    source_pair_555 = *source;
                    if ((source_pair_555 & 0xFFFFFF) != 0)
                    {
                        dest_pair_555 = ((*dest & 0xFEFEFE) >> 1) + ((source_pair_555 & 0xFEFEFE) >> 1);
                        *(unsigned short *)dest = dest_pair_555;
                        dest = (unsigned int *)((unsigned char *)dest + 2);
                        *(unsigned char *)dest = dest_pair_555 >> 16;
                        dest = (unsigned int *)((unsigned char *)dest + 1);
                    }
                    else
                        dest = (unsigned int *)((char *)dest + 3);
                    source = (unsigned int *)((char *)source + 3);
                }
                dest = (unsigned int *)((char *)dest + dest_skip);
                source = (unsigned int *)((char *)source + source_skip);
            } while (--rows_left > 0);
        }
        if (pixel_format == 32)
        {
            dest_skip = dest_desc->Pitch - 4 * width;
            source_skip = source_desc->Pitch - 4 * width;
            dest = (unsigned int *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            source = (unsigned int *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    source_pair_555 = *source;
                    if ((source_pair_555 & 0xFFFFFF) != 0)
                    {
                        dest_pair_555 = *dest;
                        *dest = ((dest_pair_555 & 0xFEFEFE) >> 1) + ((source_pair_555 & 0xFEFEFE) >> 1);
                    }
                    dest++;
                    source++;
                }
                dest = (unsigned int *)((char *)dest + dest_skip);
                source = (unsigned int *)((char *)source + source_skip);
            } while (--rows_left > 0);
        }
        dest_surface->Unlock(&dest_rect);
        source_surface->Unlock(source_rect);
    }
    catch (...)
    {
        dest_surface->Unlock(&dest_rect);
        source_surface->Unlock(source_rect);
    }
    source_desc->Clear();
    dest_desc->Clear();
    delete source_desc;
    delete dest_desc;
    return result;
}
