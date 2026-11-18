#ifndef UnitMainH
#define UnitMainH

#include <Buttons.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>
#include <fstream>
#include <string>
#include <vector.h>

#include "Configuration.h"
#include "Innvalues.h"
#include "Learnvalues.h"
#include "Npcvalues.h"
#include "Pubfileheader.h"
#include "Shopvalues.h"

// The importer's window: the import and output folders, the five .ini files found with their counts, and the lists.
class TMainForm : public TForm
{
__published:
    TPanel *Panel1;
    TImage *imageEndless;
    TPanel *Panel2;
    TPanel *Panel4;
    TLabel *labelCredits;
    TPanel *Panel3;
    TSpeedButton *buttonOk;
    TPageControl *PageControl1;
    TTabSheet *TabSheet1;
    TPanel *Panel5;
    TLabel *labelStatus;
    TLabel *labelNoNeed;
    TPanel *panelPath;
    TLabel *labelPath;
    TSpeedButton *buttonBrowse;
    TImage *ImageGfxFileOk;
    TImage *ImageGfxFileBad;
    TPanel *Panel6;
    TImage *imageNoFile;
    TImage *imageOk;
    TImage *imageBad;
    TImage *imageReady;
    TOpenDialog *OpenDialog1;
    TTabSheet *TabSheet2;
    TPanel *Panel7;
    TImage *Image1;
    TPanel *Panel8;
    TListView *exportGrid;
    TLabel *Label2;
    TPanel *Panel9;
    TLabel *labelWritePath;
    TSpeedButton *buttonBrowseWrite;
    TImage *ImageGfxFileOk2;
    TTabSheet *TabSheet3;
    TPanel *Panel10;
    TImage *imageOkExport;
    TPanel *Panel11;
    TLabel *labelFilesTotal;
    TPanel *Panel12;
    TLabel *labelFilesCreated;
    TLabel *Label3;
    TImage *imageNoExport;
    TPanel *Panel13;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall buttonBrowseClick(TObject *Sender);
    void __fastcall labelPathClick(TObject *Sender);
    void __fastcall buttonOkClick(TObject *Sender);
    void __fastcall PageControl1Change(TObject *Sender);
    void __fastcall labelWritePathClick(TObject *Sender);
    void __fastcall buttonBrowseWriteClick(TObject *Sender);
private:
public:
    Config *config;
    NpcValues *npcs;
    ShopValues *shops;
    LearnValues *trainers;
    InnValues *inns;
    String write_path_caption;
    String write_path;
    bool write_path_picked;
    String read_path_caption;
    String read_path;
    bool files_found;
    bool drops_found;
    String drops_ini;
    int drops_count;
    String drops_pub;
    bool chats_found;
    String chats_ini;
    int chats_count;
    String chats_pub;
    bool shops_found;
    String shops_ini;
    int shops_count;
    String shops_pub;
    bool trainers_found;
    String trainers_ini;
    int trainers_count;
    String trainers_pub;
    bool inns_found;
    String inns_ini;
    int inns_count;
    String inns_pub;
    int pub_version;
    bool dry_run;
    int files_created;
    int write_failed;

    __fastcall TMainForm(TComponent *owner);
    String FindImportFolder();
    String FindOutputFolder();
    void ScanImportFolder(String folder);
    String FindIniFile(String folder, String file_name);
    void UpdateReadPage();
    void UpdateWritePath();
    void FillExportGrid();
    bool PathExists(String path);
    String AddBackslash(String path);
    String RemoveBackslash(String path);
    String ExportChats();
    String ExportDrops();
    String ExportShops();
    String ExportTrainers();
    String ExportInns();
    String EncodeNumber(String text, int width);
    String EncodeNumber(unsigned int value, int width);
};

extern PACKAGE TMainForm *MainForm;

#endif
