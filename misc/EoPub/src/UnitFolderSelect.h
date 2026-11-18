#ifndef UnitFolderSelectH
#define UnitFolderSelectH

#include <Classes.hpp>
#include <ElMenus.hpp>
#include <ElPopBtn.hpp>
#include <ExtCtrls.hpp>
#include <FileCtrl.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

// Picks a gfx, pub, export or backup folder, checking it for the files it needs.
class TFormFolderSelect : public TForm
{
__published:
    TPanel *Panel4;
    TLabel *problemText;
    TPanel *Panel2;
    TDirectoryListBox *DirectoryListBox1;
    TPanel *Panel1;
    TPanel *Panel5;
    TPanel *Panel6;
    TLabel *Label1;
    TPanel *Panel3;
    TImage *imageFileError;
    TImage *ImageFileOk;
    TPanel *Panel7;
    TLabel *labelChangeItemView;
    TLabel *labelSelectCaption;
    TImage *imageFolderArrow;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TTimer *Timer1;
    TElPopupButton *ElPopupButton1;
    TPanel *Panel8;
    TPanel *Panel9;
    TDriveComboBox *DriveComboBox1;
    TImage *imageGotoHome;
    TImage *imageGotoOldPath;
    TElPopupMenu *popupAutoScan;
    TElMenuItem *ElMenuItem1;
    TEdit *editFolderPath;
    void __fastcall SpeedButtonCancelClick();
    void __fastcall DirectoryListBox1Change(TObject *Sender);
    void __fastcall DriveComboBox1Change(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall ElPopupButton1Click(TObject *Sender);
    void __fastcall imageGotoOldPathClick(TObject *Sender);
    void __fastcall imageGotoHomeClick(TObject *Sender);
    void __fastcall ElMenuItem1Click(TObject *Sender);
    void __fastcall editFolderPathKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
private:
public:
    unsigned char folder_kind;
    String old_folder;
    String chosen_folder;
    char last_drive;
    bool path_pasted;

    __fastcall TFormFolderSelect(TComponent *owner);
    void Open(TForm *owner, String caption, String folder, unsigned char kind, char scan);
    char IsPubFolder(String folder);
    void OpenSelectedFolder();
    void ScanDrive();
    String ParentFolder(String path);
    String FullPath(String path);
    String WithoutSlash(String path);
    String WithSlash(String path);
    String SelectedFolder();
    void LayoutButtons();
};

extern PACKAGE TFormFolderSelect *FormFolderSelect;

#endif
