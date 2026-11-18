#include <vcl.h>
#pragma hdrstop

#include "Ctrl_crafter.h"
#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Ctrl_guild.h"
#include "Ctrl_inn.h"
#include "Ctrl_interface.h"
#include "Ctrl_inventory.h"
#include "Ctrl_law.h"
#include "Ctrl_learn.h"
#include "Ctrl_locker.h"
#include "Ctrl_memo.h"
#include "Ctrl_tooltip.h"

#pragma package(smart_init)

CrafterController::CrafterController()
{
    window_x = 170;
    window_y = 30;
    session_id = -1;
    unused_value = -1;
    is_visible = false;
    blink_counter = 0;
    close_hovered = false;
    craft_hovered = false;
    is_dragging = false;
    tooltip = new Tooltip(80, 36, 2);
    tooltip->SetFlipEdge(630, 83);
    tooltip->SetVerticalAlign(TooltipAlign_Centered);
}

CrafterController::~CrafterController()
{
}

// Steps the blink of the crafting window, redrawing it twice in every 21 ticks.
void CrafterController::Update()
{
    blink_counter++;
    if (blink_counter == 1)
        MainForm->redraw_needed = true;
    if (blink_counter == 10)
        MainForm->redraw_needed = true;
    if (blink_counter > 20)
        blink_counter = 0;
}

// Opens the crafting window for the item, closing the locker, guild, law, skill master, memo and inn windows first.
void CrafterController::Open(String new_result_name, int new_result_item_id, int new_session_id)
{
    MainForm->animation->locker->is_visible = false;
    MainForm->animation->guild->is_visible = false;
    MainForm->animation->law->is_visible = false;
    MainForm->animation->learn->is_visible = false;
    MainForm->animation->memo->is_visible = false;
    MainForm->animation->inn->is_visible = false;
    MainForm->gfx->CreateCrafterGraphics();
    MainForm->gfx->LoadCrafterGraphics();
    result_name = new_result_name;
    result_item_id = new_result_item_id;
    session_id = new_session_id;
    ingredients.clear();
    needs_layout = true;
    is_visible = true;
    ingredient_hovered = false;
    blink_counter = 0;
    MainForm->redraw_needed = true;
}

// Closes the crafting window and frees its pictures.
void CrafterController::Close()
{
    tooltip->Hide();
    is_visible = false;
    session_id = -1;
    MainForm->gfx->FreeCrafterGraphics();
    MainForm->redraw_needed = true;
}

void CrafterController::AddIngredient(String name, int item_id, int amount)
{
    CraftIngredient ingredient(item_id);
    ingredient.name = name;
    ingredient.amount = amount;
    ingredients.insert(ingredients.end(), ingredient);
}

// Places the ingredient slots by how many the recipe has and works out from the held amounts whether, and how many
// times, the item can be crafted.
void CrafterController::Layout()
{
    if (needs_layout)
    {
        needs_layout = false;
        ingredient_count = ingredients.size();
        AddIngredient(result_name, result_item_id, 1);
    }
    // The ingredients are placed by their number, one to four.
    if (ingredient_count == 1)
    {
        ingredients.begin()->x = 30;
        ingredients.begin()->y = 60;
        ingredients[1].x = 183;
        ingredients[1].y = 60;
        ingredients[1].is_result = true;
    }
    else if (ingredient_count == 2)
    {
        ingredients.begin()->x = 18;
        ingredients.begin()->y = 60;
        ingredients[1].x = 106;
        ingredients[1].y = 60;
        ingredients[2].x = 195;
        ingredients[2].y = 60;
        ingredients[2].is_result = true;
    }
    else if (ingredient_count == 3)
    {
        ingredients.begin()->x = 60;
        ingredients.begin()->y = 60;
        ingredients[1].x = 18;
        ingredients[1].y = 135;
        ingredients[2].x = 106;
        ingredients[2].y = 135;
        ingredients[3].x = 195;
        ingredients[3].y = 135;
        ingredients[3].is_result = true;
    }
    else if (ingredient_count == 4)
    {
        ingredients.begin()->x = 18;
        ingredients.begin()->y = 60;
        ingredients[1].x = 106;
        ingredients[1].y = 60;
        ingredients[2].x = 18;
        ingredients[2].y = 135;
        ingredients[3].x = 106;
        ingredients[3].y = 135;
        ingredients[4].x = 195;
        ingredients[4].y = 99;
        ingredients[4].is_result = true;
    }
    can_craft = true;
    max_crafts = 99;
    for (vector<CraftIngredient>::iterator ingredient = ingredients.begin(); ingredient != ingredients.end();
        ingredient++)
    {
        ingredient->owned = MainForm->animation->interface_controller->inventory->GetItemAmount(ingredient->item_id);
        if (!ingredient->is_result)
        {
            if (ingredient->owned < ingredient->amount)
                can_craft = false;
            int times = ingredient->owned / ingredient->amount;
            if (times < max_crafts)
                max_crafts = times;
        }
    }
    window_x = 170;
    if (ingredient_count < 3)
        window_y = 60;
    else
        window_y = 30;
}

void CrafterController::ClearHover()
{
    ingredient_hovered = false;
    for (vector<CraftIngredient>::iterator ingredient = MainForm->animation->crafter->ingredients.begin();
        ingredient != MainForm->animation->crafter->ingredients.end(); ingredient++)
        ingredient->is_hovered = false;
}

// Asks to confirm the craft when the result fits the inventory; otherwise says it does not.
void CrafterController::ConfirmCraft()
{
    if (MainForm->animation->interface_controller->inventory->TestAddItem(MainForm->animation->crafter->result_item_id))
    {
        String question =
            "Do you want to exchange all ingredient to craft " + MainForm->animation->crafter->result_name + "?";
        MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_CraftItem, "Confirm craft",
            question);
    }
    else
    {
        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
            MainForm->strings->GetText(GameText_YouHaveNotEnough));
        MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
            MainForm->strings->GetText(GameText_Warning), MainForm->strings->GetText(GameText_YouHaveNotEnough));
        MainForm->edits->caret_enabled = false;
    }
}

// A click in the crafting window: the craft button asks to confirm when the item can be made, the close button closes
// it, and elsewhere the window starts moving.
void CrafterController::MouseDown(int x, int y, int button)
{
    short window_height;
    char handled = false;
    short buttons_y = 146;
    if (ingredient_count > 2)
        buttons_y += 70;
    if (can_craft)
    {
        if (x >= window_x + 48 && x <= window_x + 140 && buttons_y + window_y <= y && buttons_y + window_y + 28 >= y)
        {
            Close();
            handled = true;
        }
        for (vector<CraftIngredient>::iterator ingredient = MainForm->animation->crafter->ingredients.begin();
            ingredient != MainForm->animation->crafter->ingredients.end(); ingredient++)
        {
            if (ingredient->is_result)
            {
                int slot_x = MainForm->animation->crafter->window_x + ingredient->x;
                int slot_y = MainForm->animation->crafter->window_y + ingredient->y;
                if (x >= slot_x && x <= slot_x + 77 && slot_y <= y && slot_y + 70 >= y)
                    ConfirmCraft();
            }
        }
        if (x >= window_x + 144 && x <= window_x + 236 && buttons_y + window_y <= y && buttons_y + window_y + 28 >= y)
        {
            ConfirmCraft();
            handled = true;
        }
    }
    else if (x >= window_x + 96 && x <= window_x + 188 && buttons_y + window_y <= y && buttons_y + window_y + 28 >= y)
    {
        Close();
        handled = true;
    }
    if (!handled)
    {
        window_height = 190;
        if (ingredient_count > 2)
            window_height = 261;
        if (x >= window_x && x <= window_x + 290 && y >= window_y && window_height + window_y >= y)
        {
            is_dragging = true;
            ClearHover();
            tooltip->Hide();
            drag_offset_x = x - window_x;
            drag_offset_y = y - window_y;
        }
    }
}

// Moves a dragged window, kept on screen; shows the tip of the ingredient under the mouse and tracks the craft and
// close buttons.
void CrafterController::MouseMove(int x, int y)
{
    vector<CraftIngredient>::iterator ingredient;
    short buttons_y;
    if (is_dragging)
    {
        short max_y = 49;
        // A window of one or two ingredients is shorter, so it may go 70 lower.
        if (ingredient_count < 3)
            max_y += 70;
        window_x = x - drag_offset_x;
        window_y = y - drag_offset_y;
        if (window_x <= 10)
            window_x = 10;
        if (window_y <= 8)
            window_y = 8;
        if (window_x >= 338)
            window_x = 338;
        if (max_y <= window_y)
            window_y = max_y;
        MainForm->redraw_needed = true;
    }
    else
    {
        char over_ingredient = false;
        for (ingredient = MainForm->animation->crafter->ingredients.begin();
            ingredient != MainForm->animation->crafter->ingredients.end(); ingredient++)
        {
            int slot_x = MainForm->animation->crafter->window_x + ingredient->x;
            int slot_y = MainForm->animation->crafter->window_y + ingredient->y;
            if (x >= slot_x && x <= slot_x + 77 && y >= slot_y && slot_y + 70 >= y)
            {
                if (!ingredient->is_hovered)
                {
                    MainForm->redraw_needed = true;
                    tooltip_dirty = true;
                    tooltip->ShowItemCount(ingredient->item_id, ingredient->amount);
                    tooltip->SetAnchorX(slot_x);
                    tooltip->SetAnchorY(slot_y);
                }
                ingredient->is_hovered = true;
                ingredient_hovered = true;
                over_ingredient = true;
            }
            else
                ingredient->is_hovered = false;
        }
        if (ingredient_hovered && !over_ingredient)
        {
            ingredient_hovered = false;
            tooltip->Hide();
            MainForm->redraw_needed = true;
            tooltip_dirty = true;
        }
        buttons_y = 146;
        if (ingredient_count > 2)
            buttons_y += 70;
        if (can_craft)
        {
            if (x >= window_x + 48 && x <= window_x + 140 && buttons_y + window_y <= y &&
                buttons_y + window_y + 28 >= y)
            {
                if (!close_hovered)
                {
                    close_hovered = true;
                    MainForm->redraw_needed = true;
                }
            }
            else if (close_hovered)
            {
                close_hovered = false;
                MainForm->redraw_needed = true;
            }
            if (x >= window_x + 144 && x <= window_x + 236 && buttons_y + window_y <= y &&
                buttons_y + window_y + 28 >= y)
            {
                if (!craft_hovered)
                {
                    craft_hovered = true;
                    MainForm->redraw_needed = true;
                }
            }
            else if (craft_hovered)
            {
                craft_hovered = false;
                MainForm->redraw_needed = true;
            }
        }
        else if (x >= window_x + 96 && x <= window_x + 188 && buttons_y + window_y <= y &&
            buttons_y + window_y + 28 >= y)
        {
            if (!close_hovered)
            {
                close_hovered = true;
                MainForm->redraw_needed = true;
            }
        }
        else if (close_hovered)
        {
            close_hovered = false;
            MainForm->redraw_needed = true;
        }
    }
}

void CrafterController::MouseUp()
{
    is_dragging = false;
}
