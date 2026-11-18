#ifndef SerialH
#define SerialH

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



struct SerialKey;

// The Watchdog's licence, read from config/serial.ini.
struct SerialKey
{
    TStringList *ini_lines;
    String key_base;
    String serial_code;
    String unlock_code;
    char valid;
    int counter;

    SerialKey();
    ~SerialKey();
    void SetIniPath(String path);
    String ReadKey(String key, String default_value);
    void CloseIni();
    char IsValid();
    String GetDisplayCode();
    void Validate();
    String DeobfuscateString(String src);
    void SetCounter(int value);
};

// True when the line holds {BEGIN_LIST} ( which = 0 ) or {END_LIST} ( which = 1 ).
// Shared body: a class defines its IsListMarker with DEFINE_IS_LIST_MARKER(TheClass).
#define DEFINE_IS_LIST_MARKER(Class) \
bool Class::IsListMarker(String line, int which) \
{ \
    bool found = false; \
    char in_tag = false; \
    char changed = false; \
    int tag_start = 0; \
    int tag_length = 0; \
    for (int i = 1; i <= line.Length(); i++) \
    { \
        if (!in_tag) \
        { \
            if (line[i] == '{') \
            { \
                in_tag = true; \
                tag_length = 1; \
                tag_start = i; \
            } \
        } \
        else \
        { \
            tag_length++; \
            if (line[i] == '}') \
            { \
                String tag = line.SubString(tag_start, tag_length); \
                if (!which) \
                { \
                    if (tag == "{BEGIN_LIST}") \
                        found = true; \
                } \
                if (which == 1) \
                { \
                    if (tag == "{END_LIST}") \
                        found = true; \
                } \
            } \
        } \
    } \
    return found; \
}

// Takes every '<' and '>' out of a title.
// Shared body: a class defines its StripTags with DEFINE_STRIP_TAGS(TheClass).
#define DEFINE_STRIP_TAGS(Class) \
String Class::StripTags(String text) \
{ \
    int i; \
    String result = ""; \
    for (i = 1; i <= text.Length(); i++) \
    { \
        if (text[i] != '<' && text[i] != '>') \
            result = result + text[i]; \
    } \
    return result; \
}

#endif
