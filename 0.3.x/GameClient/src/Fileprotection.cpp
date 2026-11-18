#include <vcl.h>
#pragma hdrstop

#include "Fileprotection.h"
#include <io.h>
#include <Forms.hpp>
#include <stdlib.h>

#pragma package(smart_init)

FileController::FileController()
{
    file_names = new TStringList();
    file_sizes = new TStringList();
    file_kinds = new TStringList();
    error_message = "";
    error_kind = "";
    BuildFileList();
}

FileController::~FileController()
{
    file_names->Clear();
    file_sizes->Clear();
    file_kinds->Clear();
    delete file_names;
    delete file_sizes;
    delete file_kinds;
}

// Lists the files to check, each with its kind: the program, 13 data files, 25 graphics files, 41 music files and
// three sound files.
void FileController::BuildFileList()
{
    int data_number;
    int gfx_number;
    int music_number;
    file_names->Add("Endless.exe");
    file_kinds->Add("D");
    file_sizes->Add("");
    // Then 13 data files, 25 graphics files and 41 music files, and the first sound of each kind.
    for (data_number = DataFile_Credits; data_number <= DataFile_PortugueseUiTexts; data_number++)
    {
        file_names->Add(GetDataFilePath(data_number));
        file_kinds->Add("D");
        file_sizes->Add("");
    }
    for (gfx_number = GfxLibrary_First; gfx_number <= GfxLibrary_LastChecked; gfx_number++)
    {
        file_names->Add(GetGfxPath(gfx_number));
        file_kinds->Add("G");
        file_sizes->Add("");
    }
    for (music_number = 1; music_number <= 41; music_number++)
    {
        file_names->Add(GetMusicPath(music_number));
        file_kinds->Add("M");
        file_sizes->Add("");
    }
    file_names->Add(GetClientSoundPath(1));
    file_kinds->Add("V3");
    file_sizes->Add("");
    file_names->Add(GetControlSoundPath(1));
    file_kinds->Add("V3");
    file_sizes->Add("");
    file_names->Add(GetInstrumentSoundPath(1));
    file_kinds->Add("V3");
    file_sizes->Add("");
}

// Checks that every listed file is there and, where a size is given, has that size; a missing or unreadable file
// names itself in the error.
bool FileController::VerifyFiles()
{
    int file_handle;
    bool all_ok = true;
    for (int i = 0; i < file_names->Count; i++)
    {
        String lp_file_name = file_names->Strings[i];
        if (!access(lp_file_name.c_str(), 0))
        {
            if (file_sizes->Strings[i] != "")
            {
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
                    error_message = "Corrupt file : " + lp_file_name;
                    error_kind = file_kinds->Strings[i];
                    all_ok = false;
                }
            }
        }
        else
        {
            error_message = "Corrupt file : " + lp_file_name;
            error_kind = file_kinds->Strings[i];
            all_ok = false;
            break;
        }
    }
    return all_ok;
}

// The path of a data file by number, padded to three digits; the path text is kept scrambled.
String FileController::GetDataFilePath(int file_number)
{
    string scrambled_name = ".f!ddea.ttaa!d";
    String file_name = RestorePathSeparators(UnscrambleString(4, scrambled_name.begin(), scrambled_name.end()));
    file_name.Insert(PadFileNumber(file_number), 11);
    return file_name;
}

// The path of a graphics file by number, padded to three digits; the path text is kept scrambled.
String FileController::GetGfxPath(int file_number)
{
    string scrambled_name = ".f!ggef.xx!fg";
    String file_name = RestorePathSeparators(UnscrambleString(4, scrambled_name.begin(), scrambled_name.end()));
    file_name.Insert(PadFileNumber(file_number), 10);
    return file_name;
}

String FileController::GetClientSoundPath(int file_number)
{
    string scrambled_name = ".v!aswf.xx!fcsl!iten";
    String file_name = RestorePathSeparators(UnscrambleString(4, scrambled_name.begin(), scrambled_name.end()));
    file_name.Insert(PadFileNumber(file_number), 17);
    return file_name;
}

// The path of the numbered control sound, one of its four variants at random.
String FileController::GetControlSoundPath(int file_number)
{
    string scrambled_name = ".v!aswf.xx!fcst!rl";
    String file_name = RestorePathSeparators(UnscrambleString(4, scrambled_name.begin(), scrambled_name.end()));
    String variant_name = PadFileNumber(file_number) + "-" + IntToStr(random(4) + 1);
    file_name.Insert(variant_name, 15);
    return file_name;
}

// The path of a sound file by number, padded to three digits; the path text is kept scrambled.
String FileController::GetSoundPath(int file_number)
{
    string scrambled_name = ".v!aswf.xx!fs";
    String file_name = RestorePathSeparators(UnscrambleString(4, scrambled_name.begin(), scrambled_name.end()));
    file_name.Insert(PadFileNumber(file_number), 10);
    return file_name;
}

String FileController::GetEffectSoundPath(int file_number)
{
    string scrambled_name = ".v!aswf.xx!feef!ftec";
    String file_name = RestorePathSeparators(UnscrambleString(4, scrambled_name.begin(), scrambled_name.end()));
    file_name.Insert(PadFileNumber(file_number), 17);
    return file_name;
}

String FileController::GetAmbientSoundPath(int file_number)
{
    string scrambled_name = ".v!aswf.xx!fs";
    String file_name = RestorePathSeparators(UnscrambleString(4, scrambled_name.begin(), scrambled_name.end()));
    file_name.Insert(PadFileNumber(file_number), 10);
    return file_name;
}

// The path of the instrument's sound; instruments 49 and 50 play as 1 and 2.
String FileController::GetInstrumentSoundPath(int instrument)
{
    if (instrument == 49)
        instrument = 1;
    if (instrument == 50)
        instrument = 2;
    string scrambled_name = ".v!aswf.xx!fiin!srt";
    String file_name = RestorePathSeparators(UnscrambleString(4, scrambled_name.begin(), scrambled_name.end()));
    file_name.Insert(PadFileNumber(instrument), 16);
    return file_name;
}

// The path of the numbered jukebox song, the number padded to three digits.
String FileController::GetJukeboxPath(int file_number)
{
    string scrambled_name = ".d!ijmb.oxxo!bj";
    String file_name = RestorePathSeparators(UnscrambleString(4, scrambled_name.begin(), scrambled_name.end()));
    file_name.Insert(IntToStr(file_number), 12);
    if (file_number <= 9)
        file_name.Insert("0", 12);
    if (file_number <= 99)
        file_name.Insert("0", 12);
    return file_name;
}

// The path of a music file by number, padded to three digits; the path text is kept scrambled.
DEFINE_GET_MUSIC_PATH(FileController)

// The number padded with zeros to three digits.
String FileController::PadFileNumber(int file_number)
{
    if (file_number < 10)
        return "00" + IntToStr(file_number);
    if (file_number < 100)
        return "0" + IntToStr(file_number);
    return IntToStr(file_number);
}

// Turns each marker sign of an unscrambled path into a backslash.
DEFINE_RESTORE_PATH_SEPARATORS(FileController)

// Unscrambles a path text in three passes.
// Characters at odd places go into a queue, the others onto a stack that is then emptied onto the queue.
// Characters whose code divides by divisor are held back and put out in reverse order before the next other one.
DEFINE_UNSCRAMBLE_STRING(FileController)
