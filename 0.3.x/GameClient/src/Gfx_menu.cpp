#include <vcl.h>
#pragma hdrstop

#include "Gfx_menu.h"
#include "Tmainform.h"
#include "Ctrl_menu.h"
#include "Gfx_game.h"
#include "Gfx_interface.h"
#include <stdlib.h>
#include "Gfx_global_sprites.h"

#pragma package(smart_init)

// Creates the menu surfaces and picks one of the seven backgrounds at random.
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
    checkbox_sheet = new TDx_DrawSurface(NULL);
    form_sheet = new TDx_DrawSurface(NULL);
    upper_panel = new TDx_DrawSurface(NULL);
    key_sheet = new TDx_DrawSurface(NULL);
    slot_frame = new TDx_DrawSurface(NULL);
    label_sheet = new TDx_DrawSurface(NULL);
    scroll_icons = new TDx_DrawSurface(NULL);
    hair_styles = new TDx_DrawSurface(NULL);
    hair_surface = new TDx_DrawSurface(NULL);
    randomize();
    background_id = random(7) + 130;
    shown_gender = 1;
    background_desc = new TDDSurfaceDesc(NULL);
    background_caps = new TDDSCaps(NULL);
    surface_desc = new TDDSurfaceDesc(NULL);
    surface_caps = new TDDSCaps(NULL);
    color_key = new TDDColorKey(NULL);
    progress_fill_fx = new TDDBltFX(NULL);
    progress_fill_fx->FillColor = 0xDCDCB4;
    clear_fx = new TDDBltFX(NULL);
    clear_fx->FillColor = 0;
    color_key->ColorSpaceHighValue = 0;
    color_key->ColorSpaceLowValue = 0;
    background_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    background_desc->SCaps = background_caps;
    background_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    background_desc->Width = 640;
    background_desc->Height = 480;
    background->GetSurfaceDesc(background_desc);
    background->Create(background_desc, MainForm->Dx_Draw1);
    surface_desc->Flags = DDSD_HEIGHT | DDSD_WIDTH;
    surface_desc->SCaps = surface_caps;
    surface_desc->SCaps->Caps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
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
    surface_desc->Width = 199;
    surface_desc->Height = 30;
    checkbox_sheet->GetSurfaceDesc(surface_desc);
    checkbox_sheet->Create(surface_desc, MainForm->Dx_Draw1);
    surface_desc->Width = 243;
    surface_desc->Height = 136;
    form_sheet->GetSurfaceDesc(surface_desc);
    form_sheet->Create(surface_desc, MainForm->Dx_Draw1);
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
    surface_desc->Height = 200;
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
    surface_desc->Width = 465;
    surface_desc->Height = 257;
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
    surface_desc->Width = 252;
    surface_desc->Height = 32;
    hair_styles->GetSurfaceDesc(surface_desc);
    hair_styles->Create(surface_desc, MainForm->Dx_Draw1);
    hair_styles->SetColorKey(DDCKEY_SRCBLT, color_key);
    surface_desc->Width = 28;
    surface_desc->Height = 54;
    hair_surface->GetSurfaceDesc(surface_desc);
    hair_surface->Create(surface_desc, MainForm->Dx_Draw1);
    hair_surface->SetColorKey(DDCKEY_SRCBLT, color_key);
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
    create_character_rect.bottom = 257;
    create_character_rect.right = 465;
    list_box_rect.top = 0;
    list_box_rect.left = 0;
    list_box_rect.bottom = 240;
    list_box_rect.right = 360;
    hair_styles_rect.top = 0;
    hair_styles_rect.left = 0;
    hair_styles_rect.bottom = 32;
    hair_styles_rect.right = 248;
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
    cursor_rect.left = 234;
    cursor_rect.right = 240;
    cursor_rect.top = 4;
    cursor_rect.bottom = 12;
}

// One key of the soft keypad sheet, by column and row, with a one-pixel gap between keys.
TRect MenuGraphics::GetKeyRect(int column, int row)
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
TRect MenuGraphics::GetMainMenuButtonRect(int button, int state)
{
    TRect main_menu_button_rect;
    main_menu_button_rect.left = 180 * state;
    main_menu_button_rect.top = MENU_BUTTON_PITCH * button;
    main_menu_button_rect.right = 180 * state + 179;
    main_menu_button_rect.bottom = MENU_BUTTON_PITCH * button + 39;
    return main_menu_button_rect;
}

// One wide menu button from its sheet: the button picks the row, the state the plain or lit column.
TRect MenuGraphics::GetWideButtonRect(int button, int state)
{
    TRect wide_button_rect;
    wide_button_rect.left = 120 * state;
    wide_button_rect.top = MENU_BUTTON_PITCH * button;
    wide_button_rect.right = 120 * state + 119;
    wide_button_rect.bottom = MENU_BUTTON_PITCH * button + 39;
    return wide_button_rect;
}

// One small menu button from its sheet: the button picks the row, the state the plain or lit column.
TRect MenuGraphics::GetSmallButtonRect(int button, int state)
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

TRect MenuGraphics::GetArrowRect(int arrow)
{
    TRect rect;
    rect.left = 8 * arrow + 65;
    rect.right = 8 * arrow + 72;
    rect.top = 0;
    rect.bottom = 11;
    return rect;
}

TRect MenuGraphics::GetCheckboxRect(int checked)
{
    TRect rect;
    rect.left = 0;
    rect.right = 199;
    rect.top = 15 * checked;
    rect.bottom = 15 * checked + 14;
    return rect;
}

// Destroys and frees every menu surface.
MenuGraphics::~MenuGraphics()
{
    background->Destroy();
    artwork->Destroy();
    main_menu_buttons->Destroy();
    wide_buttons->Destroy();
    small_buttons->Destroy();
    lower_panel->Destroy();
    checkbox_sheet->Destroy();
    form_sheet->Destroy();
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
    hair_styles->Destroy();
    hair_surface->Destroy();
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
    delete checkbox_sheet;
    delete form_sheet;
    delete slot_frame;
    delete label_sheet;
    delete scroll_icons;
    delete hair_styles;
    delete hair_surface;
    progress_fill_fx->Clear();
    clear_fx->Clear();
    color_key->Clear();
    delete progress_fill_fx;
    delete clear_fx;
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

// Loads the library's bitmap onto the surface; false when it cannot.
bool MenuGraphics::LoadSurface(TDx_DrawSurface *surface, int library, int resource_id)
{
    if (!MainForm->gfx_transfers->LoadResourceBitmap(library, resource_id))
        return false;
    if (MainForm->gfx_transfers->BeginDraw(surface))
    {
        MainForm->gfx_transfers->Draw(surface);
        MainForm->gfx_transfers->EndDraw(surface);
        return true;
    }
    return false;
}

// Draws the menu backdrop bitmap onto the background surface, opaque.
void MenuGraphics::LoadBackground(int library, int resource_id)
{
    LoadSurface(background, library, resource_id);
}

// Draws the menu scroll icons bitmap onto its surface.
void MenuGraphics::LoadScrollIcons(int library, int resource_id)
{
    LoadSurface(scroll_icons, library, resource_id);
}

// Draws the menu label sheet bitmap onto its surface, with black see-through.
void MenuGraphics::LoadLabelSheet(int library, int resource_id)
{
    LoadSurface(label_sheet, library, resource_id);
    label_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the menu upper panel bitmap onto its surface.
void MenuGraphics::LoadUpperPanel(int library, int resource_id)
{
    LoadSurface(upper_panel, library, resource_id);
}

// Draws the menu key sheet bitmap onto its surface.
void MenuGraphics::LoadKeySheet(int library, int resource_id)
{
    LoadSurface(key_sheet, library, resource_id);
}

// Draws the menu lower panel bitmap onto its surface.
void MenuGraphics::LoadLowerPanel(int library, int resource_id)
{
    LoadSurface(lower_panel, library, resource_id);
}

void MenuGraphics::LoadCheckboxSheet(int library, int resource_id)
{
    LoadSurface(checkbox_sheet, library, resource_id);
}

void MenuGraphics::LoadFormSheet(int library, int resource_id)
{
    LoadSurface(form_sheet, library, resource_id);
    form_sheet->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the menu slot frame bitmap onto its surface.
void MenuGraphics::LoadSlotFrame(int library, int resource_id)
{
    LoadSurface(slot_frame, library, resource_id);
}

// Loads the artwork and keeps its size.
void MenuGraphics::LoadArtwork(int library, int resource_id)
{
    LoadSurface(artwork, library, resource_id);
    artwork_width = MainForm->gfx_transfers->width;
    artwork_height = MainForm->gfx_transfers->height;
    artwork->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the menu main menu buttons bitmap onto its surface, with black see-through.
void MenuGraphics::LoadMainMenuButtons(int library, int resource_id)
{
    LoadSurface(main_menu_buttons, library, resource_id);
    main_menu_buttons->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the menu wide buttons bitmap onto its surface, with black see-through.
void MenuGraphics::LoadWideButtons(int library, int resource_id)
{
    LoadSurface(wide_buttons, library, resource_id);
    wide_buttons->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the menu small buttons bitmap onto its surface, with black see-through.
void MenuGraphics::LoadSmallButtons(int library, int resource_id)
{
    LoadSurface(small_buttons, library, resource_id);
    small_buttons->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the menu corner button bitmap onto its surface, with black see-through.
void MenuGraphics::LoadCornerButton(int library, int resource_id)
{
    LoadSurface(corner_button, library, resource_id);
    corner_button->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the menu list box background bitmap onto its surface.
void MenuGraphics::LoadListBoxBackground(int library, int resource_id)
{
    LoadSurface(list_box_background, library, resource_id);
}

// Draws the menu message box background bitmap onto its surface.
void MenuGraphics::LoadMessageBoxBackground(int library, int resource_id)
{
    LoadSurface(message_box_background, library, resource_id);
}

// Draws the menu create character background bitmap onto its surface.
void MenuGraphics::LoadCreateCharacterBackground(int library, int resource_id)
{
    LoadSurface(create_character_background, library, resource_id);
}

// Draws the menu progress bar bitmap onto its surface.
void MenuGraphics::LoadProgressBar(int library, int resource_id)
{
    LoadSurface(progress_bar, library, resource_id);
}

// Draws the menu slot icons bitmap onto its surface.
void MenuGraphics::LoadSlotIcons(int library, int resource_id)
{
    LoadSurface(slot_icons, library, resource_id);
    slot_icons->SetColorKey(DDCKEY_SRCBLT, color_key);
}

// Draws the menu animation sheet bitmap onto its surface.
void MenuGraphics::LoadAnimationSheet(int library, int resource_id)
{
    LoadSurface(animation_sheet, library, resource_id);
}

// Copies the loaded artwork onto the menu backdrop at x, y, with black see-through.
void MenuGraphics::DrawArtworkOnBackground(int x, int y)
{
    TRect source_rect;
    source_rect.left = 0;
    source_rect.top = 0;
    source_rect.right = artwork_width;
    source_rect.bottom = artwork_height;
    background->BltFast(x, y, artwork, &source_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Copies the upper panel onto the menu backdrop at x, y, opaque.
void MenuGraphics::DrawUpperPanelOnBackground(int x, int y)
{
    TRect source_rect;
    source_rect.left = 0;
    source_rect.top = 0;
    source_rect.right = 320;
    source_rect.bottom = 259;
    background->BltFast(x, y, upper_panel, &source_rect, DDBLTFAST_WAIT);
}

// Copies the lower panel onto the menu backdrop at x, y, opaque.
void MenuGraphics::DrawLowerPanelOnBackground(int x, int y)
{
    TRect source_rect;
    source_rect.left = 0;
    source_rect.top = 0;
    source_rect.right = 320;
    source_rect.bottom = 159;
    background->BltFast(x, y, lower_panel, &source_rect, DDBLTFAST_WAIT);
}

// Copies one character slot frame onto the menu backdrop at x, y, opaque.
void MenuGraphics::DrawSlotFrameOnBackground(int x, int y)
{
    TRect source_rect;
    source_rect.left = 0;
    source_rect.top = 0;
    source_rect.right = 276;
    source_rect.bottom = 123;
    background->BltFast(x, y, slot_frame, &source_rect, DDBLTFAST_WAIT);
}

// Copies the create-account form labels from the label sheet onto the menu backdrop, beside their boxes.
void MenuGraphics::DrawFormLabels(int origin_x, int origin_y)
{
    TRect label_left;
    label_left.left = 3;
    label_left.right = 23;
    label_left.top = 93;
    label_left.bottom = 113;
    background->BltFast(580, 362, label_sheet, &label_left, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    label_left.left = 0;
    label_left.right = 110;
    label_left.top = 0;
    label_left.bottom = 13;
    background->BltFast(358, 50, label_sheet, &label_left, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    label_left.left = 0;
    label_left.right = 61;
    label_left.top = 14;
    label_left.bottom = 27;
    background->BltFast(360, 102, label_sheet, &label_left, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    label_left.left = 0;
    label_left.right = 124;
    label_left.top = 28;
    label_left.bottom = 44;
    background->BltFast(362, 153, label_sheet, &label_left, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    label_left.left = 0;
    label_left.right = 124;
    label_left.top = 44;
    label_left.bottom = 59;
    background->BltFast(362, 243, label_sheet, &label_left, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    label_left.left = 0;
    label_left.right = 124;
    label_left.top = 59;
    label_left.bottom = 75;
    background->BltFast(361, 293, label_sheet, &label_left, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    label_left.left = 0;
    label_left.right = 70;
    label_left.top = 75;
    label_left.bottom = 89;
    background->BltFast(361, 344, label_sheet, &label_left, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Copies the sign-up form's background onto the menu backdrop for the style.
void MenuGraphics::DrawSignupFormBackground(int x, int y, int style)
{
    TRect rect;
    if (style == 1)
    {
        rect.left = 7;
        rect.right = 80;
        rect.top = 13;
        rect.bottom = 27;
        background->BltFast(362, 303, form_sheet, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    rect.left = 0;
    rect.right = 110;
    rect.top = 0;
    rect.bottom = 13;
    background->BltFast(362, 123, label_sheet, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    rect.left = 0;
    rect.right = 243;
    rect.top = 28;
    rect.bottom = 32;
    background->BltFast(357, 140, form_sheet, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    background->BltFast(357, 210, form_sheet, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    rect.left = 1;
    rect.right = 242;
    rect.top = 30;
    rect.bottom = 97;
    MainForm->blitter->AlphaBlt(358, 143, background, form_sheet, &rect, 94);
    rect.left = 0;
    rect.right = 243;
    rect.top = 28;
    rect.bottom = 32;
    background->BltFast(357, 220, form_sheet, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    background->BltFast(357, 270, form_sheet, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    rect.left = 1;
    rect.right = 242;
    rect.top = 30;
    rect.bottom = 77;
    MainForm->blitter->AlphaBlt(358, 224, background, form_sheet, &rect, 94);
}

// Copies the password reset form's background for the step onto the menu backdrop.
void MenuGraphics::DrawPasswordResetBackground(int x, int y, int step)
{
    TRect rect;
    if (step == 1)
    {
        rect.left = 0;
        rect.right = 110;
        rect.top = 0;
        rect.bottom = 13;
        background->BltFast(362, 283, label_sheet, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    if (step == 2)
    {
        rect.left = 7;
        rect.right = 80;
        rect.top = 13;
        rect.bottom = 27;
        background->BltFast(362, 283, form_sheet, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    if (step == 3)
    {
        rect.left = 0;
        rect.right = 61;
        rect.top = 14;
        rect.bottom = 27;
        background->BltFast(362, 283, label_sheet, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        rect.left = 0;
        rect.right = 124;
        rect.top = 28;
        rect.bottom = 44;
        background->BltFast(360, 333, label_sheet, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    rect.left = 7;
    rect.right = 107;
    rect.top = 1;
    rect.bottom = 13;
    background->BltFast(362, 123, form_sheet, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    rect.left = 0;
    rect.right = 243;
    rect.top = 28;
    rect.bottom = 32;
    background->BltFast(357, 140, form_sheet, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    background->BltFast(357, 240, form_sheet, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    rect.left = 1;
    rect.right = 242;
    rect.top = 30;
    rect.bottom = 127;
    MainForm->blitter->AlphaBlt(358, 143, background, form_sheet, &rect, 94);
}

// Draws the account name typed on the login screen and the password as stars.
void MenuGraphics::DrawLoginText()
{
    int password_length;
    MainForm->fonts->color = 0x8CA0B4;
    MainForm->fonts->face = 1;
    MainForm->fonts->size = 8;
    MainForm->fonts->DrawString(450, 261, MainForm->animation->menu->account_name);
    if (MainForm->animation->menu->password != "")
    {
        String masked_password = MainForm->animation->menu->password;
        String *password_text = &MainForm->animation->menu->password;
        password_length = password_text->Length();
        for (int i = 1; i <= password_length; i++)
            masked_password[i] = '*';
        MainForm->fonts->DrawString(450, 305, masked_password);
    }
}

// Draws each character's name, centred, and the number sent with it in its slot on the character selection screen.
void MenuGraphics::DrawCharacterSlotText()
{
    int name_x;
    MainForm->fonts->color = 0x8CA0B4;
    MainForm->fonts->face = 1;
    MainForm->fonts->size = 8;
    if (MainForm->players->GetCount() >= 1)
    {
        for (int i = 0; i < (int)MainForm->players->GetCount(); i++)
        {
            Player handle = MainForm->players->GetPlayer(i);
            if (!handle.draw_group)
            {
                name_x = 529 - (short)MainForm->fonts->TextWidth(handle.name) / 2;
                if (handle.player_id == PlayerId_SlotFirst)
                {
                    MainForm->fonts->DrawString(name_x, 66, handle.name);
                    MainForm->fonts->DrawString(356, 136, IntToStr(handle.display_number));
                }
                if (handle.player_id == PlayerId_SlotSecond)
                {
                    MainForm->fonts->DrawString(name_x, 190, handle.name);
                    MainForm->fonts->DrawString(356, 260, IntToStr(handle.display_number));
                }
                if (handle.player_id == PlayerId_SlotThird)
                {
                    MainForm->fonts->DrawString(name_x, 313, handle.name);
                    MainForm->fonts->DrawString(356, 384, IntToStr(handle.display_number));
                }
            }
        }
    }
}

// Draws the menu text window's seven visible lines from its scroll position.
void MenuGraphics::DrawListBoxText()
{
    int row;
    int text_x;
    int text_y;
    MainForm->fonts->face = 1;
    MainForm->fonts->size = 8;
    MainForm->fonts->color = 0xC8C8C8;
    int first_index = MainForm->animation->menu->GetScrollOffset();
    int entry_count = MainForm->animation->menu->text_lines->Count;
    row = 0;
    for (int i = first_index; i < entry_count && i < first_index + 7; i++)
    {
        String entry = MainForm->animation->menu->text_lines->Strings[i];
        if (entry.Length() > 0)
        {
            MainForm->fonts->color = 0xC8C8C8;
            text_x = MainForm->animation->menu->text_window_x + 30;
            text_y = MainForm->animation->menu->text_window_y + 16 * row + 70;
            MainForm->fonts->DrawString(text_x, text_y, entry);
        }
        row++;
    }
}

// Draws the menu message box title and two message lines.
// A first line too wide for the box is wrapped at spaces into the lines below it.
void MenuGraphics::DrawMessageBoxText()
{
    int break_at;
    int line_start;
    int line_end;
    MainForm->fonts->color = 0xC8F0F0;
    MainForm->fonts->face = 1;
    MainForm->fonts->size = 10;
    int wrap_width = 250;
    int text_x = 20;
    // A box with an image keeps the text clear of it, narrower.
    if (MainForm->animation->menu->message_shows_image)
    {
        wrap_width = 170;
        text_x = 100;
    }
    if ((short)MainForm->fonts->TextWidth(MainForm->animation->menu->message_lines[0]) > wrap_width)
    {
        String message = MainForm->animation->menu->message_lines[0];
        line_start = 1;
        line_end = 1;
        int line_index = 0;
        do
        {
            if ((short)MainForm->fonts->TextWidth(message.SubString(line_start, line_end)) >= wrap_width ||
                line_end == message.Length())
            {
                char seeking_break = true;
                for (break_at = line_end; break_at >= 1 && seeking_break; break_at--)
                {
                    if (message[break_at] == ' ' || break_at == message.Length())
                    {
                        if (line_index <= MESSAGE_BOX_LINE_COUNT &&
                            message.SubString(line_start, break_at).Length() >= 1)
                        {
                            if (message.SubString(line_start, break_at)[1] == ' ')
                                MainForm->animation->menu->message_lines[line_index] =
                                    message.SubString(line_start + 1, break_at);
                            else
                                MainForm->animation->menu->message_lines[line_index] =
                                    message.SubString(line_start, break_at);
                        }
                        seeking_break = false;
                        line_start = break_at;
                        line_end = break_at;
                        line_end++;
                        line_index++;
                    }
                }
            }
            else
                line_end++;
        } while (message.Length() >= line_end && line_index <= 1);
    }
    MainForm->fonts->DrawString((short)MainForm->animation->menu->message_x + 60,
        (short)MainForm->animation->menu->message_y + 26, MainForm->animation->menu->message_title);
    MainForm->fonts->DrawString((short)MainForm->animation->menu->message_x + text_x,
        (short)MainForm->animation->menu->message_y + 61, MainForm->animation->menu->message_lines[0]);
    MainForm->fonts->DrawString((short)MainForm->animation->menu->message_x + text_x,
        (short)MainForm->animation->menu->message_y + 77, MainForm->animation->menu->message_lines[1]);
}

// Draws the password reset steps, with the account name once it has been entered.
void MenuGraphics::DrawPasswordResetText()
{
    MainForm->fonts->color = 0xDCDCDC;
    MainForm->fonts->face = 1;
    MainForm->fonts->size = 8;
    MainForm->fonts->DrawString(370, 152, "Password reset instructions:");
    if (!MainForm->animation->menu->recover_step)
        MainForm->fonts->DrawString(378, 178, "1. Enter your accountname.");
    else
        MainForm->fonts->DrawString(378, 178, "1. Account: " + MainForm->animation->account_name);
    MainForm->fonts->DrawString(378, 192, "2. Receive an email with 7-digit code.");
    MainForm->fonts->DrawString(378, 206, "3. Enter the code in the next step.");
    MainForm->fonts->DrawString(378, 220, "4. Setup and confirm a new password.");
}

// Sets the email text; text wider than 175 scrolls.
void MenuGraphics::SetEmailText(String email)
{
    email_text = email;
    email_text_set = true;
    MainForm->fonts->TextWidth(email);
    if ((short)MainForm->fonts->TextWidth(email) <= 175)
    {
        email_scrolls = false;
        return;
    }
    email_scrolls = true;
    email_scroll_length = (short)(10 * email.Length()) + 120;
    email_scroll_tick = 0;
}

// Draws the sign-up code notice with the account and the email, a long email scrolling through 26 characters.
void MenuGraphics::DrawSignupCodeText()
{
    MainForm->fonts->color = 0xDCDCDC;
    MainForm->fonts->face = 1;
    MainForm->fonts->size = 8;
    if (!email_text_set)
        SetEmailText(MainForm->animation->account_email);
    if (email_scrolls)
    {
        email_scroll_tick++;
        if (email_scroll_tick > email_scroll_length)
            email_scroll_tick = 0;
        int start = 1;
        if (email_scroll_tick > 59)
        {
            start = (email_scroll_tick - 50) / 10;
            if (start > email_text.Length() - 25)
                start = email_text.Length() - 25;
        }
        MainForm->fonts->DrawString(416, 186, email_text.SubString(start, 26));
    }
    else
        MainForm->fonts->DrawString(416, 186, email_text);
    MainForm->fonts->DrawString(370, 152, "A signup code has been send to your email.");
    MainForm->fonts->DrawString(370, 172, "account: " + MainForm->animation->account_name);
    MainForm->fonts->DrawString(370, 186, "email:");
    MainForm->fonts->DrawString(378, 232, "1. Receive an email with 7-digit code.");
    MainForm->fonts->DrawString(378, 246, "2. Enter the code in the next step.");
}

// Draws the menu text window's scroll arrows, scroll thumb and OK button, each lit under the mouse.
void MenuGraphics::DrawListBoxControls()
{
    if (MainForm->animation->menu->text_scroll_up_hover)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->text_window_x + 320,
            MainForm->animation->menu->text_window_y + 66, MainForm->gfx->menu->scroll_icons,
            &MainForm->gfx->menu->GetScrollIconRect(11), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->text_window_x + 320,
            MainForm->animation->menu->text_window_y + 66, MainForm->gfx->menu->scroll_icons,
            &MainForm->gfx->menu->GetScrollIconRect(10), DDBLTFAST_WAIT);
    if (MainForm->animation->menu->text_scroll_down_hover)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->text_window_x + 320,
            MainForm->animation->menu->text_window_y + 170, MainForm->gfx->menu->scroll_icons,
            &MainForm->gfx->menu->GetScrollIconRect(9), DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->text_window_x + 320,
            MainForm->animation->menu->text_window_y + 170, MainForm->gfx->menu->scroll_icons,
            &MainForm->gfx->menu->GetScrollIconRect(8), DDBLTFAST_WAIT);
    MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->text_window_x + 320,
        MainForm->animation->menu->text_window_y + MainForm->animation->menu->GetScrollbarPosition() + 82,
        MainForm->gfx->menu->scroll_icons, &MainForm->gfx->menu->GetScrollIconRect(7), DDBLTFAST_WAIT);
    if (MainForm->animation->menu->text_window_ok_button_hover)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->text_window_x + 137,
            MainForm->animation->menu->text_window_y + 196, small_buttons, &GetSmallButtonRect(4, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->text_window_x + 137,
            MainForm->animation->menu->text_window_y + 196, small_buttons, &GetSmallButtonRect(4, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the menu message box buttons, the progress bar of a progress box, and the image of a box that has one.
void MenuGraphics::DrawMessageBoxControls()
{
    TRect bar_rect;
    TRect fill_rect;
    int ok_button = 4;
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
        fill_rect.right = MainForm->animation->menu->message_x + MainForm->animation->GetAccountProgress() + 16;
        fill_rect.bottom = MainForm->animation->menu->message_y + 101;
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->message_x + 14,
            MainForm->animation->menu->message_y + 91, progress_bar, &bar_rect, DDBLTFAST_WAIT);
        MainForm->OffscreenSurface->Blt(&fill_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, progress_fill_fx);
    }
    if (MainForm->animation->menu->message_close_button_hover)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->message_x + 181,
            MainForm->animation->menu->message_y + 113, small_buttons, &GetSmallButtonRect(ok_button, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->message_x + 181,
            MainForm->animation->menu->message_y + 113, small_buttons, &GetSmallButtonRect(ok_button, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->message_kind == MessageBoxKind_Confirm)
    {
        if (MainForm->animation->menu->message_confirm_button_hover)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->message_x + 88,
                MainForm->animation->menu->message_y + 113, small_buttons, &GetSmallButtonRect(4, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->message_x + 88,
                MainForm->animation->menu->message_y + 113, small_buttons, &GetSmallButtonRect(4, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    if (MainForm->animation->menu->message_shows_image)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->message_x + 16,
            MainForm->animation->menu->message_y + 56, key_sheet, &message_icon_rect, DDBLTFAST_WAIT);
}

// Draws the change password form's four labels.
void MenuGraphics::DrawChangePasswordText()
{
    if (MainForm->animation->menu->form_kind == CHANGE_PASSWORD_DIALOG && MainForm->fonts->BeginOffscreen())
    {
        try
        {
            MainForm->fonts->color = 0x8CA0B4;
            MainForm->fonts->face = 1;
            MainForm->fonts->size = 8;
            MainForm->fonts->DrawString((short)MainForm->animation->menu->form_x + 40,
                (short)MainForm->animation->menu->form_y + 40, "Account name");
            MainForm->fonts->DrawString((short)MainForm->animation->menu->form_x + 40,
                (short)MainForm->animation->menu->form_y + 78, "Current password");
            MainForm->fonts->DrawString((short)MainForm->animation->menu->form_x + 40,
                (short)MainForm->animation->menu->form_y + 130, "New password");
            MainForm->fonts->DrawString((short)MainForm->animation->menu->form_x + 40,
                (short)MainForm->animation->menu->form_y + 168, "Retype password");
        }
        catch (Exception &)
        {
        }
        MainForm->fonts->End();
    }
}

// Draws the create character form's controls: the skin choices, the hair styles of the page with its arrows, and
// the ok and cancel buttons, each lit when hovered.
void MenuGraphics::DrawCreateCharacterControls()
{
    int skin_marker_x;
    TRect toggle_rect;
    TRect marker_rect;
    TRect lock_rect;
    TRect gender_rect;
    int left_button_x = 218;
    int right_button_x = 311;
    int button_y = 216;
    // Form kind 2 is the create-character form: its head and the arrows of each choice.
    if (MainForm->animation->menu->form_kind == CHARACTER_CREATE_DIALOG)
    {
        left_button_x = 79;
        right_button_x = 172;
        int gender = MainForm->animation->menu->new_gender;
        int hair_style = MainForm->animation->menu->new_hair_style;
        int hair_color = MainForm->animation->menu->new_hair_color;
        int skin = MainForm->animation->menu->new_skin;
        toggle_rect.left = 76;
        toggle_rect.right = 87;
        toggle_rect.top = 14;
        toggle_rect.bottom = 30;
        if (MainForm->animation->menu->preview_hover)
        {
            toggle_rect.left += 11;
            toggle_rect.right += 11;
        }
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 429,
            MainForm->animation->menu->form_y + 46, slot_icons, &toggle_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        marker_rect.left = 66;
        marker_rect.right = 70;
        marker_rect.top = 13;
        marker_rect.bottom = 17;
        lock_rect.left = 72;
        lock_rect.right = 76;
        lock_rect.top = 23;
        lock_rect.bottom = 31;
        gender_rect.left = 32 * gender;
        gender_rect.right = 32 * gender + 32;
        gender_rect.top = 0;
        gender_rect.bottom = 32;
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 30,
            MainForm->animation->menu->form_y + 28, slot_icons, &gender_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        int skin_x = 41;
        int skin_y = 87;
        int skin_step = 17;
        int skin_gap = 5;
        for (int i = 0; i < 12; i++)
        {
            int x = i * skin_step + skin_x + i * skin_gap;
            MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + x,
                MainForm->animation->menu->form_y + skin_y, MainForm->gfx->sprites->stand_sheet,
                &MainForm->gfx->sprites->GetHeadRect(0, i), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            if (i >= MainForm->animation->menu->skin_count)
                MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + x + 16,
                    MainForm->animation->menu->form_y + skin_y + 8, slot_icons, &lock_rect,
                    DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        }
        if (MainForm->animation->menu->skin_count <= skin)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 410,
                MainForm->animation->menu->form_y + 90, slot_icons, &lock_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        skin_marker_x = skin * skin_step + skin_x + skin * skin_gap + 6;
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + skin_marker_x,
            MainForm->animation->menu->form_y + skin_y, slot_icons, &marker_rect,
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (MainForm->animation->menu->hair_page > 0)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 35,
                MainForm->animation->menu->form_y + 137, slot_icons,
                &GetArrowRect(MainForm->animation->menu->left_arrow_state), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (8 * MainForm->animation->menu->hair_page + 8 < MainForm->animation->menu->hair_style_count)
            MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 297,
                MainForm->animation->menu->form_y + 137, slot_icons,
                &GetArrowRect(MainForm->animation->menu->right_arrow_state + 2),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (RenderHairStyles(gender, skin, hair_color, MainForm->animation->menu->hair_page))
            MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 41,
                MainForm->animation->menu->form_y + 127, hair_styles, &hair_styles_rect,
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (8 * MainForm->animation->menu->hair_page < hair_style)
        {
            if (8 * MainForm->animation->menu->hair_page + 8 >= hair_style)
            {
                if (MainForm->animation->menu->hair_style_count >= hair_style)
                {
                    int column = hair_style % 8;
                    if (!column)
                        column = 8;
                    MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + 31 * column + 28,
                        MainForm->animation->menu->form_y + 126, slot_icons, &marker_rect,
                        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                }
            }
        }
        int color_x = 60;
        int color_y = 165;
        int color_step = 17;
        int color_gap = 5;
        int color_marker_x = color_x + hair_color * color_step + hair_color * color_gap + 6;
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + color_marker_x,
            MainForm->animation->menu->form_y + color_y, slot_icons, &marker_rect,
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    if (MainForm->animation->menu->form_ok_button_hover)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + left_button_x,
            MainForm->animation->menu->form_y + button_y, small_buttons, &GetSmallButtonRect(4, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + left_button_x,
            MainForm->animation->menu->form_y + button_y, small_buttons, &GetSmallButtonRect(4, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->form_cancel_button_hover)
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + right_button_x,
            MainForm->animation->menu->form_y + button_y, small_buttons, &GetSmallButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->form_x + right_button_x,
            MainForm->animation->menu->form_y + button_y, small_buttons, &GetSmallButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the page's eight hair styles on a head, by gender, skin and hair colour, for the create character form;
// nothing new when the same page is shown already.
bool MenuGraphics::RenderHairStyles(int gender, int skin, int hair_color, int page)
{
    int male_library;
    int female_library;
    int x;
    TRect hair_rect;
    bool result;
    // Drawn again only when the gender, skin, page or hair colour changed.
    if (gender == shown_gender && shown_skin == skin && shown_page == page && shown_hair_color == hair_color)
        result = true;
    else
    {
        hair_styles->Blt(&hair_styles_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, clear_fx);
        int style = 8 * page;
        int shown = 0;
        male_library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MaleHair);
        female_library = MainForm->gfx_vault->OpenLibrary(GfxLibrary_FemaleHair);
        int male_hair_library = male_library;
        int female_hair_library = female_library;
        for (; MainForm->animation->menu->hair_style_count > style && shown < 8; style++)
        {
            int head_x = 12;
            int head_y = 2 * gender + 7;
            int head_step = 22;
            int head_gap = 9;
            hair_rect.left = 0;
            hair_rect.top = 5;
            hair_rect.right = 28;
            hair_rect.bottom = 32;
            if (!gender)
                hair_rect.bottom += 2;
            int resource_id = 40 * style + 4 * MainForm->animation->menu->new_hair_color + 101;
            if (!gender)
            {
                if (!MainForm->gfx_transfers->LoadResourceBitmap(female_hair_library, resource_id))
                    continue;
            }
            if (gender == 1)
            {
                if (!MainForm->gfx_transfers->LoadResourceBitmap(male_hair_library, resource_id))
                    continue;
            }
            if (MainForm->gfx_transfers->BeginDraw(hair_surface))
            {
                MainForm->gfx_transfers->Draw(hair_surface);
                MainForm->gfx_transfers->EndDraw(hair_surface);
                hair_styles->BltFast(31 * shown + 6, 2 * gender, hair_surface, &hair_rect,
                    DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            }
            x = shown * head_step + head_x + shown * head_gap;
            hair_styles->BltFast(x, head_y, MainForm->gfx->sprites->stand_sheet,
                &MainForm->gfx->sprites->GetHeadAndNeckRect(gender, skin), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            resource_id = 40 * style + 4 * MainForm->animation->menu->new_hair_color + 102;
            if (!gender)
            {
                if (!MainForm->gfx_transfers->LoadResourceBitmap(female_hair_library, resource_id))
                    continue;
            }
            if (gender == 1)
            {
                if (!MainForm->gfx_transfers->LoadResourceBitmap(male_hair_library, resource_id))
                    continue;
            }
            if (MainForm->gfx_transfers->BeginDraw(hair_surface))
            {
                MainForm->gfx_transfers->Draw(hair_surface);
                MainForm->gfx_transfers->EndDraw(hair_surface);
                hair_styles->BltFast(31 * shown + 6, 2 * gender, hair_surface, &hair_rect,
                    DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                shown++;
                if (shown >= 8)
                    break;
            }
        }
        shown_gender = gender;
        shown_skin = skin;
        result = true;
        shown_hair_color = hair_color;
        shown_page = page;
    }
    return result;
}

// Draws the four main menu buttons, each lit under the mouse.
void MenuGraphics::DrawMainMenuButtons()
{
    if (MainForm->animation->menu->create_account_button_hover)
        MainForm->OffscreenSurface->BltFast(25, 280, main_menu_buttons, &GetMainMenuButtonRect(0, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(25, 280, main_menu_buttons, &GetMainMenuButtonRect(0, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->play_button_hover)
        MainForm->OffscreenSurface->BltFast(25, 320, main_menu_buttons, &GetMainMenuButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(25, 320, main_menu_buttons, &GetMainMenuButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->credits_button_hover)
        MainForm->OffscreenSurface->BltFast(25, 360, main_menu_buttons, &GetMainMenuButtonRect(2, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(25, 360, main_menu_buttons, &GetMainMenuButtonRect(2, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->exit_button_hover)
        MainForm->OffscreenSurface->BltFast(25, 400, main_menu_buttons, &GetMainMenuButtonRect(3, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(25, 400, main_menu_buttons, &GetMainMenuButtonRect(3, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the corner close button and the create and cancel buttons of the create-account screen.
void MenuGraphics::DrawAccountCreationButtons()
{
    if (MainForm->animation->menu->corner_close_hover)
        MainForm->OffscreenSurface->BltFast(589, 0, MainForm->gfx->menu->corner_button,
            &MainForm->gfx->menu->GetCornerButtonRect(1), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(589, 0, MainForm->gfx->menu->corner_button,
            &MainForm->gfx->menu->GetCornerButtonRect(0), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->account_create_button_hover)
        MainForm->OffscreenSurface->BltFast(358, 416, wide_buttons, &GetWideButtonRect(0, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(358, 416, wide_buttons, &GetWideButtonRect(0, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->account_cancel_button_hover)
        MainForm->OffscreenSurface->BltFast(480, 416, wide_buttons, &GetWideButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(480, 416, wide_buttons, &GetWideButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the arrow beside the current reset step, the corner close button, and the confirm and cancel buttons, each
// lit while hovered; no confirm button on step 1.
void MenuGraphics::DrawPasswordResetControls()
{
    MainForm->animation->menu->reset_anim_ticks++;
    if (MainForm->animation->menu->reset_anim_ticks > 60)
        MainForm->animation->menu->reset_anim_ticks = 0;
    MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->reset_anim_ticks / 20 + 370,
        14 * MainForm->animation->menu->recover_step + 181, MainForm->gfx->menu->form_sheet, &cursor_rect,
        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->corner_close_hover)
        MainForm->OffscreenSurface->BltFast(589, 0, MainForm->gfx->menu->corner_button,
            &MainForm->gfx->menu->GetCornerButtonRect(1), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(589, 0, MainForm->gfx->menu->corner_button,
            &MainForm->gfx->menu->GetCornerButtonRect(0), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->recover_step != 1)
    {
        if (MainForm->animation->menu->wide_confirm_hover)
            MainForm->OffscreenSurface->BltFast(358, 416, wide_buttons, &GetWideButtonRect(4, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(358, 416, wide_buttons, &GetWideButtonRect(4, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    if (MainForm->animation->menu->wide_cancel_hover)
        MainForm->OffscreenSurface->BltFast(480, 416, wide_buttons, &GetWideButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(480, 416, wide_buttons, &GetWideButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

// Draws the arrow beside the current sign-up step, the corner close button, the confirm button from step 2 and the
// cancel button, each lit while hovered.
void MenuGraphics::DrawSignupCodeControls()
{
    MainForm->animation->menu->signup_anim_ticks++;
    if (MainForm->animation->menu->signup_anim_ticks > 60)
        MainForm->animation->menu->signup_anim_ticks = 0;
    unsigned char row = MainForm->animation->menu->activation_step - 1;
    if (row > 1)
        row = 1;
    MainForm->OffscreenSurface->BltFast(MainForm->animation->menu->signup_anim_ticks / 20 + 369, 14 * row + 235,
        MainForm->gfx->menu->form_sheet, &cursor_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->corner_close_hover)
        MainForm->OffscreenSurface->BltFast(589, 0, MainForm->gfx->menu->corner_button,
            &MainForm->gfx->menu->GetCornerButtonRect(1), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(589, 0, MainForm->gfx->menu->corner_button,
            &MainForm->gfx->menu->GetCornerButtonRect(0), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->activation_step >= 2)
    {
        if (MainForm->animation->menu->wide_confirm_hover)
            MainForm->OffscreenSurface->BltFast(358, 416, wide_buttons, &GetWideButtonRect(0, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(358, 416, wide_buttons, &GetWideButtonRect(0, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
    if (MainForm->animation->menu->wide_cancel_hover)
        MainForm->OffscreenSurface->BltFast(480, 416, wide_buttons, &GetWideButtonRect(1, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(480, 416, wide_buttons, &GetWideButtonRect(1, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
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
        int key_x = 22 * MainForm->animation->menu->grid_column + 294;
        int key_y = 20 * MainForm->animation->menu->grid_row + 250;
        int key_column = MainForm->animation->menu->grid_column;
        int key_row = MainForm->animation->menu->grid_row;
        MainForm->OffscreenSurface->BltFast(key_x, key_y, key_sheet, &GetKeyRect(key_column, key_row), DDBLTFAST_WAIT);
    }
    if (MainForm->animation->menu->up_key_lit)
        MainForm->OffscreenSurface->BltFast(464, 334, key_sheet, &up_key_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->menu->left_key_lit)
        MainForm->OffscreenSurface->BltFast(441, 354, key_sheet, &left_key_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->menu->down_key_lit)
        MainForm->OffscreenSurface->BltFast(464, 354, key_sheet, &down_key_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->menu->right_key_lit)
        MainForm->OffscreenSurface->BltFast(487, 354, key_sheet, &right_key_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->menu->select_key_lit)
        MainForm->OffscreenSurface->BltFast(514, 354, key_sheet, &lower_wide_key_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->menu->space_key_lit)
        MainForm->OffscreenSurface->BltFast(514, 334, key_sheet, &upper_wide_key_rect, DDBLTFAST_WAIT);
    if (MainForm->animation->menu->delete_key_lit)
        MainForm->OffscreenSurface->BltFast(548, 283, key_sheet, &small_key_rect, DDBLTFAST_WAIT);
}

// Draws the login screen's two buttons, each lit while hovered, with the checkbox when the soft keypad is skipped.
void MenuGraphics::DrawConfirmButtons()
{
    if (MainForm->config->GetSkipSoftpad())
    {
        if (MainForm->animation->menu->confirm_ok_hover)
            MainForm->OffscreenSurface->BltFast(360, 394, small_buttons, &GetSmallButtonRect(0, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(360, 394, small_buttons, &GetSmallButtonRect(0, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (MainForm->animation->menu->login_ok_button_hover)
            MainForm->OffscreenSurface->BltFast(452, 394, small_buttons, &GetSmallButtonRect(1, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(452, 394, small_buttons, &GetSmallButtonRect(1, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (MainForm->animation->menu->login_cancel_button_hover)
            MainForm->OffscreenSurface->BltFast(344, 371, checkbox_sheet, &GetCheckboxRect(1), DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(344, 371, checkbox_sheet, &GetCheckboxRect(0), DDBLTFAST_WAIT);
    }
    else
    {
        int confirm_button = 4;
        // On the keypad's password step the first button is the login button.
        if (MainForm->animation->GetSubState() == MenuScreen_PasswordGrid)
            confirm_button = 2;
        if (MainForm->animation->menu->confirm_ok_hover)
            MainForm->OffscreenSurface->BltFast(382, 391, small_buttons, &GetSmallButtonRect(confirm_button, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(382, 391, small_buttons, &GetSmallButtonRect(confirm_button, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        if (MainForm->animation->menu->login_ok_button_hover)
            MainForm->OffscreenSurface->BltFast(474, 391, small_buttons, &GetSmallButtonRect(1, 1),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        else
            MainForm->OffscreenSurface->BltFast(474, 391, small_buttons, &GetSmallButtonRect(1, 0),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    }
}

// Draws the character selection controls: corner close, change password, new character, each slot's login and
// delete buttons, and each character's badge.
void MenuGraphics::DrawCharacterSelect()
{
    int i;
    if (MainForm->animation->menu->corner_close_hover)
        MainForm->OffscreenSurface->BltFast(589, 0, MainForm->gfx->menu->corner_button,
            &MainForm->gfx->menu->GetCornerButtonRect(1), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(589, 0, MainForm->gfx->menu->corner_button,
            &MainForm->gfx->menu->GetCornerButtonRect(0), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->change_password_button_hover)
        MainForm->OffscreenSurface->BltFast(452, 416, wide_buttons, &GetWideButtonRect(3, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(452, 416, wide_buttons, &GetWideButtonRect(3, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->new_character_button_hover)
        MainForm->OffscreenSurface->BltFast(332, 416, wide_buttons, &GetWideButtonRect(0, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(332, 416, wide_buttons, &GetWideButtonRect(0, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->first_slot_login_hover)
        MainForm->OffscreenSurface->BltFast(485, 93, small_buttons, &GetSmallButtonRect(2, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(485, 93, small_buttons, &GetSmallButtonRect(2, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->second_slot_login_hover)
        MainForm->OffscreenSurface->BltFast(485, 217, small_buttons, &GetSmallButtonRect(2, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(485, 217, small_buttons, &GetSmallButtonRect(2, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->third_slot_login_hover)
        MainForm->OffscreenSurface->BltFast(485, 341, small_buttons, &GetSmallButtonRect(2, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(485, 341, small_buttons, &GetSmallButtonRect(2, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->first_slot_delete_hover)
        MainForm->OffscreenSurface->BltFast(485, 121, small_buttons, &GetSmallButtonRect(3, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(485, 121, small_buttons, &GetSmallButtonRect(3, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->second_slot_delete_hover)
        MainForm->OffscreenSurface->BltFast(485, 245, small_buttons, &GetSmallButtonRect(3, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(485, 245, small_buttons, &GetSmallButtonRect(3, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    if (MainForm->animation->menu->third_slot_delete_hover)
        MainForm->OffscreenSurface->BltFast(485, 369, small_buttons, &GetSmallButtonRect(3, 1),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    else
        MainForm->OffscreenSurface->BltFast(485, 369, small_buttons, &GetSmallButtonRect(3, 0),
            DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
    for (i = 0; i < (int)MainForm->players->GetCount(); i++)
    {
        Player handle = MainForm->players->GetPlayer(i);
        if (!handle.draw_group)
        {
            // Badge level one has a badge of its own; the higher levels share another.
            if (handle.badge_level == 1)
            {
                TRect single_badge_rect;
                single_badge_rect.left = 252;
                single_badge_rect.top = 39;
                single_badge_rect.right = 269;
                single_badge_rect.bottom = 55;
                MainForm->OffscreenSurface->BltFast(handle.x + 40, handle.y + 62, slot_icons, &single_badge_rect,
                    DDBLTFAST_WAIT);
            }
            if (handle.badge_level > 1)
            {
                TRect badge_rect;
                badge_rect.left = 233;
                badge_rect.top = 39;
                badge_rect.right = 250;
                badge_rect.bottom = 55;
                MainForm->OffscreenSurface->BltFast(handle.x + 40, handle.y + 62, slot_icons, &badge_rect,
                    DDBLTFAST_WAIT);
            }
        }
    }
}
