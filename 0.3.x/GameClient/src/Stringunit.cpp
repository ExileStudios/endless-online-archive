#include <vcl.h>
#pragma hdrstop

#include "Stringunit.h"
#include "Fileprotection.h"

#pragma package(smart_init)

StringContainer::StringContainer(int initial_language)
{
    language = initial_language;
    dialog_texts_loaded = false;
    ui_texts_loaded = false;
    song_names_loaded = false;
}

StringContainer::~StringContainer()
{
    if (dialog_texts_loaded)
    {
        dialog_texts->Clear();
        delete dialog_texts;
    }
    if (ui_texts_loaded)
    {
        ui_texts->Clear();
        delete ui_texts;
    }
    if (song_names_loaded)
    {
        song_names->Clear();
        delete song_names;
    }
}

// Switches the language and loads again the text files that are already loaded.
void StringContainer::SetLanguage(int new_language)
{
    language = new_language;
    if (dialog_texts_loaded)
    {
        dialog_texts->Clear();
        dialog_texts->LoadFromFile(GetDataFilePath(2 * new_language + DataFile_EnglishDialogTexts));
        for (int i = 0; i < dialog_texts->Count; i++)
        {
            string line_bytes = dialog_texts->Strings[i].c_str();
            dialog_texts->Strings[i] = UnscrambleString(7, line_bytes.begin(), line_bytes.end());
        }
    }
    if (ui_texts_loaded)
    {
        ui_texts->Clear();
        ui_texts->LoadFromFile(GetDataFilePath(2 * new_language + DataFile_EnglishUiTexts));
        for (int j = 0; j < ui_texts->Count; j++)
        {
            string second_line_bytes = ui_texts->Strings[j].c_str();
            ui_texts->Strings[j] = UnscrambleString(7, second_line_bytes.begin(), second_line_bytes.end());
        }
    }
}

// A line of the language's dialog file: each dialog has two lines, its title and then its body.
String StringContainer::GetDialogText(int dialog_id, bool want_body)
{
    int line_index;
    if (!dialog_texts_loaded)
    {
        dialog_texts_loaded = true;
        TStringList *new_list = new TStringList();
        dialog_texts = new_list;
        new_list->LoadFromFile(GetDataFilePath(2 * language + DataFile_EnglishDialogTexts));
        for (int i = 0; i < dialog_texts->Count; i++)
        {
            string line_bytes = dialog_texts->Strings[i].c_str();
            dialog_texts->Strings[i] = UnscrambleString(7, line_bytes.begin(), line_bytes.end());
        }
    }
    line_index = 2 * dialog_id - 2;
    if (want_body)
        line_index++;
    return dialog_texts->Strings[line_index];
}

// A line of the language's text file, counted from 1.
String StringContainer::GetText(int text_id)
{
    if (!ui_texts_loaded)
    {
        ui_texts_loaded = true;
        TStringList *new_list = new TStringList();
        ui_texts = new_list;
        new_list->LoadFromFile(GetDataFilePath(2 * language + DataFile_EnglishUiTexts));
        for (int i = 0; i < ui_texts->Count; i++)
        {
            string line_bytes = ui_texts->Strings[i].c_str();
            ui_texts->Strings[i] = UnscrambleString(7, line_bytes.begin(), line_bytes.end());
        }
    }
    return ui_texts->Strings[text_id - 1];
}

// A song's name from the song file, counted from 1.
String StringContainer::GetSongName(int song_id)
{
    if (!song_names_loaded)
    {
        song_names_loaded = true;
        TStringList *new_list = new TStringList();
        song_names = new_list;
        new_list->LoadFromFile(GetDataFilePath(DataFile_SongNames));
        for (int i = 0; i < song_names->Count; i++)
        {
            string line_bytes = song_names->Strings[i].c_str();
            song_names->Strings[i] = UnscrambleString(7, line_bytes.begin(), line_bytes.end());
        }
    }
    return song_names->Strings[song_id - 1];
}

// The path of a graphics file, gfx\gfxNNN.egf, the number padded to three digits.
DEFINE_GET_GFX_PATH(StringContainer)

// The sound file of one note of an instrument, sfx\guiNNN.wav or sfx\harNNN.wav.
String StringContainer::GetInstrumentNotePath(int instrument_id, int note_id)
{
    string scrambled_name = "";
    if (instrument_id == 50)
        scrambled_name = ".v!aswf.xi!ug";
    if (instrument_id == 49)
        scrambled_name = ".v!aswf.xr!ah";
    String file_name = RestorePathSeparators(UnscrambleString(4, scrambled_name.begin(), scrambled_name.end()));
    file_name.Insert(IntToStr(note_id), 10);
    if (note_id <= 9)
        file_name.Insert("0", 10);
    if (note_id <= 99)
        file_name.Insert("0", 10);
    return file_name;
}

// The path of a sound effect, sfx\sfxNNN.wav, the number padded to three digits.
String StringContainer::GetSoundPath(int file_number)
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

// The path of a music file, mfx\mfxNNN.mid, the number padded to three digits.
DEFINE_GET_MUSIC_PATH(StringContainer)

// The path of a jukebox song, jbox\jboxNNN.mid, the number padded to three digits.
String StringContainer::GetJukeboxMusicPath(int file_number)
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

// The path of a data file, data\datNNN.edf, the number padded to three digits.
DEFINE_GET_DATA_FILE_PATH(StringContainer)

// The file names are kept with ! for each path separator; this puts the backslashes back.
DEFINE_RESTORE_PATH_SEPARATORS(StringContainer)

// Undoes the scrambling of the text files and the file names.
// The first, third, fifth bytes and on come first, then the others from last to first.
// Then each run of bytes that the divisor divides is reversed where it stands.
DEFINE_UNSCRAMBLE_STRING(StringContainer)
