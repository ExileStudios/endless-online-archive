#include <vcl.h>
#pragma hdrstop

#include "Ctrl_tooltip.h"
#include "Tmainform.h"
#include "Ctrl_paperdoll.h"
#include "Gfx_interface.h"

#pragma package(smart_init)

// An empty tip at left, top; fixed is the number of its fixed lines.
Tooltip::Tooltip(int left, int fixed)
{
    x = left;
    y = fixed;
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
    for (int i = 0; i < 6; i++)
    {
        lines[i] = "";
        line_colors[i] = 6;
    }
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
    ClearLines();
    if (item_id <= 0)
        return;
    if (MainForm->item_values->Count() < item_id)
        return;
    ItemValue item = MainForm->item_values->GetItem(item_id);
    int type = item.type;
    if (count > 1)
    {
        if (type == EifItemType_General)
            lines[line_count] = IntToStr(count) + " " + item.name;
        if (type == EifItemType_Currency)
            lines[line_count] = IntToStr(count) + " " + item.name;
        else
            lines[line_count] = item.name + " x" + IntToStr(count);
    }
    else
    {
        if (type == EifItemType_General)
            lines[line_count] = IntToStr(count) + " " + item.name;
        if (type == EifItemType_Currency)
            lines[line_count] = IntToStr(count) + " " + item.name;
        else
            lines[line_count] = item.name;
    }
    line_colors[line_count] = (char)item.special;
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
        lines[line_count] += " misc.";
    else if (type == EifItemType_Belt)
        lines[line_count] += " belt";
    else if (type == EifItemType_Necklace)
        lines[line_count] += " necklace";
    else if (type == EifItemType_Ring)
        lines[line_count] += " ring";
    else if (type == EifItemType_Armlet)
        lines[line_count] += " barclet";
    else if (type == EifItemType_Bracer)
        lines[line_count] += " bracer";
    line_colors[line_count] = 5;
    line_count++;
    if (type >= EifItemType_Weapon && type <= EifItemType_Bracer)
    {
        if (item.min_damage > 0 || item.max_damage > 0)
        {
            if (item.gender >= 10 && item.gender <= 16)
                lines[line_count] += "aoe: ";
            else
                lines[line_count] += "damage: ";
            lines[line_count] += IntToStr(item.min_damage) + "-" + IntToStr(item.max_damage);
            if (item.gender == 11)
                lines[line_count] += " (1x1)";
            if (item.gender == 12)
                lines[line_count] += " (2x2)";
            if (item.gender == 13)
                lines[line_count] += " (3x3)";
            if (item.gender == 14)
                lines[line_count] += " (4x4)";
            if (item.gender == 15)
                lines[line_count] += " (5x5)";
            if (item.gender == 16)
                lines[line_count] += " (6x6)";
            line_count++;
        }
        if (item.accuracy > 0)
            lines[line_count] += " " + IntToStr(item.accuracy) + "hit";
        if (item.evade > 0)
            lines[line_count] += " " + IntToStr(item.evade) + "dg";
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
    int current_line;
    ClearLines();
    if (spell_id < 0)
        return;
    SkillValue spell = MainForm->skill_values->GetSkill(spell_id);
    lines[line_count] = spell.name;
    line_colors[line_count] = 0;
    line_count++;
    if (!MainForm->config->GetExtendedTooltip())
    {
        Measure();
        return;
    }
    int spell_kind = spell.type;
    int spell_type = spell.spare_byte;
    int target_type = spell.target_type;
    int heal_amount = spell.hp_heal;
    if (!spell_kind)
    {
        if (target_type == 1)
            lines[line_count] += "self heal +" + String(heal_amount);
        if (!target_type)
            lines[line_count] += "heal +" + String(heal_amount);
        if (target_type == 3)
            lines[line_count] += "group heal +" + String(heal_amount);
        line_colors[line_count] = 7;
    }
    else if (spell_kind == 1)
    {
        lines[line_count] += "attack";
        line_colors[line_count] = 6;
    }
    else if (spell_kind == 2)
    {
        lines[line_count] += "bard";
        line_colors[line_count] = 9;
    }
    if (spell_type >= 10 && spell_type <= 16)
    {
        lines[line_count] = "aoe " + lines[line_count];
        if (spell_type == 11)
            lines[line_count] += " (1x1)";
        if (spell_type == 12)
            lines[line_count] += " (2x2)";
        if (spell_type == 13)
            lines[line_count] += " (3x3)";
        if (spell_type == 14)
            lines[line_count] += " (4x4)";
        if (spell_type == 15)
            lines[line_count] += " (5x5)";
        if (spell_type == 16)
            lines[line_count] += " (6x6)";
    }
    lines[line_count] = "type : " + lines[line_count];
    line_count++;
    if (spell_kind == 1 || (spell_kind >= 10 && spell_kind <= 16))
    {
        lines[line_count] += "damage: " + IntToStr(spell.min_damage) + "-" + IntToStr(spell.max_damage);
        line_count++;
    }
    if (spell.accuracy > 0)
        lines[line_count] += " " + IntToStr(spell.accuracy) + "hit";
    if (spell.evade > 0)
        lines[line_count] += " " + IntToStr(spell.evade) + "dg";
    if (spell.armor > 0)
        lines[line_count] += " " + IntToStr(spell.armor) + "ar";
    current_line = spell.return_damage;
    if (current_line > 0)
        lines[line_count] += " " + IntToStr(current_line) + "ar";
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
    if (line_count > 0)
    {
        int height = 2 * pad_y;
        int left = x;
        int top = y;
        height += 15;
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
                bottom = anchor_y + top + height;
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
        if (flip_edge_x > 0 && flip_edge_x < x + anchor_x + width + 2 * pad_x)
            left -= 2 * pad_x + width + flip_gap + flip_extra;
        MainForm->blitter->HalfBlendBlt(left + anchor_x, top + anchor_y, MainForm->OffscreenSurface,
            MainForm->gfx->interface_graphics->chat_panel_surface,
            &MainForm->gfx->interface_graphics->GetBackRect(2 * pad_x + width, height));
    }
}

// Draws the tip's lines at the place its box takes, the fixed lines first, then the scrolled ones.
void Tooltip::DrawString()
{
    int flip_edge;
    int text_width;
    int flip_shift;
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
        for (height = 0; height < line_count; height++)
        {
            MainForm->text_canvas->Font->Color = (TColor)colors[line_colors[height]];
            MainForm->text_canvas->TextOut(left + anchor_x + pad_x, top + anchor_y + pad_y + 15 * height,
                lines[height]);
        }
        MainForm->text_canvas->Font->Color = (TColor)colors[default_color];
    }
}

// Sets the tip's width from its widest line, only the title unless the extended tips setting is on.
void Tooltip::Measure()
{
    HDC dc;
    width = 0;
    if (MainForm->OffscreenSurface->GetDC(&dc))
    {
        try
        {
            MainForm->text_canvas->Handle = dc;
            int measured_lines = 1;
            if (MainForm->config->GetExtendedTooltip())
                measured_lines = line_count;
            for (int i = 0; i < measured_lines; i++)
            {
                int line_width = MainForm->text_canvas->TextWidth(lines[i]);
                if (line_width > width)
                    width = line_width;
            }
        }
        catch (Exception &)
        {
        }
        MainForm->OffscreenSurface->ReleaseDC(dc);
    }
}

bool Tooltip::IsVisible()
{
    if (line_count > 0)
        return true;
    return false;
}
