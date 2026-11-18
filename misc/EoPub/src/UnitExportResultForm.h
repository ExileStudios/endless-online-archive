#ifndef UnitExportResultFormH
#define UnitExportResultFormH

#include <Classes.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

// A small note at the corner of the main window telling how the export went.
class TFormExportResult : public TForm
{
__published:
    TLabel *labelExportCaption;
    TLabel *Label2;
    TLabel *labelExportFIles;
    TTimer *Timer1;
    TImage *imageExportOk;
    TImage *ImageExportError;
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall labelExportFIlesClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
private:
public:

    __fastcall TFormExportResult(TComponent *owner);
    void ShowResult(String summary);
};

extern PACKAGE TFormExportResult *FormExportResult;

#endif
