#ifndef QuesttypeH
#define QuesttypeH

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



struct QuestType;

// A name the quest files use, and its id.
struct QuestType
{
    int value;
    String name;

    QuestType(int value, String name);
    ~QuestType();
};


#endif
