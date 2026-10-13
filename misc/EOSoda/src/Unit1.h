#ifndef Unit1H
#define Unit1H

#include <Classes.hpp>
#include <DBTables.hpp>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>

class TForm1 : public TForm
{
__published:
        TEdit *Edit1;
        TPanel *Panel1;
        TPanel *Panel2;
        TPanel *buttonOk;
        TMemo *MemoSodanized;
        TPanel *Panel4;
        TLabel *Label1;
        TPanel *Panel5;
        TPanel *Panel6;
        TLabel *Label2;
        TCheckBox *CheckBox1;
        TLabel *Label3;
        TTimer *Timer1;
        TImage *Image1;
        TPanel *Panel3;
        void __fastcall buttonOkClick(TObject *Sender);
        void __fastcall Edit1KeyUp(TObject *Sender, Word &Key, TShiftState Shift);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall Panel3Click(TObject *Sender);
        void __fastcall MemoSodanizedEnter(TObject *Sender);
private:
public:
        __fastcall TForm1(TComponent *owner);
        char pad_0x30c[4];

        static String Sodanize(TForm1 *self, String text);
        void CopyToClipboard(String text);
};

extern PACKAGE TForm1 *Form1;

#endif
