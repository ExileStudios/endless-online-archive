#include <vcl.h>
#pragma hdrstop

#include "Fileprotection.h"
#include <io.h>
#include <Forms.hpp>

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
    file_names->Add(GetDataFilePath(DataFile_Credits));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(DataFile_CreditsCheck));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(DataFile_WordFilter));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(DataFile_SongNames));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(DataFile_EnglishDialogTexts));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(DataFile_EnglishUiTexts));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(DataFile_DutchDialogTexts));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(DataFile_DutchUiTexts));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(DataFile_SwedishDialogTexts));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(DataFile_SwedishUiTexts));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(DataFile_PortugueseDialogTexts));
    file_sizes->Add("");
    file_names->Add(GetDataFilePath(DataFile_PortugueseUiTexts));
    file_sizes->Add("");
    for (int i = 1; i <= GFX_LIBRARY_COUNT; i++)
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

// Checks that every listed file is there and, where a size is given, has that size; a missing or unreadable file
// names itself in the error.
bool FileController::VerifyFiles()
{
    bool all_ok = true;
    for (int i = 0; i < file_names->Count; i++)
    {
        if (!all_ok)
            return false;
        String lp_file_name = file_names->Strings[i];
        // Set before each file, so after a failure it names that file.
        error_message = "Corrupt file : " + lp_file_name;
        if (!access(lp_file_name.c_str(), 0))
        {
            if (file_sizes->Strings[i] != "")
            {
                int file_handle;
                try
                {
                    file_handle = open(lp_file_name.c_str(), 0);
                    int file_length = filelength(file_handle);
                    if (file_length != StrToInt(file_sizes->Strings[i]))
                    {
                        all_ok = false;
                        break;
                    }
                    close(file_handle);
                }
                catch (...)
                {
                    all_ok = false;
                    close(file_handle);
                }
            }
        }
        else
        {
            all_ok = false;
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
    bool prefixed = false;
    String app_dir = Application->ExeName;
    if (app_dir.Length() > 0)
    {
        position = app_dir.Length();
        for (; position > 0; position--)
        {
            if (app_dir[position] == '/' || app_dir[position] == '\\')
            {
                app_dir = app_dir.SubString(1, position);
                for (int i = 0; i < file_names->Count; i++)
                    file_names->Strings[i] = app_dir + file_names->Strings[i];
                prefixed = true;
                break;
            }
        }
    }
    return prefixed;
}

// The path of a graphics file by number, padded to three digits; the path text is kept scrambled.
DEFINE_GET_GFX_PATH(FileController)

// The path of a sound file by number, padded to three digits; the path text is kept scrambled.
String FileController::GetSoundPath(int file_number)
{
    string scrambled_name = ".v!aswf.xx!fs";
    String file_name = RestorePathSeparators(UnscrambleString(4, scrambled_name.begin(), scrambled_name.end()));
    file_name.Insert(IntToStr(file_number), 10);
    if (file_number <= 9)
        file_name.Insert("0", 10);
    if (file_number <= 99)
        file_name.Insert("0", 10);
    return file_name;
}

// The path of a music file by number, padded to three digits; the path text is kept scrambled.
DEFINE_GET_MUSIC_PATH(FileController)

// The path of a data file by number, padded to three digits; the path text is kept scrambled.
DEFINE_GET_DATA_FILE_PATH(FileController)

// Turns each marker sign of an unscrambled path into a backslash.
DEFINE_RESTORE_PATH_SEPARATORS(FileController)

// Unscrambles a path text in three passes.
// Characters at odd places go into a queue, the others onto a stack that is then emptied onto the queue.
// Characters whose code divides by divisor are held back and put out in reverse order before the next other one.
DEFINE_UNSCRAMBLE_STRING(FileController)
