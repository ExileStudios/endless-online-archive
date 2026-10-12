#include <vcl.h>
#pragma hdrstop

#include "Killcounters.h"
#include "Killcounter.h"
#include "Constants.h"

#pragma package(smart_init)

#include <io.h>
#include <stdio.h>

KillCounters::KillCounters()
{
    string_list = new TStringList();
    Init();
}

KillCounters::~KillCounters()
{
}

// Forgets the counts and deletes cache/kills.chk.
void KillCounters::Clear()
{
    for (int i = 0; i < 26; i++)
        buckets[i].clear();
    String path = "./cache/kills.chk";
    if (!access(path.c_str(), 0))
        remove(path.c_str());
}

int Killcounters_Size(vector<KillCounter> &list)
{
    return list.size();
}

// Counts one more kill for the name and returns the new count.
int KillCounters::IncrementAndGet(String name)
{
    name = name.LowerCase();
    int bucket = name[1] - 97;
    if (bucket < 0 || bucket > 26)
        bucket = 26;
    for (vector<KillCounter>::iterator it = buckets[bucket].begin(); it != buckets[bucket].end(); it++)
    {
        if (name == it->name)
        {
            it->count++;
            return it->count;
        }
    }
    KillCounter killcounter(name);
    buckets[bucket].insert(buckets[bucket].end(), killcounter);
    return 1;
}

void KillCounters::Add(String name, int count)
{
    name = name.LowerCase();
    int bucket = name[1] - 97;
    if (bucket < 0 || bucket > 26)
        bucket = 26;
    KillCounter killcounter(name, count);
    buckets[bucket].insert(buckets[bucket].end(), killcounter);
}

int KillCounters::Get(String name)
{
    name = name.LowerCase();
    int bucket = name[1] - 97;
    if (bucket < 0 || bucket > 26)
        bucket = 26;
    for (vector<KillCounter>::iterator it = buckets[bucket].begin(); it != buckets[bucket].end(); it++)
    {
        if (name == it->name)
            return it->count;
    }
    return 0;
}

// Reads cache/kills.chk: one `name;count;` line per player.
void KillCounters::Init()
{
    string_list->Clear();
    try
    {
        string_list->LoadFromFile("./cache/kills.chk");
    }
    catch (...)
    {
    }
    for (int i = 0; i < string_list->Count; i++)
    {
        current_line = string_list->Strings[i];
        String key = Extract();
        Add(key, StrToInt(Extract()));
    }
}

DEFINE_EXTRACT(KillCounters)

// Writes the counts back to cache/kills.chk.
void KillCounters::Save()
{
    vector<KillCounter>::iterator it;
    int i;
    String path = "./cache/kills.chk";
    if (!access(path.c_str(), 0))
        remove(path.c_str());
    string_list->Clear();
    for (i = 0; i < 26; i++)
    {
        for (it = buckets[i].begin(); it != buckets[i].end(); it++)
        {
            String line = it->name + ";";
            line = line + IntToStr(it->count) + ";";
            string_list->Add(line);
        }
    }
    string_list->SaveToFile("./cache/kills.chk");
}
