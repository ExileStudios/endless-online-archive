#include <vcl.h>
#pragma hdrstop

#include "Ctrl_playerlist.h"
#include "Tmainform.h"
#include "Ctrl_chat.h"
#include "Ctrl_interface.h"
#include "Ctrl_social.h"
#include "Ctrl_status.h"
#pragma package(smart_init)

#define PLAYERLIST_VISIBLE_NAMES 7

PlayerlistController::PlayerlistController()
{
    filter_mode = PlayerFilter_All;
    shown_record_values = new TStringList();
    shown_icon_types = new TStringList();
    shown_names = new TStringList();
    shown_second_texts = new TStringList();
    shown_guild_tags = new TStringList();
    shown_class_names = new TStringList();
    all_record_values = new TStringList();
    all_icon_types = new TStringList();
    all_names = new TStringList();
    all_second_texts = new TStringList();
    all_guild_tags = new TStringList();
    all_class_names = new TStringList();
    scroll_up_hovered = 0;
    scroll_down_hovered = 0;
    scroll_dirty = 0;
    list_dirty = 0;
    top_line = 0;
    scroll_thumb_offset = 0;
    scroll_up_held = 0;
    scroll_down_held = 0;
}

// Moves to the next filter; the guild filter is offered only to a character in a guild. Scrolls back to the top.
void PlayerlistController::CycleFilter()
{
    filter_mode++;
    if (MainForm->animation->interface_controller->status->IsInGuild())
    {
        if (filter_mode > 3)
            filter_mode = PlayerFilter_All;
    }
    else if (filter_mode > 2)
        filter_mode = PlayerFilter_All;
    top_line = 0;
    scroll_thumb_offset = 0;
}

void PlayerlistController::SetFilter(int mode)
{
    filter_mode = mode;
}

int PlayerlistController::GetFilter()
{
    return filter_mode;
}

// Empties the full and the shown lists and scrolls back to the top.
void PlayerlistController::Clear()
{
    top_line = 0;
    scroll_thumb_offset = 0;
    shown_record_values->Clear();
    shown_icon_types->Clear();
    shown_names->Clear();
    shown_second_texts->Clear();
    shown_guild_tags->Clear();
    shown_class_names->Clear();
    all_record_values->Clear();
    all_icon_types->Clear();
    all_names->Clear();
    all_second_texts->Clear();
    all_guild_tags->Clear();
    all_class_names->Clear();
}

// Rebuilds the shown list from the full list with the current filter.
// Each full-list entry starts with the player's name, so after sorting the lists line up by index; the name prefix
// is cut off the shown entries.
void PlayerlistController::ApplyFilter()
{
    int prefix_length;
    char social_match;
    char icon_match;
    char same_guild;
    int shown_prefix_length;
    int i;
    int j;
    shown_record_values->Clear();
    shown_icon_types->Clear();
    shown_names->Clear();
    shown_second_texts->Clear();
    shown_guild_tags->Clear();
    shown_class_names->Clear();
    all_record_values->Sort();
    all_icon_types->Sort();
    all_names->Sort();
    all_second_texts->Sort();
    all_guild_tags->Sort();
    all_class_names->Sort();
    for (i = 0; i < all_names->Count; i++)
    {
        String entry_value = "";
        String name = all_names->Strings[i];
        prefix_length = name.Length() + 1;
        social_match = 0;
        icon_match = 0;
        same_guild = 0;
        if (filter_mode == PlayerFilter_Friends && MainForm->animation->social->IsFriend(name))
            social_match = 1;
        // This filter keeps players with certain icons
        if (filter_mode == PlayerFilter_Marked)
        {
            entry_value = all_icon_types->Strings[i];
            entry_value.Delete(1, prefix_length);
            if (entry_value == "4" || entry_value == "5" || entry_value == "9" || entry_value == "10")
                icon_match = 1;
        }
        if (filter_mode == PlayerFilter_Guild)
        {
            entry_value = all_guild_tags->Strings[i];
            entry_value.Delete(1, prefix_length);
            String guild_tag = LowerCase(entry_value);
            String own_guild_tag = LowerCase(MainForm->animation->interface_controller->status->guild_tag);
            // A two-letter tag padded with a space matches its unpadded form
            if (guild_tag.Length() == 3)
            {
                if (guild_tag[3] == ' ')
                    guild_tag = guild_tag.SubString(1, 2);
            }
            if (own_guild_tag.Length() == 3)
            {
                if (own_guild_tag[3] == ' ')
                    own_guild_tag = own_guild_tag.SubString(1, 2);
            }
            if (guild_tag == own_guild_tag)
                same_guild = 1;
        }
        if ((filter_mode == PlayerFilter_All) || social_match || icon_match || same_guild)
        {
            shown_record_values->Add(all_record_values->Strings[i]);
            shown_icon_types->Add(all_icon_types->Strings[i]);
            shown_names->Add(all_names->Strings[i]);
            shown_second_texts->Add(all_second_texts->Strings[i]);
            shown_guild_tags->Add(all_guild_tags->Strings[i]);
            shown_class_names->Add(all_class_names->Strings[i]);
        }
    }
    for (j = 0; j < shown_names->Count; j++)
    {
        String entry = "";
        String shown_name = shown_names->Strings[j];
        shown_prefix_length = shown_name.Length() + 1;
        entry = shown_record_values->Strings[j];
        entry.Delete(1, shown_prefix_length);
        shown_record_values->Strings[j] = entry;
        entry = shown_icon_types->Strings[j];
        entry.Delete(1, shown_prefix_length);
        shown_icon_types->Strings[j] = entry;
        entry = shown_second_texts->Strings[j];
        entry.Delete(1, shown_prefix_length);
        shown_second_texts->Strings[j] = entry;
        entry = shown_guild_tags->Strings[j];
        entry.Delete(1, shown_prefix_length);
        shown_guild_tags->Strings[j] = entry;
        entry = shown_class_names->Strings[j];
        entry.Delete(1, shown_prefix_length);
        shown_class_names->Strings[j] = entry;
    }
}

// Adds a player to the full list; empty texts show as a dash.
void PlayerlistController::AddPlayer(String record_value, int icon_type, String name, String second_text,
    String guild_tag, String class_name)
{
    all_record_values->Add(name + " " + record_value);
    all_icon_types->Add(name + " " + IntToStr(icon_type));
    all_names->Add(Capitalize(this, name));
    if (second_text.Length() >= 1)
        this->all_second_texts->Add(name + " " + PlayerlistController::Capitalize(this, second_text));
    else
        all_second_texts->Add(name + " -");
    if (guild_tag.Length() >= 1)
        this->all_guild_tags->Add(name + " " + PlayerlistController::Capitalize(this, guild_tag));
    else
        all_guild_tags->Add(name + " -");
    if (class_name.Length() >= 1)
        this->all_class_names->Add(name + " " + PlayerlistController::Capitalize(this, class_name));
    else
        all_class_names->Add(name + " -");
}

// Empties the shown lists.
PlayerlistController::~PlayerlistController()
{
    shown_record_values->Clear();
    shown_icon_types->Clear();
    shown_names->Clear();
    shown_second_texts->Clear();
    shown_guild_tags->Clear();
    shown_class_names->Clear();
}

int PlayerlistController::GetTopLine()
{
    return top_line;
}

int PlayerlistController::GetScrollThumbOffset()
{
    return scroll_thumb_offset;
}

// Repeats a held scroll arrow.
DEFINE_TICK(PlayerlistController)

// Scrolls the list up one line and moves the scroll thumb to match.
void PlayerlistController::ScrollUp()
{
    if (shown_names->Count > PLAYERLIST_VISIBLE_NAMES && top_line > 0)
    {
        top_line--;
        double scroll_fraction = (double)top_line / (double)(shown_names->Count - 7);
        double thumb_pos = PANEL_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_dirty = 1;
        list_dirty = 1;
    }
}

// Scrolls the list down one line and moves the scroll thumb to match.
void PlayerlistController::ScrollDown()
{
    if (shown_names->Count - 7 > top_line)
    {
        top_line++;
        double scroll_fraction = (double)top_line / (double)(shown_names->Count - 7);
        double thumb_pos = PANEL_THUMB_TRAVEL * scroll_fraction;
        scroll_thumb_offset = thumb_pos;
        scroll_dirty = 1;
        list_dirty = 1;
    }
}

// Follows a grabbed scroll thumb, kept on its track, and picks the top line from its position.
void PlayerlistController::DragScrollThumb(int y)
{
    double thumb_fraction;
    double scaled_line;
    int __slot1;
    if (thumb_grabbed)
    {
        int thumb_offset = y - thumb_grab_offset;
        if (thumb_offset < 0)
            thumb_offset = 0;
        if (thumb_offset > 50)
            thumb_offset = 50;
        if (shown_names->Count > PLAYERLIST_VISIBLE_NAMES)
        {
            thumb_fraction = thumb_offset / PANEL_THUMB_TRAVEL;
            scaled_line = (shown_names->Count - 7.0) * thumb_fraction;
            int new_top_line = scaled_line;
            if (new_top_line != top_line)
                top_line = new_top_line;
        }
        scroll_thumb_offset = thumb_offset;
        scroll_dirty = 1;
        list_dirty = 1;
    }
}

// A right-click on a name starts a private message to that player in the chat edit; the filter button moves to
// the next filter; also the scroll bar.
void PlayerlistController::MouseDown(int x, int y, int button)
{
    if (button == 1)
    {
        if (x >= 127 && x <= 560 && y >= 347 && y <= 444)
        {
            int row = top_line + (y - 347) / 14;
            if (shown_names->Count > row)
            {
                if (shown_names->Strings[row] != "")
                {
                    MainForm->edits->SetText(EditId_Chat, "!" + shown_names->Strings[row] + " ");
                    MainForm->edits->HasDirtyEdit();
                }
            }
        }
    }
    if (x >= 100 && x <= 115 && y >= 332 && y <= 347)
    {
        CycleFilter();
        ApplyFilter();
        scroll_dirty = 1;
        list_dirty = 1;
        MainForm->sfx->PlaySfx(3);
    }
    if (x >= 569 && x <= 585)
    {
        if (y >= scroll_thumb_offset + 366 && y <= scroll_thumb_offset + 381)
        {
            thumb_grabbed = 1;
            thumb_grab_offset = y - scroll_thumb_offset;
            DragScrollThumb(y);
        }
        if (y >= 350 && y <= 365)
        {
            scroll_up_held = 1;
            scroll_repeat_delay = 4;
            ScrollUp();
        }
        if (y >= 432 && y <= 447)
        {
            scroll_down_held = 1;
            scroll_repeat_delay = 4;
            ScrollDown();
        }
    }
}

// Lights the scroll arrow under the mouse, lets go of an arrow the mouse has left, and drags a grabbed thumb.
DEFINE_HELP_UPDATE_SCROLL_HOVER(PlayerlistController)

// Lets go of the held scroll arrows and the grabbed thumb.
void PlayerlistController::MouseUp()
{
    scroll_up_held = 0;
    scroll_down_held = 0;
    thumb_grabbed = 0;
}

void PlayerlistController::MouseMove(int x, int y)
{
    UpdateScrollHover(x, y);
}

DEFINE_TEST_AND_CLEAR_SCROLL_DIRTY(PlayerlistController)

DEFINE_TEST_AND_CLEAR_LIST_DIRTY(PlayerlistController)

// The text with its first letter in upper case.
DEFINE_CAPITALIZE(PlayerlistController)

void PlayerlistController::MarkListDirty()
{
    list_dirty = 1;
}
