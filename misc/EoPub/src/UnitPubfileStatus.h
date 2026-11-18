#ifndef UnitPubfileStatusH
#define UnitPubfileStatusH

#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <ElPgCtl.hpp>
#include <ElPopBtn.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

struct TFormMain;

// Shows the health of one kind's pub files, one line for each file read.
class TFormPubstatus : public TForm
{
__published:
    TPanel *Panel2;
    TPanel *Panel4;
    TElPageControl *ElPageControl1;
    TElTabSheet *ElTabSheet1;
    TLabel *labelPubTotals;
    TListView *listPubfiles;
    TElTabSheet *ElTabSheet3;
    TLabel *labelWeirdEof;
    TPanel *Panel1;
    TLabel *Label1;
    TLabel *labelDataLoss;
    TPanel *Panel5;
    TLabel *Label8;
    TLabel *labelTruncation;
    TPanel *Panel6;
    TLabel *Label10;
    TLabel *labelOOB;
    TLabel *Label4;
    TLabel *Label12;
    TImage *imageDataLossBad;
    TImage *imageTruncationBad;
    TImage *imageOOBbad;
    TImage *imageDataLossGood;
    TImage *imageTruncationGood;
    TImage *imageOOGood;
    TImage *imagePub;
    TLabel *labelPubTitle;
    TLabel *Label3;
    TPanel *Panel3;
    TImage *imageGoodHealth;
    TImage *imageBadHealth;
    TLabel *labelPubHealth;
    TPanel *Panel7;
    TLabel *Label2;
    TLabel *labelCorrupt;
    TImage *imageCorruptBad;
    TImage *imageCorruptGood;
    TElPopupButton *buttonOk;
    TElPopupButton *buttonTabCatch;
    void __fastcall FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall buttonTabCatchKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
private:
public:
    char health_level;

    __fastcall TFormPubstatus(TComponent *owner);
    void Open(TFormMain *main, String caption);
    void ResetHealth();
    void SetHealth(unsigned char level);
    void SetCorruption(unsigned char good, String text);
    void SetCorruptionHint(String hint);
    void SetDataLoss(unsigned char good, String text);
    void SetDataLossHint(String hint);
    void SetTruncation(unsigned char good, String text);
    void SetTruncationHint(String hint);
    void SetOutOfBounds(unsigned char good, String text);
    void SetOutOfBoundsHint(String hint);
};

extern PACKAGE TFormPubstatus *FormPubstatus;

#endif
