#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include <mem.h>
#include <stdlib.h>
#pragma package(smart_init)

#pragma resource "*.dfm"
TForm1 *Form1;

__fastcall TForm1::TForm1(Classes::TComponent *owner)
    : Forms::TForm(owner)
{
}

// Turns plain chat text into "sodanized" drunk talk: random letters after vowel runs, doubled letters, starred vowels
// and "*hic*" after spaces, then cut to fit one chat line.
String TForm1::Sodanize(TForm1 *self, String text)
{
    if (text.Length() > 0)
    {
        int unused_roll = random(10);
        // First pass: slur the vowels and double some letters.
        for (int i = 1; i <= text.Length(); i++)
        {
            char vowel_end = 0;
            if (text[i] == 'e' || text[i] == 'a' || text[i] == 'E' || text[i] == 'A')
            {
                if (i + 1 <= text.Length() && text[i + 1] != 'e' && text[i + 1] != 'a')
                {
                    if (text[i + 1] != 'E')
                    {
                        if (text[i + 1] != 'A')
                            vowel_end = 1;
                    }
                }
                if (i == text.Length())
                    vowel_end = 1;
                if (vowel_end)
                {
                    int j_roll = random(10);
                    if (j_roll < 7)
                        text.Insert("j", i + 1);
                }
            }
            if (text[i] == 'u' || text[i] == 'o' || text[i] == 'U' || text[i] == 'O')
            {
                if (i + 1 <= text.Length())
                {
                    if (text[i + 1] != 'u')
                    {
                        if (text[i + 1] != 'o')
                        {
                            if (text[i + 1] != 'U')
                            {
                                if (text[i + 1] != 'O')
                                    vowel_end = 1;
                            }
                        }
                    }
                }
                if (i == text.Length())
                    vowel_end = 1;
                if (vowel_end)
                {
                    int w_roll = random(10);
                    if (w_roll < 7)
                        text.Insert("w", i + 1);
                }
            }
            if (text[i] == 'i')
            {
                vowel_end = 1;
                int u_roll = random(10);
                if (u_roll < 4)
                    text.Insert("u", i + 1);
            }
            if (!vowel_end && text[i] != ' ')
            {
                int double_roll = random(10);
                if (double_roll < 4)
                    text.Insert(text[i], i + 1);
            }
        }
        // Second pass: blank out a few vowels with '*'.
        for (int j = 1; j <= text.Length(); j++)
        {
            if (text[j] == 'u' || text[j] == 'o' || text[j] == 'U' || text[j] == 'O' || text[j] == 'e' ||
                text[j] == 'a' || text[j] == 'E' || text[j] == 'A' || text[j] == 'I' || text[j] == 'i')
            {
                int star_roll = random(24);
                if (star_roll < 2)
                    text[j] = 42;
            }
        }
        // Third pass: hiccup after some of the spaces.
        for (int k = 1; k <= text.Length(); k++)
        {
            if (text[k] != ' ')
                continue;
            int hic_roll = random(10);
            if (hic_roll < 3)
            {
                text.Insert("*hic* ", k + 1);
                k += 6;
            }
        }
    }
    // Keep the result short enough for one chat message.
    if (text.Length() >= 128)
    {
        text = text.SubString(1, 126);
        text.Insert("..", text.Length() + 1);
    }
    return text;
}

// Sodanizes the input, shows it, and copies it to the clipboard when the box is ticked.
void __fastcall TForm1::buttonOkClick(System::TObject *Sender)
{
    String sodanized = Sodanize(this, Edit1->Text);
    MemoSodanized->Lines->Clear();
    MemoSodanized->Text = sodanized;
    if (CheckBox1->Checked)
    {
        if (sodanized.Length() > 0)
        {
            CopyToClipboard(sodanized);
            Timer1->Enabled = true;
            Label3->Caption = "'" + sodanized.SubString(1, 22) + "..' copied to clipboard";
            Label3->Visible = true;
        }
    }
}

// Enter converts the text, F12 clears the input.
void __fastcall TForm1::Edit1KeyUp(System::TObject *Sender, Word &Key, Classes::TShiftState Shift)
{
    if (Key == 13)
        buttonOkClick(buttonOk);
    if (Key == 123)
        Edit1->Text = "";
}

// Puts the text on the Windows clipboard as plain text.
void TForm1::CopyToClipboard(String text)
{
    HGLOBAL memory;
    OpenClipboard(GetDesktopWindow());
    EmptyClipboard();
    memory = GlobalAlloc(2, text.Length() + 1);
    if (!memory)
    {
        CloseClipboard();
        return;
    }
    else
    {
        memcpy(GlobalLock(memory), text.c_str(), text.Length() + 1);
        GlobalUnlock(memory);
        SetClipboardData(1, memory);
        CloseClipboard();
        GlobalFree(memory);
        return;
    }
}

void __fastcall TForm1::Timer1Timer(System::TObject *Sender)
{
    Timer1->Enabled = false;
    Label3->Visible = false;
}

void __fastcall TForm1::Panel3Click(System::TObject *Sender)
{
    Edit1->Text = "";
    Edit1->SetFocus();
}

void __fastcall TForm1::MemoSodanizedEnter(System::TObject *Sender)
{
    MemoSodanized->SelStart = 1;
    MemoSodanized->SelectAll();
}
