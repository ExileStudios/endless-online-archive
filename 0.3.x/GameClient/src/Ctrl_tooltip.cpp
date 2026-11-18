#include <vcl.h>
#pragma hdrstop

#include "Ctrl_tooltip.h"
#include "Tmainform.h"
#include "Ctrl_paperdoll.h"
#include "Gfx_interface.h"

#pragma package(smart_init)

// An empty tip at left, top; fixed is the number of its fixed lines.
Tooltip::Tooltip(int left, int top, int fixed)
{
    x = left;
    y = top;
    shown_id = -1;
    fixed_lines = fixed;
    anchor_x = 0;
    anchor_y = 0;
    flip_edge_x = 0;
    bottom_limit = 0;
    flip_gap = 0;
    bottom_margin = 0;
    flip_extra = 0;
    bottom_extra = 0;
    raise = 0;
    vertical_align = 0;
    pad_x = 10;
    pad_y = 4;
    colors[0] = 0xE1E1E1;
    color_1 = 0xE6B791;
    color_2 = 0xD99CD9;
    color_3 = 0x5AC8FA;
    color_4 = 0x46AAFA;
    color_5 = 0xB0BCC0;
    color_6 = 0xBEBEBE;
    color_7 = 0xAAC8BE;
    color_8 = 0x9191F5;
    color_9 = 0xF0BEF5;
    default_color = 0;
    show_cursed = false;
    ClearLines();
}

Tooltip::~Tooltip()
{
}

bool Tooltip::Hide()
{
    if (line_count > 0)
    {
        line_count = 0;
        return true;
    }
    return false;
}

void Tooltip::ClearLines()
{
    for (int i = 0; i < 9; i++)
    {
        lines[i] = "";
        line_colors[i] = 6;
    }
    scroll_offset = 0;
    line_count = 0;
}

void Tooltip::ShowItem(int item_id)
{
    ShowItemCount(item_id, 1);
}

// Builds the tip of an item, with the amount when more than one: its name and kind, and with the extended tips on,
// its stats, the class it asks for, its elements and whether it is cursed.
void Tooltip::ShowItemCount(int item_id, int count)
{
    if (item_id <= 0)
    {
        ClearLines();
        return;
    }
    if (MainForm->item_values->Count() < item_id)
        return;
    if (line_count > 0 && item_id == shown_id)
        return;
    shown_id = item_id;
    ClearLines();
    ItemValue item = MainForm->item_values->GetItem(item_id);
    int type = item.type;
    String item_name = item.name;
    if (item_name.Length() < 1)
        item_name = "NoName-" + IntToStr(item.item_id);
    if (count > 1)
    {
        if (type == EifItemType_General)
            lines[line_count] = IntToStr(count) + " " + item_name;
        if (type == EifItemType_Currency)
            lines[line_count] = IntToStr(count) + " " + item_name;
        else
            lines[line_count] = item_name + " x" + IntToStr(count);
    }
    else
    {
        if (type == EifItemType_General)
            lines[line_count] = IntToStr(count) + " " + item_name;
        if (type == EifItemType_Currency)
            lines[line_count] = IntToStr(count) + " " + item_name;
        else
            lines[line_count] = item_name;
    }
    if (MainForm->config->GetElementalIcons())
        element = item.element;
    else
        element = 0;
    line_colors[line_count] = item.special;
    if (line_colors[line_count] > 5)
        line_colors[line_count] = 0;
    if (show_cursed && item.special == 5)
        line_colors[line_count] = 8;
    line_count++;
    if (!MainForm->config->GetExtendedTooltip())
    {
        Measure();
        return;
    }
    // Types 10 to 21 are worn items; type 12, armour, shows the gender it is made for.
    if (type <= EifItemType_General)
        lines[line_count] += "general item";
    else if (type == EifItemType_Currency)
        lines[line_count] += "currency";
    else if (type == EifItemType_Heal)
        lines[line_count] += "potion";
    else if (type == EifItemType_Teleport)
        lines[line_count] += "teleport";
    else if (type == EifItemType_Spell)
        lines[line_count] += "spell";
    else if (type == EifItemType_ExpReward)
        lines[line_count] += "exp reward";
    else if (type == EifItemType_StatReward)
        lines[line_count] += "stat reward";
    else if (type == EifItemType_SkillReward)
        lines[line_count] += "skill reward";
    else if (type == EifItemType_Key)
        lines[line_count] += "key";
    else if (type == EifItemType_Armor)
    {
        if (item.gender == 1)
            lines[line_count] += "male";
        else
            lines[line_count] += "female";
    }
    else if (type <= EifItemType_Bracer)
    {
        if (show_cursed && item.special == 5)
            lines[line_count] += "cursed";
        else
            lines[line_count] += "wearable";
    }
    else if (type == EifItemType_Alcohol)
        lines[line_count] += "beverage";
    else if (type == EifItemType_EffectPotion)
        lines[line_count] += "effect";
    else if (type == EifItemType_HairDye)
        lines[line_count] += "hairdye";
    else if (type == EifItemType_CureCurse)
        lines[line_count] += "medicine";
    else
        lines[line_count] += "unknown";
    if (type == EifItemType_Heal)
    {
        if (item.hp > 0)
            lines[line_count] += " + " + IntToStr(item.hp) + "hp";
        if (item.tp > 0)
            lines[line_count] += " + " + IntToStr(item.tp) + "tp";
    }
    if (type == EifItemType_ExpReward && item.spec_value > 0)
        lines[line_count] += " + " + IntToStr(item.spec_value);
    if (type == EifItemType_Weapon)
        lines[line_count] += " weapon";
    else if (type == EifItemType_Shield)
        lines[line_count] += " shield";
    else if (type == EifItemType_Armor)
        lines[line_count] += " clothing";
    else if (type == EifItemType_Hat)
        lines[line_count] += " hat";
    else if (type == EifItemType_Boots)
        lines[line_count] += " boots";
    else if (type == EifItemType_Gloves)
        lines[line_count] += " gloves";
    else if (type == EifItemType_Accessory)
        lines[line_count] += " pants";
    else if (type == EifItemType_Belt)
        lines[line_count] += " belt";
    else if (type == EifItemType_Necklace)
        lines[line_count] += " necklace";
    else if (type == EifItemType_Ring)
        lines[line_count] += " ring";
    else if (type == EifItemType_Armlet)
        lines[line_count] += " bracelet";
    else if (type == EifItemType_Bracer)
        lines[line_count] += " bracer";
    line_colors[line_count] = 5;
    line_count++;
    if (type >= EifItemType_Weapon && type <= EifItemType_Bracer)
    {
        if (item.min_damage > 0 || item.max_damage > 0)
        {
            if (item.spare_byte >= 1 && item.spare_byte <= 5)
                lines[line_count] += "aoe: ";
            else
                lines[line_count] += "damage: ";
            lines[line_count] += IntToStr(item.min_damage) + "-" + IntToStr(item.max_damage);
            if (item.spare_byte == 2)
                lines[line_count] += " (3x3)";
            if (item.spare_byte == 3)
                lines[line_count] += " (5x5)";
            if (item.spare_byte == 4)
                lines[line_count] += " (7x7)";
            if (item.spare_byte == 5)
                lines[line_count] += " (9x9)";
            line_count++;
        }
        if (item.element > 0)
        {
            lines[line_count] += "damage: " + IntToStr(item.element_power);
            if (item.element == 1)
                lines[line_count] += " light";
            if (item.element == 2)
                lines[line_count] += " dark";
            if (item.element == 3)
                lines[line_count] += " earth";
            if (item.element == 4)
                lines[line_count] += " air";
            if (item.element == 5)
                lines[line_count] += " water";
            if (item.element == 6)
                lines[line_count] += " fire";
            line_count++;
        }
        if (item.hp > 0 || item.tp > 0)
        {
            lines[line_count] = "add+";
            if (item.hp > 0)
                lines[line_count] += " " + IntToStr(item.hp) + "hp";
            if (item.tp > 0)
                lines[line_count] += " " + IntToStr(item.tp) + "tp";
            line_count++;
        }
        if (item.accuracy > 0)
            lines[line_count] += " " + IntToStr(item.accuracy) + "hit";
        if (item.evade > 0)
            lines[line_count] += " " + IntToStr(item.evade) + "eva";
        if (item.armor > 0)
            lines[line_count] += " " + IntToStr(item.armor) + "ar";
        if (item.return_damage > 0)
            lines[line_count] += " " + IntToStr(item.return_damage) + "rd";
        if (item.accuracy > 0)
        {
            lines[line_count] = "plus+" + lines[line_count];
            line_count++;
        }
        else if (item.evade > 0 || item.armor > 0)
        {
            lines[line_count] = "def+" + lines[line_count];
            line_count++;
        }
        else if (item.return_damage > 0)
        {
            lines[line_count] = "r.dam+" + lines[line_count];
            line_count++;
        }
        int stat_count = 0;
        if (item.bonus_str > 0 && stat_count < 4)
        {
            lines[line_count] += " " + IntToStr(item.bonus_str) + "s";
            stat_count++;
        }
        if (item.bonus_int > 0 && stat_count < 4)
        {
            lines[line_count] += " " + IntToStr(item.bonus_int) + "i";
            stat_count++;
        }
        if (item.bonus_wis > 0 && stat_count < 4)
        {
            lines[line_count] += " " + IntToStr(item.bonus_wis) + "w";
            stat_count++;
        }
        if (item.bonus_agi > 0 && stat_count < 4)
        {
            lines[line_count] += " " + IntToStr(item.bonus_agi) + "a";
            stat_count++;
        }
        if (item.bonus_con > 0 && stat_count < 4)
        {
            lines[line_count] += " " + IntToStr(item.bonus_con) + "c";
            stat_count++;
        }
        if (item.bonus_cha > 0 && stat_count < 4)
            lines[line_count] += " " + IntToStr(item.bonus_cha) + "ch";
        if (lines[line_count].Length() > 0)
        {
            lines[line_count] = "stat+" + lines[line_count];
            line_count++;
        }
        int resist_count = 0;
        if (item.light_resist > 0 && resist_count < 4)
        {
            lines[line_count] += " " + IntToStr(item.light_resist) + "l";
            resist_count++;
        }
        if (item.dark_resist > 0 && resist_count < 4)
        {
            lines[line_count] += " " + IntToStr(item.dark_resist) + "d";
            resist_count++;
        }
        if (item.earth_resist > 0 && resist_count < 4)
        {
            lines[line_count] += " " + IntToStr(item.earth_resist) + "e";
            resist_count++;
        }
        if (item.air_resist > 0 && resist_count < 4)
        {
            lines[line_count] += " " + IntToStr(item.air_resist) + "a";
            resist_count++;
        }
        if (item.water_resist > 0 && resist_count < 4)
        {
            lines[line_count] += " " + IntToStr(item.water_resist) + "w";
            resist_count++;
        }
        if (item.fire_resist > 0 && resist_count < 4)
        {
            lines[line_count] += " " + IntToStr(item.fire_resist) + "f";
            resist_count++;
        }
        if (lines[line_count].Length() > 0)
        {
            lines[line_count] = "res+" + lines[line_count];
            line_count++;
        }
        if (item.required_class > 0)
        {
            String class_name = MainForm->class_values->GetName(item.required_class);
            lines[line_count] = " " + class_name;
        }
        if (item.required_level > 0)
        {
            if (!lines[line_count].Length())
                lines[line_count] += " " + IntToStr(item.required_level) + "LVL";
            else
                lines[line_count] += " (" + IntToStr(item.required_level) + "LVL)";
        }
        if (!item.required_class)
        {
            int requirement_count = 0;
            if (item.required_str > 0 && requirement_count < 3)
            {
                lines[line_count] += " " + IntToStr(item.required_str) + "s";
                requirement_count++;
            }
            if (item.required_int > 0 && requirement_count < 3)
            {
                lines[line_count] += " " + IntToStr(item.required_int) + "i";
                requirement_count++;
            }
            if (item.required_wis > 0 && requirement_count < 3)
            {
                lines[line_count] += " " + IntToStr(item.required_wis) + "w";
                requirement_count++;
            }
            if (item.required_agi > 0 && requirement_count < 3)
            {
                lines[line_count] += " " + IntToStr(item.required_agi) + "a";
                requirement_count++;
            }
            if (item.required_con > 0 && requirement_count < 3)
            {
                lines[line_count] += " " + IntToStr(item.required_con) + "c";
                requirement_count++;
            }
            if (item.required_cha > 0 && requirement_count < 3)
            {
                lines[line_count] += " " + IntToStr(item.required_cha) + "ch";
                requirement_count++;
            }
        }
        if (lines[line_count].Length() > 0)
        {
            lines[line_count] = "req:" + lines[line_count];
            line_count++;
        }
    }
    Measure();
}

// Builds the tip of a spell: its kind and target, and with the extended tips on, its area, element and damage, the
// stats it changes and its costs.
void Tooltip::ShowSpell(int spell_id)
{
    if (spell_id < 0)
    {
        ClearLines();
        return;
    }
    // The tip of the spell already shown is kept as it is.
    if (line_count > 0 && spell_id == shown_id)
        return;
    shown_id = spell_id;
    ClearLines();
    SkillValue spell = MainForm->skill_values->GetSkill(spell_id);
    lines[line_count] = spell.name;
    line_colors[line_count] = 0;
    line_count++;
    if (!MainForm->config->GetExtendedTooltip())
    {
        Measure();
        return;
    }
    if (MainForm->config->GetElementalIcons())
        element = spell.element;
    else
        element = 0;
    int spell_type = spell.type;
    int area = spell.area;
    int target_type = spell.target_type;
    int heal_amount = spell.hp_heal;
    if (!spell_type)
    {
        if (target_type == 1)
            lines[line_count] += "self heal +" + String(heal_amount);
        if (!target_type)
            lines[line_count] += "heal +" + String(heal_amount);
        if (target_type == 3)
            lines[line_count] += "group heal +" + String(heal_amount);
        line_colors[line_count] = 7;
    }
    else if (spell_type == 1)
    {
        lines[line_count] += "attack";
        line_colors[line_count] = 6;
    }
    else if (spell_type == 2)
    {
        lines[line_count] += "bard";
        line_colors[line_count] = 9;
    }
    else if (spell_type == 3)
    {
        lines[line_count] += "discover";
        line_colors[line_count] = 9;
    }
    if (area >= 1 && area <= 5)
    {
        lines[line_count] = "aoe " + lines[line_count];
        if (area == 2)
            lines[line_count] += " (3x3)";
        if (area == 3)
            lines[line_count] += " (5x5)";
        if (area == 4)
            lines[line_count] += " (7x7)";
        if (area == 5)
            lines[line_count] += " (9x9)";
    }
    lines[line_count] = "type : " + lines[line_count];
    line_count++;
    if (spell_type == 1)
    {
        lines[line_count] += "damage: " + IntToStr(spell.min_damage) + "-" + IntToStr(spell.max_damage);
        line_count++;
    }
    if (spell_type == 1 && spell.element > 0)
    {
        lines[line_count] += "damage: " + IntToStr(spell.element_power);
        if (spell.element == 1)
            lines[line_count] += " light";
        if (spell.element == 2)
            lines[line_count] += " dark";
        if (spell.element == 3)
            lines[line_count] += " earth";
        if (spell.element == 4)
            lines[line_count] += " air";
        if (spell.element == 5)
            lines[line_count] += " water";
        if (spell.element == 6)
            lines[line_count] += " fire";
        line_count++;
    }
    if (spell.accuracy > 0)
        lines[line_count] += " " + IntToStr(spell.accuracy) + "hit";
    if (spell.evade > 0)
        lines[line_count] += " " + IntToStr(spell.evade) + "eva";
    if (spell.armor > 0)
        lines[line_count] += " " + IntToStr(spell.armor) + "ar";
    if (spell.return_damage > 0)
        lines[line_count] += " " + IntToStr(spell.return_damage) + "rd";
    if (spell.accuracy > 0)
    {
        lines[line_count] = "amp+" + lines[line_count];
        line_count++;
    }
    else if (spell.evade > 0 || spell.armor > 0)
    {
        lines[line_count] = "aid+" + lines[line_count];
        line_count++;
    }
    else if (spell.return_damage > 0)
    {
        lines[line_count] = "r.dam+" + lines[line_count];
        line_count++;
    }
    if (spell.tp_cost > 0)
        lines[line_count] += " " + IntToStr(spell.tp_cost) + "tp";
    if (spell.sp_cost > 0)
        lines[line_count] += " " + IntToStr(spell.sp_cost) + "sp";
    if (lines[line_count].Length() > 0)
    {
        lines[line_count] = "use:" + lines[line_count];
        line_count++;
    }
    Measure();
}

void Tooltip::SetFlipExtra(int extra)
{
    flip_extra = extra;
}

void Tooltip::SetAnchorX(int value)
{
    anchor_x = value;
}

void Tooltip::SetAnchorY(int value)
{
    anchor_y = value;
}

void Tooltip::SetRaise(int value)
{
    raise = value;
}

void Tooltip::SetFlipEdge(int edge_x, int gap)
{
    flip_edge_x = edge_x;
    flip_gap = gap;
}

void Tooltip::SetBottomLimit(int limit, int margin)
{
    bottom_limit = limit;
    bottom_margin = margin;
}

void Tooltip::SetVerticalAlign(int align)
{
    vertical_align = align;
}

void Tooltip::SetShowCursed(char show)
{
    show_cursed = show;
}

// Draws the tip's box, sized by its lines, kept above the bottom limit and flipped at the screen edge, with the
// element icon and the head preview some items show.
void Tooltip::DrawFrame()
{
    int bottom;
    int right_limit;
    int text_width;
    int flip_shift;
    if (line_count > 0)
    {
        int height = pad_y;
        int left = x;
        height = 2 * height + 15;
        int top = y;
        if (MainForm->config->GetExtendedTooltip())
        {
            if (line_count >= 2)
                height += 15;
            if (line_count >= 3)
                height += 15;
            if (line_count >= 4)
                height += 15;
            if (line_count >= 5)
                height += 15;
            if (line_count >= 6)
                height += 15;
            top -= raise;
            if (bottom_limit > 0)
            {
                bottom = top + anchor_y + height;
                if (bottom > bottom_limit)
                    top -= bottom - bottom_limit + bottom_margin + bottom_extra;
            }
            else
            {
                if (vertical_align == TooltipAlign_Above)
                    top -= bottom_margin + height + bottom_extra;
                if (vertical_align == TooltipAlign_Centered)
                    top -= (bottom_margin + height + bottom_extra) / 2;
            }
        }
        // Near the flip edge the tip is drawn on the other side of the mouse.
        right_limit = flip_edge_x;
        if (right_limit > 0)
        {
            text_width = width;
            if (right_limit < x + anchor_x + text_width + 2 * pad_x)
            {
                flip_shift = 2 * pad_x + text_width + flip_gap + flip_extra;
                left -= flip_shift;
            }
        }
        MainForm->blitter->HalfBlendBlt(left + anchor_x, top + anchor_y, MainForm->OffscreenSurface,
            MainForm->gfx->interface_graphics->chat_panel_surface,
            &MainForm->gfx->interface_graphics->GetBackRect(2 * pad_x + width, height));
        if (element > 0)
        {
            if (element > 6)
                element = 6;
            MainForm->OffscreenSurface->BltFast(left + anchor_x + pad_x + title_width, top + anchor_y + pad_y + 2,
                MainForm->gfx->interface_graphics->element_icons,
                &MainForm->gfx->interface_graphics->GetElementIconRect(element),
                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        }
        if (line_count > 6)
        {
            if (line_count - 6 > scroll_offset)
                MainForm->OffscreenSurface->BltFast(left + anchor_x + pad_x + width - 2, top + anchor_y + pad_y + 81,
                    MainForm->gfx->interface_graphics->element_icons,
                    &MainForm->gfx->interface_graphics->scroll_down_arrow_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
            if (scroll_offset)
                MainForm->OffscreenSurface->BltFast(left + anchor_x + pad_x + width - 2, top + anchor_y + pad_y + 21,
                    MainForm->gfx->interface_graphics->element_icons,
                    &MainForm->gfx->interface_graphics->scroll_up_arrow_rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
        }
    }
}

// Draws the tip's lines at the place its box takes, the fixed lines first, then the scrolled ones.
void Tooltip::DrawString()
{
    int flip_edge;
    int text_width;
    int flip_shift;
    int line;
    int index;
    int bottom;
    if (line_count > 0)
    {
        int height = pad_y;
        int left = x;
        height = 2 * height + 15;
        int top = y;
        if (MainForm->config->GetExtendedTooltip())
        {
            if (line_count >= 2)
                height += 15;
            if (line_count >= 3)
                height += 15;
            if (line_count >= 4)
                height += 15;
            if (line_count >= 5)
                height += 15;
            if (line_count >= 6)
                height += 15;
            top -= raise;
            if (bottom_limit > 0)
            {
                bottom = top + anchor_y + height;
                if (bottom > bottom_limit)
                    top -= bottom - bottom_limit + bottom_margin + bottom_extra;
            }
            else
            {
                if (vertical_align == TooltipAlign_Above)
                    top -= bottom_margin + height + bottom_extra;
                if (vertical_align == TooltipAlign_Centered)
                    top -= (height + bottom_margin + bottom_extra) / 2;
            }
        }
        flip_edge = flip_edge_x;
        if (flip_edge > 0)
        {
            text_width = width;
            if (flip_edge < x + anchor_x + text_width + 2 * pad_x)
            {
                flip_shift = 2 * pad_x + text_width + flip_gap + flip_extra;
                left -= flip_shift;
            }
        }
        for (line = 0; line < fixed_lines; line++)
        {
            index = line;
            MainForm->text_canvas->Font->Color = (TColor)colors[line_colors[index]];
            MainForm->text_canvas->TextOut(left + anchor_x + pad_x, top + anchor_y + pad_y + 15 * line, lines[index]);
        }
        for (; line < 6; line++)
        {
            int scrolled_index = scroll_offset + line;
            MainForm->text_canvas->Font->Color = (TColor)colors[line_colors[scrolled_index]];
            MainForm->text_canvas->TextOut(left + anchor_x + pad_x, top + anchor_y + pad_y + 15 * line,
                lines[scrolled_index]);
        }
        MainForm->text_canvas->Font->Color = (TColor)colors[default_color];
    }
}

// Sets the tip's width from its widest line, only the title unless the extended tips setting is on.
void Tooltip::Measure()
{
    short grow;
    HDC dc;
    width = 0;
    title_width = 0;
    if (MainForm->OffscreenSurface->GetDC(&dc))
    {
        try
        {
            MainForm->text_canvas->Handle = dc;
            MainForm->text_canvas->Font->Size = 8;
            int line_total = 1;
            if (MainForm->config->GetExtendedTooltip())
                line_total = line_count;
            for (int i = 0; i < line_total; i++)
            {
                int line_width = MainForm->text_canvas->TextWidth(lines[i]);
                if (line_width > width)
                    width = line_width;
                if (!i)
                    title_width = line_width + 4;
            }
        }
        catch (Exception &)
        {
        }
        MainForm->OffscreenSurface->ReleaseDC(dc);
    }
    if (element > 0 && title_width + 6 > width)
    {
        grow = (short)title_width + 6 - (short)width;
        width += grow;
    }
    if (line_count > 6)
        width += 8;
}

// Scrolls the lines down one; false when all of them fit in the six shown.
bool Tooltip::ScrollDown()
{
    if (line_count <= 6)
        return false;
    if (line_count - 6 > scroll_offset)
    {
        scroll_offset++;
        MainForm->redraw_needed = true;
        return true;
    }
    return true;
}

// Scrolls the lines up one; false when all of them fit in the six shown.
bool Tooltip::ScrollUp()
{
    int count = line_count;
    if (count <= 6)
        return false;
    if (scroll_offset > 0)
    {
        scroll_offset--;
        MainForm->redraw_needed = true;
        return true;
    }
    return true;
}

bool Tooltip::IsVisible()
{
    if (line_count > 0)
        return true;
    return false;
}
