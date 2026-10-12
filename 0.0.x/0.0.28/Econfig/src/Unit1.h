#ifndef Unit1H
#define Unit1H

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <ScktComp.hpp>
#include <DBTables.hpp>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <deque.h>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ElCheckCtl.hpp>
#include <ElCombos.hpp>
#include <ElEdits.hpp>
#include <ElPgCtl.hpp>
#include <ElSpin.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>

// The pages of the settings window, after the connection page ( 0 ).
enum SettingsPage
{
    SettingsPage_Graphics = 1,
    SettingsPage_Sound = 2,
    SettingsPage_Language = 3,
    SettingsPage_Chat = 4,
    SettingsPage_Advanced = 5
};

struct Config;

// The settings window: one page each for connection, graphics, sound, language, chat and advanced.
class TForm1 : public TForm
{
__published:
        TPanel *Panel1;
        TElPageControl *ElPageControl1;
        TElTabSheet *ElTabSheet1;
        TElTabSheet *ElTabSheet2;
        TElTabSheet *ElTabSheet3;
        TElTabSheet *ElTabSheet4;
        TElTabSheet *ElTabSheet5;
        TLabel *Label1;
        TLabel *Label2;
        TElSpinEdit *ElSpinEdit1;
        TLabel *Label3;
        TLabel *Label4;
        TElRadioButton *ElRadioButton1;
        TElRadioButton *ElRadioButton2;
        TLabel *Label5;
        TLabel *Label13;
        TElCheckBox *ElCheckBox1;
        TElCheckBox *ElCheckBox2;
        TLabel *Label14;
        TLabel *Label15;
        TElCheckBox *ElCheckBox3;
        TLabel *Label12;
        TLabel *Label11;
        TLabel *Label6;
        TElCheckBox *ElCheckBox5;
        TLabel *Label8;
        TLabel *Label9;
        TElCheckBox *ElCheckBox4;
        TLabel *Label10;
        TLabel *Label7;
        TLabel *Label16;
        TElRadioButton *ElRadioButton3;
        TElRadioButton *ElRadioButton4;
        TElRadioButton *ElRadioButton5;
        TLabel *Label18;
        TLabel *Label19;
        TLabel *Label17;
        TLabel *Label25;
        TLabel *Label26;
        TLabel *Label21;
        TElEdit *ElEdit2;
        TElCheckBox *ElCheckBox8;
        TElCheckBox *ElCheckBox7;
        TElCheckBox *ElCheckBox6;
        TLabel *Label23;
        TLabel *Label24;
        TLabel *Label20;
        TLabel *Label22;
        TLabel *Label27;
        TLabel *Label29;
        TLabel *Label31;
        TLabel *Label32;
        TLabel *Label33;
        TLabel *Label42;
        TElCheckBox *ElCheckBox14;
        TElCheckBox *ElCheckBox13;
        TElCheckBox *ElCheckBox12;
        TElCheckBox *ElCheckBox11;
        TElCheckBox *ElCheckBox10;
        TElCheckBox *ElCheckBox9;
        TLabel *Label28;
        TLabel *Label30;
        TLabel *Label34;
        TLabel *Label35;
        TLabel *Label36;
        TLabel *Label43;
        TElTabSheet *ElTabSheet6;
        TElComboBox *ElComboBox1;
        TLabel *Label37;
        TLabel *Label39;
        TElComboBox *ElComboBox2;
        TPanel *Panel2;
        TImage *Image1;
        TPanel *Panel3;
        TPanel *Panel4;
        TSpeedButton *SpeedButton2;
        TSpeedButton *SpeedButton1;
        TPanel *Panel5;
        TSpeedButton *SpeedButton9;
        TElComboBox *ElEdit1;
        void __fastcall SpeedButton2Click(TObject *Sender);
        void __fastcall ElCheckBox1Click(TObject *Sender);
        void __fastcall ElCheckBox2Click(TObject *Sender);
        void __fastcall ElCheckBox3Click(TObject *Sender);
        void __fastcall ElCheckBox4Click(TObject *Sender);
        void __fastcall ElCheckBox5Click(TObject *Sender);
        void __fastcall ElCheckBox6Click(TObject *Sender);
        void __fastcall ElCheckBox7Click(TObject *Sender);
        void __fastcall ElCheckBox8Click(TObject *Sender);
        void __fastcall ElCheckBox9Click(TObject *Sender);
        void __fastcall ElCheckBox10Click(TObject *Sender);
        void __fastcall ElCheckBox11Click(TObject *Sender);
        void __fastcall ElCheckBox12Click(TObject *Sender);
        void __fastcall ElCheckBox13Click(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Label38Click(TObject *Sender);
        void __fastcall SpeedButton1Click(TObject *Sender);
        void __fastcall ElCheckBox14Click(TObject *Sender);
        void __fastcall SpeedButton9Click(TObject *Sender);
private:
public:
        __fastcall TForm1(TComponent *Owner);
        String enabled_text;
        String disabled_text;
        char pad_0x418[4];
        Config *config;
};

extern PACKAGE TForm1 *Form1;


#endif
