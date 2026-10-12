#include <vcl.h>
#pragma hdrstop

#include "Gfx_menu.h"
#include "Tmainform.h"
#include "Ctrl_menu.h"
#include "Gfx_game.h"
#include "Gfx_interface.h"
#include <TDx_Draw_Library_Install.H>
#include <stdlib.h>
#pragma package(smart_init)
#define MENU_BUTTON_PITCH 40

MenuGraphics::MenuGraphics()
{
    background = new TDx_DrawSurface(NULL);
    artwork = new TDx_DrawSurface(NULL);
    main_menu_buttons = new TDx_DrawSurface(NULL);
    wide_buttons = new TDx_DrawSurface(NULL);
    small_buttons = new TDx_DrawSurface(NULL);
    message_box_background = new TDx_DrawSurface(NULL);
    list_box_background = new TDx_DrawSurface(NULL);
    create_character_background = new TDx_DrawSurface(NULL);
    progress_bar = new TDx_DrawSurface(NULL);
    slot_icons = new TDx_DrawSurface(NULL);
    animation_sheet = new TDx_DrawSurface(NULL);
    corner_button = new TDx_DrawSurface(NULL);
    lower_panel = new TDx_DrawSurface(NULL);
    upper_panel = new TDx_DrawSurface(NULL);
    key_sheet = new TDx_DrawSurface(NULL);
    slot_frame = new TDx_DrawSurface(NULL);
    label_sheet = new TDx_DrawSurface(NULL);
    scroll_icons = new TDx_DrawSurface(NULL);
    randomize();
    background_id = random(7) + 130;
    background_desc = new TDDSurfaceDesc(NULL);
    background_caps = new TDDSCaps(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    progress_fill_fx = new TDDBltFX(NULL);
    progress_fill_fx->FillColor = 11853030;
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
    background_desc->Flags = 6;
    background_desc->SCaps = background_caps;
    background_desc->SCaps->Caps = 2112;
    background_desc->Width = 640;
    background_desc->Height = 480;
    background->GetSurfaceDesc(background_desc);
    background->Create(background_desc, MainForm->Dx_Draw1);
    surface_desc->Flags = 6;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = 2112;
    surface_desc->Width = 360;
    surface_desc->Height = 160;
    main_menu_buttons->GetSurfaceDesc(surface_desc);
    main_menu_buttons->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 640;
    surface_desc->Height = 480;
    artwork->GetSurfaceDesc(surface_desc);
    artwork->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 149;
    surface_desc->Height = 115;
    label_sheet->GetSurfaceDesc(surface_desc);
    label_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 320;
    surface_desc->Height = 159;
    lower_panel->GetSurfaceDesc(surface_desc);
    lower_panel->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 253;
    surface_desc->Height = 119;
    key_sheet->GetSurfaceDesc(surface_desc);
    key_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 320;
    surface_desc->Height = 259;
    upper_panel->GetSurfaceDesc(surface_desc);
    upper_panel->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 276;
    surface_desc->Height = 123;
    slot_frame->GetSurfaceDesc(surface_desc);
    slot_frame->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 240;
    surface_desc->Height = 160;
    wide_buttons->GetSurfaceDesc(surface_desc);
    wide_buttons->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 182;
    surface_desc->Height = 290;
    small_buttons->GetSurfaceDesc(surface_desc);
    small_buttons->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 290;
    surface_desc->Height = 157;
    message_box_background->GetSurfaceDesc(surface_desc);
    message_box_background->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 360;
    surface_desc->Height = 240;
    list_box_background->GetSurfaceDesc(surface_desc);
    list_box_background->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 360;
    surface_desc->Height = 240;
    create_character_background->GetSurfaceDesc(surface_desc);
    create_character_background->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 260;
    surface_desc->Height = 12;
    progress_bar->GetSurfaceDesc(surface_desc);
    progress_bar->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 460;
    surface_desc->Height = 57;
    slot_icons->GetSurfaceDesc(surface_desc);
    slot_icons->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 880;
    surface_desc->Height = 127;
    animation_sheet->GetSurfaceDesc(surface_desc);
    animation_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 51;
    surface_desc->Height = 106;
    corner_button->GetSurfaceDesc(surface_desc);
    corner_button->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 16;
    surface_desc->Height = 255;
    scroll_icons->GetSurfaceDesc(surface_desc);
    scroll_icons->Create(surface_desc, MainForm->Dx_Draw1);
    background_rect.top = 0;
    background_rect.left = 0;
    background_rect.bottom = 480;
    background_rect.right = 640;
    message_box_rect.top = 0;
    message_box_rect.left = 0;
    message_box_rect.bottom = 157;
    message_box_rect.right = 290;
    create_character_rect.top = 0;
    create_character_rect.left = 0;
    create_character_rect.bottom = 240;
    create_character_rect.right = 360;
    list_box_rect.top = 0;
    list_box_rect.left = 0;
    list_box_rect.bottom = 240;
    list_box_rect.right = 360;
    up_key_rect.top = 0;
    up_key_rect.left = 154;
    up_key_rect.bottom = 20;
    up_key_rect.right = 177;
    down_key_rect.top = 20;
    down_key_rect.left = 154;
    down_key_rect.bottom = 40;
    down_key_rect.right = 177;
    left_key_rect.top = 20;
    left_key_rect.left = 131;
    left_key_rect.bottom = 40;
    left_key_rect.right = 154;
    right_key_rect.top = 20;
    right_key_rect.left = 177;
    right_key_rect.bottom = 40;
    right_key_rect.right = 200;
    upper_wide_key_rect.top = 41;
    upper_wide_key_rect.left = 131;
    upper_wide_key_rect.bottom = 61;
    upper_wide_key_rect.right = 180;
    lower_wide_key_rect.top = 61;
    lower_wide_key_rect.left = 131;
    lower_wide_key_rect.bottom = 81;
    lower_wide_key_rect.right = 180;
    small_key_rect.top = 81;
    small_key_rect.left = 131;
    small_key_rect.bottom = 96;
    small_key_rect.right = 146;
    message_icon_rect.top = 41;
    message_icon_rect.left = 181;
    message_icon_rect.bottom = 99;
    message_icon_rect.right = 252;
}

// One key of the soft keypad sheet, by column and row, with a one-pixel gap between keys.
Windows::TRect MenuGraphics::GetKeyRect(MenuGraphics *self, int column, int row)
{
    TRect key_rect;
    key_rect.left = 21 * column + column;
    key_rect.top = 19 * row + row;
    key_rect.right = 21 * column + column + 21;
    key_rect.bottom = 19 * row + row + 19;
    return key_rect;
}

// One icon of the menu scroll icon strip.
DEFINE_GET_SCROLL_ICON_RECT(MenuGraphics)

// One main menu button from its sheet: the button picks the row, the state the plain or lit column.
Windows::TRect MenuGraphics::GetMainMenuButtonRect(MenuGraphics *self, int button, int state)
{
    TRect main_menu_button_rect;
    main_menu_button_rect.left = 180 * state;
    main_menu_button_rect.top = MENU_BUTTON_PITCH * button;
    main_menu_button_rect.right = 180 * state + 179;
    main_menu_button_rect.bottom = 40 * button + 39;
    return main_menu_button_rect;
}

// One wide menu button from its sheet: the button picks the row, the state the plain or lit column.
Windows::TRect MenuGraphics::GetWideButtonRect(MenuGraphics *self, int button, int state)
{
    TRect wide_button_rect;
    wide_button_rect.left = 120 * state;
    wide_button_rect.top = MENU_BUTTON_PITCH * button;
    wide_button_rect.right = 120 * state + 119;
    wide_button_rect.bottom = 40 * button + 39;
    return wide_button_rect;
}

// One small menu button from its sheet: the button picks the row, the state the plain or lit column.
Windows::TRect MenuGraphics::GetSmallButtonRect(MenuGraphics *self, int button, int state)
{
    TRect small_button_rect;
    small_button_rect.left = BUTTON_COLUMN_PITCH * state;
    small_button_rect.top = BUTTON_ROW_PITCH * button;
    small_button_rect.right = 91 * state + 90;
    small_button_rect.bottom = 29 * button + 28;
    return small_button_rect;
}

// The menu corner button, plain or lit.
DEFINE_GET_CORNER_BUTTON_RECT(MenuGraphics)

MenuGraphics::~MenuGraphics()
{
    background->Destroy();
    artwork->Destroy();
    main_menu_buttons->Destroy();
    wide_buttons->Destroy();
    small_buttons->Destroy();
    lower_panel->Destroy();
    upper_panel->Destroy();
    key_sheet->Destroy();
    slot_frame->Destroy();
    label_sheet->Destroy();
    message_box_background->Destroy();
    list_box_background->Destroy();
    create_character_background->Destroy();
    progress_bar->Destroy();
    slot_icons->Destroy();
    animation_sheet->Destroy();
    corner_button->Destroy();
    scroll_icons->Destroy();
    delete background;
    delete artwork;
    delete main_menu_buttons;
    delete wide_buttons;
    delete small_buttons;
    delete message_box_background;
    delete list_box_background;
    delete create_character_background;
    delete progress_bar;
    delete slot_icons;
    delete animation_sheet;
    delete corner_button;
    delete upper_panel;
    delete key_sheet;
    delete lower_panel;
    delete slot_frame;
    delete label_sheet;
    delete scroll_icons;
    progress_fill_fx->Clear();
    color_key->Clear();
    delete progress_fill_fx;
    delete color_key;
    background_desc->Clear();
    background_caps->Clear();
    surface_desc->Clear();
    surface_caps->Clear();
    delete background_desc;
    delete background_caps;
    delete surface_desc;
    delete surface_caps;
}

// Draws the menu backdrop bitmap onto the background surface, opaque.
void MenuGraphics::LoadBackground(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (background->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        background->ReleaseDC(dc);
    }
}

// Draws the menu scroll icons bitmap onto its surface.
DEFINE_LOAD_SCROLL_ICONS(MenuGraphics)

// Draws the menu label sheet bitmap onto its surface, with black see-through.
void MenuGraphics::LoadLabelSheet(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (label_sheet->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        label_sheet->ReleaseDC(dc);
    }
    label_sheet->SetColorKey(8, color_key);
}

// Draws the menu upper panel bitmap onto its surface.
void MenuGraphics::LoadUpperPanel(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (upper_panel->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        upper_panel->ReleaseDC(dc);
    }
}

// Draws the menu key sheet bitmap onto its surface.
void MenuGraphics::LoadKeySheet(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (key_sheet->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        key_sheet->ReleaseDC(dc);
    }
}

// Draws the menu lower panel bitmap onto its surface.
void MenuGraphics::LoadLowerPanel(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (lower_panel->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        lower_panel->ReleaseDC(dc);
    }
}

// Draws the menu slot frame bitmap onto its surface.
void MenuGraphics::LoadSlotFrame(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (slot_frame->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        slot_frame->ReleaseDC(dc);
    }
}

// Draws a menu artwork bitmap onto the artwork surface and keeps its size, with black see-through.
void MenuGraphics::LoadArtwork(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    artwork_width = MainForm->resource_bitmap->Width;
    artwork_height = MainForm->resource_bitmap->Height;
    if (artwork->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        artwork->ReleaseDC(dc);
    }
    artwork->SetColorKey(8, color_key);
}

// Draws the menu main menu buttons bitmap onto its surface, with black see-through.
void MenuGraphics::LoadMainMenuButtons(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (main_menu_buttons->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        main_menu_buttons->ReleaseDC(dc);
    }
    main_menu_buttons->SetColorKey(8, color_key);
}

// Draws the menu wide buttons bitmap onto its surface, with black see-through.
void MenuGraphics::LoadWideButtons(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (wide_buttons->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        wide_buttons->ReleaseDC(dc);
    }
    wide_buttons->SetColorKey(8, color_key);
}

// Draws the menu small buttons bitmap onto its surface, with black see-through.
void MenuGraphics::LoadSmallButtons(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (small_buttons->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        small_buttons->ReleaseDC(dc);
    }
    small_buttons->SetColorKey(8, color_key);
}

// Draws the menu corner button bitmap onto its surface, with black see-through.
DEFINE_LOAD_CORNER_BUTTON(MenuGraphics)

// Draws the menu list box background bitmap onto its surface.
void MenuGraphics::LoadListBoxBackground(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (list_box_background->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        list_box_background->ReleaseDC(dc);
    }
}

// Draws the menu message box background bitmap onto its surface.
void MenuGraphics::LoadMessageBoxBackground(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (message_box_background->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        message_box_background->ReleaseDC(dc);
    }
}

// Draws the menu create character background bitmap onto its surface.
void MenuGraphics::LoadCreateCharacterBackground(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (create_character_background->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        create_character_background->ReleaseDC(dc);
    }
}

// Draws the menu progress bar bitmap onto its surface.
void MenuGraphics::LoadProgressBar(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (progress_bar->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        progress_bar->ReleaseDC(dc);
    }
}

// Draws the menu slot icons bitmap onto its surface.
void MenuGraphics::LoadSlotIcons(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (slot_icons->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        slot_icons->ReleaseDC(dc);
    }
}

// Draws the menu animation sheet bitmap onto its surface.
void MenuGraphics::LoadAnimationSheet(int library, int resource_id)
{
    HDC dc;
    MainForm->resource_bitmap->Width = 0;
    MainForm->resource_bitmap->Height = 0;
    MainForm->resource_bitmap->Transparent = false;
    MainForm->resource_bitmap->LoadFromResourceID(library, resource_id);
    if (animation_sheet->GetDC(&dc))
    {
        try
        {
            MainForm->draw_canvas->Handle = dc;
            MainForm->draw_canvas->Draw(0, 0, MainForm->resource_bitmap);
        }
        catch (Exception &)
        {
        }
        animation_sheet->ReleaseDC(dc);
    }
}

// Copies the loaded artwork onto the menu backdrop at x, y, with black see-through.
void MenuGraphics::DrawArtworkOnBackground(int x, int y)
{
    TRect source_rect;
    source_rect.left = 0;
    source_rect.top = 0;
    source_rect.right = artwork_width;
    source_rect.bottom = artwork_height;
    background->BltFast(x, y, artwork, &source_rect, 17);
}

// Copies the upper panel onto the menu backdrop at x, y, opaque.
void MenuGraphics::DrawUpperPanelOnBackground(int x, int y)
{
    TRect source_rect;
    source_rect.left = 0;
    source_rect.top = 0;
    source_rect.right = 320;
    source_rect.bottom = 259;
    background->BltFast(x, y, upper_panel, &source_rect, 16);
}

// Copies the lower panel onto the menu backdrop at x, y, opaque.
void MenuGraphics::DrawLowerPanelOnBackground(int x, int y)
{
    TRect source_rect;
    source_rect.left = 0;
    source_rect.top = 0;
    source_rect.right = 320;
    source_rect.bottom = 159;
    background->BltFast(x, y, lower_panel, &source_rect, 16);
}

// Copies one character slot frame onto the menu backdrop at x, y, opaque.
void MenuGraphics::DrawSlotFrameOnBackground(int x, int y)
{
    TRect source_rect;
    source_rect.left = 0;
    source_rect.top = 0;
    source_rect.right = 276;
    source_rect.bottom = 123;
    background->BltFast(x, y, slot_frame, &source_rect, 16);
}

// Copies the create-account form labels from the label sheet onto the menu backdrop, beside their boxes.
void MenuGraphics::DrawFormLabels(int origin_x, int origin_y)
{
    TRect label_left;
    label_left.left = 3;
    label_left.top = 93;
    label_left.right = 23;
    label_left.bottom = 113;
    background->BltFast(580, 362, label_sheet, &label_left, 17);
    label_left.left = 0;
    label_left.top = 0;
    label_left.right = 110;
    label_left.bottom = 13;
    background->BltFast(358, 50, label_sheet, &label_left, 17);
    label_left.left = 0;
    label_left.top = 14;
    label_left.right = 61;
    label_left.bottom = 27;
    background->BltFast(360, 102, label_sheet, &label_left, 17);
    label_left.left = 0;
    label_left.top = 28;
    label_left.right = 124;
    label_left.bottom = 44;
    background->BltFast(362, 153, label_sheet, &label_left, 17);
    label_left.left = 0;
    label_left.top = 44;
    label_left.right = 124;
    label_left.bottom = 59;
    background->BltFast(362, 243, label_sheet, &label_left, 17);
    label_left.left = 0;
    label_left.top = 59;
    label_left.right = 124;
    label_left.bottom = 75;
    background->BltFast(361, 293, label_sheet, &label_left, 17);
    label_left.left = 0;
    label_left.top = 75;
    label_left.right = 70;
    label_left.bottom = 89;
    background->BltFast(361, 344, label_sheet, &label_left, 17);
}

// Draws the account name and, as stars, the password on the soft keypad login screen.
void MenuGraphics::DrawLoginText()
{
    int password_length;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = (TColor)0x8CA0B4;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 8;
    MainForm->text_canvas->TextOut(450, 261, MainForm->animation->menu->account_name);
    if (MainForm->animation->menu->password != "")
    {
        String masked_password = MainForm->animation->menu->password;
        password_length = MainForm->animation->menu->password.Length();
        for (int i = 1; i <= password_length; i++)
            masked_password[i] = 42;
        MainForm->text_canvas->TextOut(450, 305, masked_password);
    }
}

// Draws each character's name, centred, and the number sent with it in its slot on the character selection screen.
void MenuGraphics::DrawCharacterSlotText()
{
    int i;
    int name_x;
    MainForm->text_canvas->Brush->Color = clWhite;
    MainForm->text_canvas->Brush->Style = bsClear;
    MainForm->text_canvas->Font->Color = (TColor)0x8CA0B4;
    MainForm->text_canvas->Font->Name = "MS Sans Serif";
    MainForm->text_canvas->Font->Size = 8;
    if (MainForm->players->GetCount() >= 1)
    {
        for (i = 0; i < (int)MainForm->players->GetCount(); i++)
        {
            Player handle = MainForm->players->GetPlayer(i);
            if (!handle.slot_state)
            {
                name_x = 539 - MainForm->text_canvas->TextWidth(handle.name) / 2;
                if (handle.player_id == PlayerId_SlotFirst)
                {
                    MainForm->text_canvas->TextOut(name_x, 66, handle.name);
                    MainForm->text_canvas->TextOut(364, 136, IntToStr(handle.display_number));
                }
                if (handle.player_id == PlayerId_SlotSecond)
                {
                    MainForm->text_canvas->TextOut(name_x, 190, handle.name);
                    MainForm->text_canvas->TextOut(364, 260, IntToStr(handle.display_number));
                }
                if (handle.player_id == PlayerId_SlotThird)
                {
                    MainForm->text_canvas->TextOut(name_x, 313, handle.name);
                    MainForm->text_canvas->TextOut(364, 384, IntToStr(handle.display_number));
                }
            }
        }
    }
}

// Draws the visible lines of the menu text window, from its scroll offset.
void MenuGraphics::DrawListBoxText()
{
    int first_index;
    int entry_count;
    int row;
    int i;
    int text_x;
    int text_y;
    MainForm->draw_canvas->Brush->Color = clWhite;
    MainForm->draw_canvas->Brush->Style = bsClear;
    MainForm->draw_canvas->Font->Color = clBlack;
    MainForm->draw_canvas->Font->Name = "MS Sans Serif";
    MainForm->draw_canvas->Font->Size = 7;
    MainForm->draw_canvas->Font->Color = (TColor)0xC8C8C8;
    MenuController *menu = MainForm->animation->menu;
    first_index = menu->GetScrollOffset();
    entry_count = MainForm->animation->menu->text_lines->Count;
    row = 0;
    for (i = first_index; i < entry_count && i < first_index + 7; i++)
    {
        String entry = MainForm->animation->menu->text_lines->Strings[i];
        if (entry.Length() > 0)
        {
            MainForm->draw_canvas->Font->Color = (TColor)0xC8C8C8;
            text_x = MainForm->animation->menu->text_window_x + 30;
            text_y = MainForm->animation->menu->text_window_y + 16 * row + 70;
            MainForm->draw_canvas->TextOut(text_x, text_y, entry);
        }
        row++;
    }
}

// Draws the menu message box title and two message lines.
// A first line too wide for the box is wrapped at spaces into the lines below it.
void MenuGraphics::DrawMessageBoxText()
{
    int wrap_width;
    int text_x;
    int line_index;
    int line_start;
    int line_end;
    int i;
    MainForm->draw_canvas->Brush->Color = clWhite;
    MainForm->draw_canvas->Brush->Style = bsClear;
    MainForm->draw_canvas->Font->Color = (TColor)0xC8F0F0;
    MainForm->draw_canvas->Font->Name = "MS Sans Serif";
    MainForm->draw_canvas->Font->Size = 10;
    wrap_width = 250;
    text_x = 20;
    // A box with an image keeps the text clear of it, narrower.
    if (MainForm->animation->menu->message_shows_image)
    {
        wrap_width = 170;
        text_x = 100;
    }
    if (MainForm->draw_canvas->TextWidth(MainForm->animation->menu->message_lines[0]) > wrap_width)
    {
        String message = MainForm->animation->menu->message_lines[0];
        line_start = 1;
        line_end = 1;
        line_index = 0;
        do
        {
            if (MainForm->draw_canvas->TextWidth(message.SubString(line_start, line_end)) >= wrap_width ||
                line_end == message.Length())
            {
                char seeking_break = 1;
                for (i = line_end; i >= 1 && seeking_break; i--)
                {
                    if (message[i] != ' ')
                    {
                        if (i != message.Length())
                            continue;
                    }
                    if (line_index <= MESSAGE_BOX_LINE_COUNT && message.SubString(line_start, i).Length() >= 1)
                    {
                        if (message.SubString(line_start, i)[1] == 32)
                            MainForm->animation->menu->message_lines[line_index] = message.SubString(line_start + 1, i);
                        else
                            MainForm->animation->menu->message_lines[line_index] = message.SubString(line_start, i);
                    }
                    seeking_break = 0;
                    line_start = i;
                    line_end = i;
                    line_end++;
                    line_index++;
                }
            }
            else
                line_end++;
        } while (message.Length() >= line_end && line_index <= 1);
    }
    MainForm->draw_canvas->TextOut(MainForm->animation->menu->message_x + 60, MainForm->animation->menu->message_y + 26,
        MainForm->animation->menu->message_title);
    MainForm->draw_canvas->TextOut(text_x + MainForm->animation->menu->message_x,
        MainForm->animation->menu->message_y + 61, MainForm->animation->menu->message_lines[0]);
    MainForm->draw_canvas->TextOut(text_x + MainForm->animation->menu->message_x,
        MainForm->animation->menu->message_y + 77, MainForm->animation->menu->message_lines[1]);
}

// Draws the menu text window's scroll arrows, scroll thumb and OK button, each lit under the mouse.
void MenuGraphics::DrawListBoxControls()
{
    if (MainForm->animation->menu->text_scroll_up_hover)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->text_window_x + 320,
            MainForm->animation->menu->text_window_y + 66, MainForm->gfx->menu->scroll_icons,
            &GetScrollIconRect(MainForm->gfx->menu, 11), 16);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->text_window_x + 320,
            MainForm->animation->menu->text_window_y + 66, MainForm->gfx->menu->scroll_icons,
            &GetScrollIconRect(MainForm->gfx->menu, 10), 16);
    if (MainForm->animation->menu->text_scroll_down_hover)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->text_window_x + 320,
            MainForm->animation->menu->text_window_y + 170, MainForm->gfx->menu->scroll_icons,
            &GetScrollIconRect(MainForm->gfx->menu, 9), 16);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->text_window_x + 320,
            MainForm->animation->menu->text_window_y + 170, MainForm->gfx->menu->scroll_icons,
            &GetScrollIconRect(MainForm->gfx->menu, 8), 16);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->text_window_x + 320,
        MainForm->animation->menu->text_window_y + MainForm->animation->menu->GetScrollbarPosition() + 82,
        MainForm->gfx->menu->scroll_icons, &GetScrollIconRect(MainForm->gfx->menu, 7), 16);
    if (MainForm->animation->menu->text_window_ok_button_hover)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->text_window_x + 137,
            MainForm->animation->menu->text_window_y + 196, small_buttons, &GetSmallButtonRect(this, 4, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->text_window_x + 137,
            MainForm->animation->menu->text_window_y + 196, small_buttons, &GetSmallButtonRect(this, 4, 0), 17);
}

// Draws the menu message box buttons, the progress bar of a progress box, and the image of a box that has one.
void MenuGraphics::DrawMessageBoxControls()
{
    int ok_button;
    TRect bar_rect;
    TRect fill_rect;
    ok_button = 4;
    // A confirm box has a second button beside the first.
    if (MainForm->animation->menu->message_kind == MessageBoxKind_Confirm)
        ok_button = 1;
    // A progress box fills its bar with the account creation progress.
    if (MainForm->animation->menu->message_kind == MessageBoxKind_Progress)
    {
        ok_button = 1;
        bar_rect.left = 0;
        bar_rect.top = 0;
        bar_rect.right = 260;
        bar_rect.bottom = 12;
        fill_rect.left = MainForm->animation->menu->message_x + 16;
        fill_rect.top = MainForm->animation->menu->message_y + 93;
        fill_rect.right = MainForm->animation->menu->message_x + MainForm->animation->GetAccountProgress() / 2 + 16;
        fill_rect.bottom = MainForm->animation->menu->message_y + 101;
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->message_x + 14,
            MainForm->animation->menu->message_y + 91, progress_bar, &bar_rect, 16);
        MainForm->OffscreenSurface->Blt(&fill_rect, 0, 0, 16778240, progress_fill_fx);
    }
    if (MainForm->animation->menu->message_close_button_hover)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->message_x + 181,
            MainForm->animation->menu->message_y + 113, small_buttons, &GetSmallButtonRect(this, ok_button, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->message_x + 181,
            MainForm->animation->menu->message_y + 113, small_buttons, &GetSmallButtonRect(this, ok_button, 0), 17);
    if (MainForm->animation->menu->message_kind == MessageBoxKind_Confirm)
    {
        if (MainForm->animation->menu->message_confirm_button_hover)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->message_x + 88,
                MainForm->animation->menu->message_y + 113, small_buttons, &GetSmallButtonRect(this, 4, 1), 17);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->message_x + 88,
                MainForm->animation->menu->message_y + 113, small_buttons, &GetSmallButtonRect(this, 4, 0), 17);
    }
    if (MainForm->animation->menu->message_shows_image)
    {
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->message_x + 16,
            MainForm->animation->menu->message_y + 56, key_sheet, &message_icon_rect, 16);
        return;
    }
}

// Draws the create-character choices (gender, hair style, hair colour, skin) with their arrows, and the form buttons.
void MenuGraphics::DrawCreateCharacterControls()
{
    int hair_style;
    int hair_color;
    int skin;
    TRect icon_left;
    TRect arrow_rect;
    TRect arrow_pressed_rect;
    // Only the create-character form has the choices; other forms show just the buttons.
    if (MainForm->animation->menu->form_kind == CHARACTER_CREATE_DIALOG)
    {
        int gender = MainForm->animation->menu->new_gender;
        hair_style = MainForm->animation->menu->new_hair_style;
        hair_color = MainForm->animation->menu->new_hair_color;
        skin = MainForm->animation->menu->new_skin;
        icon_left.left = 23 * gender;
        icon_left.right = 23 * gender + 22;
        icon_left.top = 38;
        icon_left.bottom = 56;
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 170,
            MainForm->animation->menu->form_y + 84, slot_icons, &icon_left, 16);
        // Hair styles count from one; the other choices count from zero.
        icon_left.left = 23 * hair_style - 23;
        icon_left.right = 23 * hair_style - 1;
        icon_left.top = 19;
        icon_left.bottom = 38;
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 170,
            MainForm->animation->menu->form_y + 111, slot_icons, &icon_left, 16);
        icon_left.left = 23 * hair_color;
        icon_left.right = 23 * hair_color + 22;
        icon_left.top = 0;
        icon_left.bottom = 19;
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 170,
            MainForm->animation->menu->form_y + 138, slot_icons, &icon_left, 16);
        icon_left.left = 23 * skin + 46;
        icon_left.right = 23 * skin + 68;
        icon_left.top = 38;
        icon_left.bottom = 56;
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 170,
            MainForm->animation->menu->form_y + 165, slot_icons, &icon_left, 16);
        arrow_rect.left = 184;
        arrow_rect.top = 38;
        arrow_rect.right = 204;
        arrow_rect.bottom = 56;
        arrow_pressed_rect.left = 205;
        arrow_pressed_rect.top = 38;
        arrow_pressed_rect.right = 225;
        arrow_pressed_rect.bottom = 56;
        if (MainForm->animation->menu->gender_arrow_hover)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 194,
                MainForm->animation->menu->form_y + 84, slot_icons, &arrow_pressed_rect, 16);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 194,
                MainForm->animation->menu->form_y + 84, slot_icons, &arrow_rect, 16);
        if (MainForm->animation->menu->hair_style_arrow_hover)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 194,
                MainForm->animation->menu->form_y + 111, slot_icons, &arrow_pressed_rect, 16);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 194,
                MainForm->animation->menu->form_y + 111, slot_icons, &arrow_rect, 16);
        if (MainForm->animation->menu->hair_color_arrow_hover)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 194,
                MainForm->animation->menu->form_y + 138, slot_icons, &arrow_pressed_rect, 16);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 194,
                MainForm->animation->menu->form_y + 138, slot_icons, &arrow_rect, 16);
        if (MainForm->animation->menu->skin_arrow_hover)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 194,
                MainForm->animation->menu->form_y + 165, slot_icons, &arrow_pressed_rect, 16);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 194,
                MainForm->animation->menu->form_y + 165, slot_icons, &arrow_rect, 16);
    }
    if (MainForm->animation->menu->form_ok_button_hover)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 157,
            MainForm->animation->menu->form_y + 195, small_buttons, &GetSmallButtonRect(this, 4, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 157,
            MainForm->animation->menu->form_y + 195, small_buttons, &GetSmallButtonRect(this, 4, 0), 17);
    if (MainForm->animation->menu->form_cancel_button_hover)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 250,
            MainForm->animation->menu->form_y + 195, small_buttons, &GetSmallButtonRect(this, 1, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 250,
            MainForm->animation->menu->form_y + 195, small_buttons, &GetSmallButtonRect(this, 1, 0), 17);
}

// Draws the four main menu buttons, each lit under the mouse.
void MenuGraphics::DrawMainMenuButtons()
{
    if (MainForm->animation->menu->create_account_button_hover)
        MainForm->OffscreenSurface->BltFast(25, 280, main_menu_buttons, &GetMainMenuButtonRect(this, 0, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(25, 280, main_menu_buttons, &GetMainMenuButtonRect(this, 0, 0), 17);
    if (MainForm->animation->menu->play_button_hover)
        MainForm->OffscreenSurface->BltFast(25, 320, main_menu_buttons, &GetMainMenuButtonRect(this, 1, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(25, 320, main_menu_buttons, &GetMainMenuButtonRect(this, 1, 0), 17);
    if (MainForm->animation->menu->credits_button_hover)
        MainForm->OffscreenSurface->BltFast(25, 360, main_menu_buttons, &GetMainMenuButtonRect(this, 2, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(25, 360, main_menu_buttons, &GetMainMenuButtonRect(this, 2, 0), 17);
    if (MainForm->animation->menu->exit_button_hover)
        MainForm->OffscreenSurface->BltFast(25, 400, main_menu_buttons, &GetMainMenuButtonRect(this, 3, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(25, 400, main_menu_buttons, &GetMainMenuButtonRect(this, 3, 0), 17);
}

// Draws the corner close button and the create and cancel buttons of the create-account screen.
void MenuGraphics::DrawAccountCreationButtons()
{
    if (MainForm->animation->menu->corner_close_hover)
        MainForm->OffscreenSurface->BltFast(589, 0, MainForm->gfx->menu->corner_button,
            &GetCornerButtonRect(MainForm->gfx->menu, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(589, 0, MainForm->gfx->menu->corner_button,
            &GetCornerButtonRect(MainForm->gfx->menu, 0), 17);
    if (MainForm->animation->menu->account_create_button_hover)
        MainForm->OffscreenSurface->BltFast(358, 416, wide_buttons, &GetWideButtonRect(this, 0, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(358, 416, wide_buttons, &GetWideButtonRect(this, 0, 0), 17);
    if (MainForm->animation->menu->account_cancel_button_hover)
        MainForm->OffscreenSurface->BltFast(480, 416, wide_buttons, &GetWideButtonRect(this, 1, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(480, 416, wide_buttons, &GetWideButtonRect(this, 1, 0), 17);
}

// Draws the soft keypad's blinking cursor key and the keys lit by the keyboard.
void MenuGraphics::DrawKeyHighlights()
{
    int key_y;
    int key_column;
    int key_row;
    int key_x = 0;
    if (MainForm->animation->menu->cursor_blink > 4)
    {
        key_x = 22 * MainForm->animation->menu->grid_column + 294;
        key_y = 20 * MainForm->animation->menu->grid_row + 250;
        key_column = MainForm->animation->menu->grid_column;
        key_row = MainForm->animation->menu->grid_row;
        MainForm->OffscreenSurface->BltFast(key_x, key_y, this->key_sheet, &GetKeyRect(this, key_column, key_row), 16);
    }
    if (MainForm->animation->menu->up_key_lit)
        MainForm->OffscreenSurface->BltFast(464, 334, key_sheet, &up_key_rect, 16);
    if (MainForm->animation->menu->left_key_lit)
        MainForm->OffscreenSurface->BltFast(441, 354, key_sheet, &left_key_rect, 16);
    if (MainForm->animation->menu->down_key_lit)
        MainForm->OffscreenSurface->BltFast(464, 354, key_sheet, &down_key_rect, 16);
    if (MainForm->animation->menu->right_key_lit)
        MainForm->OffscreenSurface->BltFast(487, 354, key_sheet, &right_key_rect, 16);
    if (MainForm->animation->menu->select_key_lit)
        MainForm->OffscreenSurface->BltFast(514, 354, key_sheet, &lower_wide_key_rect, 16);
    if (MainForm->animation->menu->space_key_lit)
        MainForm->OffscreenSurface->BltFast(514, 334, key_sheet, &upper_wide_key_rect, 16);
    if (MainForm->animation->menu->delete_key_lit)
    {
        MainForm->OffscreenSurface->BltFast(548, 283, key_sheet, &small_key_rect, 16);
        return;
    }
}

// Draws the two buttons of the login screen.
void MenuGraphics::DrawConfirmButtons()
{
    if (MainForm->config->GetSkipSoftpad())
    {
        if (MainForm->animation->menu->login_ok_button_hover)
            MainForm->OffscreenSurface->BltFast(360, 388, small_buttons, &GetSmallButtonRect(this, 0, 1), 17);
        else
            MainForm->OffscreenSurface->BltFast(360, 388, small_buttons, &GetSmallButtonRect(this, 0, 0), 17);
        if (MainForm->animation->menu->login_cancel_button_hover)
            MainForm->OffscreenSurface->BltFast(452, 388, small_buttons, &GetSmallButtonRect(this, 1, 1), 17);
        else
            MainForm->OffscreenSurface->BltFast(452, 388, small_buttons, &GetSmallButtonRect(this, 1, 0), 17);
    }
    else
    {
        int confirm_button = 4;
        // On the keypad's password step the first button is the login button.
        if (MainForm->animation->GetSubState() == 8)
            confirm_button = 2;
        if (MainForm->animation->menu->login_ok_button_hover)
            MainForm->OffscreenSurface->BltFast(382, 391, small_buttons, &GetSmallButtonRect(this, confirm_button, 1),
                17);
        else
            MainForm->OffscreenSurface->BltFast(382, 391, small_buttons, &GetSmallButtonRect(this, confirm_button, 0),
                17);
        if (MainForm->animation->menu->login_cancel_button_hover)
            MainForm->OffscreenSurface->BltFast(474, 391, small_buttons, &GetSmallButtonRect(this, 1, 1), 17);
        else
            MainForm->OffscreenSurface->BltFast(474, 391, small_buttons, &GetSmallButtonRect(this, 1, 0), 17);
    }
}

// Draws the character selection controls: corner close, change password, new character, each slot's login and
// delete buttons, and each character's badge.
void MenuGraphics::DrawCharacterSelect()
{
    if (MainForm->animation->menu->corner_close_hover)
        MainForm->OffscreenSurface->BltFast(589, 0, MainForm->gfx->menu->corner_button,
            &GetCornerButtonRect(MainForm->gfx->menu, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(589, 0, MainForm->gfx->menu->corner_button,
            &GetCornerButtonRect(MainForm->gfx->menu, 0), 17);
    if (MainForm->animation->menu->change_password_button_hover)
        MainForm->OffscreenSurface->BltFast(452, 416, wide_buttons, &GetWideButtonRect(this, 3, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(452, 416, wide_buttons, &GetWideButtonRect(this, 3, 0), 17);
    if (MainForm->animation->menu->new_character_button_hover)
        MainForm->OffscreenSurface->BltFast(332, 416, wide_buttons, &GetWideButtonRect(this, 0, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(332, 416, wide_buttons, &GetWideButtonRect(this, 0, 0), 17);
    if (MainForm->animation->menu->first_slot_login_hover)
        MainForm->OffscreenSurface->BltFast(495, 93, small_buttons, &GetSmallButtonRect(this, 2, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(495, 93, small_buttons, &GetSmallButtonRect(this, 2, 0), 17);
    if (MainForm->animation->menu->second_slot_login_hover)
        MainForm->OffscreenSurface->BltFast(495, 217, small_buttons, &GetSmallButtonRect(this, 2, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(495, 217, small_buttons, &GetSmallButtonRect(this, 2, 0), 17);
    if (MainForm->animation->menu->third_slot_login_hover)
        MainForm->OffscreenSurface->BltFast(495, 341, small_buttons, &GetSmallButtonRect(this, 2, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(495, 341, small_buttons, &GetSmallButtonRect(this, 2, 0), 17);
    if (MainForm->animation->menu->first_slot_delete_hover)
        MainForm->OffscreenSurface->BltFast(495, 121, small_buttons, &GetSmallButtonRect(this, 3, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(495, 121, small_buttons, &GetSmallButtonRect(this, 3, 0), 17);
    if (MainForm->animation->menu->second_slot_delete_hover)
        MainForm->OffscreenSurface->BltFast(495, 245, small_buttons, &GetSmallButtonRect(this, 3, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(495, 245, small_buttons, &GetSmallButtonRect(this, 3, 0), 17);
    if (MainForm->animation->menu->third_slot_delete_hover)
        MainForm->OffscreenSurface->BltFast(495, 369, small_buttons, &GetSmallButtonRect(this, 3, 1), 17);
    else
        MainForm->OffscreenSurface->BltFast(495, 369, small_buttons, &GetSmallButtonRect(this, 3, 0), 17);
    for (int i = 0; i < (int)MainForm->players->GetCount(); i++)
    {
        Player handle = MainForm->players->GetPlayer(i);
        if (!handle.slot_state)
        {
            // Badge level one has a badge of its own; the higher levels share another.
            if (handle.badge_level == 1)
            {
                TRect single_badge_rect;
                single_badge_rect.left = 252;
                single_badge_rect.top = 39;
                single_badge_rect.right = 269;
                single_badge_rect.bottom = 55;
                MainForm->OffscreenSurface->BltFast(handle.x + 42, handle.y + 62, slot_icons, &single_badge_rect, 16);
            }
            if (handle.badge_level > 1)
            {
                TRect badge_rect;
                badge_rect.left = 233;
                badge_rect.top = 39;
                badge_rect.right = 250;
                badge_rect.bottom = 55;
                MainForm->OffscreenSurface->BltFast(handle.x + 42, handle.y + 62, slot_icons, &badge_rect, 16);
            }
        }
    }
}
