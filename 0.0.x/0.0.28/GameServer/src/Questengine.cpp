#include <vcl.h>
#pragma hdrstop

#include "Questengine.h"
#include "Quest.h"
#include "Queststate.h"
#include "Questtype.h"
#include "Settings.h"
#include "Constants.h"

#pragma package(smart_init)

// Registers the action and condition names the quest files use, then loads the quests.
QuestContainer::QuestContainer(Settings *settings)
{
    this->settings = settings;
    this->max_quests = this->settings->GetMaxQuests();
    this->encode_scratch = new char[8];
    this->RegisterAction(QuestActionId_AddNpcText, "AddNpcText");
    this->RegisterAction(QuestActionId_AddNpcInput, "AddNpcInput");
    this->RegisterAction(QuestActionId_AddNpcChat, "AddNpcChat");
    this->RegisterAction(QuestActionId_SetMap, "SetMap");
    this->RegisterAction(QuestActionId_GiveItem, "GiveItem");
    this->RegisterAction(QuestActionId_RemoveItem, "RemoveItem");
    this->RegisterAction(QuestActionId_End, "End");
    this->RegisterAction(QuestActionId_Reset, "Reset");
    this->RegisterAction(QuestActionId_SetClass, "SetClass");
    this->RegisterAction(QuestActionId_PlayMusic, "PlayMusic");
    this->RegisterAction(QuestActionId_PlaySound, "PlaySound");
    this->RegisterAction(QuestActionId_ShowHint, "ShowHint");
    this->RegisterAction(QuestActionId_GiveExp, "GiveExp");
    this->RegisterAction(QuestActionId_RemoveExp, "RemoveExp");
    this->RegisterAction(QuestActionId_GiveKarma, "GiveKarma");
    this->RegisterAction(QuestActionId_RemoveKarma, "RemoveKarma");
    this->RegisterAction(QuestActionId_Quake, "Quake");
    this->RegisterAction(QuestActionId_EffectOnPlayer, "EffectOnPlayer");
    this->RegisterAction(QuestActionId_EffectOnCoord, "EffectOnCoord");
    this->RegisterAction(QuestActionId_ResetDaily, "ResetDaily");
    this->RegisterCondition(QuestCondition_TalkedToNpc, "TalkedToNpc");
    this->RegisterCondition(QuestCondition_InputNpc, "InputNpc");
    this->RegisterCondition(QuestCondition_GotItems, "GotItems");
    this->RegisterCondition(QuestCondition_LostItems, "LostItems");
    this->RegisterCondition(QuestCondition_Die, "Die");
    this->RegisterCondition(QuestCondition_Disconnect, "Disconnect");
    this->RegisterCondition(QuestCondition_TimeElapsed, "TimeElapsed");
    this->RegisterCondition(QuestCondition_KilledNpcs, "KilledNpcs");
    this->RegisterCondition(QuestCondition_KilledPlayers, "KilledPlayers");
    this->RegisterCondition(QuestCondition_EnterCoord, "EnterCoord");
    this->RegisterCondition(QuestCondition_EnterMap, "EnterMap");
    this->RegisterCondition(QuestCondition_LeaveMap, "LeaveMap");
    this->RegisterCondition(QuestCondition_Always, "Always");
    this->RegisterCondition(QuestCondition_DoneDaily, "DoneDaily");
    this->LoadQuests();
}

QuestContainer::~QuestContainer()
{
}

// Loads quests 1 to maxquests.
void QuestContainer::LoadQuests()
{
    quest_list.clear();
    for (int quest_id = 1; quest_id <= 64000 && max_quests >= quest_id; quest_id++)
        LoadQuest(quest_id);
}

// Reads quests/<id>.txt ( five digits ) token by token, then points each rule's goto at its state.
bool QuestContainer::LoadQuest(int quest_id)
{
    String text;
    String unused_str;
    int handle;
    int file_size;
    char *file_buf;
    Quest *quest = new Quest(quest_id);
    quest_list.insert(quest_list.end(), quest);
    try
    {
        text = IntToStr(quest_id);
        for (int i = text.Length(); i <= 4; i++)
            text.Insert("0", 0);
        text.Insert("./quests/", 0);
        text.Insert(".txt", text.Length() + 1);
        handle = FileOpen(text.c_str(), 0);
        if (handle < 0)
            return false;
        file_size = FileSeek(handle, 0, 2);
        FileSeek(handle, 0, 0);
        file_buf = new char[file_size + 1];
        FileRead(handle, file_buf, file_size);
        FileClose(handle);
        text = file_buf;
        text.SetLength(file_size);
        delete[] file_buf;
        expect_brace = false;
        expect_state_name = false;
        in_state = false;
        expect_quest_name = false;
        expect_version = false;
        expect_desc = false;
        expect_goto_name = false;
        expect_goto = false;
        state_count = 0;
        action_type = 0;
        rule_type = QuestCondition_None;
        int pos = 1;
        char in_quote = 0;
        char in_comment = 0;
        // .. a token ends at a space, tab, comma, ; or line end
        // ( ) { } stand alone, "a text" is one token, and // starts a comment
        while (text.Length() > 0)
        {
            if (in_quote)
            {
                if (text[pos] == '"')
                {
                    in_quote = 0;
                    if (pos > 1)
                        ParseToken(quest, text.SubString(1, pos - 1));
                    text.Delete(1, pos);
                    pos = 1;
                }
                else
                {
                    if (text[pos] == '\n')
                    {
                        in_quote = 0;
                        if (pos > 2)
                            ParseToken(quest, text.SubString(1, pos - 2));
                        text.Delete(1, pos);
                        pos = 1;
                    }
                    else
                    {
                        if (text.Length() == pos)
                            break;
                        pos++;
                    }
                }
            }
            else if (in_comment)
            {
                if (text[pos] == '\n')
                {
                    in_comment = 0;
                    text.Delete(1, pos);
                    pos = 1;
                }
                else
                {
                    if (text.Length() == pos)
                        break;
                    pos++;
                }
            }
            else if (text[pos] == '"')
            {
                in_quote = 1;
                text.Delete(1, pos);
                pos = 1;
            }
            else if (text[pos] == '/' && text.Length() != pos && text[pos + 1] == '/')
            {
                in_comment = 1;
                text.Delete(1, pos + 1);
                pos = 1;
            }
            else
            {
                if (text[pos] == ' ')
                {
                    if (pos > 1)
                        ParseToken(quest, text.SubString(1, pos - 1));
                    text.Delete(1, pos);
                    pos = 1;
                }
                else
                {
                    if (text[pos] == ',')
                    {
                        if (pos > 1)
                            ParseToken(quest, text.SubString(1, pos - 1));
                        text.Delete(1, pos);
                        pos = 1;
                    }
                    else
                    {
                        if (text[pos] == '(' || text[pos] == ')' || text[pos] == '{' || text[pos] == '}')
                        {
                            if (pos > 1)
                                ParseToken(quest, text.SubString(1, pos - 1));
                            ParseToken(quest, String(text[pos]));
                            text.Delete(1, pos);
                            pos = 1;
                        }
                        else
                        {
                            if (text[pos] == ';')
                            {
                                if (pos > 1)
                                    ParseToken(quest, text.SubString(1, pos - 1));
                                text.Delete(1, pos);
                                pos = 1;
                            }
                            else
                            {
                                if (text[pos] == '\t')
                                {
                                    if (pos > 1)
                                        ParseToken(quest, text.SubString(1, pos - 1));
                                    text.Delete(1, pos);
                                    pos = 1;
                                }
                                else
                                {
                                    if (text[pos] == '\n')
                                    {
                                        if (pos > 2)
                                            ParseToken(quest, text.SubString(1, pos - 2));
                                        text.Delete(1, pos);
                                        pos = 1;
                                    }
                                    else
                                    {
                                        if (text.Length() == pos)
                                        {
                                            if (pos > 0)
                                                ParseToken(quest, text);
                                            break;
                                        }
                                        pos++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    catch (...)
    {
        FileClose(handle);
        return false;
    }
    vector<QuestState *>::iterator step;
    vector<QuestState *>::iterator lookup;
    vector<QuestRule *>::iterator ref;
    // .. each rule's goto names a state: keep the state's index
    for (step = quest->states.begin(); step != quest->states.end(); step++)
    {
        for (ref = (*step)->rules.begin(); ref != (*step)->rules.end(); ref++)
        {
            for (lookup = quest->states.begin(); lookup != quest->states.end(); lookup++)
            {
                if ((*lookup)->name == (*ref)->goto_name)
                {
                    (*ref)->goto_state_index = (*lookup)->state_index;
                    break;
                }
            }
        }
    }
    quest->state_count = state_count;
    quest->loaded = 1;
    return true;
}

// Takes one token of a quest file into the quest being built:
// questname <name>, version <n>, state <name> { .. }
// inside a state: desc <text>, the actions Name( args ) and the rules Name( args ) goto <state>
void QuestContainer::ParseToken(Quest *quest, String token)
{
    if (in_state)
    {
        if (action_type > 0)
        {
            if (token == ")")
            {
                action_type = 0;
                in_action_args = false;
                return;
            }
            if (in_action_args)
            {
                for (int i = 0; i < QUEST_ACTION_ARGS; i++)
                {
                    if (current_action->data[i] == "")
                    {
                        current_action->data[i] = token; current_action->args[i] = ParseInt(token);
                        return;
                    }
                }
            }
            if (token == "(")
            {
                current_action = new QuestAction(action_type);
                current_state->actions.insert(current_state->actions.end(), current_action);
                in_action_args = true;
                return;
            }
        }
        if (expect_goto)
        {
            if (expect_goto_name)
            {
                current_rule->goto_name = token;
                expect_goto_name = true;
                expect_goto = false;
                return;
            }
            if (token == "goto")
            {
                expect_goto_name = true;
                return;
            }
            expect_goto = false;
        }
        if (rule_type > QuestCondition_None)
        {
            if (token == ")")
            {
                rule_type = QuestCondition_None;
                in_rule_args = false;
                expect_goto = true;
                return;
            }
            if (in_rule_args)
            {
                for (int i = 0; i < QUEST_ACTION_ARGS; i++)
                {
                    if (current_rule->data[i] == "")
                    {
                        current_rule->data[i] = token; current_rule->args[i] = ParseInt(token);
                        return;
                    }
                }
            }
            if (token == "(")
            {
                // the state keeps its first item, kill, coord or map rule to show its progress in the quest list
                if (!current_state->progress_condition_type &&
                    (rule_type == QuestCondition_GotItems || rule_type == QuestCondition_KilledNpcs ||
                    rule_type == QuestCondition_KilledPlayers || rule_type == QuestCondition_EnterCoord ||
                    rule_type == QuestCondition_EnterMap))
                {
                    current_state->progress_condition_type = rule_type;
                    current_state->progress_rule_index = (int)current_state->rules.size();
                }
                current_rule = new QuestRule(rule_type);
                current_state->rules.insert(current_state->rules.end(), current_rule);
                in_rule_args = true;
                expect_goto_name = false;
                expect_goto = false;
                return;
            }
        }
        if (LowerCase(token) == "desc")
        {
            expect_desc = true;
            return;
        }
        if (expect_desc)
        {
            current_state->description = token;
            expect_desc = false;
            return;
        }
        if (GetActionType(token) > 0)
        {
            action_type = GetActionType(token);
            in_action_args = false;
            return;
        }
        if (GetConditionType(token) > 0)
        {
            rule_type = GetConditionType(token);
            in_rule_args = false;
            return;
        }
    }
    if (LowerCase(token) == "questname")
    {
        expect_quest_name = true;
        return;
    }
    else
    {
        if (LowerCase(token) == "version")
        {
            expect_version = true;
            return;
        }
        else
        {
            if (LowerCase(token) == "state")
            {
                expect_brace = true;
                expect_state_name = true;
                return;
            }
            else
            {
                if (expect_quest_name)
                {
                    quest->name = token;
                    expect_quest_name = false;
                }
                if (expect_version)
                {
                    try
                    {
                        quest->version = StrToInt(token);
                    }
                    catch (...)
                    {
                    }
                    expect_version = false;
                }
                if (expect_state_name)
                {
                    current_state = new QuestState(state_count, token);
                    state_count++;
                    expect_state_name = false;
                }
                if (token == "{")
                {
                    if (expect_brace)
                    {
                        expect_brace = false;
                        in_state = true;
                        return;
                    }
                    else
                    {
                        expect_brace = false;
                        in_state = false;
                        return;
                    }
                }
                else
                {
                    if (token == "}")
                    {
                        if (in_state)
                        {
                            quest->states.insert(quest->states.end(), current_state);
                            expect_desc = false;
                            in_state = false;
                            return;
                        }
                    }
                }
            }
        }
    }
}

char QuestContainer::GetQuestLoaded(int quest_id)
{
    if (quest_id < 1 || quest_list.size() < (unsigned int)quest_id)
        return 0;
    else
        return quest_list[quest_id - 1]->loaded;
}

int QuestContainer::GetQuestVersion(int quest_id)
{
    if (quest_id < 1 || quest_list.size() < (unsigned int)quest_id)
        return 0;
    else
        return quest_list[quest_id - 1]->version;
}

String QuestContainer::GetQuestName(int quest_id)
{
    if (quest_id < 1 || quest_list.size() < (unsigned int)quest_id)
        return "";
    else
        return quest_list[quest_id - 1]->name;
}

void Questengine_UnusedLiterals1()
{
    "";
    "";
}

QuestState * &Questengine_Slot(vector<QuestState *> &list, int index)
{
    return list[index];
}

// The state's AddNpcChat lines for the npc, each ended by a break byte.
String QuestContainer::GetActionData(int quest_id, int state_index, int behavior_id)
{
    QuestState *state = GetState(quest_id, state_index);
    if (!state)
        return "";
    String data = "";
    for (vector<QuestAction *>::iterator it = state->actions.begin(); it != state->actions.end(); it++)
    {
        if ((*it)->args[0] == behavior_id && (*it)->action == QuestActionId_AddNpcChat)
        {
            data.Insert((*it)->data[1], data.Length() + 1);
            data.Insert((char)EO_BREAK_BYTE, data.Length() + 1);
        }
    }
    return data;
}

// The state's AddNpcText and AddNpcInput lines for the npc, each ended by a break byte.
String QuestContainer::GetActionData2(int quest_id, int state_index, int behavior_id)
{
    QuestState *state = GetState(quest_id, state_index);
    if (!state)
        return "";
    String data = "";
    for (vector<QuestAction *>::iterator it = state->actions.begin(); it != state->actions.end(); it++)
    {
        if ((*it)->args[0] != behavior_id)
            continue;
        if ((*it)->action == QuestActionId_AddNpcText)
        {
            data.Insert(EncodeNumber((*it)->action, 2), data.Length() + 1);
            data.Insert((*it)->data[1], data.Length() + 1);
            data.Insert((char)EO_BREAK_BYTE, data.Length() + 1);
        }
        if ((*it)->action == QuestActionId_AddNpcInput)
        {
            data.Insert(EncodeNumber((*it)->action, 2), data.Length() + 1);
            data.Insert(EncodeNumber((*it)->args[1], 2), data.Length() + 1);
            data.Insert((*it)->data[2], data.Length() + 1);
            data.Insert((char)EO_BREAK_BYTE, data.Length() + 1);
        }
    }
    return data;
}

void Questengine_UnusedLiterals2()
{
    "";
    "";
}

// The state a TalkedToNpc rule for the npc goes to, or -1.
int QuestContainer::GetRuleValue(int quest_id, int state_index, int behavior_id)
{
    QuestState *state = GetState(quest_id, state_index);
    if (!state)
        return -1;
    String unused = "";
    for (vector<QuestRule *>::iterator it = state->rules.begin(); it != state->rules.end(); it++)
    {
        if ((*it)->rule == QuestCondition_TalkedToNpc && (*it)->args[0] == behavior_id)
            return (*it)->goto_state_index;
    }
    return -1;
}

// The state an InputNpc rule for the input goes to, or -1.
int QuestContainer::GetRuleValue2(int quest_id, int state_index, int input_id)
{
    QuestState *state = GetState(quest_id, state_index);
    if (!state)
        return -1;
    String unused = "";
    for (vector<QuestRule *>::iterator it = state->rules.begin(); it != state->rules.end(); it++)
    {
        if ((*it)->rule == QuestCondition_InputNpc && (*it)->args[0] == input_id)
            return (*it)->goto_state_index;
    }
    return -1;
}

// The quest's state at state_index, or 0.
QuestState * QuestContainer::GetState(int quest_id, int state_index)
{
    if (quest_id < 1 || quest_list.size() < (unsigned int)quest_id)
        return 0;
    if (state_index < 0 || quest_list[quest_id - 1]->states.size() <= (unsigned int)state_index)
        return 0;
    return quest_list[quest_id - 1]->states[state_index];
}

void QuestContainer::RegisterAction(int action_id, String name)
{
    QuestType entry(action_id, name);
    action_names.insert(action_names.end(), entry);
}

void QuestContainer::RegisterCondition(int condition_id, String name)
{
    QuestType entry(condition_id, name);
    cond_names.insert(cond_names.end(), entry);
}

// The action id of a name ( AddNpcText, GiveItem, .. ), or 0.
int QuestContainer::GetActionType(String name)
{
    if (name.Length() < 3)
        return 0;
    for (vector<QuestType>::iterator it = action_names.begin(); it != action_names.end(); it++)
    {
        if (it->name == name)
            return it->value;
    }
    return 0;
}

// The condition id of a name ( TalkedToNpc, GotItems, .. ), or 0.
int QuestContainer::GetConditionType(String name)
{
    if (name.Length() < 3)
        return 0;
    for (vector<QuestType>::iterator it = cond_names.begin(); it != cond_names.end(); it++)
    {
        if (it->name == name)
            return it->value;
    }
    return 0;
}

// The token as a number, or 0 when it is not all digits.
int QuestContainer::ParseInt(String token)
{
    for (int i = 1; i <= token.Length(); i++)
    {
        if ((unsigned char)token[i] < 48 || (unsigned char)token[i] > 57)
            return 0;
    }
    return StrToInt(token);
}

// Writes a number the protocol's way, base 253 and padded with 254; a number too big for width gets more bytes.
DEFINE_ENCODE_NUMBER(QuestContainer)
