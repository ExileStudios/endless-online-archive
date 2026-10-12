#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "Configuration.h"
#include "Unit2.h"

#pragma package(smart_init)

#pragma link "ElXPThemedControl"
#pragma link "ElEdits"
#pragma link "ElBtnEdit"
#pragma link "ElBtnCtl"
#pragma link "ElScrollBox"
#pragma link "ElCheckCtl"
#pragma link "ElSpin"
#pragma link "ElCombos"
#pragma link "ElScrollBar"
#pragma link "ElPgCtl"
#pragma resource "*.dfm"
TForm1 *Form1;

// The words the switches show.
__fastcall TForm1::TForm1(Classes::TComponent *Owner)
    : Forms::TForm(Owner)
{
    enabled_text = "enabled";
    disabled_text = "disabled";
}

void __fastcall TForm1::SpeedButton2Click(System::TObject *Sender)
{
    Close();
}

// Page flipping on or off.
void __fastcall TForm1::ElCheckBox1Click(System::TObject *Sender)
{
    if (ElCheckBox1->Checked)
        Label13->Caption = enabled_text;
    else
        Label13->Caption = disabled_text;
}

// Shadows on or off.
void __fastcall TForm1::ElCheckBox2Click(System::TObject *Sender)
{
    if (ElCheckBox2->Checked)
        Label14->Caption = enabled_text;
    else
        Label14->Caption = disabled_text;
}

// Balloons on or off.
void __fastcall TForm1::ElCheckBox3Click(System::TObject *Sender)
{
    if (ElCheckBox3->Checked)
        Label15->Caption = enabled_text;
    else
        Label15->Caption = disabled_text;
}

// Sound effects on or off.
void __fastcall TForm1::ElCheckBox4Click(System::TObject *Sender)
{
    if (ElCheckBox4->Checked)
        Label8->Caption = enabled_text;
    else
        Label8->Caption = disabled_text;
}

// Music on or off.
void __fastcall TForm1::ElCheckBox5Click(System::TObject *Sender)
{
    if (ElCheckBox5->Checked)
        Label9->Caption = enabled_text;
    else
        Label9->Caption = disabled_text;
}

// Whispers on or off.
void __fastcall TForm1::ElCheckBox6Click(System::TObject *Sender)
{
    if (ElCheckBox6->Checked)
        Label25->Caption = enabled_text;
    else
        Label25->Caption = disabled_text;
}

// Group and trade requests on or off.
void __fastcall TForm1::ElCheckBox7Click(System::TObject *Sender)
{
    if (ElCheckBox7->Checked)
        Label26->Caption = enabled_text;
    else
        Label26->Caption = disabled_text;
}

// The chat log on or off; the log file's name can be changed only while it is on.
void __fastcall TForm1::ElCheckBox8Click(System::TObject *Sender)
{
    if (ElCheckBox8->Checked)
    {
        Label21->Caption = enabled_text;
        ElEdit2->Enabled = true;
        ElEdit2->Font->Color = clBlack;
    }
    else
    {
        Label21->Caption = disabled_text;
        ElEdit2->Enabled = false;
        ElEdit2->Font->Color = clGray;
    }
}

// A window that can be resized, on or off.
void __fastcall TForm1::ElCheckBox9Click(System::TObject *Sender)
{
    if (ElCheckBox9->Checked)
        Label28->Caption = enabled_text;
    else
        Label28->Caption = disabled_text;
}

// The window stays on top, on or off.
void __fastcall TForm1::ElCheckBox10Click(System::TObject *Sender)
{
    if (ElCheckBox10->Checked)
        Label30->Caption = enabled_text;
    else
        Label30->Caption = disabled_text;
}

// Static sprites on or off.
void __fastcall TForm1::ElCheckBox11Click(System::TObject *Sender)
{
    if (ElCheckBox11->Checked)
        Label34->Caption = enabled_text;
    else
        Label34->Caption = disabled_text;
}

// Static objects on or off.
void __fastcall TForm1::ElCheckBox12Click(System::TObject *Sender)
{
    if (ElCheckBox12->Checked)
        Label35->Caption = enabled_text;
    else
        Label35->Caption = disabled_text;
}

// Static tiles on or off.
void __fastcall TForm1::ElCheckBox13Click(System::TObject *Sender)
{
    if (ElCheckBox13->Checked)
        Label36->Caption = enabled_text;
    else
        Label36->Caption = disabled_text;
}

// Loads the settings and shows each one on its page.
void __fastcall TForm1::FormCreate(System::TObject *Sender)
{
    Panel1->Top = 0;
    config = new Config();
    ElEdit1->Text = config->GetHost();
    ElEdit2->Text = config->GetLogFile();
    ElSpinEdit1->Value = config->GetPort();
    if (config->GetFullscreen())
        ElRadioButton1->Checked = true;
    else
        ElRadioButton2->Checked = true;
    if (config->GetPageFlipping())
    {
        ElCheckBox1->Checked = true;
        Label13->Caption = enabled_text;
    }
    else
    {
        ElCheckBox1->Checked = false;
        Label13->Caption = disabled_text;
    }
    if (config->GetShowShadows())
    {
        ElCheckBox2->Checked = true;
        Label14->Caption = enabled_text;
    }
    else
    {
        ElCheckBox2->Checked = false;
        Label14->Caption = disabled_text;
    }
    if (config->GetShowBaloons())
    {
        ElCheckBox3->Checked = true;
        Label15->Caption = enabled_text;
    }
    else
    {
        ElCheckBox3->Checked = false;
        Label15->Caption = disabled_text;
    }
    if (config->GetSound())
    {
        ElCheckBox4->Checked = true;
        Label8->Caption = enabled_text;
    }
    else
    {
        ElCheckBox4->Checked = false;
        Label8->Caption = disabled_text;
    }
    if (config->GetMusic())
    {
        ElCheckBox5->Checked = true;
        Label9->Caption = enabled_text;
    }
    else
    {
        ElCheckBox5->Checked = false;
        Label9->Caption = disabled_text;
    }
    // the filter: off, on, or on for everything
    if (config->GetFilter())
    {
        if (config->GetFilterAll())
            ElRadioButton4->Checked = true;
        else
            ElRadioButton3->Checked = true;
    }
    else
        ElRadioButton5->Checked = true;
    if (config->GetHearWhisper())
    {
        ElCheckBox6->Checked = true;
        Label25->Caption = enabled_text;
    }
    else
    {
        ElCheckBox6->Checked = false;
        Label25->Caption = disabled_text;
    }
    if (config->GetInteraction())
    {
        ElCheckBox7->Checked = true;
        Label26->Caption = enabled_text;
    }
    else
    {
        ElCheckBox7->Checked = false;
        Label26->Caption = disabled_text;
    }
    if (config->GetLogChat())
    {
        ElCheckBox8->Checked = true;
        Label21->Caption = enabled_text;
        ElEdit2->Enabled = true;
        ElEdit2->Font->Color = clBlack;
    }
    else
    {
        ElCheckBox8->Checked = false;
        Label21->Caption = disabled_text;
        ElEdit2->Enabled = false;
        ElEdit2->Font->Color = clGray;
    }
    if (config->GetSizeable())
    {
        ElCheckBox9->Checked = true;
        Label28->Caption = enabled_text;
    }
    else
    {
        ElCheckBox9->Checked = false;
        Label28->Caption = disabled_text;
    }
    if (config->GetStayOnTop())
    {
        ElCheckBox10->Checked = true;
        Label30->Caption = enabled_text;
    }
    else
    {
        ElCheckBox10->Checked = false;
        Label30->Caption = disabled_text;
    }
    if (config->GetStaticSprites())
    {
        ElCheckBox11->Checked = true;
        Label34->Caption = enabled_text;
    }
    else
    {
        ElCheckBox11->Checked = false;
        Label34->Caption = disabled_text;
    }
    if (config->GetStaticObjects())
    {
        ElCheckBox12->Checked = true;
        Label35->Caption = enabled_text;
    }
    else
    {
        ElCheckBox12->Checked = false;
        Label35->Caption = disabled_text;
    }
    if (config->GetStaticTiles())
    {
        ElCheckBox13->Checked = true;
        Label36->Caption = enabled_text;
    }
    else
    {
        ElCheckBox13->Checked = false;
        Label36->Caption = disabled_text;
    }
    if (config->GetBlockTransfer())
    {
        ElCheckBox14->Checked = true;
        Label43->Caption = enabled_text;
    }
    else
    {
        ElCheckBox14->Checked = false;
        Label43->Caption = disabled_text;
    }
    ElComboBox1->ItemIndex = config->GetLanguage();
    ElComboBox2->ItemIndex = config->GetKeyboard();
}

// Opens the game's web site.
void __fastcall TForm1::Label38Click(System::TObject *Sender)
{
    String url = "http://www.endless-online.com";
    try
    {
        ShellExecuteA(Handle, "open", url.c_str(), 0, 0, 10);
    }
    catch (...)
    {
    }
}

// Saves: each setting goes on its line of config/setup.ini, then a box says it was saved.
void __fastcall TForm1::SpeedButton1Click(System::TObject *Sender)
{
    config->SetValue("Host", ElEdit1->Text);
    config->SetValue("Port", IntToStr(ElSpinEdit1->Value));
    config->SetValue("LogFile", ElEdit2->Text);
    if (ElRadioButton1->Checked)
        config->SetValue("Fullscreen", "on");
    if (ElRadioButton2->Checked)
        config->SetValue("Fullscreen", "off");
    if (ElRadioButton5->Checked)
    {
        config->SetValue("Filter", "off");
        config->SetValue("FilterAll", "off");
    }
    if (ElRadioButton4->Checked)
    {
        config->SetValue("Filter", "on");
        config->SetValue("FilterAll", "on");
    }
    if (ElRadioButton3->Checked)
    {
        config->SetValue("Filter", "on");
        config->SetValue("FilterAll", "off");
    }
    if (ElCheckBox1->Checked)
        config->SetValue("Pageflipping", "on");
    else
        config->SetValue("Pageflipping", "off");
    if (ElCheckBox2->Checked)
        config->SetValue("ShowShadows", "on");
    else
        config->SetValue("ShowShadows", "off");
    if (ElCheckBox3->Checked)
        config->SetValue("ShowBaloons", "on");
    else
        config->SetValue("ShowBaloons", "off");
    if (ElCheckBox4->Checked)
        config->SetValue("Sound", "on");
    else
        config->SetValue("Sound", "off");
    if (ElCheckBox5->Checked)
        config->SetValue("Music", "on");
    else
        config->SetValue("Music", "off");
    if (ElCheckBox6->Checked)
        config->SetValue("HearWhisper", "on");
    else
        config->SetValue("HearWhisper", "off");
    if (ElCheckBox7->Checked)
        config->SetValue("Interaction", "on");
    else
        config->SetValue("Interaction", "off");
    if (ElCheckBox8->Checked)
        config->SetValue("LogChat", "on");
    else
        config->SetValue("LogChat", "off");
    if (ElCheckBox9->Checked)
        config->SetValue("Sizeable", "on");
    else
        config->SetValue("Sizeable", "off");
    if (ElCheckBox10->Checked)
        config->SetValue("StayOnTop", "on");
    else
        config->SetValue("StayOnTop", "off");
    if (ElCheckBox11->Checked)
        config->SetValue("StaticSprites", "on");
    else
        config->SetValue("StaticSprites", "off");
    if (ElCheckBox12->Checked)
        config->SetValue("StaticObjects", "on");
    else
        config->SetValue("StaticObjects", "off");
    if (ElCheckBox13->Checked)
        config->SetValue("StaticTiles", "on");
    else
        config->SetValue("StaticTiles", "off");
    if (ElCheckBox14->Checked)
        config->SetValue("BlockTransfer", "on");
    else
        config->SetValue("BlockTransfer", "off");
    config->SetValue("Language", IntToStr(ElComboBox1->ItemIndex));
    config->SetValue("Keyboard", IntToStr(ElComboBox2->ItemIndex));
    config->SaveConfig();
    Form2->Memo1->Text = " The configuration has been saved.";
    Form2->Left = Left + (Width - Form2->Width) / 2;
    Form2->Top = Top + (Height - Form2->Height) / 2;
    Form2->ShowModal();
}

// Block transfer on or off.
void __fastcall TForm1::ElCheckBox14Click(System::TObject *Sender)
{
    if (ElCheckBox14->Checked)
        Label43->Caption = enabled_text;
    else
        Label43->Caption = disabled_text;
}

// The help box for the page that is open.
void __fastcall TForm1::SpeedButton9Click(System::TObject *Sender)
{
    Form2->Memo1->Lines->Clear();
    if (!ElPageControl1->ActivePageIndex)
    {
        Form2->Memo1->Lines->Add(" Connection settings");
        Form2->Memo1->Lines->Add(" - Are used to establish a connection with an Endless server");
        Form2->Memo1->Lines->Add(" ");
        Form2->Memo1->Lines->Add(" [Server host] address which the client should try to connect to.");
        Form2->Memo1->Lines->Add(" [Server port] port on which the server is listening (default 8078)");
    }
    if (ElPageControl1->ActivePageIndex == SettingsPage_Graphics)
    {
        Form2->Memo1->Lines->Add(" Graphics settings");
        Form2->Memo1->Lines->Add(" - Are used to setup game mode and speed.");
        Form2->Memo1->Lines->Add(" ");
        Form2->Memo1->Lines->Add(" [Screen mode] will run the game in fullscreen or a window.");
        Form2->Memo1->Lines->Add(" [Page flipping] only for fullscreen, pageflipping is faster.");
        Form2->Memo1->Lines->Add(" [Show shadows] disable this feature if you need more speed.");
        Form2->Memo1->Lines->Add(" [Show baloons] disable this feature if you need more speed.");
    }
    if (ElPageControl1->ActivePageIndex == SettingsPage_Sound)
    {
        Form2->Memo1->Lines->Add(" Sound settings");
        Form2->Memo1->Lines->Add(" - Are used to setup game sounds");
        Form2->Memo1->Lines->Add(" ");
        Form2->Memo1->Lines->Add(" [Sound] Enables / disabled sound effects.");
        Form2->Memo1->Lines->Add(" [Music] Enables / disabled background music (midi files)");
    }
    if (ElPageControl1->ActivePageIndex == SettingsPage_Language)
    {
        Form2->Memo1->Lines->Add(" Language settings");
        Form2->Memo1->Lines->Add(" :: Use only if your keyboard is not working.");
        Form2->Memo1->Lines->Add(" ");
        Form2->Memo1->Lines->Add(" [Language] The interface language which Endless will use.");
        Form2->Memo1->Lines->Add(" [Keyboard] Here you can change the keyboard mapping.");
    }
    if (ElPageControl1->ActivePageIndex == SettingsPage_Chat)
    {
        Form2->Memo1->Lines->Add(" Chat settings");
        Form2->Memo1->Lines->Add(" - Used to setup your communications");
        Form2->Memo1->Lines->Add(" ");
        Form2->Memo1->Lines->Add(" [Filter] Will filter out received bad words.");
        Form2->Memo1->Lines->Add(" [Hear whisper] Enables / disables private messages.");
        Form2->Memo1->Lines->Add(" [Interaction] Enables / disables group and trade requests");
        Form2->Memo1->Lines->Add(" [Log file] Enables / disables logging all chatlines to file.");
    }
    if (ElPageControl1->ActivePageIndex == SettingsPage_Advanced)
    {
        Form2->Memo1->Lines->Add(" Advanced settings");
        Form2->Memo1->Lines->Add(" - Only use when having troubles!");
        Form2->Memo1->Lines->Add(" ");
        Form2->Memo1->Lines->Add(" [Sizeable] For window mode only, lock window resize.");
        Form2->Memo1->Lines->Add(" [Stay on top] For window mode only, keeps window visible.");
        Form2->Memo1->Lines->Add(" [Static loading] Enabled will not free up the memory.");
        Form2->Memo1->Lines->Add(" [Blocktransfer] Enable if you see black areas in game.");
    }
    Form2->Left = Left + (Width - Form2->Width) / 2;
    Form2->Top = Top + (Height - Form2->Height) / 2;
    Form2->ShowModal();
}
