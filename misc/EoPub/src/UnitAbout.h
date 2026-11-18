#ifndef UnitAboutH
#define UnitAboutH

#include <Classes.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

// The about box, with the version and a few hidden animations.
class TFormAbout : public TForm
{
__published:
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TLabel *Label4;
    TPanel *Panel1;
    TImage *Image1;
    TShape *Shape1;
    TLabel *Label5;
    TLabel *Label6;
    TShape *Shape2;
    TTimer *Timer1;
    TImage *Image2;
    TImage *Image3;
    void __fastcall Panel1Click(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall Label5Click(TObject *Sender);
    void __fastcall Label6Click(TObject *Sender);
    void __fastcall Image1Click(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
private:
public:
    short logo_left;
    short logo_shake_ticks;
    short window_shake_ticks;
    short window_left;
    short logo_clicks;
    short npc_walk_ticks;
    HINSTANCE__ *npc_library;

    __fastcall TFormAbout(TComponent *owner);
};

extern PACKAGE TFormAbout *FormAbout;

#endif
