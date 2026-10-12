#ifndef FileloggerH
#define FileloggerH

#include <stdio.h>

// The chat log file.
struct FFileLog
{
    FILE *file;
    char is_open;
    String file_path;
    String unused_text_at_12;
    String unused_text_at_16;
    String unused_text_at_20;
    String unused_text_at_24;

    FFileLog(char open_now, String log_path);
    ~FFileLog();
    void Open(String path);
    void Close();
    void WriteLine(int channel, String text);
};

#endif
