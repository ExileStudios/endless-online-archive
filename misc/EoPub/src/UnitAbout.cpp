#include <vcl.h>
#pragma hdrstop

#include "UnitAbout.h"
#include "UnitMain.h"
#include <stdlib.h>

#pragma package(smart_init)
#pragma resource "*.dfm"

TFormAbout *FormAbout;

__fastcall TFormAbout::TFormAbout(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
    logo_clicks = 0;
    logo_shake_ticks = 1000;
    window_shake_ticks = 1000;
    npc_walk_ticks = 1000;
}

void __fastcall TFormAbout::Panel1Click(TObject *Sender)
{
    ((TCustomForm *)this)->Close();
}

// Themes the form and shows the version; the credits line moves aside and the logo starts to shake.
void __fastcall TFormAbout::FormShow(TObject *Sender)
{
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        Color = (TColor)0x525252;
    }
    Label3->Caption = "Version " + FormMain->version;
    if (FormAbout->Label5->Caption[1] != 'v')
        return;
    Shape2->Left = Label3->Left + Label3->Width + 14;
    Label6->Left = Shape2->Left + Shape2->Width + 14;
    Timer1->Enabled = true;
    logo_left = 80;
    logo_shake_ticks = 1000;
}

// Opens the vult-r web site in the browser.
void __fastcall TFormAbout::Label5Click(TObject *Sender)
{
    try
    {
        ShellExecuteA(Application->Handle, "open", "http://www.vult-r.com", 0, 0, SW_SHOWMAXIMIZED);
    }
    catch (...)
    {
    }
}

// Opens the Endless Online downloads page in the browser.
void __fastcall TFormAbout::Label6Click(TObject *Sender)
{
    try
    {
        ShellExecuteA(Application->Handle, "open", "http://www.endless-online.com/dev/downloads.html", 0, 0,
            SW_SHOWMAXIMIZED);
    }
    catch (...)
    {
    }
}

// Each click on the logo shakes it; the seventh also shakes the window, the ninth sends an npc walking past.
void __fastcall TFormAbout::Image1Click(TObject *Sender)
{
    if (logo_clicks <= 8)
    {
        logo_clicks++;
        logo_shake_ticks = 0;
        if (logo_clicks >= 7)
        {
            window_left = (short)Left;
            window_shake_ticks = 0;
        }
        if (logo_clicks == 9)
        {
            ((TControl *)Image1)->BringToFront();
            npc_library = FormMain->gfx_vault->npcs_library;
            Image2->Picture->Bitmap->LoadFromResourceID((unsigned int)npc_library, 585);
            Image2->Width = 2 * Image2->Picture->Bitmap->Width;
            Image2->Height = 2 * Image2->Picture->Bitmap->Height;
            Image2->Left = 130;
            Image2->Top = 174;
            npc_walk_ticks = 0;
            Image3->Visible = false;
            ((TControl *)Image1)->BringToFront();
        }
    }
}

// Steps the walking npc and the logo and window shakes; stops when the form is hidden.
void __fastcall TFormAbout::Timer1Timer(TObject *Sender)
{
    if (!Visible)
    {
        Timer1->Enabled = false;
        return;
    }
    if (npc_walk_ticks < 100)
    {
        npc_walk_ticks++;
        if (npc_walk_ticks % 24 == 0)
            Image2->Picture->Bitmap->LoadFromResourceID((unsigned int)npc_library, 585);
        if (npc_walk_ticks % 24 == 3)
            Image2->Picture->Bitmap->LoadFromResourceID((unsigned int)npc_library, 586);
        if (npc_walk_ticks % 24 == 6)
            Image2->Picture->Bitmap->LoadFromResourceID((unsigned int)npc_library, 587);
        if (npc_walk_ticks % 24 == 9)
            Image2->Picture->Bitmap->LoadFromResourceID((unsigned int)npc_library, 588);
        if (npc_walk_ticks % 24 == 12)
            Image2->Picture->Bitmap->LoadFromResourceID((unsigned int)npc_library, 589);
        if (npc_walk_ticks % 24 == 15)
            Image2->Picture->Bitmap->LoadFromResourceID((unsigned int)npc_library, 590);
        if (npc_walk_ticks % 24 == 18)
            Image2->Picture->Bitmap->LoadFromResourceID((unsigned int)npc_library, 591);
        if (npc_walk_ticks % 24 == 21)
            Image2->Picture->Bitmap->LoadFromResourceID((unsigned int)npc_library, 592);
        Image2->Left = 130 - 3 * npc_walk_ticks;
        if (npc_walk_ticks == 28)
            Image3->Visible = true;
        if (npc_walk_ticks == 50)
            Image3->Visible = false;
        if (npc_walk_ticks > 28 && npc_walk_ticks < 50)
        {
            Image3->Left = Image2->Left;
            Image3->Top = Image2->Top - 30;
        }
        Paint();
    }
    // The window shakes for a few ticks after the seventh click on the logo.
    if (window_shake_ticks < 5)
    {
        window_shake_ticks++;
        if (window_shake_ticks % 2 == 0)
            Left = window_left - 2;
        if (window_shake_ticks % 2 == 1)
            Left = window_left + 2;
        if (window_shake_ticks >= 4)
            Left = window_left;
    }
    if (logo_shake_ticks < 5)
    {
        logo_shake_ticks++;
        if (logo_left % 2 == 0)
            logo_left = 80 - (short)(random(2) + 1);
        else
            logo_left = (short)random(2) + 81;
        if (logo_shake_ticks >= 5)
            logo_left = 80;
        Image1->Left = logo_left;
        Paint();
    }
}

void __fastcall TFormAbout::FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE || Key == VK_END)
        ((TCustomForm *)this)->Close();
}
