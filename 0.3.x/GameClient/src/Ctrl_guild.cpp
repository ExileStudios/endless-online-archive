#include <vcl.h>
#pragma hdrstop

#include "Ctrl_guild.h"
#include "Tmainform.h"
#include "Ctrl_game.h"
#include "Ctrl_inn.h"
#include "Ctrl_interface.h"
#include "Ctrl_law.h"
#include "Ctrl_memo.h"
#include "Packetoperations.h"
#include "Ctrl_status.h"
#include "Ctrl_inventory.h"
#include "Ctrl_learn.h"
#include "Ctrl_locker.h"
#include "Ctrl_shop.h"

#pragma package(smart_init)

GuildController::GuildController()
{
    lines = new TStringList();
    member_rank_titles = new TStringList();
    window_x = 49;
    window_y = 15;
    unused_flag = true;
    unused_value = 0;
    scroll_up_hovered = false;
    scroll_down_hovered = false;
    scroll_changed = false;
    npc_session_id = -1;
    top_line = 0;
    scroll_thumb_offset = 0;
    hovered_row = -1;
    is_visible = false;
    close_hovered = false;
    back_hovered = false;
    scroll_up_held = false;
    scroll_down_held = false;
    is_dragging = false;
}

GuildController::~GuildController()
{
}

DEFINE_ADD_TEXT(GuildController)

// Shows the guild master's window on its menu page with no text, closing the other npc windows.
void GuildController::Open(String window_title, int session_id)
{
    MainForm->animation->shop->is_visible = false;
    MainForm->animation->locker->is_visible = false;
    MainForm->animation->law->is_visible = false;
    MainForm->animation->learn->is_visible = false;
    MainForm->animation->memo->is_visible = false;
    MainForm->animation->inn->is_visible = false;
    MainForm->gfx->CreateShopGraphics();
    MainForm->gfx->LoadShopGraphics();
    lines->Clear();
    top_line = 0;
    scroll_thumb_offset = 0;
    title = window_title;
    npc_session_id = session_id;
    page = GuildPage_Main;
    guild_bank_gold = 0;
    link_line_index = 0;
    is_visible = true;
    MainForm->redraw_needed = true;
}

// Hides the window and frees its graphics.
void GuildController::Close()
{
    is_visible = false;
    npc_session_id = -1;
    MainForm->gfx->FreeShopGraphics();
    MainForm->redraw_needed = true;
}

// Repeats a held scroll arrow.
DEFINE_TICK(GuildController)

int GuildController::GetScrollThumbOffset()
{
    return scroll_thumb_offset;
}

int GuildController::GetTopLine()
{
    return top_line;
}

// A click in the window. A menu row opens its page or sends its request; on a text page the link line opens the
// input box, form or confirm box for that request. Guild requests check that the character is in a guild, and the
// management rows also check the guild rank. Otherwise the scroll bar and moving the window.
void GuildController::MouseDown(int x, int y, int button)
{
    int gold;
    char row_handled = false;
    if (button == 0 || button == 1)
    {
        if (window_y + 43 < y && window_y + 243 > y && window_x + 17 < x && window_x + 252 > x)
        {
            is_dragging = false;
            int row = top_line + (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
            if (page >= GuildPage_Join)
            {
                row = top_line + (y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;
                if (row == link_line_index)
                {
                    MainForm->sfx->PlaySfx(SfxSound_Select);
                    if (page == GuildPage_Join)
                    {
                        if (!MainForm->animation->interface_controller->status->IsInGuild())
                        {
                            MainForm->animation->game->OpenInputForm(FormKind_GuildJoin, 2, EditCharFilter_Letters, 3,
                                MainForm->strings->GetDialogText(DialogText_JoinGuild, false),
                                MainForm->strings->GetDialogText(DialogText_JoinGuild, true));
                            MainForm->animation->game->AddInputFormRow(MainForm->strings->GetText(GameText_GuildTag),
                                "", EditCharFilter_Letters, 3, EditCaseMode_Upper, false);
                            MainForm->animation->game->AddInputFormRow(MainForm->strings->GetText(GameText_Recruiter),
                                "", EditCharFilter_Letters, 14, EditCaseMode_None, false);
                        }
                        else
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_AlreadyInGuild, false),
                                MainForm->strings->GetDialogText(DialogText_AlreadyInGuild, true));
                    }
                    if (page == GuildPage_Leave)
                    {
                        if (MainForm->animation->interface_controller->status->IsInGuild())
                            MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_LeaveGuild,
                                MainForm->strings->GetDialogText(DialogText_LeaveGuild, false),
                                MainForm->strings->GetDialogText(DialogText_LeaveGuild, true));
                        else
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_NotInGuild, false),
                                MainForm->strings->GetDialogText(DialogText_NotInGuild, true));
                    }
                    if (page == GuildPage_Create)
                    {
                        if (!MainForm->animation->interface_controller->status->IsInGuild())
                        {
                            MainForm->animation->game->OpenInputForm(FormKind_GuildCreate, 3, EditCharFilter_Password,
                                12, MainForm->strings->GetText(GameText_RegisterGuild),
                                MainForm->strings->GetText(GameText_EnterYourGuildDetails));
                            MainForm->animation->game->AddInputFormRow(MainForm->strings->GetText(GameText_GuildTag),
                                "", EditCharFilter_Letters, 3, EditCaseMode_Upper, false);
                            MainForm->animation->game->AddInputFormRow(MainForm->strings->GetText(GameText_GuildName),
                                "", EditCharFilter_Alphanumeric, 24, EditCaseMode_None, false);
                            MainForm->animation->game->AddInputFormRow(MainForm->strings->GetText(GameText_Description),
                                "", EditCharFilter_Password, 240, EditCaseMode_None, false);
                        }
                        else
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_AlreadyInGuild, false),
                                MainForm->strings->GetDialogText(DialogText_AlreadyInGuild, true));
                    }
                    if (page == GuildPage_Description)
                    {
                        MainForm->animation->game->OpenInputBox(InputBoxKind_GuildDescription,
                            MainForm->strings->GetText(GameText_Description), EditCharFilter_Alphanumeric, 14);
                        MainForm->edits->SetCharFilter(EditId_InputBox, EditCharFilter_Letters);
                        MainForm->edits->SetCaseMode(EditId_InputBox, EditCaseMode_Name);
                        MainForm->edits->SetAllowSpaces(EditId_InputBox, false);
                        MainForm->edits->SetMaxLength(EditId_InputBox, 14);
                    }
                    if (page == GuildPage_RemoveMember)
                        MainForm->animation->game->OpenInputBox(InputBoxKind_RemoveGuildMember,
                            MainForm->strings->GetText(GameText_WhoDoYouWantToRemove), EditCharFilter_Alphanumeric, 14);
                    if (page == GuildPage_Disband)
                        MainForm->animation->game->ShowMessageBox(DialogButtons_OkCancel, ConfirmAction_DisbandGuild,
                            MainForm->strings->GetDialogText(DialogText_DisbandGuild, false),
                            MainForm->strings->GetDialogText(DialogText_DisbandGuild, true));
                    if (page == GuildPage_Bank)
                    {
                        gold = MainForm->animation->interface_controller->inventory->GetItemAmount(1);
                        // The amount box opens only with at least this much gold carried
                        if (gold >= 1000)
                        {
                            MainForm->animation->game->OpenAmountBox(AmountBoxKind_GuildDeposit, gold, ItemId_Gold, 0);
                            MainForm->animation->interface_controller->inventory->input_enabled = true;
                        }
                        else
                            MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                                MainForm->strings->GetDialogText(DialogText_DepositMinimum, false),
                                MainForm->strings->GetDialogText(DialogText_DepositMinimum, true) + " " +
                                    MainForm->item_values->GetName(ItemId_Gold));
                    }
                    if (page == GuildPage_Rank)
                    {
                        MainForm->animation->game->OpenInputForm(FormKind_GuildMemberRank, 2, EditCharFilter_Password,
                            12, MainForm->strings->GetText(GameText_Ranking),
                            MainForm->strings->GetText(GameText_AssignRankToMember));
                        MainForm->animation->game->AddInputFormRow(MainForm->strings->GetText(GameText_Name), "",
                            EditCharFilter_Letters, 14, EditCaseMode_None, false);
                        MainForm->animation->game->AddInputFormRow(MainForm->strings->GetText(GameText_Rank), "",
                            EditCharFilter_Alphanumeric, 1, EditCaseMode_None, false);
                        MainForm->edits->SetNumericRange(EditId_FormSecondRow, 1, 9);
                    }
                }
            }
            if (page == GuildPage_Lookup && row >= 0 && row <= 1)
            {
                if (!row)
                {
                    MainForm->animation->game->OpenInputBox(InputBoxKind_GuildInfo,
                        MainForm->strings->GetText(GameText_ToViewInformationAbout), EditCharFilter_Alphanumeric, 3);
                    MainForm->edits->SetCharFilter(EditId_InputBox, EditCharFilter_Letters);
                    MainForm->edits->SetCaseMode(EditId_InputBox, EditCaseMode_Upper);
                    MainForm->edits->SetAllowSpaces(EditId_InputBox, false);
                    MainForm->edits->SetMaxLength(EditId_InputBox, 3);
                }
                if (row == 1)
                {
                    MainForm->animation->game->OpenInputBox(InputBoxKind_GuildMemberList,
                        MainForm->strings->GetText(GameText_ToViewInformationAbout), EditCharFilter_Alphanumeric, 3);
                    MainForm->edits->SetCharFilter(EditId_InputBox, EditCharFilter_Letters);
                    MainForm->edits->SetCaseMode(EditId_InputBox, EditCaseMode_Upper);
                    MainForm->edits->SetAllowSpaces(EditId_InputBox, false);
                    MainForm->edits->SetMaxLength(EditId_InputBox, 3);
                }
            }
            if (page == GuildPage_Manage && row >= 0 && row <= 4)
            {
                if (MainForm->animation->interface_controller->status->IsInGuild())
                {
                    // The management rows need a low rank number
                    if (MainForm->animation->interface_controller->status->guild_rank <= 1)
                    {
                        if (!row)
                            MainForm->net_client->sender->RequestGuildDescription(npc_session_id,
                                MainForm->animation->interface_controller->status->guild_tag);
                        if (row == 1)
                            MainForm->net_client->sender->RequestGuildRanks(npc_session_id,
                                MainForm->animation->interface_controller->status->guild_tag);
                        if (row == 2)
                        {
                            ClearText();
                            AddText(MainForm->strings->GetText(GameText_Ranking), true);
                            AddText(MainForm->strings->GetText(GameText_YouAreAboutToAssign), true);
                            AddText(MainForm->strings->GetText(GameText_RankingsGoFrom1), true);
                            AddText(MainForm->strings->GetText(GameText_ClickHereToAssign), true);
                            page = GuildPage_Rank;
                            MainForm->redraw_needed = true;
                        }
                        if (row == 3)
                        {
                            ClearText();
                            AddText(MainForm->strings->GetText(GameText_RemoveMember), true);
                            AddText(MainForm->strings->GetText(GameText_YouAreAboutToRemove), true);
                            AddText(MainForm->strings->GetText(GameText_UseThisOptionTo), true);
                            AddText(MainForm->strings->GetText(GameText_ClickHereToRemove), true);
                            page = GuildPage_RemoveMember;
                            MainForm->redraw_needed = true;
                        }
                        if (row == 4)
                        {
                            ClearText();
                            AddText(MainForm->strings->GetText(GameText_Disband), true);
                            AddText(MainForm->strings->GetText(GameText_YouAreAboutToDisband), true);
                            AddText(MainForm->strings->GetText(GameText_DisbandingGuildWillRemove), true);
                            AddText(MainForm->strings->GetText(GameText_ClickHereToDisband), true);
                            page = GuildPage_Disband;
                            MainForm->redraw_needed = true;
                        }
                    }
                    else
                        MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_RefusedRanking, false),
                            MainForm->strings->GetDialogText(DialogText_RefusedRanking, true));
                }
                else
                    MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                        MainForm->strings->GetDialogText(DialogText_NotInGuild, false),
                        MainForm->strings->GetDialogText(DialogText_NotInGuild, true));
            }
            if (page == GuildPage_Membership)
            {
                if (!row)
                {
                    ClearText();
                    AddText(MainForm->strings->GetText(GameText_JoinGuild), true);
                    AddText(MainForm->strings->GetText(GameText_YouAreAboutTo), true);
                    AddText(MainForm->strings->GetText(GameText_JoiningGuildIsFree) + " " +
                        MainForm->strings->GetText(GameText_GuildRecruiterMustBe), true);
                    AddText(MainForm->strings->GetText(GameText_ConsiderCarefullyWhatGuild), true);
                    AddText(MainForm->strings->GetText(GameText_ClickHereToJoin), true);
                    page = GuildPage_Join;
                    MainForm->redraw_needed = true;
                }
                if (row == 1)
                {
                    ClearText();
                    AddText(MainForm->strings->GetText(GameText_LeaveGuild), true);
                    AddText(MainForm->strings->GetText(GameText_YouAreAboutToLeave), true);
                    AddText(MainForm->strings->GetText(GameText_RememberThatAfterYou), true);
                    AddText(MainForm->strings->GetText(GameText_ClickHereToLeave), true);
                    page = GuildPage_Leave;
                    MainForm->redraw_needed = true;
                }
                if (row == 2)
                {
                    if (!MainForm->animation->interface_controller->status->IsInGuild())
                    {
                        ClearText();
                        AddText(MainForm->strings->GetText(GameText_RegisterGuild), true);
                        AddText(MainForm->strings->GetText(GameText_YouAreAboutToCreate), true);
                        AddText(MainForm->strings->GetText(GameText_YouNeedToHave), true);
                        AddText(MainForm->strings->GetText(GameText_GuildMasterWillAsk) + " " +
                            MainForm->item_values->GetName(ItemId_Gold), true);
                        AddText(MainForm->strings->GetText(GameText_ClickHereToRegister), true);
                        page = GuildPage_Create;
                        MainForm->redraw_needed = true;
                    }
                    else
                        MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_AlreadyInGuild, false),
                            MainForm->strings->GetDialogText(DialogText_AlreadyInGuild, true));
                }
            }
            if (page == GuildPage_Main)
            {
                if (!row)
                {
                    page = GuildPage_Lookup;
                    MainForm->redraw_needed = true;
                }
                if (row == 1)
                {
                    page = GuildPage_Membership;
                    MainForm->redraw_needed = true;
                }
                if (row == 2)
                {
                    page = GuildPage_Manage;
                    MainForm->redraw_needed = true;
                }
                if (row == 3)
                {
                    if (MainForm->animation->interface_controller->status->IsInGuild())
                        MainForm->net_client->sender->RequestGuildBank(npc_session_id);
                    else
                        MainForm->animation->game->ShowMessageBox(DialogButtons_Ok, ConfirmAction_None,
                            MainForm->strings->GetDialogText(DialogText_NotInGuild, false),
                            MainForm->strings->GetDialogText(DialogText_NotInGuild, true));
                }
            }
        }
    }
    if (!row_handled)
    {
        if (window_x + 252 <= x && window_x + 267 >= x && window_y + scroll_thumb_offset + 58 <= y &&
            window_y + scroll_thumb_offset + 73 >= y)
        {
            thumb_grabbed = true;
            thumb_grab_offset = y - scroll_thumb_offset - window_y;
            DragScrollThumb(y);
        }
        if (window_x + 252 <= x && window_x + 267 >= x && window_y + 44 <= y && window_y + 59 >= y)
        {
            scroll_up_held = true;
            scroll_repeat_delay = 4;
            ScrollUp();
        }
        if (window_x + 252 <= x && window_x + 267 >= x && window_y + 228 <= y && window_y + 243 >= y)
        {
            scroll_down_held = true;
            scroll_repeat_delay = 4;
            ScrollDown();
        }
        if (window_x <= x && window_x + 284 >= x && y >= window_y && window_y + 290 >= y && !thumb_grabbed)
        {
            is_dragging = true;
            drag_offset_x = x - window_x;
            drag_offset_y = y - window_y;
        }
    }
}

// Moves a dragged window, kept on screen, and tracks the hovered row and the close and back buttons.
void GuildController::MouseMove(int x, int y)
{
    int row;
    UpdateScrollHover(x, y);
    // A dragged window stays inside the game view.
    if (is_dragging)
    {
        window_x = x - drag_offset_x;
        window_y = y - drag_offset_y;
        if (window_x <= 10)
            window_x = 10;
        if (window_y <= 8)
            window_y = 8;
        if (window_x >= 346)
            window_x = 346;
        if (window_y >= 19)
            window_y = 19;
        MainForm->redraw_needed = true;
    }
    if (y > window_y + 43 && y < window_y + 243 && x > window_x + 17 && x < window_x + 252 && !is_dragging)
    {
        if (page == GuildPage_Main)
        {
            row = (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
            if (row > 3)
                row = -1;
        }
        if (page == GuildPage_Membership)
        {
            row = (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
            if (row > 2)
                row = -1;
        }
        if (page == GuildPage_Manage)
        {
            row = (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
            if (row > 4)
                row = -1;
        }
        if (page == GuildPage_Lookup)
        {
            row = (y - window_y - 50) / NPC_LIST_ROW_HEIGHT;
            if (row > 1)
                row = -1;
        }
        if (page >= GuildPage_Join)
        {
            row = (y - window_y - 47) / NPC_TEXT_LINE_HEIGHT;
            if (row > 11)
                row = -1;
            if (lines->Count <= row)
                row = -1;
        }
        if (row != hovered_row)
        {
            hovered_row = row;
            MainForm->redraw_needed = true;
        }
    }
    else if (hovered_row >= 0)
    {
        hovered_row = -1;
        MainForm->redraw_needed = true;
    }
    if (page == GuildPage_Main)
    {
        if (x >= window_x + 92 && x <= window_x + 182 && y >= window_y + 253 && y <= window_y + 279)
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
    else
    {
        if (x >= window_x + 144 && x <= window_x + 234 && y >= window_y + 253 && y <= window_y + 279)
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
        if (x >= window_x + 48 && x <= window_x + 138 && y >= window_y + 253 && y <= window_y + 279)
        {
            if (!back_hovered)
            {
                back_hovered = true;
                MainForm->redraw_needed = true;
            }
        }
        else if (back_hovered)
        {
            back_hovered = false;
            MainForm->redraw_needed = true;
        }
    }
}

// Lights the scroll arrow under the mouse and drags a grabbed thumb.
DEFINE_UPDATE_SCROLL_HOVER(GuildController)

// Follows a grabbed scroll thumb, kept on its track, and picks the top line of a text page from its position.
void GuildController::DragScrollThumb(int y)
{
    int new_top_line;
    if (thumb_grabbed)
    {
        int thumb_offset = y - thumb_grab_offset - window_y;
        if (thumb_offset < 0)
            thumb_offset = 0;
        if (thumb_offset > 152)
            thumb_offset = 152;
        if (page >= GuildPage_Join)
        {
            int line_count = lines->Count;
            if (line_count > 12)
            {
                double thumb_fraction = thumb_offset / NPC_THUMB_TRAVEL;
                double scaled_line = (line_count - 12.0) * thumb_fraction;
                new_top_line = scaled_line;
                if (new_top_line != top_line)
                    top_line = new_top_line;
            }
        }
        scroll_thumb_offset = thumb_offset;
        scroll_changed = true;
        MainForm->redraw_needed = true;
    }
}

// Scrolls a text page up one line and moves the scroll thumb to match.
void GuildController::ScrollUp()
{
    if (page >= GuildPage_Join)
    {
        int line_count = lines->Count;
        if (line_count > 12 && top_line > 0)
        {
            top_line--;
            double scroll_fraction = (double)top_line / (line_count - 12);
            double thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
            scroll_thumb_offset = thumb_pos;
            scroll_changed = true;
            MainForm->redraw_needed = true;
        }
    }
}

// Scrolls a text page down one line and moves the scroll thumb to match.
void GuildController::ScrollDown()
{
    double scroll_fraction;
    double thumb_pos;
    if (page >= GuildPage_Join)
    {
        int line_count = lines->Count;
        if (line_count - 12 > top_line)
        {
            top_line++;
            double scroll_fraction = (double)top_line / (line_count - 12);
            double thumb_pos = NPC_THUMB_TRAVEL * scroll_fraction;
            scroll_thumb_offset = thumb_pos;
            scroll_changed = true;
            MainForm->redraw_needed = true;
        }
    }
}

// Ends moving the window, the held scroll arrows and the grabbed thumb.
void GuildController::MouseUp()
{
    is_dragging = false;
    scroll_up_held = false;
    scroll_down_held = false;
    thumb_grabbed = false;
}

// Empties the text page and its rank titles and scrolls back to the top.
void GuildController::ClearText()
{
    lines->Clear();
    member_rank_titles->Clear();
    link_line_index = 0;
    hovered_row = 0;
    top_line = 0;
    scroll_thumb_offset = 0;
}
