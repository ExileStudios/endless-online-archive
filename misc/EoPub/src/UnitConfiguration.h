#ifndef UnitConfigurationH
#define UnitConfigurationH

#include <Buttons.hpp>
#include <Classes.hpp>
#include <ElCheckCtl.hpp>
#include <ElPgCtl.hpp>
#include <ElPopBtn.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

// The configuration window: the folders and every switch of the .ini file.
class TFormConfiguration : public TForm
{
__published:
    TPanel *Panel1;
    TImage *Image1;
    TPanel *Panel6;
    TPanel *Panel7;
    TLabel *Label1;
    TPanel *Panel4;
    TLabel *problemText;
    TElPageControl *ElPageControl1;
    TElTabSheet *ElTabSheet1;
    TElTabSheet *ElTabSheet2;
    TLabel *Label3;
    TLabel *Label8;
    TLabel *Label4;
    TPanel *Panel3;
    TSpeedButton *SpeedButton3;
    TImage *imageGfxFileError;
    TImage *ImageGfxFileOk;
    TPanel *Panel5;
    TSpeedButton *SpeedButton4;
    TImage *imagePubFileError;
    TImage *ImagePubFileOk;
    TPanel *Panel2;
    TImage *imagePubExportFileError;
    TSpeedButton *SpeedButton1;
    TImage *ImagePubExportFileOk;
    TPanel *Panel8;
    TSpeedButton *SpeedButton5;
    TImage *ImageBackupFileError;
    TImage *ImageBackupFileOk;
    TLabel *Label2;
    TLabel *Label5;
    TElCheckBox *checkBackup;
    TLabel *editGfxPath;
    TLabel *editPubPath;
    TLabel *editExportPath;
    TLabel *editBackupPath;
    TElTabSheet *ElTabSheet3;
    TTimer *Timer1;
    TLabel *Label7;
    TElCheckBox *checkListNumbers;
    TLabel *Label9;
    TElCheckBox *checkShowRecycled;
    TShape *Shape1;
    TElTabSheet *ElTabSheet4;
    TLabel *Label6;
    TLabel *Label10;
    TElCheckBox *checkExitSave;
    TElCheckBox *checkFullscreen;
    TShape *Shape2;
    TLabel *Label14;
    TElCheckBox *checkViewAll;
    TElTabSheet *ElTabSheet5;
    TLabel *Label12;
    TLabel *Label11;
    TElCheckBox *checkSmartFilter;
    TElCheckBox *checkShowBasics;
    TLabel *Label15;
    TElCheckBox *checkDeleteWarning;
    TLabel *Label17;
    TElCheckBox *checkHotkeys;
    TElCheckBox *checkBackupFolders;
    TLabel *Label18;
    TElCheckBox *checkExpertMode;
    TLabel *Label19;
    TElCheckBox *checkPasteWarning;
    TElPopupButton *ElPopupButton1;
    TElPopupButton *buttonOk;
    TElPopupButton *buttonCancel;
    TLabel *Label20;
    TElCheckBox *checkSmartEditors;
    TLabel *Label21;
    TElCheckBox *checkCreateRecycled;
    TLabel *Label16;
    TElCheckBox *checkMouseQuicksave;
    TShape *Shape3;
    TLabel *Label22;
    TElCheckBox *checkDarkmode;
    TLabel *Label23;
    TElCheckBox *checkRowSelect;
    TLabel *Label24;
    TElCheckBox *checkAutoCapitalization;
    TLabel *Label25;
    TElCheckBox *checkNameHints;
    TLabel *Label26;
    TElCheckBox *checkShowZeros;
    TLabel *Label27;
    TElCheckBox *checkMouseQuickedit;
    TLabel *Label13;
    TElCheckBox *checkExportEmpty;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall SpeedButton3Click(TObject *Sender);
    void __fastcall SpeedButton4Click(TObject *Sender);
    void __fastcall SpeedButton1Click(TObject *Sender);
    void __fastcall SpeedButton5Click(TObject *Sender);
    void __fastcall editGfxPathClick(TObject *Sender);
    void __fastcall editPubPathClick(TObject *Sender);
    void __fastcall editExportPathClick(TObject *Sender);
    void __fastcall editBackupPathClick(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall checkExpertModeClick(TObject *Sender);
    void __fastcall ElPopupButton1Click(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
private:
public:
    String enabled_text;
    String disabled_text;
    int status_ticks;
    String old_export_path;
    String shown_pub_path;

    __fastcall TFormConfiguration(TComponent *owner);
    void CheckPaths();
};

extern PACKAGE TFormConfiguration *FormConfiguration;

#endif
