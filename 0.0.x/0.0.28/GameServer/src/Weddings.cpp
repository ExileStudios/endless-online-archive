#include <vcl.h>
#pragma hdrstop

#include "Weddings.h"
#include "Itemvalues.h"
#include "MainForm.h"
#include "Packets.h"
#include "Player.h"
#include "Players.h"
#include "Wedding.h"
#include "Constants.h"

#pragma package(smart_init)

WeddingController::WeddingController(Players *players, Packets *server)
{
    this->encode_scratch = new char[8];
    this->players = players;
    this->server = server;
}

WeddingController::~WeddingController()
{
}

// True while the priest holds a wedding on the map.
char WeddingController::Has(int map_id, int priest_npc_index)
{
    for (vector<Wedding *>::iterator it = active_weddings.begin(); it != active_weddings.end(); it++)
    {
        if ((*it)->map_id == map_id && (*it)->priest_npc_index == priest_npc_index)
            return 1;
    }
    return 0;
}

// Starts a wedding: the ceremony begins in 20 seconds.
void WeddingController::Add(int map_id, int priest_npc_index, int player1_id, String player1_name, int player2_id,
    String player2_name)
{
    Wedding *wedding = new Wedding(map_id, priest_npc_index);
    wedding->step = 0;
    wedding->countdown = 20;
    wedding->player1_id = player1_id;
    wedding->player2_id = player2_id;
    wedding->player1_name = player1_name;
    wedding->player2_name = player2_name;
    active_weddings.insert(active_weddings.end(), wedding);
}

// A partner says yes; once both have, each gets item 374.
void WeddingController::Confirm(int map_id, int priest_npc_index, int player_id)
{
    for (vector<Wedding *>::iterator it = active_weddings.begin(); it != active_weddings.end(); it++)
    {
        if ((*it)->map_id != map_id)
            continue;
        if ((*it)->priest_npc_index != priest_npc_index)
            continue;
        if ((*it)->player1_id == player_id && !(*it)->player1_confirmed)
        {
            (*it)->player1_confirmed = true;
            (*it)->step++;
            (*it)->countdown = 20;
        }
        if ((*it)->player2_id == player_id && !(*it)->player2_confirmed)
        {
            (*it)->player2_confirmed = true;
            (*it)->step++;
            (*it)->countdown = 25;
        }
        if ((*it)->player1_confirmed)
        {
            if ((*it)->player2_confirmed)
            {
                Player *player1 = players->GetById((*it)->player1_id);
                Player *player2 = players->GetById((*it)->player2_id);
                if (player1)
                {
                    if (player2)
                    {
                        player1->partner_name = (*it)->player2_name;
                        player2->partner_name = (*it)->player1_name;
                        int item = 374;
                        players->Player_AddItem(player1, item, 1);
                        player1->weight_current += GUI->item_values->GetWeight(item);
                        int weight = player1->weight_current;
                        if (weight > WEIGHT_DISPLAY_MAX)
                            weight = WEIGHT_DISPLAY_MAX;
                        String data = EncodeNumber(item, 2);
                        data.Insert(EncodeNumber(1, 3), data.Length() + 1);
                        data.Insert(EncodeNumber(weight, 1), data.Length() + 1);
                        server->Client_SendEncoded(player1, AC_GIVE, OP_ITEM, data);
                        players->Player_AddItem(player2, item, 1);
                        player2->weight_current += GUI->item_values->GetWeight(item);
                        weight = player2->weight_current;
                        if (weight > WEIGHT_DISPLAY_MAX)
                            weight = WEIGHT_DISPLAY_MAX;
                        data = EncodeNumber(item, 2);
                        data.Insert(EncodeNumber(1, 3), data.Length() + 1);
                        data.Insert(EncodeNumber(weight, 1), data.Length() + 1);
                        server->Client_SendEncoded(player2, AC_GIVE, OP_ITEM, data);
                    }
                }
            }
        }
    }
}

// Once a second: the ceremony's lines, questions and answers, each at its second of the countdown.
void WeddingController::Tick()
{
    for (vector<Wedding *>::iterator it = active_weddings.begin(); it != active_weddings.end(); it++)
    {
        if ((*it)->countdown <= 0)
            continue;
        (*it)->countdown--;
        // .. step 0: the wait before the ceremony
        if (!(*it)->step)
        {
            if ((*it)->countdown == 19)
                BroadcastPriestLine(*it, "Very well, the ceremony will start in 20 seconds.");
            if ((*it)->countdown == 18)
                server->Server_BroadcastToMap((*it)->map_id, AC_SET, OP_MUSIC,
                    EncodeNumber(40, 1));
        }
        // .. step 1: the welcome, then the question to the first partner
        if ((*it)->step == 1)
        {
            if ((*it)->countdown == 25)
            {
                String welcome_line = "we are here at the invitation of ";
                welcome_line.Insert((*it)->player1_name, welcome_line.Length() + 1);
                welcome_line.Insert(" and ", welcome_line.Length() + 1);
                welcome_line.Insert((*it)->player2_name, welcome_line.Length() + 1);
                welcome_line.Insert(", who have come before us to join together in marriage.",
                    welcome_line.Length() + 1);
                BroadcastPriestLine(*it, welcome_line);
            }
            if ((*it)->countdown == 19)
                BroadcastPriestLine(*it,
                    "their relationship is based on love, respect, and a determination to face the future together in "
                    "health or sickness, in joy and sorrow.");
            if ((*it)->countdown == 13)
            {
                String first_vow_line = (*it)->player1_name;
                first_vow_line.Insert(", do you take ", first_vow_line.Length() + 1);
                first_vow_line.Insert((*it)->player2_name, first_vow_line.Length() + 1);
                first_vow_line.Insert(" to be your partner, and promise to love, comfort and stay together as long as "
                    "you both shall live?",
                    first_vow_line.Length() + 1);
                BroadcastPriestLine(*it, first_vow_line);
            }
            if ((*it)->countdown == 10 && BothPresent(*it))
            {
                Player *opening_player = players->GetById((*it)->player1_id);
                if (opening_player)
                    server->Client_SendEncoded(opening_player, AC_RESULT, OP_PRIEST,
                        EncodeNumber(PriestReply_DoYou, 2));
            }
        }
        // .. step 2: the first yes, then the question to the second partner
        if ((*it)->step == 2)
        {
            if ((*it)->countdown == 19)
            {
                Player *first_partner = players->GetById((*it)->player1_id);
                if (first_partner)
                {
                    String first_answer_data = EncodeNumber((*it)->player1_id, 2);
                    first_answer_data.Insert("Yes, i do", first_answer_data.Length() + 1);
                    server->Server_BroadcastToMap((*it)->map_id, AC_SET, OP_CHAT,
                        first_answer_data);
                }
            }
            if ((*it)->countdown == 13)
            {
                String first_answer_line = (*it)->player2_name;
                first_answer_line.Insert(", do you take ", first_answer_line.Length() + 1);
                first_answer_line.Insert((*it)->player1_name, first_answer_line.Length() + 1);
                first_answer_line.Insert(" to be your partner, and promise to love, comfort and stay together as long "
                    "as you both shall live?",
                    first_answer_line.Length() + 1);
                BroadcastPriestLine(*it, first_answer_line);
            }
            if ((*it)->countdown == 10 && BothPresent(*it))
            {
                Player *first_answer_player = players->GetById((*it)->player2_id);
                if (first_answer_player)
                    server->Client_SendEncoded(first_answer_player, AC_RESULT, OP_PRIEST,
                        EncodeNumber(PriestReply_DoYou, 2));
            }
        }
        // .. step 3: the second yes, the rings and the end
        if ((*it)->step == 3)
        {
            if ((*it)->countdown == 24)
            {
                Player *second_partner = players->GetById((*it)->player2_id);
                if (second_partner)
                {
                    String second_yes_data = EncodeNumber((*it)->player2_id, 2);
                    second_yes_data.Insert("Yes, i do", second_yes_data.Length() + 1);
                    server->Server_BroadcastToMap((*it)->map_id, AC_SET, OP_CHAT,
                        second_yes_data);
                }
            }
            if ((*it)->countdown == 18)
            {
                if (BothPresent(*it))
                    BroadcastPriestLine(*it,
                        "Let these rings be given and received as a token of your affection, sincerity and trust in "
                        "one another.");
                else
                    (*it)->countdown == 1;
            }
            if ((*it)->countdown == 12)
                BroadcastPriestLine(*it, "Please place these rings on eachothers finger..");
            if ((*it)->countdown == 7)
            {
                if (BothPresent(*it))
                {
                    String rings_data = EncodeNumber((*it)->player1_id, 2);
                    rings_data.Insert(EncodeNumber(1, 3), rings_data.Length() + 1);
                    rings_data.Insert(EncodeNumber((*it)->player2_id, 2), rings_data.Length() + 1);
                    rings_data.Insert(EncodeNumber(1, 3), rings_data.Length() + 1);
                    server->Server_BroadcastToMap((*it)->map_id, AC_SET, OP_EFFECT, rings_data);
                }
                else
                    (*it)->countdown == 1;
            }
            if ((*it)->countdown == 6)
            {
                if (BothPresent(*it))
                {
                    String second_answer_line = (*it)->player1_name;
                    second_answer_line.Insert(" and ", second_answer_line.Length() + 1);
                    second_answer_line.Insert((*it)->player2_name, second_answer_line.Length() + 1);
                    second_answer_line.Insert(" have consented together in marriage. And are now partners for as long "
                        "you both shall live.",
                        second_answer_line.Length() + 1);
                    BroadcastPriestLine(*it, second_answer_line);
                    Player *player1 = players->GetById((*it)->player1_id);
                    Player *player2 = players->GetById((*it)->player2_id);
                    if (player1 && player2)
                    {
                        String final_data = EncodeNumber(player1->x, 1);
                        final_data.Insert(EncodeNumber(player1->y, 1), final_data.Length() + 1);
                        final_data.Insert(EncodeNumber(11, 2), final_data.Length() + 1);
                        final_data.Insert(EncodeNumber(player2->x, 1), final_data.Length() + 1);
                        final_data.Insert(EncodeNumber(player2->y, 1), final_data.Length() + 1);
                        final_data.Insert(EncodeNumber(11, 2), final_data.Length() + 1);
                        server->Server_BroadcastToMap((*it)->map_id, AC_UPDATE, OP_EFFECT,
                            final_data);
                    }
                }
                else
                    (*it)->countdown == 1;
            }
            if ((*it)->countdown == 1)
                BroadcastPriestLine(*it, "Congratulations to the couple!");
        }
        // .. the countdown ran out: the wedding ends, or after step 0 the ceremony starts
        if ((*it)->countdown <= 0)
        {
            if ((*it)->step > 0)
            {
                if ((*it)->step == 1 || (*it)->step == 2)
                    BroadcastPriestLine(*it, "Im sorry, something went wrong..");
                Wedding *ended_wedding = *it;
                active_weddings.erase(it);
                delete ended_wedding;
                break;
            }
            if (!(*it)->step)
            {
                if (!BothPresent(*it))
                {
                    BroadcastPriestLine(*it, "Im sorry, something went wrong..");
                    Wedding *cancelled_wedding = *it;
                    active_weddings.erase(it);
                    delete cancelled_wedding;
                    break;
                }
                else
                {
                    (*it)->step = 1;
                    (*it)->countdown = 26;
                }
            }
        }
    }
}

// The priest says a line to the whole map.
void WeddingController::BroadcastPriestLine(Wedding *record, String text)
{
    String data = EncodeNumber(record->priest_npc_index, 2);
    data.Insert(text, data.Length() + 1);
    server->Server_BroadcastToMap(record->map_id, AC_TALK, OP_NPC, data);
}

// True while both partners are online and on the wedding's map.
bool WeddingController::BothPresent(Wedding *record)
{
    Player *player1 = players->GetById(record->player1_id);
    Player *player2 = players->GetById(record->player2_id);
    if (!player1 || !player2)
        return false;
    if (player1->map_id != record->map_id || player2->map_id != record->map_id)
        return false;
    return true;
}

// Writes a number the protocol's way, base 253 and padded with 254; a number too big for width gets more bytes.
DEFINE_ENCODE_NUMBER(WeddingController)
