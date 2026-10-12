#include <vcl.h>
#pragma hdrstop

#include "Stringunit.h"
#include <TDx_Input_Library_Install.H>
#include <TDx_Sound_Library_Install.H>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <DBTables.hpp>
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
        dialog_texts->LoadFromFile(GetDataFilePath(2 * new_language + 5));
        for (int i = 0; i < dialog_texts->Count; i++)
        {
            std::string line_bytes = dialog_texts->Strings[i].c_str();
            dialog_texts->Strings[i] = UnscrambleString(this, 7, line_bytes.begin(), line_bytes.end());
        }
    }
    if (ui_texts_loaded)
    {
        ui_texts->Clear();
        ui_texts->LoadFromFile(GetDataFilePath(2 * new_language + 6));
        for (int j = 0; j < ui_texts->Count; j++)
        {
            std::string second_line_bytes = ui_texts->Strings[j].c_str();
            ui_texts->Strings[j] = UnscrambleString(this, 7, second_line_bytes.begin(), second_line_bytes.end());
        }
    }
}

// A line of the language's dialog file: each dialog has two lines, its title and then its body.
String StringContainer::GetDialogText(int dialog_id, bool want_body)
{
    if (!dialog_texts_loaded)
    {
        dialog_texts_loaded = true;
        TStringList *new_list = new Classes::TStringList();
        dialog_texts = new_list;
        new_list->LoadFromFile(GetDataFilePath(2 * language + 5));
        for (int i = 0; i < dialog_texts->Count; i++)
        {
            std::string line_bytes = dialog_texts->Strings[i].c_str();
            dialog_texts->Strings[i] = UnscrambleString(this, 7, line_bytes.begin(), line_bytes.end());
        }
    }
    int line_index = 2 * dialog_id - 2;
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
        TStringList *new_list = new Classes::TStringList();
        ui_texts = new_list;
        new_list->LoadFromFile(GetDataFilePath(2 * language + 6));
        for (int i = 0; i < ui_texts->Count; i++)
        {
            std::string line_bytes = ui_texts->Strings[i].c_str();
            ui_texts->Strings[i] = UnscrambleString(this, 7, line_bytes.begin(), line_bytes.end());
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
        TStringList *new_list = new Classes::TStringList();
        song_names = new_list;
        new_list->LoadFromFile(GetDataFilePath(4));
        for (int i = 0; i < song_names->Count; i++)
        {
            std::string line_bytes = song_names->Strings[i].c_str();
            song_names->Strings[i] = UnscrambleString(this, 7, line_bytes.begin(), line_bytes.end());
        }
    }
    return song_names->Strings[song_id - 1];
}

// The path of a graphics file, gfx\gfxNNN.egf, the number padded to three digits.
String StringContainer::GetGfxPath(int file_number)
{
    std::string scrambled_name = ".f!ggef.xx!fg";
    String file_name = RestorePathSeparators(this,
        UnscrambleString(this, 4, scrambled_name.begin(), scrambled_name.end()));
    file_name.Insert(IntToStr(file_number), 10);
    if (file_number <= SfxSound_TalkTell)
        file_name.Insert("0", 10);
    if (file_number <= 99)
        file_name.Insert("0", 10);
    return file_name;
}

// The sound file of one note of an instrument, sfx\guiNNN.wav or sfx\harNNN.wav.
String StringContainer::GetInstrumentNotePath(int instrument_id, int note_id)
{
    std::string scrambled_name = "";
    if (instrument_id == 50)
        scrambled_name = ".v!aswf.xi!ug";
    if (instrument_id == 49)
        scrambled_name = ".v!aswf.xr!ah";
    String file_name = RestorePathSeparators(this,
        UnscrambleString(this, 4, scrambled_name.begin(), scrambled_name.end()));
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
    std::string scrambled_name = ".v!aswf.xx!fs";
    String file_name = RestorePathSeparators(this,
        UnscrambleString(this, 4, scrambled_name.begin(), scrambled_name.end()));
    file_name.Insert(IntToStr(file_number), 10);
    if (file_number <= SfxSound_TalkTell)
        file_name.Insert("0", 10);
    if (file_number <= 99)
        file_name.Insert("0", 10);
    return file_name;
}

// The path of a music file, mfx\mfxNNN.mid, the number padded to three digits.
String StringContainer::GetMusicPath(int file_number)
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

// The path of a jukebox song, jbox\jboxNNN.mid, the number padded to three digits.
String StringContainer::GetJukeboxMusicPath(int file_number)
{
    std::string scrambled_name = ".d!ijmb.oxxo!bj";
    String file_name = RestorePathSeparators(this,
        UnscrambleString(this, 4, scrambled_name.begin(), scrambled_name.end()));
    file_name.Insert(IntToStr(file_number), 12);
    if (file_number <= 9)
        file_name.Insert("0", 12);
    if (file_number <= 99)
        file_name.Insert("0", 12);
    return file_name;
}

// The path of a data file, data\datNNN.edf, the number padded to three digits.
String StringContainer::GetDataFilePath(int file_number)
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

// The file names are kept with ! for each path separator; this puts the backslashes back.
String StringContainer::RestorePathSeparators(StringContainer *self, String path)
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

// Undoes the scrambling of the text files and the file names.
// The first, third, fifth bytes and on come first, then the others from last to first.
// Then each run of bytes that the divisor divides is reversed where it stands.
String StringContainer::UnscrambleString(StringContainer *self, int divisor, unsigned char *data_begin,
    unsigned char *data_end)
{
    int divisor_copy;
    char to_first_list;
    String result = "";
    divisor_copy = divisor;
    try
    {
        std::stack<char > char_stack;
        std::queue<char > byte_queue;
        to_first_list = 1;
        while (data_begin != data_end)
        {
            if (to_first_list)
            {
                byte_queue.push(*(char *)data_begin);
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
            byte_queue.push(char_stack.top());
            char_stack.pop();
        }
        char_stack.empty();
        while (!byte_queue.empty())
        {
            char current_char = byte_queue.front();
            byte_queue.pop();
            unsigned char current_byte = current_char;
            int char_code = current_byte;
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
