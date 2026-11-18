#ifndef UnitPathScanH
#define UnitPathScanH

#include <Classes.hpp>
#include <ElPopBtn.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

struct TFormFolderSelect;

// Searches a drive for the gfx or pub folder.
class TFormPathScan : public TForm
{
__published:
    TLabel *labelScanDrive;
    TPanel *Panel2;
    TPanel *Panel4;
    TLabel *problemText;
    TElPopupButton *buttonOk;
    TElPopupButton *buttonTabCatch;
    TLabel *labelCurrentFile;
    TImage *Image1;
    TTimer *Timer1;
    TLabel *labelCurrentFolder;
    TLabel *Label1;
    TLabel *labelScanStatus;
    TLabel *Label2;
    TLabel *Label3;
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall labelCurrentFolderClick(TObject *Sender);
private:
public:
    char drive;
    String target_file;
    bool stopped;
    unsigned char update_ticks;
    bool found;
    bool aborted;
    String found_folder;

    __fastcall TFormPathScan(TComponent *owner);
    void ShowScan(TFormFolderSelect *parent, char drive_letter, String file_name);
    void ScanFolder(String folder, unsigned char depth);
};

extern PACKAGE TFormPathScan *FormPathScan;

#endif
