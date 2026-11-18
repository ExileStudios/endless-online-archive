#include <vcl.h>
#pragma hdrstop

#include "Ctrl_notification.h"
#include "Tmainform.h"
#include "Ctrl_paperdoll.h"
#include "Gfx_game.h"
#include "Gfx_icon.h"
#include "Gfx_interface.h"
#include "Gfx_mapicons.h"
#include "Gfx_mapitems.h"
#include "Gfx_subitem.h"

#pragma package(smart_init)

#define STATUS_MARK_BACKING 0

Notification::Notification(int left, int top)
{
    x = left;
    y = top;
    Hide();
}

Notification::~Notification()
{
}

// Shows the notice when its delay runs out and hides it six ticks later.
void Notification::Tick()
{
    if (show_delay > 0)
    {
        show_delay--;
        if (show_delay <= 0)
        {
            MainForm->redraw_needed = true;
            hide_countdown = 6;
            is_visible = true;
        }
    }
    if (hide_countdown > 0)
    {
        hide_countdown--;
        if (hide_countdown <= 0)
        {
            MainForm->redraw_needed = true;
            Hide();
        }
    }
}

void Notification::Notify(int new_kind, int new_id)
{
    NotifyWithStatus(new_kind, new_id, NotifyStatus_Ready);
}

// Shows the hotkey notice of an item or a skill with its status, an item's healing amounts included; nothing when the
// hotkey notices are off.
void Notification::NotifyWithStatus(int new_kind, int new_id, int new_status)
{
    ItemValue *item;
    SkillValue *spell;
    // Nothing shows while hotkey notifications are turned off.
    if (!MainForm->config->GetHotkeyNotifications())
        return;
    amount_kind = 0;
    amount_text = "";
    second_amount_text = "";
    kind = -1;
    status = new_status;
    if (new_kind == NotifyKind_Item)
    {
        if (new_id <= 0)
            return;
        if ((unsigned int)MainForm->item_values->Count() < new_id)
            return;
        item = MainForm->item_values->GetItem(new_id - 1);
        icon_size = item->size;
        graphic = item->graphic - 1;
        MainForm->gfx->items->RequestSubItemGraphic(graphic);
        if (!new_status)
        {
            if (item->type == EifItemType_Heal)
            {
                if (item->tp > 0 && item->hp > 0)
                {
                    amount_text = FormatAmount(item->hp, "+");
                    second_amount_text = FormatAmount(item->tp, "+");
                    amount_kind = 1;
                }
                else if (item->tp > 0)
                {
                    amount_text = FormatAmount(item->tp, "+");
                    amount_kind = 2;
                }
                else if (item->hp > 0)
                {
                    amount_text = FormatAmount(item->hp, "+");
                    amount_kind = 1;
                }
            }
            if (item->type == EifItemType_ExpReward)
                amount_text = FormatAmount(item->spec_value, "+");
        }
    }
    if (new_kind == NotifyKind_Spell)
    {
        if (new_id < 0)
            return;
        if ((unsigned int)MainForm->skill_values->Count() < new_id)
            return;
        spell = MainForm->skill_values->GetSkill(new_id - 1);
        graphic = spell->icon;
        MainForm->gfx->icons->RequestGraphic(graphic);
    }
    kind = new_kind;
    id = new_id;
    if (is_visible)
    {
        Hide();
        show_delay = 2;
        return;
    }
    MainForm->redraw_needed = true;
    hide_countdown = 6;
    is_visible = true;
}

// The amount after the prefix, in thousands with a K from 10000; empty when negative.
String Notification::FormatAmount(int amount, String prefix)
{
    if (amount < 0)
        return "";
    if (amount < 10000)
        return prefix + IntToStr(amount);
    return prefix + IntToStr(amount / 1000) + "K";
}

void Notification::Hide()
{
    MainForm->redraw_needed = true;
    hide_countdown = 0;
    show_delay = 0;
    is_visible = false;
}

bool Notification::IsVisible()
{
    return is_visible;
}

// Draws the hotkey notice: its frame, the item's picture or the skill's icon, the amounts, and the status mark over
// it.
void Notification::Draw()
{
    int icon_pos;
    int mark_x;
    IconWrapper *icon;
    int spell_mark_x;
    SubItemWrapper *picture;
    int icon_x;
    int mark_y;
    int icon_y;
    int frame;
    int spell_mark_y;
    if (is_visible && kind >= 0)
    {
        if (second_amount_text.Length() > 0)
        {
            MainForm->gfx->game->BlitClippedAlpha(x, y, MainForm->gfx->interface_graphics->hud_icons,
                MainForm->gfx->interface_graphics->notification_top_rect, 70);
            MainForm->gfx->game->BlitClippedAlpha(x, y + 45, MainForm->gfx->interface_graphics->hud_icons,
                MainForm->gfx->interface_graphics->notification_bottom_rect, 70);
            TRect divider;
            divider.top = 49;
            divider.bottom = 50;
            divider.left = 52;
            divider.right = 68;
            MainForm->gfx->game->BlitClipped(x + 16, y + 45, MainForm->gfx->interface_graphics->hud_icons, divider);
        }
        else
            MainForm->gfx->game->BlitClippedAlpha(x, y, MainForm->gfx->interface_graphics->hud_icons,
                MainForm->gfx->interface_graphics->notification_rect, 70);
        // Kind 1 shows an item's picture, kind 2 a spell's icon.
        if (kind == NotifyKind_Item)
        {
            if (MainForm->gfx->items->RequestSubItemGraphic(graphic))
            {
                icon_x = x + 13;
                icon_pos = y + 12;
                if (amount_text.Length() > 0)
                    icon_pos -= 7;
                if (icon_size == 2 || icon_size == 5)
                    icon_pos -= 11;
                if (icon_size > 3)
                    icon_x -= 11;
                picture = MainForm->gfx->items->GetSubItemGraphic(graphic);
                if (picture)
                    MainForm->OffscreenSurface->BltFast(icon_x + 8, icon_pos + 9, picture->resource->surface,
                        &picture->resource->surface_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                if (status > 0 && status < 4)
                {
                    mark_x = 13;
                    mark_y = 15;
                    if (icon_size == 2 || icon_size == 5)
                        mark_y += 7;
                    if (icon_size > 3)
                        mark_x = mark_x + 7;
                    MainForm->gfx->game->BlitClippedAlpha(icon_x + mark_x, mark_y + icon_pos,
                        MainForm->gfx->interface_graphics->hud_icons,
                        MainForm->gfx->interface_graphics->GetInventoryWideButtonRect(STATUS_MARK_BACKING), 150);
                    MainForm->OffscreenSurface->BltFast(icon_x + mark_x + 9, mark_y + icon_pos + 8,
                        MainForm->gfx->interface_graphics->hud_icons,
                        &MainForm->gfx->interface_graphics->GetInventoryWideButtonRect(status),
                        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                }
                if (amount_text.Length() > 0)
                    DrawAmounts();
            }
        }
        if (kind == NotifyKind_Spell)
        {
            if (MainForm->gfx->icons->RequestGraphic(graphic))
            {
                icon_pos = x + 9;
                icon_y = y + 8;
                frame = 1;
                if (status > 0)
                    frame = 0;
                icon = MainForm->gfx->icons->GetGraphic(graphic);
                if (icon)
                    MainForm->OffscreenSurface->BltFast(icon_pos + 8, icon_y + 9, icon->resource->surface,
                        &icon->resource->GetIconRect(frame), DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                if (status > 0 && status < 4)
                {
                    spell_mark_x = 17;
                    spell_mark_y = 19;
                    MainForm->gfx->game->BlitClippedAlpha(icon_pos + spell_mark_x, icon_y + spell_mark_y,
                        MainForm->gfx->interface_graphics->hud_icons,
                        MainForm->gfx->interface_graphics->GetInventoryWideButtonRect(STATUS_MARK_BACKING), 150);
                    MainForm->OffscreenSurface->BltFast(icon_pos + spell_mark_x + 9, icon_y + spell_mark_y + 8,
                        MainForm->gfx->interface_graphics->hud_icons,
                        &MainForm->gfx->interface_graphics->GetInventoryWideButtonRect(status),
                        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                }
            }
        }
    }
}

// Draws the notice's amount centred under its picture, coloured by its kind, and a second amount below it when there
// is one.
void Notification::DrawAmounts()
{
    int text_width;
    int second_amount_width;
    if (MainForm->fonts->BeginOffscreenIf(201))
    {
        try
        {
            MainForm->fonts->color = 0xDCDCDC;
            MainForm->fonts->face = 1;
            MainForm->fonts->size = 8;
            int text_x = x + 32;
            int text_y = y + 41;
            text_width = (short)MainForm->fonts->TextWidth(amount_text);
            text_x -= text_width / 2;
            if (amount_kind == 1)
                MainForm->fonts->color = 0xD4E0D2;
            if (amount_kind == 2)
                MainForm->fonts->color = 0xE0C8BE;
            MainForm->fonts->DrawString(text_x, text_y, amount_text);
            if (second_amount_text.Length() > 0)
            {
                text_x = x + 32;
                text_y += 12;
                second_amount_width = (short)MainForm->fonts->TextWidth(second_amount_text);
                text_x -= second_amount_width / 2;
                MainForm->fonts->color = 0xE0C8BE;
                MainForm->fonts->DrawString(text_x, text_y, second_amount_text);
            }
        }
        catch (Exception &)
        {
        }
        MainForm->fonts->End();
    }
}
