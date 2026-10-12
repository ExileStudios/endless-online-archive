#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Ctrl_interface.h"
#include "Packetoperations.h"
#include "Ctrl_status.h"
#pragma package(smart_init)

PathFinder::PathFinder()
{
    current_step = 0;
    is_active = 0;
}

PathFinder::~PathFinder()
{
    nodes.clear();
}

// Searches outward from the player, one step a pass, for a free tile path to the target, 12 steps at most.
// It refuses a blocked or taken target and a player who is casting, attacking or sitting.
char PathFinder::FindPath(int target_x, int target_y)
{
    char found;
    int start_x;
    int start_y;
    int i;
    int j;
    found = 0;
    if (!MainForm->animation->IsPathDelayed() && !MainForm->map->IsBlocked(target_x, target_y) &&
        !MainForm->players->HasPlayerAt(target_x, target_y) && !MainForm->npcs->HasNpcAt(target_x, target_y) &&
        target_x >= 0 && target_y >= 0 && MainForm->map->GetWidth() >= target_x &&
        MainForm->map->GetHeight() >= target_y)
    {
        if (!MainForm->players->IsCasting(MainForm->net_client->GetPlayerId()))
        {
            if (!MainForm->players->IsAttacking(MainForm->net_client->GetPlayerId()))
            {
                if (!MainForm->players->IsSittingOnChair(MainForm->net_client->GetPlayerId()))
                {
                    if (!MainForm->players->IsSittingOnFloor(MainForm->net_client->GetPlayerId()))
                    {
                        MainForm->animation->StartPathDelay(2);
                        nodes.clear();
                        char reached_expanded;
                        int next_node_id = 0;
                        int depth = 0;
                        int path_node_id = 0;
                        generation_end = 1;
                        previous_generation_end = 1;
                        generation_start = 1;
                        start_x = MainForm->players->GetX(MainForm->net_client->GetPlayerId());
                        start_y = MainForm->players->GetY(MainForm->net_client->GetPlayerId());
                        PathNode start_node(next_node_id, -1, start_x, start_y, depth);
                        nodes.insert(nodes.end(), start_node);
                        next_node_id++;
                        if (start_x != target_x || start_y != target_y)
                        {
                            do
                            {
                                depth++;
                                reached_expanded = 0;
                                generation_start = previous_generation_end;
                                previous_generation_end = generation_end;
                                generation_end = nodes.size();
                                int node_count = nodes.size();
                                for (i = node_count - 1; i >= 0 && !reached_expanded; i--)
                                {
                                    if (!nodes[i].expanded)
                                    {
                                        nodes[i].expanded = true;
                                        int node_x = nodes[i].x;
                                        int node_y = nodes[i].y;
                                        if (MainForm->map->GetWidth() >= node_x + 1)
                                        {
                                            if (!MainForm->map->IsBlocked(node_x + 1, node_y))
                                            {
                                                if (!ContainsNode(node_x + 1, node_y))
                                                {
                                                    PathNode handle(next_node_id, nodes[i].id, node_x + 1, node_y,
                                                        depth);
                                                    next_node_id++;
                                                    nodes.insert(nodes.end(), handle);
                                                    if (node_x + 1 == target_x && node_y == target_y)
                                                    {
                                                        path_node_id = handle.id;
                                                        found = 1;
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                        if (MainForm->map->GetHeight() >= node_y + 1)
                                        {
                                            if (!MainForm->map->IsBlocked(node_x, node_y + 1))
                                            {
                                                if (!ContainsNode(node_x, node_y + 1))
                                                {
                                                    PathNode down_node(next_node_id, nodes[i].id, node_x, node_y + 1,
                                                        depth);
                                                    next_node_id++;
                                                    nodes.insert(nodes.end(), down_node);
                                                    if (node_x == target_x && node_y + 1 == target_y)
                                                    {
                                                        path_node_id = down_node.id;
                                                        found = 1;
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                        if (node_x - 1 >= 0)
                                        {
                                            if (!MainForm->map->IsBlocked(node_x - 1, node_y))
                                            {
                                                if (!ContainsNode(node_x - 1, node_y))
                                                {
                                                    PathNode left_node(next_node_id, nodes[i].id, node_x - 1, node_y,
                                                        depth);
                                                    next_node_id++;
                                                    nodes.insert(nodes.end(), left_node);
                                                    if (node_x - 1 == target_x && node_y == target_y)
                                                    {
                                                        path_node_id = left_node.id;
                                                        found = 1;
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                        if (node_y - 1 >= 0)
                                        {
                                            if (!MainForm->map->IsBlocked(node_x, node_y - 1))
                                            {
                                                if (!ContainsNode(node_x, node_y - 1))
                                                {
                                                    PathNode up_node(next_node_id, nodes[i].id, node_x, node_y - 1,
                                                        depth);
                                                    next_node_id++;
                                                    nodes.insert(nodes.end(), up_node);
                                                    if (node_x == target_x && node_y - 1 == target_y)
                                                    {
                                                        path_node_id = up_node.id;
                                                        found = 1;
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else
                                        reached_expanded = 1;
                                }
                            } while (depth <= 11 && !found);
                        }
                        if (found)
                        {
                            step_count = depth;
                            int trace_node_count = nodes.size();
                            int unused_zero = 0;
                            for (j = trace_node_count - 1; j >= 0 && depth >= 1; j--)
                            {
                                if (nodes[j].id != path_node_id)
                                    continue;
                                path_node_id = nodes[j].parent_id;
                                steps[depth - 1].x = nodes[j].x;
                                steps[depth-- - 1].y = nodes[j].y;
                            }
                        }
                        current_step = 0;
                        is_active = found;
                        nodes.clear();
                        MainForm->map->SetPathMarker(target_x, target_y);
                    }
                }
            }
        }
    }
    return found;
}

// True when the tile is in the search already and has been expanded; only the last two passes are searched.
char PathFinder::ContainsNode(int x, int y)
{
    char is_expanded = 0;
    for (int i = nodes.size() - 1; i >= generation_start - 1; i--)
    {
        if (nodes[i].x == x && nodes[i].y == y)
        {
            if (nodes[i].expanded)
            {
                is_expanded = 1;
                break;
            }
            break;
        }
    }
    return is_expanded;
}

// Takes the next step of the path, as a walk or a jump, unless the tile is taken or the warp needs a higher level.
// A step that cannot be taken ends the path.
void PathFinder::FollowPath()
{
    if (IsPathActive())
    {
        if (!MainForm->net_client->CanAct())
            return;
        if (!MainForm->map->IsActionBlocked() && !MainForm->players->IsCasting(MainForm->net_client->GetPlayerId()) &&
            !MainForm->players->IsAttacking(MainForm->net_client->GetPlayerId()) &&
            !MainForm->players->IsSittingOnChair(MainForm->net_client->GetPlayerId()) &&
            !MainForm->players->IsSittingOnFloor(MainForm->net_client->GetPlayerId()) && !MainForm->map->IsScrolling())
        {
            int player_x = MainForm->players->GetX(MainForm->net_client->GetPlayerId());
            int player_y = MainForm->players->GetY(MainForm->net_client->GetPlayerId());
            int v51_1 = MainForm->players->GetDirection(MainForm->net_client->GetPlayerId());
            int next_x = GetCurrentStep(MainForm->map->path_finder).x;
            int next_y = GetCurrentStep(MainForm->map->path_finder).y;
            MainForm->map->path_finder->AdvanceStep();
            char is_stuck = 1;
            bool on_jump_tile = 0;
            if (MainForm->map->GetTileSpec(player_x, player_y) == TileSpec_Jump)
                on_jump_tile = 1;
            if (next_x > player_x && next_y == player_y)
            {
                if (MainForm->map->GetCenterX() <= MainForm->map->GetWidth() &&
                    !MainForm->map->IsBlocked(next_x, next_y) && !MainForm->players->HasPlayerAt(next_x, next_y) &&
                    !MainForm->npcs->HasNpcAt(next_x, next_y))
                {
                    int right_warp_level = MainForm->map->GetWarpLevel(next_x, next_y);
                    if (MainForm->animation->interface_controller->status->level >= right_warp_level)
                    {
                        MainForm->map->StartScroll(FacingDirection_Right);
                        MainForm->animation->game->attack_count = 0;
                        MainForm->net_client->sender->SendWalk(FacingDirection_Right, next_x, next_y);
                        char walk_right = 1;
                        if (MainForm->map->GetTileSpec(next_x, next_y) == TileSpec_Jump || on_jump_tile)
                        {
                            walk_right = 0;
                            MainForm->players->StartJump(MainForm->net_client->GetPlayerId(), next_x, next_y,
                                FacingDirection_Right);
                        }
                        if (MainForm->map->GetTileSpec(next_x, next_y) == TileSpec_Splash)
                        {
                            MainForm->players->StartEffect(MainForm->net_client->GetPlayerId(), 8);
                            MainForm->sfx->PlaySfx(42);
                        }
                        if (walk_right)
                            MainForm->players->StartWalk(MainForm->net_client->GetPlayerId(), next_x, next_y,
                                FacingDirection_Right);
                        is_stuck = 0;
                    }
                    else
                    {
                        MainForm->map->BlockActionsBriefly();
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                            MainForm->strings->GetText(GameText_YouAreNotReady) + " - LVL " +
                            IntToStr(right_warp_level));
                    }
                }
            }
            if (next_x < player_x && next_y == player_y && MainForm->map->GetCenterX() >= 0 &&
                !MainForm->map->IsBlocked(next_x, next_y) && !MainForm->players->HasPlayerAt(next_x, next_y) &&
                !MainForm->npcs->HasNpcAt(next_x, next_y))
            {
                int left_warp_level = MainForm->map->GetWarpLevel(next_x, next_y);
                if (MainForm->animation->interface_controller->status->level >= left_warp_level)
                {
                    MainForm->map->StartScroll(FacingDirection_Left);
                    MainForm->animation->game->attack_count = 0;
                    MainForm->net_client->sender->SendWalk(FacingDirection_Left, next_x, next_y);
                    char walk_left = 1;
                    if (MainForm->map->GetTileSpec(next_x, next_y) == TileSpec_Jump || on_jump_tile)
                    {
                        walk_left = 0;
                        MainForm->players->StartJump(MainForm->net_client->GetPlayerId(), next_x, next_y,
                            FacingDirection_Left);
                    }
                    if (MainForm->map->GetTileSpec(next_x, next_y) == TileSpec_Splash)
                    {
                        MainForm->players->StartEffect(MainForm->net_client->GetPlayerId(), 8);
                        MainForm->sfx->PlaySfx(42);
                    }
                    if (walk_left)
                        MainForm->players->StartWalk(MainForm->net_client->GetPlayerId(), next_x, next_y,
                            FacingDirection_Left);
                    is_stuck = 0;
                }
                else
                {
                    MainForm->map->BlockActionsBriefly();
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                        MainForm->strings->GetText(GameText_YouAreNotReady) + " - LVL " + IntToStr(left_warp_level));
                }
            }
            if (next_y > player_y && next_x == player_x && MainForm->map->GetCenterY() >= 0 &&
                !MainForm->map->IsBlocked(next_x, next_y) && !MainForm->players->HasPlayerAt(next_x, next_y) &&
                !MainForm->npcs->HasNpcAt(next_x, next_y))
            {
                int down_warp_level = MainForm->map->GetWarpLevel(next_x, next_y);
                if (MainForm->animation->interface_controller->status->level >= down_warp_level)
                {
                    MainForm->map->StartScroll(FacingDirection_Down);
                    MainForm->animation->game->attack_count = 0;
                    MainForm->net_client->sender->SendWalk(FacingDirection_Down, next_x, next_y);
                    char walk_down = 1;
                    if (MainForm->map->GetTileSpec(next_x, next_y) == TileSpec_Jump || on_jump_tile)
                    {
                        walk_down = 0;
                        MainForm->players->StartJump(MainForm->net_client->GetPlayerId(), next_x, next_y,
                            FacingDirection_Down);
                    }
                    if (MainForm->map->GetTileSpec(next_x, next_y) == TileSpec_Splash)
                    {
                        MainForm->players->StartEffect(MainForm->net_client->GetPlayerId(), 8);
                        MainForm->sfx->PlaySfx(42);
                    }
                    if (walk_down)
                        MainForm->players->StartWalk(MainForm->net_client->GetPlayerId(), next_x, next_y,
                            FacingDirection_Down);
                    is_stuck = 0;
                }
                else
                {
                    MainForm->map->BlockActionsBriefly();
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                        MainForm->strings->GetText(GameText_YouAreNotReady) + " - LVL " + IntToStr(down_warp_level));
                }
            }
            if (next_y < player_y && next_x == player_x)
            {
                if (MainForm->map->GetCenterY() <= MainForm->map->GetHeight() &&
                    !MainForm->map->IsBlocked(next_x, next_y) && !MainForm->players->HasPlayerAt(next_x, next_y) &&
                    !MainForm->npcs->HasNpcAt(next_x, next_y))
                {
                    int up_warp_level = MainForm->map->GetWarpLevel(next_x, next_y);
                    if (MainForm->animation->interface_controller->status->level >= up_warp_level)
                    {
                        MainForm->map->StartScroll(FacingDirection_Up);
                        MainForm->animation->game->attack_count = 0;
                        MainForm->net_client->sender->SendWalk(FacingDirection_Up, next_x, next_y);
                        char walk_up = 1;
                        if (MainForm->map->GetTileSpec(next_x, next_y) == TileSpec_Jump || on_jump_tile)
                        {
                            walk_up = 0;
                            MainForm->players->StartJump(MainForm->net_client->GetPlayerId(), next_x, next_y,
                                FacingDirection_Up);
                        }
                        if (MainForm->map->GetTileSpec(next_x, next_y) == TileSpec_Splash)
                        {
                            MainForm->players->StartEffect(MainForm->net_client->GetPlayerId(), 8);
                            MainForm->sfx->PlaySfx(42);
                        }
                        if (walk_up)
                            MainForm->players->StartWalk(MainForm->net_client->GetPlayerId(), next_x, next_y,
                                FacingDirection_Up);
                        is_stuck = 0;
                    }
                    else
                    {
                        MainForm->map->BlockActionsBriefly();
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                            MainForm->strings->GetText(GameText_YouAreNotReady) + " - LVL " + IntToStr(up_warp_level));
                    }
                }
            }
            if (is_stuck)
                MainForm->map->path_finder->CancelPath();
        }
    }
    return;
}

char PathFinder::IsPathActive()
{
    return is_active;
}

void PathFinder::CancelPath()
{
    is_active = 0;
}

Windows::TPoint PathFinder::GetCurrentStep(PathFinder *self)
{
    return self->steps[self->current_step];
}

// Moves to the next step; the path ends after the last.
void PathFinder::AdvanceStep()
{
    if (++current_step >= step_count)
        is_active = 0;
}
