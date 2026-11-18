#ifndef UnitPathConfigH
#define UnitPathConfigH

#include <Buttons.hpp>
#include <Classes.hpp>
#include <ElPopBtn.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

// Asks for the gfx and pub folders on the first start, or when they are not found.
class TFormPathConfig : public TForm
{
__published:
    TPanel *Panel1;
    TImage *Image1;
    TPanel *Panel6;
    TPanel *Panel7;
    TLabel *Label1;
    TLabel *pubPathProblemCaption;
    TLabel *Label6;
    TImage *Image2;
    TPanel *Panel4;
    TLabel *Label2;
    TPanel *Panel3;
    TSpeedButton *SpeedButton3;
    TImage *imageGfxFileError;
    TImage *ImageGfxFileOk;
    TLabel *Label8;
    TPanel *Panel5;
    TSpeedButton *SpeedButton4;
    TImage *imagePubFileError;
    TImage *ImagePubFileOk;
    TLabel *problemText;
    TLabel *pubPathInforCaption;
    TLabel *editGfxPath;
    TLabel *editPubPath;
    TLabel *Label5;
    TPanel *Panel2;
    TSpeedButton *SpeedButton5;
    TImage *ImagePubExportError;
    TImage *ImagePubExportOk;
    TLabel *editExportPath;
    TLabel *Label9;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TElPopupButton *buttonTabCatch;
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall SpeedButton3Click(TObject *Sender);
    void __fastcall SpeedButton4Click(TObject *Sender);
    void __fastcall SpeedButton5Click(TObject *Sender);
    void __fastcall editPubPathClick(TObject *Sender);
    void __fastcall editExportPathClick(TObject *Sender);
    void __fastcall editGfxPathClick(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
private:
public:

    __fastcall TFormPathConfig(TComponent *owner);
    void CheckPaths();
};

extern PACKAGE TFormPathConfig *FormPathConfig;

#endif
