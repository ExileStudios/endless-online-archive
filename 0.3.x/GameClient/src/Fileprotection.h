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

enum GfxLibrary
{
    GfxLibrary_MenuInterface = 1,
    GfxLibrary_First = 1,
    GfxLibrary_GameInterface = 2,
    GfxLibrary_MapTiles = 3,
    GfxLibrary_MapObjects = 4,
    GfxLibrary_MapMasks = 5,
    GfxLibrary_MapWalls = 6,
    GfxLibrary_MapTops = 7,
    GfxLibrary_CharacterBodies = 8,
    GfxLibrary_MaleHair = 9,
    GfxLibrary_FirstPaperdoll = 9,
    GfxLibrary_FemaleHair = 10,
    GfxLibrary_MaleBoots = 11,
    GfxLibrary_FemaleBoots = 12,
    GfxLibrary_MaleArmor = 13,
    GfxLibrary_FemaleArmor = 14,
    GfxLibrary_MaleHats = 15,
    GfxLibrary_FemaleHats = 16,
    GfxLibrary_MaleWeapons = 17,
    GfxLibrary_FemaleWeapons = 18,
    GfxLibrary_MaleShields = 19,
    GfxLibrary_FemaleShields = 20,
    GfxLibrary_LastPaperdoll = 20,
    GfxLibrary_Npcs = 21,
    GfxLibrary_Shadows = 22,
    GfxLibrary_MapItems = 23,
    GfxLibrary_Effects = 24,
    GfxLibrary_MapIcons = 25,
    GfxLibrary_LastChecked = 25,
    GfxLibrary_VisualDocuments = 26
};

#include <Classes.hpp>
#include <deque.h>
#include <queue.h>
#include <stack.h>
#include <string>

// The start-up file check: the files the client needs, each with an expected size, and the error text of a failure.
struct FileController
{
    String error_message;
    String error_kind;
    TStringList *file_names;
    TStringList *file_sizes;
    TStringList *file_kinds;

    FileController();
    ~FileController();
    void BuildFileList();
    bool VerifyFiles();
    String GetGfxPath(int file_number);
    String GetClientSoundPath(int file_number);
    String GetControlSoundPath(int file_number);
    String GetSoundPath(int file_number);
    String GetEffectSoundPath(int file_number);
    String GetAmbientSoundPath(int file_number);
    String GetInstrumentSoundPath(int instrument);
    String GetJukeboxPath(int file_number);
    String GetMusicPath(int file_number);
    String PadFileNumber(int file_number);
    String GetDataFilePath(int file_number);
    String RestorePathSeparators(String path);
    String UnscrambleString(int divisor, char *data_begin, char *data_end);
};


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
