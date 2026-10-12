#include <vcl.h>
#pragma hdrstop

#include "Questcounter.h"
#include "Questcounterlist.h"
#include "Constants.h"

#pragma package(smart_init)

#include <io.h>
#include <stdio.h>

QuestCounter::QuestCounter(int quest_id)
{
    this->quest_id = quest_id;
    this->string_list = new TStringList();
}

QuestCounter::~QuestCounter()
{
}

void QuestCounter::Clear()
{
    counters.clear();
}

int Questcounter_Size(vector<QuestCounterList> &list)
{
    return list.size();
}

// Counts one more for the name and returns the new count.
int QuestCounter::Increment(String name)
{
    name = name.LowerCase();
    vector<QuestCounterList>::iterator it;
    for (it = counters.begin(); it != counters.end(); it++)
    {
        if (name == it->name)
        {
            it->count++;
            return it->count;
        }
    }
    QuestCounterList counter(name);
    counters.insert(counters.end(), counter);
    return 1;
}

int QuestCounter::Get(String name)
{
    name = name.LowerCase();
    vector<QuestCounterList>::iterator it;
    for (it = counters.begin(); it != counters.end(); it++)
    {
        if (name == it->name)
            return it->count;
    }
    return 0;
}

void QuestCounter::Set(String name, int count)
{
    name = name.LowerCase();
    vector<QuestCounterList>::iterator it;
    for (it = counters.begin(); it != counters.end(); it++)
    {
        if (name == it->name)
        {
            it->count = count;
            return;
        }
    }
    QuestCounterList counter(name, count);
    counters.insert(counters.end(), counter);
}

// Reads cache/quest<id>.chk: one `name;count;` line per player.
void QuestCounter::Load()
{
    string_list->Clear();
    try
    {
        string_list->LoadFromFile("./cache/quest" + IntToStr(quest_id) + ".chk");
    }
    catch (...)
    {
    }
    for (int i = 0; i < string_list->Count; i++)
    {
        current_line = string_list->Strings[i];
        String name = Extract();
        Set(name, StrToInt(Extract()));
    }
}

DEFINE_EXTRACT(QuestCounter)

// Writes the counts back to cache/quest<id>.chk.
void QuestCounter::Save()
{
    String path = "./cache/quest" + IntToStr(quest_id) + ".chk";
    if (!access(path.c_str(), 0))
        remove(path.c_str());
    string_list->Clear();
    for (vector<QuestCounterList>::iterator it = counters.begin(); it != counters.end(); it++)
    {
        String line = it->name + ";";
        line = line + IntToStr(it->count) + ";";
        string_list->Add(line);
    }
    string_list->SaveToFile(path);
}
