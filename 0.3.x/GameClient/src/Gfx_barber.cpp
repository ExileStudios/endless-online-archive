#include <vcl.h>
#pragma hdrstop

#include "Gfx_barber.h"
#include "Tmainform.h"
#include "Gfx_bank.h"
#include "Ctrl_barber.h"
#include "Gfx_global_sprites.h"

#pragma package(smart_init)

// Creates the barber window's surfaces: the window, the strip of hair styles and one style's part.
BarberGraphics::BarberGraphics()
{
    surface = new TDx_DrawSurface(NULL);
    styles_surface = new TDx_DrawSurface(NULL);
    part_surface = new TDx_DrawSurface(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
    fill_fx = new TDDBltFX(NULL);
    fill_fx->FillColor = 0;
    surface_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    surface_desc->Width = 252;
    surface_desc->Height = 108;
    styles_surface->GetSurfaceDesc(surface_desc);
    styles_surface->Create(surface_desc, MainForm->Dx_Draw1);
    styles_surface->SetColorKey(DDCKEY_SRCBLT, color_key);
    surface_desc->Width = 28;
    surface_desc->Height = 54;
    part_surface->GetSurfaceDesc(surface_desc);
    part_surface->Create(surface_desc, MainForm->Dx_Draw1);
    part_surface->SetColorKey(DDCKEY_SRCBLT, color_key);
    styles_rect.top = 0;
    styles_rect.left = 0;
    styles_rect.bottom = 108;
    styles_rect.right = 248;
    background_rect.top = 0;
    background_rect.left = 0;
    background_rect.bottom = 193;
    background_rect.right = 425;
    arrow_glow_rect.top = 0;
    arrow_glow_rect.left = 426;
    arrow_glow_rect.bottom = 91;
    arrow_glow_rect.right = 441;
    previous_arrow_rect.top = 96;
    previous_arrow_rect.left = 426;
    previous_arrow_rect.bottom = 101;
    previous_arrow_rect.right = 431;
    next_arrow_rect.top = 96;
    next_arrow_rect.left = 432;
    next_arrow_rect.bottom = 101;
    next_arrow_rect.right = 436;
}

BarberGraphics::~BarberGraphics()
{
    styles_surface->Destroy();
    part_surface->Destroy();
    surface->Destroy();
    delete styles_surface;
    delete part_surface;
    delete surface;
    surface_desc->Clear();
    surface_caps->Clear();
    delete surface_desc;
    delete surface_caps;
    color_key->Clear();
    delete color_key;
}

// Draws the page's sixteen hair styles on the character's head, by gender, skin and hair colour; nothing new when the
// same page is shown already.
bool BarberGraphics::RenderHairStyles(int gender, int skin, int hair_color, int page)
{
    int male_file;
    int back_resource;
    int female_file;
    int front_resource;
    int head_x;
    TRect source;
    bool ok;
    // Drawn again only when the gender, skin, page or hair colour changed.
    if (gender == shown_gender && shown_skin == skin && shown_page == page && shown_hair_color == hair_color)
        ok = true;
    else
    {
        styles_surface->Blt(&styles_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, fill_fx);
        styles_surface->Blt(&styles_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, fill_fx);
        int style = 16 * page;
        int shown = 0;
        male_file = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MaleHair);
        female_file = MainForm->gfx_vault->OpenLibrary(GfxLibrary_FemaleHair);
        int male_hair_file = male_file;
        int female_hair_file = female_file;
        while (MainForm->animation->barber->style_count > style && shown < 16)
        {
            int column = shown % 8;
            int row_y = 36 * (shown / 8);
            int left_margin = 12;
            int sheet_row = 2 * gender + 7;
            int cell_width = 22;
            int cell_gap = 9;
            source.left = 0;
            source.top = 5;
            source.right = 28;
            source.bottom = 32;
            if (!gender)
                source.bottom += 2;
            back_resource = 40 * style + 4 * hair_color + 101;
            if ((gender || MainForm->gfx_transfers->LoadResourceBitmap(female_hair_file, back_resource)) &&
                (gender != 1 || MainForm->gfx_transfers->LoadResourceBitmap(male_hair_file, back_resource)))
            {
                if (MainForm->gfx_transfers->BeginDraw(part_surface))
                {
                    MainForm->gfx_transfers->Draw(part_surface);
                    MainForm->gfx_transfers->EndDraw(part_surface);
                    styles_surface->BltFast(31 * column + 6, 2 * gender + row_y, part_surface, &source,
                        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                }
                head_x = column * cell_width + left_margin + column * cell_gap;
                styles_surface->BltFast(head_x, sheet_row + row_y, MainForm->gfx->sprites->stand_sheet,
                    &MainForm->gfx->sprites->GetHeadAndNeckRect(gender, skin), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                front_resource = 40 * style + 4 * hair_color + 102;
                if ((gender || MainForm->gfx_transfers->LoadResourceBitmap(female_hair_file, front_resource)) &&
                    (gender != 1 || MainForm->gfx_transfers->LoadResourceBitmap(male_hair_file, front_resource)) &&
                    MainForm->gfx_transfers->BeginDraw(part_surface))
                {
                    MainForm->gfx_transfers->Draw(part_surface);
                    MainForm->gfx_transfers->EndDraw(part_surface);
                    styles_surface->BltFast(31 * column + 6, 2 * gender + row_y, part_surface, &source,
                        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                    shown++;
                    if (shown >= 16)
                        break;
                }
            }
            style++;
        }
        shown_gender = gender;
        shown_skin = skin;
        ok = true;
        shown_hair_color = hair_color;
        shown_page = page;
    }
    return ok;
}

// Loads a picture from the library into the barber surface; its size, or -1, -1 when it cannot be drawn.
TPoint BarberGraphics::LoadSurface(int library, int resource_id)
{
    TPoint size;
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return TPoint(-1, -1);
    size = *(TPoint *)MainForm->gfx_transfers;
    surface_desc->SCaps = surface_caps;
    MainForm->gfx_transfers->CreateSurface(surface, surface_desc, size.x, size.y);
    surface_left = 0;
    surface_top = 0;
    surface_right = size.x;
    surface_bottom = size.y;
    if (MainForm->gfx_transfers->BeginDraw(surface))
    {
        MainForm->gfx_transfers->Draw(surface);
        MainForm->gfx_transfers->EndDraw(surface);
        return size;
    }
    return TPoint(-1, -1);
}
