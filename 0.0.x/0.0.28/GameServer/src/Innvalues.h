#ifndef InnvaluesH
#define InnvaluesH

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

struct InnValue;

#include "Innvalue.h"

struct InnValues;

// The inns, read from pub/din001.eid: where their citizens spawn and sleep, and their three questions.
struct InnValues
{
    char loaded;
    vector<InnValue> record_list;
    int record_index;

    InnValues();
    ~InnValues();
    void LoadInns();
    String GetName(unsigned int index);
    int GetSleepMap(unsigned int index);
    int GetSleepX(int index);
    int GetSleepY(int index);
    int GetSpawnMap(unsigned int index, int level);
    int GetSpawnX(int index, int level);
    int GetSpawnY(int index, int level);
    String GetQuestion(unsigned int index);
    String GetAnswer(unsigned int index, int number);
    void Clear();
    unsigned int GetCount();
    int DecodeNumber(String value);
};


#endif
