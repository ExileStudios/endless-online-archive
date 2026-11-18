#include <vcl.h>
#pragma hdrstop

#include "Stringunit.h"
#include "Fileprotection.h"
#include "Tmainform.h"

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
        dialog_texts->LoadFromFile(MainForm->files->GetDataFilePath(2 * new_language + DataFile_EnglishDialogTexts));
        int i = 0;
        for (; i < dialog_texts->Count; i++)
        {
            string line_bytes = dialog_texts->Strings[i].c_str();
            dialog_texts->Strings[i] = UnscrambleString(7, line_bytes.begin(), line_bytes.end());
        }
    }
    if (ui_texts_loaded)
    {
        ui_texts->Clear();
        ui_texts->LoadFromFile(MainForm->files->GetDataFilePath(2 * new_language + DataFile_EnglishUiTexts));
        for (int j = 0; j < ui_texts->Count; j++)
        {
            string second_line_bytes = ui_texts->Strings[j].c_str();
            ui_texts->Strings[j] = UnscrambleString(7, second_line_bytes.begin(), second_line_bytes.end());
        }
    }
}

// The title or the body of a dialog's text, loading the language's dialog file on first use.
String StringContainer::GetDialogText(int dialog_id, bool want_body)
{
    int line_index;
    if (!dialog_texts_loaded)
    {
        dialog_texts_loaded = true;
        TStringList *new_list = new TStringList();
        dialog_texts = new_list;
        new_list->LoadFromFile(MainForm->files->GetDataFilePath(2 * language + DataFile_EnglishDialogTexts));
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

// The interface text of the id in the chosen language, loading the texts on first use.
String StringContainer::GetText(int text_id)
{
    if (!ui_texts_loaded)
    {
        ui_texts_loaded = true;
        TStringList *new_list = new TStringList();
        ui_texts = new_list;
        new_list->LoadFromFile(MainForm->files->GetDataFilePath(2 * language + DataFile_EnglishUiTexts));
        for (int i = 0; i < ui_texts->Count; i++)
        {
            string line_bytes = ui_texts->Strings[i].c_str();
            ui_texts->Strings[i] = UnscrambleString(7, line_bytes.begin(), line_bytes.end());
        }
    }
    return ui_texts->Strings[text_id - 1];
}

// The name of the jukebox song, loading the names on first use.
String StringContainer::GetSongName(int song_id)
{
    if (!song_names_loaded)
    {
        song_names_loaded = true;
        TStringList *new_list = new TStringList();
        song_names = new_list;
        new_list->LoadFromFile(MainForm->files->GetDataFilePath(DataFile_SongNames));
        for (int i = 0; i < song_names->Count; i++)
        {
            string line_bytes = song_names->Strings[i].c_str();
            song_names->Strings[i] = UnscrambleString(7, line_bytes.begin(), line_bytes.end());
        }
    }
    return song_names->Strings[song_id - 1];
}

// The file names are kept with ! for each path separator; this puts the backslashes back.
String StringContainer::RestorePathSeparators(String path)
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
DEFINE_UNSCRAMBLE_STRING(StringContainer)
