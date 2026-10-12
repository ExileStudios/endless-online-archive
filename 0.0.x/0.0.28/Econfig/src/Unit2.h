#ifndef Unit2H
#define Unit2H

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
#include <ExtCtrls.hpp>
#include <StdCtrls.hpp>

class TForm2 : public TForm
{
__published:
        TPanel *Panel14;
        TPanel *Panel16;
        TPanel *Panel13;
        TPanel *Panel2;
        TSpeedButton *SpeedButton2;
        TPanel *Panel15;
        TMemo *Memo1;
        void __fastcall SpeedButton2Click(TObject *Sender);
private:
public:
        __fastcall TForm2(TComponent *Owner);
        char pad_0x2ec[4];
};

extern PACKAGE TForm2 *Form2;


#endif
