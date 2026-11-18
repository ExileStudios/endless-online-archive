#ifndef UnitExportFormH
#define UnitExportFormH

#include <Classes.hpp>
#include <ElBiProgr.hpp>
#include <ElCheckCtl.hpp>
#include <ElPopBtn.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>
#include <fstream>
#include <string>

// Exports every tab to pub files, after a backup of the old ones.
class TFormExport : public TForm
{
__published:
    TPanel *Panel1;
    TImage *Image1;
    TPanel *Panel6;
    TPanel *Panel7;
    TLabel *Label1;
    TPanel *Panel4;
    TPanel *panelExportStatusHolder;
    TLabel *Label2;
    TLabel *Label3;
    TShape *Shape1;
    TLabel *Label4;
    TLabel *exportPathTxt;
    TLabel *exportPathText;
    TTimer *Timer1;
    TPanel *panelWorking;
    TTimer *Timer2;
    TPanel *panelComplete;
    TLabel *panelCompleteTxt;
    TLabel *labelExportResult;
    TShape *Shape2;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TElPopupButton *buttonTabCatch;
    TPanel *panelProgress;
    TElBiProgressBar *ProgressBar1;
    TElCheckBox *checkWriteOversized;
    TPanel *panelDifferent;
    TLabel *Label5;
    TImage *Image2;
    TTimer *Timer3;
    TLabel *Label6;
    TShape *Shape3;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall exportPathTextClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall Timer2Timer(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall buttonTabCatchKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall Timer3Timer(TObject *Sender);
private:
public:
    int slide_stage;
    int slide_top;
    int signature;
    String backup_readme;
    unsigned char flash_ticks;
    unsigned char repaint_step;
    int export_rows;
    bool shared_signature;
    bool dry_run;
    unsigned char header_version;
    int rows_per_file;
    int file_size_limit;
    int files_saved;
    unsigned char export_error;
    bool export_items;
    bool export_npcs;
    bool export_skills;
    bool export_classes;
    bool export_shops;
    bool export_trainers;
    bool export_inns;
    bool export_drops;
    bool export_chats;
    int combo_index;

    __fastcall TFormExport(TComponent *owner);
    int CountExportRows();
    void StepProgress();
    void BackupPubFiles();
    void BackupFileParts(String backup_folder, String prefix, String extension);
    void BackupFile(String backup_folder, String file_name);
    String DigitsOnly(String text);
    String ExportClasses();
    String ExportItems();
    String ExportNpcs();
    String ExportSkills();
    String ExportShops();
    String ExportTrainers();
    String ExportInns();
    String ExportChats();
    String ExportDrops();
    String AppendFileNumber(String name, int number);
    String EncodeNumber(String text, int width);
    String EncodeInteger(unsigned int value, int width);
    int FileSignature();
    int Random();
};

extern PACKAGE TFormExport *FormExport;

#endif
