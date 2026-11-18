#include <vcl.h>
#pragma hdrstop

#include "Gfx_alpha.h"
#include <TDDPixelFormat.H>
#include <TDDSurfaceDesc.H>

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
int ExtBlts::MakeColor(int red, int green, int blue)
{
    unsigned int red_part = (1 << red_bits) * red;
    unsigned int green_part = (1 << green_bits) * green;
    unsigned int blue_part = (1 << blue_bits) * blue;
    red_part = (red_part >> 8) << red_shift;
    green_part = (green_part >> 8) << green_shift;
    blue_part = (blue_part >> 8) << blue_shift;
    return red_part | green_part | blue_part;
}

// The position of the lowest set bit of a colour mask, and the run of set bits from there.
int ExtBlts::GetMaskShiftAndWidth(int mask, unsigned short *shift, unsigned short *width)
{
    int bit = 1;
    *shift = 0;
    while (*shift < 32)
    {
        if ((mask & bit) != 0)
            break;
        bit = 2 * bit;
        (*shift)++;
    }
    bit = 2 * bit;
    *width = 1;
    while (*width < 32)
    {
        if ((mask & bit) == 0)
            break;
        bit = 2 * bit;
        (*width)++;
    }
    return bit;
}

ExtBlts::~ExtBlts()
{
}

// Blends a rect of a source surface onto a destination surface at an alpha from none to full, pixel by pixel.
int ExtBlts::AlphaBlt(int dest_x, int dest_y, TDx_DrawSurface *dest_surface, TDx_DrawSurface *source_surface,
    TRect *source_rect, int alpha)
{
    unsigned int source_pair;
    unsigned int dest_pair;
    char *source_pixel;
    unsigned int blended_blue;
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
    char *source;
    char odd_width;
    int result;
    int columns_left;
    TRect dest_rect;
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
    dest_surface->Lock(&dest_rect, dest_desc, DDLOCK_WAIT);
    source_surface->Lock(source_rect, source_desc, DDLOCK_WAIT);
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
            packed_full = 0x400040;
            packed_alpha = alpha / 4 | (alpha / 4 << 16);
            source_pixel = (char *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            source = (char *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    source_pair = *(int *)source;
                    if (source_pair)
                    {
                        dest_pair = *(int *)source_pixel;
                        dest_red = (dest_pair >> 10) & 0x1F001F;
                        source_red = (source_pair >> 10) & 0x1F001F;
                        dest_green = (dest_pair >> 5) & 0x1F001F;
                        source_green = (source_pair >> 5) & 0x1F001F;
                        dest_blue = dest_pair & 0x1F001F;
                        source_blue = source_pair & 0x1F001F;
                        blended_red =
                            (((unsigned int)((source_red + packed_full - dest_red) * alpha) >> 8) + dest_red -
                                packed_alpha & 0x1F001F) << 10;
                        blended_green =
                            (((unsigned int)((source_green + packed_full - dest_green) * alpha) >> 8) + dest_green -
                                packed_alpha & 0x1F001F) << 5;
                        blended_blue =
                            ((unsigned int)((source_blue + packed_full - dest_blue) * alpha) >> 8) + dest_blue -
                                packed_alpha & 0x1F001F;
                        if ((source_pair >> 16) == 0)
                            *(int *)source_pixel =
                                ((blended_blue | (blended_red | blended_green)) & 0xFFFF) | (dest_pair & -65536);
                        else if ((source_pair & 0xFFFF) == 0)
                            *(int *)source_pixel =
                                ((blended_blue | (blended_red | blended_green)) & -65536) | (dest_pair & 0xFFFF);
                        else
                            *(int *)source_pixel = blended_blue | (blended_red | blended_green);
                    }
                    source_pixel = source_pixel + 4;
                    source += 4;
                }
                if (odd_width)
                {
                    source_pair = *(unsigned short *)source;
                    if (source_pair)
                    {
                        dest_pair = *(unsigned short *)source_pixel;
                        dest_red = (dest_pair >> 10) & 0x1F;
                        source_red = (source_pair >> 10) & 0x1F;
                        dest_green = (dest_pair >> 5) & 0x1F;
                        source_green = (source_pair >> 5) & 0x1F;
                        dest_blue = dest_pair & 0x1F;
                        source_blue = source_pair & 0x1F;
                        *(short *)source_pixel =
                            (short)(((short)((unsigned int)((source_red - dest_red) * alpha) >> 8) + dest_red) << 10) |
                                (short)(((short)((unsigned int)((source_green - dest_green) * alpha) >> 8) +
                                    dest_green) << 5) |
                                (short)((unsigned int)((source_blue - dest_blue) * alpha) >> 8) + dest_blue;
                    }
                    source_pixel = source_pixel + 2;
                    source += 2;
                }
                source_pixel = source_pixel + dest_skip;
                source += source_skip;
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
            packed_full = 0x400040;
            packed_alpha = alpha / 4 | (alpha / 4 << 16);
            source_pixel = (char *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            source = (char *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    source_pair = *(int *)source;
                    if (source_pair)
                    {
                        dest_pair = *(int *)source_pixel;
                        dest_red = (dest_pair >> 11) & 0x1F001F;
                        source_red = (source_pair >> 11) & 0x1F001F;
                        dest_green = (dest_pair >> 5) & 0x3F003F;
                        source_green = (source_pair >> 5) & 0x3F003F;
                        dest_blue = dest_pair & 0x1F001F;
                        source_blue = source_pair & 0x1F001F;
                        blended_red =
                            (((unsigned int)((source_red + packed_full - dest_red) * alpha) >> 8) + dest_red -
                                packed_alpha & 0x1F001F) << 11;
                        blended_green =
                            (((unsigned int)((source_green + packed_full - dest_green) * alpha) >> 8) + dest_green -
                                packed_alpha & 0x3F003F) << 5;
                        blended_blue =
                            ((unsigned int)((source_blue + packed_full - dest_blue) * alpha) >> 8) + dest_blue -
                                packed_alpha & 0x1F001F;
                        if ((source_pair >> 16) == 0)
                            *(int *)source_pixel =
                                ((blended_blue | (blended_red | blended_green)) & 0xFFFF) | (dest_pair & -65536);
                        else if ((source_pair & 0xFFFF) == 0)
                            *(int *)source_pixel =
                                ((blended_blue | (blended_red | blended_green)) & -65536) | (dest_pair & 0xFFFF);
                        else
                            *(int *)source_pixel = blended_blue | (blended_red | blended_green);
                    }
                    source_pixel = source_pixel + 4;
                    source += 4;
                }
                if (odd_width)
                {
                    source_pair = *(unsigned short *)source;
                    if (source_pair)
                    {
                        dest_pair = *(unsigned short *)source_pixel;
                        dest_red = (dest_pair >> 11) & 0x1F;
                        source_red = (source_pair >> 11) & 0x1F;
                        dest_green = (dest_pair >> 5) & 0x3F;
                        source_green = (source_pair >> 5) & 0x3F;
                        dest_blue = dest_pair & 0x1F;
                        source_blue = source_pair & 0x1F;
                        *(short *)source_pixel =
                            (short)(((short)((unsigned int)((source_red - dest_red) * alpha) >> 8) + dest_red) << 11) |
                                (short)(((short)((unsigned int)((source_green - dest_green) * alpha) >> 8) +
                                    dest_green) << 5) |
                                (short)((unsigned int)((source_blue - dest_blue) * alpha) >> 8) + dest_blue;
                    }
                    source_pixel = source_pixel + 2;
                    source += 2;
                }
                source_pixel = source_pixel + dest_skip;
                source += source_skip;
            } while (--rows_left > 0);
        }
        if (pixel_format == 16)
        {
            dest_skip = dest_desc->Pitch - 2 * width;
            source_skip = source_desc->Pitch - 2 * width;
            source_pixel = (char *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            source = (char *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    source_pair = *(unsigned short *)source;
                    if (source_pair)
                    {
                        dest_pair = *(unsigned short *)source_pixel;
                        dest_red = dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwRBitMask & dest_pair;
                        source_red = source_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwRBitMask & source_pair;
                        dest_green = dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwGBitMask & dest_pair;
                        source_green =
                            source_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwGBitMask & source_pair;
                        dest_blue = dest_pair & dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwBBitMask;
                        source_blue =
                            source_pair & source_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwBBitMask;
                        blended_red =
                            dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwRBitMask &
                                ((unsigned int)((source_red - dest_red) * alpha) >> 8) + dest_red;
                        blended_green =
                            dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwGBitMask &
                                ((unsigned int)((source_green - dest_green) * alpha) >> 8) + dest_green;
                        blended_blue =
                            dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwBBitMask &
                                ((unsigned int)((source_blue - dest_blue) * alpha) >> 8) + dest_blue;
                        *(short *)source_pixel = blended_red | blended_green | (short)blended_blue;
                    }
                    source_pixel = source_pixel + 2;
                    source += 2;
                }
                source_pixel = source_pixel + dest_skip;
                source += source_skip;
            } while (--rows_left > 0);
        }
        if (pixel_format == 24)
        {
            dest_skip = dest_desc->Pitch - 3 * width;
            source_skip = source_desc->Pitch - 3 * width;
            source_pixel = (char *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            source = (char *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    source_pair = *(int *)source;
                    if ((source_pair & 0xFFFFFF) != 0)
                    {
                        dest_pair = *(int *)source_pixel;
                        dest_red = (dest_pair >> 16) & 0xFF;
                        source_red = (source_pair >> 16) & 0xFF;
                        dest_green = (dest_pair >> 8) & 0xFF;
                        source_green = (source_pair >> 8) & 0xFF;
                        dest_blue = dest_pair & 0xFF;
                        source_blue = source_pair & 0xFF;
                        dest_pair =
                            ((((unsigned int)((source_red - dest_red) * alpha) >> 8) + dest_red) << 16) |
                                ((((unsigned int)((source_green - dest_green) * alpha) >> 8) + dest_green) << 8) |
                                ((unsigned int)((source_blue - dest_blue) * alpha) >> 8) + dest_blue;
                        *(unsigned short *)source_pixel = (unsigned short)dest_pair & 0xFFFF;
                        source_pixel = source_pixel + 2;
                        *source_pixel = dest_pair >> 16;
                        source_pixel++;
                    }
                    else
                        source_pixel = source_pixel + 3;
                    source += 3;
                }
                source_pixel = source_pixel + dest_skip;
                source += source_skip;
            } while (--rows_left > 0);
        }
        if (pixel_format == 32)
        {
            dest_skip = dest_desc->Pitch - 4 * width;
            source_skip = source_desc->Pitch - 4 * width;
            source_pixel = (char *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            source = (char *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    source_pair = *(int *)source;
                    if ((source_pair & 0xFFFFFF) != 0)
                    {
                        dest_pair = *(int *)source_pixel;
                        dest_red = dest_pair & 0xFF0000;
                        source_red = source_pair & 0xFF0000;
                        dest_green = dest_pair & 0xFF00;
                        source_green = source_pair & 0xFF00;
                        dest_blue = dest_pair & 0xFF;
                        source_blue = source_pair & 0xFF;
                        dest_pair =
                            (((unsigned int)((source_red - dest_red) * alpha) >> 8) + dest_red & 0xFF0000) |
                                (((unsigned int)((source_green - dest_green) * alpha) >> 8) + dest_green & 0xFF00) |
                                ((unsigned int)((source_blue - dest_blue) * alpha) >> 8) + dest_blue;
                        *(int *)source_pixel = dest_pair;
                    }
                    source_pixel = source_pixel + 4;
                    source += 4;
                }
                source_pixel = source_pixel + dest_skip;
                source += source_skip;
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
    TRect *source_rect)
{
    char *dest_words;
    char *rgb555_source;
    unsigned int rgb555_source_pair;
    unsigned int rgb555_dest_pair;
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
    source_desc = new TDDSurfaceDesc(NULL);
    dest_desc = new TDDSurfaceDesc(NULL);
    result = 0;
    width = source_rect->right - source_rect->left;
    rows_left = source_rect->bottom - source_rect->top;
    dest_rect.left = dest_x;
    dest_rect.top = dest_y;
    dest_rect.right = dest_x + width;
    dest_rect.bottom = dest_y + rows_left;
    dest_surface->Lock(&dest_rect, dest_desc, DDLOCK_WAIT);
    source_surface->Lock(source_rect, source_desc, DDLOCK_WAIT);
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
            dest_words = (char *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            rgb555_source = (char *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    rgb555_source_pair = *(int *)rgb555_source;
                    if (rgb555_source_pair)
                    {
                        rgb555_dest_pair = *(int *)dest_words;
                        if ((rgb555_source_pair >> 16) == 0)
                            rgb555_source_pair = rgb555_source_pair | (rgb555_dest_pair & -65536);
                        if ((rgb555_source_pair & 0xFFFF) == 0)
                            rgb555_source_pair = rgb555_source_pair | (rgb555_dest_pair & 0xFFFF);
                        *(int *)dest_words =
                            ((unsigned int)(rgb555_dest_pair & 0x7BDE7BDE) >> 1) +
                                ((unsigned int)(rgb555_source_pair & 0x7BDE7BDE) >> 1);
                    }
                    dest_words = dest_words + 4;
                    rgb555_source = rgb555_source + 4;
                }
                if (odd_width)
                {
                    rgb555_source_pair = *(unsigned short *)rgb555_source;
                    if (rgb555_source_pair)
                    {
                        rgb555_dest_pair = *(unsigned short *)dest_words;
                        *(short *)dest_words =
                            (short)((unsigned int)(rgb555_dest_pair & 0x7BDE) >> 1) +
                                (short)((unsigned int)(rgb555_source_pair & 0x7BDE) >> 1);
                    }
                    dest_words = dest_words + 2;
                    rgb555_source = rgb555_source + 2;
                }
                dest_words = dest_words + dest_skip;
                rgb555_source = rgb555_source + source_skip;
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
            dest_words = (char *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            rgb555_source = (char *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    rgb555_source_pair = *(int *)rgb555_source;
                    if (rgb555_source_pair)
                    {
                        rgb555_dest_pair = *(int *)dest_words;
                        if ((rgb555_source_pair >> 16) == 0)
                            rgb555_source_pair = rgb555_source_pair | (rgb555_dest_pair & -65536);
                        if ((rgb555_source_pair & 0xFFFF) == 0)
                            rgb555_source_pair = rgb555_source_pair | (rgb555_dest_pair & 0xFFFF);
                        *(int *)dest_words =
                            ((unsigned int)(rgb555_dest_pair & -136382498) >> 1) +
                                ((unsigned int)(rgb555_source_pair & -136382498) >> 1);
                    }
                    dest_words = dest_words + 4;
                    rgb555_source = rgb555_source + 4;
                }
                if (odd_width)
                {
                    rgb555_source_pair = *(unsigned short *)rgb555_source;
                    if (rgb555_source_pair)
                    {
                        rgb555_dest_pair = *(unsigned short *)dest_words;
                        *(short *)dest_words =
                            (short)((unsigned int)(rgb555_dest_pair & 0xF7DE) >> 1) +
                                (short)((unsigned int)(rgb555_source_pair & 0xF7DE) >> 1);
                    }
                    dest_words = dest_words + 2;
                    rgb555_source = rgb555_source + 2;
                }
                dest_words = dest_words + dest_skip;
                rgb555_source = rgb555_source + source_skip;
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
            half_mask =
                ((short)dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwRBitMask &
                    2 * (short)dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwRBitMask) |
                    ((short)dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwGBitMask &
                        2 * (short)dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwGBitMask) |
                    ((short)dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwBBitMask &
                        2 * (short)dest_desc->Internal_DDSURFACEDESC2_Ptr->ddpfPixelFormat.dwBBitMask);
            packed_half_mask = half_mask | (half_mask << 16);
            dest_words = (char *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            rgb555_source = (char *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    rgb555_source_pair = *(int *)rgb555_source;
                    if (rgb555_source_pair)
                    {
                        rgb555_dest_pair = *(int *)dest_words;
                        if ((rgb555_source_pair >> 16) == 0)
                            rgb555_source_pair = rgb555_source_pair | (rgb555_dest_pair & -65536);
                        if ((rgb555_source_pair & 0xFFFF) == 0)
                            rgb555_source_pair = rgb555_source_pair | (rgb555_dest_pair & 0xFFFF);
                        *(int *)dest_words =
                            ((unsigned int)(rgb555_dest_pair & packed_half_mask) >> 1) +
                                ((unsigned int)(rgb555_source_pair & packed_half_mask) >> 1);
                    }
                    dest_words = dest_words + 4;
                    rgb555_source = rgb555_source + 4;
                }
                if (odd_width)
                {
                    rgb555_source_pair = *(unsigned short *)rgb555_source;
                    if (rgb555_source_pair)
                    {
                        rgb555_dest_pair = *(unsigned short *)dest_words;
                        *(short *)dest_words =
                            (short)((unsigned int)(rgb555_dest_pair & half_mask) >> 1) +
                                (short)((unsigned int)(rgb555_source_pair & half_mask) >> 1);
                    }
                    dest_words = dest_words + 2;
                    rgb555_source = rgb555_source + 2;
                }
                dest_words = dest_words + dest_skip;
                rgb555_source = rgb555_source + source_skip;
            } while (--rows_left > 0);
        }
        if (pixel_format == 24)
        {
            dest_skip = dest_desc->Pitch - 3 * width;
            source_skip = source_desc->Pitch - 3 * width;
            dest_words = (char *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            rgb555_source = (char *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    rgb555_source_pair = *(int *)rgb555_source;
                    if ((rgb555_source_pair & 0xFFFFFF) != 0)
                    {
                        rgb555_dest_pair =
                            ((unsigned int)(*(unsigned int *)(char *)dest_words & 0xFEFEFE) >> 1) +
                                ((unsigned int)(rgb555_source_pair & 0xFEFEFE) >> 1);
                        *(short *)dest_words = rgb555_dest_pair;
                        dest_words = dest_words + 2;
                        *dest_words = rgb555_dest_pair >> 16;
                        dest_words++;
                    }
                    else
                        dest_words = dest_words + 3;
                    rgb555_source = rgb555_source + 3;
                }
                dest_words = dest_words + dest_skip;
                rgb555_source = rgb555_source + source_skip;
            } while (--rows_left > 0);
        }
        if (pixel_format == 32)
        {
            dest_skip = dest_desc->Pitch - 4 * width;
            source_skip = source_desc->Pitch - 4 * width;
            dest_words = (char *)dest_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            rgb555_source = (char *)source_desc->Internal_DDSURFACEDESC2_Ptr->lpSurface;
            do
            {
                columns_left = width;
                while (columns_left-- > 0)
                {
                    rgb555_source_pair = *(int *)rgb555_source;
                    if ((rgb555_source_pair & 0xFFFFFF) != 0)
                    {
                        rgb555_dest_pair = *(int *)dest_words;
                        *(int *)dest_words =
                            ((unsigned int)(rgb555_dest_pair & 0xFEFEFE) >> 1) +
                                ((unsigned int)(rgb555_source_pair & 0xFEFEFE) >> 1);
                    }
                    dest_words = dest_words + 4;
                    rgb555_source = rgb555_source + 4;
                }
                dest_words = dest_words + dest_skip;
                rgb555_source = rgb555_source + source_skip;
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
