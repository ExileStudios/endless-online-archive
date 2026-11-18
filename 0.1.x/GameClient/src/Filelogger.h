#ifndef FileloggerH
#define FileloggerH

#define LOG_CHANNEL_GLOBAL 2

// The chat log file.
struct FFileLog
{
    FILE *file;
    char is_open;
    String file_path;
    String unused_text;
    String second_unused_text;
    String third_unused_text;
    String fourth_unused_text;

    FFileLog(char open_now, String log_path);
    ~FFileLog();
    void Open(String path);
    void Close();
    void WriteLine(int channel, String text);
};

#endif
