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
    credit_texts->Add(MainForm->net_client->DeobfuscateString("l;_>]>i1l20"));
    credit_styles->Add("n");
    credit_texts->Add(MainForm->net_client->DeobfuscateString("l-X4lL"));
    credit_styles->Add("n");
    credit_texts->Add(MainForm->net_client->DeobfuscateString("j6Z*`"));
    credit_styles->Add("t");
    credit_texts->Add(MainForm->net_client->DeobfuscateString("Z0l7. i(-"));
    credit_styles->Add("n");
    credit_texts->Add(MainForm->net_client->DeobfuscateString("`>h+"));
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
    if (MainForm->themes->is_loaded)
    {
        credit_texts->Add(MainForm->net_client->DeobfuscateString("h2h7Y"));
        credit_styles->Add("t");
        credit_texts->Add(MainForm->themes->theme_author);
        credit_styles->Add("n");
        if (MainForm->themes->co_author_1.Length() > 3)
        {
            credit_texts->Add(MainForm->themes->co_author_1);
            credit_styles->Add("n");
        }
        if (MainForm->themes->co_author_2.Length() > 3)
        {
            credit_texts->Add(MainForm->themes->co_author_2);
            credit_styles->Add("n");
        }
        if (MainForm->themes->co_author_3.Length() > 3)
        {
            credit_texts->Add(MainForm->themes->co_author_3);
            credit_styles->Add("n");
        }
    }
    credit_texts->Add(MainForm->net_client->DeobfuscateString("^+ ,b1l7Y"));
    credit_styles->Add("t");
    credit_texts->Add(MainForm->net_client->DeobfuscateString("k0k/^7z"));
    credit_styles->Add("n");
    credit_texts->Add(MainForm->net_client->DeobfuscateString("]0e, nC0_ [*^&"));
    credit_styles->Add("n");
    credit_texts->Add(MainForm->net_client->DeobfuscateString("T+d1X2`0j h7Y i1l"));
    credit_styles->Add("t");
    credit_texts->Add(MainForm->net_client->DeobfuscateString("X0T g0 3a>"));
    credit_styles->Add("n");
    is_running = false;
    top_y = 50;
    start_y = 420;
    tick_counter = 0;
    next_line_index = 0;
}

void Ctrl_credits_UnusedLiterals1()
{
    "";
    "";
    "";
    "h";
    "n";
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
