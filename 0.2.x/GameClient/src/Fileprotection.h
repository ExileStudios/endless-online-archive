#ifndef FileprotectionH
#define FileprotectionH

enum DataFile
{
    DataFile_Credits = 1,
    DataFile_CreditsCheck = 2,
    DataFile_WordFilter = 3,
    DataFile_SongNames = 4,
    DataFile_EnglishDialogTexts = 5,
    DataFile_EnglishUiTexts = 6,
    DataFile_DutchDialogTexts = 7,
    DataFile_DutchUiTexts = 8,
    DataFile_SwedishDialogTexts = 9,
    DataFile_SwedishUiTexts = 10,
    DataFile_PortugueseDialogTexts = 11,
    DataFile_PortugueseUiTexts = 12,
    DataFile_Captcha = 13
};

#define GFX_LIBRARY_COUNT 25

#include <Classes.hpp>
#include <deque.h>
#include <queue.h>
#include <stack.h>
#include <string>

// The start-up file check: the files the client needs, each with an expected size, and the error text of a failure.
struct FileController
{
    String error_message;
    TStringList *file_names;
    TStringList *file_sizes;

    FileController();
    ~FileController();
    void BuildFileList();
    bool VerifyFiles();
    bool PrefixAppDirectory();
    String GetGfxPath(int file_number);
    String GetSoundPath(int file_number);
    String GetMusicPath(int file_number);
    String GetDataFilePath(int file_number);
    String RestorePathSeparators(String path);
    String UnscrambleString(int divisor, char *data_begin, char *data_end);
};


// The path of a graphics file by number, padded to three digits; the path text is kept scrambled.
// Shared body: a class defines its GetGfxPath with DEFINE_GET_GFX_PATH(TheClass); each class has its own
// RestorePathSeparators, UnscrambleString.
#define DEFINE_GET_GFX_PATH(Class) \
String Class::GetGfxPath(int file_number) \
{ \
    string scrambled_name = ".f!ggef.xx!fg"; \
    String file_name = RestorePathSeparators(UnscrambleString(4, scrambled_name.begin(), scrambled_name.end())); \
    file_name.Insert(IntToStr(file_number), 10); \
    if (file_number <= 9) \
        file_name.Insert("0", 10); \
    if (file_number <= 99) \
        file_name.Insert("0", 10); \
    return file_name; \
}

// The path of a music file by number, padded to three digits; the path text is kept scrambled.
// Shared body: a class defines its GetMusicPath with DEFINE_GET_MUSIC_PATH(TheClass); each class has its own
// RestorePathSeparators, UnscrambleString.
#define DEFINE_GET_MUSIC_PATH(Class) \
String Class::GetMusicPath(int file_number) \
{ \
    string scrambled_name = ".d!immf.xx!fm"; \
    String file_name = RestorePathSeparators(UnscrambleString(4, scrambled_name.begin(), scrambled_name.end())); \
    file_name.Insert(IntToStr(file_number), 10); \
    if (file_number <= 9) \
        file_name.Insert("0", 10); \
    if (file_number <= 99) \
        file_name.Insert("0", 10); \
    return file_name; \
}

// The path of a data file by number, padded to three digits; the path text is kept scrambled.
// Shared body: a class defines its GetDataFilePath with DEFINE_GET_DATA_FILE_PATH(TheClass); each class has its own
// RestorePathSeparators, UnscrambleString.
#define DEFINE_GET_DATA_FILE_PATH(Class) \
String Class::GetDataFilePath(int file_number) \
{ \
    string scrambled_name = ".f!ddea.ttaa!d"; \
    String file_name = RestorePathSeparators(UnscrambleString(4, scrambled_name.begin(), scrambled_name.end())); \
    file_name.Insert(IntToStr(file_number), 11); \
    if (file_number <= 9) \
        file_name.Insert("0", 11); \
    if (file_number <= 99) \
        file_name.Insert("0", 11); \
    return file_name; \
}

// Turns each marker sign of an unscrambled path into a backslash.
// Shared body: a class defines its RestorePathSeparators with DEFINE_RESTORE_PATH_SEPARATORS(TheClass).
#define DEFINE_RESTORE_PATH_SEPARATORS(Class) \
String Class::RestorePathSeparators(String path) \
{ \
    int position = path.Length(); \
    for (; position > 0; position--) \
    { \
        if (path[position] == '!') \
        { \
            path.Delete(position, 1); \
            path.Insert("\\", position); \
        } \
    } \
    return path; \
}

// Unscrambles a path text in three passes.
// Characters at odd places go into a queue, the others onto a stack that is then emptied onto the queue.
// Characters whose code divides by divisor are held back and put out in reverse order before the next other one.
// Shared body: a class defines its UnscrambleString with DEFINE_UNSCRAMBLE_STRING(TheClass).
#define DEFINE_UNSCRAMBLE_STRING(Class) \
String Class::UnscrambleString(int divisor, char *data_begin, char *data_end) \
{ \
    String result = ""; \
    int divisor_copy = divisor; \
    try \
    { \
        stack<char> char_stack; \
        queue<char> woven; \
        char to_first_list = true; \
        for (; data_begin != data_end; data_begin++) \
        { \
            if (to_first_list) \
            { \
                woven.push(*data_begin); \
                to_first_list = false; \
            } \
            else \
            { \
                char_stack.push(*data_begin); \
                to_first_list = true; \
            } \
        } \
        while (!char_stack.empty()) \
        { \
            woven.push(char_stack.top()); \
            char_stack.pop(); \
        } \
        char_stack.empty(); \
        while (!woven.empty()) \
        { \
            char current_char = woven.front(); \
            woven.pop(); \
            unsigned char char_byte = current_char; \
            int char_code = char_byte; \
            if (char_code % divisor_copy == 0) \
                char_stack.push(current_char); \
            else \
            { \
                while (!char_stack.empty()) \
                { \
                    result.Insert((char)char_stack.top(), result.Length() + 1); \
                    char_stack.pop(); \
                } \
                result.Insert(current_char, result.Length() + 1); \
            } \
        } \
        while (!char_stack.empty()) \
        { \
            result.Insert((char)char_stack.top(), result.Length() + 1); \
            char_stack.pop(); \
        } \
    } \
    catch (...) \
    { \
        result = ""; \
    } \
    return result; \
}

#endif
