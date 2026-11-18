#include <vcl.h>
#pragma hdrstop

#include "Ctrl_credits.h"
#include "Tmainform.h"

#pragma package(smart_init)

// Fills the credits with the built-in list of names and their styles, ready to scroll up from the bottom.
CreditsController::CreditsController()
{
    data_file_size = 0;
    vowel_count = 0;
    shown_styles = new TStringList();
    shown_y = new TStringList();
    shown_lines = new TStringList();
    credit_texts = new TStringList();
    credit_styles = new TStringList();
    LoadCredits();
    if (!VerifyCredits())
    {
        credit_texts->Clear();
        credit_styles->Clear();
        credit_texts->Add("Endless Online");
        credit_styles->Add("h");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("h;^<"));
        credit_styles->Add("t");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("[rY3XI"));
        credit_styles->Add("n");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("b-^(Y-l"));
        credit_styles->Add("t");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("i6W-0"));
        credit_styles->Add("n");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("l-X4lL"));
        credit_styles->Add("n");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("j6Z*`"));
        credit_styles->Add("t");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("-(-"));
        credit_styles->Add("n");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("Z0l7."));
        credit_styles->Add("n");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("Y-^/]*Z"));
        credit_styles->Add("t");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("h6f1h^"));
        credit_styles->Add("n");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("h1d>%"));
        credit_styles->Add("n");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("T%S6$"));
        credit_styles->Add("n");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("[0[:y"));
        credit_styles->Add("n");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("[0U:{"));
        credit_styles->Add("n");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("^+ ,b1l7Y"));
        credit_styles->Add("t");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("k0k/^7z"));
        credit_styles->Add("n");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("]0e, nC0_ [*^&"));
        credit_styles->Add("n");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("^+ ;_>"));
        credit_styles->Add("t");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("X0T g0 3a>"));
        credit_styles->Add("n");
        credit_texts->Add(MainForm->net_client->DeobfuscateString("oqo"));
        credit_styles->Add("n");
    }
    is_running = false;
    top_y = 50;
    start_y = 420;
    tick_counter = 0;
    next_line_index = 0;
}

CreditsController::~CreditsController()
{
    shown_styles->Clear();
    shown_y->Clear();
    shown_lines->Clear();
    credit_texts->Clear();
    credit_styles->Clear();
    delete shown_styles;
    delete shown_y;
    delete shown_lines;
    delete credit_texts;
    delete credit_styles;
}

// Reads data\dat001.edf: the first line is the heading, a line after an empty line is a title, any other a name.
// It also counts the letters a, e and o of the file for VerifyCredits.
void CreditsController::LoadCredits()
{
    int file_size;
    char *buffer;
    int h_file;
    String file_text;
    TStringList *lines = new TStringList();
    try
    {
        h_file = FileOpen(MainForm->strings->GetDataFilePath(DataFile_Credits), fmOpenRead);
        file_size = FileSeek(h_file, 0, 2);
        FileSeek(h_file, 0, 0);
        buffer = new char[file_size + 1];
        FileRead(h_file, buffer, file_size);
        FileClose(h_file);
        data_file_size = file_size;
        file_text = buffer;
        file_text.SetLength(file_size);
        delete[] buffer;
    }
    catch (...)
    {
        FileClose(h_file);
    }
    try
    {
        String line = "";
        int line_start = 1;
        do
        {
            line = "";
            for (int i = line_start; i <= file_text.Length(); i++)
            {
                if (file_text[i] == 'a' || file_text[i] == 'A' || file_text[i] == 'e' || file_text[i] == 'E' ||
                    file_text[i] == 'o' || file_text[i] == 'O')
                    vowel_count++;
                if (file_text[i] == '\n' || i == file_text.Length())
                {
                    if (i == file_text.Length() && (unsigned char)file_text[i] != '\n' &&
                        (unsigned char)file_text[i] != '\r')
                        line = line + file_text[i];
                    lines->Add(line);
                    line_start = i + 1;
                    break;
                }
                if ((unsigned char)file_text[i] != '\n' && (unsigned char)file_text[i] != '\r')
                    line = line + file_text[i];
            }
        } while (file_text.Length() >= line_start);
    }
    catch (...)
    {
    }
    try
    {
        credit_texts->Add(lines->Strings[0]);
        credit_styles->Add("h");
        char next_is_title = false;
        for (int j = 1; j < lines->Count; j++)
        {
            if (lines->Strings[j] == "")
                next_is_title = true;
            else
            {
                credit_texts->Add(lines->Strings[j]);
                if (next_is_title)
                    credit_styles->Add("t");
                else
                    credit_styles->Add("n");
                next_is_title = false;
            }
        }
    }
    catch (...)
    {
    }
}

// Checks data\dat001.edf against data\dat002.edf, which holds "DAT001.ID{" and then four fields split by ":".
// field 1 = (file size x 3013) - 11, read back with integer division
// field 2 = not read
// field 3 = the count of the letters a, e and o
// field 4 = file size x 21
bool CreditsController::VerifyCredits()
{
    int position;
    char *buffer;
    bool valid;
    int h_file;
    valid = true;
    String file_text;
    try
    {
        h_file = FileOpen(MainForm->strings->GetDataFilePath(DataFile_CreditsCheck), fmOpenRead);
        position = FileSeek(h_file, 0, 2);
        FileSeek(h_file, 0, 0);
        buffer = new char[position + 1];
        FileRead(h_file, buffer, position);
        FileClose(h_file);
        file_text = buffer;
        file_text.SetLength(position);
        delete[] buffer;
    }
    catch (...)
    {
        FileClose(h_file);
    }
    if (file_text.Length() <= 14)
        valid = false;
    if (valid)
    {
        if (UpperCase(file_text.SubString(1, 10)) != "DAT001.ID{")
            valid = false;
    }
    String size_code = "";
    String vowel_code = "";
    String size_check_code = "";
    if (valid)
    {
        int colon_count = 0;
        position = 11;
        while (position <= file_text.Length())
        {
            if (file_text[position] == '}' || file_text[position] == '\n')
                break;
            if (file_text[position] == ':')
                colon_count++;
            else
            {
                if (!colon_count)
                    size_code = size_code + (char)file_text[position];
                if (colon_count == 2)
                    vowel_code = vowel_code + (char)file_text[position];
                if (colon_count == 3)
                    size_check_code = size_check_code + (char)file_text[position];
            }
            position++;
        }
        if (colon_count != 3)
            valid = false;
    }
    if (valid)
    {
        try
        {
            if ((StrToInt(size_code) + 11) / 2003 != data_file_size)
                valid = false;
            if (StrToInt(vowel_code) != vowel_count)
                valid = false;
            if (StrToInt(size_check_code) / 21 != data_file_size)
                valid = false;
        }
        catch (...)
        {
            valid = false;
        }
    }
    return valid;
}

// Stops the credits and clears the lines on screen.
void CreditsController::Stop()
{
    is_running = false;
    next_line_index = 0;
    tick_counter = 0;
    shown_styles->Clear();
    shown_y->Clear();
    shown_lines->Clear();
}

void CreditsController::Start()
{
    is_running = true;
    next_line_index = 0;
    tick_counter = 0;
}
