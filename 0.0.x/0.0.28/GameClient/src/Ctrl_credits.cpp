#include <vcl.h>
#pragma hdrstop

#include "Ctrl_credits.h"
#include "Tmainform.h"
#pragma package(smart_init)

// Loads the credits; when the file fails its check, a short built-in list is shown instead.
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
        credit_styles->Add("head");
        credit_texts->Add("Programming");
        credit_styles->Add("title");
        credit_texts->Add("Vult-r");
        credit_styles->Add("name");
        credit_texts->Add("Artwork");
        credit_styles->Add("title");
        credit_texts->Add("Teror");
        credit_styles->Add("name");
        credit_texts->Add("Tester");
        credit_styles->Add("title");
        credit_texts->Add("Aengie");
        credit_styles->Add("name");
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
    String file_text;
    int hFile;
    int file_size;
    char *buffer;
    TStringList *lines = new Classes::TStringList();
    try
    {
        hFile = FileOpen(MainForm->strings->GetDataFilePath(1), 0);
        file_size = FileSeek(hFile, 0, 2);
        FileSeek(hFile, 0, 0);
        buffer = new char[file_size + 1];
        FileRead(hFile, buffer, file_size);
        FileClose(hFile);
        data_file_size = file_size;
        file_text = buffer;
        file_text.SetLength(file_size);
        delete[] buffer;
    }
    catch (...)
    {
        FileClose(hFile);
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
                    if (i == file_text.Length())
                    {
                        if ((unsigned char)file_text[i] != 10 && (unsigned char)file_text[i] != 13)
                            line = line + file_text[i];
                    }
                    lines->Add(line);
                    line_start = i + 1;
                    break;
                }
                if ((unsigned char)file_text[i] != 10 && (unsigned char)file_text[i] != 13)
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
        credit_styles->Add("head");
        char next_is_title = 0;
        for (int j = 1; j < lines->Count; j++)
        {
            if (lines->Strings[j] == "")
                next_is_title = 1;
            else
            {
                credit_texts->Add(lines->Strings[j]);
                if (next_is_title)
                    credit_styles->Add("title");
                else
                    credit_styles->Add("name");
                next_is_title = 0;
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
    bool valid;
    int hFile;
    DWORD file_size;
    char *buffer;
    int colon_count;
    int i;
    valid = 1;
    String file_text;
    try
    {
        hFile = FileOpen(MainForm->strings->GetDataFilePath(2), 0);
        file_size = FileSeek(hFile, 0, 2);
        FileSeek(hFile, 0, 0);
        buffer = new char[file_size + 1];
        FileRead(hFile, buffer, file_size);
        FileClose(hFile);
        file_text = buffer;
        file_text.SetLength(file_size);
        delete[] buffer;
    }
    catch (...)
    {
        FileClose(hFile);
    }
    if (file_text.Length() <= 14)
        valid = 0;
    if (valid)
    {
        if (UpperCase(file_text.SubString(1, 10)) != "DAT001.ID{")
            valid = 0;
    }
    String size_code = "";
    String vowel_code = "";
    String size_check_code = "";
    if (valid)
    {
        colon_count = 0;
        for (i = 11; i <= file_text.Length(); i++)
        {
            if (file_text[i] == '}')
                break;
            if (file_text[i] == '\n')
                break;
            if (file_text[i] == ':')
                colon_count++;
            else
            {
                if (!colon_count)
                    size_code = size_code + file_text[i];
                if (colon_count == 2)
                    vowel_code = vowel_code + file_text[i];
                if (colon_count == 3)
                    size_check_code = size_check_code + file_text[i];
            }
        }
        if (colon_count != 3)
            valid = 0;
    }
    if (valid)
    {
        try
        {
            if ((StrToInt(size_code) + 11) / 3013 != data_file_size)
                valid = 0;
            if (StrToInt(vowel_code) != vowel_count)
                valid = 0;
            if (StrToInt(size_check_code) / 21 != data_file_size)
                valid = 0;
        }
        catch (...)
        {
            valid = 0;
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
