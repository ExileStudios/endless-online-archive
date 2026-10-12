#include <vcl.h>
#pragma hdrstop

#include "Newscontrol.h"

#pragma package(smart_init)

NewsTopics::NewsTopics()
{
    ini_file = new TStringList();
    LoadNews();
}

NewsTopics::~NewsTopics()
{
}

void NewsTopics::LoadNews()
{
    LoadFile("./config/news.ini");
}

// The news line at index, or "" past the last one.
String NewsTopics::Get(int index)
{
    String result = "";
    if (ini_file->Count > index)
        result = ini_file->Strings[index];
    return result;
}

// Loads a file from the folder the exe is in, without its empty lines and its `#` comment lines.
void NewsTopics::LoadFile(String file_name)
{
    ini_file->Clear();
    try
    {
        int last_slash_pos;
        String exe = Application->ExeName;
        last_slash_pos = 0;
        if (exe.Length() >= 1)
        {
            int i = exe.Length();
            while (i >= 1)
            {
                if (exe[i] == '\\')
                {
                    last_slash_pos = i;
                    break;
                }
                i--;
            }
        }
        String dir = "";
        if (exe.Length() >= 1 && last_slash_pos >= 1)
        {
            for (int char_index = 1; char_index < last_slash_pos; char_index++)
                dir = dir + exe[char_index];
        }
        exe = dir;
        String tail;
        if (exe[exe.Length()] != '\\')
            tail = "\\" + file_name;
        else
            tail = file_name;
        ini_file->LoadFromFile(exe + tail);
        for (int i = ini_file->Count - 1; i >= 0; i--)
        {
            if (ini_file->Strings[i].Length() > 1)
            {
                if (ini_file->Strings[i][1] == 35)  // '#'
                    ini_file->Delete(i);
            }
            else
                ini_file->Delete(i);
        }
    }
    catch (...)
    {
    }
}
