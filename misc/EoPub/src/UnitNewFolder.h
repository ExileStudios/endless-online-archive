#ifndef UnitNewFolderH
#define UnitNewFolderH

#include <Classes.hpp>
#include <ElPopBtn.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

struct TFormFolderSelect;

// Asks for the name of a new folder.
class TFormNewFolder : public TForm
{
__published:
    TPanel *Panel2;
    TPanel *Panel4;
    TLabel *problemText;
    TElPopupButton *buttonCancel;
    TElPopupButton *buttonOk;
    TLabel *Label1;
    TLabel *Label2;
    TPanel *Panel1;
    TEdit *editName;
    TLabel *Label3;
    TImage *Image1;
    void __fastcall buttonCancelClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall editNameKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall Image1Click(TObject *Sender);
private:
public:
    String folder_name;

    __fastcall TFormNewFolder(TComponent *owner);
    void PlaceOver(TFormFolderSelect *parent, String folder);
    String GetNewFolderPath();
};

extern PACKAGE TFormNewFolder *FormNewFolder;

#endif
