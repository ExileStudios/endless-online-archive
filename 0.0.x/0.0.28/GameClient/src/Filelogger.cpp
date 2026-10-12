#include <vcl.h>
#pragma hdrstop

#include "Filelogger.h"
#include <stdio.h>
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

// Opens the log at once when open_now, the LogChat setting.
FFileLog::FFileLog(char open_now, String log_path)
{
    is_open = 0;
    file_path = log_path;
    if (open_now)
        Open(file_path);
}

FFileLog::~FFileLog()
{
    if (is_open)
        fclose(file);
}

// Opens the log file to add lines at its end.
void FFileLog::Open(String path)
{
    try
    {
        file = fopen(file_path.c_str(), "a");
        is_open = 1;
    }
    catch (...)
    {
        is_open = 0;
    }
}

void FFileLog::Close()
{
    if (is_open)
    {
        fclose(file);
        is_open = 0;
    }
}

// Adds a line with the date, the time and a letter for the channel:
// 0 = P (local), 1 = W (private), 2 = G (global, group, and local lines of icon 4), 3 = S (system)
void FFileLog::WriteLine(int channel, String text)
{
    if (is_open)
    {
        try
        {
            String line = DateToStr(Date());
            line.Insert(" ", line.Length() + 1);
            line.Insert(TimeToStr(Time()), line.Length() + 1);
            line.Insert(" [", line.Length() + 1);
            if (!channel)
                line.Insert("P", line.Length() + 1);
            if (channel == 1)
                line.Insert("W", line.Length() + 1);
            if (channel == 2)
                line.Insert("G", line.Length() + 1);
            if (channel == 3)
                line.Insert("S", line.Length() + 1);
            line.Insert("] ", line.Length() + 1);
            line.Insert(text, line.Length() + 1);
            fprintf(file, "%s\n", line);
        }
        catch (...)
        {
        }
    }
}
