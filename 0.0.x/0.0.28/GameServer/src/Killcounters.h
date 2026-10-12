#ifndef KillcountersH
#define KillcountersH

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <ScktComp.hpp>
#include <DBTables.hpp>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <deque.h>
#include <list.h>
#include <stack.h>
#include <queue.h>

struct KillCounter;

#include "Killcounter.h"

struct KillCounters;

// The kills of each player, in 27 lists by the first letter ( a to z, then the rest ), kept in cache/kills.chk.
struct KillCounters
{
    TStringList *string_list;
    String current_line;
    vector<KillCounter> buckets[27];

    KillCounters();
    ~KillCounters();
    void Clear();
    int IncrementAndGet(String name);
    void Add(String name, int count);
    int Get(String name);
    void Init();
    String Extract();
    void Save();
};

#endif
