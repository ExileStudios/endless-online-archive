#include <vcl.h>
#pragma hdrstop

#include "Questcounters.h"
#include "Questcounter.h"

#pragma package(smart_init)

#include <dirent.h>
#include <stdio.h>

QuestCounters::QuestCounters()
{
    Load();
}

QuestCounters::~QuestCounters()
{
}

// Forgets every count and deletes the cache/quest*.chk files.
void QuestCounters::Clear()
{
    vector<QuestCounter>::iterator it;
    DIR * dir_stream;
    for (it = counters.begin(); it != counters.end(); it++)
        it->Clear();
    counters.clear();
    String dir = "./cache/";
    dir_stream = opendir(dir.c_str());
    struct dirent *entry;
    while ((entry = readdir(dir_stream)) != 0)
    {
        String file = entry->d_name;
        if (file != "." && file != ".." && file.Pos(".") > 0 && file.Length() > 6)
        {
            if (file.SubString(1, 5) == "quest" && file.SubString(file.Pos(".") + 1, 4) == "chk")
            {
                String path = dir + file;
                remove(path.c_str());
            }
        }
    }
}

// Loads a counter for every cache/quest<id>.chk file.
void QuestCounters::Load()
{
    String dir = "./cache/";
    DIR * dir_stream = opendir(dir.c_str());
    struct dirent *entry;
    while ((entry = readdir(dir_stream)) != 0)
    {
        String file = entry->d_name;
        if (file != "." && file != ".." && file.Pos(".") > 0 && file.Length() > 6)
        {
            if (file.SubString(1, 5) == "quest" && file.SubString(file.Pos(".") + 1, 4) == "chk")
            {
                QuestCounter counter(StrToInt(file.SubString(6, file.Pos(".") - 6)));
                counters.insert(counters.end(), counter);
            }
        }
    }
    vector<QuestCounter>::iterator it;
    for (it = counters.begin(); it != counters.end(); it++)
    {
        int quest_id = it->quest_id;
        it->Load();
    }
}

void QuestCounters::Save()
{
    for (vector<QuestCounter>::iterator it = counters.begin(); it != counters.end(); it++)
        it->Save();
}

// Counts one more finish of the quest for the player and returns the new count.
int QuestCounters::RecordCompletion(String name, int quest_id)
{
    name = name.LowerCase();
    int bucket = name[1] - 97;
    if (bucket < 0 || bucket > 26)
        bucket = 26;
    for (vector<QuestCounter>::iterator it = counters.begin(); it != counters.end(); it++)
    {
        if (quest_id == it->quest_id)
            return it->Increment(name);
    }
    QuestCounter counter(quest_id);
    counter.Increment(name);
    counters.insert(counters.end(), counter);
    return 1;
}

// How often the player finished the quest.
int QuestCounters::GetCompletionCount(String player_name, int quest_id)
{
    player_name = player_name.LowerCase();
    int bucket = player_name[1] - 97;
    if (bucket < 0 || bucket > 26)
        bucket = 26;
    for (vector<QuestCounter>::iterator it = counters.begin(); it != counters.end(); it++)
    {
        if (quest_id == it->quest_id)
            return it->Get(player_name);
    }
    return 0;
}
