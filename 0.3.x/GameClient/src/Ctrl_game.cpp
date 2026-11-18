#include <vcl.h>
#pragma hdrstop

#include "Ctrl_game.h"
#include "Tmainform.h"
#include "Gfx_game.h"
#include "Ctrl_interface.h"
#include "Packetoperations.h"
#include "Ctrl_paperdoll.h"
#include "Ctrl_status.h"
#include "Ctrl_inventory.h"
#include "Ctrl_spells.h"

#pragma package(smart_init)

// Starts the game screen with every box closed and placed at its default spot.
GameController::GameController()
{
    last_action_time = DateTimeToTimeStamp(Now());
    unused_flag = true;
    ghost_walk = false;
    spot_countdown = 0;
    message_visible = false;
    message_x = 190;
    message_y = 100;
    message_lines[0] = "";
    message_lines[1] = "";
    form_top_row = 0;
    form_thumb_offset = 0;
    form_scroll_up_held = false;
    form_scroll_down_held = false;
    form_thumb_grabbed = false;
    large_box_visible = false;
    large_box_x = 190;
    large_box_y = 68;
    large_box_lines[0] = "";
    large_box_lines[1] = "";
    large_box_lines[2] = "";
    large_box_lines[3] = "";
    large_box_lines[4] = "";
    large_box_lines[5] = "";
    large_box_lines[6] = "";
    input_box_visible = false;
    input_box_x = 175;
    input_box_y = 95;
    input_box_lines[0] = "";
    input_box_lines[1] = "";
    input_box_ok_hovered = false;
    input_box_cancel_hovered = false;
    player_menu_visible = false;
    player_menu_x = 100;
    player_menu_y = 100;
    message_ok_hovered = false;
    message_cancel_hovered = false;
    large_box_ok_hovered = false;
    large_box_cancel_hovered = false;
    amount_slider_hovered = false;
    amount_box_visible = false;
    amount_box_x = 190;
    amount_box_y = 70;
    amount_item_id = -1;
    amount_ok_hovered = false;
    amount_cancel_hovered = false;
    restricted_map_id = -1;
    form_visible = false;
    form_x = 155;
    form_y = 32;
    form_ok_hovered = false;
    form_cancel_hovered = false;
    blocking_player_id = -1;
    idle_ticks = 0;
    attack_count = 0;
}

GameController::~GameController()
{
}

// Centres the map view on the player's tile.
void GameController::CenterViewOnPlayer(int player_id)
{
    MainForm->map->SetCenterX(MainForm->players->GetX(player_id));
    MainForm->map->SetCenterY(MainForm->players->GetY(player_id));
}

// Sends a stand-up request when the own character sits on the floor and no other action runs; true when sent.
bool GameController::StandUp()
{
    ResetIdleTimer();
    bool stood_up = false;
    if (!MainForm->animation->IsSitDelayed() && !MainForm->map->IsActionBlocked() && !MainForm->map->IsScrolling() &&
        !MainForm->players->IsWalking(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsCasting(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsAttacking(MainForm->net_client->GetPlayerId()) &&
        MainForm->players->IsSittingOnFloor(MainForm->net_client->GetPlayerId()))
    {
        if (MainForm->animation->game->player_menu_visible)
            MainForm->animation->game->ClosePlayerMenu();
        MainForm->animation->StartSitDelay(SIT_DELAY_TICKS);
        MainForm->net_client->sender->SendFloorStand();
        stood_up = true;
    }
    return stood_up;
}

// Sits down on the floor at the tile, or stands up when already sitting there; true only when a stand request went out.
bool GameController::ToggleSit(int x, int y)
{
    ResetIdleTimer();
    bool stood_up = false;
    if (!MainForm->animation->IsSitDelayed() && !MainForm->map->IsActionBlocked() && !MainForm->map->IsScrolling() &&
        !MainForm->players->IsWalking(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsCasting(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsAttacking(MainForm->net_client->GetPlayerId()))
    {
        // Standing, the player sits on the floor of the tile; sitting, they stand up.
        if (!MainForm->players->IsSittingOnFloor(MainForm->net_client->GetPlayerId()))
        {
            if (MainForm->animation->game->player_menu_visible)
                MainForm->animation->game->ClosePlayerMenu();
            MainForm->animation->StartSitDelay(SIT_DELAY_TICKS);
            MainForm->animation->interface_controller->spells->CancelCast();
            MainForm->players->HideTextLabel(MainForm->net_client->GetPlayerId());
            int player_x = MainForm->players->GetX(MainForm->net_client->GetPlayerId());
            MainForm->net_client->sender->SendFloorSit(player_x,
                MainForm->players->GetY(MainForm->net_client->GetPlayerId()));
        }
        else
        {
            if (MainForm->animation->game->player_menu_visible)
                MainForm->animation->game->ClosePlayerMenu();
            MainForm->animation->StartSitDelay(SIT_DELAY_TICKS);
            MainForm->net_client->sender->SendFloorStand();
            stood_up = true;
        }
        MainForm->map->BlockActionsBriefly();
    }
    return stood_up;
}

// Sits on the chair at the tile, or stands up from a chair; true when a request went out.
bool GameController::SitOnChair(int x, int y)
{
    ResetIdleTimer();
    bool acted = false;
    if (!MainForm->animation->IsSitDelayed() && !MainForm->map->IsActionBlocked() && !MainForm->map->IsScrolling() &&
        !MainForm->players->IsWalking(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsCasting(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsAttacking(MainForm->net_client->GetPlayerId()))
    {
        if (!MainForm->players->IsSittingOnChair(MainForm->net_client->GetPlayerId()))
        {
            if (MainForm->animation->game->player_menu_visible)
                MainForm->animation->game->ClosePlayerMenu();
            int tile_spec = MainForm->map->GetUsableTileSpec(x, y);
            // The chair specs; the tile must be free and the player must not sit on the floor
            if (tile_spec >= TileSpec_ChairYHigh && tile_spec <= TileSpec_ChairAnySide &&
                !MainForm->players->IsSittingOnFloor(MainForm->net_client->GetPlayerId()) &&
                !MainForm->players->HasPlayerAt(x, y))
            {
                MainForm->animation->StartSitDelay(SIT_DELAY_TICKS);
                MainForm->animation->interface_controller->spells->CancelCast();
                MainForm->players->HideTextLabel(MainForm->net_client->GetPlayerId());
                MainForm->net_client->sender->SendChairSit(x, y);
                acted = true;
                MainForm->map->BlockActionsBriefly();
            }
        }
        else
        {
            if (MainForm->animation->game->player_menu_visible)
                MainForm->animation->game->ClosePlayerMenu();
            MainForm->animation->StartSitDelay(8);
            MainForm->net_client->sender->SendChairStand();
            acted = true;
            MainForm->map->BlockActionsBriefly();
        }
    }
    return acted;
}

// Starts the cast animation in the facing direction and sends the cast without a target.
void GameController::CastUntargetedSpell(int spell_id)
{
    ResetIdleTimer();
    int direction = MainForm->players->GetDirection(MainForm->net_client->GetPlayerId());
    MainForm->players->StartSpellCast(MainForm->net_client->GetPlayerId(), direction);
    MainForm->net_client->sender->SendSpellCastDirectional(direction, spell_id);
}

void GameController::AttackForward(char animation_only)
{
    Attack(animation_only, -1);
}

// One attack in the facing direction, which costs one stamina; with animation_only the attack is shown and not sent.
// It is refused while another action runs, while sitting, out of stamina, or with a ranged weapon and no ammunition.
void GameController::Attack(char animation_only, int face_direction)
{
    ResetIdleTimer();
    if (!MainForm->players->HasPlayer(MainForm->net_client->GetPlayerId()))
        return;
    if (!MainForm->map->IsActionBlocked() && !MainForm->animation->IsSitDelayed() && !MainForm->map->IsScrolling() &&
        !MainForm->players->IsWalking(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsCasting(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsAttacking(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsSittingOnChair(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsSittingOnFloor(MainForm->net_client->GetPlayerId()))
    {
        if (MainForm->item_values->GetSubType(MainForm->animation->paperdoll->equipped_weapon) == 1 &&
            MainForm->item_values->GetSubType(MainForm->animation->paperdoll->equipped_shield) != 2)
        {
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                MainForm->strings->GetText(GameText_YouHaveNoArrows));
            MainForm->sfx->PlaySfx(SfxSound_NoArrows);
            return;
        }
        if (IsActionDelayElapsed())
        {
        }
        // Attacks in a row without a step are capped; each step resets the count
        if (attack_count > 10001)
            return;
        int direction = MainForm->players->GetDirection(MainForm->net_client->GetPlayerId());
        int player_x = MainForm->players->GetX(MainForm->net_client->GetPlayerId());
        int player_y = MainForm->players->GetY(MainForm->net_client->GetPlayerId());
        if (face_direction >= 0)
        {
            if (direction != face_direction)
                MainForm->net_client->sender->SendFace(face_direction);
            direction = face_direction;
        }
        if (MainForm->map->GetTileSpec(player_x, player_y) == TileSpec_Splash)
        {
            MainForm->players->StartEffect(MainForm->net_client->GetPlayerId(), SpellEffect_WaterSplash);
            MainForm->sfx->PlaySfx(SfxSound_WaterSplash);
        }
        attack_count++;
        MainForm->animation->interface_controller->spells->CancelCast();
        MainForm->players->HideTextLabel(MainForm->net_client->GetPlayerId());
        MainForm->animation->interface_controller->status->SetStamina(MainForm->animation->interface_controller->status->stamina -
            1, false);
        MainForm->players->StartAttack(MainForm->net_client->GetPlayerId(), direction, animation_only);
        if (!animation_only)
            MainForm->net_client->sender->SendAttack(direction);
    }
}

// Plays the emote on the own character and sends it, unless another action runs or a balloon shows.
void GameController::PlayEmote(int emote_id)
{
    if (!MainForm->animation->IsEmoteDelayed() && !MainForm->players->IsEmoting(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsWalking(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsCasting(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsAttacking(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->HasBalloon(MainForm->net_client->GetPlayerId()))
    {
        MainForm->players->StartEmote(MainForm->net_client->GetPlayerId(), emote_id);
        MainForm->net_client->sender->SendEmote(emote_id);
        MainForm->animation->StartEmoteDelay(4);
    }
}

// One step of the own character in the direction, or a jump on a jump tile; a player facing elsewhere turns first.
// With ghost walking on, players and npcs do not block, and a warp tile asks for its level.
// Against a wall the tile is used instead: a door, chest, locker, board, jukebox or chair, tried in that order.
void GameController::Walk(int direction)
{
    int target_x;
    int target_y;
    bool tile_action_used;
    bool opened_up;
    bool opened_right;
    bool opened_down;
    char on_jump_tile;
    int warp_level_left;
    char walk_left;
    char tile_used_left;
    int usable_spec_left;
    int warp_level_up;
    char walk_up;
    char tile_used_up;
    int usable_spec_up;
    int warp_level_right;
    char walk_right;
    char tile_used_right;
    int usable_spec_right;
    int warp_level_down;
    char walk_down;
    char tile_used_down;
    int usable_spec_down;
    ResetIdleTimer();
    if (!MainForm->players->HasPlayer(MainForm->net_client->GetPlayerId()))
        return;
    // A step only while no action is blocked and the player is not casting, attacking or sitting.
    if (!MainForm->map->IsActionBlocked() && !MainForm->animation->IsWalkingBlocked() &&
        !MainForm->players->IsCasting(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsAttacking(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsSittingOnChair(MainForm->net_client->GetPlayerId()) &&
        !MainForm->players->IsSittingOnFloor(MainForm->net_client->GetPlayerId()))
    {
        if (MainForm->animation->game->player_menu_visible)
            MainForm->animation->game->ClosePlayerMenu();
        target_x = MainForm->players->GetX(MainForm->net_client->GetPlayerId());
        target_y = MainForm->players->GetY(MainForm->net_client->GetPlayerId());
        if (MainForm->players->GetDirection(MainForm->net_client->GetPlayerId()) != direction &&
            !MainForm->map->IsCursorUpdatePending())
        {
            if (MainForm->players->HasPlayer(MainForm->net_client->GetPlayerId()))
            {
                MainForm->animation->interface_controller->spells->CancelCast();
                MainForm->players->HideTextLabel(MainForm->net_client->GetPlayerId());
                MainForm->players->SetDirection(MainForm->net_client->GetPlayerId(), direction);
                MainForm->net_client->sender->SendFace(direction);
                MainForm->map->BlockActionsBriefly();
            }
        }
        else if (MainForm->players->HasPlayer(MainForm->net_client->GetPlayerId()))
        {
            on_jump_tile = false;
            if (MainForm->map->GetTileSpec(target_x, target_y) == TileSpec_Jump)
                on_jump_tile = true;
            // Directions: 0 = down, 1 = left, 2 = up, 3 = right
            if (direction == FacingDirection_Left && MainForm->map->GetCenterX() > 0 && !MainForm->map->IsScrolling())
            {
                target_x--;
                // While the spot countdown runs, a step back onto the spot is refused
                if (spot_countdown > 0 && target_x == spot_x && target_y == spot_y)
                    return;
                if (!MainForm->map->IsBlocked(target_x, target_y) || ghost_walk)
                {
                    warp_level_left = MainForm->map->GetWarpLevel(target_x, target_y);
                    if (MainForm->animation->interface_controller->status->level >= warp_level_left)
                    {
                        if (!MainForm->players->HasPlayerAt(target_x, target_y) || ghost_walk)
                        {
                            if (!MainForm->npcs->HasNpcAt(target_x, target_y) || ghost_walk ||
                                MainForm->config->GetAlwaysGhost())
                            {
                                walk_left = true;
                                if (MainForm->map->GetTileSpec(target_x, target_y) == TileSpec_Jump || on_jump_tile)
                                {
                                    walk_left = false;
                                    MainForm->players->StartJump(MainForm->net_client->GetPlayerId(), target_x,
                                        target_y, direction);
                                }
                                if (MainForm->map->GetTileSpec(target_x, target_y) == TileSpec_Splash)
                                {
                                    MainForm->players->StartEffect(MainForm->net_client->GetPlayerId(),
                                        SpellEffect_WaterSplash);
                                    MainForm->sfx->PlaySfx(SfxSound_WaterSplash);
                                }
                                if (walk_left)
                                    MainForm->players->StartWalk(MainForm->net_client->GetPlayerId(), target_x,
                                        target_y, direction);
                                MainForm->animation->interface_controller->spells->CancelCast();
                                MainForm->players->HideTextLabel(MainForm->net_client->GetPlayerId());
                                MainForm->map->StartScroll(FacingDirection_Left);
                                attack_count = 0;
                                if (ghost_walk)
                                {
                                    MainForm->sfx->PlaySfx(SfxSound_GhostWalk);
                                    MainForm->net_client->sender->SendWalkNoWall(direction, target_x, target_y);
                                }
                                else
                                    MainForm->net_client->sender->SendWalk(direction, target_x, target_y);
                            }
                        }
                        else
                        {
                            // A player in the way is noted; Tick walks through them when they keep blocking
                            blocked_direction = direction;
                            blocked_x = target_x;
                            blocked_y = target_y;
                            if (MainForm->map->GetTileSpec(target_x, target_y) != TileSpec_CountdownSpot)
                                blocking_player_id = MainForm->players->FindPlayerAt(target_x, target_y);
                        }
                    }
                    else
                    {
                        MainForm->map->BlockActionsBriefly();
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                            MainForm->strings->GetText(GameText_YouAreNotReady) + " - LVL " +
                                IntToStr(warp_level_left));
                    }
                }
                else
                {
                    tile_used_left = false;
                    usable_spec_left = MainForm->map->GetUsableTileSpec(target_x, target_y);
                    if (usable_spec_left >= 0 && !MainForm->animation->IsTileUseDelayed())
                    {
                        MainForm->animation->StartTileUseDelay();
                        tile_action_used = TryOpenDoor(target_x, target_y, usable_spec_left);
                        if (!tile_action_used)
                        {
                            tile_action_used = TryOpenChest(target_x, target_y, usable_spec_left);
                            tile_used_left = true;
                        }
                        if (!tile_action_used)
                        {
                            tile_action_used = TryOpenLocker(target_x, target_y, usable_spec_left);
                            tile_used_left = true;
                        }
                        if (!tile_action_used)
                        {
                            tile_action_used = TryOpenBoard(target_x, target_y, usable_spec_left);
                            tile_used_left = true;
                        }
                        if (!tile_action_used)
                        {
                            tile_action_used = TryOpenJukebox(target_x, target_y, usable_spec_left);
                            tile_used_left = true;
                        }
                        if (!tile_action_used)
                        {
                            TrySitOnChair(target_x, target_y, usable_spec_left);
                            tile_used_left = true;
                        }
                        if (tile_used_left)
                            MainForm->animation->BlockWalking();
                    }
                }
            }
            if (direction == FacingDirection_Up && MainForm->map->GetCenterY() > 0 && !MainForm->map->IsScrolling())
            {
                target_y--;
                if (spot_countdown > 0 && target_x == spot_x && target_y == spot_y)
                    return;
                if (!MainForm->map->IsBlocked(target_x, target_y) || ghost_walk)
                {
                    warp_level_up = MainForm->map->GetWarpLevel(target_x, target_y);
                    if (MainForm->animation->interface_controller->status->level >= warp_level_up)
                    {
                        if (!MainForm->players->HasPlayerAt(target_x, target_y) || ghost_walk)
                        {
                            if (!MainForm->npcs->HasNpcAt(target_x, target_y) || ghost_walk ||
                                MainForm->config->GetAlwaysGhost())
                            {
                                walk_up = true;
                                if (MainForm->map->GetTileSpec(target_x, target_y) == TileSpec_Jump || on_jump_tile)
                                {
                                    walk_up = false;
                                    MainForm->players->StartJump(MainForm->net_client->GetPlayerId(), target_x,
                                        target_y, direction);
                                }
                                if (MainForm->map->GetTileSpec(target_x, target_y) == TileSpec_Splash)
                                {
                                    MainForm->players->StartEffect(MainForm->net_client->GetPlayerId(),
                                        SpellEffect_WaterSplash);
                                    MainForm->sfx->PlaySfx(SfxSound_WaterSplash);
                                }
                                if (walk_up)
                                    MainForm->players->StartWalk(MainForm->net_client->GetPlayerId(), target_x,
                                        target_y, direction);
                                MainForm->animation->interface_controller->spells->CancelCast();
                                MainForm->players->HideTextLabel(MainForm->net_client->GetPlayerId());
                                MainForm->map->StartScroll(FacingDirection_Up);
                                attack_count = 0;
                                if (ghost_walk)
                                {
                                    MainForm->sfx->PlaySfx(SfxSound_GhostWalk);
                                    MainForm->net_client->sender->SendWalkNoWall(direction, target_x, target_y);
                                }
                                else
                                    MainForm->net_client->sender->SendWalk(direction, target_x, target_y);
                            }
                        }
                        else
                        {
                            blocked_direction = direction;
                            blocked_x = target_x;
                            blocked_y = target_y;
                            if (MainForm->map->GetTileSpec(target_x, target_y) != TileSpec_CountdownSpot)
                                blocking_player_id = MainForm->players->FindPlayerAt(target_x, target_y);
                        }
                    }
                    else
                    {
                        MainForm->map->BlockActionsBriefly();
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                            MainForm->strings->GetText(GameText_YouAreNotReady) + " - LVL " + IntToStr(warp_level_up));
                    }
                }
                else
                {
                    tile_used_up = false;
                    usable_spec_up = MainForm->map->GetUsableTileSpec(target_x, target_y);
                    if (usable_spec_up >= 0 && !MainForm->animation->IsTileUseDelayed())
                    {
                        MainForm->animation->StartTileUseDelay();
                        opened_up = TryOpenDoor(target_x, target_y, usable_spec_up);
                        if (!opened_up)
                        {
                            opened_up = TryOpenChest(target_x, target_y, usable_spec_up);
                            tile_used_up = true;
                        }
                        if (!opened_up)
                        {
                            opened_up = TryOpenLocker(target_x, target_y, usable_spec_up);
                            tile_used_up = true;
                        }
                        if (!opened_up)
                        {
                            opened_up = TryOpenBoard(target_x, target_y, usable_spec_up);
                            tile_used_up = true;
                        }
                        if (!opened_up)
                        {
                            opened_up = TryOpenJukebox(target_x, target_y, usable_spec_up);
                            tile_used_up = true;
                        }
                        if (!opened_up)
                        {
                            TrySitOnChair(target_x, target_y, usable_spec_up);
                            tile_used_up = true;
                        }
                        if (tile_used_up)
                            MainForm->animation->BlockWalking();
                    }
                }
            }
            if (direction == FacingDirection_Right && MainForm->map->GetCenterX() < MainForm->map->GetWidth() &&
                !MainForm->map->IsScrolling())
            {
                target_x++;
                if (spot_countdown > 0 && target_x == spot_x && target_y == spot_y)
                    return;
                if (!MainForm->map->IsBlocked(target_x, target_y) || ghost_walk)
                {
                    warp_level_right = MainForm->map->GetWarpLevel(target_x, target_y);
                    if (MainForm->animation->interface_controller->status->level >= warp_level_right)
                    {
                        if (!MainForm->players->HasPlayerAt(target_x, target_y) || ghost_walk)
                        {
                            if (!MainForm->npcs->HasNpcAt(target_x, target_y) || ghost_walk ||
                                MainForm->config->GetAlwaysGhost())
                            {
                                walk_right = true;
                                if (MainForm->map->GetTileSpec(target_x, target_y) == TileSpec_Jump || on_jump_tile)
                                {
                                    walk_right = false;
                                    MainForm->players->StartJump(MainForm->net_client->GetPlayerId(), target_x,
                                        target_y, direction);
                                }
                                if (MainForm->map->GetTileSpec(target_x, target_y) == TileSpec_Splash)
                                {
                                    MainForm->players->StartEffect(MainForm->net_client->GetPlayerId(),
                                        SpellEffect_WaterSplash);
                                    MainForm->sfx->PlaySfx(SfxSound_WaterSplash);
                                }
                                if (walk_right)
                                    MainForm->players->StartWalk(MainForm->net_client->GetPlayerId(), target_x,
                                        target_y, direction);
                                MainForm->animation->interface_controller->spells->CancelCast();
                                MainForm->players->HideTextLabel(MainForm->net_client->GetPlayerId());
                                MainForm->map->StartScroll(FacingDirection_Right);
                                attack_count = 0;
                                if (ghost_walk)
                                {
                                    MainForm->sfx->PlaySfx(SfxSound_GhostWalk);
                                    MainForm->net_client->sender->SendWalkNoWall(direction, target_x, target_y);
                                }
                                else
                                    MainForm->net_client->sender->SendWalk(direction, target_x, target_y);
                            }
                        }
                        else
                        {
                            blocked_direction = direction;
                            blocked_x = target_x;
                            blocked_y = target_y;
                            if (MainForm->map->GetTileSpec(target_x, target_y) != TileSpec_CountdownSpot)
                                blocking_player_id = MainForm->players->FindPlayerAt(target_x, target_y);
                        }
                    }
                    else
                    {
                        MainForm->map->BlockActionsBriefly();
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                            MainForm->strings->GetText(GameText_YouAreNotReady) + " - LVL " +
                                IntToStr(warp_level_right));
                    }
                }
                else
                {
                    tile_used_right = false;
                    usable_spec_right = MainForm->map->GetUsableTileSpec(target_x, target_y);
                    if (usable_spec_right >= 0 && !MainForm->animation->IsTileUseDelayed())
                    {
                        MainForm->animation->StartTileUseDelay();
                        opened_right = TryOpenDoor(target_x, target_y, usable_spec_right);
                        if (!opened_right)
                        {
                            opened_right = TryOpenChest(target_x, target_y, usable_spec_right);
                            tile_used_right = true;
                        }
                        if (!opened_right)
                        {
                            opened_right = TryOpenLocker(target_x, target_y, usable_spec_right);
                            tile_used_right = true;
                        }
                        if (!opened_right)
                        {
                            opened_right = TryOpenBoard(target_x, target_y, usable_spec_right);
                            tile_used_right = true;
                        }
                        if (!opened_right)
                        {
                            opened_right = TryOpenJukebox(target_x, target_y, usable_spec_right);
                            tile_used_right = true;
                        }
                        if (!opened_right)
                        {
                            TrySitOnChair(target_x, target_y, usable_spec_right);
                            tile_used_right = true;
                        }
                        if (tile_used_right)
                            MainForm->animation->BlockWalking();
                    }
                }
            }
            if (direction == FacingDirection_Down && MainForm->map->GetCenterY() < MainForm->map->GetHeight() &&
                !MainForm->map->IsScrolling())
            {
                target_y++;
                if (spot_countdown > 0 && target_x == spot_x && target_y == spot_y)
                    return;
                if (!MainForm->map->IsBlocked(target_x, target_y) || ghost_walk)
                {
                    warp_level_down = MainForm->map->GetWarpLevel(target_x, target_y);
                    if (MainForm->animation->interface_controller->status->level >= warp_level_down)
                    {
                        if (!MainForm->players->HasPlayerAt(target_x, target_y) || ghost_walk)
                        {
                            if (!MainForm->npcs->HasNpcAt(target_x, target_y) || ghost_walk ||
                                MainForm->config->GetAlwaysGhost())
                            {
                                walk_down = true;
                                if (MainForm->map->GetTileSpec(target_x, target_y) == TileSpec_Jump || on_jump_tile)
                                {
                                    walk_down = false;
                                    MainForm->players->StartJump(MainForm->net_client->GetPlayerId(), target_x,
                                        target_y, direction);
                                }
                                if (MainForm->map->GetTileSpec(target_x, target_y) == TileSpec_Splash)
                                {
                                    MainForm->players->StartEffect(MainForm->net_client->GetPlayerId(),
                                        SpellEffect_WaterSplash);
                                    MainForm->sfx->PlaySfx(SfxSound_WaterSplash);
                                }
                                if (walk_down)
                                    MainForm->players->StartWalk(MainForm->net_client->GetPlayerId(), target_x,
                                        target_y, direction);
                                MainForm->animation->interface_controller->spells->CancelCast();
                                MainForm->players->HideTextLabel(MainForm->net_client->GetPlayerId());
                                MainForm->map->StartScroll(FacingDirection_Down);
                                attack_count = 0;
                                if (ghost_walk)
                                {
                                    MainForm->sfx->PlaySfx(SfxSound_GhostWalk);
                                    MainForm->net_client->sender->SendWalkNoWall(direction, target_x, target_y);
                                }
                                else
                                    MainForm->net_client->sender->SendWalk(direction, target_x, target_y);
                            }
                        }
                        else
                        {
                            blocked_direction = direction;
                            blocked_x = target_x;
                            blocked_y = target_y;
                            if (MainForm->map->GetTileSpec(target_x, target_y) != TileSpec_CountdownSpot)
                                blocking_player_id = MainForm->players->FindPlayerAt(target_x, target_y);
                        }
                    }
                    else
                    {
                        MainForm->map->BlockActionsBriefly();
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                            MainForm->strings->GetText(GameText_YouAreNotReady) + " - LVL " +
                                IntToStr(warp_level_down));
                    }
                }
                else
                {
                    tile_used_down = false;
                    usable_spec_down = MainForm->map->GetUsableTileSpec(target_x, target_y);
                    if (usable_spec_down >= 0 && !MainForm->animation->IsTileUseDelayed())
                    {
                        MainForm->animation->StartTileUseDelay();
                        opened_down = TryOpenDoor(target_x, target_y, usable_spec_down);
                        if (!opened_down)
                        {
                            opened_down = TryOpenChest(target_x, target_y, usable_spec_down);
                            tile_used_down = true;
                        }
                        if (!opened_down)
                        {
                            opened_down = TryOpenLocker(target_x, target_y, usable_spec_down);
                            tile_used_down = true;
                        }
                        if (!opened_down)
                        {
                            opened_down = TryOpenBoard(target_x, target_y, usable_spec_down);
                            tile_used_down = true;
                        }
                        if (!opened_down)
                        {
                            opened_down = TryOpenJukebox(target_x, target_y, usable_spec_down);
                            tile_used_down = true;
                        }
                        if (!opened_down)
                        {
                            TrySitOnChair(target_x, target_y, usable_spec_down);
                            tile_used_down = true;
                        }
                        if (tile_used_down)
                            MainForm->animation->BlockWalking();
                    }
                }
            }
        }
    }
}

// Opens the player menu for target_id beside the click, moved so it stays inside the view.
void GameController::OpenPlayerMenu(int x, int y, int target_id, String player_name)
{
    player_menu_name = player_name;
    player_menu_target_id = target_id;
    player_menu_hovered_option = -1;
    player_menu_visible = true;
    player_menu_x = x + 20;
    player_menu_y = y - 20;
    if (MainForm->gfx->game->view_clip_rect.left + 4 > player_menu_x)
        player_menu_x = MainForm->gfx->game->view_clip_rect.left + 4;
    if (MainForm->gfx->game->view_clip_rect.top + 4 > player_menu_y)
        player_menu_y = MainForm->gfx->game->view_clip_rect.top + 4;
    if (player_menu_x + 100 > MainForm->gfx->game->view_clip_rect.right)
        player_menu_x = player_menu_x - 134;
    if (MainForm->gfx->game->view_clip_rect.bottom < player_menu_y + 148)
        player_menu_y = MainForm->gfx->game->view_clip_rect.bottom - 148;
    MainForm->redraw_needed = true;
}

// Closes the player menu.
void GameController::ClosePlayerMenu()
{
    player_menu_target_id = PlayerId_None;
    player_menu_hovered_option = -1;
    player_menu_visible = false;
    MainForm->redraw_needed = true;
}

// Closes the input box and removes its edit.
void GameController::CloseInputBox()
{
    input_box_visible = false;
    MainForm->edits->RemoveEdit(EditId_InputBox);
    MainForm->edits->FocusEdit(EditId_Chat);
    MainForm->edits->caret_enabled = true;
    MainForm->redraw_needed = true;
}

// Closes the form and removes its five edits.
void GameController::CloseInputForm()
{
    form_visible = false;
    MainForm->edits->RemoveEdit(EditId_FormFifthRow);
    MainForm->edits->RemoveEdit(EditId_FormFourthRow);
    MainForm->edits->RemoveEdit(EditId_FormThirdRow);
    MainForm->edits->RemoveEdit(EditId_FormSecondRow);
    MainForm->edits->RemoveEdit(EditId_FormFirstRow);
    MainForm->edits->FocusEdit(EditId_Chat);
    MainForm->edits->caret_enabled = true;
    MainForm->redraw_needed = true;
}

// Shows the large message box in place of the small one; kinds above 1 start with their lines empty.
void GameController::ShowLargeMessageBox(int buttons, int kind, String title, String text)
{
    large_box_visible = true;
    message_visible = false;
    large_box_dragging = false;
    large_box_text = text;
    if (kind > LargeBoxKind_CraftItem)
    {
        large_box_lines[0] = "";
        large_box_lines[1] = "";
        large_box_lines[2] = "";
        large_box_lines[3] = "";
        large_box_lines[4] = "";
        large_box_lines[5] = "";
    }
    large_box_buttons = buttons;
    large_box_kind = kind;
    large_box_title = title;
    MainForm->edits->SetVisible(EditId_Amount, false);
    MainForm->edits->SetVisible(EditId_InputBox, false);
    amount_box_dragging = false;
    input_box_dragging = false;
    amount_ok_hovered = false;
    amount_cancel_hovered = false;
    amount_slider_grabbed = false;
    MainForm->redraw_needed = true;
}

// Shows the small message box with one line of text, hiding the edits of the other boxes.
void GameController::ShowMessageBox(int buttons, int kind, String title, String text)
{
    large_box_visible = false;
    message_visible = true;
    message_dragging = false;
    message_lines[0] = text;
    message_lines[1] = "";
    message_buttons = buttons;
    message_kind = kind;
    message_title = title;
    MainForm->edits->SetVisible(EditId_Amount, false);
    MainForm->edits->SetVisible(EditId_InputBox, false);
    MainForm->edits->SetVisible(EditId_FormFirstRow, false);
    MainForm->edits->SetVisible(EditId_FormSecondRow, false);
    MainForm->edits->SetVisible(EditId_FormThirdRow, false);
    MainForm->edits->SetVisible(EditId_FormFourthRow, false);
    MainForm->edits->SetVisible(EditId_FormFifthRow, false);
    MainForm->edits->SetEditable(POST_SUBJECT_EDIT, false);
    MainForm->edits->SetEditable(POST_MESSAGE_EDIT, false);
    amount_box_dragging = false;
    input_box_dragging = false;
    amount_ok_hovered = false;
    amount_cancel_hovered = false;
    amount_slider_grabbed = false;
    MainForm->redraw_needed = true;
}

// Opens the form: up to 25 rows of label and edit, five on screen, scrolled when there are more.
void GameController::OpenInputForm(int kind, int row_count, int char_filter, int max_length, String title,
    String caption)
{
    int unused_height;
    if (row_count < 2)
        row_count = 2;
    for (int row = 0; row < INPUT_FORM_MAX_ROWS; row++)
    {
        form_row_labels[row] = "";
        form_row_texts[row] = "";
        form_row_char_filters[row] = char_filter;
        form_row_max_lengths[row] = max_length;
        form_row_password_flags[row] = 0;
        form_row_case_modes[row] = EditCaseMode_None;
    }
    form_visible = true;
    form_dragging = false;
    form_row_count = row_count;
    form_kind = kind;
    form_title = title;
    form_caption = caption;
    form_top_row = 0;
    form_thumb_offset = 0;
    form_scroll_up_held = false;
    form_scroll_down_held = false;
    form_thumb_grabbed = false;
    form_row_labels[0] = "";
    form_row_labels[1] = "";
    form_row_labels[2] = "";
    form_row_labels[3] = "";
    form_row_labels[4] = "";
    int edit_width = 170;
    if (form_row_count > 5)
        edit_width = 145;
    MainForm->edits->AddEdit(EditId_FormFirstRow, EditGroup_InputForm, form_x + 126, form_y + 70, edit_width,
        EDIT_LINE_HEIGHT);
    MainForm->edits->EnableChangeNotify(EditId_FormFirstRow);
    MainForm->edits->SetText(EditId_FormFirstRow, "");
    MainForm->edits->SetTextColor(EditId_FormFirstRow, 0xB4C8DC);
    MainForm->edits->SetCharFilter(EditId_FormFirstRow, char_filter);
    MainForm->edits->SetMaxLength(EditId_FormFirstRow, max_length);
    MainForm->edits->SetBackgroundVisible(EditId_FormFirstRow, false);
    MainForm->edits->SetBorderVisible(EditId_FormFirstRow, false);
    MainForm->edits->AddEdit(EditId_FormSecondRow, EditGroup_InputForm, form_x + 126, form_y + 93, edit_width,
        EDIT_LINE_HEIGHT);
    MainForm->edits->EnableChangeNotify(EditId_FormSecondRow);
    MainForm->edits->SetText(EditId_FormSecondRow, "");
    MainForm->edits->SetTextColor(EditId_FormSecondRow, 0xB4C8DC);
    MainForm->edits->SetCharFilter(EditId_FormSecondRow, char_filter);
    MainForm->edits->SetMaxLength(EditId_FormSecondRow, max_length);
    MainForm->edits->SetBackgroundVisible(EditId_FormSecondRow, false);
    MainForm->edits->SetBorderVisible(EditId_FormSecondRow, false);
    if (form_row_count > 2)
    {
        MainForm->edits->AddEdit(EditId_FormThirdRow, EditGroup_InputForm, form_x + 126, form_y + 116, edit_width,
            EDIT_LINE_HEIGHT);
        MainForm->edits->EnableChangeNotify(EditId_FormThirdRow);
        MainForm->edits->SetText(EditId_FormThirdRow, "");
        MainForm->edits->SetTextColor(EditId_FormThirdRow, 0xB4C8DC);
        MainForm->edits->SetCharFilter(EditId_FormThirdRow, char_filter);
        MainForm->edits->SetMaxLength(EditId_FormThirdRow, max_length);
        MainForm->edits->SetBackgroundVisible(EditId_FormThirdRow, false);
        MainForm->edits->SetBorderVisible(EditId_FormThirdRow, false);
    }
    if (form_row_count > 3)
    {
        MainForm->edits->AddEdit(EditId_FormFourthRow, EditGroup_InputForm, form_x + 126, form_y + 139, edit_width,
            EDIT_LINE_HEIGHT);
        MainForm->edits->EnableChangeNotify(EditId_FormFourthRow);
        MainForm->edits->SetText(EditId_FormFourthRow, "");
        MainForm->edits->SetTextColor(EditId_FormFourthRow, 0xB4C8DC);
        MainForm->edits->SetCharFilter(EditId_FormFourthRow, char_filter);
        MainForm->edits->SetMaxLength(EditId_FormFourthRow, max_length);
        MainForm->edits->SetBackgroundVisible(EditId_FormFourthRow, false);
        MainForm->edits->SetBorderVisible(EditId_FormFourthRow, false);
    }
    if (form_row_count > 4)
    {
        MainForm->edits->AddEdit(EditId_FormFifthRow, EditGroup_InputForm, form_x + 126, form_y + 162, edit_width,
            EDIT_LINE_HEIGHT);
        MainForm->edits->EnableChangeNotify(EditId_FormFifthRow);
        MainForm->edits->SetText(EditId_FormFifthRow, "");
        MainForm->edits->SetTextColor(EditId_FormFifthRow, 0xB4C8DC);
        MainForm->edits->SetCharFilter(EditId_FormFifthRow, char_filter);
        MainForm->edits->SetMaxLength(EditId_FormFifthRow, max_length);
        MainForm->edits->SetBackgroundVisible(EditId_FormFifthRow, false);
        MainForm->edits->SetBorderVisible(EditId_FormFifthRow, false);
    }
    int form_rows = form_row_count;
    // Each row under five makes the form one row shorter, so it may sit lower on screen
    if (form_rows < 5)
        form_rows = INPUT_FORM_ROW_HEIGHT * (5 - form_rows);
    else
        form_rows = 0;
    if (form_rows + 69 <= form_y)
        form_y = form_rows + 69;
    MainForm->edits->FocusEdit(EditId_FormFirstRow);
    form_dragging = false;
    form_ok_hovered = false;
    form_cancel_hovered = false;
    MainForm->redraw_needed = true;
}

// Opens the input box with the prompt and an empty box for the kind's answer, with its character filter and length;
// a box of 14 characters takes no spaces.
void GameController::OpenInputBox(int kind, String prompt, int char_filter, int max_length)
{
    input_box_visible = true;
    input_box_dragging = false;
    input_box_lines[0] = prompt;
    input_box_lines[1] = "";
    input_box_kind = kind;
    MainForm->edits->AddEdit(EditId_InputBox, EditGroup_Game, input_box_x + 43, input_box_y + 73, 178,
        EDIT_LINE_HEIGHT);
    MainForm->edits->EnableChangeNotify(EditId_InputBox);
    MainForm->edits->SetText(EditId_InputBox, "");
    MainForm->edits->SetTextColor(EditId_InputBox, 0xB4C8DC);
    MainForm->edits->SetCharFilter(EditId_InputBox, char_filter);
    MainForm->edits->SetMaxLength(EditId_InputBox, max_length);
    MainForm->edits->SetBackgroundVisible(EditId_InputBox, false);
    MainForm->edits->SetBorderVisible(EditId_InputBox, false);
    if (max_length == 14)
    {
        MainForm->edits->SetCharFilter(EditId_InputBox, EditCharFilter_Letters);
        MainForm->edits->SetAllowSpaces(EditId_InputBox, false);
    }
    input_box_dragging = false;
    input_box_ok_hovered = false;
    input_box_cancel_hovered = false;
    MainForm->redraw_needed = true;
}

// Opens the amount box for an item dropped on a map tile; the tile is kept for the drop request.
void GameController::OpenAmountBoxAtTile(int kind, int max_amount, int item_id, int reserved, int x, int y)
{
    amount_box_visible = true;
    amount_slider_grabbed = false;
    amount_box_dragging = false;
    amount_item_id = item_id;
    amount_box_reserved = reserved;
    amount_box_kind = kind;
    amount_max = max_amount;
    amount_box_opened = true;
    amount_slider_grabbed = false;
    amount_slider_offset = 0;
    drop_x = x;
    drop_y = y;
    MainForm->edits->AddEdit(EditId_Amount, EditGroup_Game, amount_box_x + 158, amount_box_y + 94, 72,
        EDIT_LINE_HEIGHT);
    MainForm->edits->SetNumericRange(EditId_Amount, 1, max_amount);
    MainForm->edits->EnableChangeNotify(EditId_Amount);
    MainForm->edits->SetText(EditId_Amount, "1");
    MainForm->edits->SetTextColor(EditId_Amount, 0xB4C8DC);
    MainForm->edits->SetMaxLength(EditId_Amount, 8);
    MainForm->edits->SetBackgroundVisible(EditId_Amount, false);
    MainForm->edits->SetBorderVisible(EditId_Amount, false);
    MainForm->redraw_needed = true;
}

// Opens the amount box, an edit and a slider from 1 to max_amount; kind picks its prompt and what OK does.
void GameController::OpenAmountBox(int kind, int max_amount, int item_id, int reserved)
{
    amount_box_visible = true;
    amount_slider_grabbed = false;
    amount_box_dragging = false;
    amount_item_id = item_id;
    amount_box_reserved = reserved;
    amount_box_kind = kind;
    amount_max = max_amount;
    amount_box_opened = true;
    amount_slider_grabbed = false;
    amount_slider_offset = 0;
    drop_x = -1;
    drop_y = -1;
    MainForm->edits->AddEdit(EditId_Amount, EditGroup_Game, amount_box_x + 158, amount_box_y + 94, 72,
        EDIT_LINE_HEIGHT);
    MainForm->edits->SetNumericRange(EditId_Amount, 1, max_amount);
    MainForm->edits->EnableChangeNotify(EditId_Amount);
    MainForm->edits->SetText(EditId_Amount, "1");
    MainForm->edits->SetTextColor(EditId_Amount, 0xB4C8DC);
    MainForm->edits->SetMaxLength(EditId_Amount, 8);
    MainForm->edits->SetBackgroundVisible(EditId_Amount, false);
    MainForm->edits->SetBorderVisible(EditId_Amount, false);
    MainForm->redraw_needed = true;
}

// Starts dragging the open box, or grabs the form's scroll thumb or the amount slider; the first open box takes it.
void GameController::MouseDown(int x, int y)
{
    char handled = false;
    if (!handled && message_visible)
    {
        handled = true;
        if (x >= message_x && x <= message_x + 290 && y >= message_y && y <= message_y + 125)
        {
            message_dragging = true;
            message_drag_mode = 1;
            message_drag_offset_x = x - message_x;
            message_drag_offset_y = y - message_y;
        }
    }
    if (!handled && large_box_visible)
    {
        handled = true;
        if (x >= large_box_x && x <= large_box_x + 290 && y >= large_box_y && y <= large_box_y + 189)
        {
            large_box_dragging = true;
            large_box_drag_offset_x = x - large_box_x;
            large_box_drag_offset_y = y - large_box_y;
        }
    }
    if (!handled && input_box_visible)
    {
        handled = true;
        if (x >= input_box_x && x <= input_box_x + 265 && y >= input_box_y && y <= input_box_y + 148)
        {
            input_box_dragging = true;
            input_box_drag_offset_x = x - input_box_x;
            input_box_drag_offset_y = y - input_box_y;
        }
    }
    if (!handled && form_visible)
    {
        handled = false;
        if (form_row_count > 5)
        {
            if (x >= form_x + 284 && x <= form_x + 299)
            {
                if (y >= form_y + form_thumb_offset + 85 && y <= form_y + form_thumb_offset + 100)
                {
                    handled = true;
                    form_thumb_grabbed = true;
                    form_thumb_grab_offset = y - form_thumb_offset - form_y;
                    InputFormDragThumb(y);
                }
            }
            if (x >= form_x + 284 && x <= form_x + 299 && y >= form_y + 71 && y <= form_y + 85)
            {
                form_scroll_up_held = true;
                form_scroll_repeat_delay = 4;
                InputFormScrollUp();
            }
            if (x >= form_x + 284 && x <= form_x + 299 && y >= form_y + 163 && y <= form_y + 177)
            {
                form_scroll_down_held = true;
                form_scroll_repeat_delay = 4;
                InputFormScrollDown();
            }
        }
        if (!handled)
        {
            int body_offset = form_row_count;
            if (body_offset < 5)
                body_offset = 23 * (5 - body_offset);
            else
                body_offset = 0;
            if (x >= form_x && x <= form_x + 330 && y >= form_y - body_offset && y <= form_y + 240 - body_offset)
            {
                form_dragging = true;
                form_drag_offset_x = x - form_x;
                form_drag_offset_y = y - form_y;
            }
        }
    }
    if (!handled && amount_box_visible)
    {
        if (x >= amount_box_x + amount_slider_offset + 25 && x <= amount_box_x + amount_slider_offset + 40 &&
            y >= amount_box_y + 97 && y <= amount_box_y + 111)
        {
            amount_slider_grabbed = true;
            amount_slider_grab_offset = x - (amount_box_x + amount_slider_offset + 25);
        }
        else if (x >= amount_box_x && x <= amount_box_x + 265 && y >= amount_box_y && y <= amount_box_y + 170)
        {
            amount_box_dragging = true;
            amount_box_drag_offset_x = x - amount_box_x;
            amount_box_drag_offset_y = y - amount_box_y;
        }
    }
}

// Moves a dragged box (kept on screen), the form's scroll thumb and the amount slider, and tracks hovers.
void GameController::MouseMove(int x, int y)
{
    char handled = false;
    if (!handled && message_visible)
    {
        handled = true;
        if (message_dragging && message_drag_mode == 1)
        {
            message_x = x - message_drag_offset_x;
            message_y = y - message_drag_offset_y;
            if (message_x <= 10)
                message_x = 10;
            if (message_y <= 8)
                message_y = 8;
            if (message_x >= 340)
                message_x = 340;
            if (message_y >= 184)
                message_y = 184;
            MainForm->redraw_needed = true;
        }
    }
    if (!handled && large_box_visible)
    {
        handled = true;
        if (large_box_dragging)
        {
            large_box_x = x - large_box_drag_offset_x;
            large_box_y = y - large_box_drag_offset_y;
            if (large_box_x <= 10)
                large_box_x = 10;
            if (large_box_y <= 8)
                large_box_y = 8;
            if (large_box_x >= 340)
                large_box_x = 340;
            if (large_box_y >= 120)
                large_box_y = 120;
            MainForm->redraw_needed = true;
        }
    }
    if (!handled && input_box_visible)
    {
        handled = true;
        if (input_box_dragging)
        {
            input_box_x = x - input_box_drag_offset_x;
            input_box_y = y - input_box_drag_offset_y;
            if (input_box_x <= 10)
                input_box_x = 10;
            if (input_box_y <= 8)
                input_box_y = 8;
            if (input_box_x >= 365)
                input_box_x = 365;
            if (input_box_y >= 161)
                input_box_y = 161;
            MainForm->edits->SetPosition(EditId_InputBox, input_box_x + 43, input_box_y + 73);
            MainForm->redraw_needed = true;
        }
    }
    if (!handled && form_visible)
    {
        handled = true;
        if (x >= form_x + 284 && x <= form_x + 299)
        {
            if (form_y + 85 <= y && form_y + 160 >= y)
                InputFormDragThumb(y);
        }
        if (form_dragging)
        {
            int unused_height = form_row_count;
            if (unused_height < 5)
                unused_height = INPUT_FORM_ROW_HEIGHT * (5 - unused_height);
            else
                unused_height = 0;
            form_x = x - form_drag_offset_x;
            form_y = y - form_drag_offset_y;
            if (form_x <= 10)
                form_x = 10;
            if (form_y <= 8)
                form_y = 8;
            if (form_x >= 300)
                form_x = 300;
            if (unused_height + 69 <= form_y)
                form_y = unused_height + 69;
            MainForm->edits->SetPosition(EditId_FormFifthRow, form_x + 126, form_y + 162);
            MainForm->edits->SetPosition(EditId_FormFourthRow, form_x + 126, form_y + 139);
            MainForm->edits->SetPosition(EditId_FormThirdRow, form_x + 126, form_y + 116);
            MainForm->edits->SetPosition(EditId_FormSecondRow, form_x + 126, form_y + 93);
            MainForm->edits->SetPosition(EditId_FormFirstRow, form_x + 126, form_y + 70);
            MainForm->redraw_needed = true;
        }
        if (x >= form_x + 284 && x <= form_x + 299 && form_y + 71 <= y && form_y + 85 >= y)
        {
            if (!form_scroll_up_hovered)
            {
                form_scroll_up_hovered = true;
                MainForm->redraw_needed = true;
            }
        }
        else if (form_scroll_up_hovered)
        {
            form_scroll_up_hovered = false;
            MainForm->redraw_needed = true;
        }
        if (x >= form_x + 284 && x <= form_x + 299 && form_y + 163 <= y && form_y + 177 >= y)
        {
            if (!form_scroll_down_hovered)
            {
                form_scroll_down_hovered = true;
                MainForm->redraw_needed = true;
            }
        }
        else if (form_scroll_down_hovered)
        {
            form_scroll_down_hovered = false;
            MainForm->redraw_needed = true;
        }
    }
    if (!handled && amount_box_visible)
    {
        handled = true;
        if (x >= amount_box_x + amount_slider_offset + 25 && x <= amount_box_x + amount_slider_offset + 40 &&
            amount_box_y + 97 <= y && amount_box_y + 111 >= y)
        {
            if (!amount_slider_hovered)
            {
                amount_slider_hovered = true;
                MainForm->redraw_needed = true;
            }
        }
        else if (amount_slider_hovered)
        {
            amount_slider_hovered = false;
            MainForm->redraw_needed = true;
        }
        if (amount_slider_grabbed)
        {
            int slider_offset = x - (amount_box_x + 25) - amount_slider_grab_offset;
            if (slider_offset < 0)
                slider_offset = 0;
            if (slider_offset > 107)
                slider_offset = 107;
            if (amount_max > 1)
            {
                // The slider's left end is 1 and its right end max_amount
                double slider_fraction = slider_offset / 107.0;
                int amount_offset = (amount_max - 1) * slider_fraction;
                amount_slider_offset = slider_offset;
                amount_offset++;
                MainForm->edits->SetText(EditId_Amount, IntToStr(amount_offset));
            }
            else
            {
                amount_slider_offset = 0;
                MainForm->edits->SetText(EditId_Amount, "1");
            }
            MainForm->redraw_needed = true;
        }
        else if (amount_box_dragging)
        {
            amount_box_x = x - amount_box_drag_offset_x;
            amount_box_y = y - amount_box_drag_offset_y;
            if (amount_box_x <= 10)
                amount_box_x = 10;
            if (amount_box_y <= 8)
                amount_box_y = 8;
            if (amount_box_x >= 365)
                amount_box_x = 365;
            if (amount_box_y >= 139)
                amount_box_y = 139;
            MainForm->edits->SetPosition(EditId_Amount, amount_box_x + 158, amount_box_y + 94);
            MainForm->redraw_needed = true;
        }
    }
    if (!handled && player_menu_visible)
    {
        player_menu_hovered_option = -1;
        if (x > player_menu_x + 12 && x < player_menu_x + 92)
        {
            if (player_menu_y + 13 < y && player_menu_y + 131 > y)
            {
                // The options are rows of equal height from the menu's top
                player_menu_hovered_option = (y - (player_menu_y + 17)) / 15;
                MainForm->redraw_needed = true;
            }
        }
    }
}

// Fills the first empty row of the form; a row on the first page shows at once.
void GameController::AddInputFormRow(String label, String text, short char_filter, short max_length, short case_mode,
    char password)
{
    for (int row = 0; row < INPUT_FORM_MAX_ROWS; row++)
    {
        if (form_row_labels[row] == "")
        {
            form_row_labels[row] = label;
            form_row_texts[row] = text;
            form_row_char_filters[row] = char_filter;
            form_row_max_lengths[row] = max_length;
            form_row_case_modes[row] = case_mode;
            form_row_password_flags[row] = password;
            if (row < 5 && !form_top_row)
                ShowInputFormRows(0);
            break;
        }
    }
}

// Loads the five rows from first_row into the form's five edits.
void GameController::ShowInputFormRows(int first_row)
{
    MainForm->edits->SetText(EditId_FormFirstRow, form_row_texts[first_row]);
    MainForm->edits->SetText(EditId_FormSecondRow, form_row_texts[first_row + 1]);
    MainForm->edits->SetText(EditId_FormThirdRow, form_row_texts[first_row + 2]);
    MainForm->edits->SetText(EditId_FormFourthRow, form_row_texts[first_row + 3]);
    MainForm->edits->SetText(EditId_FormFifthRow, form_row_texts[first_row + 4]);
    MainForm->edits->SetMaxLength(EditId_FormFirstRow, form_row_max_lengths[first_row]);
    MainForm->edits->SetMaxLength(EditId_FormSecondRow, form_row_max_lengths[first_row + 1]);
    MainForm->edits->SetMaxLength(EditId_FormThirdRow, form_row_max_lengths[first_row + 2]);
    MainForm->edits->SetMaxLength(EditId_FormFourthRow, form_row_max_lengths[first_row + 3]);
    MainForm->edits->SetMaxLength(EditId_FormFifthRow, form_row_max_lengths[first_row + 4]);
    if (form_row_max_lengths[first_row] <= 3)
        MainForm->edits->SetAllowSpaces(EditId_FormFirstRow, false);
    if (form_row_max_lengths[first_row + 1] <= 3)
        MainForm->edits->SetAllowSpaces(EditId_FormSecondRow, false);
    if (form_row_max_lengths[first_row + 2] <= 3)
        MainForm->edits->SetAllowSpaces(EditId_FormThirdRow, false);
    if (form_row_max_lengths[first_row + 3] <= 3)
        MainForm->edits->SetAllowSpaces(EditId_FormFourthRow, false);
    if (form_row_max_lengths[first_row + 4] <= 3)
        MainForm->edits->SetAllowSpaces(EditId_FormFifthRow, false);
    MainForm->edits->SetCharFilter(EditId_FormFirstRow, form_row_char_filters[first_row]);
    MainForm->edits->SetCharFilter(EditId_FormSecondRow, form_row_char_filters[first_row + 1]);
    MainForm->edits->SetCharFilter(EditId_FormThirdRow, form_row_char_filters[first_row + 2]);
    MainForm->edits->SetCharFilter(EditId_FormFourthRow, form_row_char_filters[first_row + 3]);
    MainForm->edits->SetCharFilter(EditId_FormFifthRow, form_row_char_filters[first_row + 4]);
    MainForm->edits->SetCaseMode(EditId_FormFirstRow, form_row_case_modes[first_row]);
    MainForm->edits->SetCaseMode(EditId_FormSecondRow, form_row_case_modes[first_row + 1]);
    MainForm->edits->SetCaseMode(EditId_FormThirdRow, form_row_case_modes[first_row + 2]);
    MainForm->edits->SetCaseMode(EditId_FormFourthRow, form_row_case_modes[first_row + 3]);
    MainForm->edits->SetCaseMode(EditId_FormFifthRow, form_row_case_modes[first_row + 4]);
    MainForm->edits->SetPasswordMode(EditId_FormFirstRow, form_row_password_flags[first_row]);
    MainForm->edits->SetPasswordMode(EditId_FormSecondRow, second_row_password_flags[first_row]);
    MainForm->edits->SetPasswordMode(EditId_FormThirdRow, third_row_password_flags[first_row]);
    MainForm->edits->SetPasswordMode(EditId_FormFourthRow, fourth_row_password_flags[first_row]);
    MainForm->edits->SetPasswordMode(EditId_FormFifthRow, fifth_row_password_flags[first_row]);
}

// Stores the texts of the five edits back into the rows on screen.
void GameController::SaveInputFormRows()
{
    form_row_texts[form_top_row] = MainForm->edits->GetText(EditId_FormFirstRow);
    form_row_texts[form_top_row + 1] = MainForm->edits->GetText(EditId_FormSecondRow);
    form_row_texts[form_top_row + 2] = MainForm->edits->GetText(EditId_FormThirdRow);
    form_row_texts[form_top_row + 3] = MainForm->edits->GetText(EditId_FormFourthRow);
    form_row_texts[form_top_row + 4] = MainForm->edits->GetText(EditId_FormFifthRow);
}

int GameController::GetInputFormThumbOffset()
{
    return form_thumb_offset;
}

int GameController::GetInputFormTopRow()
{
    return form_top_row;
}

// Scrolls the form up one row, keeping the texts typed so far, and moves the thumb.
void GameController::InputFormScrollUp()
{
    if (form_row_count > 5 && form_top_row > 0)
    {
        SaveInputFormRows();
        form_top_row--;
        ShowInputFormRows(form_top_row);
        double scroll_fraction = (double)form_top_row / (form_row_count - 5);
        double thumb_pos = INPUT_FORM_THUMB_TRAVEL * scroll_fraction;
        form_thumb_offset = thumb_pos;
        MainForm->redraw_needed = true;
    }
}

// Scrolls the form down one row, keeping the texts typed so far, and moves the thumb.
void GameController::InputFormScrollDown()
{
    if (form_row_count - 5 > form_top_row)
    {
        SaveInputFormRows();
        form_top_row++;
        ShowInputFormRows(form_top_row);
        double scroll_fraction = (double)form_top_row / (form_row_count - 5);
        double thumb_pos = INPUT_FORM_THUMB_TRAVEL * scroll_fraction;
        form_thumb_offset = thumb_pos;
        MainForm->redraw_needed = true;
    }
}

// Sets the form's top row from the grabbed thumb's place on its track.
void GameController::InputFormDragThumb(int y)
{
    int new_top_row;
    if (form_thumb_grabbed)
    {
        int thumb_offset = y - form_thumb_grab_offset - form_y;
        if (thumb_offset < 0)
            thumb_offset = 0;
        if (thumb_offset > 60)
            thumb_offset = 60;
        if (form_row_count > 5)
        {
            double thumb_fraction = thumb_offset / 60.0;
            double scaled_row = (form_row_count - 5.0) * thumb_fraction;
            new_top_row = scaled_row;
            if (new_top_row != form_top_row)
            {
                SaveInputFormRows();
                form_top_row = new_top_row;
                ShowInputFormRows(form_top_row);
            }
        }
        form_thumb_offset = thumb_offset;
        MainForm->redraw_needed = true;
    }
}

// Sends a chair-sit request when the tile is a chair; true when sent.
char GameController::TrySitOnChair(int x, int y, int tile_spec)
{
    char sat = false;
    if (tile_spec >= TileSpec_ChairYHigh && tile_spec <= TileSpec_ChairAnySide)
    {
        MainForm->map->BlockActionsBriefly();
        MainForm->animation->StartTileUseDelay();
        MainForm->animation->StartSitDelay(SIT_DELAY_TICKS);
        MainForm->net_client->sender->SendChairSit(x, y);
        sat = true;
    }
    return sat;
}

// Asks to open the locker at the tile when the tile is a locker.
bool GameController::TryOpenLocker(int x, int y, int tile_spec)
{
    bool handled = false;
    if (tile_spec == TileSpec_Locker)
    {
        handled = true;
        MainForm->animation->StartTileUseDelay();
        TPoint pos;
        pos.x = x;
        pos.y = y;
        MainForm->net_client->sender->RequestLockerOpen(pos);
    }
    return handled;
}

// Asks to open the board when the tile is a board; the board specs are consecutive, the number sent is the offset.
bool GameController::TryOpenBoard(int x, int y, int tile_spec)
{
    bool handled = false;
    if (tile_spec >= TileSpec_Board1 && tile_spec <= TileSpec_Board8)
    {
        handled = true;
        MainForm->net_client->sender->RequestBoardOpen(tile_spec - TileSpec_Board1);
    }
    return handled;
}

// Asks to open the jukebox at the tile when the tile is a jukebox.
bool GameController::TryOpenJukebox(int x, int y, int tile_spec)
{
    bool handled = false;
    if (tile_spec == TileSpec_Jukebox)
    {
        handled = true;
        TPoint pos;
        pos.x = x;
        pos.y = y;
        MainForm->net_client->sender->RequestJukeboxOpen(pos);
    }
    return handled;
}

// Asks to open the chest at the tile; a locked chest needs its key in the inventory, else the key's name shows.
bool GameController::TryOpenChest(int x, int y, int tile_spec)
{
    int key_id;
    bool handled = false;
    if (tile_spec == TileSpec_Chest)
    {
        handled = true;
        TPoint pos;
        pos.x = x;
        pos.y = y;
        key_id = MainForm->map->GetChestKey(x, y);
        // A chest with a key opens only for a player who holds it.
        if (key_id > 0)
        {
            if (MainForm->animation->interface_controller->inventory->HasKey(key_id))
                MainForm->net_client->sender->RequestChestOpen(pos);
            else
            {
                String key_name = MainForm->item_values->GetKeyName(key_id);
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                    MainForm->strings->GetText(GameText_ChestIsLocked2) + " - " + key_name);
                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_ChestLocked, false),
                    MainForm->strings->GetDialogText(DialogText_ChestLocked, true));
                MainForm->sfx->PlaySfx(SfxSound_Locked);
            }
        }
        else
            MainForm->net_client->sender->RequestChestOpen(pos);
    }
    return handled;
}

// Opens the door at the tile; a locked door needs its key in the inventory, else the key's name shows.
bool GameController::TryOpenDoor(int x, int y, int tile_spec)
{
    bool handled = false;
    if (tile_spec == TileSpec_Door)
    {
        int key_id = MainForm->map->GetDoorKey(x, y);
        if (key_id > 0)
        {
            handled = true;
            if (MainForm->animation->interface_controller->inventory->HasKey(key_id))
            {
                MainForm->net_client->sender->SendOpenDoor(x, y);
                MainForm->map->BlockActionsBriefly();
            }
            else
            {
                String key_name = MainForm->item_values->GetKeyName(key_id);
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                    MainForm->strings->GetText(GameText_DoorIsLocked2) + " - " + key_name);
                MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_DoorLocked, false),
                    MainForm->strings->GetDialogText(DialogText_DoorLocked, true));
                MainForm->sfx->PlaySfx(SfxSound_Locked);
            }
        }
        // A door with no key opens at once.
        if (!key_id)
        {
            handled = true;
            MainForm->net_client->sender->SendOpenDoor(x, y);
            MainForm->map->BlockActionsBriefly();
        }
    }
    return handled;
}

// One timer tick: the idle emote and logout, the spot countdown, walking through a blocking player, held form scroll
// buttons.
void GameController::Tick()
{
    if (MainForm->animation->GetScreenMode() != ScreenMode_Game)
        return;
    idle_ticks++;
    // Idle in the game: an emote now and then, later two warnings, then the logout
    if (idle_ticks >= 2500)
    {
        if (idle_ticks % 250 == 0)
            PlayEmote(Emote_Moon);
        if (MainForm->config->GetIdleDisconnect())
        {
            if (idle_ticks >= 15000)
            {
                if (idle_ticks % 500 == 0)
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                        MainForm->strings->GetText(GameText_YouHaveBeenIdle));
                if (idle_ticks % 500 == 30)
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Button,
                        MainForm->strings->GetText(GameText_StartMovingYourCharacter));
            }
            if (idle_ticks >= 17500)
            {
                idle_ticks = 0;
                MainForm->sfx->PlaySfx(SfxSound_DialogButtonClick);
                MainForm->animation->interface_controller->Logout();
            }
        }
    }
    if (spot_countdown > 0)
    {
        spot_countdown--;
        // While the player stays on the spot: a sound and a warning at each of these counts, then the
        // connection closes
        if ((spot_countdown == 80 || spot_countdown == 70 || spot_countdown == 60 || spot_countdown == 50 ||
            spot_countdown == 40) && MainForm->players->GetX(MainForm->net_client->GetPlayerId()) == spot_x &&
            MainForm->players->GetY(MainForm->net_client->GetPlayerId()) == spot_y)
        {
            MainForm->sfx->PlaySfx(SfxSound_Countdown);
            if (spot_countdown == 70)
                MainForm->players->ShowBalloon(MainForm->net_client->GetPlayerId(),
                    MainForm->strings->GetText(GameText_MoveAwayFromPlace), -1);
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                MainForm->strings->GetText(GameText_DoNotBlockArena));
        }
        if (spot_countdown == 30 && MainForm->players->GetX(MainForm->net_client->GetPlayerId()) == spot_x &&
            MainForm->players->GetY(MainForm->net_client->GetPlayerId()) == spot_y)
            ((TAbstractSocket *)MainForm->client)->Close();
    }
    // Walk notes the player in the way; the same one blocking for long enough is walked through
    if (blocking_player_id > -1)
    {
        if (blocking_player_id == last_blocking_player_id)
        {
            blocked_ticks++;
            if (blocked_ticks == 4)
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                    MainForm->strings->GetText(GameText_KeepMovingIntoPlayer));
            if (blocked_ticks > 42 && !MainForm->map->IsActionBlocked() && !MainForm->animation->IsWalkingBlocked() &&
                !MainForm->players->IsCasting(MainForm->net_client->GetPlayerId()) &&
                !MainForm->players->IsAttacking(MainForm->net_client->GetPlayerId()) &&
                !MainForm->players->IsSittingOnChair(MainForm->net_client->GetPlayerId()) &&
                !MainForm->players->IsSittingOnFloor(MainForm->net_client->GetPlayerId()))
            {
                MainForm->sfx->PlaySfx(SfxSound_GhostWalk);
                MainForm->map->StartScroll(blocked_direction);
                MainForm->net_client->sender->SendWalkGhost(blocked_direction, blocked_x, blocked_y,
                    last_blocking_player_id);
                MainForm->players->StartWalk(MainForm->net_client->GetPlayerId(), blocked_x, blocked_y,
                    blocked_direction);
                blocked_ticks = 0;
            }
        }
        else
            blocked_ticks = 0;
        last_blocking_player_id = blocking_player_id;
        blocking_player_id = -1;
    }
    else
        blocked_ticks = 0;
    if (form_row_count > 5)
    {
        if (form_scroll_repeat_delay > 0)
            form_scroll_repeat_delay--;
        else
        {
            if (form_scroll_up_held)
                InputFormScrollUp();
            if (form_scroll_down_held)
                InputFormScrollDown();
        }
    }
}

// Ends every drag and held scroll button.
void GameController::MouseUp()
{
    message_dragging = false;
    large_box_dragging = false;
    input_box_dragging = false;
    form_dragging = false;
    form_thumb_grabbed = false;
    form_scroll_up_held = false;
    form_scroll_down_held = false;
    amount_box_dragging = false;
    amount_slider_grabbed = false;
}

void GameController::ResetIdleTimer()
{
    idle_ticks = 0;
}

// The total experience at which a level starts; 0 for level 0 and below.
int GameController::GetExperienceForLevel(int level)
{
    int experience = 0;
    if (level > 0)
        experience = (int)(level * 1.1 * (level * 1.1) * (level * 1.1) * 100.0);
    return experience;
}

// Starts the spot countdown when the tile or one next to it is a spot tile; the arena notice passes the own tile.
void GameController::StartSpotCountdown(int x, int y)
{
    if (MainForm->map->GetTileSpec(x, y) == TileSpec_CountdownSpot ||
        MainForm->map->GetTileSpec(x - 1, y) == TileSpec_CountdownSpot ||
        MainForm->map->GetTileSpec(x, y + 1) == TileSpec_CountdownSpot ||
        MainForm->map->GetTileSpec(x + 1, y) == TileSpec_CountdownSpot ||
        MainForm->map->GetTileSpec(x, y + 1) == TileSpec_CountdownSpot)
    {
        spot_countdown = 130;
        spot_x = x;
        spot_y = y;
    }
}

void GameController::SetGhostWalk(char enabled)
{
    ghost_walk = enabled;
}

// Turns walking through walls, players and npcs on or off, for the higher admin levels.
void GameController::ToggleGhostWalk()
{
    if (MainForm->net_client->GetAdminLevel() > 2)
    {
        if (ghost_walk)
            ghost_walk = false;
        else
            ghost_walk = true;
    }
}

// True when enough time passed since the last call; every call restarts the time.
bool GameController::IsActionDelayElapsed()
{
    return true;
}
