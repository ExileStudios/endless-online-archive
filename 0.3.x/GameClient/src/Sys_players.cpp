#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Gfx_interface.h"
#include "Gfx_emotebuffer.h"
#include "Ctrl_game.h"
#include "Gfx_player.h"
#include "Ctrl_social.h"
#include "Gfx_global_sprites.h"

#pragma package(smart_init)

Players::Players(char preload_option)
{
    current_index = -1;
    hover_label_shown = false;
}

Players::~Players()
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
        delete it;
}

// Frees the loaded equipment libraries; with preload_graphics they are loaded again at once.
void Players::ReloadGraphicsLibraries()
{
}

// One step of every player's timers and animations, then the name label of the player under the mouse.
void Players::Animate()
{
    Player *i;
    int screen_x;
    int screen_y;
    char position_changed;
    char recenter_view;
    int effect_volume;
    position_changed = false;
    recenter_view = false;
    i = players.begin();
    while (i != players.end())
    {
        // A text label counts down; a blinking one redraws on odd counts.
        if (i->text_label_shown)
        {
            i->text_label_timer--;
            if (i->text_label_blinking && (i->text_label_timer % 4 == 1 || i->text_label_timer % 4 == 3))
            {
                i->redraw = true;
                MainForm->redraw_needed = true;
            }
            if (i->text_label_timer < 0)
            {
                i->text_label_shown = false;
                MainForm->redraw_needed = true;
            }
        }
        if (i->emote_start && i->emote_sheets_loaded)
        {
            i->emote_start = false;
            i->emote_frame = -1;
            i->emote_step = 0;
            i->emoting = true;
        }
        if (i->emoting)
        {
            if (i->emote_frame < i->emote_frame_count)
                i->emote_frame++;
            else
                i->emote_frame = 0;
            if (i->emote_step <= 6)
                i->emote_step++;
            else
            {
                i->emote_step = 0;
                i->emoting = false;
            }
            if (i->emote_frame_count > 0 || !i->emote_step)
                i->redraw = true;
        }
        if (i->effect_start)
        {
            i->effect_start = false;
            i->effect_running = true;
            i->effect_frame = 0;
            effect_volume = MainForm->players->GetDistanceVolume(i->player_id);
            MainForm->sound_mapper->PlayEffectSound(i->effect_id, effect_volume);
        }
        if (i->effect_running)
        {
            if (i->effect_frame <= i->effect_last_frame)
                i->effect_frame++;
            else
            {
                i->effect_loops_left--;
                i->effect_frame = 0;
                if (i->effect_loops_left <= 0)
                    i->effect_running = false;
            }
            i->redraw = true;
        }
        // A pending effect, set when the player was added, starts here.
        if (i->pending_effect >= 0)
        {
            i->effect_start = true;
            i->effect_running = false;
            i->effect_id = i->pending_effect;
            i->effect_loops_left = 1;
            i->effect_frame = 0;
            i->effect_base_layer = MainForm->magic_mapper->base_layer[i->pending_effect];
            i->effect_blend_layer = MainForm->magic_mapper->blend_layer[i->pending_effect];
            i->effect_solid_layer = MainForm->magic_mapper->solid_layer[i->pending_effect];
            i->effect_last_frame = MainForm->magic_mapper->GetFrameCount(i->pending_effect) - 2;
            i->effect_loops_left = MainForm->magic_mapper->GetLoopCount(i->pending_effect);
            i->pending_effect = -1;
        }
        if (i->dying)
        {
            i->death_countdown--;
            i->redraw = true;
        }
        if (i->hiding)
        {
            i->hide_countdown--;
            if (i->hide_countdown <= 0)
            {
                i->hiding = false;
                i->hidden = true;
            }
            i->redraw = true;
        }
        if (i->unhiding)
        {
            i->unhide_countdown--;
            if ((int)i->unhide_countdown <= 0)
            {
                i->unhiding = false;
                i->hidden = false;
            }
            i->redraw = true;
        }
        if (i->jump_start && i->walk_sheets_loaded)
        {
            i->jump_start = false;
            i->x = i->target_x;
            i->y = i->target_y;
            i->jump_direction = i->target_direction;
            i->direction = i->target_direction;
            i->jump_frame = -1;
            i->jumping = true;
            position_changed = true;
        }
        // A step waits for its cells; the player is placed on the new tile as it starts.
        if (i->walk_start && i->walk_sheets_loaded)
        {
            i->walk_start = false;
            i->x = i->target_x;
            i->y = i->target_y;
            i->walk_direction = i->walk_target_direction;
            i->direction = i->target_direction;
            i->walk_frame = -1;
            i->walking = true;
            position_changed = true;
        }
        if (i->jumping)
        {
            if (i->jump_frame <= 2)
            {
                if (!i->jump_frame)
                {
                    int step_volume = MainForm->sfx->GetDistanceVolume((short)i->x, (short)i->y);
                    MainForm->sfx->PlaySfxAt(SfxSound_Jump, step_volume);
                }
                i->jump_frame++;
            }
            else
            {
                i->jump_frame = 0;
                i->jumping = false;
            }
            i->sitting_on_floor = false;
            i->redraw = true;
        }
        // Four frames, one per pass from the pass the step starts; the fifth pass ends it.
        if (i->walking)
        {
            if (i->walk_frame <= 2)
                i->walk_frame++;
            else
            {
                i->walk_frame = 0;
                i->walking = false;
            }
            i->sitting_on_floor = false;
            i->redraw = true;
        }
        if (i->attack_start && i->attack_sheets_loaded)
        {
            i->attack_start = false;
            i->attack_direction = i->direction;
            i->attack_frame = -1;
            i->attacking = true;
        }
        if (i->chair_sit_start && i->sit_sheets_loaded)
        {
            i->sitting_on_chair = true;
            i->chair_sit_start = false;
            i->sitting_on_floor = false;
            i->floor_sit_start = false;
            i->x = i->target_x;
            i->y = i->target_y;
            i->direction = i->target_direction;
            i->redraw = true;
            position_changed = true;
            if (i->recenter_view)
            {
                i->recenter_view = false;
                recenter_view = true;
            }
        }
        if (i->floor_sit_start && i->sit_sheets_loaded)
        {
            i->sitting_on_floor = true;
            i->floor_sit_start = false;
            i->sitting_on_chair = false;
            i->chair_sit_start = false;
            i->x = i->target_x;
            i->y = i->target_y;
            i->direction = i->target_direction;
            i->redraw = true;
            position_changed = true;
        }
        if (i->stand_up_start && i->stand_sheet_loaded)
        {
            i->sitting_on_chair = false;
            i->sitting_on_floor = false;
            i->stand_up_start = false;
            i->x = i->target_x;
            i->y = i->target_y;
            i->redraw = true;
            position_changed = true;
            if (i->recenter_view)
            {
                i->recenter_view = false;
                recenter_view = true;
            }
        }
        if (i->attacking)
        {
            if (i->attack_frame <= 2)
            {
                i->attack_frame++;
                if (i->attack_frame == 1)
                {
                    if (!i->silent_attack)
                    {
                        int weapon_volume;
                        weapon_volume = MainForm->players->GetDistanceVolume(i->player_id);
                        MainForm->sound_mapper->PlayWeaponSound(i->weapon, weapon_volume);
                    }
                    i->silent_attack = false;
                    if (i->weapon == 49 || i->weapon == 50)
                    {
                        i->emote_start = true;
                        i->emoting = false;
                        i->emote_id = 15;
                        i->emote_first_frame = 57;
                        i->emote_frame_count = EMOTE_FRAME_COUNT;
                    }
                }
            }
            else
            {
                i->attack_frame = 0;
                i->attacking = false;
            }
            i->redraw = true;
        }
        if (i->cast_start && i->cast_sheets_loaded)
        {
            i->cast_start = false;
            i->cast_direction = i->direction;
            i->cast_frame = -1;
            i->casting = true;
        }
        if (i->casting)
        {
            if (i->cast_frame <= 2)
                i->cast_frame++;
            else
            {
                i->cast_frame = 0;
                i->casting = false;
            }
            i->redraw = true;
        }
        if (i->health_bar_start)
        {
            i->health_bar_start = false;
            if (i->health_bar_timer > 0)
            {
                i->health_bar_shown = true;
                i->redraw = true;
            }
        }
        if (i->health_bar_shown)
        {
            i->health_bar_timer--;
            if (i->health_bar_timer <= 0)
            {
                i->health_bar_shown = false;
                i->redraw = true;
            }
        }
        if (i->damage_start)
        {
            i->damage_start = false;
            if (i->damage_timer > 0)
            {
                i->damage_shown = true;
                i->redraw = true;
            }
        }
        if (i->damage_shown)
        {
            i->damage_timer--;
            i->redraw = true;
            if (i->damage_timer <= 0)
            {
                i->damage_shown = false;
                i->redraw = true;
            }
        }
        // The level-up emote waits until no step, jump, attack, cast or emote runs.
        if (i->level_up_pending && !i->walk_start && !i->walking && !i->jump_start && !i->jumping && !i->attack_start &&
            !i->attacking && !i->cast_start && !i->casting && !i->emote_start && !i->emoting)
        {
            i->level_up_pending = false;
            int arrive_volume = MainForm->sfx->GetDistanceVolume((short)i->x, (short)i->y);
            MainForm->sfx->PlaySfxAt(SfxSound_LevelUp, arrive_volume);
            MainForm->map->BlockActionsBriefly();
            i->emote_start = true;
            i->emoting = false;
            i->emote_id = 13;
            i->emote_first_frame = 49;
            i->emote_frame_count = EMOTE_FRAME_COUNT;
        }
        if (i->balloon_shown)
        {
            i->balloon_timer--;
            if (i->balloon_timer <= 1)
                i->HideBalloon();
        }
        i++;
    }
    char no_hover_yet = true;
    vector<Player>::reverse_iterator it(players.end());
    for (; it != vector<Player>::reverse_iterator(players.begin()); it++)
    {
        // The tile's position on screen.
        screen_x = 32 * (*it).x - 32 * (*it).y + MainForm->map->GetScrollOffsetX() + 310;
        screen_y = 16 * (*it).x + 16 * (*it).y + MainForm->map->GetScrollOffsetY() + 102;
        if (screen_x - 1 <= mouse_x && screen_x + 19 >= mouse_x && screen_y <= mouse_y && screen_y + 58 >= mouse_y)
        {
            if (no_hover_yet)
            {
                if (!(*it).hover_label_shown)
                {
                    int label_style = 0;
                    if (MainForm->animation->social->IsIgnored((*it).name))
                        label_style = 1;
                    if (MainForm->animation->social->IsFriend((*it).name))
                        label_style = 6;
                    String label_text = (&*it)->name;
                    if ((*it).guild_tag.Length() > 1 && (&*it)->guild_tag[2] != 32 && (&*it)->guild_tag[1] != 32 &&
                        (&*it)->guild_tag[1] != 48)
                    {
                        // A three-letter tag ending in a space shows its first two letters.
                        if ((*it).guild_tag.Length() == 3)
                        {
                            if ((&*it)->guild_tag[3] == 32)
                                label_text = label_text + " " + (&*it)->guild_tag.SubString(1, 2);
                            else
                                label_text = label_text + " " + (&*it)->guild_tag;
                        }
                        else
                            label_text = label_text + " " + (&*it)->guild_tag;
                    }
                    MainForm->gfx->sprites->ShowNameLabel(label_text, label_style, -1);
                    (*it).hover_label_shown = true;
                    (*it).redraw = true;
                }
                no_hover_yet = false;
            }
            else if ((*it).hover_label_shown)
            {
                (*it).hover_label_shown = false;
                (*it).redraw = true;
            }
        }
        else if ((*it).hover_label_shown)
        {
            (*it).hover_label_shown = false;
            (*it).redraw = true;
        }
    }
    if (no_hover_yet)
        hover_label_shown = false;
    else
        hover_label_shown = true;
    if (position_changed)
        SortByPosition();
    if (recenter_view)
        MainForm->animation->game->CenterViewOnPlayer(MainForm->net_client->GetPlayerId());
}

// Drops players whose death countdown ran out, and players that stay out of view; preview and slot players stay.
void Players::RemoveExpiredPlayers(int center_x, int center_y)
{
    // Starts over after each erase: an erase moves the rest of the list.
    do
    {
        list_changed = false;
        for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
        {
            if (it->dying)
            {
                if (it->death_countdown <= 0)
                {
                    it->FreeResources();
                    it->CloseBalloon();
                    players.erase(it);
                    list_changed = true;
                    break;
                }
            }
            else if (!IsInViewRange(center_x, center_y, it->x, it->y) && it->player_id <= PlayerId_LocalLimit)
            {
                if (!it->out_of_range)
                {
                    it->out_of_range = true;
                    it->removal_countdown = 4;
                }
                else
                {
                    it->removal_countdown--;
                    if (it->removal_countdown <= 0)
                    {
                        it->FreeResources();
                        it->CloseBalloon();
                        players.erase(it);
                        list_changed = true;
                        break;
                    }
                }
            }
            else
                it->out_of_range = false;
        }
    } while (list_changed);
}

// Opens the hair, boots, armor, hat, weapon and shield graphics files of the gender.
void Players::LoadGraphicsLibraries(int gender)
{
    if (gender)
    {
        hair_library_gender_one = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MaleHair);
        boots_library_gender_one = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MaleBoots);
        armor_library_gender_one = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MaleArmor);
        hat_library_gender_one = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MaleHats);
        weapon_library_gender_one = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MaleWeapons);
        shield_library_gender_one = MainForm->gfx_vault->OpenLibrary(GfxLibrary_MaleShields);
    }
    else
    {
        hair_library_gender_zero = MainForm->gfx_vault->OpenLibrary(GfxLibrary_FemaleHair);
        boots_library_gender_zero = MainForm->gfx_vault->OpenLibrary(GfxLibrary_FemaleBoots);
        armor_library_gender_zero = MainForm->gfx_vault->OpenLibrary(GfxLibrary_FemaleArmor);
        hat_library_gender_zero = MainForm->gfx_vault->OpenLibrary(GfxLibrary_FemaleHats);
        weapon_library_gender_zero = MainForm->gfx_vault->OpenLibrary(GfxLibrary_FemaleWeapons);
        shield_library_gender_zero = MainForm->gfx_vault->OpenLibrary(GfxLibrary_FemaleShields);
    }
}

// Takes the gender's open graphics files as the ones to draw from.
void Players::SelectGraphicsLibraries(int gender)
{
    if (gender)
    {
        hair_library = hair_library_gender_one;
        boots_library = boots_library_gender_one;
        armor_library = armor_library_gender_one;
        hat_library = hat_library_gender_one;
        weapon_library = weapon_library_gender_one;
        shield_library = shield_library_gender_one;
        return;
    }
    hair_library = hair_library_gender_zero;
    boots_library = boots_library_gender_zero;
    armor_library = armor_library_gender_zero;
    hat_library = hat_library_gender_zero;
    weapon_library = weapon_library_gender_zero;
    shield_library = shield_library_gender_zero;
}

// Builds the sprite sheets of up to four players whose graphics are waiting: every frame of standing, sitting,
// walking, attacking and casting, layering skin, clothes, hair and gear in the order each hat and shield needs.
void Players::LoadPendingGraphics()
{
    int loaded_count = 0;
    vector<Player>::iterator i = players.begin();
    while (i != players.end() && loaded_count <= 3)
    {
        // A reload frees the player's sheets and marks every one of them to load again.
        if (i->reload_requested && Application->Active)
        {
            i->FreeResources();
            i->CreateResources();
            i->reload_requested = false;
            i->graphics_pending = true;
            i->sheets_cleared = false;
            i->walk_sheets_loaded = false;
            i->walk_tail_sheets_loaded = false;
            i->stand_sheet_loaded = false;
            i->sit_sheets_loaded = false;
            i->attack_sheets_loaded = false;
            i->extra_sheets_loaded = false;
            i->cast_sheets_loaded = false;
            i->emote_sheets_loaded = false;
        }
        if (i->graphics_pending && !i->reload_requested && (Application->Active || !MainForm->config->GetFullscreen()))
        {
            int gender = i->gender;
            int skin = i->skin;
            int hair_resource = 40 * i->hair_style + 4 * i->hair_color + 61;
            int boots_resource = 40 * i->boots + 61;
            int armor_resource = 50 * i->armor + 51;
            int hat_resource = 10 * i->hat + 91;
            int weapon_resource = 100 * i->weapon + 1;
            int shield_resource = 50 * i->shield + 51;
            char hair_hidden = false;
            char hair_over_hat = false;
            char back_shield = false;
            char kind_skin = false;
            // A graphic number outside 101 to 10000 draws nothing.
            if (hair_resource < MIN_PAPERDOLL_RESOURCE || hair_resource > 10000)
                hair_resource = 0;
            if (boots_resource < MIN_PAPERDOLL_RESOURCE || boots_resource > 10000)
                boots_resource = 0;
            if (armor_resource < MIN_PAPERDOLL_RESOURCE || armor_resource > 10000)
                armor_resource = 0;
            if (hat_resource < MIN_PAPERDOLL_RESOURCE || hat_resource > 10000)
                hat_resource = 0;
            if (weapon_resource < MIN_PAPERDOLL_RESOURCE || weapon_resource > 10000)
                weapon_resource = 0;
            if (shield_resource < MIN_PAPERDOLL_RESOURCE || shield_resource > 10000)
                shield_resource = 0;
            // Some hats tuck the hair in and others hide it; some shields are worn on the back.
            if (i->hat >= HatGraphic_FirstHairOverHat && i->hat <= HatGraphic_DragonMask)
                hair_over_hat = true;
            if (i->hat == HatGraphic_FrogHead || i->hat == HatGraphic_Pilotte || i->hat == HatGraphic_Glasses ||
                i->hat == HatGraphic_MedicCap)
                hair_over_hat = true;
            if (i->hat >= HatGraphic_BlackHood && i->hat <= HatGraphic_ChainmailHat)
                hair_hidden = true;
            if (i->hat == HatGraphic_HornedHat || i->hat == HatGraphic_MerchantHat || i->hat == HatGraphic_Helmy ||
                i->hat == HatGraphic_EloffHelmet || i->hat == HatGraphic_AirHat || i->hat == HatGraphic_Beruta ||
                i->hat == HatGraphic_PirateHat || i->hat == HatGraphic_LotusHelmet || i->hat == HatGraphic_KittyHat ||
                i->hat == HatGraphic_HulaHulaHat || i->hat == HatGraphic_GobHelm ||
                i->hat == HatGraphic_HornedGobHelm || i->hat == HatGraphic_HelmetDarkness ||
                i->hat == HatGraphic_FladHat || i->hat == HatGraphic_CookHat)
                hair_hidden = true;
            if (i->shield == ShieldGraphic_GoodWings || i->shield == ShieldGraphic_Bag ||
                i->shield == ShieldGraphic_NormalArrows || i->shield == ShieldGraphic_FrostArrows ||
                i->shield == ShieldGraphic_FireArrows || i->shield == ShieldGraphic_GoodForceWings ||
                i->shield == ShieldGraphic_FireForceWings)
                back_shield = true;
            if (i->skin == 8 || i->skin == 9 || i->skin == 11)
                kind_skin = true;
            // The standing frames, while the player stands still or gets up.
            if (((!i->sitting_on_chair && !i->sitting_on_floor && !i->walking && !i->jumping && !i->attacking &&
                !i->casting) || i->stand_up_start) && !i->stand_sheet_loaded)
            {
                try
                {
                    LoadGraphicsLibraries(gender);
                    SelectGraphicsLibraries(gender);
                    if (!i->sheets_cleared)
                    {
                        i->resources->ClearSheets();
                        i->sheets_cleared = true;
                    }
                    if (kind_skin)
                        i->resources->DrawFrontLayerFrom(PlayerFrame_Stand, 39, 17 - i->gender,
                            MainForm->gfx->sprites->kind_sheet, MainForm->gfx->sprites->GetKindFrameRect(gender, 0,
                            i->skin));
                    if (((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one)) &&
                        shield_resource >= MIN_PAPERDOLL_RESOURCE && back_shield)
                        i->resources->DrawFrontLayer(PlayerFrame_Stand, 18, 0, shield_library, shield_resource);
                    if (((!gender && weapon_library_gender_zero) || (gender == 1 && weapon_library_gender_one)) &&
                        weapon_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_Stand, 0, 0, weapon_library, weapon_resource);
                        i->resources->DrawBackLayer(PlayerFrame_Stand, 0, 0, weapon_library, weapon_resource + 1);
                    }
                    if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                        hair_resource >= MIN_PAPERDOLL_RESOURCE && !hair_hidden)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_Stand, 28, 7, hair_library, hair_resource);
                        i->resources->DrawBackLayer(PlayerFrame_Stand, 28, 7, hair_library, hair_resource + 2);
                    }
                    if (!i->resources->DrawFrontLayerFrom(PlayerFrame_Stand, 34, gender + 19,
                        MainForm->gfx->sprites->stand_sheet, MainForm->gfx->sprites->GetStandFrameRect(gender,
                        FacingDirection_Down, skin)) || !i->resources->DrawBackLayerFrom(PlayerFrame_Stand, 34, gender +
                        19, MainForm->gfx->sprites->stand_sheet, MainForm->gfx->sprites->GetStandFrameRect(gender,
                        FacingDirection_Left, skin)))
                        i->reload_requested = true;
                    if (((!gender && boots_library_gender_zero) || (gender == 1 && boots_library_gender_one)) &&
                        boots_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_Stand, 26, 56, boots_library, boots_resource);
                        i->resources->DrawBackLayer(PlayerFrame_Stand, 26, 56, boots_library, boots_resource + 1);
                    }
                    if (((!gender && armor_library_gender_zero) || (gender == 1 && armor_library_gender_one)) &&
                        armor_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_Stand, 26, 7, armor_library, armor_resource);
                        i->resources->DrawBackLayer(PlayerFrame_Stand, 26, 7, armor_library, armor_resource + 1);
                    }
                    if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                        hair_resource >= MIN_PAPERDOLL_RESOURCE && !hair_hidden && !hair_over_hat)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_Stand, 28, 7, hair_library, hair_resource + 1);
                        i->resources->DrawBackLayer(PlayerFrame_Stand, 28, 7, hair_library, hair_resource + 3);
                    }
                    if (((!gender && hat_library_gender_zero) || (gender == 1 && hat_library_gender_one)) &&
                        hat_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayerPixelKeyed(PlayerFrame_Stand, 28, 4, hat_library, hat_resource);
                        i->resources->DrawBackLayerPixelKeyed(PlayerFrame_Stand, 28, 4, hat_library, hat_resource + 2);
                    }
                    if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                        hair_resource >= MIN_PAPERDOLL_RESOURCE && hair_over_hat)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_Stand, 28, 7, hair_library, hair_resource + 1);
                        i->resources->DrawBackLayer(PlayerFrame_Stand, 28, 7, hair_library, hair_resource + 3);
                    }
                    if (kind_skin && !back_shield)
                        i->resources->DrawBackLayerFrom(PlayerFrame_Stand, 39, 17 - i->gender,
                            MainForm->gfx->sprites->kind_sheet, MainForm->gfx->sprites->GetKindFrameRect(gender, 1,
                            i->skin));
                    if (((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one)) &&
                        shield_resource >= MIN_PAPERDOLL_RESOURCE && !back_shield)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_Stand, 19, 34, shield_library, shield_resource);
                        i->resources->DrawBackLayer(PlayerFrame_Stand, 19, 34, shield_library, shield_resource + 1);
                    }
                    if (((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one)) &&
                        shield_resource >= MIN_PAPERDOLL_RESOURCE && back_shield)
                        i->resources->DrawBackLayer(PlayerFrame_Stand, 18, 0, shield_library, shield_resource + 1);
                    if (kind_skin && back_shield)
                        i->resources->DrawBackLayerFrom(PlayerFrame_Stand, 39, 17 - i->gender,
                            MainForm->gfx->sprites->kind_sheet, MainForm->gfx->sprites->GetKindFrameRect(gender, 1,
                            i->skin));
                    i->redraw = true;
                    i->stand_sheet_loaded = true;
                    loaded_count++;
                }
                catch (...)
                {
                }
            }
            if ((i->sitting_on_chair || i->sitting_on_floor || i->chair_sit_start || i->floor_sit_start) &&
                !i->sit_sheets_loaded)
            {
                try
                {
                    LoadGraphicsLibraries(gender);
                    SelectGraphicsLibraries(gender);
                    if (!i->sheets_cleared)
                    {
                        i->resources->ClearSheets();
                        i->sheets_cleared = true;
                    }
                    if (kind_skin)
                        i->resources->DrawFrontLayerFrom(PlayerFrame_ChairSit, 39, 23 - i->gender,
                            MainForm->gfx->sprites->kind_sheet, MainForm->gfx->sprites->GetKindFrameRect(gender, 0,
                            i->skin));
                    if (((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one)) &&
                        shield_resource >= MIN_PAPERDOLL_RESOURCE && back_shield)
                        i->resources->DrawFrontLayer(PlayerFrame_ChairSit, 18, 6, shield_library, shield_resource);
                    if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                        hair_resource >= MIN_PAPERDOLL_RESOURCE && !hair_hidden)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_ChairSit, 28, 13, hair_library, hair_resource);
                        i->resources->DrawBackLayer(PlayerFrame_ChairSit, 28, 13, hair_library, hair_resource + 2);
                    }
                    i->resources->DrawFrontLayerFrom(PlayerFrame_ChairSit, 29 - gender, 27 - gender,
                        MainForm->gfx->sprites->chair_sit_sheet,
                        MainForm->gfx->sprites->GetChairSitFrameRect(gender, FacingDirection_Down, skin));
                    i->resources->DrawBackLayerFrom(PlayerFrame_ChairSit, 28, 27 - gender,
                        MainForm->gfx->sprites->chair_sit_sheet, MainForm->gfx->sprites->GetChairSitFrameRect(gender,
                        FacingDirection_Left, skin));
                    if (((!gender && boots_library_gender_zero) || (gender == 1 && boots_library_gender_one)) &&
                        boots_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_ChairSit, 26, 55, boots_library, boots_resource + 12);
                        i->resources->DrawBackLayer(PlayerFrame_ChairSit, 26, 48, boots_library, boots_resource + 13);
                    }
                    if (((!gender && armor_library_gender_zero) || (gender == 1 && armor_library_gender_one)) &&
                        armor_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_ChairSit, 26, 7, armor_library, armor_resource + 16);
                        i->resources->DrawBackLayer(PlayerFrame_ChairSit, 26, 7, armor_library, armor_resource + 17);
                    }
                    if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                        hair_resource >= MIN_PAPERDOLL_RESOURCE && !hair_hidden && !hair_over_hat)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_ChairSit, 28, 13, hair_library, hair_resource + 1);
                        i->resources->DrawBackLayer(PlayerFrame_ChairSit, 28, 13, hair_library, hair_resource + 3);
                    }
                    if (((!gender && hat_library_gender_zero) || (gender == 1 && hat_library_gender_one)) &&
                        hat_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayerPixelKeyed(PlayerFrame_ChairSit, 28, 10, hat_library, hat_resource);
                        i->resources->DrawBackLayerPixelKeyed(PlayerFrame_ChairSit, 28, 10, hat_library, hat_resource +
                            2);
                    }
                    if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                        hair_resource >= MIN_PAPERDOLL_RESOURCE && hair_over_hat)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_ChairSit, 28, 13, hair_library, hair_resource + 1);
                        i->resources->DrawBackLayer(PlayerFrame_ChairSit, 28, 13, hair_library, hair_resource + 3);
                    }
                    if (((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one)) &&
                        shield_resource >= MIN_PAPERDOLL_RESOURCE && back_shield)
                        i->resources->DrawBackLayer(PlayerFrame_ChairSit, 18, 6, shield_library, shield_resource + 1);
                    if (kind_skin)
                        i->resources->DrawBackLayerFrom(PlayerFrame_ChairSit, 39, 23 - i->gender,
                            MainForm->gfx->sprites->kind_sheet, MainForm->gfx->sprites->GetKindFrameRect(gender, 1,
                            i->skin));
                    if (kind_skin)
                        i->resources->DrawFrontLayerFrom(PlayerFrame_FloorSit, 39, 24 - i->gender,
                            MainForm->gfx->sprites->kind_sheet, MainForm->gfx->sprites->GetKindFrameRect(gender, 0,
                            i->skin));
                    if (((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one)) &&
                        shield_resource >= MIN_PAPERDOLL_RESOURCE && back_shield)
                        i->resources->DrawFrontLayer(PlayerFrame_FloorSit, 18, 6, shield_library, shield_resource);
                    if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                        hair_resource >= MIN_PAPERDOLL_RESOURCE && !hair_hidden)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_FloorSit, 28, 13, hair_library, hair_resource);
                        i->resources->DrawBackLayer(PlayerFrame_FloorSit, gender + 30, 13, hair_library, hair_resource +
                            2);
                    }
                    i->resources->DrawFrontLayerFrom(PlayerFrame_FloorSit, 29 - gender, 27 - gender,
                        MainForm->gfx->sprites->floor_sit_sheet,
                        MainForm->gfx->sprites->GetFloorSitFrameRect(gender, FacingDirection_Down, skin));
                    i->resources->DrawBackLayerFrom(PlayerFrame_FloorSit, 29, 27 - gender,
                        MainForm->gfx->sprites->floor_sit_sheet, MainForm->gfx->sprites->GetFloorSitFrameRect(gender,
                        FacingDirection_Left, skin));
                    if (((!gender && boots_library_gender_zero) || (gender == 1 && boots_library_gender_one)) &&
                        boots_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_FloorSit, gender + 25, gender + 48, boots_library,
                            boots_resource + 14);
                        i->resources->DrawBackLayer(PlayerFrame_FloorSit, 27, 41, boots_library, boots_resource + 15);
                    }
                    if (((!gender && armor_library_gender_zero) || (gender == 1 && armor_library_gender_one)) &&
                        armor_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_FloorSit, gender + 25, 7, armor_library,
                            armor_resource + 18);
                        i->resources->DrawBackLayer(PlayerFrame_FloorSit, 27, 7, armor_library, armor_resource + 19);
                    }
                    if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                        hair_resource >= MIN_PAPERDOLL_RESOURCE && !hair_hidden && !hair_over_hat)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_FloorSit, 28, 13, hair_library, hair_resource + 1);
                        i->resources->DrawBackLayer(PlayerFrame_FloorSit, gender + 30, 13, hair_library, hair_resource +
                            3);
                    }
                    if (((!gender && hat_library_gender_zero) || (gender == 1 && hat_library_gender_one)) &&
                        hat_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayerPixelKeyed(PlayerFrame_FloorSit, 28, 10, hat_library, hat_resource);
                        i->resources->DrawBackLayerPixelKeyed(PlayerFrame_FloorSit, gender + 30, 10, hat_library,
                            hat_resource + 2);
                    }
                    if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                        hair_resource >= MIN_PAPERDOLL_RESOURCE && hair_over_hat)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_FloorSit, 28, 13, hair_library, hair_resource + 1);
                        i->resources->DrawBackLayer(PlayerFrame_FloorSit, gender + 30, 13, hair_library, hair_resource +
                            3);
                    }
                    if (((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one)) &&
                        shield_resource >= MIN_PAPERDOLL_RESOURCE && back_shield)
                        i->resources->DrawBackLayer(PlayerFrame_FloorSit, 21, 6, shield_library, shield_resource + 1);
                    if (kind_skin)
                        i->resources->DrawBackLayerFrom(PlayerFrame_FloorSit, 39, 24 - i->gender,
                            MainForm->gfx->sprites->kind_sheet,
                            MainForm->gfx->sprites->GetKindFrameRect(gender, 1, i->skin));
                    i->redraw = true;
                    i->sit_sheets_loaded = true;
                    loaded_count++;
                }
                catch (...)
                {
                }
            }
            // The sitting frames, on a chair or on the floor.
            if ((i->walking || i->walk_start || i->jumping || i->jump_start ||
                MainForm->net_client->GetPlayerId() == i->player_id) && !i->walk_sheets_loaded)
            {
                try
                {
                    LoadGraphicsLibraries(gender);
                    SelectGraphicsLibraries(gender);
                    // The walking frames; the player's own character loads them even while standing.
                    if (!i->sheets_cleared)
                    {
                        i->resources->ClearSheets();
                        i->sheets_cleared = true;
                    }
                    if (kind_skin)
                    {
                        for (int kind_walk_frame = 1; kind_walk_frame <= 2; kind_walk_frame++)
                            i->resources->DrawFrontLayerFrom(kind_walk_frame, 39, 17 - i->gender,
                                MainForm->gfx->sprites->kind_sheet,
                                MainForm->gfx->sprites->GetKindFrameRect(gender, 0, i->skin));
                    }
                    if ((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one))
                    {
                        for (int back_shield_walk_sheet = 1; back_shield_walk_sheet <= 2; back_shield_walk_sheet++)
                        {
                            if (shield_resource >= MIN_PAPERDOLL_RESOURCE && back_shield)
                                i->resources->DrawFrontLayer(back_shield_walk_sheet, 18, 1, shield_library,
                                    shield_resource);
                        }
                    }
                    if ((!gender && weapon_library_gender_zero) || (gender == 1 && weapon_library_gender_one))
                    {
                        for (int weapon_walk_frame = 1; weapon_walk_frame <= 2; weapon_walk_frame++)
                        {
                            if (weapon_resource >= MIN_PAPERDOLL_RESOURCE)
                            {
                                i->resources->DrawFrontLayer(weapon_walk_frame, 0, 0, weapon_library,
                                    weapon_resource + weapon_walk_frame + 1);
                                i->resources->DrawBackLayer(weapon_walk_frame, 0, 0, weapon_library,
                                    weapon_resource + weapon_walk_frame + 5);
                            }
                        }
                    }
                    if ((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one))
                    {
                        for (int hair_walk_frame = 1; hair_walk_frame <= 2; hair_walk_frame++)
                        {
                            if (hair_resource >= MIN_PAPERDOLL_RESOURCE && !hair_hidden)
                            {
                                i->resources->DrawFrontLayer(hair_walk_frame, 28, 8, hair_library, hair_resource);
                                i->resources->DrawBackLayer(hair_walk_frame, 28, 8, hair_library, hair_resource + 2);
                            }
                        }
                    }
                    i->resources->DrawFrontLayerFrom(PlayerFrame_Walk1, 30 - gender, gender + 19,
                        MainForm->gfx->sprites->walk_sheet, MainForm->gfx->sprites->GetWalkFrameRect(gender,
                        FacingDirection_Down, skin, 0));
                    i->resources->DrawFrontLayerFrom(PlayerFrame_Walk2, 30 - gender, gender + 19,
                        MainForm->gfx->sprites->walk_sheet, MainForm->gfx->sprites->GetWalkFrameRect(gender,
                        FacingDirection_Down, skin, 1));
                    i->resources->DrawBackLayerFrom(PlayerFrame_Walk1, 30, gender + 19,
                        MainForm->gfx->sprites->walk_sheet, MainForm->gfx->sprites->GetWalkFrameRect(gender,
                        FacingDirection_Left, skin, 0));
                    i->resources->DrawBackLayerFrom(PlayerFrame_Walk2, 30, gender + 19,
                        MainForm->gfx->sprites->walk_sheet, MainForm->gfx->sprites->GetWalkFrameRect(gender,
                        FacingDirection_Left, skin, 1));
                    if ((!gender && boots_library_gender_zero) || (gender == 1 && boots_library_gender_one))
                    {
                        if (boots_resource >= MIN_PAPERDOLL_RESOURCE)
                        {
                            for (int boots_walk_sheet = 1; boots_walk_sheet <= 2; boots_walk_sheet++)
                            {
                                i->resources->DrawFrontLayer(boots_walk_sheet, 26, 56, boots_library,
                                    boots_resource + boots_walk_sheet + 1);
                                i->resources->DrawBackLayer(boots_walk_sheet, 26, 56, boots_library,
                                    boots_resource + boots_walk_sheet + 5);
                            }
                        }
                    }
                    if ((!gender && armor_library_gender_zero) || (gender == 1 && armor_library_gender_one))
                    {
                        if (armor_resource >= MIN_PAPERDOLL_RESOURCE)
                        {
                            for (int armor_walk_sheet = 1; armor_walk_sheet <= 2; armor_walk_sheet++)
                            {
                                i->resources->DrawFrontLayer(armor_walk_sheet, 26, 7, armor_library,
                                    armor_resource + armor_walk_sheet + 1);
                                i->resources->DrawBackLayer(armor_walk_sheet, 26, 7, armor_library,
                                    armor_resource + armor_walk_sheet + 5);
                            }
                        }
                    }
                    if ((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one))
                    {
                        for (int hair_top_walk_frame = 1; hair_top_walk_frame <= 2; hair_top_walk_frame++)
                        {
                            if (hair_resource >= MIN_PAPERDOLL_RESOURCE && !hair_hidden && !hair_over_hat)
                            {
                                i->resources->DrawFrontLayer(hair_top_walk_frame, 28, 8, hair_library,
                                    hair_resource + 1);
                                i->resources->DrawBackLayer(hair_top_walk_frame, 28, 8, hair_library,
                                    hair_resource + 3);
                            }
                        }
                    }
                    if ((!gender && hat_library_gender_zero) || (gender == 1 && hat_library_gender_one))
                    {
                        for (int hat_walk_frame = 1; hat_walk_frame <= 2; hat_walk_frame++)
                        {
                            if (hat_resource >= MIN_PAPERDOLL_RESOURCE)
                            {
                                i->resources->DrawFrontLayerPixelKeyed(hat_walk_frame, 28, 5, hat_library,
                                    hat_resource);
                                i->resources->DrawBackLayerPixelKeyed(hat_walk_frame, 28, 5, hat_library,
                                    hat_resource + 2);
                            }
                        }
                    }
                    if ((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one))
                    {
                        for (int late_hair_walk_sheet = 1; late_hair_walk_sheet <= 2; late_hair_walk_sheet++)
                        {
                            if (hair_resource >= MIN_PAPERDOLL_RESOURCE && hair_over_hat)
                            {
                                i->resources->DrawFrontLayer(late_hair_walk_sheet, 28, 8, hair_library,
                                    hair_resource + 1);
                                i->resources->DrawBackLayer(late_hair_walk_sheet, 28, 8, hair_library,
                                    hair_resource + 3);
                            }
                        }
                    }
                    if (kind_skin && !back_shield)
                    {
                        for (int kind_back_walk_frame = 1; kind_back_walk_frame <= 2; kind_back_walk_frame++)
                            i->resources->DrawBackLayerFrom(kind_back_walk_frame, 39, 17 - i->gender,
                                MainForm->gfx->sprites->kind_sheet,
                                MainForm->gfx->sprites->GetKindFrameRect(gender, 1, i->skin));
                    }
                    if ((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one))
                    {
                        for (int shield_walk_frame = 1; shield_walk_frame <= 2; shield_walk_frame++)
                        {
                            if (shield_resource >= MIN_PAPERDOLL_RESOURCE && !back_shield)
                            {
                                i->resources->DrawFrontLayer(shield_walk_frame, 19, 34, shield_library,
                                    shield_resource + shield_walk_frame + 1);
                                i->resources->DrawBackLayer(shield_walk_frame, 19, 34, shield_library,
                                    shield_resource + shield_walk_frame + 5);
                            }
                        }
                    }
                    if ((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one))
                    {
                        for (int back_shield_rear_walk_sheet = 1; back_shield_rear_walk_sheet <= 2;
                            back_shield_rear_walk_sheet++)
                        {
                            if (shield_resource >= MIN_PAPERDOLL_RESOURCE && back_shield)
                                i->resources->DrawBackLayer(back_shield_rear_walk_sheet, 18, 1, shield_library,
                                    shield_resource + 1);
                        }
                    }
                    if (kind_skin && back_shield)
                    {
                        for (int kind_item_walk_frame = 1; kind_item_walk_frame <= 2; kind_item_walk_frame++)
                            i->resources->DrawBackLayerFrom(kind_item_walk_frame, 39, 17 - i->gender,
                                MainForm->gfx->sprites->kind_sheet,
                                MainForm->gfx->sprites->GetKindFrameRect(gender, 1, i->skin));
                    }
                    i->redraw = true;
                    i->walk_sheets_loaded = true;
                    loaded_count++;
                }
                catch (...)
                {
                }
            }
            if ((i->walking || i->walk_start || i->jumping || i->jump_start) && !i->walk_tail_sheets_loaded)
            {
                try
                {
                    LoadGraphicsLibraries(gender);
                    SelectGraphicsLibraries(gender);
                    if (!i->sheets_cleared)
                    {
                        i->resources->ClearSheets();
                        i->sheets_cleared = true;
                    }
                    if (kind_skin)
                    {
                        for (int kind_tail_frame = 3; kind_tail_frame <= 4; kind_tail_frame++)
                            i->resources->DrawFrontLayerFrom(kind_tail_frame, 39, 18 - i->gender,
                                MainForm->gfx->sprites->kind_sheet,
                                MainForm->gfx->sprites->GetKindFrameRect(gender, 0, i->skin));
                    }
                    if ((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one))
                    {
                        for (int back_shield_tail_sheet = 3; back_shield_tail_sheet <= 4; back_shield_tail_sheet++)
                        {
                            if (shield_resource >= MIN_PAPERDOLL_RESOURCE && back_shield)
                                i->resources->DrawFrontLayer(back_shield_tail_sheet, 18, 1, shield_library,
                                    shield_resource);
                        }
                    }
                    if ((!gender && weapon_library_gender_zero) || (gender == 1 && weapon_library_gender_one))
                    {
                        for (int j = 3; j <= 4; j++)
                        {
                            if (weapon_resource >= MIN_PAPERDOLL_RESOURCE)
                            {
                                i->resources->DrawFrontLayer(j, 0, 0, weapon_library, j + weapon_resource + 1);
                                i->resources->DrawBackLayer(j, 0, 0, weapon_library, j + weapon_resource + 5);
                            }
                        }
                    }
                    if ((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one))
                    {
                        for (int k = 3; k <= 4; k++)
                        {
                            if (hair_resource >= MIN_PAPERDOLL_RESOURCE && !hair_hidden)
                            {
                                i->resources->DrawFrontLayer(k, 28, 8, hair_library, hair_resource);
                                i->resources->DrawBackLayer(k, 28, 8, hair_library, hair_resource + 2);
                            }
                        }
                    }
                    i->resources->DrawFrontLayerFrom(PlayerFrame_Walk3, 30 - gender, gender + 19,
                        MainForm->gfx->sprites->walk_sheet, MainForm->gfx->sprites->GetWalkFrameRect(gender,
                        FacingDirection_Down, skin, 2));
                    i->resources->DrawFrontLayerFrom(PlayerFrame_Walk4, 30 - gender, gender + 19,
                        MainForm->gfx->sprites->walk_sheet, MainForm->gfx->sprites->GetWalkFrameRect(gender,
                        FacingDirection_Down, skin, 3));
                    i->resources->DrawBackLayerFrom(PlayerFrame_Walk3, 30, gender + 19,
                        MainForm->gfx->sprites->walk_sheet, MainForm->gfx->sprites->GetWalkFrameRect(gender,
                        FacingDirection_Left, skin, 2));
                    i->resources->DrawBackLayerFrom(PlayerFrame_Walk4, 30, gender + 19,
                        MainForm->gfx->sprites->walk_sheet, MainForm->gfx->sprites->GetWalkFrameRect(gender,
                        FacingDirection_Left, skin, 3));
                    if ((!gender && boots_library_gender_zero) || (gender == 1 && boots_library_gender_one))
                    {
                        if (boots_resource >= MIN_PAPERDOLL_RESOURCE)
                        {
                            for (int boots_tail_frame = 3; boots_tail_frame <= 4; boots_tail_frame++)
                            {
                                i->resources->DrawFrontLayer(boots_tail_frame, 26, 56, boots_library,
                                    boots_resource + boots_tail_frame + 1);
                                i->resources->DrawBackLayer(boots_tail_frame, 26, 56, boots_library,
                                    boots_resource + boots_tail_frame + 5);
                            }
                        }
                    }
                    if ((!gender && armor_library_gender_zero) || (gender == 1 && armor_library_gender_one))
                    {
                        if (armor_resource >= MIN_PAPERDOLL_RESOURCE)
                        {
                            for (int armor_tail_frame = 3; armor_tail_frame <= 4; armor_tail_frame++)
                            {
                                i->resources->DrawFrontLayer(armor_tail_frame, 26, 7, armor_library,
                                    armor_resource + armor_tail_frame + 1);
                                i->resources->DrawBackLayer(armor_tail_frame, 26, 7, armor_library,
                                    armor_resource + armor_tail_frame + 5);
                            }
                        }
                    }
                    if ((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one))
                    {
                        for (int m = 3; m <= 4; m++)
                        {
                            if (hair_resource >= MIN_PAPERDOLL_RESOURCE && !hair_hidden && !hair_over_hat)
                            {
                                i->resources->DrawFrontLayer(m, 28, 8, hair_library, hair_resource + 1);
                                i->resources->DrawBackLayer(m, 28, 8, hair_library, hair_resource + 3);
                            }
                        }
                    }
                    if ((!gender && hat_library_gender_zero) || (gender == 1 && hat_library_gender_one))
                    {
                        for (int ii = 3; ii <= 4; ii++)
                        {
                            if (hat_resource >= MIN_PAPERDOLL_RESOURCE)
                            {
                                i->resources->DrawFrontLayerPixelKeyed(ii, 28, 5, hat_library, hat_resource);
                                i->resources->DrawBackLayerPixelKeyed(ii, 28, 5, hat_library, hat_resource + 2);
                            }
                        }
                    }
                    if ((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one))
                    {
                        for (int late_hair_tail_sheet = 3; late_hair_tail_sheet <= 4; late_hair_tail_sheet++)
                        {
                            if (hair_resource >= MIN_PAPERDOLL_RESOURCE && hair_over_hat)
                            {
                                i->resources->DrawFrontLayer(late_hair_tail_sheet, 28, 8, hair_library,
                                    hair_resource + 1);
                                i->resources->DrawBackLayer(late_hair_tail_sheet, 28, 8, hair_library,
                                    hair_resource + 3);
                            }
                        }
                    }
                    if (kind_skin && !back_shield)
                    {
                        for (int kind_back_tail_frame = 3; kind_back_tail_frame <= 4; kind_back_tail_frame++)
                            i->resources->DrawBackLayerFrom(kind_back_tail_frame, 39, 18 - i->gender,
                                MainForm->gfx->sprites->kind_sheet,
                                MainForm->gfx->sprites->GetKindFrameRect(gender, 1, i->skin));
                    }
                    if ((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one))
                    {
                        for (int kk = 3; kk <= 4; kk++)
                        {
                            if (shield_resource >= MIN_PAPERDOLL_RESOURCE && !back_shield)
                            {
                                i->resources->DrawFrontLayer(kk, 19, 34, shield_library, kk + shield_resource + 1);
                                i->resources->DrawBackLayer(kk, 19, 34, shield_library, kk + shield_resource + 5);
                            }
                        }
                    }
                    if ((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one))
                    {
                        for (int back_shield_rear_tail_sheet = 3; back_shield_rear_tail_sheet <= 4;
                            back_shield_rear_tail_sheet++)
                        {
                            if (shield_resource >= MIN_PAPERDOLL_RESOURCE && back_shield)
                                i->resources->DrawBackLayer(back_shield_rear_tail_sheet, 18, 1, shield_library,
                                    shield_resource + 1);
                        }
                    }
                    if (kind_skin)
                    {
                        if (back_shield)
                        {
                            for (int kind_item_tail_frame = 3; kind_item_tail_frame <= 4; kind_item_tail_frame++)
                                i->resources->DrawBackLayerFrom(kind_item_tail_frame, 39, 18 - i->gender,
                                    MainForm->gfx->sprites->kind_sheet,
                                    MainForm->gfx->sprites->GetKindFrameRect(gender, 1, i->skin));
                        }
                    }
                    i->redraw = true;
                    i->walk_tail_sheets_loaded = true;
                    loaded_count++;
                }
                catch (...)
                {
                }
            }
            if ((i->attacking || i->attack_start) && !i->attack_sheets_loaded)
            {
                try
                {
                    LoadGraphicsLibraries(gender);
                    SelectGraphicsLibraries(gender);
                    if (!i->sheets_cleared)
                    {
                        i->resources->ClearSheets();
                        i->sheets_cleared = true;
                    }
                    if (kind_skin)
                    {
                        i->resources->DrawFrontLayerFrom(PlayerFrame_Attack1, 40, 18 - i->gender,
                            MainForm->gfx->sprites->kind_sheet, MainForm->gfx->sprites->GetKindFrameRect(gender, 2,
                            i->skin));
                        i->resources->DrawFrontLayerFrom(PlayerFrame_Attack2, 41, 18 - i->gender,
                            MainForm->gfx->sprites->kind_sheet, MainForm->gfx->sprites->GetKindFrameRect(gender, 0,
                            i->skin));
                        i->resources->DrawFrontLayerFrom(PlayerFrame_RangedAttack, 43, 16 - i->gender,
                            MainForm->gfx->sprites->kind_sheet, MainForm->gfx->sprites->GetKindFrameRect(gender, 2,
                            i->skin));
                    }
                    if (((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one)) &&
                        shield_resource >= MIN_PAPERDOLL_RESOURCE && back_shield)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_Attack1, 18, 0, shield_library, shield_resource + 2);
                        if (!MainForm->weapon_mapper->IsRangedWeapon(i->weapon))
                            i->resources->DrawFrontLayer(PlayerFrame_Attack2, 18, 0, shield_library, shield_resource +
                                2);
                    }
                    if (((!gender && weapon_library_gender_zero) || (gender == 1 && weapon_library_gender_one)) &&
                        weapon_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_Attack1, 0, 0, weapon_library, weapon_resource + 12);
                        i->resources->DrawBackLayer(PlayerFrame_Attack1, 0, 0, weapon_library, weapon_resource + 14);
                        if (!MainForm->weapon_mapper->IsRangedWeapon(i->weapon))
                        {
                            i->resources->DrawFrontLayer(PlayerFrame_Attack2, 0, 0, weapon_library, weapon_resource +
                                13);
                            i->resources->DrawBackLayer(PlayerFrame_Attack2, 0, 0, weapon_library, weapon_resource +
                                15);
                        }
                    }
                    if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                        hair_resource >= MIN_PAPERDOLL_RESOURCE && !hair_hidden)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_Attack1, 28, 7, hair_library, hair_resource);
                        i->resources->DrawBackLayer(PlayerFrame_Attack1, 28, 7, hair_library, hair_resource + 2);
                        if (!MainForm->weapon_mapper->IsRangedWeapon(i->weapon))
                        {
                            i->resources->DrawFrontLayer(PlayerFrame_Attack2, 26, 12 - gender, hair_library,
                                hair_resource);
                            i->resources->DrawBackLayer(PlayerFrame_Attack2, 2 * gender + 26, 8, hair_library,
                                hair_resource + 2);
                        }
                    }
                    i->resources->DrawFrontLayerFrom(PlayerFrame_Attack1, 30 - gender, gender + 19,
                        MainForm->gfx->sprites->attack_sheet, MainForm->gfx->sprites->GetAttackFrameRect(gender,
                        FacingDirection_Down, skin, 0));
                    i->resources->DrawBackLayerFrom(PlayerFrame_Attack1, 30 - gender, gender + 19,
                        MainForm->gfx->sprites->attack_sheet, MainForm->gfx->sprites->GetAttackFrameRect(gender,
                        FacingDirection_Left, skin, 0));
                    if (!MainForm->weapon_mapper->IsRangedWeapon(i->weapon))
                    {
                        i->resources->DrawFrontLayerFrom(PlayerFrame_Attack2, gender + 32, gender + 19,
                            MainForm->gfx->sprites->attack_sheet,
                            MainForm->gfx->sprites->GetAttackFrameRect(gender, FacingDirection_Down, skin, 1));
                        i->resources->DrawBackLayerFrom(PlayerFrame_Attack2, gender + 32, gender + 19,
                            MainForm->gfx->sprites->attack_sheet,
                            MainForm->gfx->sprites->GetAttackFrameRect(gender, FacingDirection_Left, skin, 1));
                    }
                    if (((!gender && boots_library_gender_zero) || (gender == 1 && boots_library_gender_one)) &&
                        boots_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_Attack1, 26, 56, boots_library, boots_resource);
                        i->resources->DrawBackLayer(PlayerFrame_Attack1, 26, 56, boots_library, boots_resource + 1);
                        if (!MainForm->weapon_mapper->IsRangedWeapon(i->weapon))
                        {
                            i->resources->DrawFrontLayer(PlayerFrame_Attack2, 26, 56, boots_library, boots_resource +
                                10);
                            i->resources->DrawBackLayer(PlayerFrame_Attack2, 26, 56, boots_library, boots_resource +
                                11);
                        }
                    }
                    if (((!gender && armor_library_gender_zero) || (gender == 1 && armor_library_gender_one)) &&
                        armor_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_Attack1, 26, 7, armor_library, armor_resource + 12);
                        i->resources->DrawBackLayer(PlayerFrame_Attack1, 26, 7, armor_library, armor_resource + 14);
                        if (!MainForm->weapon_mapper->IsRangedWeapon(i->weapon))
                        {
                            i->resources->DrawFrontLayer(PlayerFrame_Attack2, 26, 7, armor_library, armor_resource +
                                13);
                            i->resources->DrawBackLayer(PlayerFrame_Attack2, 26, 7, armor_library, armor_resource + 15);
                        }
                    }
                    if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                        hair_resource >= MIN_PAPERDOLL_RESOURCE && !hair_hidden && !hair_over_hat)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_Attack1, 28, 7, hair_library, hair_resource + 1);
                        i->resources->DrawBackLayer(PlayerFrame_Attack1, 28, 7, hair_library, hair_resource + 3);
                        if (!MainForm->weapon_mapper->IsRangedWeapon(i->weapon))
                        {
                            i->resources->DrawFrontLayer(PlayerFrame_Attack2, 26, 12 - gender, hair_library,
                                hair_resource + 1);
                            i->resources->DrawBackLayer(PlayerFrame_Attack2, 2 * gender + 26, 8, hair_library,
                                hair_resource + 3);
                        }
                    }
                    if (((!gender && hat_library_gender_zero) || (gender == 1 && hat_library_gender_one)) &&
                        hat_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayerPixelKeyed(PlayerFrame_Attack1, 28, 4, hat_library, hat_resource);
                        i->resources->DrawBackLayerPixelKeyed(PlayerFrame_Attack1, 28, 4, hat_library, hat_resource +
                            2);
                        if (!MainForm->weapon_mapper->IsRangedWeapon(i->weapon))
                        {
                            i->resources->DrawFrontLayerPixelKeyed(PlayerFrame_Attack2, 26, 9 - gender, hat_library,
                                hat_resource);
                            i->resources->DrawBackLayerPixelKeyed(PlayerFrame_Attack2, 2 * gender + 26, 5, hat_library,
                                hat_resource + 2);
                        }
                    }
                    if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                        hair_resource >= MIN_PAPERDOLL_RESOURCE && hair_over_hat)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_Attack1, 28, 7, hair_library, hair_resource + 1);
                        i->resources->DrawBackLayer(PlayerFrame_Attack1, 28, 7, hair_library, hair_resource + 3);
                        if (!MainForm->weapon_mapper->IsRangedWeapon(i->weapon))
                        {
                            i->resources->DrawFrontLayer(PlayerFrame_Attack2, 26, 12 - gender, hair_library,
                                hair_resource + 1);
                            i->resources->DrawBackLayer(PlayerFrame_Attack2, 2 * gender + 26, 8, hair_library,
                                hair_resource + 3);
                        }
                    }
                    if (kind_skin && !back_shield)
                    {
                        i->resources->DrawBackLayerFrom(PlayerFrame_Attack1, 40, 17 - i->gender,
                            MainForm->gfx->sprites->kind_sheet, MainForm->gfx->sprites->GetKindFrameRect(gender, 3,
                            i->skin));
                        i->resources->DrawBackLayerFrom(PlayerFrame_Attack2, 41, 17 - i->gender,
                            MainForm->gfx->sprites->kind_sheet, MainForm->gfx->sprites->GetKindFrameRect(gender, 1,
                            i->skin));
                    }
                    if (((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one)) &&
                        shield_resource >= MIN_PAPERDOLL_RESOURCE && !back_shield)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_Attack1, 19, 34, shield_library, shield_resource + 12);
                        i->resources->DrawBackLayer(PlayerFrame_Attack1, 19, 34, shield_library, shield_resource + 14);
                        if (!MainForm->weapon_mapper->IsRangedWeapon(i->weapon))
                        {
                            i->resources->DrawFrontLayer(PlayerFrame_Attack2, 19, 34, shield_library, shield_resource +
                                13);
                            i->resources->DrawBackLayer(PlayerFrame_Attack2, 19, 34, shield_library, shield_resource +
                                15);
                        }
                    }
                    if (((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one)) &&
                        shield_resource >= MIN_PAPERDOLL_RESOURCE && back_shield)
                    {
                        i->resources->DrawBackLayer(PlayerFrame_Attack1, 18, 0, shield_library, shield_resource + 3);
                        if (!MainForm->weapon_mapper->IsRangedWeapon(i->weapon))
                            i->resources->DrawBackLayer(PlayerFrame_Attack2, 18, 0, shield_library, shield_resource +
                                3);
                    }
                    if (kind_skin && back_shield)
                    {
                        i->resources->DrawBackLayerFrom(PlayerFrame_Attack1, 40, 17 - i->gender,
                            MainForm->gfx->sprites->kind_sheet, MainForm->gfx->sprites->GetKindFrameRect(gender, 3,
                            i->skin));
                        i->resources->DrawBackLayerFrom(PlayerFrame_Attack2, 41, 17 - i->gender,
                            MainForm->gfx->sprites->kind_sheet, MainForm->gfx->sprites->GetKindFrameRect(gender, 1,
                            i->skin));
                    }
                    if (!MainForm->weapon_mapper->IsRangedWeapon(i->weapon))
                    {
                        if (((!gender && weapon_library_gender_zero) || (gender == 1 && weapon_library_gender_one)) &&
                            weapon_resource >= MIN_PAPERDOLL_RESOURCE)
                            i->resources->DrawFrontLayer(PlayerFrame_Attack2, 0, 0, weapon_library, weapon_resource +
                                16);
                    }
                    else
                    {
                        if (((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one)) &&
                            shield_resource >= MIN_PAPERDOLL_RESOURCE && back_shield)
                            i->resources->DrawFrontLayer(PlayerFrame_RangedAttack, 21, 0, shield_library,
                                shield_resource + 2);
                        if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                            hair_resource >= MIN_PAPERDOLL_RESOURCE && !hair_hidden)
                        {
                            i->resources->DrawFrontLayer(PlayerFrame_RangedAttack, 2 - 2 * gender + 31, 7 - (1 -
                                gender), hair_library, hair_resource);
                            i->resources->DrawBackLayer(PlayerFrame_RangedAttack, 2 - 2 * gender + 29, 7, hair_library,
                                hair_resource + 2);
                        }
                        if (MainForm->weapon_mapper->IsRangedWeapon(i->weapon) &&
                            ((!gender && weapon_library_gender_zero) || (gender == 1 && weapon_library_gender_one)) &&
                            weapon_resource >= MIN_PAPERDOLL_RESOURCE)
                        {
                            i->resources->DrawFrontLayer(PlayerFrame_RangedAttack, 0, 0, weapon_library,
                                weapon_resource + 17);
                            i->resources->DrawBackLayer(PlayerFrame_RangedAttack, 0, 0, weapon_library,
                                weapon_resource + 18);
                        }
                        i->resources->DrawFrontLayerFrom(PlayerFrame_RangedAttack, 30 - gender, gender + 19,
                            MainForm->gfx->sprites->ranged_attack_sheet,
                            MainForm->gfx->sprites->GetRangedAttackFrameRect(gender, FacingDirection_Down, skin));
                        i->resources->DrawBackLayerFrom(PlayerFrame_RangedAttack, 30 - gender, gender + 19,
                            MainForm->gfx->sprites->ranged_attack_sheet,
                            MainForm->gfx->sprites->GetRangedAttackFrameRect(gender, FacingDirection_Left, skin));
                        if (((!gender && boots_library_gender_zero) || (gender == 1 && boots_library_gender_one)) &&
                            boots_resource >= MIN_PAPERDOLL_RESOURCE)
                        {
                            i->resources->DrawFrontLayer(PlayerFrame_RangedAttack, 27 - gender, 56, boots_library,
                                boots_resource + 10);
                            i->resources->DrawBackLayer(PlayerFrame_RangedAttack, 27 - 4 * gender, 56, boots_library,
                                boots_resource + 11);
                        }
                        if (((!gender && armor_library_gender_zero) || (gender == 1 && armor_library_gender_one)) &&
                            armor_resource >= MIN_PAPERDOLL_RESOURCE)
                        {
                            i->resources->DrawFrontLayer(PlayerFrame_RangedAttack, 26, 7, armor_library,
                                armor_resource + 20);
                            i->resources->DrawBackLayer(PlayerFrame_RangedAttack, 26, 7, armor_library, armor_resource +
                                21);
                        }
                        if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                            hair_resource >= MIN_PAPERDOLL_RESOURCE && !hair_hidden && !hair_over_hat)
                        {
                            i->resources->DrawFrontLayer(PlayerFrame_RangedAttack, 2 - 2 * gender + 31, 7 - (1 -
                                gender), hair_library, hair_resource + 1);
                            i->resources->DrawBackLayer(PlayerFrame_RangedAttack, 2 - 2 * gender + 29, 7, hair_library,
                                hair_resource + 3);
                        }
                        if (((!gender && hat_library_gender_zero) || (gender == 1 && hat_library_gender_one)) &&
                            hat_resource >= MIN_PAPERDOLL_RESOURCE)
                        {
                            i->resources->DrawFrontLayerPixelKeyed(PlayerFrame_RangedAttack, 33 - 2 * gender, gender +
                                3, hat_library, hat_resource);
                            i->resources->DrawBackLayerPixelKeyed(PlayerFrame_RangedAttack, 31 - 4 * gender + 2 *
                                gender, 4, hat_library, hat_resource + 2);
                        }
                        if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                            hair_resource >= MIN_PAPERDOLL_RESOURCE && hair_over_hat)
                        {
                            i->resources->DrawFrontLayer(PlayerFrame_RangedAttack, 2 - 2 * gender + 31, 7 - (1 -
                                gender), hair_library, hair_resource + 1);
                            i->resources->DrawBackLayer(PlayerFrame_RangedAttack, 2 - 2 * gender + 29, 7, hair_library,
                                hair_resource + 3);
                        }
                        if (((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one)) &&
                            shield_resource >= MIN_PAPERDOLL_RESOURCE && back_shield)
                        {
                            unsigned char item_shift = 0;
                            if (i->gender == 1)
                                item_shift = 2;
                            TPoint item_point =
                                i->resources->DrawBackLayer(PlayerFrame_RangedAttack, 21 - item_shift, 1,
                                    shield_library, shield_resource + 3);
                        }
                        if (kind_skin)
                        {
                            unsigned char kind_shift = 0;
                            if (i->gender == 1)
                                kind_shift = 2;
                            i->resources->DrawBackLayerFrom(PlayerFrame_RangedAttack, 42 - kind_shift, 17 - i->gender,
                                MainForm->gfx->sprites->kind_sheet,
                                MainForm->gfx->sprites->GetKindFrameRect(gender, 3, i->skin));
                        }
                    }
                    i->redraw = true;
                    i->attack_sheets_loaded = true;
                    loaded_count++;
                }
                catch (...)
                {
                }
            }
            if ((i->casting || i->cast_start) && !i->cast_sheets_loaded)
            {
                try
                {
                    LoadGraphicsLibraries(gender);
                    SelectGraphicsLibraries(gender);
                    if (!i->sheets_cleared)
                    {
                        i->resources->ClearSheets();
                        i->sheets_cleared = true;
                    }
                    if (kind_skin)
                        i->resources->DrawFrontLayerFrom(PlayerFrame_SpellCast, 39, 17 - i->gender,
                            MainForm->gfx->sprites->kind_sheet, MainForm->gfx->sprites->GetKindFrameRect(gender, 0,
                            i->skin));
                    if (((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one)) &&
                        shield_resource >= MIN_PAPERDOLL_RESOURCE && back_shield)
                        i->resources->DrawFrontLayer(PlayerFrame_SpellCast, 18, 0, shield_library, shield_resource);
                    if (((!gender && weapon_library_gender_zero) || (gender == 1 && weapon_library_gender_one)) &&
                        weapon_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_SpellCast, 0, 0, weapon_library, weapon_resource + 10);
                        i->resources->DrawBackLayer(PlayerFrame_SpellCast, 0, 0, weapon_library, weapon_resource + 11);
                    }
                    if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                        hair_resource >= MIN_PAPERDOLL_RESOURCE && !hair_hidden)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_SpellCast, 28, 7, hair_library, hair_resource);
                        i->resources->DrawBackLayer(PlayerFrame_SpellCast, 28, 7, hair_library, hair_resource + 2);
                    }
                    i->resources->DrawFrontLayerFrom(PlayerFrame_SpellCast, 34, gender + 15,
                        MainForm->gfx->sprites->spell_cast_sheet, MainForm->gfx->sprites->GetSpellCastCellRect(gender,
                        FacingDirection_Down, skin));
                    i->resources->DrawBackLayerFrom(PlayerFrame_SpellCast, 34, gender + 15,
                        MainForm->gfx->sprites->spell_cast_sheet, MainForm->gfx->sprites->GetSpellCastCellRect(gender,
                        FacingDirection_Left, skin));
                    if (((!gender && boots_library_gender_zero) || (gender == 1 && boots_library_gender_one)) &&
                        boots_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_SpellCast, 26, 56, boots_library, boots_resource);
                        i->resources->DrawBackLayer(PlayerFrame_SpellCast, 26, 56, boots_library, boots_resource + 1);
                    }
                    if (((!gender && armor_library_gender_zero) || (gender == 1 && armor_library_gender_one)) &&
                        armor_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_SpellCast, 26, 7, armor_library, armor_resource + 10);
                        i->resources->DrawBackLayer(PlayerFrame_SpellCast, 26, 7, armor_library, armor_resource + 11);
                    }
                    if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                        hair_resource >= MIN_PAPERDOLL_RESOURCE && !hair_hidden && !hair_over_hat)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_SpellCast, 28, 7, hair_library, hair_resource + 1);
                        i->resources->DrawBackLayer(PlayerFrame_SpellCast, 28, 7, hair_library, hair_resource + 3);
                    }
                    if (((!gender && hat_library_gender_zero) || (gender == 1 && hat_library_gender_one)) &&
                        hat_resource >= MIN_PAPERDOLL_RESOURCE)
                    {
                        i->resources->DrawFrontLayerPixelKeyed(PlayerFrame_SpellCast, 28, 4, hat_library, hat_resource);
                        i->resources->DrawBackLayerPixelKeyed(PlayerFrame_SpellCast, 28, 4, hat_library, hat_resource +
                            2);
                    }
                    if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                        hair_resource >= MIN_PAPERDOLL_RESOURCE && hair_over_hat)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_SpellCast, 28, 7, hair_library, hair_resource + 1);
                        i->resources->DrawBackLayer(PlayerFrame_SpellCast, 28, 7, hair_library, hair_resource + 3);
                    }
                    if (kind_skin && !back_shield)
                        i->resources->DrawBackLayerFrom(PlayerFrame_SpellCast, 39, 17 - i->gender,
                            MainForm->gfx->sprites->kind_sheet, MainForm->gfx->sprites->GetKindFrameRect(gender, 1,
                            i->skin));
                    if (((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one)) &&
                        shield_resource >= MIN_PAPERDOLL_RESOURCE && !back_shield)
                    {
                        i->resources->DrawFrontLayer(PlayerFrame_SpellCast, 19, 34, shield_library, shield_resource +
                            10);
                        i->resources->DrawBackLayer(PlayerFrame_SpellCast, 19, 34, shield_library, shield_resource +
                            11);
                    }
                    if (((!gender && shield_library_gender_zero) || (gender == 1 && shield_library_gender_one)) &&
                        shield_resource >= MIN_PAPERDOLL_RESOURCE && back_shield)
                        i->resources->DrawBackLayer(PlayerFrame_SpellCast, 18, 0, shield_library, shield_resource + 1);
                    if (kind_skin && back_shield)
                        i->resources->DrawBackLayerFrom(PlayerFrame_SpellCast, 39, 17 - i->gender,
                            MainForm->gfx->sprites->kind_sheet, MainForm->gfx->sprites->GetKindFrameRect(gender, 1,
                            i->skin));
                    i->redraw = true;
                    i->cast_sheets_loaded = true;
                    loaded_count++;
                }
                catch (...)
                {
                }
            }
            if ((i->emoting || i->emote_start) && !i->emote_sheets_loaded)
            {
                try
                {
                    LoadGraphicsLibraries(gender);
                    SelectGraphicsLibraries(gender);
                    if (!i->sheets_cleared)
                    {
                        i->resources->ClearSheets();
                        i->sheets_cleared = true;
                    }
                    MainForm->gfx->sprites->CreateEmoteBuffer();
                    if (((!gender && armor_library_gender_zero) || (gender == 1 && armor_library_gender_one)) &&
                        armor_resource >= MIN_PAPERDOLL_RESOURCE)
                        MainForm->gfx->sprites->DrawLayerToEmoteBuffer(10, 14 - gender, armor_library, armor_resource);
                    if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                        hair_resource >= MIN_PAPERDOLL_RESOURCE && !hair_hidden && !hair_over_hat)
                        MainForm->gfx->sprites->DrawLayerToEmoteBuffer(8, 14 - gender, hair_library, hair_resource + 1);
                    if (((!gender && hat_library_gender_zero) || (gender == 1 && hat_library_gender_one)) &&
                        hat_resource >= MIN_PAPERDOLL_RESOURCE)
                        MainForm->gfx->sprites->DrawLayerToEmoteBufferPixelKeyed(8, 17 - gender, hat_library,
                            hat_resource);
                    if (((!gender && hair_library_gender_zero) || (gender == 1 && hair_library_gender_one)) &&
                        hair_resource >= MIN_PAPERDOLL_RESOURCE && hair_over_hat)
                        MainForm->gfx->sprites->DrawLayerToEmoteBuffer(8, 14 - gender, hair_library, hair_resource + 1);
                    if (MainForm->gfx->sprites->emote_buffer_created)
                    {
                        i->resources->emote_strip->BltFast(0, 0, MainForm->gfx->sprites->face_sheet,
                            &MainForm->gfx->sprites->GetFaceStripRect(gender, skin), DDBLTFAST_WAIT);
                        for (int face_index = 0; face_index < 11; face_index++)
                            i->resources->emote_strip->BltFast(13 * face_index, 0,
                                MainForm->gfx->sprites->emote_buffer->surface,
                                &MainForm->gfx->sprites->emote_buffer->surface_rect,
                                DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
                        MainForm->gfx->sprites->FreeEmoteBuffer();
                    }
                    i->redraw = true;
                    i->emote_sheets_loaded = true;
                    loaded_count++;
                }
                catch (...)
                {
                }
            }
            if (i->sheets_cleared && i->walk_sheets_loaded && i->walk_tail_sheets_loaded && i->stand_sheet_loaded &&
                i->sit_sheets_loaded && i->attack_sheets_loaded && i->extra_sheets_loaded && i->cast_sheets_loaded &&
                i->emote_sheets_loaded)
            {
                i->graphics_pending = false;
                i->redraw = true;
            }
        }
        i++;
    }
}

void Players::HideTextLabel(int player_id)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
            it->text_label_shown = false;
    }
}

// Shows a text label over the player for duration passes, blinking when asked.
void Players::ShowTextLabel(int player_id, String text, int label_style, char blinking, int duration)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->resources->RenderNameTag(text, label_style);
            it->text_label_shown = true;
            it->text_label_blinking = blinking;
            it->text_label_timer = duration;
            break;
        }
    }
}

// Marks every cell of the player to be drawn again, after a change of looks.
void Players::ReloadGraphics(int player_id)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->graphics_pending = true;
            it->sheets_cleared = false;
            it->walk_sheets_loaded = false;
            it->walk_tail_sheets_loaded = false;
            it->stand_sheet_loaded = false;
            it->sit_sheets_loaded = false;
            it->attack_sheets_loaded = false;
            it->extra_sheets_loaded = false;
            it->cast_sheets_loaded = false;
            it->emote_sheets_loaded = false;
            it->load_progress = 0;
            return;
        }
    }
}

// True when the tile is near enough to the centre to be in view; the range is larger past the centre on both axes.
bool Players::IsInViewRange(int center_x, int center_y, int x, int y)
{
    bool in_range = false;
    int dx = x - center_x;
    int dy = y - center_y;
    if (dx < 0)
        dx = 0 - dx;
    if (dy < 0)
        dy = 0 - dy;
    if (center_x < x && center_y < y)
    {
        if (dx + dy <= 14)
            in_range = true;
    }
    else if (dx + dy <= 12)
        in_range = true;
    return in_range;
}

// Sorts the list by y, then by x.
void Players::SortByPosition()
{
    char swapped;
    do
    {
        swapped = false;
        for (int i = 0; i < (int)GetCount() - 1; i++)
        {
            Player handle;
            if (players[i].y == players[i + 1].y)
            {
                if (players[i].x > players[i + 1].x)
                {
                    handle = players[i];
                    players[i] = players[i + 1];
                    players[i + 1] = handle;
                    swapped = true;
                }
            }
            if (players[i].y > players[i + 1].y)
            {
                handle = players[i];
                players[i] = players[i + 1];
                players[i + 1] = handle;
                swapped = true;
            }
        }
    } while (swapped);
}

// Adds the preview player, a template whose looks the SetPreview functions change.
void Players::AddPreviewPlayer(int x, int y)
{
    if (!HasPlayer(PlayerId_Preview))
    {
        Player handle(x, y, "template", 0, 0, 1, 0, true);
        handle.CreateResources();
        handle.boots = 0;
        handle.hat = HatGraphic_None;
        handle.shield = ShieldGraphic_None;
        handle.weapon = 0;
        handle.armor = 0;
        handle.player_id = PlayerId_Preview;
        handle.draw_group = CharacterSlotState_Preview;
        players.insert(players.end(), handle);
    }
}

// Adds the three character-slot players, one per row of the character list.
void Players::AddCharacterSlots()
{
    if (!HasPlayer(PlayerId_SlotFirst))
    {
        Player handle(390, 71, "", 0, 0, 0, 0, false);
        handle.CreateResources();
        handle.player_id = PlayerId_SlotFirst;
        handle.draw_group = CharacterSlotState_Taken;
        players.insert(players.end(), handle);
    }
    if (!HasPlayer(PlayerId_SlotSecond))
    {
        Player second_slot(390, 195, "", 0, 0, 0, 0, false);
        second_slot.CreateResources();
        second_slot.player_id = PlayerId_SlotSecond;
        second_slot.draw_group = CharacterSlotState_Taken;
        players.insert(players.end(), second_slot);
    }
    if (!HasPlayer(PlayerId_SlotThird))
    {
        Player third_slot(390, 319, "", 0, 0, 0, 0, false);
        third_slot.CreateResources();
        third_slot.player_id = PlayerId_SlotThird;
        third_slot.draw_group = CharacterSlotState_Taken;
        players.insert(players.end(), third_slot);
    }
}

void Players::RemovePreviewPlayer()
{
    RemovePlayer(PlayerId_Preview);
}

void Players::SetPreviewPosition(int x, int y)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (it->player_id == PlayerId_Preview)
        {
            it->x = x;
            it->y = y;
            return;
        }
    }
}

void Players::SetPreviewGender(int gender)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (it->player_id == PlayerId_Preview)
        {
            it->gender = gender;
            return;
        }
    }
}

void Players::SetPreviewSkin(int skin)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (it->player_id == PlayerId_Preview)
        {
            it->skin = skin;
            return;
        }
    }
}

void Players::SetPreviewHairStyle(int hair_style)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (it->player_id == PlayerId_Preview)
        {
            it->hair_style = hair_style;
            return;
        }
    }
}

void Players::SetPreviewHairColor(int hair_color)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (it->player_id == PlayerId_Preview)
        {
            it->hair_color = hair_color;
            return;
        }
    }
}

// Adds a player from the nearby-entities packet, unless one with the id is on the list.
// The parameters come in the order of the player's record; the name is the token before it.
void Players::AddPlayer(int player_id, String name, String secondary_name, int position_extra, int x, int y,
    int direction, int record_value_a, String guild_tag, int unused_value, int gender, int hair_style, int hair_color,
    int skin, int equip_slot_a, int equip_slot_b, int equip_slot_c, int equip_slot_d, int boots,
    int equip_after_boots_a, int equip_after_boots_b, int equip_after_armor, int armor, int equip_before_hat, int hat,
    int shield, int weapon, int sit_state, int hidden, int arrival_type)
{
    if (!HasPlayer(player_id))
    {
        Player handle(x, y, name, gender, skin, hair_style, hair_color, true);
        handle.player_id = player_id;
        handle.secondary_name = secondary_name;
        handle.position_extra = position_extra;
        handle.direction = direction;
        handle.record_value_a = record_value_a;
        handle.guild_tag = guild_tag;
        handle.hair_style = hair_style;
        handle.hair_color = hair_color;
        handle.skin = skin;
        handle.equip_slot_a = equip_slot_a;
        handle.equip_slot_b = equip_slot_b;
        handle.equip_slot_c = equip_slot_c;
        handle.equip_slot_d = equip_slot_d;
        handle.boots = boots;
        handle.equip_after_boots_a = equip_after_boots_a;
        handle.equip_after_boots_b = equip_after_boots_b;
        handle.equip_after_armor = equip_after_armor;
        handle.armor = armor;
        handle.equip_before_hat = equip_before_hat;
        handle.hat = hat;
        handle.shield = shield;
        handle.weapon = weapon;
        if (!hidden)
            handle.hidden = false;
        if (hidden == 1)
            handle.hidden = true;
        // A sit_state of 1 is a chair, 2 the floor.
        if (sit_state == 1)
            handle.sitting_on_chair = true;
        if (sit_state == 2)
            handle.sitting_on_floor = true;
        // The arrival type picks whether the player appears with an effect.
        if (!hidden && arrival_type == 2)
            handle.pending_effect = 3;
        else
            handle.pending_effect = -1;
        handle.draw_group = CharacterSlotState_Empty;
        handle.CreateResources();
        players.insert(players.end(), handle);
        SortByPosition();
    }
}

// The player becomes hidden at the end of a short countdown.
void Players::HidePlayer(int player_id)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->hiding = true;
            it->hide_countdown = 9;
            return;
        }
    }
}

// The player is shown again on the next pass.
void Players::UnhidePlayer(int player_id)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->unhiding = true;
            it->unhide_countdown = 1;
            return;
        }
    }
}

// Starts the player's death countdown; RemoveExpiredPlayers drops them when it ends.
void Players::KillPlayer(int player_id)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->dying = true;
            it->death_countdown = 9;
            return;
        }
    }
}

// Removes the player, closing the player menu on them; a dying player stays until their countdown ends.
void Players::RemovePlayer(int player_id)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            if (it->dying && it->death_countdown > 0)
                return;
            if (MainForm->animation->game->player_menu_visible &&
                player_id == MainForm->animation->game->player_menu_target_id)
                MainForm->animation->game->ClosePlayerMenu();
            it->FreeResources();
            it->CloseBalloon();
            players.erase(it);
            MainForm->redraw_needed = true;
            break;
        }
    }
}

// Queues a jump to the tile; a blinking text label closes.
void Players::StartJump(int player_id, int x, int y, int direction)
{
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            if (it->text_label_shown && it->text_label_blinking)
                it->text_label_shown = false;
            it->jump_start = true;
            it->target_x = x;
            it->target_y = y;
            it->target_direction = direction;
            break;
        }
    }
}

// Queues a step to the tile; Animate starts it once the step cells are drawn. A blinking text label closes.
void Players::StartWalk(int player_id, int x, int y, int direction)
{
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            if (it->text_label_shown && it->text_label_blinking)
                it->text_label_shown = false;
            it->walk_start = true;
            it->target_x = x;
            it->target_y = y;
            it->walk_target_direction = direction;
            it->target_direction = direction;
            break;
        }
    }
}

// Turns the player; they also stop sitting.
void Players::SetDirection(int player_id, int direction)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            if (it->text_label_shown && it->text_label_blinking)
                it->text_label_shown = false;
            it->redraw = true;
            it->direction = direction;
            it->sitting_on_chair = false;
            it->sitting_on_floor = false;
            return;
        }
    }
}

int Players::GetDirection(int player_id)
{
    int direction = FacingDirection_Down;
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            direction = it->direction;
            break;
        }
    }
    return direction;
}

// Sets the player's gender, kept to 0 or 1.
void Players::SetGender(int player_id, int gender)
{
    if (gender < 0)
        gender = 0;
    if (gender > 1)
        gender = 1;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->gender = gender;
            return;
        }
    }
}

void Players::SetHairColor(int player_id, int hair_color)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->hair_color = hair_color;
            return;
        }
    }
}

void Players::SetHairStyle(int player_id, int hair_style)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->hair_style = hair_style;
            return;
        }
    }
}

// Sets the player's skin; skins past 11 are 11.
void Players::SetSkin(int player_id, int skin)
{
    if (skin > 11)
        skin = 11;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->skin = skin;
            return;
        }
    }
}

void Players::SetBadgeLevel(int player_id, int badge_level)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->badge_level = badge_level;
            return;
        }
    }
}

void Players::SetDisplayNumber(int player_id, int display_number)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->display_number = display_number;
            return;
        }
    }
}

void Players::SetCharacterId(int player_id, int character_id)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->character_id = character_id;
            return;
        }
    }
}

void Players::SetDrawGroup(int player_id, int draw_group)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->draw_group = draw_group;
            return;
        }
    }
}

void Players::SetName(int player_id, String name)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->name = name;
            break;
        }
    }
}

void Players::SetBoots(int player_id, int boots)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->boots = boots;
            return;
        }
    }
}

void Players::SetArmor(int player_id, int armor)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->armor = armor;
            return;
        }
    }
}

void Players::SetHat(int player_id, int hat)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->hat = hat;
            return;
        }
    }
}

void Players::SetShield(int player_id, int shield)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->shield = shield;
            return;
        }
    }
}

void Players::SetWeapon(int player_id, int weapon)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->weapon = weapon;
            return;
        }
    }
}

int Players::GetPositionExtra(int player_id)
{
    int value = 0;
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            value = it->position_extra;
            break;
        }
    }
    return value;
}

int Players::GetX(int player_id)
{
    int x = 0;
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            x = it->x;
            break;
        }
    }
    return x;
}

int Players::GetY(int player_id)
{
    int y = 0;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            y = it->y;
            break;
        }
    }
    return y;
}

int Players::GetGender(int player_id)
{
    int gender = 0;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            gender = it->gender;
            break;
        }
    }
    return gender;
}

int Players::GetHairColor(int player_id)
{
    int hair_color = 0;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            hair_color = it->hair_color;
            break;
        }
    }
    return hair_color;
}

int Players::GetHairStyle(int player_id)
{
    int hair_style = 0;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            hair_style = it->hair_style;
            break;
        }
    }
    return hair_style;
}

int Players::GetSkin(int player_id)
{
    int skin = 0;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            skin = it->skin;
            break;
        }
    }
    return skin;
}

int Players::GetCharacterId(int player_id)
{
    int character_id = 0;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            character_id = it->character_id;
            break;
        }
    }
    return character_id;
}

int Players::GetDrawGroup(int player_id)
{
    int draw_group = CharacterSlotState_Empty;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            draw_group = it->draw_group;
            break;
        }
    }
    return draw_group;
}

String Players::GetName(int player_id)
{
    String name = "";
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            name = it->name;
            break;
        }
    }
    return name;
}

String Players::GetNameByCharacterId(int character_id)
{
    String name = "";
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (character_id == it->character_id)
        {
            name = it->name;
            break;
        }
    }
    return name;
}

// True when any player needs a redraw; clears their flags.
bool Players::ConsumeRedraw()
{
    bool any_redraw = false;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (it->redraw)
        {
            it->redraw = false;
            any_redraw = true;
        }
    }
    return any_redraw;
}

bool Players::HasPlayer(int player_id)
{
    bool found = false;
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            found = true;
            break;
        }
    }
    return found;
}

int Players::GetBoots(int player_id)
{
    int attacking = false;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            attacking = it->boots;
            break;
        }
    }
    return attacking;
}

int Players::GetArmor(int player_id)
{
    int casting = false;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            casting = it->armor;
            break;
        }
    }
    return casting;
}

int Players::GetShield(int player_id)
{
    int result = 0;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            result = it->shield;
            break;
        }
    }
    return result;
}

int Players::GetWeapon(int player_id)
{
    int result = 0;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            result = it->weapon;
            break;
        }
    }
    return result;
}

// Shows a speech balloon over the player when balloons are on, named after the player unless a speaker is given; a
// style below 0 shows no name.
void Players::ShowBalloonFrom(int player_id, String speaker, String text, int style)
{
    vector<Player>::iterator it;
    if (!MainForm->config->GetShowBalloons())
        return;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (it->player_id == player_id)
        {
            if (speaker.Length() < 1)
                speaker = it->name;
            if (style < 0)
                speaker = "";
            it->ShowBalloon(speaker, text, style);
            return;
        }
    }
}

void Players::ShowBalloon(int player_id, String text, int style)
{
    ShowBalloonFrom(player_id, "", text, style);
}

bool Players::HasBalloon(int player_id)
{
    bool has_balloon = false;
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            if (it->balloon_shown)
                has_balloon = true;
            break;
        }
    }
    return has_balloon;
}

// The id of the first player on the tile, or -1.
int Players::FindPlayerAt(int x, int y)
{
    int player_id = -1;
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (x == (int)it->x && y == (int)it->y)
        {
            player_id = it->player_id;
            break;
        }
    }
    return player_id;
}

// True when the first player found on the tile is not hidden.
bool Players::HasVisiblePlayerAt(int x, int y)
{
    bool found = false;
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if ((x == (int)it->x))
        {
            if ((y == (int)it->y))
            {
                if (!it->hidden)
                    found = true;
                break;
            }
        }
    }
    return found;
}

bool Players::HasPlayerAt(int x, int y)
{
    bool found = false;
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (x == (int)it->x && y == (int)it->y)
        {
            found = true;
            break;
        }
    }
    return found;
}

// Shows a damage or heal number over the player for a few passes.
void Players::ShowDamage(int player_id, int amount, char heal)
{
    vector<Player>::iterator it = players.begin();
    while (it != players.end())
    {
        if (player_id == it->player_id)
        {
            if (amount >= 0)
            {
                String amount_text = IntToStr(amount);
                it->damage_digits = amount_text.Length();
            }
            it->damage_start = true;
            it->damage_shown = false;
            it->damage_timer = 8;
            it->damage_amount = amount;
            it->damage_heal = heal;
            break;
        }
        it++;
    }
}

// Shows the player's health bar at the percent for the duration.
void Players::ShowHealthBar(int player_id, int percent, int duration)
{
    ShowBar(player_id, 0, percent, duration);
}

void Players::ShowManaBar(int player_id, int percent, int duration)
{
    ShowBar(player_id, 1, percent, duration);
}

// Shows one of the player's three bars at the percent for the duration.
void Players::ShowBar(int player_id, int bar, int percent, int duration)
{
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            double width = percent * HEALTH_BAR_SCALE;
            it->health_bar_start = true;
            it->health_bar_shown = false;
            it->health_bar_kind = bar;
            it->health_bar_timer = duration;
            if (!bar)
                it->health_bar_width = (int)width;
            if (bar == 1)
                it->second_bar_width = (int)width;
            if (bar == 2)
                it->third_bar_width = (int)width;
            break;
        }
    }
}

// Starts a spell effect on the player, with its layers, frames and loops from the magic map.
void Players::StartEffect(int player_id, int effect_id)
{
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->effect_start = true;
            it->effect_running = false;
            it->effect_id = effect_id;
            it->effect_frame = 0;
            it->effect_base_layer = MainForm->magic_mapper->base_layer[effect_id];
            it->effect_blend_layer = MainForm->magic_mapper->blend_layer[effect_id];
            it->effect_solid_layer = MainForm->magic_mapper->solid_layer[effect_id];
            it->effect_last_frame = MainForm->magic_mapper->GetFrameCount(effect_id) - 2;
            it->effect_loops_left = MainForm->magic_mapper->GetLoopCount(effect_id);
            break;
        }
    }
}

// True while a spell effect on the player is starting or running.
char Players::IsEffectPlaying(int player_id)
{
    int playing = false;
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            playing = it->effect_running || it->effect_start;
            break;
        }
    }
    return playing;
}

// Starts the level up effect on the player.
void Players::StartLevelUpEffect(int player_id)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
            it->level_up_pending = true;
    }
}

// Starts an emote; emote_id picks its first frame in the face strip.
void Players::StartEmote(int player_id, int emote_id)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->emote_start = true;
            it->emoting = false;
            it->emote_id = emote_id;
            if (emote_id == 14)
                it->emote_id = 11;
            it->emote_first_frame = 1;
            it->emote_frame_count = EMOTE_FRAME_COUNT;
            // Each emote's first frame on the sheet; the emotes sit four frames apart, in their own order.
            if (emote_id == 1)
                it->emote_first_frame = 1;
            if (emote_id == 3)
                it->emote_first_frame = 5;
            if (emote_id == 6)
                it->emote_first_frame = 9;
            if (emote_id == 5)
                it->emote_first_frame = 13;
            if (emote_id == 8)
                it->emote_first_frame = 17;
            if (emote_id == 4)
                it->emote_first_frame = 21;
            if (emote_id == 7)
                it->emote_first_frame = 25;
            if (emote_id == 2)
                it->emote_first_frame = 29;
            if (emote_id == 10)
                it->emote_first_frame = 33;
            if (emote_id == 9)
                it->emote_first_frame = 37;
            if (emote_id == 11)
                it->emote_first_frame = 41;
            if (emote_id == 12)
                it->emote_first_frame = 45;
            if (emote_id == 13)
                it->emote_first_frame = 49;
            if (emote_id == 14)
                it->emote_first_frame = 53;
            if (emote_id == 15)
                it->emote_first_frame = 57;
            it->emote_frame_count = EMOTE_FRAME_COUNT;
            return;
        }
    }
}

char Players::IsEmoting(int player_id)
{
    int emoting = false;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            emoting = it->emoting || it->emote_start;
            break;
        }
    }
    return emoting;
}

char Players::IsWalking(int player_id)
{
    char walking = false;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            walking = (unsigned char)it->walking;
            break;
        }
    }
    return walking;
}

// Queues an attack in the direction; silent leaves out the weapon sound.
void Players::StartAttack(int player_id, int direction, char silent)
{
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->attack_start = true;
            it->silent_attack = silent;
            it->direction = direction;
            break;
        }
    }
}

// True while the player's attack is starting or running.
char Players::IsAttacking(int player_id)
{
    int attacking = false;
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            attacking = it->attacking || it->attack_start;
            break;
        }
    }
    return attacking;
}

// Queues a spell cast; a direction of 0 or less keeps the one the player faces.
void Players::StartSpellCast(int player_id, int direction)
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->cast_start = true;
            if (direction > FacingDirection_Down)
                it->direction = direction;
            return;
        }
    }
}

// True while the player's spell cast is starting or running.
char Players::IsCasting(int player_id)
{
    int casting = false;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            casting = it->casting || it->cast_start;
            break;
        }
    }
    return casting;
}

// Queues standing up on the tile; for the own character the view centres on them after.
void Players::StartStandUp(int player_id, int x, int y)
{
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            it->stand_up_start = true;
            it->target_x = x;
            it->target_y = y;
            if (player_id == MainForm->net_client->GetPlayerId())
                it->recenter_view = true;
            break;
        }
    }
}

// Queues sitting on the floor at the tile, facing direction.
void Players::StartSitOnFloor(int player_id, int x, int y, int direction)
{
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            if (it->text_label_shown && it->text_label_blinking)
                it->text_label_shown = false;
            it->floor_sit_start = true;
            it->floor_direction = direction;
            it->target_x = x;
            it->target_y = y;
            it->target_direction = direction;
            break;
        }
    }
}

// Queues sitting on a chair at the tile; for the own character the view centres on them after.
void Players::StartSitOnChair(int player_id, int x, int y, int direction)
{
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            if (it->text_label_shown && it->text_label_blinking)
                it->text_label_shown = false;
            it->chair_sit_start = true;
            it->chair_direction = direction;
            it->target_x = x;
            it->target_y = y;
            it->target_direction = direction;
            if (player_id == MainForm->net_client->GetPlayerId())
                it->recenter_view = true;
            break;
        }
    }
}

// True while the player sits on the floor or is sitting down there.
char Players::IsSittingOnFloor(int player_id)
{
    int sitting = false;
    vector<Player>::iterator it;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            sitting = it->sitting_on_floor || it->floor_sit_start;
            break;
        }
    }
    return sitting;
}

// True while the player sits on a chair or is sitting down on one.
char Players::IsSittingOnChair(int player_id)
{
    int sitting = false;
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            sitting = it->sitting_on_chair || it->chair_sit_start;
            break;
        }
    }
    return sitting;
}

// A volume for sounds of the player, 200 lower for each step between them and the own character.
int Players::GetDistanceVolume(int player_id)
{
    vector<Player>::iterator it;
    int volume = 0;
    for (it = players.begin(); it != players.end(); it++)
    {
        if (player_id == it->player_id)
        {
            int own_x = MainForm->players->GetX(MainForm->net_client->GetPlayerId());
            int own_y = MainForm->players->GetY(MainForm->net_client->GetPlayerId());
            volume -= abs((int)it->x - own_x);
            volume -= abs((int)it->y - own_y);
            volume = 200 * volume;
            break;
        }
    }
    return volume;
}

// A copy of the player with the id.
Player Players::GetPlayerById(int player_id)
{
    vector<Player>::iterator i;
    for (i = players.begin(); i != players.end(); i++)
    {
        if (i->player_id == player_id)
            break;
    }
    return *i;
}

Player Players::GetPlayer(int index)
{
    return players[index];
}

// Frees every player's cells and balloon, then empties the list.
void Players::Clear()
{
    for (vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        it->FreeResources();
        it->CloseBalloon();
    }
    if (GetCount() >= 1)
    {
        players.clear();
        current_index = -1;
    }
}

unsigned int Players::GetCount()
{
    return players.size();
}

void Players::SetMousePosition(int cursor_x, int cursor_y)
{
    mouse_x = cursor_x;
    mouse_y = cursor_y;
}
