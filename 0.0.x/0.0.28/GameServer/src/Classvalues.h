#ifndef ClassvaluesH
#define ClassvaluesH

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

struct ClassValue;

#include "Classvalue.h"

struct ClassValues;

// The classes, read from the pub/dat*.ecf files.
struct ClassValues
{
    int file_id;
    int num_records;
    int rid[2];
    char loaded;
    TStringList *string_list;
    void *encode_scratch;
    vector<ClassValue> record_list;
    int record_index;

    ClassValues();
    ~ClassValues();
    void LoadClasses();
    void AddClass(int id, int parent_type, String name, short stat_group, short str, short intl, short wis, short agi,
        short con, short cha);
    char ClassMatches(int class_id, int class_requirement);
    ClassValue GetByIndex(int index);
    int GetCount();
    int DecodeNumber(String value);
};


#endif
