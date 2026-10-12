#ifndef NewscontrolH
#define NewscontrolH

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



struct NewsTopics;

// The news lines from config/news.ini.
struct NewsTopics
{
    TStringList *ini_file;

    NewsTopics();
    ~NewsTopics();
    void LoadNews();
    String Get(int index);
    void LoadFile(String file_name);
};


#endif
