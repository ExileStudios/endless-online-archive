#include <vcl.h>
#pragma hdrstop

#include "Fileprotection.h"
#include <io.h>
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
#pragma package(smart_init)

FileController::FileController()
{
    file_names = new TStringList();
    file_sizes = new TStringList();
    BuildFileList();
}

FileController::~FileController()
{
    file_names->Clear();
    file_sizes->Clear();
    delete file_names;
    delete file_sizes;
}

// Lists the program, its data files and every graphics, sound and music file the client needs.
void FileController::BuildFileList()
{
    file_names->Add("Endless.exe");
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(1));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(2));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(3));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(4));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(5));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(6));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(7));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(8));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(9));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(10));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(11));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(12));
    file_sizes->Add("");
    for (int i = 1; i <= 25; i++)
    {
        file_names->Add(GetGfxPath(i));
        file_sizes->Add("");
    }
    for (int j = 1; j <= 72; j++)
    {
        file_names->Add(GetSoundPath(j));
        file_sizes->Add("");
    }
    for (int k = 1; k <= 41; k++)
    {
        file_names->Add(GetMusicPath(k));
        file_sizes->Add("");
    }
}

// Checks the listed files in order and returns false at the first one that is missing or has a wrong size.
char FileController::VerifyFiles()
{
    char all_ok;
    int i;
    int FileHandle;
    all_ok = 1;
    for (i = 0; i < file_names->Count; i++)
    {
        String lpFileName = file_names->Strings[i];
        // Set before each file, so after a failure it names that file.
        error_message = "Corrupt file : " + lpFileName;
        if (!access(lpFileName.c_str(), 0))
        {
            if (file_sizes->Strings[i] != "")
            {
                try
                {
                    FileHandle = open(lpFileName.c_str(), 0);
                    int file_length = filelength(FileHandle);
                    if (file_length != StrToInt(file_sizes->Strings[i]))
                    {
                        all_ok = 0;
                        break;
                    }
                    close(FileHandle);
                }
                catch (...)
                {
                    close(FileHandle);
                }
            }
        }
        else
        {
            all_ok = 0;
            // When the program itself is not found by its bare name, every listed name gets the program's folder.
            if (!i)
                all_ok = PrefixAppDirectory();
            break;
        }
    }
    return all_ok;
}

// Puts the program's folder in front of every listed file name; false when the program path has no folder.
bool FileController::PrefixAppDirectory()
{
    int position;
    bool prefixed = 0;
    String app_dir = Application->ExeName;
    if (app_dir.Length() > 0)
    {
        for (position = app_dir.Length(); position > 0; position--)
        {
            if (app_dir[position] == '/' || app_dir[position] == '\\')
            {
                app_dir = app_dir.SubString(1, position);
                for (int i = 0; i < file_names->Count; i++)
                    file_names->Strings[i] = app_dir + file_names->Strings[i];
                prefixed = 1;
                break;
            }
        }
    }
    return prefixed;
}

// The path of a graphics file by number, padded to three digits; the path text is kept scrambled.
String FileController::GetGfxPath(int file_number)
{
    std::string scrambled_name = ".f!ggef.xx!fg";
    String file_name = RestorePathSeparators(this,
        UnscrambleString(this, 4, scrambled_name.begin(), scrambled_name.end()));
    file_name.Insert(IntToStr(file_number), 10);
    if (file_number <= 9)
        file_name.Insert("0", 10);
    if (file_number <= 99)
        file_name.Insert("0", 10);
    return file_name;
}

// The path of a sound file by number, padded to three digits; the path text is kept scrambled.
String FileController::GetSoundPath(int file_number)
{
    std::string scrambled_name = ".v!aswf.xx!fs";
    String file_name = RestorePathSeparators(this,
        UnscrambleString(this, 4, scrambled_name.begin(), scrambled_name.end()));
    file_name.Insert(IntToStr(file_number), 10);
    if (file_number <= 9)
        file_name.Insert("0", 10);
    if (file_number <= 99)
        file_name.Insert("0", 10);
    return file_name;
}

// The path of a music file by number, padded to three digits; the path text is kept scrambled.
String FileController::GetMusicPath(int file_number)
{
    std::string scrambled_name = ".d!immf.xx!fm";
    String file_name = RestorePathSeparators(this,
        UnscrambleString(this, 4, scrambled_name.begin(), scrambled_name.end()));
    file_name.Insert(IntToStr(file_number), 10);
    if (file_number <= 9)
        file_name.Insert("0", 10);
    if (file_number <= 99)
        file_name.Insert("0", 10);
    return file_name;
}

// The path of a data file by number, padded to three digits; the path text is kept scrambled.
String FileController::GetDataFilePath(int file_number)
{
    std::string scrambled_name = ".f!ddea.ttaa!d";
    String file_name = RestorePathSeparators(this,
        UnscrambleString(this, 4, scrambled_name.begin(), scrambled_name.end()));
    file_name.Insert(IntToStr(file_number), 11);
    if (file_number <= 9)
        file_name.Insert("0", 11);
    if (file_number <= 99)
        file_name.Insert("0", 11);
    return file_name;
}

// Turns each marker sign of an unscrambled path into a backslash.
String FileController::RestorePathSeparators(FileController *self, String path)
{
    int position = path.Length();
    for (; position > 0; position--)
    {
        if (path[position] == '!')
        {
            path.Delete(position, 1);
            path.Insert("\\", position);
        }
    }
    return path;
}

// Unscrambles a path text in three passes.
// Characters at odd places go into a queue, the others onto a stack that is then emptied onto the queue.
// Characters whose code divides by divisor are held back and put out in reverse order before the next other one.
String FileController::UnscrambleString(FileController *self, int divisor, unsigned char *data_begin,
    unsigned char *data_end)
{
    int divisor_copy;
    char to_first_list;
    String result = "";
    divisor_copy = divisor;
    try
    {
        std::stack<char > char_stack;
        std::queue<char > char_queue;
        to_first_list = 1;
        while (data_begin != data_end)
        {
            if (to_first_list)
            {
                char_queue.push(*(char *)data_begin);
                to_first_list = 0;
            }
            else
            {
                char_stack.push(*(char *)data_begin);
                to_first_list = 1;
            }
            data_begin++;
        }
        while (!char_stack.empty())
        {
            char_queue.push(char_stack.top());
            char_stack.pop();
        }
        char_stack.empty();
        while (!char_queue.empty())
        {
            char current_char = char_queue.front();
            char_queue.pop();
            unsigned char char_byte = current_char;
            int char_code = char_byte;
            if (!(char_code % divisor_copy))
                char_stack.push(current_char);
            else
            {
                while (!char_stack.empty())
                {
                    result.Insert(char_stack.top(), result.Length() + 1);
                    char_stack.pop();
                }
                result.Insert(current_char, result.Length() + 1);
            }
        }
        while (!char_stack.empty())
        {
            result.Insert(char_stack.top(), result.Length() + 1);
            char_stack.pop();
        }
    }
    catch (...)
    {
        result = "";
    }
    return result;
}
