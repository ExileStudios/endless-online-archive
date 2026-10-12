#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Gfx_sprites.h"
#pragma package(smart_init)

#define MAP_ITEM_REMOVAL_TICKS 6

MapItems::MapItems()
{
    hover_label_shown = 0;
}

MapItems::~MapItems()
{
}

// Shows the name label of the item on the cursor tile, with its amount; player and npc labels take precedence.
void MapItems::UpdateHoverLabel()
{
    if (!MainForm->players->hover_label_shown && !MainForm->npcs->hover_label_shown)
    {
        char first_match = 1;
        for (vector<MapItem>::reverse_iterator it(items.end()); it != vector<MapItem>::reverse_iterator(items.begin());
            it++)
        {
            if (MainForm->map->GetCursorX() == (*it).x && MainForm->map->GetCursorY() == (*it).y)
            {
                if (first_match)
                {
                    if (!(*it).label_shown)
                    {
                        ItemValue handle = MainForm->item_values->GetItem((*it).item_id);
                        if ((*it).amount > 1)
                        {
                            // Gold, item 1, shows its amount first; other stacks show it after the name.
                            if ((*it).item_id == 1)
                            {
                                String label_text = IntToStr((*it).amount) + " " + handle.name;
                                MainForm->gfx->sprites->ShowNameLabel(label_text, handle.special + 2);
                            }
                            else
                            {
                                String stack_label = handle.name + " x" + IntToStr((*it).amount);
                                MainForm->gfx->sprites->ShowNameLabel(stack_label, handle.special + 2);
                            }
                        }
                        else if ((*it).item_id == 1)
                        {
                            String single_gold_label = "1 " + handle.name;
                            MainForm->gfx->sprites->ShowNameLabel(single_gold_label, handle.special + 2);
                        }
                        else
                            MainForm->gfx->sprites->ShowNameLabel(handle.name, handle.special + 2);
                        (*it).label_shown = 1;
                        MainForm->redraw_needed = true;
                    }
                    first_match = 0;
                }
                else if ((*it).label_shown)
                {
                    (*it).label_shown = 0;
                    MainForm->redraw_needed = true;
                }
            }
            else if ((*it).label_shown)
            {
                (*it).label_shown = 0;
                MainForm->redraw_needed = true;
            }
        }
        if (first_match)
            hover_label_shown = 0;
        else
            hover_label_shown = 1;
    }
    else
    {
        for (vector<MapItem>::reverse_iterator it(items.end()); it != vector<MapItem>::reverse_iterator(items.begin());
            it++)
        {
            if ((*it).label_shown)
            {
                (*it).label_shown = 0;
                MainForm->redraw_needed = true;
            }
        }
        hover_label_shown = 0;
    }
}

// Counts protection down, and drops items that stay out of view; the out-of-view count waits during a warp.
void MapItems::RemoveDistantItems(int center_x, int center_y)
{
    list_changed = 0;
    vector<MapItem>::iterator it;
    for (it = protected_items.begin(); it != protected_items.end(); it++)
    {
        if (it->protection_ticks > 0)
        {
            it->protection_ticks--;
            list_changed = 1;
        }
    }
    if (!list_changed)
    {
        if (protected_items.size() > 0)
            protected_items.clear();
    }
    do
    {
        list_changed = 0;
        for (it = items.begin(); it != items.end(); it++)
        {
            if (it->protection_ticks > 0)
                it->protection_ticks--;
            if (!IsInViewRange(center_x, center_y, it->x, it->y))
            {
                if (!it->out_of_range)
                {
                    it->out_of_range = 1;
                    it->removal_countdown = 4;
                }
                else
                {
                    if (!MainForm->map->warp_pending)
                        it->removal_countdown--;
                    if (it->removal_countdown <= 0)
                    {
                        items.erase(it);
                        list_changed = 1;
                        break;
                    }
                }
            }
            else
                it->out_of_range = 0;
        }
    } while (list_changed);
}

// Sorts the items by their position on the map.
void MapItems::SortByPosition()
{
    char swapped;
    do
    {
        swapped = 0;
        for (int i = 0; i < (int)GetCount() - 1; i++)
        {
            MapItem handle;
            if (items[i].y == items[i + 1].y && items[i].x > items[i + 1].x)
            {
                handle = items[i];
                items[i] = items[i + 1];
                items[i + 1] = handle;
                swapped = 1;
            }
            if (items[i].y > items[i + 1].y)
            {
                handle = items[i];
                items[i] = items[i + 1];
                items[i + 1] = handle;
                swapped = 1;
            }
        }
    } while (swapped);
}

// Adds an item, unless one with the uid is on the map.
void MapItems::AddItem(int uid, int item_id, int x, int y, int amount)
{
    if (!HasItem(uid))
    {
        MapItem handle(uid, item_id, x, y, amount);
        handle.removal_countdown = MAP_ITEM_REMOVAL_TICKS;
        items.insert(items.end(), handle);
    }
}

// Adds an item that owner_id holds for protection_ticks, to both lists.
void MapItems::AddProtectedItem(int uid, int item_id, int x, int y, int amount, int owner_id, int protection_ticks)
{
    if (!HasItem(uid))
    {
        MapItem handle(uid, item_id, x, y, amount);
        handle.owner_id = owner_id;
        handle.protection_ticks = protection_ticks;
        handle.removal_countdown = MAP_ITEM_REMOVAL_TICKS;
        items.insert(items.end(), handle);
        protected_items.insert(protected_items.end(), handle);
    }
}

void MapItems::RemoveItem(int uid)
{
    for (vector<MapItem>::iterator it = items.begin(); it != items.end(); it++)
    {
        if (uid == it->uid)
        {
            items.erase(it);
            MainForm->redraw_needed = true;
            break;
        }
    }
}

MapItem MapItems::GetItem(int index)
{
    return items[index];
}

// The owner protecting an item on the tile, or -1; the protected list is searched when the map list gives none.
int MapItems::GetProtectionOwnerAt(int x, int y)
{
    int owner_id = -1;
    vector<MapItem>::iterator it;
    for (it = items.begin(); it != items.end(); it++)
    {
        if (x == it->x && y == it->y)
        {
            if (it->protection_ticks > 0)
                owner_id = it->owner_id;
        }
    }
    if (owner_id == -1)
    {
        for (it = protected_items.begin(); it != protected_items.end(); it++)
        {
            if (x == it->x && y == it->y && it->protection_ticks > 0)
                owner_id = it->owner_id;
        }
    }
    return owner_id;
}

// The owner protecting the item with the uid, or -1.
int MapItems::GetProtectionOwner(int uid)
{
    int owner_id = -1;
    vector<MapItem>::iterator it;
    for (it = items.begin(); it != items.end(); it++)
    {
        if (uid == it->uid)
        {
            if (it->protection_ticks > 0)
                owner_id = it->owner_id;
            break;
        }
    }
    if (owner_id == -1)
    {
        for (it = protected_items.begin(); it != protected_items.end(); it++)
        {
            if (uid == it->uid && it->protection_ticks > 0)
                owner_id = it->owner_id;
        }
    }
    return owner_id;
}

bool MapItems::HasItem(int uid)
{
    bool found = 0;
    for (vector<MapItem>::iterator it = items.begin(); it != items.end(); it++)
    {
        if (uid == it->uid)
        {
            found = 1;
            break;
        }
    }
    return found;
}

bool MapItems::HasItemAt(int x, int y)
{
    bool found = 0;
    vector<MapItem>::iterator it;
    for (it = items.begin(); it != items.end(); it++)
    {
        if (x != it->x)
            continue;
        if (y == it->y)
        {
            found = 1;
            break;
        }
    }
    return found;
}

// The record id of the item with the uid, or -1.
int MapItems::GetItemId(int uid)
{
    int item_id = -1;
    vector<MapItem>::iterator it;
    for (it = items.begin(); it != items.end(); it++)
    {
        if (uid == it->uid)
        {
            item_id = it->item_id;
            break;
        }
    }
    return item_id;
}

// The uid of the last item listed on the tile, or -1.
int MapItems::FindItemAt(int x, int y)
{
    int uid = -1;
    vector<MapItem>::iterator it;
    for (it = items.begin(); it != items.end(); it++)
    {
        if (x == it->x && y == it->y)
            uid = it->uid;
    }
    return uid;
}

// Empties both lists, when the map list has items.
void MapItems::ClearAll()
{
    if (GetCount() >= 1)
    {
        protected_items.clear();
        items.clear();
        current_index = -1;
    }
}

// Empties the map list; the protected list stays.
void MapItems::ClearKeepingProtection()
{
    if (GetCount() >= 1)
    {
        items.clear();
        current_index = -1;
    }
}

unsigned int MapItems::GetCount()
{
    return items.size();
}

// True when the tile is within the item view range of the centre.
bool MapItems::IsInViewRange(int center_x, int center_y, int x, int y)
{
    bool in_range = 0;
    int dx = x - center_x;
    int dy = y - center_y;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (dx + dy <= 12)
        in_range = 1;
    return in_range;
}

void MapItems::SetMousePosition(int cursor_x, int cursor_y)
{
    mouse_x = cursor_x;
    mouse_y = cursor_y;
}
