#include <vcl.h>
#pragma hdrstop

#include "Packetproces.h"
#include "Tmainform.h"
#include "Ctrl_chat.h"
#include "Ctrl_menu.h"
#include "Ctrl_priest.h"
#include "Packetoperations.h"
#include "Ctrl_bank.h"
#include "Ctrl_barber.h"
#include "Ctrl_book.h"
#include "Ctrl_dialog.h"
#include "Ctrl_game.h"
#include "Ctrl_guild.h"
#include "Ctrl_inn.h"
#include "Ctrl_interface.h"
#include "Gfx_interface.h"
#include "Ctrl_jukebox.h"
#include "Ctrl_law.h"
#include "Ctrl_memo.h"
#include "Gfx_npcs.h"
#include "Ctrl_paperdoll.h"
#include "Ctrl_quests.h"
#include "Ctrl_social.h"
#include "Gfx_sprites.h"
#include "Ctrl_news.h"
#include "Ctrl_playerlist.h"
#include "Ctrl_status.h"
#pragma package(smart_init)

#define ENTRY_ID_LENGTH 2

PacketProcessor::PacketProcessor()
{
}

PacketProcessor::~PacketProcessor()
{
}

// Handles one packet from the server, by its family and action.
// Numbers come encoded (DecodeNumber), and a byte of 255 ends each token of a list.
void PacketProcessor::DispatchPacket(int action, int family, String packet)
{
    int i;
    int npc_graphic = 0;
    int missing_count = 0;
    int ii = 0;
    // Entering the game: the map and the four data files come in parts, then the welcome replies.
    if (family == OP_PLAY)
    {
        if (action == AC_SYN1)
        {
            MainForm->map->map_data = packet.SubString(1, packet.Length() + 1);
            MainForm->map->SaveMapFile(MainForm->map->map_id);
            MainForm->animation->menu->SetMapNeeded(0);
            MainForm->animation->menu->StartRequestDelay();
        }
        if (action == AC_SYN2)
        {
            MainForm->item_values->received_part_count++;
            MainForm->item_values->SaveFilePart(DecodeNumber(String(packet[1])), packet.SubString(2, packet.Length()));
            if (MainForm->item_values->received_part_count > MainForm->item_values->expected_part_count ||
                !MainForm->item_values->is_downloading)
            {
                MainForm->item_values->is_downloading = 0;
                MainForm->animation->menu->SetItemFileNeeded(0);
            }
            MainForm->animation->menu->StartRequestDelay();
        }
        if (action == AC_SYN3)
        {
            MainForm->npc_values->received_part_count++;
            MainForm->npc_values->SaveFilePart(DecodeNumber(String(packet[1])), packet.SubString(2, packet.Length()));
            if (MainForm->npc_values->received_part_count > MainForm->npc_values->expected_part_count ||
                !MainForm->npc_values->is_downloading)
            {
                MainForm->npc_values->is_downloading = 0;
                MainForm->animation->menu->SetNpcFileNeeded(0);
            }
            MainForm->animation->menu->StartRequestDelay();
        }
        if (action == AC_SYN4)
        {
            MainForm->skill_values->received_part_count++;
            MainForm->skill_values->SaveFilePart(DecodeNumber(String(packet[1])), packet.SubString(2, packet.Length()));
            if (MainForm->skill_values->received_part_count > MainForm->skill_values->expected_part_count ||
                !MainForm->skill_values->is_downloading)
            {
                MainForm->skill_values->is_downloading = 0;
                MainForm->animation->menu->SetSkillFileNeeded(0);
            }
            MainForm->animation->menu->StartRequestDelay();
        }
        if (action == AC_SYN5)
        {
            MainForm->class_values->received_part_count++;
            MainForm->class_values->SaveFilePart(DecodeNumber(String(packet[1])), packet.SubString(2, packet.Length()));
            if (MainForm->class_values->received_part_count > MainForm->class_values->expected_part_count ||
                !MainForm->class_values->is_downloading)
            {
                MainForm->class_values->is_downloading = 0;
                MainForm->animation->menu->SetClassFileNeeded(0);
            }
            MainForm->animation->menu->StartRequestDelay();
        }
        if (action == AC_RESULT)
        {
            // The welcome reply:
            // 1 = the character, and each file's checksum and record count (a file that differs is downloaded)
            // 2 = the news, items, spells and what is in view: the game starts
            // 3, 4 = refused, with a message
            int welcome_reply = DecodeNumber(packet.SubString(1, 2));
            if (welcome_reply == WelcomeReply_FileCheck)
            {
                MainForm->net_client->SetSessionId(DecodeNumber(packet.SubString(3, 2)));
                MainForm->animation->menu->selected_character_id = DecodeNumber(packet.SubString(5, 4));
                MainForm->map->map_id = DecodeNumber(packet.SubString(9, 2));
                int map_checksum_high = DecodeNumber(packet.SubString(11, 2));
                int map_checksum_low = DecodeNumber(packet.SubString(13, 2));
                int map_file_size = DecodeNumber(packet.SubString(15, 3));
                MainForm->map->ReadMapChecksum(MainForm->map->map_id);
                if (MainForm->map->checksum_high != map_checksum_high ||
                    MainForm->map->checksum_low != map_checksum_low || MainForm->map->file_size != map_file_size)
                {
                    MainForm->map->map_file_size = map_file_size;
                    MainForm->animation->menu->SetMapNeeded(1);
                }
                // Each file: two checksum words and its record count, 900 records to a part.
                int item_checksum_high = DecodeNumber(packet.SubString(18, 2));
                int item_checksum_low = DecodeNumber(packet.SubString(20, 2));
                int item_record_count = DecodeNumber(packet.SubString(22, 2));
                MainForm->item_values->ReadHeader();
                MainForm->item_values->unread_flag_at_1 = 0;
                if (MainForm->item_values->checksum_high != item_checksum_high ||
                    MainForm->item_values->checksum_low != item_checksum_low ||
                    MainForm->item_values->record_count != item_record_count)
                {
                    MainForm->item_values->received_part_count = 1;
                    MainForm->item_values->expected_part_count = item_record_count / PUB_RECORDS_PER_FILE + 1;
                    MainForm->item_values->is_downloading = 1;
                    MainForm->animation->menu->SetItemFileNeeded(1);
                    MainForm->item_values->DeleteFiles();
                    MainForm->item_values->is_loaded = 0;
                    MainForm->item_values->Clear();
                }
                int npc_checksum_high = DecodeNumber(packet.SubString(24, 2));
                int npc_checksum_low = DecodeNumber(packet.SubString(26, 2));
                int npc_record_count = DecodeNumber(packet.SubString(28, 2));
                MainForm->npc_values->ReadHeader();
                MainForm->npc_values->unread_flag_at_1 = 0;
                if (MainForm->npc_values->checksum_high != npc_checksum_high ||
                    MainForm->npc_values->checksum_low != npc_checksum_low ||
                    MainForm->npc_values->record_count != npc_record_count)
                {
                    MainForm->npc_values->received_part_count = 1;
                    MainForm->npc_values->expected_part_count = npc_record_count / PUB_RECORDS_PER_FILE + 1;
                    MainForm->npc_values->is_downloading = 1;
                    MainForm->animation->menu->SetNpcFileNeeded(1);
                    MainForm->npc_values->DeleteFiles();
                    MainForm->npc_values->is_loaded = 0;
                    MainForm->npc_values->Clear();
                }
                int skill_checksum_high = DecodeNumber(packet.SubString(30, 2));
                int skill_checksum_low = DecodeNumber(packet.SubString(32, 2));
                int skill_record_count = DecodeNumber(packet.SubString(34, 2));
                MainForm->skill_values->ReadHeader();
                MainForm->skill_values->unread_flag_at_1 = 0;
                if (MainForm->skill_values->checksum_high != skill_checksum_high ||
                    MainForm->skill_values->checksum_low != skill_checksum_low ||
                    MainForm->skill_values->record_count != skill_record_count)
                {
                    MainForm->skill_values->received_part_count = 1;
                    MainForm->skill_values->expected_part_count = skill_record_count / PUB_RECORDS_PER_FILE + 1;
                    MainForm->skill_values->is_downloading = 1;
                    MainForm->animation->menu->SetSkillFileNeeded(1);
                    MainForm->skill_values->DeleteFiles();
                    MainForm->skill_values->is_loaded = 0;
                    MainForm->skill_values->Clear();
                }
                int class_checksum_high = DecodeNumber(packet.SubString(36, 2));
                int class_checksum_low = DecodeNumber(packet.SubString(38, 2));
                int class_record_count = DecodeNumber(packet.SubString(40, 2));
                MainForm->class_values->ReadHeader();
                MainForm->class_values->unread_flag_at_1 = 0;
                if (MainForm->class_values->checksum_high != class_checksum_high ||
                    MainForm->class_values->checksum_low != class_checksum_low ||
                    MainForm->class_values->record_count != class_record_count)
                {
                    MainForm->class_values->received_part_count = 1;
                    MainForm->class_values->expected_part_count = skill_record_count / CLASS_RECORDS_PER_FILE + 1;
                    MainForm->class_values->is_downloading = 1;
                    MainForm->animation->menu->SetClassFileNeeded(1);
                    MainForm->class_values->DeleteFiles();
                    MainForm->class_values->is_loaded = 0;
                    MainForm->class_values->Clear();
                }
                ParseWelcomeCharacter(packet.SubString(42, packet.Length() - 41));
                MainForm->animation->StartReplyTimeout();
                MainForm->animation->menu->first_slot_login_hover = false;
                MainForm->animation->menu->second_slot_login_hover = false;
                MainForm->animation->menu->third_slot_login_hover = false;
                MainForm->animation->menu->first_slot_delete_hover = false;
                MainForm->animation->menu->second_slot_delete_hover = false;
                MainForm->animation->menu->third_slot_delete_hover = false;
                MainForm->animation->menu->change_password_button_hover = false;
                MainForm->animation->menu->new_character_button_hover = false;
                MainForm->animation->menu->is_form_open = false;
                MainForm->animation->menu->is_message_open = false;
                MainForm->animation->menu->is_loading_world = true;
                MainForm->animation->menu->loading_frame = 0;
                MainForm->animation->menu->loading_tip = random(9);
                MainForm->gfx->CreateGameGraphics();
                MainForm->gfx->LoadGameGraphics();
                MainForm->animation->interface_controller->gold_drop_warning_pending = true;
                MainForm->animation->social->LoadIgnoreList();
                MainForm->animation->social->LoadFriendList();
            }
            if (welcome_reply == WelcomeReply_EnterGame)
            {
                MainForm->registry->BuildCharacterKeys();
                MainForm->animation->StopReplyTimeout();
                MainForm->animation->menu->is_loading_world = false;
                MainForm->edits->Clear();
                MainForm->players->Clear();
                MainForm->effects->Clear();
                MainForm->map_items->ClearAll();
                MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
                MainForm->net_client->NextToken();
                String welcome_message = MainForm->net_client->NextToken();
                MainForm->animation->interface_controller->news->Clear();
                for (i = 0; i < 8; i++)
                    MainForm->animation->interface_controller->news->AddNewsItem(MainForm->net_client->NextToken());
                if (MainForm->animation->interface_controller->news->GetLineCount() > 0)
                {
                    MainForm->gfx->LoadNewsPanel();
                    MainForm->animation->SetSubState(GamePanel_News);
                }
                else
                {
                    MainForm->gfx->LoadChatPanel();
                    MainForm->animation->SetSubState(GamePanel_Chat);
                }
                ParseInventory(MainForm->net_client->NextToken());
                ParseSpellList(MainForm->net_client->NextToken());
                MainForm->animation->interface_controller->inventory->Clear();
                MainForm->animation->interface_controller->inventory->PlaceLoadedItems();
                MainForm->animation->interface_controller->spells->RestoreLayout();
                MainForm->animation->SetScreenMode(ScreenMode_Game);
                MainForm->animation->interface_controller->RequestFullRedraw();
                MainForm->edits->AddEdit(EditId_Chat, 3, 120, 309, 470, EDIT_LINE_HEIGHT);
                MainForm->edits->SetMaxLength(EditId_Chat, 128);
                MainForm->edits->SetBackgroundVisible(EditId_Chat, 0);
                MainForm->edits->SetBorderVisible(EditId_Chat, 0);
                MainForm->gfx->FreeMenuGraphics();
                MainForm->sfx->StopMusic();
                MainForm->map->LoadMap(MainForm->map->map_id);
                if (MainForm->map->IsMinimapDisabled())
                    MainForm->gfx->interface_graphics->minimap_visible = false;
                ParseNearbyEntities(MainForm->net_client->RemainingTokens());
                MainForm->animation->game->CenterViewOnPlayer(MainForm->net_client->GetPlayerId());
                if (MainForm->map->GetName() != "")
                    MainForm->animation->interface_controller->chat->AddSystemMessage(3, 0,
                        MainForm->strings->GetText(GameText_YouEntered) + " " + MainForm->map->GetName(), false);
                if (welcome_message.Length() > 0)
                    MainForm->animation->interface_controller->chat->AddLocalMessage(1, 2,
                        MainForm->strings->GetText(GameText_Server), welcome_message, false);
                MainForm->animation->interface_controller->chat->AddGlobalMessage(1, 2,
                    MainForm->strings->GetText(GameText_Server), MainForm->strings->GetText(GameText_BeginYourLineWith),
                    true, false);
                MainForm->animation->interface_controller->chat->AddGlobalMessage(1, 2,
                    MainForm->strings->GetText(GameText_Server), MainForm->strings->GetText(GameText_DoNotCurseHarass),
                    true, false);
                if (MainForm->animation->interface_controller->news->GetLineCount() > 0)
                {
                    MainForm->animation->interface_controller->news->StartScrollHint();
                    MainForm->animation->interface_controller->Invalidate();
                    MainForm->animation->interface_controller->news->MarkTextDirty();
                }
                else
                {
                    MainForm->animation->interface_controller->Invalidate();
                    MainForm->animation->interface_controller->chat->Invalidate();
                }
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                    MainForm->strings->GetDialogText(DialogText_WarningKeep, true));
                if (MainForm->animation->interface_controller->welcome_notice_pending)
                {
                    MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                        MainForm->strings->GetDialogText(DialogText_SafetyCommentOnceAdmins, false),
                        MainForm->strings->GetDialogText(DialogText_SafetyCommentOnceAdmins, true));
                    MainForm->animation->interface_controller->welcome_notice_pending = false;
                }
            }
            if (welcome_reply == WelcomeReply_RejectedDialog19)
            {
                MainForm->gfx->FreeGameGraphics();
                MainForm->animation->StopReplyTimeout();
                MainForm->animation->menu->is_loading_world = false;
                MainForm->edits->caret_enabled = false;
                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                    MainForm->strings->GetDialogText(DialogText_ServerBusy19, false),
                    MainForm->strings->GetDialogText(DialogText_ServerBusy19, true));
            }
            if (welcome_reply == WelcomeReply_RejectedDialog20)
            {
                MainForm->gfx->FreeGameGraphics();
                MainForm->animation->StopReplyTimeout();
                MainForm->animation->menu->is_loading_world = false;
                MainForm->edits->caret_enabled = false;
                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                    MainForm->strings->GetDialogText(DialogText_LoginRefusedCharacters, false),
                    MainForm->strings->GetDialogText(DialogText_LoginRefusedCharacters, true));
            }
        }
    }
    if (family == OP_LOGIN && action == AC_RESULT)
    {
        // The reply to a login:
        // 1, 2 = refused, with a message; the third refusal goes back to the main menu
        // 3 = accepted when the challenge answer matches; the character list follows
        // 4 to 6 = name and password are cleared and logging in is disabled
        int login_reply = DecodeNumber(packet.SubString(1, 2));
        if (login_reply == LoginReply_FailedDialog21)
        {
            MainForm->net_client->login_failure_count++;
            MainForm->edits->caret_enabled = false;
            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                MainForm->strings->GetDialogText(DialogText_LoginRefusedName, false),
                MainForm->strings->GetDialogText(DialogText_LoginRefusedName, true));
            MainForm->edits->SetText(2, "");
            MainForm->edits->FocusEdit(2);
            MainForm->animation->menu->login_enabled = true;
            if (MainForm->net_client->login_failure_count > 2)
            {
                MainForm->edits->Clear();
                MainForm->gfx->LoadMainMenuScreen(false);
                MainForm->animation->SetSubState(MenuScreen_Main);
            }
            if (MainForm->animation->GetSubState() == 8)
            {
                MainForm->animation->menu->password = "";
                MainForm->animation->menu->account_name = "";
                MainForm->edits->Clear();
                MainForm->net_client->login_failure_count = 0;
                MainForm->gfx->LoadLoginScreen();
                MainForm->animation->SetSubState(MenuScreen_GridAccountName);
                MainForm->edits->AddEdit(1, 2, LOGIN_EDIT_X, GRID_LOGIN_NAME_EDIT_Y, 161, EDIT_LINE_HEIGHT);
                MainForm->edits->SetCharFilter(1, 2);
                MainForm->edits->SetMaxLength(1, 16);
                MainForm->edits->FocusEdit(1);
            }
        }
        if (login_reply == LoginReply_FailedDialog22)
        {
            MainForm->net_client->login_failure_count++;
            MainForm->edits->caret_enabled = false;
            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                MainForm->strings->GetDialogText(DialogText_LoginRefusedPassword, false),
                MainForm->strings->GetDialogText(DialogText_LoginRefusedPassword, true));
            MainForm->edits->SetText(2, "");
            MainForm->edits->FocusEdit(2);
            MainForm->animation->menu->login_enabled = true;
            if (MainForm->net_client->login_failure_count > 2)
            {
                MainForm->edits->Clear();
                MainForm->gfx->LoadMainMenuScreen(false);
                MainForm->animation->SetSubState(MenuScreen_Main);
            }
            if (MainForm->animation->GetSubState() == 8)
                MainForm->animation->menu->password = "";
        }
        if (login_reply == LoginReply_Ok)
        {
            int login_expected_response = MainForm->net_client->server_challenge_hash;
            int login_challenge = MainForm->net_client->challenge;
            if (MainForm->net_client->HashChallenge(login_challenge) == login_expected_response)
            {
                MainForm->edits->Clear();
                MainForm->players->Clear();
                MainForm->effects->Clear();
                MainForm->players->AddCharacterSlots();
                MainForm->gfx->LoadCharacterSelectScreen();
                MainForm->animation->SetSubState(MenuScreen_CharacterSelect);
                MainForm->sfx->PlaySfx(5);
                MainForm->animation->menu->login_enabled = true;
                MainForm->animation->menu->delete_armed_slot = 0;
                if (packet.Length() >= 3)
                    ParseCharacterList(packet.SubString(3, packet.Length()));
            }
        }
        if (login_reply == LoginReply_Banned)
        {
            MainForm->net_client->is_banned = true;
            MainForm->edits->SetText(1, "");
            MainForm->edits->SetText(2, "");
            MainForm->edits->FocusEdit(2);
            MainForm->animation->menu->login_enabled = false;
        }
        if (login_reply == LoginReply_RefusedDialog23)
        {
            MainForm->edits->caret_enabled = false;
            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                MainForm->strings->GetDialogText(DialogText_LoginRefusedAccountLogged, false),
                MainForm->strings->GetDialogText(DialogText_LoginRefusedAccountLogged, true));
            MainForm->edits->SetText(1, "");
            MainForm->edits->SetText(2, "");
            MainForm->edits->FocusEdit(2);
            MainForm->animation->menu->login_enabled = false;
        }
        if (login_reply == LoginReply_RetryLater)
        {
            MainForm->net_client->login_retry_pending = true;
            MainForm->edits->SetText(1, "");
            MainForm->edits->SetText(2, "");
            MainForm->edits->FocusEdit(2);
            MainForm->animation->menu->login_enabled = false;
        }
    }
    if (family == OP_ACCOUNT && action == AC_RESULT)
    {
        int account_reply = DecodeNumber(packet.SubString(1, 2));
        if (account_reply == AccountReply_NameDialog24)
        {
            MainForm->edits->caret_enabled = false;
            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                MainForm->strings->GetDialogText(DialogText_AlreadyExistAccount, false),
                MainForm->strings->GetDialogText(DialogText_AlreadyExistAccount, true));
            MainForm->animation->menu->create_account_enabled = true;
            MainForm->edits->SetText(1, "");
            MainForm->edits->FocusEdit(1);
        }
        if (account_reply == AccountReply_NameDialog25)
        {
            MainForm->edits->caret_enabled = false;
            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                MainForm->strings->GetDialogText(DialogText_NotApprovedAccount, false),
                MainForm->strings->GetDialogText(DialogText_NotApprovedAccount, true));
            MainForm->animation->menu->create_account_enabled = true;
            MainForm->edits->SetText(1, "");
            MainForm->edits->FocusEdit(1);
        }
        if (account_reply == AccountReply_Created)
        {
            MainForm->edits->Clear();
            MainForm->gfx->LoadMainMenuScreen(false);
            MainForm->animation->SetSubState(MenuScreen_Main);
            MainForm->edits->caret_enabled = false;
            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                MainForm->strings->GetDialogText(DialogText_Welcome, false),
                MainForm->strings->GetDialogText(DialogText_Welcome, true));
        }
        if (account_reply == AccountReply_FieldsDialog27)
        {
            MainForm->edits->SetText(1, "");
            MainForm->edits->SetText(5, "");
            MainForm->edits->FocusEdit(1);
            MainForm->animation->menu->change_password_enabled = true;
            MainForm->edits->caret_enabled = false;
            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                MainForm->strings->GetDialogText(DialogText_RequestDeniedAccount, false),
                MainForm->strings->GetDialogText(DialogText_RequestDeniedAccount, true));
        }
        if (account_reply == AccountReply_ResetDialog28)
        {
            MainForm->edits->Clear();
            MainForm->animation->menu->is_form_open = false;
            MainForm->edits->caret_enabled = false;
            MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                MainForm->strings->GetDialogText(DialogText_PasswordChanged, false),
                MainForm->strings->GetDialogText(DialogText_PasswordChanged, true));
        }
        if (account_reply == AccountReply_Disconnect)
        {
            MainForm->net_client->account_reply_disconnect = true;
            MainForm->edits->Clear();
            MainForm->edits->caret_enabled = false;
        }
        // 10 and up = accepted: the reply is the session id.
        if (account_reply >= AccountReply_SessionBase)
        {
            int account_expected_response = MainForm->net_client->server_challenge_hash;
            int account_challenge = MainForm->net_client->challenge;
            if (MainForm->net_client->HashChallenge(account_challenge) == account_expected_response)
            {
                MainForm->net_client->SetSessionId(account_reply);
                if (packet.Length() > 2)
                {
                    MainForm->net_client->SetSequenceStart(DecodeNumber(packet.SubString(3, 1)));
                    MainForm->animation->StartAccountProgress();
                    MainForm->edits->caret_enabled = false;
                    MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Progress,
                        MainForm->strings->GetDialogText(DialogText_AccountAccepted, false),
                        MainForm->strings->GetDialogText(DialogText_AccountAccepted, true));
                }
            }
        }
    }
    if (family == OP_BOOK && action == AC_RESULT && !MainForm->animation->book->is_visible)
        ParseBook(packet);
    if (family == OP_PAPERDOLL)
    {
        if (action == AC_RESULT && !MainForm->animation->paperdoll->is_visible)
            ParsePaperdoll(packet);
        if (action == AC_SYN1)
            MainForm->animation->interface_controller->status->class_id = DecodeNumber(packet.SubString(1, 1));
        if (action == AC_UPDATE)
        {
            ApplyAppearanceChange(packet);
            // After the look change: the item, the amount left, the side of a paired slot, the stats.
            int equipped_item_id = DecodeNumber(packet.SubString(15, 2));
            int equipped_item_remaining = DecodeNumber(packet.SubString(17, 3));
            int equip_sub_slot = DecodeNumber(packet.SubString(20, 1));
            MainForm->animation->interface_controller->status->SetMaxHealth(DecodeNumber(packet.SubString(21, 2)));
            MainForm->animation->interface_controller->status->SetMaxMana(DecodeNumber(packet.SubString(23, 2)));
            MainForm->animation->interface_controller->status->stat_str = DecodeNumber(packet.SubString(25, 2));
            MainForm->animation->interface_controller->status->stat_int = DecodeNumber(packet.SubString(27, 2));
            MainForm->animation->interface_controller->status->stat_wis = DecodeNumber(packet.SubString(29, 2));
            MainForm->animation->interface_controller->status->stat_agi = DecodeNumber(packet.SubString(31, 2));
            MainForm->animation->interface_controller->status->stat_con = DecodeNumber(packet.SubString(33, 2));
            MainForm->animation->interface_controller->status->stat_cha = DecodeNumber(packet.SubString(35, 2));
            MainForm->animation->interface_controller->status->label_413_from = DecodeNumber(packet.SubString(37, 2));
            MainForm->animation->interface_controller->status->label_413_to = DecodeNumber(packet.SubString(39, 2));
            MainForm->animation->interface_controller->status->label_414_value = DecodeNumber(packet.SubString(41, 2));
            MainForm->animation->interface_controller->status->label_416_value = DecodeNumber(packet.SubString(43, 2));
            MainForm->animation->interface_controller->status->label_415_value = DecodeNumber(packet.SubString(45, 2));
            MainForm->animation->interface_controller->status->MarkDirty();
            int equipped_item_type = MainForm->item_values->GetType(equipped_item_id);
            if (equipped_item_type == EifItemType_Weapon)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetWeaponSlot(equipped_item_id);
                MainForm->animation->paperdoll->equipped_weapon = equipped_item_id;
            }
            if (equipped_item_type == EifItemType_Shield)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetShieldSlot(equipped_item_id);
                MainForm->animation->paperdoll->equipped_shield = equipped_item_id;
            }
            if (equipped_item_type == EifItemType_Armor)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetArmorSlot(equipped_item_id);
                MainForm->animation->paperdoll->equipped_armor = equipped_item_id;
            }
            if (equipped_item_type == EifItemType_Hat)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetHeadSlot(equipped_item_id);
                MainForm->animation->paperdoll->equipped_head = equipped_item_id;
            }
            if (equipped_item_type == EifItemType_Boots)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetBootsSlot(equipped_item_id);
                MainForm->animation->paperdoll->equipped_boots = equipped_item_id;
            }
            if (equipped_item_type == EifItemType_Gloves)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetGlovesSlot(equipped_item_id);
                MainForm->animation->paperdoll->equipped_gloves = equipped_item_id;
            }
            if (equipped_item_type == EifItemType_Accessory)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetType16Slot(equipped_item_id);
                MainForm->animation->paperdoll->equipped_type16 = equipped_item_id;
            }
            if (equipped_item_type == EifItemType_Belt)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetBeltSlot(equipped_item_id);
                MainForm->animation->paperdoll->equipped_belt = equipped_item_id;
            }
            if (equipped_item_type == EifItemType_Necklace)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetNecklaceSlot(equipped_item_id);
                MainForm->animation->paperdoll->equipped_necklace = equipped_item_id;
            }
            if (equipped_item_type == EifItemType_Ring)
            {
                if (!equip_sub_slot)
                {
                    if (MainForm->animation->paperdoll->is_own_character)
                        MainForm->animation->paperdoll->SetType19LeftSlot(equipped_item_id);
                    MainForm->animation->paperdoll->equipped_type19_left = equipped_item_id;
                }
                if (equip_sub_slot == 1)
                {
                    if (MainForm->animation->paperdoll->is_own_character)
                        MainForm->animation->paperdoll->SetType19RightSlot(equipped_item_id);
                    MainForm->animation->paperdoll->equipped_type19_right = equipped_item_id;
                }
            }
            if (equipped_item_type == EifItemType_Armlet)
            {
                if (!equip_sub_slot)
                {
                    if (MainForm->animation->paperdoll->is_own_character)
                        MainForm->animation->paperdoll->SetType20LeftSlot(equipped_item_id);
                    MainForm->animation->paperdoll->equipped_type20_left = equipped_item_id;
                }
                if (equip_sub_slot == 1)
                {
                    if (MainForm->animation->paperdoll->is_own_character)
                        MainForm->animation->paperdoll->SetType20RightSlot(equipped_item_id);
                    MainForm->animation->paperdoll->equipped_type20_right = equipped_item_id;
                }
            }
            if (equipped_item_type == EifItemType_Bracer)
            {
                if (!equip_sub_slot)
                {
                    if (MainForm->animation->paperdoll->is_own_character)
                        MainForm->animation->paperdoll->SetType21LeftSlot(equipped_item_id);
                    MainForm->animation->paperdoll->equipped_type21_left = equipped_item_id;
                }
                if (equip_sub_slot == 1)
                {
                    if (MainForm->animation->paperdoll->is_own_character)
                        MainForm->animation->paperdoll->SetType21RightSlot(equipped_item_id);
                    MainForm->animation->paperdoll->equipped_type21_right = equipped_item_id;
                }
            }
            MainForm->animation->interface_controller->inventory->RestoreLayoutBackup();
            MainForm->animation->interface_controller->inventory->SetItemAmount(equipped_item_id,
                equipped_item_remaining);
            MainForm->animation->interface_controller->inventory->MarkItemsDirty();
            MainForm->redraw_needed = true;
        }
        if (action == AC_DELETE)
        {
            ApplyAppearanceChange(packet);
            int unequipped_item_id = DecodeNumber(packet.SubString(15, 2));
            int unequipped_item_type = MainForm->item_values->GetType(unequipped_item_id);
            int unequip_sub_slot = DecodeNumber(packet.SubString(17, 1));
            MainForm->animation->interface_controller->status->SetMaxHealth(DecodeNumber(packet.SubString(18, 2)));
            MainForm->animation->interface_controller->status->SetMaxMana(DecodeNumber(packet.SubString(20, 2)));
            MainForm->animation->interface_controller->status->stat_str = DecodeNumber(packet.SubString(22, 2));
            MainForm->animation->interface_controller->status->stat_int = DecodeNumber(packet.SubString(24, 2));
            MainForm->animation->interface_controller->status->stat_wis = DecodeNumber(packet.SubString(26, 2));
            MainForm->animation->interface_controller->status->stat_agi = DecodeNumber(packet.SubString(28, 2));
            MainForm->animation->interface_controller->status->stat_con = DecodeNumber(packet.SubString(30, 2));
            MainForm->animation->interface_controller->status->stat_cha = DecodeNumber(packet.SubString(32, 2));
            MainForm->animation->interface_controller->status->label_413_from = DecodeNumber(packet.SubString(34, 2));
            MainForm->animation->interface_controller->status->label_413_to = DecodeNumber(packet.SubString(36, 2));
            MainForm->animation->interface_controller->status->label_414_value = DecodeNumber(packet.SubString(38, 2));
            MainForm->animation->interface_controller->status->label_416_value = DecodeNumber(packet.SubString(40, 2));
            MainForm->animation->interface_controller->status->label_415_value = DecodeNumber(packet.SubString(42, 2));
            MainForm->animation->interface_controller->status->MarkDirty();
            if (unequipped_item_type == EifItemType_Weapon)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetWeaponSlot(0);
                MainForm->animation->paperdoll->equipped_weapon = 0;
            }
            if (unequipped_item_type == EifItemType_Shield)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetShieldSlot(0);
                MainForm->animation->paperdoll->equipped_shield = 0;
            }
            if (unequipped_item_type == EifItemType_Armor)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetArmorSlot(0);
                MainForm->animation->paperdoll->equipped_armor = 0;
            }
            if (unequipped_item_type == EifItemType_Hat)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetHeadSlot(0);
                MainForm->animation->paperdoll->equipped_head = 0;
            }
            if (unequipped_item_type == EifItemType_Boots)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetBootsSlot(0);
                MainForm->animation->paperdoll->equipped_boots = 0;
            }
            if (unequipped_item_type == EifItemType_Gloves)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetGlovesSlot(0);
                MainForm->animation->paperdoll->equipped_gloves = 0;
            }
            if (unequipped_item_type == EifItemType_Accessory)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetType16Slot(0);
                MainForm->animation->paperdoll->equipped_type16 = 0;
            }
            if (unequipped_item_type == EifItemType_Belt)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetBeltSlot(0);
                MainForm->animation->paperdoll->equipped_belt = 0;
            }
            if (unequipped_item_type == EifItemType_Necklace)
            {
                if (MainForm->animation->paperdoll->is_own_character)
                    MainForm->animation->paperdoll->SetNecklaceSlot(0);
                MainForm->animation->paperdoll->equipped_necklace = 0;
            }
            if (unequipped_item_type == EifItemType_Ring)
            {
                if (!unequip_sub_slot)
                {
                    if (MainForm->animation->paperdoll->is_own_character)
                        MainForm->animation->paperdoll->SetType19LeftSlot(0);
                    MainForm->animation->paperdoll->equipped_type19_left = 0;
                }
                if (unequip_sub_slot == 1)
                {
                    if (MainForm->animation->paperdoll->is_own_character)
                        MainForm->animation->paperdoll->SetType19RightSlot(0);
                    MainForm->animation->paperdoll->equipped_type19_right = 0;
                }
            }
            if (unequipped_item_type == EifItemType_Armlet)
            {
                if (!unequip_sub_slot)
                {
                    if (MainForm->animation->paperdoll->is_own_character)
                        MainForm->animation->paperdoll->SetType20LeftSlot(0);
                    MainForm->animation->paperdoll->equipped_type20_left = 0;
                }
                if (unequip_sub_slot == 1)
                {
                    if (MainForm->animation->paperdoll->is_own_character)
                        MainForm->animation->paperdoll->SetType20RightSlot(0);
                    MainForm->animation->paperdoll->equipped_type20_right = 0;
                }
            }
            if (unequipped_item_type == EifItemType_Bracer)
            {
                if (!unequip_sub_slot)
                {
                    if (MainForm->animation->paperdoll->is_own_character)
                        MainForm->animation->paperdoll->SetType21LeftSlot(0);
                    MainForm->animation->paperdoll->equipped_type21_left = 0;
                }
                if (unequip_sub_slot == 1)
                {
                    if (MainForm->animation->paperdoll->is_own_character)
                        MainForm->animation->paperdoll->SetType21RightSlot(0);
                    MainForm->animation->paperdoll->equipped_type21_right = 0;
                }
            }
            MainForm->animation->interface_controller->inventory->RestoreLayoutBackup();
            MainForm->animation->interface_controller->inventory->AddItem(unequipped_item_id, 1);
            MainForm->animation->interface_controller->inventory->MarkItemsDirty();
            MainForm->animation->interface_controller->inventory->dragging_from_paperdoll = false;
            MainForm->animation->interface_controller->inventory->CancelDrag();
            MainForm->redraw_needed = true;
        }
    }
    if (family == OP_ITEM)
    {
        if (action == AC_ADD)
        {
            int shown_item_id = DecodeNumber(packet.SubString(1, 2));
            int shown_item_uid = DecodeNumber(packet.SubString(3, 2));
            int shown_item_amount = DecodeNumber(packet.SubString(5, 3));
            int shown_item_x = DecodeNumber(packet.SubString(8, 1));
            int shown_item_y = DecodeNumber(packet.SubString(9, 1));
            MainForm->map_items->AddProtectedItem(shown_item_uid, shown_item_id, shown_item_x, shown_item_y,
                shown_item_amount, 11110000, 10);
            MainForm->map_items->SortByPosition();
            MainForm->map_items->SortByPosition();
            MainForm->redraw_needed = true;
        }
        if (action == AC_DELETE)
        {
            int removed_item_uid = DecodeNumber(packet.SubString(1, 2));
            MainForm->map_items->RemoveItem(removed_item_uid);
        }
        if (action == AC_GIVE)
        {
            int received_item_id = DecodeNumber(packet.SubString(1, 2));
            int received_amount = DecodeNumber(packet.SubString(3, 3));
            int receive_weight = DecodeNumber(packet.SubString(6, 1));
            MainForm->animation->interface_controller->inventory->AddItem(received_item_id, received_amount);
            MainForm->animation->interface_controller->status->weight = receive_weight;
        }
        if (action == AC_REMOVE)
        {
            int updated_item_id = DecodeNumber(packet.SubString(1, 2));
            int updated_amount = DecodeNumber(packet.SubString(3, 4));
            int update_weight = DecodeNumber(packet.SubString(7, 1));
            MainForm->animation->interface_controller->inventory->SetItemAmount(updated_item_id, updated_amount);
            MainForm->animation->interface_controller->status->weight = update_weight;
            MainForm->animation->interface_controller->status->MarkDirty();
        }
        if (action == AC_PICKUP)
        {
            int picked_item_uid = DecodeNumber(packet.SubString(1, 2));
            int picked_item_id = DecodeNumber(packet.SubString(3, 2));
            int picked_amount = DecodeNumber(packet.SubString(5, 3));
            int pickup_weight = DecodeNumber(packet.SubString(8, 1));
            int pickup_max_weight = DecodeNumber(packet.SubString(9, 1));
            if (picked_item_id == 1)
                MainForm->animation->interface_controller->status->gold = picked_amount +
                    MainForm->animation->interface_controller->inventory->GetItemAmount(picked_item_id);
            String picked_item_name = MainForm->item_values->GetName(picked_item_id);
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                MainForm->strings->GetText(GameText_YouPickedUp) + " " + IntToStr(picked_amount) + " " +
                picked_item_name);
            MainForm->animation->interface_controller->chat->AddSystemMessage(17, 0,
                MainForm->strings->GetText(GameText_YouPickedUp) + " " + IntToStr(picked_amount) + " " +
                picked_item_name, true);
            MainForm->animation->interface_controller->inventory->AddItem(picked_item_id, picked_amount);
            MainForm->map_items->RemoveItem(picked_item_uid);
            MainForm->animation->interface_controller->status->weight = pickup_weight;
            MainForm->animation->interface_controller->status->max_weight = pickup_max_weight;
            MainForm->animation->interface_controller->status->MarkDirty();
            MainForm->redraw_needed = true;
        }
        if (action == AC_DROP)
        {
            int dropped_item_id = DecodeNumber(packet.SubString(1, 2));
            int dropped_amount = DecodeNumber(packet.SubString(3, 3));
            int drop_remaining_amount = DecodeNumber(packet.SubString(6, 4));
            int dropped_item_uid = DecodeNumber(packet.SubString(10, 2));
            int drop_x = DecodeNumber(packet.SubString(12, 1));
            int drop_y = DecodeNumber(packet.SubString(13, 1));
            int drop_weight = DecodeNumber(packet.SubString(14, 1));
            int drop_max_weight = DecodeNumber(packet.SubString(15, 1));
            if (dropped_item_id == 1)
                MainForm->animation->interface_controller->status->gold = drop_remaining_amount;
            int amount_before_drop = MainForm->animation->interface_controller->inventory->GetItemAmount(dropped_item_id);
            if (amount_before_drop > drop_remaining_amount)
            {
                String dropped_item_name = MainForm->item_values->GetName(dropped_item_id);
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouDropped) + " " +
                    IntToStr(amount_before_drop - drop_remaining_amount) + " " + dropped_item_name);
                MainForm->animation->interface_controller->chat->AddSystemMessage(16, 0,
                    MainForm->strings->GetText(GameText_YouDropped) + " " +
                    IntToStr(amount_before_drop - drop_remaining_amount) + " " + dropped_item_name, true);
            }
            MainForm->animation->interface_controller->inventory->SetItemAmount(dropped_item_id, drop_remaining_amount);
            MainForm->map_items->AddItem(dropped_item_uid, dropped_item_id, drop_x, drop_y, dropped_amount);
            MainForm->map_items->SortByPosition();
            MainForm->animation->interface_controller->inventory->PlaceRemainingItems();
            MainForm->animation->interface_controller->status->weight = drop_weight;
            MainForm->animation->interface_controller->status->max_weight = drop_max_weight;
            MainForm->animation->interface_controller->status->MarkDirty();
            MainForm->redraw_needed = true;
        }
        if (action == AC_JUNK)
        {
            int junked_item_id = DecodeNumber(packet.SubString(1, 2));
            int junked_amount = DecodeNumber(packet.SubString(3, 3));
            int junk_remaining_amount = DecodeNumber(packet.SubString(6, 4));
            int junk_weight = DecodeNumber(packet.SubString(10, 1));
            int junk_max_weight = DecodeNumber(packet.SubString(11, 1));
            if (junked_item_id == 1)
                MainForm->animation->interface_controller->status->gold = junk_remaining_amount;
            int amount_before_junk = MainForm->animation->interface_controller->inventory->GetItemAmount(junked_item_id);
            if (amount_before_junk > junk_remaining_amount)
            {
                String junked_item_name = MainForm->item_values->GetName(junked_item_id);
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_YouJunked) + " " +
                    IntToStr(amount_before_junk - junk_remaining_amount) + " " + junked_item_name);
                MainForm->animation->interface_controller->chat->AddSystemMessage(16, 0,
                    MainForm->strings->GetText(GameText_YouJunked) + " " +
                    IntToStr(amount_before_junk - junk_remaining_amount) + " " + junked_item_name, true);
            }
            MainForm->animation->interface_controller->inventory->SetItemAmount(junked_item_id, junk_remaining_amount);
            MainForm->animation->interface_controller->status->weight = junk_weight;
            MainForm->animation->interface_controller->status->max_weight = junk_max_weight;
            MainForm->animation->interface_controller->status->MarkDirty();
        }
        if (action == AC_UPDATE)
        {
            int emptied_item_id = DecodeNumber(packet.SubString(1, 2));
            MainForm->animation->interface_controller->inventory->SetItemAmount(emptied_item_id, 0);
        }
        if (action == AC_RESULT)
        {
            // What the used item does:
            // 3 = heals, 25 = new stats and the worn items of special 5 come off, 24 = hair color,
            // 23 = an effect, 22 = drunk for the item's spec value, 6 = experience, 4 = a warp
            int use_effect = DecodeNumber(packet.SubString(1, 1));
            if (use_effect == EifItemType_Heal)
            {
                int heal_item_id = DecodeNumber(packet.SubString(2, 2));
                int heal_item_amount = DecodeNumber(packet.SubString(4, 4));
                int heal_weight = DecodeNumber(packet.SubString(8, 1));
                int heal_max_weight = DecodeNumber(packet.SubString(9, 1));
                int heal_amount = DecodeNumber(packet.SubString(10, 4));
                int healed_health = DecodeNumber(packet.SubString(14, 2));
                int healed_mana = DecodeNumber(packet.SubString(16, 2));
                MainForm->animation->interface_controller->inventory->SetItemAmount(heal_item_id, heal_item_amount);
                MainForm->animation->interface_controller->status->weight = heal_weight;
                MainForm->animation->interface_controller->status->max_weight = heal_max_weight;
                MainForm->animation->interface_controller->status->SetHealth(healed_health);
                MainForm->animation->interface_controller->status->SetMana(healed_mana);
                if (heal_amount > 0)
                {
                    int health_percent = 100 * MainForm->animation->interface_controller->status->health;
                    health_percent = health_percent / MainForm->animation->interface_controller->status->max_health;
                    MainForm->players->ShowDamage(MainForm->net_client->GetPlayerId(), heal_amount, 1);
                    MainForm->players->ShowHealthBar(MainForm->net_client->GetPlayerId(), health_percent, 8);
                }
            }
            if (use_effect == EifItemType_CureCurse)
            {
                int cure_item_id = DecodeNumber(packet.SubString(2, 2));
                int cure_item_amount = DecodeNumber(packet.SubString(4, 4));
                int cure_weight = DecodeNumber(packet.SubString(8, 1));
                int cure_max_weight = DecodeNumber(packet.SubString(9, 1));
                MainForm->animation->interface_controller->inventory->SetItemAmount(cure_item_id, cure_item_amount);
                MainForm->animation->interface_controller->status->weight = cure_weight;
                MainForm->animation->interface_controller->status->max_weight = cure_max_weight;
                MainForm->animation->interface_controller->status->SetMaxHealth(DecodeNumber(packet.SubString(10, 2)));
                MainForm->animation->interface_controller->status->SetMaxMana(DecodeNumber(packet.SubString(12, 2)));
                MainForm->animation->interface_controller->status->stat_str = DecodeNumber(packet.SubString(14, 2));
                MainForm->animation->interface_controller->status->stat_int = DecodeNumber(packet.SubString(16, 2));
                MainForm->animation->interface_controller->status->stat_wis = DecodeNumber(packet.SubString(18, 2));
                MainForm->animation->interface_controller->status->stat_agi = DecodeNumber(packet.SubString(20, 2));
                MainForm->animation->interface_controller->status->stat_con = DecodeNumber(packet.SubString(22, 2));
                MainForm->animation->interface_controller->status->stat_cha = DecodeNumber(packet.SubString(24, 2));
                MainForm->animation->interface_controller->status->label_413_from = DecodeNumber(packet.SubString(26,
                    2));
                MainForm->animation->interface_controller->status->label_413_to = DecodeNumber(packet.SubString(28, 2));
                MainForm->animation->interface_controller->status->label_414_value = DecodeNumber(packet.SubString(30,
                    2));
                MainForm->animation->interface_controller->status->label_416_value = DecodeNumber(packet.SubString(32,
                    2));
                MainForm->animation->interface_controller->status->label_415_value = DecodeNumber(packet.SubString(34,
                    2));
                MainForm->animation->interface_controller->status->MarkDirty();
                if (MainForm->animation->paperdoll->RemoveCursedItems())
                {
                    int own_player_id = MainForm->net_client->GetPlayerId();
                    if (!MainForm->animation->paperdoll->weapon_item_id)
                        MainForm->players->SetWeapon(own_player_id, 0);
                    if (!MainForm->animation->paperdoll->shield_item_id)
                        MainForm->players->SetShield(own_player_id, 0);
                    if (!MainForm->animation->paperdoll->armor_item_id)
                        MainForm->players->SetArmor(own_player_id, 0);
                    if (!MainForm->animation->paperdoll->boots_item_id)
                        MainForm->players->SetBoots(own_player_id, 0);
                    if (!MainForm->animation->paperdoll->head_item_id)
                        MainForm->players->SetHat(own_player_id, 0);
                    MainForm->players->ReloadGraphics(own_player_id);
                }
            }
            if (use_effect == EifItemType_HairDye)
            {
                int dye_item_id = DecodeNumber(packet.SubString(2, 2));
                int dye_item_amount = DecodeNumber(packet.SubString(4, 4));
                int dye_weight = DecodeNumber(packet.SubString(8, 1));
                int dye_max_weight = DecodeNumber(packet.SubString(9, 1));
                int hair_color = DecodeNumber(packet.SubString(10, 1));
                MainForm->animation->interface_controller->inventory->SetItemAmount(dye_item_id, dye_item_amount);
                MainForm->animation->interface_controller->status->weight = dye_weight;
                MainForm->animation->interface_controller->status->max_weight = dye_max_weight;
                MainForm->players->SetHairColor(MainForm->net_client->GetPlayerId(), hair_color);
                MainForm->players->ReloadGraphics(MainForm->net_client->GetPlayerId());
            }
            if (use_effect == EifItemType_EffectPotion)
            {
                int effect_item_id = DecodeNumber(packet.SubString(2, 2));
                int effect_item_amount = DecodeNumber(packet.SubString(4, 4));
                int effect_weight = DecodeNumber(packet.SubString(8, 1));
                int effect_max_weight = DecodeNumber(packet.SubString(9, 1));
                int effect_id = DecodeNumber(packet.SubString(10, 2));
                MainForm->animation->interface_controller->inventory->SetItemAmount(effect_item_id, effect_item_amount);
                MainForm->animation->interface_controller->status->weight = effect_weight;
                MainForm->animation->interface_controller->status->max_weight = effect_max_weight;
                MainForm->players->StartEffect(MainForm->net_client->GetPlayerId(), effect_id);
            }
            if (use_effect == EifItemType_Alcohol)
            {
                int drink_item_id = DecodeNumber(packet.SubString(2, 2));
                int drink_item_amount = DecodeNumber(packet.SubString(4, 4));
                int drink_weight = DecodeNumber(packet.SubString(8, 1));
                int drink_max_weight = DecodeNumber(packet.SubString(9, 1));
                int drunk_time = MainForm->item_values->GetSpecValue(drink_item_id);
                MainForm->animation->interface_controller->chat->SetDrunkTime(drunk_time);
                MainForm->animation->interface_controller->inventory->SetItemAmount(drink_item_id, drink_item_amount);
                MainForm->animation->interface_controller->status->weight = drink_weight;
                MainForm->animation->interface_controller->status->max_weight = drink_max_weight;
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                    MainForm->strings->GetText(GameText_YouFeelDrunk));
                MainForm->animation->interface_controller->inventory->SuppressHoverMessage();
            }
            if (use_effect == EifItemType_ExpReward)
            {
                int reward_item_id = DecodeNumber(packet.SubString(2, 2));
                int reward_item_amount = DecodeNumber(packet.SubString(4, 4));
                int reward_weight = DecodeNumber(packet.SubString(8, 1));
                int reward_max_weight = DecodeNumber(packet.SubString(9, 1));
                int new_experience = DecodeNumber(packet.SubString(10, 4));
                int new_level = DecodeNumber(packet.SubString(14, 1));
                int reward_stat_points = DecodeNumber(packet.SubString(15, 2));
                int reward_skill_points = DecodeNumber(packet.SubString(17, 2));
                int new_max_health = DecodeNumber(packet.SubString(19, 2));
                int new_max_mana = DecodeNumber(packet.SubString(21, 2));
                int new_max_stamina = DecodeNumber(packet.SubString(23, 2));
                if (new_level > 0)
                {
                    MainForm->animation->interface_controller->status->SetLevel(new_level);
                    MainForm->players->StartLevelUpEffect(MainForm->net_client->GetPlayerId());
                }
                if (MainForm->animation->interface_controller->status->experience < new_experience)
                {
                    int experience_gained = new_experience -
                        MainForm->animation->interface_controller->status->experience;
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouGained) + " " + IntToStr(experience_gained) + " EXP");
                    MainForm->animation->interface_controller->chat->AddSystemMessage(5, 0,
                        MainForm->strings->GetText(GameText_YouGained) + " " + IntToStr(experience_gained) + " EXP",
                        true);
                    MainForm->animation->interface_controller->inventory->SuppressHoverMessage();
                }
                MainForm->animation->interface_controller->status->stat_points = reward_stat_points;
                MainForm->animation->interface_controller->status->skill_points = reward_skill_points;
                MainForm->animation->interface_controller->status->SetMaxHealth(new_max_health);
                MainForm->animation->interface_controller->status->SetMaxMana(new_max_mana);
                MainForm->animation->interface_controller->status->SetMaxStamina(new_max_stamina);
                MainForm->animation->interface_controller->status->SetExperience(new_experience);
                MainForm->animation->interface_controller->status->weight = reward_weight;
                MainForm->animation->interface_controller->status->max_weight = reward_max_weight;
                MainForm->animation->interface_controller->status->MarkDirty();
                MainForm->animation->interface_controller->inventory->SetItemAmount(reward_item_id, reward_item_amount);
            }
            if (use_effect == EifItemType_Teleport)
            {
                int warp_item_id = DecodeNumber(packet.SubString(2, 2));
                int warp_item_amount = DecodeNumber(packet.SubString(4, 4));
                int warp_weight = DecodeNumber(packet.SubString(8, 1));
                int warp_max_weight = DecodeNumber(packet.SubString(9, 1));
                MainForm->animation->interface_controller->status->weight = warp_weight;
                MainForm->animation->interface_controller->status->max_weight = warp_max_weight;
                MainForm->animation->interface_controller->status->MarkDirty();
                MainForm->animation->interface_controller->inventory->SetItemAmount(warp_item_id, warp_item_amount);
                MainForm->map->map_data = packet.SubString(10, packet.Length() - 9);
                MainForm->map->StartWarpTimeout();
                MainForm->map->path_finder->CancelPath();
                if (!MainForm->map->IsScrolling())
                    MainForm->map->AcceptPendingWarp();
            }
        }
        if (action == AC_HIT)
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                MainForm->strings->GetText(GameText_ThisItemIsProtected));
        if (action == AC_CONFIRM)
            MainForm->players->StartLevelUpEffect(DecodeNumber(packet.SubString(1, 2)));
    }
    if (family == OP_CHARACTER)
    {
        if (action == AC_SET)
        {
            if (packet.Length() >= 4)
            {
                int delete_session_id = DecodeNumber(packet.SubString(1, 2));
                int delete_character_id = DecodeNumber(packet.SubString(3, 4));
                MainForm->net_client->SetSessionId(delete_session_id);
                MainForm->animation->menu->is_form_open = false;
                MainForm->animation->menu->delete_character_id = delete_character_id;
                MainForm->edits->caret_enabled = false;
                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Confirm,
                    MainForm->strings->GetDialogText(DialogText_DeleteCharacterSure, false),
                    "Character '" + MainForm->players->GetNameByCharacterId(delete_character_id) + "' " +
                    MainForm->strings->GetDialogText(DialogText_DeleteCharacterSure, true));
            }
        }
        if (action == AC_RESULT)
        {
            int create_character_reply = DecodeNumber(packet.SubString(1, 2));
            if (create_character_reply == CharacterReply_RetryDialog31)
            {
                MainForm->edits->caret_enabled = false;
                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                    MainForm->strings->GetDialogText(DialogText_AlreadyExistCharacter, false),
                    MainForm->strings->GetDialogText(DialogText_AlreadyExistCharacter, true));
                MainForm->animation->menu->create_character_enabled = true;
                MainForm->edits->SetText(12, "");
                MainForm->edits->FocusEdit(12);
            }
            if (create_character_reply == CharacterReply_ClosedDialog32 ||
                create_character_reply == CharacterReply_Unknown3)
            {
                MainForm->edits->Clear();
                MainForm->players->RemovePreviewPlayer();
                MainForm->animation->menu->is_form_open = false;
                MainForm->edits->caret_enabled = false;
                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                    MainForm->strings->GetDialogText(DialogText_RequestDeniedOnly, false),
                    MainForm->strings->GetDialogText(DialogText_RequestDeniedOnly, true));
            }
            if (create_character_reply == CharacterReply_RetryDialog33)
            {
                MainForm->edits->caret_enabled = false;
                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                    MainForm->strings->GetDialogText(DialogText_NotApprovedCharacter, false),
                    MainForm->strings->GetDialogText(DialogText_NotApprovedCharacter, true));
                MainForm->animation->menu->create_character_enabled = true;
                MainForm->edits->SetText(12, "");
                MainForm->edits->FocusEdit(12);
            }
            if (create_character_reply == CharacterReply_Created)
            {
                MainForm->edits->Clear();
                MainForm->players->RemovePreviewPlayer();
                MainForm->animation->menu->is_form_open = false;
                MainForm->edits->caret_enabled = false;
                MainForm->animation->menu->ShowMessageBox(MessageBoxKind_Plain,
                    MainForm->strings->GetDialogText(DialogText_CharacterCreated, false),
                    MainForm->strings->GetDialogText(DialogText_CharacterCreated, true));
                if (packet.Length() >= 3)
                    ParseCharacterList(packet.SubString(3, packet.Length()));
            }
            if (create_character_reply == CharacterReply_ListChanged)
            {
                MainForm->sfx->PlaySfx(6);
                if (packet.Length() >= 3)
                    ParseCharacterList(packet.SubString(3, packet.Length()));
            }
            // 10 and up = the create form opens: the reply is the session id.
            if (create_character_reply >= CharacterReply_SessionBase)
            {
                try
                {
                    MainForm->gfx->LoadCreateCharacterPanel();
                    MainForm->animation->menu->new_gender = 0;
                    MainForm->animation->menu->new_hair_style = 1;
                    MainForm->animation->menu->new_hair_color = 0;
                    MainForm->animation->menu->new_skin = 0;
                    MainForm->net_client->SetSessionId(create_character_reply);
                    MainForm->players->AddPreviewPlayer(0, 0);
                    MainForm->players->SetPreviewPosition(MainForm->animation->menu->form_x + 275,
                        MainForm->animation->menu->form_y + 94);
                    MainForm->edits->Clear();
                    MainForm->edits->AddEdit(12, 2, MainForm->animation->menu->form_x + 83,
                        MainForm->animation->menu->form_y + 56, 132, EDIT_LINE_HEIGHT);
                    MainForm->edits->SetTextColor(12, 11847900);
                    MainForm->edits->SetCharFilter(12, 4);
                    MainForm->edits->SetBackgroundVisible(12, 0);
                    MainForm->edits->SetBorderVisible(12, 0);
                    MainForm->edits->SetMaxLength(12, 12);
                    MainForm->edits->FocusEdit(12);
                    MainForm->animation->menu->create_character_enabled = true;
                    MainForm->edits->caret_enabled = true;
                    MainForm->animation->menu->OpenFormWindow(2);
                }
                catch (...)
                {
                }
            }
        }
    }
    if (family == OP_ATTACK)
    {
        if (action == AC_ERROR)
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                MainForm->strings->GetText(GameText_WaitMinuteBeforeTalking));
        if (action == AC_SET)
        {
            int attacker_id = DecodeNumber(packet.SubString(1, 2));
            int attack_direction = DecodeNumber(packet.SubString(3, 1));
            if (MainForm->players->HasPlayer(attacker_id))
            {
                int attacker_x = MainForm->players->GetX(attacker_id);
                int attacker_y = MainForm->players->GetY(attacker_id);
                if (MainForm->map->GetTileSpec(attacker_x, attacker_y) == TileSpec_Splash)
                {
                    MainForm->players->StartEffect(attacker_id, 8);
                    MainForm->sfx->PlaySfx(42);
                }
                MainForm->players->StartAttack(attacker_id, attack_direction, 0);
            }
            else
            {
                String attacker_request = EncodeNumber(this, attacker_id, 2);
                MainForm->net_client->SendPacket(AC_REQUEST, OP_PLAYERRANGE, attacker_request);
            }
        }
    }
    if (family == OP_GAMEBAR)
    {
        if (action == AC_GROUP)
        {
            int updated_stat_points = DecodeNumber(packet.SubString(1, 2));
            int updated_skill_points = DecodeNumber(packet.SubString(3, 2));
            int updated_max_health = DecodeNumber(packet.SubString(5, 2));
            int updated_max_mana = DecodeNumber(packet.SubString(7, 2));
            int updated_max_stamina = DecodeNumber(packet.SubString(9, 2));
            MainForm->animation->interface_controller->status->stat_points = updated_stat_points;
            MainForm->animation->interface_controller->status->skill_points = updated_skill_points;
            MainForm->animation->interface_controller->status->SetMaxHealth(updated_max_health);
            MainForm->animation->interface_controller->status->SetMaxMana(updated_max_mana);
            MainForm->animation->interface_controller->status->SetMaxStamina(updated_max_stamina);
            MainForm->animation->interface_controller->status->MarkDirty();
        }
        if (action == AC_LIST)
        {
            MainForm->animation->interface_controller->status->class_id = DecodeNumber(packet.SubString(1, 2));
            MainForm->animation->interface_controller->status->stat_str = DecodeNumber(packet.SubString(3, 2));
            MainForm->animation->interface_controller->status->stat_int = DecodeNumber(packet.SubString(5, 2));
            MainForm->animation->interface_controller->status->stat_wis = DecodeNumber(packet.SubString(7, 2));
            MainForm->animation->interface_controller->status->stat_agi = DecodeNumber(packet.SubString(9, 2));
            MainForm->animation->interface_controller->status->stat_con = DecodeNumber(packet.SubString(11, 2));
            MainForm->animation->interface_controller->status->stat_cha = DecodeNumber(packet.SubString(13, 2));
            MainForm->animation->interface_controller->status->SetMaxHealth(DecodeNumber(packet.SubString(15, 2)));
            MainForm->animation->interface_controller->status->SetMaxMana(DecodeNumber(packet.SubString(17, 2)));
            MainForm->animation->interface_controller->status->SetMaxStamina(DecodeNumber(packet.SubString(19, 2)));
            MainForm->animation->interface_controller->status->max_weight = DecodeNumber(packet.SubString(21, 2));
            MainForm->animation->interface_controller->status->label_413_from = DecodeNumber(packet.SubString(23, 2));
            MainForm->animation->interface_controller->status->label_413_to = DecodeNumber(packet.SubString(25, 2));
            MainForm->animation->interface_controller->status->label_414_value = DecodeNumber(packet.SubString(27, 2));
            MainForm->animation->interface_controller->status->label_416_value = DecodeNumber(packet.SubString(29, 2));
            MainForm->animation->interface_controller->status->label_415_value = DecodeNumber(packet.SubString(31, 2));
            MainForm->animation->interface_controller->status->MarkDirty();
        }
        if (action == AC_SET)
        {
            int recovered_health = DecodeNumber(packet.SubString(1, 2));
            int recovered_mana = DecodeNumber(packet.SubString(3, 2));
            int recover_unused_word = DecodeNumber(packet.SubString(5, 2));
            MainForm->animation->interface_controller->status->SetHealth(recovered_health);
            MainForm->animation->interface_controller->status->SetMana(recovered_mana);
        }
        if (action == AC_UPDATE)
        {
            int hit_player_id = DecodeNumber(packet.SubString(1, 2));
            int hit_damage = DecodeNumber(packet.SubString(3, 4));
            int hit_health_percent = DecodeNumber(packet.SubString(7, 1));
            if (hit_damage > 0)
            {
                MainForm->players->ShowDamage(hit_player_id, hit_damage, 1);
                MainForm->players->ShowHealthBar(hit_player_id, hit_health_percent, 8);
            }
        }
        if (action == AC_RESULT)
        {
            MainForm->animation->interface_controller->status->SetExperience(DecodeNumber(packet.SubString(1, 4)));
            // Sent with 1000 added, so it can be below 0.
            MainForm->animation->interface_controller->status->alignment = DecodeNumber(packet.SubString(5, 2)) - 1000;
            MainForm->animation->interface_controller->status->MarkDirty();
            if (packet.Length() > 6)
            {
                int gained_level = DecodeNumber(packet.SubString(7, 1));
                if (gained_level > 0)
                {
                    MainForm->animation->interface_controller->status->SetLevel(gained_level);
                    MainForm->animation->interface_controller->status->stat_points = DecodeNumber(packet.SubString(8,
                        2));
                    MainForm->animation->interface_controller->status->skill_points = DecodeNumber(packet.SubString(10,
                        2));
                    MainForm->players->StartLevelUpEffect(MainForm->net_client->GetPlayerId());
                }
            }
        }
    }
    if (family == OP_EFFECT)
    {
        if (action == AC_UPDATE)
        {
            while (packet.Length() > 3)
            {
                int effect_x = DecodeNumber(packet.SubString(1, 1));
                int effect_y = DecodeNumber(packet.SubString(2, 1));
                int tile_effect_id = DecodeNumber(packet.SubString(3, 2));
                packet.Delete(1, 4);
                MainForm->effects->AddEffect(effect_x, effect_y, tile_effect_id);
            }
        }
        if (action == AC_SET)
        {
            while (packet.Length() > 4)
            {
                int effect_player_id = DecodeNumber(packet.SubString(1, 2));
                int player_effect_id = DecodeNumber(packet.SubString(3, 3));
                packet.Delete(1, 5);
                MainForm->players->StartEffect(effect_player_id, player_effect_id);
            }
        }
        if (action == AC_DO)
        {
            int is_quake = DecodeNumber(packet.SubString(1, 1));
            if (is_quake == 1)
            {
                MainForm->sfx->PlaySfx(SfxSound_EffectUse);
                int quake_strength = DecodeNumber(packet.SubString(2, 1));
                MainForm->animation->interface_controller->StartScreenShake(quake_strength);
            }
        }
        if (action == AC_HIT)
        {
            int drain_target = DecodeNumber(packet.SubString(1, 1));
            int drain_damage = DecodeNumber(packet.SubString(2, 2));
            int drained_value = DecodeNumber(packet.SubString(4, 2));
            int drained_max = DecodeNumber(packet.SubString(6, 2));
            if (drain_target == 1)
            {
                MainForm->sfx->PlaySfx(SfxSound_EffectSpec);
                MainForm->animation->interface_controller->status->SetMana(drained_value);
                MainForm->animation->interface_controller->status->SetMaxMana(drained_max);
            }
            if (drain_target == 2)
            {
                MainForm->sfx->PlaySfx(72);
                MainForm->map->trap_reveal_timer = 2;
                MainForm->redraw_needed = true;
                MainForm->animation->interface_controller->status->SetHealth(drained_value);
                MainForm->animation->interface_controller->status->SetMaxHealth(drained_max);
                int drain_health_percent = 100 * MainForm->animation->interface_controller->status->health;
                drain_health_percent = drain_health_percent /
                    MainForm->animation->interface_controller->status->max_health;
                MainForm->players->ShowDamage(MainForm->net_client->GetPlayerId(), drain_damage, 0);
                MainForm->players->ShowHealthBar(MainForm->net_client->GetPlayerId(), drain_health_percent, 8);
                if (drained_value < 1)
                {
                    MainForm->players->KillPlayer(MainForm->net_client->GetPlayerId());
                    MainForm->sfx->PlaySfx(40);
                    MainForm->map->DelayWarp();
                }
            }
        }
        if (action == AC_PUBLIC)
        {
            if (MainForm->map->IsNearTrap())
                MainForm->sfx->PlaySfx(72);
            MainForm->map->trap_reveal_timer = 2;
            MainForm->redraw_needed = true;
        }
        if (action == AC_CAST)
        {
            int hurt_player_id = DecodeNumber(packet.SubString(1, 2));
            int hurt_health_percent = DecodeNumber(packet.SubString(3, 1));
            int hurt_player_died = DecodeNumber(packet.SubString(4, 1));
            int hurt_damage = DecodeNumber(packet.SubString(5, 2));
            MainForm->players->ShowDamage(hurt_player_id, hurt_damage, 0);
            MainForm->players->ShowHealthBar(hurt_player_id, hurt_health_percent, 8);
            if (hurt_player_died == 1)
            {
                MainForm->players->KillPlayer(hurt_player_id);
                MainForm->sfx->PlaySfx(40);
            }
        }
        if (action == AC_TARGET)
        {
            MainForm->sfx->PlaySfx(SfxSound_EffectTargetOther);
            int area_damage = DecodeNumber(packet.SubString(1, 2));
            int area_health = DecodeNumber(packet.SubString(3, 2));
            int area_max_health = DecodeNumber(packet.SubString(5, 2));
            MainForm->animation->interface_controller->status->SetHealth(area_health);
            MainForm->animation->interface_controller->status->SetMaxHealth(area_max_health);
            int area_health_percent = 100 * MainForm->animation->interface_controller->status->health;
            area_health_percent = area_health_percent / MainForm->animation->interface_controller->status->max_health;
            MainForm->players->ShowDamage(MainForm->net_client->GetPlayerId(), area_damage, 0);
            MainForm->players->ShowHealthBar(MainForm->net_client->GetPlayerId(), area_health_percent, 8);
            // Then 5 bytes per other player hit: id, health percent, damage.
            packet.Delete(1, 6);
            int area_victim_count = packet.Length() / 5;
            for (i = 0; i < area_victim_count; i++)
            {
                int victim_id = DecodeNumber(packet.SubString(5 * i + 1, 2));
                int victim_health_percent = DecodeNumber(packet.SubString(5 * i + 3, 1));
                int victim_damage = DecodeNumber(packet.SubString(5 * i + 4, 2));
                if (victim_id != MainForm->net_client->GetPlayerId())
                {
                    MainForm->players->ShowDamage(victim_id, victim_damage, 0);
                    MainForm->players->ShowHealthBar(victim_id, victim_health_percent, 8);
                }
            }
        }
    }
    if (family == OP_MUSIC)
    {
        if (action == AC_SET)
        {
            MainForm->sfx->StopMusic();
            MainForm->sfx->QueueMusic(DecodeNumber(String(packet[1])));
        }
        if (action == AC_DO)
        {
            MainForm->sfx->StopMusic();
            MainForm->sfx->QueueJukeboxSong(DecodeNumber(packet.SubString(1, 2)));
            MainForm->animation->jukebox->is_playing = 1;
            MainForm->animation->jukebox->play_ticks_left = JUKEBOX_PLAY_TICKS;
        }
        if (action == AC_RESULT && MainForm->animation->jukebox->is_visible)
        {
            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                MainForm->strings->GetDialogText(DialogText_JukeboxBusy, false),
                MainForm->strings->GetDialogText(DialogText_JukeboxBusy, true));
            MainForm->edits->caret_enabled = false;
        }
        if (action == AC_UPDATE)
        {
            int jukebox_gold_left = DecodeNumber(packet.SubString(1, 4));
            packet.Delete(1, 4);
            MainForm->animation->interface_controller->inventory->SetItemAmount(1, jukebox_gold_left);
            MainForm->animation->interface_controller->inventory->MarkItemsDirty();
            MainForm->animation->interface_controller->status->gold = jukebox_gold_left;
            MainForm->animation->interface_controller->status->MarkDirty();
            MainForm->sfx->PlaySfx(26);
            MainForm->animation->jukebox->Close();
        }
        if (action == AC_OPEN)
        {
            if (packet.Length() > 1)
            {
                char jukebox_playing = 0;
                String jukebox_text = "";
                int jukebox_id = DecodeNumber(packet.SubString(1, 2));
                packet.Delete(1, 2);
                if (packet.Length() > 3)
                {
                    jukebox_playing = 1;
                    jukebox_text = packet;
                }
                MainForm->animation->jukebox->Open(jukebox_id, jukebox_playing, 0, jukebox_text);
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    MainForm->strings->GetText(GameText_JukeboxIsNowViewed));
            }
        }
        if (action == AC_PLAY)
        {
            int musician_id = DecodeNumber(packet.SubString(1, 2));
            int musician_direction = DecodeNumber(packet.SubString(3, 1));
            int instrument_id = DecodeNumber(packet.SubString(4, 1));
            int note_id = DecodeNumber(packet.SubString(5, 1));
            MainForm->sfx->PlayInstrumentNote(instrument_id, note_id);
            MainForm->players->StartAttack(musician_id, musician_direction, 1);
        }
    }
    if (family == OP_SOUND && action == AC_SET)
        MainForm->sfx->PlaySfx(DecodeNumber(String(packet[1])));
    if (family == OP_MAGIC)
    {
        if (action == AC_ERROR)
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                MainForm->strings->GetText(GameText_WaitMinuteBeforeTalking));
        if (action == AC_REQUEST)
        {
            int chanter_id = DecodeNumber(packet.SubString(1, 2));
            int chanted_spell_id = DecodeNumber(packet.SubString(3, 2));
            String chant_text = MainForm->skill_values->GetShout(chanted_spell_id);
            MainForm->players->ShowTextLabel(chanter_id, chant_text, 2, 1, SHOUT_LABEL_TICKS);
        }
        if (action == AC_GROUP)
        {
            int group_spell_id = DecodeNumber(packet.SubString(1, 2));
            int group_caster_id = DecodeNumber(packet.SubString(3, 2));
            int group_spell_type = MainForm->skill_values->GetType(group_spell_id);
            if (!group_spell_type)
            {
                int group_spell_graphic = MainForm->skill_values->GetGraphic(group_spell_id);
                int group_heal_amount = DecodeNumber(packet.SubString(7, 2));
                int group_caster_mana = DecodeNumber(packet.SubString(5, 2));
                if (MainForm->net_client->GetPlayerId() == group_caster_id)
                {
                    int group_stamina_left = MainForm->skill_values->GetSpCost(group_spell_id);
                    group_stamina_left = MainForm->animation->interface_controller->status->stamina -
                        group_stamina_left;
                    MainForm->animation->interface_controller->status->SetMana(group_caster_mana);
                    MainForm->animation->interface_controller->status->SetStamina(group_stamina_left);
                }
                String group_spell_shout = MainForm->skill_values->GetShout(group_spell_id);
                MainForm->players->ShowTextLabel(group_caster_id, group_spell_shout, 3, 0, 10);
                MainForm->players->StartSpellCast(group_caster_id, -1);
                packet.Delete(1, 8);
                do
                {
                    int group_target_id = DecodeNumber(packet.SubString(1, 2));
                    int group_target_health_percent = DecodeNumber(packet.SubString(3, 1));
                    int group_target_health = DecodeNumber(packet.SubString(4, 2));
                    MainForm->players->StartEffect(group_target_id, group_spell_graphic - 1);
                    MainForm->players->ShowDamage(group_target_id, group_heal_amount, 1);
                    MainForm->players->ShowHealthBar(group_target_id, group_target_health_percent, 12);
                    if (MainForm->net_client->GetPlayerId() == group_target_id)
                        MainForm->animation->interface_controller->status->SetHealth(group_target_health);
                    packet.Delete(1, 5);
                } while (packet.Length() > 4);
            }
        }
        if (action == AC_TARGET)
        {
            int target_player_id = DecodeNumber(packet.SubString(1, 2));
            int target_caster_id = DecodeNumber(packet.SubString(3, 2));
            int target_heal_unused_byte = DecodeNumber(String(packet[5]));
            int target_spell_id = DecodeNumber(packet.SubString(6, 2));
            int target_spell_type = MainForm->skill_values->GetType(target_spell_id);
            if (!target_spell_type)
            {
                int target_spell_graphic = MainForm->skill_values->GetGraphic(target_spell_id);
                int target_heal_amount = DecodeNumber(packet.SubString(8, 4));
                int target_health_percent = DecodeNumber(packet.SubString(12, 1));
                if (target_heal_amount > 0)
                {
                    String target_spell_shout = MainForm->skill_values->GetShout(target_spell_id);
                    MainForm->players->ShowTextLabel(target_caster_id, target_spell_shout, 3, 0, 10);
                    MainForm->players->StartSpellCast(target_caster_id, -1);
                    MainForm->players->StartEffect(target_player_id, target_spell_graphic - 1);
                    MainForm->players->ShowDamage(target_player_id, target_heal_amount, 1);
                    MainForm->players->ShowHealthBar(target_player_id, target_health_percent, 12);
                }
                if (MainForm->net_client->GetPlayerId() == target_player_id)
                {
                    int target_new_health = DecodeNumber(packet.SubString(13, 2));
                    MainForm->animation->interface_controller->status->SetHealth(target_new_health);
                }
            }
        }
        if (action == AC_SELF)
        {
            int self_caster_id = DecodeNumber(packet.SubString(1, 2));
            int self_spell_id = DecodeNumber(packet.SubString(3, 2));
            int self_spell_type = MainForm->skill_values->GetType(self_spell_id);
            if (!self_spell_type)
            {
                int self_spell_graphic = MainForm->skill_values->GetGraphic(self_spell_id);
                int self_heal_amount = DecodeNumber(packet.SubString(5, 4));
                int self_health_percent = DecodeNumber(packet.SubString(9, 1));
                if (self_heal_amount > 0)
                {
                    String self_spell_shout = MainForm->skill_values->GetShout(self_spell_id);
                    MainForm->players->ShowTextLabel(self_caster_id, self_spell_shout, 3, 0, 10);
                    MainForm->players->StartEffect(self_caster_id, self_spell_graphic - 1);
                    MainForm->players->ShowDamage(self_caster_id, self_heal_amount, 1);
                    MainForm->players->ShowHealthBar(self_caster_id, self_health_percent, 12);
                }
                if (packet.Length() > 12)
                {
                    int self_new_health = DecodeNumber(packet.SubString(10, 2));
                    int self_new_mana = DecodeNumber(packet.SubString(12, 2));
                    int self_stamina_left = MainForm->skill_values->GetSpCost(self_spell_id);
                    self_stamina_left = MainForm->animation->interface_controller->status->stamina - self_stamina_left;
                    if (self_stamina_left < 0)
                        self_stamina_left = 0;
                    MainForm->animation->interface_controller->status->SetHealth(self_new_health);
                    MainForm->animation->interface_controller->status->SetMana(self_new_mana);
                    MainForm->animation->interface_controller->status->SetStamina(self_stamina_left);
                }
            }
        }
        if (action == AC_RESULT)
        {
            int cast_spell_id = DecodeNumber(packet.SubString(1, 2));
            int cast_new_health = DecodeNumber(packet.SubString(3, 2));
            int cast_new_mana = DecodeNumber(packet.SubString(5, 2));
            int cast_stamina_left = MainForm->skill_values->GetSpCost(cast_spell_id);
            cast_stamina_left = MainForm->animation->interface_controller->status->stamina - cast_stamina_left;
            if (cast_stamina_left < 0)
                cast_stamina_left = 0;
            MainForm->animation->interface_controller->status->SetHealth(cast_new_health);
            MainForm->animation->interface_controller->status->SetMana(cast_new_mana);
            MainForm->animation->interface_controller->status->SetStamina(cast_stamina_left);
        }
        if (action == AC_SET)
        {
            int casting_player_id = DecodeNumber(packet.SubString(1, 2));
            int casting_direction = DecodeNumber(packet.SubString(3, 1));
            if (MainForm->players->HasPlayer(casting_player_id))
                MainForm->players->StartSpellCast(casting_player_id, casting_direction);
            else
            {
                String casting_player_request = EncodeNumber(this, casting_player_id, 2);
                MainForm->net_client->SendPacket(AC_REQUEST, OP_PLAYERRANGE, casting_player_request);
            }
        }
    }
    if (family == OP_DIR && action == AC_SET)
    {
        int facing_player_id = DecodeNumber(packet.SubString(1, 2));
        int facing_direction = DecodeNumber(packet.SubString(3, 1));
        if (MainForm->players->HasPlayer(facing_player_id))
            MainForm->players->SetDirection(facing_player_id, facing_direction);
        else
        {
            String facing_player_request = EncodeNumber(this, facing_player_id, 2);
            MainForm->net_client->SendPacket(AC_REQUEST, OP_PLAYERRANGE, facing_player_request);
        }
    }
    if (family == OP_REST)
    {
        if (action == AC_RESULT)
        {
            int own_sit_x = DecodeNumber(packet.SubString(3, 1));
            int own_sit_y = DecodeNumber(packet.SubString(4, 1));
            int own_sit_direction = DecodeNumber(packet.SubString(5, 1));
            int own_x = MainForm->players->GetX(MainForm->net_client->GetPlayerId());
            int own_y = MainForm->players->GetY(MainForm->net_client->GetPlayerId());
            if (MainForm->map->GetTileSpec(own_x, own_y) == TileSpec_Splash)
            {
                MainForm->players->StartEffect(MainForm->net_client->GetPlayerId(), 8);
                MainForm->sfx->PlaySfx(42);
            }
            MainForm->players->StartSitOnFloor(MainForm->net_client->GetPlayerId(), own_sit_x, own_sit_y,
                own_sit_direction);
            MainForm->sfx->PlaySfx(13);
        }
        if (action == AC_CLOSE)
        {
            int own_stand_x = DecodeNumber(packet.SubString(3, 1));
            int own_stand_y = DecodeNumber(packet.SubString(4, 1));
            MainForm->players->StartStandUp(MainForm->net_client->GetPlayerId(), own_stand_x, own_stand_y);
        }
        if (action == AC_SET)
        {
            int sitting_player_id = DecodeNumber(packet.SubString(1, 2));
            int sit_x = DecodeNumber(packet.SubString(3, 1));
            int sit_y = DecodeNumber(packet.SubString(4, 1));
            int sit_direction = DecodeNumber(packet.SubString(5, 1));
            int sitting_player_x = MainForm->players->GetX(sitting_player_id);
            int sitting_player_y = MainForm->players->GetY(sitting_player_id);
            if (MainForm->map->GetTileSpec(sitting_player_x, sitting_player_y) == TileSpec_Splash)
            {
                MainForm->players->StartEffect(sitting_player_id, 8);
                MainForm->sfx->PlaySfx(42);
            }
            MainForm->players->StartSitOnFloor(sitting_player_id, sit_x, sit_y, sit_direction);
        }
        if (action == AC_DELETE)
        {
            int standing_player_id = DecodeNumber(packet.SubString(1, 2));
            int stand_x = DecodeNumber(packet.SubString(3, 1));
            int stand_y = DecodeNumber(packet.SubString(4, 1));
            MainForm->players->StartStandUp(standing_player_id, stand_x, stand_y);
        }
    }
    if (family == OP_SIT)
    {
        if (action == AC_RESULT)
        {
            int own_chair_x = DecodeNumber(packet.SubString(3, 1));
            int own_chair_y = DecodeNumber(packet.SubString(4, 1));
            int own_chair_direction = DecodeNumber(packet.SubString(5, 1));
            MainForm->players->StartSitOnChair(MainForm->net_client->GetPlayerId(), own_chair_x, own_chair_y,
                own_chair_direction);
            MainForm->sfx->PlaySfx(13);
        }
        if (action == AC_CLOSE)
        {
            int own_chair_stand_x = DecodeNumber(packet.SubString(3, 1));
            int own_chair_stand_y = DecodeNumber(packet.SubString(4, 1));
            MainForm->players->StartStandUp(MainForm->net_client->GetPlayerId(), own_chair_stand_x, own_chair_stand_y);
        }
        if (action == AC_SET)
        {
            int chair_player_id = DecodeNumber(packet.SubString(1, 2));
            int chair_x = DecodeNumber(packet.SubString(3, 1));
            int chair_y = DecodeNumber(packet.SubString(4, 1));
            int chair_direction = DecodeNumber(packet.SubString(5, 1));
            MainForm->players->StartSitOnChair(chair_player_id, chair_x, chair_y, chair_direction);
        }
        if (action == AC_DELETE)
        {
            int chair_stand_player_id = DecodeNumber(packet.SubString(1, 2));
            int chair_stand_x = DecodeNumber(packet.SubString(3, 1));
            int chair_stand_y = DecodeNumber(packet.SubString(4, 1));
            MainForm->players->StartStandUp(chair_stand_player_id, chair_stand_x, chair_stand_y);
        }
    }
    if (family == OP_PLAYER)
    {
        if (action == AC_RESULT)
        {
            int striker_id = DecodeNumber(packet.SubString(1, 2));
            int struck_player_id = DecodeNumber(packet.SubString(3, 2));
            int strike_damage = DecodeNumber(packet.SubString(5, 3));
            int strike_direction = DecodeNumber(packet.SubString(8, 1));
            int struck_health_percent = DecodeNumber(packet.SubString(9, 1));
            int struck_player_died = DecodeNumber(packet.SubString(10, 1));
            MainForm->players->StartAttack(striker_id, strike_direction, 0);
            MainForm->players->ShowDamage(struck_player_id, strike_damage, 0);
            MainForm->players->ShowHealthBar(struck_player_id, struck_health_percent, 8);
            if (struck_player_died > 0)
            {
                MainForm->players->KillPlayer(struck_player_id);
                MainForm->sfx->PlaySfx(40);
            }
        }
        if (action == AC_CAST)
        {
            int attack_caster_id = DecodeNumber(packet.SubString(1, 2));
            int spell_victim_id = DecodeNumber(packet.SubString(3, 2));
            int spell_damage = DecodeNumber(packet.SubString(5, 3));
            int spell_attack_unused_byte = DecodeNumber(packet.SubString(8, 1));
            int spell_victim_health_percent = DecodeNumber(packet.SubString(9, 1));
            int spell_victim_died = DecodeNumber(packet.SubString(10, 1));
            int attack_spell_id = DecodeNumber(packet.SubString(11, 2));
            int attack_spell_graphic = MainForm->skill_values->GetGraphic(attack_spell_id);
            String attack_spell_shout = MainForm->skill_values->GetShout(attack_spell_id);
            MainForm->players->ShowTextLabel(attack_caster_id, attack_spell_shout, 3, 0, 10);
            MainForm->players->StartSpellCast(attack_caster_id, -1);
            MainForm->players->StartEffect(spell_victim_id, attack_spell_graphic - 1);
            MainForm->players->ShowDamage(spell_victim_id, spell_damage, 0);
            MainForm->players->ShowHealthBar(spell_victim_id, spell_victim_health_percent, 8);
            if (spell_victim_died > 0)
            {
                MainForm->players->KillPlayer(spell_victim_id);
                MainForm->sfx->PlaySfx(40);
            }
        }
        if (action == AC_SET)
        {
            MainForm->animation->interface_controller->status->character_second_text = packet;
            MainForm->animation->interface_controller->status->MarkDirty();
        }
        if (action == AC_UPDATE)
            ApplyAppearanceChange(packet);
        if (action == AC_DELETE)
        {
            int leaving_player_id = DecodeNumber(packet.SubString(1, 2));
            int leaving_x = MainForm->players->GetX(leaving_player_id);
            int leaving_y = MainForm->players->GetY(leaving_player_id);
            MainForm->players->RemovePlayer(leaving_player_id);
            if (packet.Length() == 3)
            {
                int leave_kind = DecodeNumber(packet.SubString(3, 1));
                if (leave_kind == 1)
                    MainForm->sfx->PlaySfx(33);
                if (leave_kind == 2)
                    MainForm->effects->AddEffect(leaving_x, leaving_y, 2);
            }
            MainForm->redraw_needed = true;
        }
    }
    if (family == OP_PLAYERS)
    {
        if (action == AC_LIST)
            ParseOnlinePlayers(packet);
        if (action == AC_RESULT)
            ParseOnlineNames(packet);
        if (action == AC_DELETE)
        {
            MainForm->animation->interface_controller->group->RemoveMember(DecodeNumber(packet.SubString(1, 2)));
            if ((int)MainForm->animation->interface_controller->group->members.size() <= 1)
                MainForm->animation->interface_controller->group->Clear();
            MainForm->players->RemovePlayer(DecodeNumber(packet.SubString(1, 2)));
            MainForm->redraw_needed = true;
        }
        if (action == AC_UPDATE)
            ParseNearbyEntities(packet);
        if (action == AC_SYN1)
        {
            String online_notice_text = Client::Capitalize(MainForm->net_client, packet) +
                MainForm->strings->GetText(GameText_WasNowhereToBe);
            MainForm->sfx->PlaySfx(5);
            MainForm->animation->interface_controller->chat->AddLocalMessage(7, 0,
                Client::DeobfuscateString(MainForm->net_client, "`:Y,TL"), online_notice_text, true);
        }
        if (action == AC_SYN2)
        {
            String online_notice_text = Client::Capitalize(MainForm->net_client, packet) +
                MainForm->strings->GetText(GameText_IsOnlineAndOn);
            MainForm->sfx->PlaySfx(5);
            MainForm->animation->interface_controller->chat->AddLocalMessage(7, 0,
                Client::DeobfuscateString(MainForm->net_client, "`:Y,TL"), online_notice_text, true);
        }
        if (action == AC_SYN3)
        {
            String online_notice_text = Client::Capitalize(MainForm->net_client, packet) +
                MainForm->strings->GetText(GameText_IsOnlineSomewhereIn);
            MainForm->sfx->PlaySfx(5);
            MainForm->animation->interface_controller->chat->AddLocalMessage(7, 0,
                Client::DeobfuscateString(MainForm->net_client, "`:Y,TL"), online_notice_text, true);
        }
    }
    if (family == OP_RANGE && action == AC_RESULT)
        ParseNearbyEntities(packet);
    if (family == OP_M_NPC)
    {
        if (action == AC_RESULT)
        {
            int npc_spell_id = DecodeNumber(packet.SubString(1, 2));
            int npc_spell_caster_id = DecodeNumber(packet.SubString(3, 2));
            int npc_spell_direction = DecodeNumber(packet.SubString(5, 1));
            int spelled_npc_index = DecodeNumber(packet.SubString(6, 2));
            int npc_spell_damage = DecodeNumber(packet.SubString(8, 3));
            int spelled_npc_health_percent = DecodeNumber(packet.SubString(11, 2));
            if (packet.Length() >= 12)
            {
                if (MainForm->players->HasPlayer(npc_spell_caster_id))
                {
                    int npc_spell_caster_x = MainForm->players->GetX(npc_spell_caster_id);
                    int npc_spell_caster_y = MainForm->players->GetY(npc_spell_caster_id);
                    if (MainForm->map->GetTileSpec(npc_spell_caster_x, npc_spell_caster_y) == TileSpec_Splash)
                    {
                        MainForm->players->StartEffect(npc_spell_caster_id, 8);
                        MainForm->sfx->PlaySfx(42);
                    }
                    if (MainForm->net_client->GetPlayerId() != npc_spell_caster_id)
                        MainForm->players->StartSpellCast(npc_spell_caster_id, npc_spell_direction);
                    else
                    {
                        int npc_spell_caster_mana = DecodeNumber(packet.SubString(13, 2));
                        int npc_spell_stamina_left = MainForm->skill_values->GetSpCost(npc_spell_id);
                        npc_spell_stamina_left = MainForm->animation->interface_controller->status->stamina -
                            npc_spell_stamina_left;
                        MainForm->animation->interface_controller->status->SetStamina(npc_spell_stamina_left);
                        MainForm->animation->interface_controller->status->SetMana(npc_spell_caster_mana);
                    }
                    String npc_spell_shout = MainForm->skill_values->GetShout(npc_spell_id);
                    MainForm->players->ShowTextLabel(npc_spell_caster_id, npc_spell_shout, 3, 0, 10);
                }
                else
                {
                    String npc_spell_caster_request = EncodeNumber(this, npc_spell_caster_id, 2);
                    MainForm->net_client->SendPacket(AC_REQUEST, OP_PLAYERRANGE, npc_spell_caster_request);
                }
            }
            if (packet.Length() == 15)
            {
                int npc_spell_result = DecodeNumber(packet.SubString(15, 1));
                if (npc_spell_result == 2)
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                        MainForm->strings->GetText(GameText_UnableToAttackThis));
            }
            int npc_spell_graphic = MainForm->skill_values->GetGraphic(npc_spell_id);
            MainForm->npcs->StartEffect(spelled_npc_index, npc_spell_graphic - 1);
            MainForm->npcs->ShowDamage(spelled_npc_index, npc_spell_damage, 0);
            MainForm->npcs->ShowHealthBar(spelled_npc_index, spelled_npc_health_percent, 8);
        }
        if (action == AC_HIT || action == AC_CONFIRM)
        {
            int killing_spell_id = DecodeNumber(packet.SubString(1, 2));
            int spell_killer_id = DecodeNumber(packet.SubString(3, 2));
            int spell_killer_direction = DecodeNumber(packet.SubString(5, 1));
            int spell_killed_npc = DecodeNumber(packet.SubString(6, 2));
            int spell_drop_uid = DecodeNumber(packet.SubString(8, 2));
            int spell_drop_item_id = DecodeNumber(packet.SubString(10, 2));
            int spell_drop_x = DecodeNumber(packet.SubString(12, 1));
            int spell_drop_y = DecodeNumber(packet.SubString(13, 1));
            int spell_drop_amount = DecodeNumber(packet.SubString(14, 4));
            int spell_kill_damage = DecodeNumber(packet.SubString(18, 3));
            if (packet.Length() > 23)
            {
                int spell_killer_mana = DecodeNumber(packet.SubString(21, 2));
                int spell_kill_stamina_left = MainForm->skill_values->GetSpCost(killing_spell_id);
                spell_kill_stamina_left = MainForm->animation->interface_controller->status->stamina -
                    spell_kill_stamina_left;
                MainForm->animation->interface_controller->status->SetStamina(spell_kill_stamina_left);
                MainForm->animation->interface_controller->status->SetMana(spell_killer_mana);
                int spell_kill_experience = DecodeNumber(packet.SubString(23, 4));
                if (MainForm->animation->interface_controller->status->experience < spell_kill_experience)
                {
                    int spell_kill_experience_gained = spell_kill_experience -
                        MainForm->animation->interface_controller->status->experience;
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouGained) + " " + IntToStr(spell_kill_experience_gained) +
                        " EXP");
                    MainForm->animation->interface_controller->chat->AddSystemMessage(5, 0,
                        MainForm->strings->GetText(GameText_YouGained) + " " + IntToStr(spell_kill_experience_gained) +
                        " EXP", true);
                }
                MainForm->animation->interface_controller->status->SetExperience(spell_kill_experience);
                MainForm->animation->interface_controller->status->MarkDirty();
                if (action == AC_CONFIRM)
                {
                    int spell_kill_level = DecodeNumber(packet.SubString(27, 1));
                    int spell_kill_stat_points = DecodeNumber(packet.SubString(28, 2));
                    int spell_kill_skill_points = DecodeNumber(packet.SubString(30, 2));
                    int spell_kill_max_health = DecodeNumber(packet.SubString(32, 2));
                    int spell_kill_max_mana = DecodeNumber(packet.SubString(34, 2));
                    int spell_kill_max_stamina = DecodeNumber(packet.SubString(36, 2));
                    MainForm->animation->interface_controller->status->SetLevel(spell_kill_level);
                    MainForm->animation->interface_controller->status->stat_points = spell_kill_stat_points;
                    MainForm->animation->interface_controller->status->skill_points = spell_kill_skill_points;
                    MainForm->animation->interface_controller->status->SetMaxHealth(spell_kill_max_health);
                    MainForm->animation->interface_controller->status->SetMaxMana(spell_kill_max_mana);
                    MainForm->animation->interface_controller->status->SetMaxStamina(spell_kill_max_stamina);
                }
            }
            int killing_spell_graphic = MainForm->skill_values->GetGraphic(killing_spell_id);
            MainForm->npcs->StartEffect(spell_killed_npc, killing_spell_graphic - 1);
            MainForm->npcs->ShowDamage(spell_killed_npc, spell_kill_damage, 0);
            MainForm->npcs->ShowHealthBar(spell_killed_npc, 0, 4);
            MainForm->npcs->KillNpc(spell_killed_npc);
            if (spell_killer_id > 0 && MainForm->net_client->GetPlayerId() != spell_killer_id)
            {
                int spell_killer_x = MainForm->players->GetX(spell_killer_id);
                int spell_killer_y = MainForm->players->GetY(spell_killer_id);
                if (MainForm->map->GetTileSpec(spell_killer_x, spell_killer_y) == TileSpec_Splash)
                {
                    MainForm->players->StartEffect(spell_killer_id, 8);
                    MainForm->sfx->PlaySfx(42);
                }
                if (MainForm->net_client->GetPlayerId() != spell_killer_id)
                    MainForm->players->StartSpellCast(spell_killer_id, spell_killer_direction);
            }
            if (spell_drop_item_id > 0 && spell_drop_amount > 0)
            {
                MainForm->map_items->AddProtectedItem(spell_drop_uid, spell_drop_item_id, spell_drop_x, spell_drop_y,
                    spell_drop_amount, spell_killer_id, 84);
                MainForm->animation->interface_controller->chat->AddSystemMessage(16, 0,
                    MainForm->strings->GetText(GameText_NpcDropped) + IntToStr(spell_drop_amount) + " " +
                    MainForm->item_values->GetName(spell_drop_item_id), true);
                MainForm->map_items->SortByPosition();
            }
            if (action == AC_CONFIRM)
                MainForm->players->StartLevelUpEffect(spell_killer_id);
            String killing_spell_shout = MainForm->skill_values->GetShout(killing_spell_id);
            MainForm->players->ShowTextLabel(spell_killer_id, killing_spell_shout, 3, 0, 10);
            MainForm->redraw_needed = true;
        }
    }
    if (family == OP_NPC)
    {
        if (action == AC_RESULT)
        {
            int npc_attacker_id = DecodeNumber(packet.SubString(1, 2));
            int npc_attacker_direction = DecodeNumber(packet.SubString(3, 1));
            int hit_npc_index = DecodeNumber(packet.SubString(4, 2));
            int npc_hit_damage = DecodeNumber(packet.SubString(6, 3));
            int hit_npc_health_percent = DecodeNumber(packet.SubString(9, 2));
            if (packet.Length() == 10)
            {
                if (MainForm->players->HasPlayer(npc_attacker_id))
                {
                    int npc_attacker_x = MainForm->players->GetX(npc_attacker_id);
                    int npc_attacker_y = MainForm->players->GetY(npc_attacker_id);
                    if (MainForm->map->GetTileSpec(npc_attacker_x, npc_attacker_y) == TileSpec_Splash)
                    {
                        MainForm->players->StartEffect(npc_attacker_id, 8);
                        MainForm->sfx->PlaySfx(42);
                    }
                    MainForm->players->StartAttack(npc_attacker_id, npc_attacker_direction, 0);
                }
                else
                {
                    String npc_attacker_request = EncodeNumber(this, npc_attacker_id, 2);
                    MainForm->net_client->SendPacket(AC_REQUEST, OP_PLAYERRANGE, npc_attacker_request);
                }
            }
            if (packet.Length() == 11)
            {
                int npc_hit_result = DecodeNumber(packet.SubString(11, 1));
                if (npc_hit_result == 2)
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                        MainForm->strings->GetText(GameText_UnableToAttackThis));
            }
            MainForm->npcs->ShowDamage(hit_npc_index, npc_hit_damage, 0);
            MainForm->npcs->ShowHealthBar(hit_npc_index, hit_npc_health_percent, 8);
        }
        if (action == AC_HIT || action == AC_CONFIRM)
        {
            int npc_killer_id = DecodeNumber(packet.SubString(1, 2));
            int npc_killer_direction = DecodeNumber(packet.SubString(3, 1));
            int killed_npc_index = DecodeNumber(packet.SubString(4, 2));
            int kill_drop_uid = DecodeNumber(packet.SubString(6, 2));
            int kill_drop_item_id = DecodeNumber(packet.SubString(8, 2));
            int kill_drop_x = DecodeNumber(packet.SubString(10, 1));
            int kill_drop_y = DecodeNumber(packet.SubString(11, 1));
            int kill_drop_amount = DecodeNumber(packet.SubString(12, 4));
            int kill_damage = DecodeNumber(packet.SubString(16, 3));
            if (packet.Length() > 21)
            {
                int kill_experience = DecodeNumber(packet.SubString(19, 4));
                if (MainForm->animation->interface_controller->status->experience < kill_experience)
                {
                    int kill_experience_gained = kill_experience -
                        MainForm->animation->interface_controller->status->experience;
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                        MainForm->strings->GetText(GameText_YouGained) + " " + IntToStr(kill_experience_gained) +
                        " EXP");
                    MainForm->animation->interface_controller->chat->AddSystemMessage(5, 0,
                        MainForm->strings->GetText(GameText_YouGained) + " " + IntToStr(kill_experience_gained) +
                        " EXP", true);
                }
                MainForm->animation->interface_controller->status->SetExperience(kill_experience);
                MainForm->animation->interface_controller->status->MarkDirty();
                if (action == AC_CONFIRM)
                {
                    int kill_level = DecodeNumber(packet.SubString(23, 1));
                    int kill_stat_points = DecodeNumber(packet.SubString(24, 2));
                    int kill_skill_points = DecodeNumber(packet.SubString(26, 2));
                    int kill_max_health = DecodeNumber(packet.SubString(28, 2));
                    int kill_max_mana = DecodeNumber(packet.SubString(30, 2));
                    int kill_max_stamina = DecodeNumber(packet.SubString(32, 2));
                    MainForm->animation->interface_controller->status->SetLevel(kill_level);
                    MainForm->animation->interface_controller->status->stat_points = kill_stat_points;
                    MainForm->animation->interface_controller->status->skill_points = kill_skill_points;
                    MainForm->animation->interface_controller->status->SetMaxHealth(kill_max_health);
                    MainForm->animation->interface_controller->status->SetMaxMana(kill_max_mana);
                    MainForm->animation->interface_controller->status->SetMaxStamina(kill_max_stamina);
                }
            }
            MainForm->npcs->ShowDamage(killed_npc_index, kill_damage, 0);
            MainForm->npcs->ShowHealthBar(killed_npc_index, 0, 4);
            MainForm->npcs->KillNpc(killed_npc_index);
            if (npc_killer_id > 0 && MainForm->net_client->GetPlayerId() != npc_killer_id)
            {
                int npc_killer_x = MainForm->players->GetX(npc_killer_id);
                int npc_killer_y = MainForm->players->GetY(npc_killer_id);
                if (MainForm->map->GetTileSpec(npc_killer_x, npc_killer_y) == TileSpec_Splash)
                {
                    MainForm->players->StartEffect(npc_killer_id, 8);
                    MainForm->sfx->PlaySfx(42);
                }
                MainForm->players->StartAttack(npc_killer_id, npc_killer_direction, 0);
            }
            if (kill_drop_item_id > 0 && kill_drop_amount > 0)
            {
                MainForm->map_items->AddProtectedItem(kill_drop_uid, kill_drop_item_id, kill_drop_x, kill_drop_y,
                    kill_drop_amount, npc_killer_id, 84);
                MainForm->animation->interface_controller->chat->AddSystemMessage(16, 0,
                    MainForm->strings->GetText(GameText_NpcDropped) + IntToStr(kill_drop_amount) + " " +
                    MainForm->item_values->GetName(kill_drop_item_id), true);
                MainForm->map_items->SortByPosition();
            }
            if (action == AC_CONFIRM)
                MainForm->players->StartLevelUpEffect(npc_killer_id);
            MainForm->redraw_needed = true;
        }
        if (action == AC_UPDATE)
        {
            int appearing_npc_count = DecodeNumber(String(packet[1]));
            for (int i0 = 0; i0 < appearing_npc_count; i0++)
            {
                int appearing_npc_id = DecodeNumber(packet.SubString(6 * i0 + 3, 2));
                npc_graphic = MainForm->npc_values->GetGraphic(appearing_npc_id);
                String appearing_npc_name = MainForm->npc_values->GetName(appearing_npc_id);
                MainForm->npcs->AddNpc(DecodeNumber(packet.SubString(6 * i0 + 2, 1)), appearing_npc_id, npc_graphic,
                    appearing_npc_name, DecodeNumber(packet.SubString(6 * i0 + 5, 1)),
                    DecodeNumber(packet.SubString(6 * i0 + 6, 1)), DecodeNumber(packet.SubString(6 * i0 + 7, 1)));
            }
        }
        if (action == AC_SET)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            // Four lists:
            // Npc steps (4 bytes each) and npc attacks (9 bytes each)
            // Npc talk (index, length, text), then the player's health and mana
            String npc_walk_data = MainForm->net_client->NextToken();
            String npc_attack_data = MainForm->net_client->NextToken();
            String npc_chat_data = MainForm->net_client->NextToken();
            String own_stats_data = MainForm->net_client->NextToken();
            int npc_walk_count = npc_walk_data.Length() / 4;
            for (int j = 0; j < npc_walk_count; j++)
            {
                int walking_npc_index = DecodeNumber(npc_walk_data.SubString(4 * j + 1, 1));
                int npc_walk_x = DecodeNumber(npc_walk_data.SubString(4 * j + 2, 1));
                int npc_walk_y = DecodeNumber(npc_walk_data.SubString(4 * j + 3, 1));
                int npc_walk_direction = DecodeNumber(npc_walk_data.SubString(4 * j + 4, 1));
                if (MainForm->npcs->HasNpc(walking_npc_index))
                    MainForm->npcs->StartWalk(walking_npc_index, npc_walk_x, npc_walk_y, npc_walk_direction);
                else
                {
                    String walk_npc_request = EncodeNumber(this, 1, 1);
                    walk_npc_request = walk_npc_request + String(ByteToChar(TOKEN_BREAK_BYTE));
                    walk_npc_request = walk_npc_request + EncodeNumber(this, walking_npc_index, 1);
                    MainForm->net_client->SendPacket(AC_REQUEST, OP_NPCRANGE, walk_npc_request);
                }
            }
            for (int i15 = 0; i15 < npc_attack_data.Length() / 9; i15++)
            {
                int attacking_npc_index = DecodeNumber(npc_attack_data.SubString(9 * i15 + 1, 1));
                int npc_attack_kind = DecodeNumber(npc_attack_data.SubString(9 * i15 + 2, 1));
                int npc_attack_direction = DecodeNumber(npc_attack_data.SubString(9 * i15 + 3, 1));
                int npc_target_player_id = DecodeNumber(npc_attack_data.SubString(9 * i15 + 4, 2));
                int npc_attack_damage = DecodeNumber(npc_attack_data.SubString(9 * i15 + 6, 3));
                int npc_target_health_percent = DecodeNumber(npc_attack_data.SubString(9 * i15 + 9, 1));
                if (MainForm->npcs->HasNpc(attacking_npc_index))
                {
                    // 2 = the hit killed the player.
                    if (npc_attack_kind == 1 || npc_attack_kind == 2)
                        MainForm->npcs->StartAttack(attacking_npc_index, npc_attack_direction);
                    if (npc_attack_kind == 2)
                    {
                        MainForm->players->KillPlayer(npc_target_player_id);
                        MainForm->sfx->PlaySfx(40);
                        if (npc_target_player_id == MainForm->net_client->GetPlayerId())
                            MainForm->map->DelayWarp();
                    }
                    if (npc_target_player_id == MainForm->net_client->GetPlayerId())
                    {
                        int own_health_left = MainForm->animation->interface_controller->status->health -
                            npc_attack_damage;
                        MainForm->animation->interface_controller->status->SetHealth(own_health_left);
                    }
                    MainForm->players->ShowDamage(npc_target_player_id, npc_attack_damage, 0);
                    MainForm->players->ShowHealthBar(npc_target_player_id, npc_target_health_percent, 8);
                }
                else
                {
                    String attack_npc_request = EncodeNumber(this, 1, 1);
                    attack_npc_request = attack_npc_request + String(ByteToChar(TOKEN_BREAK_BYTE));
                    attack_npc_request = attack_npc_request + EncodeNumber(this, attacking_npc_index, 1);
                    MainForm->net_client->SendPacket(AC_REQUEST, OP_NPCRANGE, attack_npc_request);
                }
            }
            char has_npc_chat = 0;
            if (npc_chat_data.Length() > 2)
                has_npc_chat = 1;
            do
            {
                if (has_npc_chat)
                {
                    int talking_npc_index = DecodeNumber(npc_chat_data.SubString(1, 1));
                    int npc_message_length = DecodeNumber(npc_chat_data.SubString(2, 1));
                    npc_chat_data.Delete(1, 2);
                    if (npc_chat_data.Length() >= npc_message_length)
                    {
                        String npc_message = npc_chat_data.SubString(1, npc_message_length);
                        npc_chat_data.Delete(1, npc_message_length);
                        int talking_npc_id = MainForm->npcs->GetNpcId(talking_npc_index);
                        String talking_npc_name = MainForm->npc_values->GetName(talking_npc_id);
                        if (talking_npc_name != "")
                        {
                            MainForm->animation->interface_controller->chat->AddLocalMessage(0, 0, talking_npc_name,
                                npc_message, false);
                            MainForm->gfx->npcs->ShowSpeechBalloon(npc_message, talking_npc_index, 0);
                        }
                    }
                    else
                        has_npc_chat = 0;
                    if (npc_chat_data.Length() < 3)
                        has_npc_chat = 0;
                }
            } while (has_npc_chat);
            if (own_stats_data.Length() == 4)
            {
                int own_health_update = DecodeNumber(own_stats_data.SubString(1, 2));
                int own_mana_update = DecodeNumber(own_stats_data.SubString(3, 2));
                MainForm->animation->interface_controller->status->SetHealth(own_health_update);
                MainForm->animation->interface_controller->status->SetMana(own_mana_update);
            }
        }
        if (action == AC_TALK)
        {
            int speaking_npc_index = DecodeNumber(packet.SubString(1, 2));
            String npc_speech = packet.SubString(3, packet.Length() - 2);
            int speaking_npc_id = MainForm->npcs->GetNpcId(speaking_npc_index);
            String speaking_npc_name = MainForm->npc_values->GetName(speaking_npc_id);
            if (speaking_npc_name != "")
            {
                MainForm->animation->interface_controller->chat->AddLocalMessage(0, 0, speaking_npc_name, npc_speech,
                    false);
                MainForm->gfx->npcs->ShowSpeechBalloon(npc_speech, speaking_npc_index, 0);
            }
        }
        if (action == AC_JUNK)
            MainForm->npcs->KillNpcsById(DecodeNumber(packet.SubString(1, 2)));
    }
    if (family == OP_MOVE)
    {
        if (action == AC_RESULT)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            String nearby_player_ids = MainForm->net_client->NextToken();
            String nearby_npc_indexes = MainForm->net_client->NextToken();
            String nearby_item_data = MainForm->net_client->NextToken();
            // The players and npcs in view not known yet are asked for in one request.
            String missing_entities = "";
            missing_count = 0;
            for (int k = 0; 2 * k + 2 <= nearby_player_ids.Length(); k++)
            {
                if (!MainForm->players->HasPlayer(DecodeNumber(nearby_player_ids.SubString(2 * k + 1, 2))))
                {
                    missing_entities = missing_entities + nearby_player_ids.SubString(2 * k + 1, 2);
                    missing_count++;
                }
            }
            missing_entities = missing_entities + ByteToChar(TOKEN_BREAK_BYTE);
            for (int i16 = 0; i16 + 1 <= nearby_npc_indexes.Length(); i16++)
            {
                if (!MainForm->npcs->HasNpc(DecodeNumber(nearby_npc_indexes.SubString(i16 + 1, 1))))
                {
                    missing_entities = missing_entities + nearby_npc_indexes.SubString(i16 + 1, 1);
                    missing_count++;
                }
            }
            if (nearby_item_data.Length() >= 1)
            {
                int nearby_item_count = nearby_item_data.Length() / 9;
                for (int i17 = 0; i17 < nearby_item_count; i17++)
                    MainForm->map_items->AddItem(DecodeNumber(nearby_item_data.SubString(9 * i17 + 1, 2)),
                        DecodeNumber(nearby_item_data.SubString(9 * i17 + 3, 2)),
                        DecodeNumber(nearby_item_data.SubString(9 * i17 + 5, 1)),
                        DecodeNumber(nearby_item_data.SubString(9 * i17 + 6, 1)),
                        DecodeNumber(nearby_item_data.SubString(9 * i17 + 7, 3)));
                MainForm->map_items->SortByPosition();
            }
            if (missing_count >= 1)
                MainForm->net_client->SendPacket(AC_REQUEST, OP_RANGE, missing_entities);
        }
        if (action == AC_SET)
        {
            int walking_player_id = DecodeNumber(packet.SubString(1, 2));
            int walk_direction = DecodeNumber(packet.SubString(3, 1));
            int walk_x = DecodeNumber(packet.SubString(4, 1));
            int walk_y = DecodeNumber(packet.SubString(5, 1));
            if (MainForm->animation->game->player_menu_visible &&
                MainForm->animation->game->player_menu_target_id == walking_player_id)
                MainForm->animation->game->ClosePlayerMenu();
            if (MainForm->players->HasPlayer(walking_player_id))
            {
                char is_plain_walk = 1;
                char walk_spare_flag = 0;
                int walker_x = MainForm->players->GetX(walking_player_id);
                int walker_y = MainForm->players->GetY(walking_player_id);
                if (MainForm->map->GetTileSpec(walk_x, walk_y) == TileSpec_Jump ||
                    MainForm->map->GetTileSpec(walker_x, walker_y) == TileSpec_Jump)
                {
                    is_plain_walk = 0;
                    MainForm->players->StartJump(walking_player_id, walk_x, walk_y, walk_direction);
                }
                if (MainForm->map->GetTileSpec(walk_x, walk_y) == TileSpec_Splash)
                {
                    MainForm->players->StartEffect(walking_player_id, 8);
                    MainForm->sfx->PlaySfx(42);
                }
                if (is_plain_walk)
                    MainForm->players->StartWalk(walking_player_id, walk_x, walk_y, walk_direction);
            }
            else
            {
                String walker_request = EncodeNumber(this, walking_player_id, 2);
                MainForm->net_client->SendPacket(AC_REQUEST, OP_PLAYERRANGE, walker_request);
            }
        }
        if (action == AC_OPEN)
        {
            MainForm->sfx->PlaySfx(28);
            MainForm->map->UnfreezePlayer();
        }
        if (action == AC_CLOSE)
        {
            MainForm->sfx->PlaySfx(28);
            MainForm->map->FreezePlayer();
        }
    }
    if (family == OP_ARENA)
    {
        if (action == AC_HIT)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            int arena_killer_id = DecodeNumber(MainForm->net_client->NextToken());
            int arena_killer_direction = DecodeNumber(MainForm->net_client->NextToken());
            int arena_kill_count = DecodeNumber(MainForm->net_client->NextToken());
            String arena_killer_name = MainForm->net_client->NextToken();
            String arena_victim_name = MainForm->net_client->NextToken();
            String arena_kill_message = Client::Capitalize(MainForm->net_client, arena_victim_name) + " " +
                MainForm->strings->GetText(GameText_WasEliminatedBy) + arena_killer_name;
            if (arena_kill_count > 1)
                arena_kill_message = arena_kill_message + ", " + arena_killer_name + " " +
                    MainForm->strings->GetText(GameText_Killed) + IntToStr(arena_kill_count) + MainForm->strings->GetText(GameText_PlayerS);
            MainForm->animation->interface_controller->chat->AddSystemMessage(20, 0, arena_kill_message, false);
            if (MainForm->players->HasPlayer(arena_killer_id) && MainForm->net_client->GetPlayerId() != arena_killer_id)
                MainForm->players->StartAttack(arena_killer_id, arena_killer_direction, 0);
        }
        if (action == AC_DROP)
        {
            MainForm->sfx->PlaySfx(51);
            MainForm->animation->interface_controller->chat->AddLocalMessage(6, 2,
                MainForm->strings->GetText(GameText_Server), MainForm->strings->GetText(GameText_NewRoundIsDelayed),
                false);
            MainForm->animation->interface_controller->chat->AddGlobalMessage(6, CHAT_COLOR_NOTICE,
                MainForm->strings->GetText(GameText_Server), MainForm->strings->GetText(GameText_NewRoundIsDelayed),
                false, false);
            MainForm->animation->interface_controller->chat->AddSystemMessage(6, 2,
                MainForm->strings->GetText(GameText_NewRoundIsDelayed), false);
        }
        if (action == AC_DO)
        {
            MainForm->sfx->PlaySfx(5);
            int arena_notice_number = DecodeNumber(String(packet[1]));
            MainForm->animation->interface_controller->chat->AddLocalMessage(6, 2,
                MainForm->strings->GetText(GameText_Server),
                IntToStr(arena_notice_number) + MainForm->strings->GetText(GameText_PlayerSWereLaunched), false);
            MainForm->animation->interface_controller->chat->AddGlobalMessage(6, CHAT_COLOR_NOTICE,
                MainForm->strings->GetText(GameText_Server),
                IntToStr(arena_notice_number) + MainForm->strings->GetText(GameText_PlayerSWereLaunched), false, false);
            MainForm->animation->interface_controller->chat->AddSystemMessage(6, 2,
                IntToStr(arena_notice_number) + MainForm->strings->GetText(GameText_PlayerSWereLaunched), false);
            int own_spot_x = MainForm->players->GetX(MainForm->net_client->GetPlayerId());
            int own_spot_y = MainForm->players->GetY(MainForm->net_client->GetPlayerId());
            MainForm->animation->game->StartSpotCountdown(own_spot_x, own_spot_y);
        }
        if (action == AC_CONFIRM)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            String arena_winner_name = MainForm->net_client->NextToken();
            int final_kill_count = DecodeNumber(MainForm->net_client->NextToken());
            String final_killer_name = MainForm->net_client->NextToken();
            String final_victim_name = MainForm->net_client->NextToken();
            MainForm->sfx->PlaySfx(SfxSound_EliminatedBy);
            String final_kill_message = Client::Capitalize(MainForm->net_client, final_victim_name) + " " +
                MainForm->strings->GetText(GameText_WasEliminatedBy) + final_killer_name;
            if (final_kill_count > 1)
                final_kill_message = final_kill_message + ", " + final_killer_name + " " +
                    MainForm->strings->GetText(GameText_Killed) + IntToStr(final_kill_count) + MainForm->strings->GetText(GameText_PlayerS);
            MainForm->animation->interface_controller->chat->AddSystemMessage(20, 0, final_kill_message, false);
            MainForm->animation->interface_controller->chat->AddLocalMessage(21, 2,
                MainForm->strings->GetText(GameText_Server),
                arena_winner_name + MainForm->strings->GetText(GameText_WonArenaEventServer), false);
            MainForm->animation->interface_controller->chat->AddGlobalMessage(21, CHAT_COLOR_NOTICE,
                MainForm->strings->GetText(GameText_Server),
                arena_winner_name + MainForm->strings->GetText(GameText_WonArenaEventServer), false, false);
            MainForm->animation->interface_controller->chat->AddSystemMessage(21, 2,
                arena_winner_name + MainForm->strings->GetText(GameText_WonArenaEventServer), false);
        }
    }
    if (family == OP_CHAT)
    {
        if (action == AC_HIT && !MainForm->edits->chat_locked)
        {
            MainForm->animation->interface_controller->chat->AddLocalMessage(6, 2,
                MainForm->strings->GetText(GameText_Server), MainForm->strings->GetText(GameText_YouHaveBeenMuted) +
                " " + Client::Capitalize(MainForm->net_client, packet), false);
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action, "5 " +
                MainForm->strings->GetText(GameText_MinutesMuted));
            MainForm->edits->LockChatInput();
        }
        if (action == AC_RESULT)
        {
            int private_reply = DecodeNumber(packet.SubString(1, 2));
            if (private_reply == 1)
            {
                String unreachable_name = packet.SubString(3, packet.Length() - 2);
                MainForm->animation->interface_controller->chat->AddSystemMessage(2, 3,
                    Client::Capitalize(MainForm->net_client, unreachable_name) + " " +
                    MainForm->strings->GetText(GameText_CouldNotBeFound), false);
                MainForm->animation->interface_controller->chat->ClosePrivateTab(MainForm->animation->interface_controller->chat->FindPrivateTab(unreachable_name) +
                    1);
            }
        }
        if (action == AC_PRIVATE && MainForm->config->GetAcceptPrivateMessages())
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            String private_sender = MainForm->net_client->NextToken();
            String private_text = Client::CensorText(MainForm->net_client, MainForm->net_client->NextToken());
            if (!MainForm->animation->social->IsIgnored(private_sender))
            {
                if (private_text.Length() >= 1)
                {
                    MainForm->sfx->PlaySfx(SfxSound_TalkTell);
                    MainForm->animation->interface_controller->chat->AddLocalMessage(1, 1,
                        Client::Capitalize(MainForm->net_client, private_sender), private_text, false);
                    int private_tab = MainForm->animation->interface_controller->chat->FindPrivateTab(private_sender);
                    if (private_tab != -1)
                        MainForm->animation->interface_controller->chat->AddPrivateMessage(private_tab, 0,
                            Client::Capitalize(MainForm->net_client, private_sender), private_text, false);
                }
            }
        }
        if (action == AC_OPEN)
        {
            if (packet.Length() > 1)
            {
                int group_speaker_id = DecodeNumber(packet.SubString(1, 2));
                if (MainForm->animation->interface_controller->group->members.size() > 0)
                {
                    String group_speaker_name = MainForm->animation->interface_controller->group->GetMemberName(group_speaker_id);
                    String group_text = Client::CensorText(MainForm->net_client,
                        packet.SubString(3, packet.Length() - 2));
                    if (!MainForm->animation->social->IsIgnored(group_speaker_name))
                    {
                        if (group_text.Length() >= 1)
                        {
                            MainForm->gfx->sprites->ShowSpeechBalloon(group_text, group_speaker_id, 1);
                            MainForm->animation->interface_controller->chat->AddGroupMessage(ChatIcon_Party, 0,
                                Client::Capitalize(MainForm->net_client, group_speaker_name), group_text, false);
                            MainForm->animation->interface_controller->chat->AddLocalMessage(ChatIcon_Party, 1,
                                Client::Capitalize(MainForm->net_client, group_speaker_name), group_text, false);
                            MainForm->sfx->PlaySfx(18);
                        }
                    }
                }
            }
        }
        if (action == AC_CAST)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            String admin_chat_sender = MainForm->net_client->NextToken();
            String admin_chat_text = Client::CensorText(MainForm->net_client, MainForm->net_client->NextToken());
            if (!MainForm->animation->social->IsIgnored(admin_chat_sender))
            {
                if (admin_chat_text.Length() >= 1)
                {
                    MainForm->sfx->PlaySfx(29);
                    MainForm->animation->interface_controller->chat->AddGroupMessage(ChatIcon_Admin, CHAT_COLOR_ADMIN,
                        Client::Capitalize(MainForm->net_client, admin_chat_sender), admin_chat_text, false);
                }
            }
        }
        if (action == AC_SYSTEM)
        {
            MainForm->sfx->PlaySfx(5);
            MainForm->animation->interface_controller->chat->AddLocalMessage(6, 2,
                MainForm->strings->GetText(GameText_Server), packet, false);
            MainForm->animation->interface_controller->chat->AddGlobalMessage(6, CHAT_COLOR_NOTICE,
                MainForm->strings->GetText(GameText_Server), packet, false, false);
            MainForm->animation->interface_controller->chat->AddSystemMessage(6, 2, packet, false);
        }
        if (action == AC_REQUEST)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            String guild_chat_sender = MainForm->net_client->NextToken();
            String guild_chat_text = Client::CensorText(MainForm->net_client, MainForm->net_client->NextToken());
            if (!MainForm->animation->social->IsIgnored(guild_chat_sender))
            {
                if (guild_chat_text.Length() >= 1)
                {
                    MainForm->sfx->PlaySfx(29);
                    MainForm->animation->interface_controller->chat->AddGroupMessage(ChatIcon_Guild, 1,
                        Client::Capitalize(MainForm->net_client, guild_chat_sender), guild_chat_text, false);
                }
            }
        }
        if (action == AC_LIST)
        {
            if (packet.Length() > 0)
            {
                MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
                for (int i18 = 1; i18 <= 7; i18++)
                {
                    String history_sender = MainForm->net_client->NextToken();
                    String history_text = Client::CensorText(MainForm->net_client, MainForm->net_client->NextToken());
                    if (history_sender != "" && history_text != "")
                    {
                        if (!MainForm->animation->social->IsIgnored(history_sender))
                        {
                            if (history_text.Length() >= 1)
                                MainForm->animation->interface_controller->chat->AddGlobalMessage(ChatIcon_Announce, 0,
                                    Client::Capitalize(MainForm->net_client, history_sender), history_text, false,
                                    true);
                        }
                    }
                    else
                        break;
                }
            }
        }
        if (action == AC_GLOBAL)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            String broadcast_sender = MainForm->net_client->NextToken();
            String broadcast_text = Client::CensorText(MainForm->net_client, MainForm->net_client->NextToken());
            if (!MainForm->animation->social->IsIgnored(broadcast_sender))
            {
                if (broadcast_text.Length() >= 1)
                {
                    MainForm->sfx->PlaySfx(14);
                    MainForm->animation->interface_controller->chat->AddGlobalMessage(ChatIcon_Announce,
                        CHAT_COLOR_NOTICE, Client::Capitalize(MainForm->net_client, broadcast_sender), broadcast_text,
                        false, false);
                    MainForm->animation->interface_controller->chat->AddLocalMessage(ChatIcon_Announce,
                        CHAT_COLOR_NOTICE, Client::Capitalize(MainForm->net_client, broadcast_sender), broadcast_text,
                        false);
                    MainForm->animation->interface_controller->chat->AddGroupMessage(ChatIcon_Announce,
                        CHAT_COLOR_NOTICE, Client::Capitalize(MainForm->net_client, broadcast_sender), broadcast_text,
                        false);
                    MainForm->gfx->sprites->ShowSpeechBalloon(broadcast_text, MainForm->net_client->GetPlayerId(), 0);
                }
            }
        }
        if (action == AC_PLAY)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            String global_sender = MainForm->net_client->NextToken();
            String global_text = Client::CensorText(MainForm->net_client, MainForm->net_client->NextToken());
            if (!MainForm->animation->social->IsIgnored(global_sender))
            {
                if (global_text.Length() >= 1)
                    MainForm->animation->interface_controller->chat->AddGlobalMessage(ChatIcon_Announce, 0,
                        Client::Capitalize(MainForm->net_client, global_sender), global_text, false, false);
            }
        }
        if (action == AC_SET)
        {
            int talker_id = DecodeNumber(packet.SubString(1, 2));
            String talk_text = Client::CensorText(MainForm->net_client, packet.SubString(3, packet.Length() - 2));
            if (talk_text.Length() >= 1)
            {
                if (MainForm->players->HasPlayer(talker_id))
                {
                    String talker_name = MainForm->players->GetName(talker_id);
                    if (!MainForm->animation->social->IsIgnored(talker_name))
                    {
                        MainForm->animation->interface_controller->chat->AddLocalMessage(0, 0,
                            Client::Capitalize(MainForm->net_client, talker_name), talk_text, false);
                        MainForm->gfx->sprites->ShowSpeechBalloon(talk_text, talker_id, 0);
                    }
                }
                else
                {
                    String talker_request = EncodeNumber(this, talker_id, 2);
                    MainForm->net_client->SendPacket(AC_REQUEST, OP_PLAYERRANGE, talker_request);
                }
            }
        }
    }
    if (family == OP_EMOTE && action == AC_SET)
    {
        int emoting_player_id = DecodeNumber(packet.SubString(1, 2));
        int emote_id = DecodeNumber(packet.SubString(3, 1));
        if (emote_id > 0 && emote_id < 15)
            MainForm->players->StartEmote(emoting_player_id, emote_id);
    }
    if (family == OP_SECURITY && action == AC_SET)
    {
        int sequence_base = DecodeNumber(packet.SubString(1, 2));
        int sequence_offset = DecodeNumber(packet.SubString(3, 1));
        int sequence_start = sequence_base - sequence_offset;
        MainForm->net_client->SetSequenceStart(sequence_start);
        MainForm->net_client->sender->SendConnectionPong();
    }
    if (family == OP_REFRESH && action == AC_RESULT)
    {
        MainForm->players->Clear();
        MainForm->npcs->Clear();
        MainForm->map_items->ClearKeepingProtection();
        MainForm->players->ReloadGraphicsLibraries();
        MainForm->gfx->items->ReopenLibrary();
        Sleep(1);
        ParseNearbyEntities(packet);
        MainForm->animation->game->CenterViewOnPlayer(MainForm->net_client->GetPlayerId());
    }
    if (family == OP_INN)
    {
        if (action == AC_REQUEST)
        {
            int sleep_price = DecodeNumber(packet.SubString(1, 4));
            MainForm->animation->game->ShowMessageBox(3, ConfirmAction_InnSleep,
                MainForm->strings->GetText(GameText_Sleep), MainForm->strings->GetText(GameText_GoodNightrestWillCost) +
                IntToStr(sleep_price) + " " + MainForm->item_values->GetName(1));
        }
        if (action == AC_CONFIRM)
        {
            int sleep_gold_left = DecodeNumber(packet.SubString(1, 4));
            MainForm->animation->interface_controller->inventory->SetItemAmount(1, sleep_gold_left);
            MainForm->animation->interface_controller->inventory->MarkItemsDirty();
            MainForm->animation->interface_controller->status->gold = sleep_gold_left;
            MainForm->animation->interface_controller->status->MarkDirty();
            int rested_health = MainForm->animation->interface_controller->status->max_health;
            int rested_mana = MainForm->animation->interface_controller->status->max_mana;
            int rested_stamina = MainForm->animation->interface_controller->status->max_stamina;
            MainForm->animation->interface_controller->status->SetHealth(rested_health);
            MainForm->animation->interface_controller->status->SetMana(rested_mana);
            MainForm->animation->interface_controller->status->SetStamina(rested_stamina);
            MainForm->sfx->PlaySfx(23);
            MainForm->animation->inn->Close();
            MainForm->animation->interface_controller->ShowTimedText("zzzz ..", 40);
        }
        if (action == AC_DELETE)
        {
            int inn_reply_kind = DecodeNumber(packet.SubString(1, 1));
            if (!inn_reply_kind)
                MainForm->animation->game->ShowMessageBox(0, ConfirmAction_None,
                    MainForm->strings->GetText(GameText_RegistrationService2),
                    MainForm->strings->GetText(GameText_YouAreNotCitizen));
            if (inn_reply_kind == 1)
                MainForm->animation->game->ShowMessageBox(0, ConfirmAction_None,
                    MainForm->strings->GetText(GameText_RegistrationService2),
                    MainForm->strings->GetText(GameText_YouAreNoLonger));
        }
        if (action == AC_RESULT)
        {
            int inn_answer_value = DecodeNumber(packet.SubString(1, 1));
            if (inn_answer_value <= 0)
            {
                MainForm->animation->game->ShowMessageBox(0, ConfirmAction_None,
                    MainForm->strings->GetText(GameText_RegistrationService2),
                    MainForm->strings->GetText(GameText_CongratulationsYouHaveBecome));
                MainForm->sfx->PlaySfx(37);
            }
            else
                MainForm->animation->game->ShowMessageBox(0, ConfirmAction_None,
                    MainForm->strings->GetText(GameText_RegistrationService2), IntToStr(inn_answer_value) + " " +
                    MainForm->strings->GetText(GameText_OfYourAnwersWere));
        }
        if (action == AC_OPEN)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            String inn_data = MainForm->net_client->NextToken();
            int inn_request_id = DecodeNumber(inn_data.SubString(1, 3));
            int is_citizen = DecodeNumber(inn_data.SubString(4, 1));
            int inn_session_id = DecodeNumber(inn_data.SubString(5, 2));
            MainForm->animation->inn->questions[0] = MainForm->net_client->NextToken();
            MainForm->animation->inn->questions[1] = MainForm->net_client->NextToken();
            MainForm->animation->inn->questions[2] = MainForm->net_client->NextToken();
            MainForm->net_client->SetSessionId(inn_session_id);
            MainForm->animation->inn->Open(inn_request_id, is_citizen);
        }
    }
    if (family == OP_BANK)
    {
        if (action == AC_RESULT)
        {
            MainForm->sfx->PlaySfx(26);
            int bank_gold_left = DecodeNumber(packet.SubString(1, 4));
            int bank_balance = DecodeNumber(packet.SubString(5, 4));
            MainForm->animation->bank->bank_gold = bank_balance;
            MainForm->animation->interface_controller->inventory->SetItemAmount(1, bank_gold_left);
            MainForm->animation->interface_controller->inventory->MarkItemsDirty();
            MainForm->animation->interface_controller->status->gold = bank_gold_left;
            MainForm->animation->interface_controller->status->MarkDirty();
            MainForm->redraw_needed = true;
        }
        if (action == AC_OPEN)
        {
            int opened_bank_balance = DecodeNumber(packet.SubString(1, 4));
            int bank_request_id = DecodeNumber(packet.SubString(5, 3));
            int opened_locker_level = DecodeNumber(packet.SubString(8, 1));
            MainForm->animation->bank->Open(bank_request_id, opened_bank_balance, opened_locker_level, "");
        }
    }
    if (family == OP_BARBER)
    {
        if (action == AC_OPEN)
        {
            int barber_session_id = DecodeNumber(packet.SubString(1, 4));
            MainForm->animation->barber->Open(barber_session_id);
        }
        if (action == AC_UPDATE)
        {
            int barber_gold_left = DecodeNumber(packet.SubString(1, 4));
            packet.Delete(1, 4);
            MainForm->animation->interface_controller->inventory->SetItemAmount(1, barber_gold_left);
            MainForm->animation->interface_controller->inventory->MarkItemsDirty();
            MainForm->animation->interface_controller->status->gold = barber_gold_left;
            MainForm->animation->interface_controller->status->MarkDirty();
            ApplyAppearanceChange(packet);
            MainForm->sfx->PlaySfx(26);
            MainForm->animation->barber->Close();
        }
    }
    if (family == OP_LAW)
    {
        if (action == AC_RESULT)
        {
            int law_reply = DecodeNumber(packet.SubString(1, 2));
            if (law_reply == MarriageReply_AlreadyMarried)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_AlreadyMarried, false),
                    MainForm->strings->GetDialogText(DialogText_AlreadyMarried, true));
                MainForm->edits->caret_enabled = false;
            }
            if (law_reply == MarriageReply_NotMarried)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_NotMarried, false),
                    MainForm->strings->GetDialogText(DialogText_NotMarried, true));
                MainForm->edits->caret_enabled = false;
            }
            if (law_reply == MarriageReply_Success)
            {
                int law_gold_left = DecodeNumber(packet.SubString(3, 3));
                MainForm->animation->interface_controller->inventory->SetItemAmount(1, law_gold_left);
                MainForm->animation->interface_controller->inventory->MarkItemsDirty();
                MainForm->animation->interface_controller->status->gold = law_gold_left;
                MainForm->animation->interface_controller->status->MarkDirty();
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_RegistrationComplete, false),
                    MainForm->strings->GetDialogText(DialogText_RegistrationComplete, true));
                MainForm->edits->caret_enabled = false;
            }
            if (law_reply == MarriageReply_NotEnoughGold)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_WarningEnough, false),
                    MainForm->strings->GetDialogText(DialogText_WarningEnough, true) + " " +
                    LowerCase(MainForm->item_values->GetName(1)));
                MainForm->edits->caret_enabled = false;
            }
            if (law_reply == MarriageReply_WrongName)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_PartnerNotMatch, false),
                    MainForm->strings->GetDialogText(DialogText_PartnerNotMatch, true));
                MainForm->edits->caret_enabled = false;
            }
            if (law_reply == MarriageReply_ServiceBusy)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_RegistrationBusy, false),
                    MainForm->strings->GetDialogText(DialogText_RegistrationBusy, true));
                MainForm->edits->caret_enabled = false;
            }
            if (law_reply == MarriageReply_DivorceNotification)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_Divorce, false),
                    MainForm->strings->GetDialogText(DialogText_Divorce, true));
                MainForm->edits->caret_enabled = false;
            }
        }
        if (action == AC_OPEN)
        {
            int law_request_id = DecodeNumber(packet.SubString(1, 3));
            MainForm->animation->law->Open(law_request_id);
        }
    }
    if (family == OP_PRIEST)
    {
        if (action == AC_RESULT)
        {
            int priest_reply = DecodeNumber(packet.SubString(1, 2));
            if (priest_reply == PriestReply_NotDressed)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_CeremonyNoticeNeed, false),
                    MainForm->strings->GetDialogText(DialogText_CeremonyNoticeNeed, true));
                MainForm->edits->caret_enabled = false;
            }
            if (priest_reply == PriestReply_LowLevel)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_CeremonyNoticeUnexperienced, false),
                    MainForm->strings->GetDialogText(DialogText_CeremonyNoticeUnexperienced, true));
                MainForm->edits->caret_enabled = false;
            }
            if (priest_reply == PriestReply_PartnerNotPresent)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_CeremonyNoticeNeeds, false),
                    MainForm->strings->GetDialogText(DialogText_CeremonyNoticeNeeds, true));
                MainForm->edits->caret_enabled = false;
            }
            if (priest_reply == PriestReply_PartnerNotDressed)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_CeremonyNoticePartnerProperly, false),
                    MainForm->strings->GetDialogText(DialogText_CeremonyNoticePartnerProperly, true));
                MainForm->edits->caret_enabled = false;
            }
            if (priest_reply == PriestReply_Busy)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_CeremonyNoticePriest, false),
                    MainForm->strings->GetDialogText(DialogText_CeremonyNoticePriest, true));
                MainForm->edits->caret_enabled = false;
            }
            if (priest_reply == PriestReply_DoYou)
            {
                MainForm->animation->game->ShowMessageBox(3, ConfirmAction_ConfirmWeddingVow,
                    MainForm->strings->GetDialogText(DialogText_CeremonyNoticeAccept, false),
                    MainForm->strings->GetDialogText(DialogText_CeremonyNoticeAccept, true));
                MainForm->edits->caret_enabled = false;
            }
            if (priest_reply == PriestReply_PartnerAlreadyMarried)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_CeremonyNoticePerson, false),
                    MainForm->strings->GetDialogText(DialogText_CeremonyNoticePerson, true));
                MainForm->edits->caret_enabled = false;
            }
            if (priest_reply == PriestReply_NoPermission)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_NoPermission, false),
                    MainForm->strings->GetDialogText(DialogText_NoPermission, true));
                MainForm->edits->caret_enabled = false;
            }
        }
        if (action == AC_OPEN)
        {
            int priest_request_id = DecodeNumber(packet.SubString(1, 4));
            Ctrl_priest_SetRequestId((int *)MainForm->animation->priest, priest_request_id);
            MainForm->animation->game->ShowLargeMessageBox(3, LargeBoxKind_WeddingRequest,
                MainForm->strings->GetDialogText(DialogText_WeddingCeremony, false),
                MainForm->strings->GetDialogText(DialogText_WeddingCeremony, true));
        }
        if (action == AC_REQUEST)
        {
            Ctrl_priest_SetRequestId((int *)MainForm->animation->priest, DecodeNumber(packet.SubString(1, 2)));
            String priest_partner_name = Client::Capitalize(MainForm->net_client,
                packet.SubString(3, packet.Length() - 2));
            MainForm->animation->game->ShowMessageBox(3, ConfirmAction_AcceptWeddingRequest,
                MainForm->strings->GetDialogText(DialogText_CeremonyNoticeNeeds, false),
                priest_partner_name + MainForm->strings->GetText(GameText_IsAskingYouTo));
            MainForm->edits->caret_enabled = false;
        }
    }
    if (family == OP_GUILD)
    {
        if (action == AC_REMOVE)
        {
            MainForm->animation->interface_controller->status->guild_tag = "";
            MainForm->animation->interface_controller->status->guild_name = "";
            MainForm->animation->interface_controller->status->guild_rank_name = "";
            MainForm->animation->interface_controller->status->guild_rank = GUILD_RANK_LOWEST;
            MainForm->animation->interface_controller->status->MarkDirty();
        }
        if (action == AC_SELL && MainForm->animation->guild->is_visible)
        {
            int guild_bank_balance = DecodeNumber(packet);
            MainForm->animation->guild->guild_bank_gold = guild_bank_balance;
            MainForm->animation->guild->page = GuildPage_Bank;
            MainForm->animation->guild->ClearText();
            MainForm->animation->guild->AddText(MainForm->strings->GetText(GameText_BankStatus) + " " +
                IntToStr(guild_bank_balance), true);
            MainForm->animation->guild->AddText(MainForm->strings->GetText(GameText_AllGuildMembersIncluding), true);
            MainForm->animation->guild->AddText(MainForm->strings->GetText(GameText_BankAccountCanBe), true);
            MainForm->animation->guild->AddText(MainForm->strings->GetText(GameText_ClickHereToDeposit), true);
        }
        if (action == AC_BUY)
        {
            if (MainForm->animation->guild->is_visible && MainForm->animation->guild->page == GuildPage_Bank)
                MainForm->animation->guild->page = GuildPage_Main;
            MainForm->sfx->PlaySfx(26);
            int guild_gold_left = DecodeNumber(packet.SubString(1, 4));
            MainForm->animation->interface_controller->inventory->SetItemAmount(1, guild_gold_left);
            MainForm->animation->interface_controller->inventory->MarkItemsDirty();
            MainForm->animation->interface_controller->status->gold = guild_gold_left;
            MainForm->animation->interface_controller->status->MarkDirty();
            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                MainForm->strings->GetDialogText(DialogText_DepositNew, false),
                MainForm->strings->GetDialogText(DialogText_DepositNew, true) + " " +
                IntToStr(MainForm->animation->guild->guild_bank_gold) + " " + MainForm->item_values->GetName(1));
            MainForm->redraw_needed = true;
        }
        if (action == AC_GET && MainForm->animation->guild->is_visible)
        {
            MainForm->animation->guild->page = GuildPage_Description;
            MainForm->animation->guild->ClearText();
            MainForm->animation->guild->AddText(MainForm->strings->GetText(GameText_CurrentDescription), false);
            MainForm->animation->guild->AddText(packet, true);
            MainForm->animation->guild->AddText(MainForm->strings->GetText(GameText_ClickHereToChange), false);
        }
        if (action == AC_RANK && MainForm->animation->guild->is_visible)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            MainForm->animation->game->OpenInputForm(FormKind_GuildRankNames, 9, 3, 16,
                MainForm->strings->GetText(GameText_Ranking), MainForm->strings->GetText(GameText_EnterYourRankings));
            for (int i19 = 1; i19 <= 9; i19++)
                MainForm->animation->game->AddInputFormRow(MainForm->strings->GetText(GameText_Rank) + " " +
                    IntToStr(i19), MainForm->net_client->NextToken(), 3, 16, 0, 0);
        }
        if (action == AC_PRIVATE)
        {
            if (MainForm->animation->guild->is_visible)
            {
                MainForm->animation->guild->page = GuildPage_MemberList;
                MainForm->animation->guild->ClearText();
                MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
                int guild_member_count = DecodeNumber(MainForm->net_client->NextToken());
                for (int i20 = 0; i20 < guild_member_count && i20 < 100; i20++)
                {
                    int member_rank = DecodeNumber(MainForm->net_client->NextToken());
                    String member_name = Client::Capitalize(MainForm->net_client, MainForm->net_client->NextToken());
                    String member_rank_name = Client::Capitalize(MainForm->net_client,
                        MainForm->net_client->NextToken());
                    MainForm->animation->guild->lines->Add(IntToStr(member_rank) + "  " + member_name);
                    MainForm->animation->guild->member_rank_titles->Add(member_rank_name);
                }
                MainForm->redraw_needed = true;
            }
        }
        if (action == AC_PUBLIC)
        {
            if (MainForm->animation->guild->is_visible)
            {
                MainForm->animation->guild->page = GuildPage_Info;
                MainForm->animation->guild->ClearText();
                MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
                String guild_title = MainForm->net_client->NextToken();
                guild_title = guild_title + " [";
                guild_title = guild_title + MainForm->net_client->NextToken();
                guild_title = guild_title + "]";
                MainForm->animation->guild->AddText(guild_title, true);
                MainForm->animation->guild->AddText(MainForm->strings->GetText(GameText_SignupDate), false);
                MainForm->animation->guild->AddText(MainForm->net_client->NextToken(), true);
                MainForm->animation->guild->AddText(MainForm->strings->GetText(GameText_GuildDescription), false);
                MainForm->animation->guild->AddText(MainForm->net_client->NextToken(), true);
                MainForm->animation->guild->AddText(MainForm->strings->GetText(GameText_BankStatus), false);
                MainForm->animation->guild->AddText(MainForm->net_client->NextToken(), true);
                MainForm->animation->guild->AddText(MainForm->strings->GetText(GameText_RankingSystem), false);
                int rank_index = 1;
                for (int m = 1; m <= 9; m++)
                {
                    String listed_rank_name = MainForm->net_client->NextToken();
                    if (listed_rank_name.Length() > 3)
                    {
                        MainForm->animation->guild->AddText(IntToStr(rank_index) + "  " + listed_rank_name, false);
                        rank_index++;
                    }
                }
                MainForm->animation->guild->AddText("", false);
                String guild_staff_data = MainForm->net_client->NextToken();
                if (guild_staff_data.Length() == 2)
                {
                    int guild_staff_count = DecodeNumber(guild_staff_data);
                    char header_added = 0;
                    if (guild_staff_count > 0)
                    {
                        MainForm->animation->guild->AddText(MainForm->strings->GetText(GameText_LeaderS), false);
                        for (int n = 0; n < guild_staff_count && n < 20; n++)
                        {
                            int staff_kind = DecodeNumber(MainForm->net_client->NextToken());
                            String staff_name = MainForm->net_client->NextToken();
                            if (staff_kind == 2 && !header_added)
                            {
                                header_added = 1;
                                MainForm->animation->guild->AddText("", false);
                                MainForm->animation->guild->AddText(MainForm->strings->GetText(GameText_RecruiterS),
                                    false);
                            }
                            if (staff_name.Length() > 0)
                                MainForm->animation->guild->AddText(Client::Capitalize(MainForm->net_client,
                                    staff_name), false);
                        }
                    }
                    MainForm->animation->guild->AddText("", false);
                }
                MainForm->redraw_needed = true;
            }
        }
        if (action == AC_RESULT)
        {
            int guild_reply = DecodeNumber(packet.SubString(1, 2));
            if (guild_reply == GuildReply_Busy)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_NpcBusy, false),
                    MainForm->strings->GetDialogText(DialogText_NpcBusy, true));
            if (guild_reply == GuildReply_NotApproved)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_NotApprovedGuild, false),
                    MainForm->strings->GetDialogText(DialogText_NotApprovedGuild, true));
            if (guild_reply == GuildReply_AlreadyMember)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_AlreadyInGuild, false),
                    MainForm->strings->GetDialogText(DialogText_AlreadyInGuild, true));
            if (guild_reply == GuildReply_NoCandidates)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_NoCandidates, false),
                    MainForm->strings->GetDialogText(DialogText_NoCandidates, true));
            if (guild_reply == GuildReply_Exists)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_AlreadyExistGuild, false),
                    MainForm->strings->GetDialogText(DialogText_AlreadyExistGuild, true));
            if (guild_reply == GuildReply_CreateBegin)
            {
                MainForm->animation->game->CloseInputForm();
                MainForm->animation->guild->ClearText();
                MainForm->animation->guild->AddText(MainForm->strings->GetText(GameText_WaitForAllMembers), true);
                String own_name = MainForm->players->GetName(MainForm->net_client->GetPlayerId());
                own_name = Client::Capitalize(MainForm->net_client, own_name);
                MainForm->animation->guild->AddText(own_name, false);
            }
            if (guild_reply == GuildReply_CreateAddConfirm)
            {
                String confirm_name = packet.SubString(3, packet.Length() - 2);
                confirm_name = Client::Capitalize(MainForm->net_client, confirm_name);
                MainForm->animation->guild->AddText(confirm_name, false);
                MainForm->sfx->PlaySfx(5);
                MainForm->animation->game->ShowMessageBox(3, ConfirmAction_CreateGuild,
                    MainForm->strings->GetDialogText(DialogText_Confirmation, false),
                    MainForm->strings->GetDialogText(DialogText_Confirmation, true) + " " +
                    MainForm->item_values->GetName(1));
            }
            if (guild_reply == GuildReply_CreateAdd)
            {
                String appended_name = packet.SubString(3, packet.Length() - 2);
                appended_name = Client::Capitalize(MainForm->net_client, appended_name);
                MainForm->animation->guild->AddText(appended_name, false);
            }
            if (guild_reply == GuildReply_RecruiterOffline)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_RecruiterNotFoundNowhere, false),
                    MainForm->strings->GetDialogText(DialogText_RecruiterNotFoundNowhere, true));
            if (guild_reply == GuildReply_RecruiterNotHere)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_RecruiterNotFoundNeeds, false),
                    MainForm->strings->GetDialogText(DialogText_RecruiterNotFoundNeeds, true));
            if (guild_reply == GuildReply_RecruiterWrongGuild)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_WrongRecruiter, false),
                    MainForm->strings->GetDialogText(DialogText_WrongRecruiter, true));
            if (guild_reply == GuildReply_NotRecruiter)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_NotRecruiter, false),
                    MainForm->strings->GetDialogText(DialogText_NotRecruiter, true));
            if (guild_reply == GuildReply_JoinRequest && !MainForm->animation->game->large_box_visible)
            {
                int join_requester_id = DecodeNumber(packet.SubString(3, 2));
                String join_requester_name = packet.SubString(5, packet.Length() - 4);
                MainForm->animation->guild->request_player_id = join_requester_id;
                MainForm->sfx->PlaySfx(5);
                String join_request_text = join_requester_name + " ";
                join_request_text = join_request_text + MainForm->strings->GetDialogText(DialogText_JoinRequest, true) +
                    " ";
                join_request_text = join_request_text + MainForm->strings->GetText(GameText_YourGuildAccountWill) + " ";
                join_request_text = join_request_text + MainForm->item_values->GetName(1) + ". ";
                join_request_text = join_request_text + MainForm->strings->GetText(GameText_ConsiderCarefullyWhatKind) +
                    " ";
                join_request_text = join_request_text + MainForm->strings->GetText(GameText_DoYouAccept);
                MainForm->animation->game->ShowLargeMessageBox(3, LargeBoxKind_AcceptGuildJoinRequest,
                    MainForm->strings->GetDialogText(DialogText_JoinRequest, false), join_request_text);
            }
            if (guild_reply == GuildReply_Unknown14)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_RefusedRanking, false),
                    MainForm->strings->GetDialogText(DialogText_RecruiterNotFoundNeeds, true));
            if (guild_reply == GuildReply_Unknown15)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_AccountLow, false),
                    MainForm->strings->GetDialogText(DialogText_AccountLow, true));
            if (guild_reply == GuildReply_Accepted)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_MemberAccepted, false),
                    MainForm->strings->GetDialogText(DialogText_MemberAccepted, true));
            if (guild_reply == GuildReply_NotFound)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_NoResult, false),
                    MainForm->strings->GetDialogText(DialogText_NoResult, true));
            if (guild_reply == GuildReply_Updated)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_AcceptedDetails, false),
                    MainForm->strings->GetDialogText(DialogText_AcceptedDetails, true));
                MainForm->animation->guild->ClearText();
                MainForm->animation->guild->page = GuildPage_Manage;
                MainForm->redraw_needed = true;
            }
            if (guild_reply == GuildReply_RanksUpdated)
            {
                MainForm->animation->game->CloseInputForm();
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_AcceptedDetails, false),
                    MainForm->strings->GetDialogText(DialogText_AcceptedDetails, true));
            }
            if (guild_reply == GuildReply_RemoveNotMember)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_CouldNotRemoveLeader, false),
                    MainForm->animation->guild->target_member_name + " " +
                    MainForm->strings->GetDialogText(DialogText_CouldNotRemoveLeader, true));
            if (guild_reply == GuildReply_RemoveLeader)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_CouldNotRemoveMember, false),
                    MainForm->animation->guild->target_member_name + " " +
                    MainForm->strings->GetDialogText(DialogText_CouldNotRemoveMember, true));
            if (guild_reply == GuildReply_Removed)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_MemberRemoved, false),
                    MainForm->animation->guild->target_member_name + " " +
                    MainForm->strings->GetDialogText(DialogText_MemberRemoved, true));
            if (guild_reply == GuildReply_RankingLeader)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetText(GameText_Ranking), MainForm->animation->guild->target_member_name + " " +
                    MainForm->strings->GetDialogText(DialogText_CouldNotRemoveLeader, true));
            if (guild_reply == GuildReply_RankingNotMember)
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetText(GameText_Ranking), MainForm->animation->guild->target_member_name + " " +
                    MainForm->strings->GetDialogText(DialogText_CouldNotRemoveMember, true));
        }
        if (action == AC_CONFIRM)
        {
            int guild_rank_number = DecodeNumber(packet.SubString(1, 1));
            String guild_rank_name = packet.SubString(2, packet.Length() - 1);
            MainForm->animation->interface_controller->status->guild_rank_name = guild_rank_name;
            MainForm->animation->interface_controller->status->guild_rank = guild_rank_number;
            MainForm->animation->interface_controller->status->MarkDirty();
        }
        if (action == AC_UPDATE)
        {
            int guild_leave_unused_word = DecodeNumber(packet.SubString(1, 2));
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            MainForm->net_client->NextToken();
            MainForm->animation->interface_controller->status->guild_tag = MainForm->net_client->NextToken();
            MainForm->animation->interface_controller->status->guild_name = MainForm->net_client->NextToken();
            MainForm->animation->interface_controller->status->guild_rank_name = MainForm->net_client->NextToken();
            MainForm->animation->interface_controller->status->guild_rank = GUILD_RANK_LOWEST;
            MainForm->animation->interface_controller->status->MarkDirty();
            MainForm->sfx->PlaySfx(18);
            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                MainForm->strings->GetDialogText(DialogText_Accepted, false),
                MainForm->strings->GetDialogText(DialogText_Accepted, true));
        }
        if (action == AC_NEW)
        {
            int guild_founder_id = DecodeNumber(packet.SubString(1, 2));
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            MainForm->net_client->NextToken();
            MainForm->animation->interface_controller->status->guild_tag = MainForm->net_client->NextToken();
            MainForm->animation->interface_controller->status->guild_name = MainForm->net_client->NextToken();
            MainForm->animation->interface_controller->status->guild_rank = GUILD_RANK_LOWEST;
            if (MainForm->net_client->GetPlayerId() == guild_founder_id)
            {
                MainForm->animation->interface_controller->status->guild_rank_name = MainForm->net_client->NextToken();
                MainForm->animation->interface_controller->status->guild_rank = 1;
                int founding_gold_left = DecodeNumber(MainForm->net_client->NextToken());
                MainForm->animation->interface_controller->inventory->SetItemAmount(1, founding_gold_left);
                MainForm->animation->interface_controller->inventory->MarkItemsDirty();
                MainForm->animation->interface_controller->status->gold = founding_gold_left;
                MainForm->animation->interface_controller->status->MarkDirty();
            }
            else
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_Accepted, false),
                    MainForm->strings->GetDialogText(DialogText_Accepted, true));
            MainForm->animation->interface_controller->status->MarkDirty();
            MainForm->sfx->PlaySfx(18);
        }
        if (action == AC_REQUEST && !MainForm->animation->game->large_box_visible)
        {
            int guild_inviter_id = DecodeNumber(packet.SubString(1, 2));
            String guild_inviter_name = packet.SubString(3, packet.Length() - 2);
            guild_inviter_name = Client::Capitalize(MainForm->net_client, guild_inviter_name);
            MainForm->animation->guild->request_player_id = guild_inviter_id;
            if (MainForm->net_client->GetPlayerId() != guild_inviter_id &&
                !MainForm->animation->interface_controller->status->IsInGuild())
            {
                MainForm->sfx->PlaySfx(5);
                String guild_invite_text = guild_inviter_name;
                guild_invite_text = guild_invite_text + " " +
                    MainForm->strings->GetDialogText(DialogText_GuildInvitation, true);
                guild_invite_text = guild_invite_text + " " + MainForm->strings->GetText(GameText_JoiningGuildIsFree);
                guild_invite_text = guild_invite_text + " " +
                    MainForm->strings->GetText(GameText_ConsiderCarefullyWhatGuild);
                guild_invite_text = guild_invite_text + " " + MainForm->strings->GetText(GameText_DoYouAccept);
                MainForm->animation->game->ShowLargeMessageBox(3, LargeBoxKind_AcceptGuildInvite,
                    MainForm->strings->GetDialogText(DialogText_GuildInvitation, false), guild_invite_text);
            }
        }
        if (action == AC_OPEN)
        {
            int guild_request_id = DecodeNumber(packet.SubString(1, 3));
            MainForm->animation->guild->Open(MainForm->strings->GetText(GameText_GuildMaster), guild_request_id);
        }
    }
    if (family == OP_MSGBOARD)
    {
        if (action == AC_OPEN)
        {
            MainForm->animation->message_board->ResetScroll();
            MainForm->animation->message_board->ClearPosts();
            MainForm->animation->message_board->LoadPosts(packet);
            MainForm->animation->message_board->Open(-1);
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                MainForm->strings->GetText(GameText_TownBoardIsNow));
        }
        if (action == AC_SET)
            MainForm->animation->message_board->SetPostBody(packet);
    }
    if (family == OP_SHOP)
    {
        if (action == AC_NEW)
        {
            int crafted_item_id = DecodeNumber(packet.SubString(1, 2));
            int craft_weight = DecodeNumber(packet.SubString(3, 1));
            int craft_max_weight = DecodeNumber(packet.SubString(4, 1));
            MainForm->animation->interface_controller->status->weight = craft_weight;
            MainForm->animation->interface_controller->status->max_weight = craft_max_weight;
            MainForm->animation->interface_controller->status->MarkDirty();
            if (packet.Length() > 9)
            {
                int used_first_ingredient_id = DecodeNumber(packet.SubString(5, 2));
                int first_ingredient_left = DecodeNumber(packet.SubString(7, 4));
                MainForm->animation->interface_controller->inventory->SetItemAmount(used_first_ingredient_id,
                    first_ingredient_left);
                if (used_first_ingredient_id == 1)
                    MainForm->animation->interface_controller->status->gold = first_ingredient_left;
            }
            if (packet.Length() > 15)
            {
                int used_second_ingredient_id = DecodeNumber(packet.SubString(11, 2));
                int second_ingredient_left = DecodeNumber(packet.SubString(13, 4));
                MainForm->animation->interface_controller->inventory->SetItemAmount(used_second_ingredient_id,
                    second_ingredient_left);
                if (used_second_ingredient_id == 1)
                    MainForm->animation->interface_controller->status->gold = second_ingredient_left;
            }
            if (packet.Length() > 21)
            {
                int used_third_ingredient_id = DecodeNumber(packet.SubString(17, 2));
                int third_ingredient_left = DecodeNumber(packet.SubString(19, 4));
                MainForm->animation->interface_controller->inventory->SetItemAmount(used_third_ingredient_id,
                    third_ingredient_left);
                if (used_third_ingredient_id == 1)
                    MainForm->animation->interface_controller->status->gold = third_ingredient_left;
            }
            if (packet.Length() > 27)
            {
                int used_fourth_ingredient_id = DecodeNumber(packet.SubString(23, 2));
                int fourth_ingredient_left = DecodeNumber(packet.SubString(25, 4));
                MainForm->animation->interface_controller->inventory->SetItemAmount(used_fourth_ingredient_id,
                    fourth_ingredient_left);
                if (used_fourth_ingredient_id == 1)
                    MainForm->animation->interface_controller->status->gold = fourth_ingredient_left;
            }
            MainForm->animation->interface_controller->inventory->AddItem(crafted_item_id, 1);
            if (crafted_item_id == 1)
                MainForm->animation->interface_controller->status->gold++;
            MainForm->animation->interface_controller->inventory->MarkItemsDirty();
            MainForm->redraw_needed = true;
            MainForm->sfx->PlaySfx(SfxSound_ShopCreate);
        }
        if (action == AC_SELL)
        {
            MainForm->sfx->PlaySfx(26);
            int sold_item_left = DecodeNumber(packet.SubString(1, 4));
            int sold_item_id = DecodeNumber(packet.SubString(5, 2));
            int sell_gold = DecodeNumber(packet.SubString(7, 4));
            int sell_weight = DecodeNumber(packet.SubString(11, 1));
            int sell_max_weight = DecodeNumber(packet.SubString(12, 1));
            MainForm->animation->interface_controller->inventory->SetItemAmount(1, sell_gold);
            MainForm->animation->interface_controller->inventory->SetItemAmount(sold_item_id, sold_item_left);
            MainForm->animation->interface_controller->inventory->MarkItemsDirty();
            MainForm->animation->interface_controller->status->weight = sell_weight;
            MainForm->animation->interface_controller->status->max_weight = sell_max_weight;
            MainForm->animation->interface_controller->status->gold = sell_gold;
            MainForm->animation->interface_controller->status->MarkDirty();
            String sell_message = MainForm->strings->GetText(GameText_YouSold) + " " +
                IntToStr(MainForm->animation->shop->selected_amount) + " " +
                MainForm->item_values->GetName(sold_item_id);
            MainForm->animation->interface_controller->chat->AddSystemMessage(16, 0, sell_message, true);
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, sell_message);
            MainForm->redraw_needed = true;
        }
        if (action == AC_BUY)
        {
            MainForm->sfx->PlaySfx(26);
            int buy_gold_left = DecodeNumber(packet.SubString(1, 4));
            int bought_item_id = DecodeNumber(packet.SubString(5, 2));
            int bought_amount = DecodeNumber(packet.SubString(7, 4));
            int buy_weight = DecodeNumber(packet.SubString(11, 1));
            int buy_max_weight = DecodeNumber(packet.SubString(12, 1));
            MainForm->animation->interface_controller->inventory->AddItem(bought_item_id, bought_amount);
            MainForm->animation->interface_controller->inventory->SetItemAmount(1, buy_gold_left);
            MainForm->animation->interface_controller->inventory->MarkItemsDirty();
            MainForm->animation->interface_controller->status->weight = buy_weight;
            MainForm->animation->interface_controller->status->max_weight = buy_max_weight;
            MainForm->animation->interface_controller->status->gold = buy_gold_left;
            MainForm->animation->interface_controller->status->MarkDirty();
            String buy_message = MainForm->strings->GetText(GameText_YouBought) + " " +
                IntToStr(MainForm->animation->shop->selected_amount) + " " +
                MainForm->item_values->GetName(bought_item_id);
            MainForm->animation->interface_controller->chat->AddSystemMessage(16, 0, buy_message, true);
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information, buy_message);
            MainForm->redraw_needed = true;
        }
        if (action == AC_OPEN)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            String shop_header = MainForm->net_client->NextToken();
            String shop_trade_data = MainForm->net_client->NextToken();
            String shop_craft_data = MainForm->net_client->NextToken();
            int shop_request_id = DecodeNumber(shop_header.SubString(1, 2));
            String shop_name = shop_header.SubString(3, shop_header.Length() - 2);
            MainForm->animation->shop->Open(shop_name, shop_request_id);
            // 9 bytes per item: id, buy price, sell price, amount; a price of 0 is not offered.
            for (ii = 0; 9 * ii + 9 <= shop_trade_data.Length(); ii++)
            {
                int trade_item_id = DecodeNumber(shop_trade_data.SubString(9 * ii + 1, 2));
                int buy_price = DecodeNumber(shop_trade_data.SubString(9 * ii + 3, 3));
                int sell_price = DecodeNumber(shop_trade_data.SubString(9 * ii + 6, 3));
                int max_amount = DecodeNumber(shop_trade_data.SubString(9 * ii + 9, 1));
                if (buy_price > 0)
                    MainForm->animation->shop->AddTradeItem(1, trade_item_id, trade_item_id, buy_price, max_amount);
                if (sell_price > 0)
                    MainForm->animation->shop->AddTradeItem(2, trade_item_id, trade_item_id, sell_price, max_amount);
            }
            // 14 bytes per craft: the item, then four ingredients with their amounts.
            for (int jj = 0; 14 * jj + 14 <= shop_craft_data.Length(); jj++)
            {
                int craft_item_id = DecodeNumber(shop_craft_data.SubString(14 * jj + 1, 2));
                int first_ingredient_id = DecodeNumber(shop_craft_data.SubString(14 * jj + 3, 2));
                int first_ingredient_amount = DecodeNumber(shop_craft_data.SubString(14 * jj + 5, 1));
                int second_ingredient_id = DecodeNumber(shop_craft_data.SubString(14 * jj + 6, 2));
                int second_ingredient_amount = DecodeNumber(shop_craft_data.SubString(14 * jj + 8, 1));
                int third_ingredient_id = DecodeNumber(shop_craft_data.SubString(14 * jj + 9, 2));
                int third_ingredient_amount = DecodeNumber(shop_craft_data.SubString(14 * jj + 11, 1));
                int fourth_ingredient_id = DecodeNumber(shop_craft_data.SubString(14 * jj + 12, 2));
                int fourth_ingredient_amount = DecodeNumber(shop_craft_data.SubString(14 * jj + 14, 1));
                MainForm->animation->shop->AddCraftItem(craft_item_id, first_ingredient_id, first_ingredient_amount,
                    second_ingredient_id, second_ingredient_amount, third_ingredient_id, third_ingredient_amount,
                    fourth_ingredient_id, fourth_ingredient_amount);
            }
        }
    }
    if (family == OP_TRADE)
    {
        if (action == AC_DO)
        {
            MainForm->animation->trade->own_status_text = MainForm->strings->GetText(GameText_Agree);
            MainForm->animation->trade->partner_status_text = MainForm->strings->GetText(GameText_Agree);
            MainForm->animation->trade->trade_locked = true;
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            String final_first_offer = MainForm->net_client->NextToken();
            String final_second_offer = MainForm->net_client->NextToken();
            if (final_first_offer.Length() > 1)
            {
                if (final_second_offer.Length() > 1)
                {
                    {
                        TradeController *done_trade_window = MainForm->animation->trade;
                        done_trade_window->partner_items.clear();
                        MainForm->animation->trade->own_items.clear();
                        MainForm->animation->trade->ResetScroll();
                    }
                    int final_first_owner = DecodeNumber(final_first_offer.SubString(1, 2));
                    int final_second_owner = DecodeNumber(final_second_offer.SubString(1, 2));
                    final_first_offer.Delete(1, 2);
                    final_second_offer.Delete(1, 2);
                    MainForm->animation->interface_controller->inventory->RestoreLayoutBackup();
                    if (MainForm->net_client->GetPlayerId() != final_first_owner)
                    {
                        for (int nn = 0; nn < final_second_offer.Length() / 6; nn++)
                        {
                            int own_traded_item_id = DecodeNumber(final_second_offer.SubString(6 * nn + 1, 2));
                            int own_traded_amount = DecodeNumber(final_second_offer.SubString(6 * nn + 3, 4));
                            if (own_traded_item_id == 1)
                                MainForm->animation->interface_controller->status->gold -= own_traded_amount;
                            MainForm->animation->interface_controller->inventory->RemoveItem(own_traded_item_id,
                                own_traded_amount);
                        }
                        for (int i1 = 0; i1 < final_first_offer.Length() / 6; i1++)
                        {
                            int partner_traded_item_id = DecodeNumber(final_first_offer.SubString(6 * i1 + 1, 2));
                            int partner_traded_amount = DecodeNumber(final_first_offer.SubString(6 * i1 + 3, 4));
                            if (partner_traded_item_id == 1)
                                MainForm->animation->interface_controller->status->gold += partner_traded_amount;
                            MainForm->animation->interface_controller->inventory->AddItem(partner_traded_item_id,
                                partner_traded_amount);
                        }
                    }
                    else
                    {
                        MainForm->animation->interface_controller->inventory->input_enabled = false;
                        for (int kk = 0; kk < final_first_offer.Length() / 6; kk++)
                        {
                            int own_offer_item_id = DecodeNumber(final_first_offer.SubString(6 * kk + 1, 2));
                            int own_offer_amount = DecodeNumber(final_first_offer.SubString(6 * kk + 3, 4));
                            if (own_offer_item_id == 1)
                                MainForm->animation->interface_controller->status->gold -= own_offer_amount;
                            MainForm->animation->interface_controller->inventory->RemoveItem(own_offer_item_id,
                                own_offer_amount);
                        }
                        for (int mm = 0; mm < final_second_offer.Length() / 6; mm++)
                        {
                            int partner_offer_item_id = DecodeNumber(final_second_offer.SubString(6 * mm + 1, 2));
                            int partner_offer_amount = DecodeNumber(final_second_offer.SubString(6 * mm + 3, 4));
                            if (partner_offer_item_id == 1)
                                MainForm->animation->interface_controller->status->gold += partner_offer_amount;
                            MainForm->animation->interface_controller->inventory->AddItem(partner_offer_item_id,
                                partner_offer_amount);
                        }
                        MainForm->animation->interface_controller->inventory->input_enabled = true;
                    }
                    MainForm->animation->trade->Close();
                    MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                        MainForm->strings->GetDialogText(DialogText_TradeComplete, false),
                        MainForm->strings->GetDialogText(DialogText_TradeComplete, true));
                    MainForm->edits->caret_enabled = false;
                    MainForm->animation->interface_controller->inventory->MarkItemsDirty();
                    MainForm->redraw_needed = true;
                }
            }
            MainForm->animation->interface_controller->status->weight = MainForm->animation->interface_controller->inventory->GetTotalWeight();
            MainForm->animation->interface_controller->status->MarkDirty();
        }
        if (action == AC_RESULT || action == AC_CAST)
        {
            MainForm->animation->trade->SetAcceptDelay(TRADE_ACCEPT_DELAY_TICKS);
            if (MainForm->animation->trade->trade_locked || action == AC_CAST)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_TradeAborted, false),
                    MainForm->strings->GetDialogText(DialogText_TradeAborted, true));
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                    MainForm->strings->GetText(GameText_OtherPlayerChangedHis));
            }
            MainForm->animation->trade->own_status_text = MainForm->strings->GetText(GameText_Trading);
            MainForm->animation->trade->partner_status_text = MainForm->strings->GetText(GameText_Trading);
            MainForm->animation->trade->trade_locked = false;
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            // Both offers start with their owner's id; 6 bytes per item: id and amount.
            String first_offer_list = MainForm->net_client->NextToken();
            String second_offer_list = MainForm->net_client->NextToken();
            int old_offer_count = MainForm->animation->trade->partner_items.size();
            if (first_offer_list.Length() > 1)
            {
                if (second_offer_list.Length() > 1)
                {
                    {
                        TradeController *updated_trade_window = MainForm->animation->trade;
                        updated_trade_window->partner_items.clear();
                        MainForm->animation->trade->own_items.clear();
                        MainForm->animation->trade->ResetScroll();
                    }
                    int first_offer_owner = DecodeNumber(first_offer_list.SubString(1, 2));
                    int second_offer_owner = DecodeNumber(second_offer_list.SubString(1, 2));
                    first_offer_list.Delete(1, 2);
                    second_offer_list.Delete(1, 2);
                    if (MainForm->net_client->GetPlayerId() != first_offer_owner)
                    {
                        for (int i4 = 0; i4 < first_offer_list.Length() / 6; i4++)
                            MainForm->animation->trade->AddItem(1,
                                DecodeNumber(first_offer_list.SubString(6 * i4 + 1, 2)),
                                DecodeNumber(first_offer_list.SubString(6 * i4 + 3, 4)));
                        for (int i5 = 0; i5 < second_offer_list.Length() / 6; i5++)
                            MainForm->animation->trade->AddItem(2,
                                DecodeNumber(second_offer_list.SubString(6 * i5 + 1, 2)),
                                DecodeNumber(second_offer_list.SubString(6 * i5 + 3, 4)));
                    }
                    else
                    {
                        for (int i2 = 0; i2 < first_offer_list.Length() / 6; i2++)
                            MainForm->animation->trade->AddItem(2,
                                DecodeNumber(first_offer_list.SubString(6 * i2 + 1, 2)),
                                DecodeNumber(first_offer_list.SubString(6 * i2 + 3, 4)));
                        for (int i3 = 0; i3 < second_offer_list.Length() / 6; i3++)
                            MainForm->animation->trade->AddItem(1,
                                DecodeNumber(second_offer_list.SubString(6 * i3 + 1, 2)),
                                DecodeNumber(second_offer_list.SubString(6 * i3 + 3, 4)));
                    }
                    MainForm->redraw_needed = true;
                    MainForm->sfx->PlaySfx(4);
                }
            }
            int new_offer_count = MainForm->animation->trade->partner_items.size();
            if (old_offer_count > new_offer_count)
            {
                MainForm->animation->trade->partner_removal_count++;
                if (MainForm->animation->trade->partner_removal_count > 1 &&
                    !MainForm->animation->game->message_visible && MainForm->animation->trade->removal_warning_armed)
                {
                    MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                        MainForm->strings->GetDialogText(DialogText_TradeWarning, false),
                        MainForm->strings->GetDialogText(DialogText_TradeWarning, true));
                    MainForm->animation->trade->removal_warning_armed = false;
                }
            }
        }
        if (action == AC_UPDATE)
        {
            int agreeing_player_id = DecodeNumber(packet.SubString(1, 2));
            int agrees = DecodeNumber(packet.SubString(3, 1));
            if (MainForm->animation->trade->is_visible)
            {
                if (MainForm->animation->trade->own_player_id == agreeing_player_id ||
                    MainForm->animation->trade->partner_player_id == agreeing_player_id)
                {
                    if (agrees == 1)
                    {
                        MainForm->sfx->PlaySfx(17);
                        MainForm->animation->trade->partner_status_text = MainForm->strings->GetText(GameText_Agree);
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                            MainForm->strings->GetText(GameText_OtherPlayerAcceptDeal));
                    }
                    if (!agrees)
                    {
                        MainForm->sfx->PlaySfx(16);
                        MainForm->animation->trade->partner_status_text = MainForm->strings->GetText(GameText_Trading);
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                            MainForm->strings->GetText(GameText_OtherPlayerCanceledDeal));
                    }
                    MainForm->redraw_needed = true;
                }
            }
        }
        if (action == AC_HIT)
        {
            int lock_state = DecodeNumber(packet.SubString(1, 1));
            if (lock_state == 1)
                MainForm->animation->trade->trade_locked = true;
            else
                MainForm->animation->trade->trade_locked = false;
        }
        if (action == AC_CLOSE)
        {
            int closing_trader_id = DecodeNumber(packet.SubString(1, 2));
            if (MainForm->animation->trade->is_visible)
            {
                if (MainForm->animation->trade->own_player_id == closing_trader_id ||
                    MainForm->animation->trade->partner_player_id == closing_trader_id)
                {
                    MainForm->sfx->PlaySfx(3);
                    MainForm->animation->trade->Close();
                    MainForm->redraw_needed = true;
                    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                        MainForm->strings->GetText(GameText_TradeHasBeenAborted));
                }
                if (MainForm->animation->interface_controller->inventory->is_dragging)
                    MainForm->animation->interface_controller->inventory->CancelDrag();
            }
        }
        if (action == AC_OPEN)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            String first_trader_name = MainForm->net_client->NextToken();
            String second_trader_name = MainForm->net_client->NextToken();
            int first_trader_id = DecodeNumber(first_trader_name.SubString(1, 2));
            int second_trader_id = DecodeNumber(second_trader_name.SubString(1, 2));
            first_trader_name.Delete(1, 2);
            second_trader_name.Delete(1, 2);
            first_trader_name = Client::Capitalize(MainForm->net_client, first_trader_name);
            second_trader_name = Client::Capitalize(MainForm->net_client, second_trader_name);
            if (MainForm->net_client->GetPlayerId() == first_trader_id)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                    MainForm->strings->GetText(GameText_YouAreTradingWith) + " " + second_trader_name + " " +
                    MainForm->strings->GetText(GameText_DragAndDropItems));
                MainForm->animation->trade->Open(first_trader_name, second_trader_name, first_trader_id,
                    second_trader_id);
            }
            else
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                    MainForm->strings->GetText(GameText_YouAreTradingWith) + " " + first_trader_name + " " +
                    MainForm->strings->GetText(GameText_DragAndDropItems));
                MainForm->animation->trade->Open(second_trader_name, first_trader_name, second_trader_id,
                    first_trader_id);
            }
        }
        if (action == AC_REQUEST && MainForm->config->GetAcceptRequests())
        {
            int trade_request_unused_byte = DecodeNumber(packet.SubString(1, 1));
            int trade_requester_id = DecodeNumber(packet.SubString(2, 2));
            String trade_requester_name = packet.SubString(4, packet.Length() - 3);
            MainForm->animation->interface_controller->SetTradeRequesterId(trade_requester_id);
            if (!MainForm->animation->game->message_visible && !MainForm->animation->trade->is_visible &&
                !MainForm->animation->chest->is_visible)
            {
                MainForm->animation->game->ShowMessageBox(3, ConfirmAction_AcceptTrade,
                    MainForm->strings->GetDialogText(DialogText_TradeRequest, false),
                    Client::Capitalize(MainForm->net_client, trade_requester_name) + " " +
                    MainForm->strings->GetDialogText(DialogText_TradeRequest, true));
                MainForm->edits->caret_enabled = false;
            }
        }
    }
    if (family == OP_GROUP)
    {
        if (action == AC_GROUP)
        {
            do
            {
                int exp_member_id = DecodeNumber(packet.SubString(1, 2));
                int shared_experience = DecodeNumber(packet.SubString(3, 4));
                int member_leveled = DecodeNumber(packet.SubString(7, 1));
                if (member_leveled > 0)
                    MainForm->players->StartLevelUpEffect(exp_member_id);
                if (MainForm->net_client->GetPlayerId() == exp_member_id)
                {
                    if (shared_experience > 0)
                    {
                        MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                            MainForm->strings->GetText(GameText_YouGained) + " " + IntToStr(shared_experience) +
                            " EXP");
                        MainForm->animation->interface_controller->chat->AddSystemMessage(5, 0,
                            MainForm->strings->GetText(GameText_YouGained) + " " + IntToStr(shared_experience) + " EXP",
                            true);
                    }
                    MainForm->animation->interface_controller->status->AddExperience(shared_experience);
                    MainForm->animation->interface_controller->status->MarkDirty();
                }
                packet.Delete(1, 7);
            } while (packet.Length() > 4);
        }
        if (action == AC_DELETE)
        {
            int leaving_member_id = DecodeNumber(packet.SubString(1, 2));
            String leaving_member_name = MainForm->animation->interface_controller->group->GetMemberName(leaving_member_id);
            int leaving_member_leader = MainForm->animation->interface_controller->group->GetMemberLeaderFlag(leaving_member_id);
            MainForm->animation->interface_controller->group->RemoveMember(leaving_member_id);
            MainForm->sfx->PlaySfx(16);
            if (leaving_member_name.Length() > 0)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                    Client::Capitalize(MainForm->net_client, leaving_member_name) + " " +
                    MainForm->strings->GetText(GameText_LeftParty));
                MainForm->animation->interface_controller->chat->AddSystemMessage(ChatIcon_Party, 1,
                    Client::Capitalize(MainForm->net_client, leaving_member_name) + " " +
                    MainForm->strings->GetText(GameText_LeftParty), false);
            }
            if ((int)MainForm->animation->interface_controller->group->members.size() <= 1 ||
                MainForm->net_client->GetPlayerId() == leaving_member_id || leaving_member_leader > 0)
                MainForm->animation->interface_controller->group->Clear();
        }
        if (action == AC_ADD)
        {
            String joined_member_name = packet.SubString(6, packet.Length() - 5);
            char joined_is_self = 0;
            if (DecodeNumber(packet.SubString(1, 2)) == (unsigned int)MainForm->net_client->GetPlayerId())
            {
                if (DecodeNumber(packet.SubString(3, 1)) >= 1)
                    MainForm->animation->interface_controller->group->is_leader = 1;
                joined_is_self = 1;
            }
            MainForm->animation->interface_controller->group->AddMember(DecodeNumber(packet.SubString(1, 2)),
                Client::Capitalize(MainForm->net_client, joined_member_name), DecodeNumber(packet.SubString(4, 1)),
                DecodeNumber(packet.SubString(3, 1)), DecodeNumber(packet.SubString(5, 1)), joined_is_self);
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                Client::Capitalize(MainForm->net_client, joined_member_name) + " " +
                MainForm->strings->GetText(GameText_JoinedYourParty));
            MainForm->animation->interface_controller->chat->AddSystemMessage(10, 1,
                Client::Capitalize(MainForm->net_client, joined_member_name) + " " +
                MainForm->strings->GetText(GameText_JoinedYourParty), false);
            if (MainForm->animation->interface_controller->group->is_leader)
                MainForm->animation->interface_controller->group->MarkMembersRemovable();
        }
        if (action == AC_LIST)
        {
            MainForm->animation->interface_controller->group->Clear();
            ParsePartyMembers(packet);
        }
        if (action == AC_CLOSE)
        {
            MainForm->animation->interface_controller->group->Clear();
            MainForm->animation->interface_controller->group->MarkListDirty();
        }
        if (action == AC_NEW)
        {
            MainForm->sfx->PlaySfx(17);
            MainForm->animation->interface_controller->group->Clear();
            ParsePartyMembers(packet);
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                MainForm->strings->GetText(GameText_YouJoinedParty));
            MainForm->animation->interface_controller->chat->AddSystemMessage(10, 1,
                MainForm->strings->GetText(GameText_YouJoinedParty), false);
        }
        if (action == AC_REQUEST && MainForm->config->GetAcceptRequests())
        {
            int party_request_kind = DecodeNumber(packet.SubString(1, 1));
            int party_requester_id = DecodeNumber(packet.SubString(2, 2));
            String party_requester_name = packet.SubString(4, packet.Length() - 3);
            MainForm->animation->interface_controller->group->SetRequesterId(party_requester_id);
            if (!MainForm->animation->game->message_visible)
            {
                if (!party_request_kind)
                {
                    MainForm->animation->game->ShowMessageBox(3, ConfirmAction_AcceptPartyJoin,
                        MainForm->strings->GetDialogText(DialogText_GroupRequestWants, false),
                        Client::Capitalize(MainForm->net_client, party_requester_name) + " " +
                        MainForm->strings->GetDialogText(DialogText_GroupRequestWants, true));
                    MainForm->edits->caret_enabled = false;
                }
                if (party_request_kind == 1)
                {
                    MainForm->animation->game->ShowMessageBox(3, ConfirmAction_AcceptPartyInvite,
                        MainForm->strings->GetDialogText(DialogText_GroupRequestSend, false),
                        Client::Capitalize(MainForm->net_client, party_requester_name) + " " +
                        MainForm->strings->GetDialogText(DialogText_GroupRequestSend, true));
                    MainForm->edits->caret_enabled = false;
                }
            }
        }
        if (action == AC_RESULT)
        {
            int party_reply_kind = DecodeNumber(packet.SubString(1, 1));
            if (!party_reply_kind)
            {
                String party_reply_name = packet.SubString(2, packet.Length() - 1);
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                    Client::Capitalize(MainForm->net_client, party_reply_name) + " " +
                    MainForm->strings->GetText(GameText_IsAlreadyInAnother));
                MainForm->animation->interface_controller->chat->AddSystemMessage(2, 3,
                    Client::Capitalize(MainForm->net_client, party_reply_name) + " " +
                    MainForm->strings->GetText(GameText_IsAlreadyInAnother), false);
            }
            if (party_reply_kind == 1)
            {
                String party_notice_name = packet.SubString(2, packet.Length() - 1);
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                    Client::Capitalize(MainForm->net_client, party_notice_name) + " " +
                    MainForm->strings->GetText(GameText_IsAlreadyInYour));
                MainForm->animation->interface_controller->chat->AddSystemMessage(2, 3,
                    Client::Capitalize(MainForm->net_client, party_notice_name) + " " +
                    MainForm->strings->GetText(GameText_IsAlreadyInYour), false);
            }
            if (party_reply_kind == 2)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                    MainForm->strings->GetText(GameText_GroupIsFull));
                MainForm->animation->interface_controller->chat->AddSystemMessage(2, 3,
                    MainForm->strings->GetText(GameText_GroupIsFull), false);
            }
        }
        if (action == AC_UPDATE)
        {
            int hp_member_id = DecodeNumber(packet.SubString(1, 2));
            int member_health = DecodeNumber(packet.SubString(3, 1));
            MainForm->animation->interface_controller->group->SetMemberHealth(hp_member_id, member_health);
        }
    }
    if (family == OP_DOOR)
    {
        if (action == AC_OPEN)
        {
            int door_x = DecodeNumber(packet.SubString(1, 1));
            int door_y = DecodeNumber(packet.SubString(2, 1));
            MainForm->sfx->PlaySfx(SfxSound_DoorOpen);
            MainForm->map->OpenDoor(door_x, door_y);
        }
        if (action == AC_CLOSE)
        {
            int required_key_id = DecodeNumber(packet.SubString(1, 2));
            String required_key_name = MainForm->item_values->GetKeyName(required_key_id);
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                MainForm->strings->GetText(GameText_DoorIsLocked2) + " - " + required_key_name);
            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                MainForm->strings->GetDialogText(DialogText_DoorLocked, false),
                MainForm->strings->GetDialogText(DialogText_DoorLocked, true));
            MainForm->sfx->PlaySfx(25);
        }
    }
    if (family == OP_LOCKER)
    {
        if (action == AC_HIT)
        {
            int locker_capacity = DecodeNumber(packet.SubString(1, 1));
            // 25 slots, and 5 more per upgrade; the 25 in the message is replaced.
            locker_capacity = 5 * locker_capacity + 25;
            String locker_full_text = MainForm->strings->GetDialogText(DialogText_LockerFullHold, true);
            int number_position = locker_full_text.Pos("25");
            locker_full_text.Delete(number_position, 2);
            locker_full_text.Insert(IntToStr(locker_capacity), number_position);
            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                MainForm->strings->GetDialogText(DialogText_LockerFullHold, false), locker_full_text);
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning, locker_full_text);
        }
        if (action == AC_BUY)
        {
            MainForm->sfx->PlaySfx(26);
            int upgrade_gold_left = DecodeNumber(packet.SubString(1, 4));
            int locker_upgrade_level = DecodeNumber(packet.SubString(5, 1));
            MainForm->animation->bank->locker_upgrade_level = locker_upgrade_level;
            MainForm->animation->interface_controller->inventory->SetItemAmount(1, upgrade_gold_left);
            MainForm->animation->interface_controller->inventory->MarkItemsDirty();
            MainForm->animation->interface_controller->status->gold = upgrade_gold_left;
            MainForm->animation->interface_controller->status->MarkDirty();
            MainForm->animation->interface_controller->chat->AddSystemMessage(16, 0,
                MainForm->strings->GetText(GameText_YourLockerCapacityIs), true);
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Information,
                MainForm->strings->GetText(GameText_YourLockerCapacityIs));
            MainForm->redraw_needed = true;
        }
        if (action == AC_OPEN)
        {
            if (packet.Length() > 1)
            {
                int locker_x = DecodeNumber(packet.SubString(1, 1));
                int locker_y = DecodeNumber(packet.SubString(2, 1));
                String locker_title = MainForm->players->GetName(MainForm->net_client->GetPlayerId());
                locker_title = Client::Capitalize(MainForm->net_client, locker_title);
                locker_title = locker_title + "'s " + MainForm->strings->GetText(GameText_PrivateLocker);
                MainForm->animation->locker->Open(locker_title, 0, locker_x, locker_y);
                MainForm->sfx->PlaySfx(4);
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                    MainForm->strings->GetText(GameText_YouOpenedYourLocker) + " " +
                    MainForm->strings->GetText(GameText_DragAndDropItems));
                if (packet.Length() > 2)
                {
                    MainForm->animation->locker->ClearItems();
                    packet.Delete(1, 2);
                    for (int i6 = 0; i6 < packet.Length() / 5; i6++)
                    {
                        int locker_item_id = DecodeNumber(packet.SubString(5 * i6 + 1, 2));
                        int locker_item_amount = DecodeNumber(packet.SubString(5 * i6 + 3, 3));
                        MainForm->animation->locker->AddItem(locker_item_id, locker_item_amount);
                    }
                }
            }
        }
        if (action == AC_RESULT)
        {
            if (packet.Length() > 7)
            {
                int deposited_item_id = DecodeNumber(packet.SubString(1, 2));
                int deposit_item_left = DecodeNumber(packet.SubString(3, 4));
                int deposit_weight = DecodeNumber(packet.SubString(7, 1));
                int deposit_max_weight = DecodeNumber(packet.SubString(8, 1));
                if (deposited_item_id == 1)
                    MainForm->animation->interface_controller->status->gold = deposit_item_left;
                MainForm->animation->interface_controller->inventory->SetItemAmount(deposited_item_id,
                    deposit_item_left);
                MainForm->animation->interface_controller->status->weight = deposit_weight;
                MainForm->animation->interface_controller->status->max_weight = deposit_max_weight;
                MainForm->animation->interface_controller->status->MarkDirty();
                if (packet.Length() > 2)
                {
                    MainForm->animation->locker->ClearItems();
                    packet.Delete(1, 8);
                    for (int i7 = 0; i7 < packet.Length() / 5; i7++)
                    {
                        int deposit_list_item_id = DecodeNumber(packet.SubString(5 * i7 + 1, 2));
                        int deposit_list_amount = DecodeNumber(packet.SubString(5 * i7 + 3, 3));
                        MainForm->animation->locker->AddItem(deposit_list_item_id, deposit_list_amount);
                    }
                }
                MainForm->redraw_needed = true;
            }
        }
        if (action == AC_PICKUP)
        {
            if (packet.Length() > 6)
            {
                int withdrawn_item_id = DecodeNumber(packet.SubString(1, 2));
                int withdrawn_amount = DecodeNumber(packet.SubString(3, 3));
                int withdraw_weight = DecodeNumber(packet.SubString(6, 1));
                int withdraw_max_weight = DecodeNumber(packet.SubString(7, 1));
                if (withdrawn_item_id == 1)
                    MainForm->animation->interface_controller->status->gold = withdrawn_amount +
                        MainForm->animation->interface_controller->inventory->GetItemAmount(withdrawn_item_id);
                MainForm->animation->interface_controller->inventory->AddItem(withdrawn_item_id, withdrawn_amount);
                MainForm->animation->interface_controller->status->weight = withdraw_weight;
                MainForm->animation->interface_controller->status->max_weight = withdraw_max_weight;
                MainForm->animation->interface_controller->status->MarkDirty();
                if (packet.Length() > 2)
                {
                    MainForm->animation->locker->ClearItems();
                    packet.Delete(1, 7);
                    for (int i8 = 0; i8 < packet.Length() / 5; i8++)
                    {
                        int withdraw_list_item_id = DecodeNumber(packet.SubString(5 * i8 + 1, 2));
                        int withdraw_list_amount = DecodeNumber(packet.SubString(5 * i8 + 3, 3));
                        MainForm->animation->locker->AddItem(withdraw_list_item_id, withdraw_list_amount);
                    }
                }
                MainForm->redraw_needed = true;
            }
        }
    }
    if (family == OP_CHEST)
    {
        if (action == AC_HIT)
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                MainForm->strings->GetText(GameText_ChestIsFullItem));
        if (action == AC_OPEN)
        {
            if (packet.Length() > 1)
            {
                int chest_x = DecodeNumber(packet.SubString(1, 1));
                int chest_y = DecodeNumber(packet.SubString(2, 1));
                MainForm->sfx->PlaySfx(4);
                MainForm->animation->chest->Open(0, chest_x, chest_y);
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
                    MainForm->strings->GetText(GameText_YouOpenedChest2) + " " +
                    MainForm->strings->GetText(GameText_DragAndDropItems));
                if (packet.Length() > 2)
                {
                    MainForm->animation->chest->ClearItems();
                    packet.Delete(1, 2);
                    for (int i9 = 0; i9 < packet.Length() / 5; i9++)
                    {
                        int chest_item_id = DecodeNumber(packet.SubString(5 * i9 + 1, 2));
                        int chest_item_amount = DecodeNumber(packet.SubString(5 * i9 + 3, 3));
                        MainForm->animation->chest->AddItem(chest_item_id, chest_item_amount);
                    }
                }
            }
        }
        if (action == AC_UPDATE)
        {
            if (packet.Length() > 1)
            {
                if (MainForm->animation->chest->is_visible)
                {
                    MainForm->animation->chest->ClearItems();
                    for (int i10 = 0; i10 < packet.Length() / 5; i10++)
                    {
                        int chest_refresh_item_id = DecodeNumber(packet.SubString(5 * i10 + 1, 2));
                        int chest_refresh_amount = DecodeNumber(packet.SubString(5 * i10 + 3, 3));
                        MainForm->animation->chest->AddItem(chest_refresh_item_id, chest_refresh_amount);
                    }
                }
                MainForm->redraw_needed = true;
            }
            else
                MainForm->animation->chest->ClearItems();
        }
        if (action == AC_RESULT)
        {
            if (packet.Length() > 7)
            {
                int chest_put_item_id = DecodeNumber(packet.SubString(1, 2));
                int chest_put_item_left = DecodeNumber(packet.SubString(3, 4));
                int chest_put_weight = DecodeNumber(packet.SubString(7, 1));
                int chest_put_max_weight = DecodeNumber(packet.SubString(8, 1));
                if (chest_put_item_id == 1)
                    MainForm->animation->interface_controller->status->gold = chest_put_item_left;
                MainForm->animation->interface_controller->inventory->SetItemAmount(chest_put_item_id,
                    chest_put_item_left);
                MainForm->animation->interface_controller->status->weight = chest_put_weight;
                MainForm->animation->interface_controller->status->max_weight = chest_put_max_weight;
                MainForm->animation->interface_controller->status->MarkDirty();
                if (packet.Length() > 2)
                {
                    MainForm->animation->chest->ClearItems();
                    packet.Delete(1, 8);
                    for (int i11 = 0; i11 < packet.Length() / 5; i11++)
                    {
                        int chest_put_list_item_id = DecodeNumber(packet.SubString(5 * i11 + 1, 2));
                        int chest_put_list_amount = DecodeNumber(packet.SubString(5 * i11 + 3, 3));
                        MainForm->animation->chest->AddItem(chest_put_list_item_id, chest_put_list_amount);
                    }
                }
                MainForm->redraw_needed = true;
            }
        }
        if (action == AC_PICKUP)
        {
            if (packet.Length() > 6)
            {
                int chest_taken_item_id = DecodeNumber(packet.SubString(1, 2));
                int chest_taken_amount = DecodeNumber(packet.SubString(3, 3));
                int chest_take_weight = DecodeNumber(packet.SubString(6, 1));
                int chest_take_max_weight = DecodeNumber(packet.SubString(7, 1));
                if (chest_taken_item_id == 1)
                    MainForm->animation->interface_controller->status->gold = chest_taken_amount +
                        MainForm->animation->interface_controller->inventory->GetItemAmount(chest_taken_item_id);
                MainForm->animation->interface_controller->inventory->AddItem(chest_taken_item_id, chest_taken_amount);
                MainForm->animation->interface_controller->status->weight = chest_take_weight;
                MainForm->animation->interface_controller->status->max_weight = chest_take_max_weight;
                MainForm->animation->interface_controller->status->MarkDirty();
                if (packet.Length() > 2)
                {
                    MainForm->animation->chest->ClearItems();
                    packet.Delete(1, 7);
                    for (int i12 = 0; i12 < packet.Length() / 5; i12++)
                    {
                        int chest_take_list_item_id = DecodeNumber(packet.SubString(5 * i12 + 1, 2));
                        int chest_take_list_amount = DecodeNumber(packet.SubString(5 * i12 + 3, 3));
                        MainForm->animation->chest->AddItem(chest_take_list_item_id, chest_take_list_amount);
                    }
                }
                MainForm->redraw_needed = true;
            }
        }
        if (action == AC_CLOSE)
        {
            MainForm->sfx->PlaySfx(4);
            if (packet.Length() < 2)
            {
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                    MainForm->strings->GetText(GameText_ChestSeemsBroken));
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_ChestBroken, false),
                    MainForm->strings->GetDialogText(DialogText_ChestBroken, true));
            }
            else
            {
                int chest_key_id = DecodeNumber(packet.SubString(1, 2));
                String chest_key_name = MainForm->item_values->GetKeyName(chest_key_id);
                MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                    MainForm->strings->GetText(GameText_ChestIsLocked2) + " - " + chest_key_name);
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_ChestLocked, false),
                    MainForm->strings->GetDialogText(DialogText_ChestLocked, true));
            }
        }
    }
    if (family == OP_SKILL)
    {
        if (action == AC_RESULT)
        {
            int trainer_reply = DecodeNumber(packet.SubString(1, 2));
            if (trainer_reply == 1)
            {
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_ResetCharacterRemove, false),
                    MainForm->strings->GetDialogText(DialogText_ResetCharacterRemove, true));
                MainForm->edits->caret_enabled = false;
            }
            if (trainer_reply == 2)
            {
                int new_class_id = DecodeNumber(packet.SubString(3, 1));
                MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                    MainForm->strings->GetDialogText(DialogText_WrongClass, false),
                    MainForm->strings->GetDialogText(DialogText_WrongClass, true));
                MainForm->animation->interface_controller->status->class_id = new_class_id;
                MainForm->edits->caret_enabled = false;
            }
        }
        if (action == AC_JUNK)
        {
            MainForm->animation->interface_controller->status->stat_points = DecodeNumber(packet.SubString(1, 2));
            MainForm->animation->interface_controller->status->skill_points = DecodeNumber(packet.SubString(3, 2));
            MainForm->animation->interface_controller->status->SetHealth(DecodeNumber(packet.SubString(5, 2)));
            MainForm->animation->interface_controller->status->SetMaxHealth(DecodeNumber(packet.SubString(7, 2)));
            MainForm->animation->interface_controller->status->SetMana(DecodeNumber(packet.SubString(9, 2)));
            MainForm->animation->interface_controller->status->SetMaxMana(DecodeNumber(packet.SubString(11, 2)));
            MainForm->animation->interface_controller->status->SetMaxStamina(DecodeNumber(packet.SubString(13, 2)));
            MainForm->animation->interface_controller->status->stat_str = DecodeNumber(packet.SubString(15, 2));
            MainForm->animation->interface_controller->status->stat_int = DecodeNumber(packet.SubString(17, 2));
            MainForm->animation->interface_controller->status->stat_wis = DecodeNumber(packet.SubString(19, 2));
            MainForm->animation->interface_controller->status->stat_agi = DecodeNumber(packet.SubString(21, 2));
            MainForm->animation->interface_controller->status->stat_con = DecodeNumber(packet.SubString(23, 2));
            MainForm->animation->interface_controller->status->stat_cha = DecodeNumber(packet.SubString(25, 2));
            MainForm->animation->interface_controller->status->label_413_from = DecodeNumber(packet.SubString(27, 2));
            MainForm->animation->interface_controller->status->label_413_to = DecodeNumber(packet.SubString(29, 2));
            MainForm->animation->interface_controller->status->label_414_value = DecodeNumber(packet.SubString(31, 2));
            MainForm->animation->interface_controller->status->label_416_value = DecodeNumber(packet.SubString(33, 2));
            MainForm->animation->interface_controller->status->label_415_value = DecodeNumber(packet.SubString(35, 2));
            MainForm->sfx->PlaySfx(37);
            MainForm->animation->interface_controller->status->MarkDirty();
            MainForm->animation->interface_controller->spells->Reset();
            MainForm->animation->interface_controller->skills->Reset();
            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                MainForm->strings->GetDialogText(DialogText_ResetComplete, false),
                MainForm->strings->GetDialogText(DialogText_ResetComplete, true));
            MainForm->edits->caret_enabled = false;
        }
        if (action == AC_GET)
        {
            int learned_skill_id = DecodeNumber(packet.SubString(1, 2));
            int learn_gold_left = DecodeNumber(packet.SubString(3, 5));
            MainForm->animation->interface_controller->spells->AddSpell(learned_skill_id, 0);
            MainForm->animation->interface_controller->spells->MarkDirty();
            MainForm->animation->interface_controller->skills->MarkDirty();
            MainForm->animation->interface_controller->inventory->SetItemAmount(1, learn_gold_left);
            MainForm->animation->interface_controller->inventory->MarkItemsDirty();
            MainForm->animation->interface_controller->status->gold = learn_gold_left;
            MainForm->animation->interface_controller->status->MarkDirty();
            MainForm->animation->learn->RemoveSkill(learned_skill_id);
            MainForm->sfx->PlaySfx(37);
            MainForm->edits->caret_enabled = false;
        }
        if (action == AC_DELETE)
        {
            int forgotten_skill_id = DecodeNumber(packet.SubString(1, 2));
            MainForm->animation->interface_controller->spells->RemoveSpell(forgotten_skill_id);
            MainForm->animation->interface_controller->skills->RemoveSkill(forgotten_skill_id);
            MainForm->animation->interface_controller->spells->MarkDirty();
            MainForm->animation->interface_controller->skills->MarkDirty();
            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                MainForm->strings->GetDialogText(DialogText_SkillRemoved, false),
                MainForm->strings->GetDialogText(DialogText_SkillRemoved, true));
            MainForm->edits->caret_enabled = false;
        }
        if (action == AC_CONFIRM)
        {
            MainForm->animation->interface_controller->status->skill_points = DecodeNumber(packet.SubString(1, 2));
            int raised_skill_id = DecodeNumber(packet.SubString(3, 2));
            int raised_skill_level = DecodeNumber(packet.SubString(5, 2));
            MainForm->animation->interface_controller->skills->SetSkillLevel(raised_skill_id, raised_skill_level);
            MainForm->animation->interface_controller->spells->SetSpellLevel(raised_skill_id, raised_skill_level);
            MainForm->sfx->PlaySfx(20);
            MainForm->animation->interface_controller->status->MarkDirty();
        }
        if (action == AC_SET)
        {
            MainForm->animation->interface_controller->status->stat_points = DecodeNumber(packet.SubString(1, 2));
            MainForm->animation->interface_controller->status->stat_str = DecodeNumber(packet.SubString(3, 2));
            MainForm->animation->interface_controller->status->stat_int = DecodeNumber(packet.SubString(5, 2));
            MainForm->animation->interface_controller->status->stat_wis = DecodeNumber(packet.SubString(7, 2));
            MainForm->animation->interface_controller->status->stat_agi = DecodeNumber(packet.SubString(9, 2));
            MainForm->animation->interface_controller->status->stat_con = DecodeNumber(packet.SubString(11, 2));
            MainForm->animation->interface_controller->status->stat_cha = DecodeNumber(packet.SubString(13, 2));
            MainForm->animation->interface_controller->status->SetMaxHealth(DecodeNumber(packet.SubString(15, 2)));
            MainForm->animation->interface_controller->status->SetMaxMana(DecodeNumber(packet.SubString(17, 2)));
            MainForm->animation->interface_controller->status->SetMaxStamina(DecodeNumber(packet.SubString(19, 2)));
            MainForm->animation->interface_controller->status->max_weight = DecodeNumber(packet.SubString(21, 2));
            MainForm->animation->interface_controller->status->label_413_from = DecodeNumber(packet.SubString(23, 2));
            MainForm->animation->interface_controller->status->label_413_to = DecodeNumber(packet.SubString(25, 2));
            MainForm->animation->interface_controller->status->label_414_value = DecodeNumber(packet.SubString(27, 2));
            MainForm->animation->interface_controller->status->label_416_value = DecodeNumber(packet.SubString(29, 2));
            MainForm->animation->interface_controller->status->label_415_value = DecodeNumber(packet.SubString(31, 2));
            MainForm->sfx->PlaySfx(20);
            MainForm->animation->interface_controller->status->MarkDirty();
        }
        if (action == AC_OPEN)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            String trainer_header = MainForm->net_client->NextToken();
            String trainer_skill_data = MainForm->net_client->NextToken();
            int trainer_request_id = DecodeNumber(trainer_header.SubString(1, 2));
            String trainer_name = trainer_header.SubString(3, trainer_header.Length() - 2);
            MainForm->animation->learn->Open(trainer_name, trainer_request_id);
            for (int i13 = 0; 28 * i13 + 28 <= trainer_skill_data.Length(); i13++)
            {
                int offered_skill_id = DecodeNumber(trainer_skill_data.SubString(28 * i13 + 1, 2));
                int offered_required_level = DecodeNumber(trainer_skill_data.SubString(28 * i13 + 3, 1));
                int offered_required_class = DecodeNumber(trainer_skill_data.SubString(28 * i13 + 4, 1));
                int offered_price = DecodeNumber(trainer_skill_data.SubString(28 * i13 + 5, 4));
                int offered_required_skill = DecodeNumber(trainer_skill_data.SubString(28 * i13 + 9, 2));
                int offered_second_required_skill = DecodeNumber(trainer_skill_data.SubString(28 * i13 + 11, 2));
                int offered_third_required_skill = DecodeNumber(trainer_skill_data.SubString(28 * i13 + 13, 2));
                int offered_fourth_required_skill = DecodeNumber(trainer_skill_data.SubString(28 * i13 + 15, 2));
                int offered_required_str = DecodeNumber(trainer_skill_data.SubString(28 * i13 + 17, 2));
                int offered_required_wis = DecodeNumber(trainer_skill_data.SubString(28 * i13 + 19, 2));
                int offered_required_int = DecodeNumber(trainer_skill_data.SubString(28 * i13 + 21, 2));
                int offered_required_agi = DecodeNumber(trainer_skill_data.SubString(28 * i13 + 23, 2));
                int offered_required_con = DecodeNumber(trainer_skill_data.SubString(28 * i13 + 25, 2));
                int offered_required_cha = DecodeNumber(trainer_skill_data.SubString(28 * i13 + 27, 2));
                MainForm->animation->learn->AddSkill(1, offered_skill_id, offered_required_level,
                    offered_required_class, offered_price, offered_required_skill, offered_second_required_skill,
                    offered_third_required_skill, offered_fourth_required_skill, offered_required_str,
                    offered_required_wis, offered_required_int, offered_required_agi, offered_required_con,
                    offered_required_cha);
            }
        }
    }
    if (family == OP_ADMIN)
    {
        if (action == AC_RESULT)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            int report_kind = DecodeNumber(MainForm->net_client->NextToken());
            String reporter_name = Client::Capitalize(MainForm->net_client, MainForm->net_client->NextToken());
            String report_text = Client::CensorText(MainForm->net_client, MainForm->net_client->NextToken());
            String reported_name = MainForm->net_client->NextToken();
            if (!MainForm->animation->social->IsIgnored(reporter_name))
            {
                MainForm->sfx->PlaySfx(SfxSound_AdminInteractReply);
                if (report_kind == 1)
                    MainForm->animation->interface_controller->chat->AddGroupMessage(ChatIcon_HelpRequest,
                        CHAT_COLOR_NOTICE, reporter_name, "needs help: " + report_text, false);
                if (report_kind == 2)
                    MainForm->animation->interface_controller->chat->AddGroupMessage(ChatIcon_HelpRequest,
                        CHAT_COLOR_NOTICE, reporter_name, "reports: " + reported_name + ", " + report_text, false);
            }
        }
        if (action == AC_PRIVATE)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            String info_name = MainForm->net_client->NextToken();
            String info_minutes_token = MainForm->net_client->NextToken();
            String info_unread_token = MainForm->net_client->NextToken();
            String info_stats = MainForm->net_client->NextToken();
            int info_minutes = DecodeNumber(info_minutes_token);
            int info_hours = info_minutes / 60;
            int info_minutes_left = info_minutes % MINUTES_PER_HOUR;
            String info_time_text = IntToStr(info_hours) + "h." + IntToStr(info_minutes_left) + "m.";
            info_name = Client::Capitalize(MainForm->net_client, info_name);
            MainForm->animation->memo->Open(info_name + " - " + info_time_text);
            MainForm->animation->memo->AddCharacterStats(info_stats);
        }
        if (action == AC_LIST)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            String inventory_owner_name = MainForm->net_client->NextToken();
            String inventory_minutes_token = MainForm->net_client->NextToken();
            String bank_gold_token = MainForm->net_client->NextToken();
            String inventory_item_list = MainForm->net_client->NextToken();
            String locker_item_list = MainForm->net_client->NextToken();
            int inventory_minutes = DecodeNumber(inventory_minutes_token);
            int inventory_hours = inventory_minutes / 60;
            int inventory_minutes_left = inventory_minutes % MINUTES_PER_HOUR;
            String inventory_time_text = IntToStr(inventory_hours) + "h." + IntToStr(inventory_minutes_left) + "m.";
            inventory_owner_name = Client::Capitalize(MainForm->net_client, inventory_owner_name);
            MainForm->animation->memo->Open(inventory_owner_name + " - " + inventory_time_text);
            MainForm->animation->memo->AddItemList(inventory_item_list, 6, 4);
            MainForm->animation->memo->AddText(" ", "", false);
            MainForm->animation->memo->AddItemList(locker_item_list, 5, 3);
            if (locker_item_list.Length() > 1)
                MainForm->animation->memo->AddText(" ", "", false);
            MainForm->animation->memo->AddText("Bank account", IntToStr(DecodeNumber(bank_gold_token)), false);
            MainForm->animation->memo->AddText(" ", "", false);
        }
        if (action == AC_DELETE)
        {
            int hidden_player_id = DecodeNumber(packet);
            MainForm->players->HidePlayer(hidden_player_id);
            MainForm->players->StartEffect(hidden_player_id, 24);
        }
        if (action == AC_UPDATE)
        {
            int unhidden_player_id = DecodeNumber(packet);
            MainForm->players->UnhidePlayer(unhidden_player_id);
            MainForm->players->StartEffect(unhidden_player_id, 24);
        }
    }
    if (family == OP_SYSTEM)
    {
        if (action == AC_CONFIRM)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            String notice_title = MainForm->net_client->NextToken();
            String notice_body = MainForm->net_client->NextToken();
            String notice_extra_line = MainForm->net_client->NextToken();
            String notice_last_line = MainForm->net_client->NextToken();
            MainForm->animation->memo->Open(notice_title);
            MainForm->animation->memo->AddText(notice_body, "", false);
            if (notice_extra_line.Length() > 0)
            {
                MainForm->animation->memo->AddText(" ", "", false);
                MainForm->animation->memo->AddText(notice_extra_line, "", false);
            }
            if (notice_last_line.Length() > 0)
            {
                MainForm->animation->memo->AddText(" ", "", false);
                MainForm->animation->memo->AddText(notice_last_line, "", false);
            }
        }
        if (action == AC_OPEN)
        {
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning, packet);
            MainForm->animation->interface_controller->chat->AddSystemMessage(23, 2, packet, false);
        }
        if (action == AC_CLOSE && MainForm->animation->GetScreenMode() == ScreenMode_Game)
        {
            if (MainForm->animation->GetSubState() != GamePanel_Chat)
            {
                MainForm->gfx->LoadChatPanel();
                MainForm->animation->SetSubState(GamePanel_Chat);
                MainForm->animation->interface_controller->Invalidate();
            }
            MainForm->animation->interface_controller->chat->AddLocalMessage(6, 2,
                MainForm->strings->GetText(GameText_Server),
                MainForm->strings->GetText(GameText_RebootSequenceStartedServer), false);
            MainForm->animation->interface_controller->chat->AddGlobalMessage(6, CHAT_COLOR_NOTICE,
                MainForm->strings->GetText(GameText_Server),
                MainForm->strings->GetText(GameText_RebootSequenceStartedServer), false, false);
            MainForm->animation->interface_controller->chat->AddSystemMessage(6, 2,
                MainForm->strings->GetText(GameText_RebootSequenceStartedServer), false);
            MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Warning,
                MainForm->strings->GetText(GameText_RebootSequenceStartedServer));
            MainForm->animation->game->ShowMessageBox(1, ConfirmAction_None,
                MainForm->strings->GetText(GameText_Server),
                MainForm->strings->GetText(GameText_ServerSystemIsRebooting));
            MainForm->sfx->PlaySfx(7);
        }
        if (action == AC_SYN2)
        {
            TDateTime now1 = Now();
            TTimeStamp sent_stamp;
            sent_stamp = DateTimeToTimeStamp(MainForm->animation->interface_controller->status->ping_sent_time);
            TTimeStamp now_stamp;
            now_stamp = DateTimeToTimeStamp(now1);
            // Both are milliseconds since midnight.
            int round_trip_ms = now_stamp.Time - sent_stamp.Time;
            MainForm->sfx->PlaySfx(5);
            MainForm->animation->interface_controller->chat->AddLocalMessage(7, 0,
                Client::DeobfuscateString(MainForm->net_client, "`:Y,TL"),
                Client::DeobfuscateString(MainForm->net_client, " eZ6 -h)[:Z ^+ 8_6] Y1h-[*. p'r") +
                IntToStr(round_trip_ms) + "  ms.", true);
        }
    }
    if (family == OP_QUEST)
    {
        if (action == AC_PUBLIC)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            String dialog_header = MainForm->net_client->NextToken();
            int dialog_npc_index = DecodeNumber(dialog_header.SubString(1, 2));
            int dialog_npc_id = MainForm->npcs->GetNpcId(dialog_npc_index);
            String dialog_npc_name = MainForm->npc_values->GetName(dialog_npc_id);
            String dialog_line = "";
            if (dialog_npc_name != "")
            {
                do
                {
                    dialog_line = MainForm->net_client->NextToken();
                    if (dialog_line.Length() > 0)
                    {
                        MainForm->animation->interface_controller->chat->AddLocalMessage(0, 0, dialog_npc_name,
                            dialog_line, false);
                        MainForm->gfx->npcs->ShowSpeechBalloon(dialog_line, dialog_npc_index, 0);
                    }
                } while (dialog_line.Length() > 0);
            }
        }
        if (action == AC_TALK)
        {
            MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
            String quest_dialog_header = MainForm->net_client->NextToken();
            // The header: choices, two values the dialog sends back, the quest, then the session id.
            int dialog_choice_count = DecodeNumber(quest_dialog_header.SubString(1, 1));
            int dialog_echo_b = DecodeNumber(quest_dialog_header.SubString(2, 2));
            int quest_id = DecodeNumber(quest_dialog_header.SubString(4, 2));
            int quest_session_id = DecodeNumber(quest_dialog_header.SubString(6, 2));
            int dialog_echo_a = DecodeNumber(quest_dialog_header.SubString(8, 2));
            int choice_index = 0;
            if (dialog_choice_count > 0)
            {
                MainForm->animation->dialog->quest_names->Clear();
                for (int i14 = 0; i14 < dialog_choice_count; i14++)
                {
                    String choice_entry = MainForm->net_client->NextToken();
                    if (choice_entry.Length() > 2)
                    {
                        int choice_id = DecodeNumber(choice_entry.SubString(1, 2));
                        choice_entry.Delete(1, 2);
                        MainForm->animation->dialog->quest_ids[choice_index] = choice_id;
                        MainForm->animation->dialog->quest_names->Add(choice_entry);
                        choice_index++;
                    }
                }
            }
            if (dialog_choice_count < 2)
                MainForm->animation->dialog->has_quest_list = false;
            else
                MainForm->animation->dialog->has_quest_list = true;
            MainForm->animation->dialog->Open(MainForm->animation->dialog->quest_names->Strings[0]);
            MainForm->net_client->SetSessionId(quest_session_id);
            String dialog_part = "";
            do
            {
                dialog_part = MainForm->net_client->NextToken();
                if (dialog_part.Length() > ENTRY_ID_LENGTH)
                {
                    int dialog_part_kind = DecodeNumber(dialog_part.SubString(1, 2));
                    dialog_part.Delete(1, 2);
                    if (dialog_part_kind == 1)
                        MainForm->animation->dialog->AddPage(dialog_part);
                    if (dialog_part_kind == 2)
                    {
                        if (dialog_part.Length() > ENTRY_ID_LENGTH)
                        {
                            int link_id = DecodeNumber(dialog_part.SubString(1, 2));
                            dialog_part.Delete(1, 2);
                            MainForm->animation->dialog->AddLink(link_id, dialog_part);
                        }
                    }
                }
            } while (dialog_part.Length() > ENTRY_ID_LENGTH);
            MainForm->animation->dialog->quest_id = quest_id;
            MainForm->animation->dialog->dialog_echo_b = dialog_echo_b;
            MainForm->animation->dialog->dialog_echo_a = dialog_echo_a;
            MainForm->animation->dialog->ShowPage();
        }
        if (action == AC_LIST)
        {
            int quest_list_kind = DecodeNumber(String(packet[1]));
            if (quest_list_kind == 1)
            {
                MainForm->animation->quests->Open();
                packet.Delete(1, 3);
                if (packet.Length() > 0)
                    MainForm->animation->quests->LoadQuestList(packet, String(ByteToChar(TOKEN_BREAK_BYTE)));
            }
            if (quest_list_kind == 2)
            {
                packet.Delete(1, 3);
                if (MainForm->animation->quests->is_visible)
                {
                    if (packet.Length() > 0)
                        MainForm->animation->quests->LoadQuestHistory(packet, String(ByteToChar(TOKEN_BREAK_BYTE)));
                }
            }
        }
    }
    if (family == OP_MAP)
    {
        if (action == AC_NEW)
        {
            MainForm->sfx->PlaySfx(6);
            MainForm->animation->interface_controller->chat->AddLocalMessage(6, 2,
                MainForm->strings->GetText(GameText_Server),
                MainForm->strings->GetText(GameText_MapMutationDetectedUpdating), false);
            MainForm->animation->interface_controller->chat->AddSystemMessage(6, 2,
                MainForm->strings->GetText(GameText_MapMutationDetectedUpdating), false);
            MainForm->map->map_data = packet.SubString(1, packet.Length() + 1);
            MainForm->map->SaveMapFile(MainForm->map->map_id);
            MainForm->map_items->ClearAll();
            MainForm->npcs->Clear();
            MainForm->npcs->ReloadGraphicsLibrary();
            MainForm->gfx->tiles->ReopenLibrary();
            MainForm->gfx->objects->ReopenLibrary();
            MainForm->gfx->masks->ReopenLibrary();
            MainForm->gfx->walls->ReopenLibrary();
            MainForm->gfx->tops->ReopenLibrary();
            MainForm->gfx->blends->ReopenLibrary();
            MainForm->gfx->items->ReopenLibrary();
            MainForm->map->LoadMap(MainForm->map->GetMapId());
            MainForm->animation->game->CenterViewOnPlayer(MainForm->net_client->GetPlayerId());
        }
        if (action == AC_SET)
        {
            MainForm->map->map_data = packet.SubString(1, packet.Length() + 1);
            MainForm->map->SaveMapFile(MainForm->map->map_id);
            String map_ack = Client::EncodeNumber(MainForm->net_client, MainForm->map->map_id, 2);
            map_ack = map_ack + Client::EncodeNumber(MainForm->net_client, MainForm->map->warp_session_id, 2);
            MainForm->net_client->SendPacket(AC_CONFIRM, OP_MAP, map_ack);
            MainForm->edits->caret_enabled = true;
            MainForm->map->loading_map = false;
            MainForm->map->SetWarpTimeoutAction(2);
            MainForm->map->StartWarpTimeout();
        }
        if (action == AC_UPDATE)
        {
            int warp_kind = DecodeNumber(packet.SubString(1, 1));
            if (warp_kind == 1)
            {
                MainForm->players->Clear();
                ParseNearbyEntities(packet.SubString(2, packet.Length()));
                MainForm->animation->game->CenterViewOnPlayer(MainForm->net_client->GetPlayerId());
            }
            if (warp_kind == 2)
            {
                int old_map_id = MainForm->map->GetMapId();
                int new_map_id = DecodeNumber(packet.SubString(2, 2));
                int warp_effect = DecodeNumber(packet.SubString(4, 1));
                MainForm->players->Clear();
                MainForm->effects->Clear();
                MainForm->npcs->Clear();
                MainForm->map_items->ClearAll();
                MainForm->players->ReloadGraphicsLibraries();
                MainForm->npcs->ReloadGraphicsLibrary();
                MainForm->gfx->tiles->ReopenLibrary();
                MainForm->gfx->objects->ReopenLibrary();
                MainForm->gfx->masks->ReopenLibrary();
                MainForm->gfx->walls->ReopenLibrary();
                MainForm->gfx->tops->ReopenLibrary();
                MainForm->gfx->blends->ReopenLibrary();
                MainForm->gfx->items->ReopenLibrary();
                MainForm->map->LoadMap(new_map_id);
                if (MainForm->map->IsMinimapDisabled())
                    MainForm->gfx->interface_graphics->minimap_visible = false;
                ParseNearbyEntities(packet.SubString(5, packet.Length()));
                MainForm->animation->game->CenterViewOnPlayer(MainForm->net_client->GetPlayerId());
                if (warp_effect == 2)
                    MainForm->players->StartEffect(MainForm->net_client->GetPlayerId(), 3);
                if (MainForm->map->GetName() != "" && old_map_id != new_map_id)
                    MainForm->animation->interface_controller->chat->AddSystemMessage(3, 0,
                        MainForm->strings->GetText(GameText_YouEntered) + " " + MainForm->map->GetName(), false);
                if (MainForm->map->GetMapType() == PVP_MAP_TYPE)
                {
                    MainForm->animation->interface_controller->chat->AddSystemMessage(3, 0,
                        MainForm->strings->GetText(GameText_CautionThisIsPlayer), false);
                    MainForm->sfx->PlaySfx(SfxSound_CautionThisPlayerKill);
                }
            }
            MainForm->map->StopWarpTimeout();
            MainForm->map->warp_pending = false;
            MainForm->map->UnfreezePlayer();
        }
        if (action == AC_REQUEST)
        {
            MainForm->animation->trade->is_visible = 0;
            MainForm->animation->chest->is_visible = 0;
            MainForm->animation->guild->is_visible = 0;
            MainForm->animation->shop->is_visible = 0;
            MainForm->animation->bank->is_visible = 0;
            MainForm->animation->interface_controller->spells->CancelCast();
            MainForm->players->HideTextLabel(MainForm->net_client->GetPlayerId());
            MainForm->map->map_data = packet;
            MainForm->map->StartWarpTimeout();
            MainForm->map->path_finder->CancelPath();
            if (!MainForm->map->IsScrolling() && !MainForm->map->IsWarpDelayed())
                MainForm->map->AcceptPendingWarp();
        }
    }
}

// Opens a player's book: six texts, the class, then one line per token until an empty one.
void PacketProcessor::ParseBook(String packet)
{
    MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
    String first_text_token = Client::Capitalize(MainForm->net_client, MainForm->net_client->NextToken());
    String second_text_token = Client::Capitalize(MainForm->net_client, MainForm->net_client->NextToken());
    String third_text_token = Client::Capitalize(MainForm->net_client, MainForm->net_client->NextToken());
    String fourth_text_token = Client::Capitalize(MainForm->net_client, MainForm->net_client->NextToken());
    String fifth_text_token = Client::Capitalize(MainForm->net_client, MainForm->net_client->NextToken());
    String sixth_text_token = Client::Capitalize(MainForm->net_client, MainForm->net_client->NextToken());
    String details = MainForm->net_client->NextToken();
    int player_id = DecodeNumber(details.SubString(1, 2));
    int class_id = DecodeNumber(details.SubString(3, 1));
    int frame_row = DecodeNumber(details.SubString(4, 1));
    int last_int_arg = DecodeNumber(details.SubString(5, 1));
    int icon_kind = DecodeNumber(details.SubString(6, 1));
    String class_name = MainForm->class_values->GetName(class_id);
    class_name = Client::Capitalize(MainForm->net_client, class_name);
    MainForm->animation->book->Open(frame_row, first_text_token, second_text_token, third_text_token, fourth_text_token,
        class_name, fifth_text_token, sixth_text_token, icon_kind, last_int_arg);
    String entry;
    do
    {
        entry = MainForm->net_client->NextToken();
        if (entry.Length() > 0)
            MainForm->animation->book->lines->Add(entry);
    } while (entry.Length() > 0);
    MainForm->redraw_needed = true;
}

// Opens a player's paperdoll with the item in each slot; the player's own one can be changed.
void PacketProcessor::ParsePaperdoll(String packet)
{
    MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
    String top_row_text = Client::Capitalize(MainForm->net_client, MainForm->net_client->NextToken());
    String second_row_text = Client::Capitalize(MainForm->net_client, MainForm->net_client->NextToken());
    String fourth_row_text = Client::Capitalize(MainForm->net_client, MainForm->net_client->NextToken());
    String fifth_row_text = Client::Capitalize(MainForm->net_client, MainForm->net_client->NextToken());
    String sixth_row_text = Client::Capitalize(MainForm->net_client, MainForm->net_client->NextToken());
    String seventh_row_text = Client::Capitalize(MainForm->net_client, MainForm->net_client->NextToken());
    String details = MainForm->net_client->NextToken();
    int player_id = DecodeNumber(details.SubString(1, 2));
    int class_id = DecodeNumber(details.SubString(3, 1));
    int frame_row = DecodeNumber(details.SubString(4, 1));
    int last_int_arg = DecodeNumber(details.SubString(5, 1));
    int boots_item = DecodeNumber(details.SubString(6, 2));
    int small_slot_item = DecodeNumber(details.SubString(8, 2));
    int gloves_item = DecodeNumber(details.SubString(10, 2));
    int belt_item = DecodeNumber(details.SubString(12, 2));
    int armor_item = DecodeNumber(details.SubString(14, 2));
    int necklace_item = DecodeNumber(details.SubString(16, 2));
    int head_item = DecodeNumber(details.SubString(18, 2));
    int shield_item = DecodeNumber(details.SubString(20, 2));
    int weapon_item = DecodeNumber(details.SubString(22, 2));
    int top_pair_first_item = DecodeNumber(details.SubString(24, 2));
    int top_pair_second_item = DecodeNumber(details.SubString(26, 2));
    int middle_pair_first_item = DecodeNumber(details.SubString(28, 2));
    int middle_pair_second_item = DecodeNumber(details.SubString(30, 2));
    int bottom_pair_first_item = DecodeNumber(details.SubString(32, 2));
    int bottom_pair_second_item = DecodeNumber(details.SubString(34, 2));
    int icon_kind = DecodeNumber(details.SubString(36, 1));
    bool is_own_paperdoll = 0;
    if (MainForm->net_client->GetPlayerId() == player_id)
        is_own_paperdoll = 1;
    String class_name = MainForm->class_values->GetName(class_id);
    class_name = Client::Capitalize(MainForm->net_client, class_name);
    MainForm->animation->paperdoll->Open(frame_row, top_row_text, second_row_text, fourth_row_text, fifth_row_text,
        class_name, sixth_row_text, seventh_row_text, icon_kind, last_int_arg, is_own_paperdoll);
    MainForm->animation->paperdoll->SetBootsSlot(boots_item);
    MainForm->animation->paperdoll->SetType16Slot(small_slot_item);
    MainForm->animation->paperdoll->SetGlovesSlot(gloves_item);
    MainForm->animation->paperdoll->SetBeltSlot(belt_item);
    MainForm->animation->paperdoll->SetArmorSlot(armor_item);
    MainForm->animation->paperdoll->SetNecklaceSlot(necklace_item);
    MainForm->animation->paperdoll->SetHeadSlot(head_item);
    MainForm->animation->paperdoll->SetShieldSlot(shield_item);
    MainForm->animation->paperdoll->SetWeaponSlot(weapon_item);
    MainForm->animation->paperdoll->SetType19LeftSlot(top_pair_first_item);
    MainForm->animation->paperdoll->SetType19RightSlot(top_pair_second_item);
    MainForm->animation->paperdoll->SetType20LeftSlot(middle_pair_first_item);
    MainForm->animation->paperdoll->SetType20RightSlot(middle_pair_second_item);
    MainForm->animation->paperdoll->SetType21LeftSlot(bottom_pair_first_item);
    MainForm->animation->paperdoll->SetType21RightSlot(bottom_pair_second_item);
    MainForm->redraw_needed = true;
}

// Changes how a player in view looks; the third byte says what changed:
// 1 = boots, armor, hat, weapon and shield
// 2 = hair style and color
// 3 = hair color
void PacketProcessor::ApplyAppearanceChange(String packet)
{
    int change_kind;
    int player_id = this->DecodeNumber(packet.SubString(1, 2));
    if (MainForm->players->HasPlayer(player_id))
    {
        int v2b = this->DecodeNumber(packet.SubString(3, 1));
        int play_sound = DecodeNumber(packet.SubString(4, 1));
        if (v2b == AppearanceChange_Equipment)
        {
            MainForm->players->SetBoots(player_id, DecodeNumber(packet.SubString(5, 2)));
            MainForm->players->SetArmor(player_id, DecodeNumber(packet.SubString(7, 2)));
            MainForm->players->SetHat(player_id, DecodeNumber(packet.SubString(9, 2)));
            MainForm->players->SetWeapon(player_id, DecodeNumber(packet.SubString(11, 2)));
            MainForm->players->SetShield(player_id, DecodeNumber(packet.SubString(13, 2)));
            MainForm->players->ReloadGraphics(player_id);
            if (play_sound >= 1)
                MainForm->sfx->PlaySfx(SfxSound_ApplyAppearanceChange);
        }
        if (v2b == AppearanceChange_Hair)
        {
            MainForm->players->SetHairStyle(player_id, DecodeNumber(packet.SubString(5, 1)));
            MainForm->players->SetHairColor(player_id, DecodeNumber(packet.SubString(6, 1)));
            MainForm->players->ReloadGraphics(player_id);
            if (play_sound >= 1)
                MainForm->sfx->PlaySfx(SfxSound_ApplyAppearanceChange);
        }
        if (v2b == AppearanceChange_HairColor)
        {
            MainForm->players->SetHairColor(player_id, DecodeNumber(packet.SubString(5, 1)));
            MainForm->players->ReloadGraphics(player_id);
            if (play_sound >= 1)
                MainForm->sfx->PlaySfx(SfxSound_ApplyAppearanceChange);
        }
    }
}

// The player's own character from the first welcome reply: names, stats, worn items and the command delays.
void PacketProcessor::ParseWelcomeCharacter(String packet)
{
    MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
    MainForm->animation->interface_controller->status->character_name = MainForm->net_client->NextToken();
    MainForm->animation->interface_controller->status->character_second_text = MainForm->net_client->NextToken();
    MainForm->animation->interface_controller->status->guild_name = MainForm->net_client->NextToken();
    MainForm->animation->interface_controller->status->guild_rank_name = MainForm->net_client->NextToken();
    String character_data = MainForm->net_client->NextToken();
    MainForm->net_client->SetAdminLevel(DecodeNumber(character_data.SubString(5, 1)));
    MainForm->animation->interface_controller->status->class_id = DecodeNumber(character_data.SubString(1, 1));
    MainForm->animation->interface_controller->status->guild_tag = character_data.SubString(2, 3);
    MainForm->animation->interface_controller->status->SetLevel(DecodeNumber(character_data.SubString(6, 1)));
    MainForm->animation->interface_controller->status->SetExperience(DecodeNumber(character_data.SubString(7, 4)));
    MainForm->animation->interface_controller->status->usage_minutes = DecodeNumber(character_data.SubString(11, 4));
    MainForm->animation->interface_controller->status->usage_start_time = Now();
    MainForm->animation->interface_controller->status->SetHealth(DecodeNumber(character_data.SubString(15, 2)));
    MainForm->animation->interface_controller->status->SetMaxHealth(DecodeNumber(character_data.SubString(17, 2)));
    MainForm->animation->interface_controller->status->SetMana(DecodeNumber(character_data.SubString(19, 2)));
    MainForm->animation->interface_controller->status->SetMaxMana(DecodeNumber(character_data.SubString(21, 2)));
    MainForm->animation->interface_controller->status->SetStamina(DecodeNumber(character_data.SubString(23, 2)));
    MainForm->animation->interface_controller->status->SetMaxStamina(DecodeNumber(character_data.SubString(23, 2)));
    MainForm->animation->interface_controller->status->stat_points = DecodeNumber(character_data.SubString(25, 2));
    MainForm->animation->interface_controller->status->skill_points = DecodeNumber(character_data.SubString(27, 2));
    MainForm->animation->interface_controller->status->alignment = DecodeNumber(character_data.SubString(29, 2)) - 1000;
    MainForm->animation->interface_controller->status->label_413_from = DecodeNumber(character_data.SubString(31, 2));
    MainForm->animation->interface_controller->status->label_413_to = DecodeNumber(character_data.SubString(33, 2));
    MainForm->animation->interface_controller->status->label_414_value = DecodeNumber(character_data.SubString(35, 2));
    MainForm->animation->interface_controller->status->label_416_value = DecodeNumber(character_data.SubString(37, 2));
    MainForm->animation->interface_controller->status->label_415_value = DecodeNumber(character_data.SubString(39, 2));
    MainForm->animation->interface_controller->status->stat_str = DecodeNumber(character_data.SubString(41, 2));
    MainForm->animation->interface_controller->status->stat_wis = DecodeNumber(character_data.SubString(43, 2));
    MainForm->animation->interface_controller->status->stat_int = DecodeNumber(character_data.SubString(45, 2));
    MainForm->animation->interface_controller->status->stat_agi = DecodeNumber(character_data.SubString(47, 2));
    MainForm->animation->interface_controller->status->stat_con = DecodeNumber(character_data.SubString(49, 2));
    MainForm->animation->interface_controller->status->stat_cha = DecodeNumber(character_data.SubString(51, 2));
    MainForm->animation->paperdoll->equipped_boots = DecodeNumber(character_data.SubString(53, 2));
    MainForm->animation->paperdoll->equipped_gloves = DecodeNumber(character_data.SubString(55, 2));
    MainForm->animation->paperdoll->equipped_type16 = DecodeNumber(character_data.SubString(57, 2));
    MainForm->animation->paperdoll->equipped_armor = DecodeNumber(character_data.SubString(59, 2));
    MainForm->animation->paperdoll->equipped_belt = DecodeNumber(character_data.SubString(61, 2));
    MainForm->animation->paperdoll->equipped_necklace = DecodeNumber(character_data.SubString(63, 2));
    MainForm->animation->paperdoll->equipped_head = DecodeNumber(character_data.SubString(65, 2));
    MainForm->animation->paperdoll->equipped_shield = DecodeNumber(character_data.SubString(67, 2));
    MainForm->animation->paperdoll->equipped_weapon = DecodeNumber(character_data.SubString(69, 2));
    MainForm->animation->paperdoll->equipped_type19_left = DecodeNumber(character_data.SubString(71, 2));
    MainForm->animation->paperdoll->equipped_type19_right = DecodeNumber(character_data.SubString(73, 2));
    MainForm->animation->paperdoll->equipped_type20_left = DecodeNumber(character_data.SubString(75, 2));
    MainForm->animation->paperdoll->equipped_type20_right = DecodeNumber(character_data.SubString(77, 2));
    MainForm->animation->paperdoll->equipped_type21_left = DecodeNumber(character_data.SubString(79, 2));
    MainForm->animation->paperdoll->equipped_type21_right = DecodeNumber(character_data.SubString(81, 2));
    MainForm->animation->interface_controller->status->guild_rank = DecodeNumber(character_data.SubString(83, 1));
    MainForm->animation->game->restricted_map_id = DecodeNumber(character_data.SubString(84, 2));
    MainForm->animation->game->welcome_value_a = DecodeNumber(character_data.SubString(86, 2));
    MainForm->animation->game->welcome_value_b = DecodeNumber(character_data.SubString(88, 1));
    MainForm->animation->game->welcome_value_c = DecodeNumber(character_data.SubString(89, 1));
    int low_rank_delay = DecodeNumber(character_data.SubString(90, 2));
    int middle_rank_delay = DecodeNumber(character_data.SubString(92, 2));
    int high_rank_delay = DecodeNumber(character_data.SubString(94, 2));
    int top_rank_delay = DecodeNumber(character_data.SubString(96, 2));
    // The delay between commands goes by admin level; levels 1 and 2 share the first.
    int admin_level = DecodeNumber(character_data.SubString(5, 1));
    if (admin_level == 1)
        MainForm->animation->SetCommandDelay(low_rank_delay);
    if (admin_level == 2)
        MainForm->animation->SetCommandDelay(low_rank_delay);
    if (admin_level == 3)
        MainForm->animation->SetCommandDelay(middle_rank_delay);
    if (admin_level == 4)
        MainForm->animation->SetCommandDelay(high_rank_delay);
    if (admin_level == 5)
        MainForm->animation->SetCommandDelay(top_rank_delay);
    if (DecodeNumber(character_data.SubString(98, 1)) > 1)
        MainForm->animation->interface_controller->welcome_notice_pending = true;
}

// The known spells, 4 bytes each: id and level.
void PacketProcessor::ParseSpellList(String packet)
{
    MainForm->animation->interface_controller->spells->known_spells.clear();
    int spell_count = packet.Length() / 4;
    for (int i = 0; i < spell_count; i++)
    {
        int spell_id = DecodeNumber(packet.SubString(4 * i + 1, 2));
        int spell_level = DecodeNumber(packet.SubString(4 * i + 3, 2));
        MainForm->animation->interface_controller->spells->AddKnownSpell(spell_id, spell_level);
    }
}

// Weight, max weight, then the items, 6 bytes each; gold is item 1 and keeps a slot even at 0.
void PacketProcessor::ParseInventory(String packet)
{
    MainForm->animation->interface_controller->status->weight = DecodeNumber(packet.SubString(1, 1));
    MainForm->animation->interface_controller->status->max_weight = DecodeNumber(packet.SubString(2, 1));
    packet.Delete(1, 2);
    MainForm->animation->interface_controller->inventory->loaded_items.clear();
    int item_count = packet.Length() / 6;
    char has_gold = 0;
    for (int i = 0; i < item_count; i++)
    {
        int item_id = DecodeNumber(packet.SubString(6 * i + 1, 2));
        int amount = DecodeNumber(packet.SubString(6 * i + 3, 4));
        if (item_id == 1)
        {
            MainForm->animation->interface_controller->status->gold = amount;
            has_gold = 1;
        }
        MainForm->animation->interface_controller->inventory->AddLoadedItem(item_id, amount);
    }
    if (!has_gold)
    {
        MainForm->animation->interface_controller->status->gold = 0;
        MainForm->animation->interface_controller->inventory->AddLoadedItem(1, 0);
    }
}

void Packetproces_UnusedLiterals1()
{
    "";
}

// What is in view: a player count and two tokens per player, then 6 bytes per npc and 9 per item.
void PacketProcessor::ParseNearbyEntities(String packet)
{
    int player_count = this->DecodeNumber(String(packet[1]));
    MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
    MainForm->net_client->NextToken();
    for (int i = 0; i < player_count; i++)
    {
        String player_data = MainForm->net_client->NextToken();
        String own_player_data = MainForm->net_client->NextToken();
        if (own_player_data.Length() >= 42)
            MainForm->players->AddPlayer(DecodeNumber(own_player_data.SubString(1, 2)), player_data, "",
                DecodeNumber(own_player_data.SubString(3, 2)), DecodeNumber(own_player_data.SubString(5, 2)),
                DecodeNumber(own_player_data.SubString(7, 2)), DecodeNumber(own_player_data.SubString(9, 1)),
                DecodeNumber(own_player_data.SubString(10, 1)), own_player_data.SubString(11, 3),
                DecodeNumber(own_player_data.SubString(14, 1)), DecodeNumber(own_player_data.SubString(15, 1)),
                DecodeNumber(own_player_data.SubString(16, 1)), DecodeNumber(own_player_data.SubString(17, 1)),
                DecodeNumber(own_player_data.SubString(18, 1)), DecodeNumber(own_player_data.SubString(19, 2)),
                DecodeNumber(own_player_data.SubString(21, 2)), DecodeNumber(own_player_data.SubString(23, 2)),
                DecodeNumber(own_player_data.SubString(25, 2)), DecodeNumber(own_player_data.SubString(27, 2)),
                DecodeNumber(own_player_data.SubString(29, 2)), DecodeNumber(own_player_data.SubString(31, 2)),
                DecodeNumber(own_player_data.SubString(33, 2)), DecodeNumber(own_player_data.SubString(35, 2)),
                DecodeNumber(own_player_data.SubString(37, 2)), DecodeNumber(own_player_data.SubString(39, 2)),
                DecodeNumber(own_player_data.SubString(41, 2)), DecodeNumber(own_player_data.SubString(43, 2)),
                DecodeNumber(own_player_data.SubString(45, 1)), DecodeNumber(own_player_data.SubString(46, 1)),
                DecodeNumber(own_player_data.SubString(47, 1)));
    }
    String npc_data = MainForm->net_client->NextToken();
    if (npc_data.Length() >= 1)
    {
        int npc_count = npc_data.Length() / 6;
        for (int j = 0; j < npc_count; j++)
        {
            int npc_id = DecodeNumber(npc_data.SubString(6 * j + 2, 2));
            int npc_graphic = MainForm->npc_values->GetGraphic(npc_id);
            String npc_name = MainForm->npc_values->GetName(npc_id);
            MainForm->npcs->AddNpc(DecodeNumber(npc_data.SubString(6 * j + 1, 1)), npc_id, npc_graphic, npc_name,
                DecodeNumber(npc_data.SubString(6 * j + 4, 1)), DecodeNumber(npc_data.SubString(6 * j + 5, 1)),
                DecodeNumber(npc_data.SubString(6 * j + 6, 1)));
        }
    }
    String item_data = MainForm->net_client->NextToken();
    if (item_data.Length() >= 1)
    {
        int item_count = item_data.Length() / 9;
        for (int k = 0; k < item_count; k++)
            MainForm->map_items->AddItem(DecodeNumber(item_data.SubString(9 * k + 1, 2)),
                DecodeNumber(item_data.SubString(9 * k + 3, 2)), DecodeNumber(item_data.SubString(9 * k + 5, 1)),
                DecodeNumber(item_data.SubString(9 * k + 6, 1)), DecodeNumber(item_data.SubString(9 * k + 7, 3)));
        MainForm->map_items->SortByPosition();
    }
}

// The names of the players online: a count, then one name per token.
void PacketProcessor::ParseOnlineNames(String packet)
{
    MainForm->animation->social->ClearOnlineNames();
    int name_count = DecodeNumber(packet.SubString(1, 2));
    MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
    MainForm->net_client->NextToken();
    for (int i = 0; i < name_count; i++)
    {
        String name = MainForm->net_client->NextToken();
        MainForm->animation->social->AddOnlineName(name);
    }
    MainForm->redraw_needed = true;
}

// Opens the online player list; a guild tag of 0 or blanks shows as -.
void PacketProcessor::ParseOnlinePlayers(String packet)
{
    MainForm->animation->interface_controller->player_list->Clear();
    MainForm->animation->interface_controller->player_list->SetFilter(0);
    int player_count = DecodeNumber(packet.SubString(1, 2));
    MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
    MainForm->net_client->NextToken();
    for (int i = 0; i < player_count; i++)
    {
        String name = MainForm->net_client->NextToken();
        String second_token = MainForm->net_client->NextToken();
        String info_token = MainForm->net_client->NextToken();
        int info_token_first_value = DecodeNumber(info_token.SubString(1, 1));
        int player_icon_kind = DecodeNumber(info_token.SubString(2, 1));
        int class_id = DecodeNumber(info_token.SubString(3, 1));
        String class_name = MainForm->class_values->GetName(class_id);
        class_name = Client::Capitalize(MainForm->net_client, class_name);
        String guild_tag = info_token.SubString(4, 3);
        if (guild_tag == "0  " || guild_tag == "   " || guild_tag == "0" || guild_tag == "")
            guild_tag = "-";
        MainForm->animation->interface_controller->player_list->AddPlayer(IntToStr(info_token_first_value),
            player_icon_kind, name, second_token, guild_tag, class_name);
    }
    MainForm->animation->interface_controller->player_list->ApplyFilter();
    MainForm->gfx->LoadPlayerlistPanel();
    MainForm->animation->SetSubState(GamePanel_Playerlist);
    MainForm->animation->interface_controller->ShowStatusMessage(StatusCategory_Action,
        MainForm->strings->GetText(GameText_OnlinePlayerlistPanelIs));
    MainForm->animation->interface_controller->Invalidate();
    MainForm->animation->interface_controller->player_list->MarkListDirty();
    MainForm->animation->interface_controller->player_list_button_ready = true;
}

// The party: one token per member (id, leader, level, health percent, name) until a short one.
void PacketProcessor::ParsePartyMembers(String packet)
{
    MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
    char has_more = 1;
    do
    {
        String member_data = MainForm->net_client->NextToken();
        if (member_data.Length() > 4)
        {
            int member_id = DecodeNumber(member_data.SubString(1, 2));
            int is_leader = DecodeNumber(member_data.SubString(3, 1));
            int member_level = DecodeNumber(member_data.SubString(4, 1));
            int member_health_percent = DecodeNumber(member_data.SubString(5, 1));
            String member_name = Client::Capitalize(MainForm->net_client,
                member_data.SubString(6, member_data.Length() - 5));
            char is_self = 0;
            if (MainForm->net_client->GetPlayerId() == member_id)
            {
                if (is_leader >= 1)
                    MainForm->animation->interface_controller->group->is_leader = 1;
                is_self = 1;
            }
            MainForm->animation->interface_controller->group->AddMember(member_id, member_name, member_level, is_leader,
                member_health_percent, is_self);
            if (MainForm->animation->interface_controller->group->is_leader)
                MainForm->animation->interface_controller->group->MarkMembersRemovable();
        }
        else
            has_more = 0;
    } while (has_more);
}

// The character slots of the select screen: the slots are players 999991 to 999993.
void PacketProcessor::ParseCharacterList(String packet)
{
    MainForm->players->SetSlotState(PlayerId_SlotFirst, 2);
    MainForm->players->SetSlotState(PlayerId_SlotSecond, 2);
    MainForm->players->SetSlotState(PlayerId_SlotThird, 2);
    int character_count = DecodeNumber(String(packet[1]));
    MainForm->net_client->BeginTokens(packet, ByteToChar(TOKEN_BREAK_BYTE));
    MainForm->net_client->NextToken();
    for (int i = 0; i < character_count; i++)
    {
        String character_name = MainForm->net_client->NextToken();
        String slot_data = MainForm->net_client->NextToken();
        if (slot_data.Length() >= 9)
        {
            MainForm->players->SetSlotState(i + 999991, 0);
            MainForm->players->SetName(i + 999991, Client::Capitalize(MainForm->net_client, character_name));
            MainForm->players->SetCharacterId(i + 999991, DecodeNumber(slot_data.SubString(1, 4)));
            MainForm->players->SetDisplayNumber(i + 999991, DecodeNumber(String(slot_data[5])));
            MainForm->players->SetGender(i + 999991, DecodeNumber(String(slot_data[6])));
            MainForm->players->SetHairStyle(i + 999991, DecodeNumber(String(slot_data[7])));
            MainForm->players->SetHairColor(i + 999991, DecodeNumber(String(slot_data[8])));
            MainForm->players->SetSkin(i + 999991, DecodeNumber(String(slot_data[9])));
            MainForm->players->SetBadgeLevel(i + 999991, DecodeNumber(String(slot_data[10])));
            MainForm->players->SetBoots(i + 999991, DecodeNumber(slot_data.SubString(11, 2)));
            MainForm->players->SetArmor(i + 999991, DecodeNumber(slot_data.SubString(13, 2)));
            MainForm->players->SetHat(i + 999991, DecodeNumber(slot_data.SubString(15, 2)));
            MainForm->players->SetShield(i + 999991, DecodeNumber(slot_data.SubString(17, 2)));
            MainForm->players->SetWeapon(i + 999991, DecodeNumber(slot_data.SubString(19, 2)));
            MainForm->players->ReloadGraphics(i + 999991);
        }
    }
}

// A number in base 253, low digit first, each digit plus 1; padded to min_length with 254.
String PacketProcessor::EncodeNumber(PacketProcessor *self, unsigned int value, int min_length)
{
    unsigned int remaining = value;
    unsigned int quotient;
    char digit;
    int i;
    String result = "";
    try
    {
        do
        {
            double quotient_real = remaining / 253.0;
            quotient = quotient_real;
            int remainder = remaining % NUMBER_ENCODING_BASE;
            digit = remainder + 1;
            result.Insert(digit, result.Length() + 1);
            if (quotient >= 1)
                remaining = quotient;
        } while (quotient >= 1);
    }
    catch (...)
    {
        result = "";
    }
    if (result.Length() < min_length)
    {
        char digit = -2;
        int pad_count = min_length - result.Length();
        for (i = 0; i < pad_count; i++)
            result.Insert(digit, result.Length() + 1);
    }
    return result;
}

// Reads up to four base-253 digits, low digit first; a 254 ends the number.
DEFINE_CLIENT_DECODE_NUMBER(PacketProcessor)

char PacketProcessor::ByteToChar(unsigned char value)
{
    return value;
}
