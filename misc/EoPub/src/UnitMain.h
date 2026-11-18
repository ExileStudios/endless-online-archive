#ifndef UnitMainH
#define UnitMainH

#include <AppEvnts.hpp>
#include <Buttons.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <ElHeader.hpp>
#include <ElMenus.hpp>
#include <ElPgCtl.hpp>
#include <ElStrUtils.hpp>
#include <ElSysHotKey.hpp>
#include <ElTree.hpp>
#include <ElTreeGrids.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

#include "Configcheck.h"
#include "Configuration.h"
#include "Datagridui.h"
#include "Gfxvault.h"
#include "Sys_magicmap.h"
#include "Uitabclasses.h"
#include "Uitabinns.h"
#include "Uitabitems.h"
#include "Uitabnpcs.h"
#include "Uitabshops.h"
#include "Uitabskills.h"
#include "Uitabtrainers.h"

// The editor's main window: one tab with a main and a sub grid for each kind of pub data.
class TFormMain : public TForm
{
__published:
    TPanel *Panel1;
    TPanel *Panel6;
    TPanel *Panel7;
    TLabel *Label1;
    TImage *Image1;
    TLabel *mainLabel;
    TElPageControl *ElPageControl1;
    TElTabSheet *ElTabSheet1;
    TPanel *Panel2;
    TPanel *Panel3;
    TPanel *Panel4;
    TPanel *Panel5;
    TPanel *Panel9;
    TPanel *Panel10;
    TPanel *Panel14;
    TPanel *panelItemImage;
    TLabel *labelItemAdditional;
    TImage *imageItem;
    TImage *imageTile;
    TImage *imageItem2;
    TPanel *itemPreviewHeader;
    TLabel *Label10;
    TImage *imageItemGfx;
    TLabel *labelItemGfxName;
    TLabel *labelItemGfxID;
    TPanel *Panel15;
    TPanel *Panel16;
    TPanel *Panel17;
    TPanel *Panel18;
    TPanel *Panel19;
    TPanel *Panel20;
    TLabel *labelItemInspectorFilter;
    TImage *imageItemInspector;
    TLabel *labelItemInspectorName;
    TLabel *labelItemInspectorID;
    TPanel *Panel8;
    TPanel *Panel11;
    TLabel *labelChangeItemFilterType;
    TLabel *labelChangeItemView;
    TImage *imageChangeItemView;
    TPanel *Panel13;
    TLabel *Label5;
    TPanel *PanelSaveItemsHint;
    TLabel *Label6;
    TLabel *Label7;
    TEdit *editSearchItems;
    TElTreeStringGrid *ElTreeStringGrid1;
    TTimer *Timer1;
    TElTabSheet *ElTabSheet2;
    TElPopupMenu *popupItems;
    TElMenuItem *popupItemEdit;
    TElMenuItem *popupItemFree;
    TSpeedButton *buttonNewItem;
    TElMenuItem *popupItemNew;
    TPanel *Panel21;
    TPanel *Panel22;
    TPanel *Panel23;
    TPanel *Panel24;
    TPanel *Panel25;
    TPanel *Panel26;
    TPanel *Panel27;
    TPanel *Panel28;
    TPanel *classPreviewHeader;
    TLabel *Label18;
    TPanel *Panel30;
    TPanel *Panel31;
    TPanel *Panel32;
    TPanel *Panel33;
    TPanel *Panel34;
    TImage *imageClass;
    TPanel *Panel36;
    TPanel *Panel37;
    TLabel *labelChangeClassFilterType;
    TLabel *labelChangeClassView;
    TImage *imageChangeClassFilterType;
    TPanel *Panel38;
    TSpeedButton *buttonNewClass;
    TPanel *PanelSaveClassesHint;
    TLabel *Label22;
    TLabel *Label23;
    TElTreeStringGrid *ElTreeStringGrid3;
    TLabel *Label17;
    TEdit *editSearchClass;
    TPanel *Panel35;
    TLabel *tabNameTxt;
    TSpeedButton *SpeedButton3;
    TSpeedButton *SpeedButton4;
    TSpeedButton *SpeedButton5;
    TShape *Shape1;
    TElSysHotKey *hotkeySave;
    TElSysHotKey *hotkeyTab1;
    TElSysHotKey *hotkeyTab2;
    TSpeedButton *buttonToolsItem;
    TElTabSheet *ElTabSheet3;
    TElTabSheet *ElTabSheet4;
    TElSysHotKey *hotkeyTab3;
    TElSysHotKey *hotkeyTab4;
    TPanel *Panel12;
    TPanel *Panel40;
    TPanel *Panel41;
    TPanel *Panel42;
    TPanel *Panel43;
    TPanel *Panel44;
    TPanel *Panel45;
    TPanel *Panel46;
    TPanel *Panel47;
    TPanel *Panel48;
    TPanel *panelNpcImage;
    TImage *imageNpc;
    TPanel *npcPreviewHeader;
    TLabel *Label24;
    TImage *imageNpcGfx;
    TLabel *labelNpcGfxName;
    TLabel *labelNpcGfxID;
    TPanel *Panel51;
    TPanel *Panel52;
    TPanel *Panel53;
    TPanel *Panel54;
    TPanel *Panel55;
    TPanel *Panel56;
    TLabel *labelNpcInspectorFilter;
    TImage *imageNpcInspector;
    TLabel *labelNpcInspectorName;
    TLabel *labelNpcInspectorID;
    TPanel *Panel57;
    TPanel *Panel58;
    TLabel *labelChangeNpcFilterType;
    TLabel *labelChangeNpcView;
    TImage *imageChangeNpcFilterType;
    TPanel *Panel59;
    TLabel *Label32;
    TSpeedButton *buttonNewNpc;
    TPanel *PanelSaveNpcsHint;
    TLabel *Label33;
    TLabel *Label34;
    TEdit *editSearchNpcs;
    TElTreeStringGrid *ElTreeStringGrid5;
    TElTreeStringGrid *ElTreeStringGrid2;
    TElTreeStringGrid *ElTreeStringGrid6;
    TPanel *Panel39;
    TPanel *Panel49;
    TLabel *labelClassInspectorFilter;
    TImage *imageClassInspector;
    TLabel *labelClassInspectorName;
    TLabel *labelClassInspectorID;
    TElTreeStringGrid *ElTreeStringGrid4;
    TPanel *Panel50;
    TPanel *Panel60;
    TPanel *Panel61;
    TPanel *Panel62;
    TPanel *panelSkillImage;
    TImage *imageSkill;
    TPanel *skillPreviewHeader;
    TLabel *Label4;
    TImage *imageSkillGfx;
    TLabel *labelSkillGfxName;
    TLabel *labelSkillGfxID;
    TPanel *Panel65;
    TPanel *Panel66;
    TPanel *Panel67;
    TPanel *Panel68;
    TPanel *Panel69;
    TPanel *Panel70;
    TLabel *labelSkillInspectorFilter;
    TImage *imageSkillInspector;
    TLabel *labelSkillInspectorName;
    TLabel *labelSkillInspectorID;
    TElTreeStringGrid *ElTreeStringGrid8;
    TPanel *Panel71;
    TPanel *Panel72;
    TLabel *labelChangeSkillFilterType;
    TLabel *labelChangeSkillView;
    TImage *imageChangeSkillView;
    TPanel *Panel73;
    TLabel *Label19;
    TSpeedButton *buttonNewSkill;
    TPanel *PanelSaveSkillsHint;
    TLabel *Label21;
    TLabel *Label25;
    TEdit *editSearchSkills;
    TElTreeStringGrid *ElTreeStringGrid7;
    TImage *imageTile2;
    TImage *imageIconSkill;
    TElPopupMenu *popupNpcs;
    TElMenuItem *popupNpcNew;
    TElMenuItem *ElMenuItem2;
    TElMenuItem *popupNpcEdit;
    TElMenuItem *popupNpcFree;
    TElPopupMenu *popupSkills;
    TElMenuItem *popupSkillNew;
    TElMenuItem *ElMenuItem3;
    TElMenuItem *popupSkillEdit;
    TElMenuItem *popupSkillFree;
    TElPopupMenu *popupClasses;
    TElMenuItem *popupClassNew;
    TElMenuItem *ElMenuItem5;
    TElMenuItem *popupClassEdit;
    TElMenuItem *popupClassFree;
    TImage *imageDarkArrow;
    TShape *Shape2;
    TShape *Shape3;
    TShape *Shape4;
    TShape *Shape5;
    TPanel *panelInfoHostItems;
    TPanel *panelInsideInfoItems;
    TPanel *panelInfoHostNpcs;
    TPanel *panelInsideInfoNpcs;
    TPanel *panelInfoHostSkills;
    TPanel *panelInsideInfoSkills;
    TPanel *panelInfoHostClasses;
    TPanel *panelInsideInfoClasses;
    TElSysHotKey *hotkeyRefresh;
    TElMenuItem *popupItemDupe;
    TElMenuItem *popupItemInject;
    TElMenuItem *popupItemDestroy;
    TElMenuItem *ElMenuItem4;
    TElMenuItem *ElMenuItem1;
    TElMenuItem *popupNpcDupe;
    TElMenuItem *ElMenuItem6;
    TElMenuItem *popupSkillDupe;
    TElMenuItem *ElMenuItem7;
    TElMenuItem *ElMenuItem8;
    TElMenuItem *popupClassDupe;
    TElMenuItem *popupNpcDestroy;
    TElMenuItem *popupNpcInject;
    TElMenuItem *popupSkillInject;
    TElMenuItem *popupSkillDestroy;
    TElMenuItem *popupClassInject;
    TElMenuItem *popupClassDestroy;
    TApplicationEvents *ApplicationEvents1;
    TSpeedButton *buttonToolsNpc;
    TSpeedButton *buttonToolsSkill;
    TSpeedButton *buttonToolsClass;
    TImage *imageGold;
    TElTabSheet *ElTabSheet5;
    TElTabSheet *ElTabSheet6;
    TPanel *Panel29;
    TPanel *Panel63;
    TPanel *Panel64;
    TPanel *Panel74;
    TPanel *Panel75;
    TPanel *Panel76;
    TPanel *Panel77;
    TPanel *Panel78;
    TLabel *labelChangeShopFilterType;
    TLabel *labelChangeShopView;
    TImage *imageChangeShopFilterType;
    TPanel *Panel79;
    TLabel *Label8;
    TSpeedButton *buttonNewShop;
    TSpeedButton *buttonToolsShop;
    TPanel *PanelSaveShopsHint;
    TLabel *Label9;
    TLabel *Label11;
    TShape *Shape6;
    TEdit *editSearchShop;
    TPanel *panelInfoHostShops;
    TPanel *panelInsideInfoShops;
    TElTreeStringGrid *ElTreeStringGrid9;
    TPanel *Panel83;
    TPanel *Panel84;
    TPanel *Panel85;
    TPanel *Panel86;
    TPanel *panelShopImage;
    TImage *imageShop;
    TPanel *shopPreviewHeader;
    TLabel *Label15;
    TPanel *Panel89;
    TPanel *Panel90;
    TPanel *Panel91;
    TPanel *Panel92;
    TPanel *Panel93;
    TPanel *Panel94;
    TLabel *labelShopInspectorFilter;
    TImage *imageShopInspector;
    TLabel *labelShopInspectorName;
    TLabel *labelShopInspectorID;
    TElTreeStringGrid *ElTreeStringGrid10;
    TPanel *Panel95;
    TPanel *Panel96;
    TLabel *labelChangeTrainerFilterType;
    TLabel *labelChangeTrainerView;
    TImage *imageChangeTrainerFilterType;
    TPanel *Panel97;
    TLabel *Label29;
    TSpeedButton *buttonNewTrainer;
    TSpeedButton *buttonToolsTrainer;
    TPanel *PanelSaveTrainersHint;
    TLabel *Label30;
    TLabel *Label31;
    TShape *Shape7;
    TEdit *editSearchTrainer;
    TPanel *panelInfoHostTrainers;
    TPanel *panelInsideInfoTrainers;
    TElTreeStringGrid *ElTreeStringGrid11;
    TPanel *Panel101;
    TPanel *Panel102;
    TPanel *Panel103;
    TPanel *Panel104;
    TPanel *panelTrainerImage;
    TImage *imageTrainer;
    TPanel *trainerPreviewHeader;
    TLabel *Label38;
    TPanel *Panel107;
    TPanel *Panel108;
    TPanel *Panel109;
    TPanel *Panel110;
    TPanel *Panel111;
    TPanel *Panel112;
    TLabel *labelTrainerInspectorFilter;
    TImage *imageTrainerInspector;
    TLabel *labelTrainerInspectorName;
    TLabel *labelTrainerInspectorID;
    TElTreeStringGrid *ElTreeStringGrid12;
    TElSysHotKey *hotkeyTab5;
    TElSysHotKey *hotkeyTab6;
    TElPopupMenu *popupShops;
    TElMenuItem *popupShopNew;
    TElMenuItem *ElMenuItem10;
    TElMenuItem *popupShopDupe;
    TElMenuItem *popupShopEdit;
    TElMenuItem *popupShopFree;
    TElMenuItem *ElMenuItem14;
    TElMenuItem *popupShopDestroy;
    TElMenuItem *popupShopInject;
    TElPopupMenu *popupTrainers;
    TElMenuItem *popupTrainerNew;
    TElMenuItem *ElMenuItem18;
    TElMenuItem *popupTrainerDupe;
    TElMenuItem *popupTrainerEdit;
    TElMenuItem *popupTrainerFree;
    TElMenuItem *ElMenuItem22;
    TElMenuItem *popupTrainerDestroy;
    TElMenuItem *popupTrainerInject;
    TPanel *darkPanel1;
    TPanel *darkPanel2;
    TPanel *darkPanel3;
    TPanel *darkPanel4;
    TPanel *darkPanel5;
    TPanel *darkPanel6;
    TImage *imageShopLight;
    TImage *imageShopDark;
    TElTabSheet *ElTabSheet7;
    TPanel *Panel80;
    TPanel *Panel81;
    TPanel *Panel82;
    TPanel *Panel87;
    TPanel *Panel88;
    TLabel *labelChangeInnFilterType;
    TLabel *labelChangeInnView;
    TImage *imageChangeInnFilterType;
    TPanel *Panel98;
    TLabel *Label12;
    TSpeedButton *buttonNewInn;
    TSpeedButton *buttonToolsInn;
    TPanel *PanelSaveInnsHint;
    TLabel *Label14;
    TLabel *Label16;
    TShape *Shape8;
    TEdit *editSearchInn;
    TPanel *panelInfoHostInns;
    TPanel *panelInsideInfoInns;
    TElTreeStringGrid *ElTreeStringGrid13;
    TPanel *darkPanel7;
    TPanel *Panel113;
    TPanel *Panel114;
    TPanel *Panel115;
    TPanel *Panel116;
    TPanel *Panel117;
    TImage *imageInn;
    TPanel *innPreviewHeader;
    TLabel *Label27;
    TPanel *Panel119;
    TPanel *Panel120;
    TPanel *Panel121;
    TPanel *Panel122;
    TPanel *Panel123;
    TPanel *Panel124;
    TLabel *labelInnInspectorFilter;
    TImage *imageInnInspector;
    TLabel *labelInnInspectorName;
    TLabel *labelInnInspectorID;
    TElTreeStringGrid *ElTreeStringGrid14;
    TElSysHotKey *hotkeyTab7;
    TElPopupMenu *popupInns;
    TElMenuItem *popupInnNew;
    TElMenuItem *ElMenuItem11;
    TElMenuItem *popupInnDupe;
    TElMenuItem *popupInnEdit;
    TElMenuItem *popupInnFree;
    TElMenuItem *ElMenuItem16;
    TElMenuItem *popupInnDestroy;
    TElMenuItem *popupInnInject;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall imageItemClick(TObject *Sender);
    void __fastcall SpeedButton5Click(TObject *Sender);
    void __fastcall SpeedButton3Click(TObject *Sender);
    void __fastcall SpeedButton4Click(TObject *Sender);
    void __fastcall hotkeySavePress(TObject *Sender);
    void __fastcall hotkeyTab1Press(TObject *Sender);
    void __fastcall hotkeyTab2Press(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall buttonToolsItemClick(TObject *Sender);
    void __fastcall hotkeyTab3Press(TObject *Sender);
    void __fastcall hotkeyTab4Press(TObject *Sender);
    void __fastcall ElTreeStringGrid5ItemFocused(TObject *Sender);
    void __fastcall ElTreeStringGrid5Enter(TObject *Sender);
    void __fastcall ElTreeStringGrid6Enter(TObject *Sender);
    void __fastcall ElTreeStringGrid5InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall ElTreeStringGrid6InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall ElTreeStringGrid6Exit(TObject *Sender);
    void __fastcall ElTreeStringGrid5ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode);
    void __fastcall ElTreeStringGrid6ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode);
    void __fastcall npcPreviewHeaderClick(TObject *Sender);
    void __fastcall itemPreviewHeaderClick(TObject *Sender);
    void __fastcall ElTreeStringGrid1Enter(TObject *Sender);
    void __fastcall ElTreeStringGrid1InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall ElTreeStringGrid1ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode);
    void __fastcall ElTreeStringGrid1ItemFocused(TObject *Sender);
    void __fastcall ElTreeStringGrid2Enter(TObject *Sender);
    void __fastcall ElTreeStringGrid2Exit(TObject *Sender);
    void __fastcall ElTreeStringGrid2InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall ElTreeStringGrid2ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode);
    void __fastcall ElTreeStringGrid3Enter(TObject *Sender);
    void __fastcall ElTreeStringGrid3InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall ElTreeStringGrid3ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode);
    void __fastcall ElTreeStringGrid3ItemFocused(TObject *Sender);
    void __fastcall ElTreeStringGrid4Enter(TObject *Sender);
    void __fastcall ElTreeStringGrid4Exit(TObject *Sender);
    void __fastcall ElTreeStringGrid4InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall ElTreeStringGrid4ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode);
    void __fastcall classPreviewHeaderClick(TObject *Sender);
    void __fastcall skillPreviewHeaderClick(TObject *Sender);
    void __fastcall ElTreeStringGrid7Enter(TObject *Sender);
    void __fastcall ElTreeStringGrid7InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall ElTreeStringGrid7ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode);
    void __fastcall ElTreeStringGrid7ItemFocused(TObject *Sender);
    void __fastcall ElTreeStringGrid8Enter(TObject *Sender);
    void __fastcall ElTreeStringGrid8Exit(TObject *Sender);
    void __fastcall ElTreeStringGrid8InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall ElTreeStringGrid8ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode);
    void __fastcall ElPageControl1Change(TObject *Sender);
    void __fastcall popupItemNewClick(TObject *Sender);
    void __fastcall popupItemEditClick(TObject *Sender);
    void __fastcall popupItemFreeClick(TObject *Sender);
    void __fastcall popupItemsPopup(TObject *Sender);
    void __fastcall popupNpcNewClick(TObject *Sender);
    void __fastcall popupNpcEditClick(TObject *Sender);
    void __fastcall popupNpcFreeClick(TObject *Sender);
    void __fastcall popupNpcsPopup(TObject *Sender);
    void __fastcall popupSkillsPopup(TObject *Sender);
    void __fastcall popupSkillNewClick(TObject *Sender);
    void __fastcall popupSkillEditClick(TObject *Sender);
    void __fastcall popupSkillFreeClick(TObject *Sender);
    void __fastcall popupClassesPopup(TObject *Sender);
    void __fastcall popupClassNewClick(TObject *Sender);
    void __fastcall popupClassEditClick(TObject *Sender);
    void __fastcall popupClassFreeClick(TObject *Sender);
    void __fastcall ElTreeStringGrid1KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall ElTreeStringGrid5KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall ElTreeStringGrid7KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall ElTreeStringGrid3KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall imageNpcClick(TObject *Sender);
    void __fastcall imageSkillClick(TObject *Sender);
    void __fastcall imageIconSkillClick(TObject *Sender);
    void __fastcall hotkeyRefreshPress(TObject *Sender);
    void __fastcall popupItemDupeClick(TObject *Sender);
    void __fastcall popupItemDestroyClick(TObject *Sender);
    void __fastcall popupItemInjectClick(TObject *Sender);
    void __fastcall popupNpcDupeClick(TObject *Sender);
    void __fastcall popupNpcInjectClick(TObject *Sender);
    void __fastcall popupNpcDestroyClick(TObject *Sender);
    void __fastcall popupSkillDupeClick(TObject *Sender);
    void __fastcall popupSkillDestroyClick(TObject *Sender);
    void __fastcall popupSkillInjectClick(TObject *Sender);
    void __fastcall popupClassDupeClick(TObject *Sender);
    void __fastcall popupClassDestroyClick(TObject *Sender);
    void __fastcall popupClassInjectClick(TObject *Sender);
    void __fastcall ApplicationEvents1Deactivate(TObject *Sender);
    void __fastcall ApplicationEvents1Activate(TObject *Sender);
    void __fastcall ElTreeStringGrid2KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall ElTreeStringGrid1Scroll(TObject *Sender, TScrollBarKind ScrollBarKind, int ScrollCode);
    void __fastcall ElTreeStringGrid5Scroll(TObject *Sender, TScrollBarKind ScrollBarKind, int ScrollCode);
    void __fastcall ElTreeStringGrid7Scroll(TObject *Sender, TScrollBarKind ScrollBarKind, int ScrollCode);
    void __fastcall ElTreeStringGrid3Scroll(TObject *Sender, TScrollBarKind ScrollBarKind, int ScrollCode);
    void __fastcall ElTreeStringGrid1ShowLineHint(TObject *Sender, TElTreeItem *Item,
    TElHeaderSection *Section, TElFString &Text, THintWindow *HintWindow, TPoint &MousePos,
    bool &DoShowHint);
    void __fastcall buttonToolsNpcClick(TObject *Sender);
    void __fastcall buttonToolsSkillClick(TObject *Sender);
    void __fastcall buttonToolsClassClick(TObject *Sender);
    void __fastcall ElTreeStringGrid1KeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall ElTreeStringGrid2ItemFocused(TObject *Sender);
    void __fastcall ElTreeStringGrid9Enter(TObject *Sender);
    void __fastcall ElTreeStringGrid11Enter(TObject *Sender);
    void __fastcall ElTreeStringGrid9InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall ElTreeStringGrid11InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall ElTreeStringGrid9ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode);
    void __fastcall ElTreeStringGrid11ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode);
    void __fastcall ElTreeStringGrid9ItemFocused(TObject *Sender);
    void __fastcall ElTreeStringGrid11ItemFocused(TObject *Sender);
    void __fastcall ElTreeStringGrid9KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall ElTreeStringGrid11KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall ElTreeStringGrid9Scroll(TObject *Sender, TScrollBarKind ScrollBarKind, int ScrollCode);
    void __fastcall ElTreeStringGrid11Scroll(TObject *Sender, TScrollBarKind ScrollBarKind, int ScrollCode);
    void __fastcall shopPreviewHeaderClick(TObject *Sender);
    void __fastcall trainerPreviewHeaderClick(TObject *Sender);
    void __fastcall ElTreeStringGrid10Enter(TObject *Sender);
    void __fastcall ElTreeStringGrid12Enter(TObject *Sender);
    void __fastcall ElTreeStringGrid10Exit(TObject *Sender);
    void __fastcall ElTreeStringGrid12Exit(TObject *Sender);
    void __fastcall ElTreeStringGrid10InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall ElTreeStringGrid12InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall ElTreeStringGrid10ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode);
    void __fastcall ElTreeStringGrid12ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode);
    void __fastcall hotkeyTab5Press(TObject *Sender);
    void __fastcall hotkeyTab6Press(TObject *Sender);
    void __fastcall popupShopsPopup(TObject *Sender);
    void __fastcall popupTrainersPopup(TObject *Sender);
    void __fastcall popupShopNewClick(TObject *Sender);
    void __fastcall popupTrainerNewClick(TObject *Sender);
    void __fastcall popupShopDupeClick(TObject *Sender);
    void __fastcall popupTrainerDupeClick(TObject *Sender);
    void __fastcall popupShopEditClick(TObject *Sender);
    void __fastcall popupTrainerEditClick(TObject *Sender);
    void __fastcall popupShopFreeClick(TObject *Sender);
    void __fastcall popupTrainerFreeClick(TObject *Sender);
    void __fastcall popupTrainerDestroyClick(TObject *Sender);
    void __fastcall popupShopDestroyClick(TObject *Sender);
    void __fastcall popupShopInjectClick(TObject *Sender);
    void __fastcall popupTrainerInjectClick(TObject *Sender);
    void __fastcall buttonToolsShopClick(TObject *Sender);
    void __fastcall buttonToolsTrainerClick(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall hotkeyTab7Press(TObject *Sender);
    void __fastcall popupInnNewClick(TObject *Sender);
    void __fastcall popupInnDupeClick(TObject *Sender);
    void __fastcall popupInnEditClick(TObject *Sender);
    void __fastcall popupInnFreeClick(TObject *Sender);
    void __fastcall popupInnDestroyClick(TObject *Sender);
    void __fastcall popupInnInjectClick(TObject *Sender);
    void __fastcall ElTreeStringGrid13Enter(TObject *Sender);
    void __fastcall ElTreeStringGrid13InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall ElTreeStringGrid13ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode);
    void __fastcall ElTreeStringGrid13ItemFocused(TObject *Sender);
    void __fastcall ElTreeStringGrid13KeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift);
    void __fastcall ElTreeStringGrid13Scroll(TObject *Sender, TScrollBarKind ScrollBarKind, int ScrollCode);
    void __fastcall ElTreeStringGrid14Enter(TObject *Sender);
    void __fastcall ElTreeStringGrid14Exit(TObject *Sender);
    void __fastcall ElTreeStringGrid14InplaceEditorNeeded(TObject *Sender, TElTreeItem *Item, int SectionIndex,
    TElFieldType SupposedFieldType, TElTreeInplaceEditor * &Editor);
    void __fastcall ElTreeStringGrid14ItemChange(TObject *Sender, TElTreeItem *Item,
    TItemChangeMode ItemChangeMode);
    void __fastcall popupInnsPopup(TObject *Sender);
    void __fastcall buttonToolsInnClick(TObject *Sender);
    void __fastcall innPreviewHeaderClick(TObject *Sender);
private:
public:
    bool unused_flag;
    bool no_pubs_found;
    bool data_loss;
    bool header_conflict;
    bool data_corrupt;
    bool out_of_bounds;
    GfxVault *gfx_vault;
    MagicMapper *magic_mapper;
    String version;
    int timer_ticks;
    UiTabItems *items_tab;
    UiTabClasses *classes_tab;
    UiTabNpcs *npcs_tab;
    UiTabSkills *skills_tab;
    UiTabShops *shops_tab;
    UiTabTrainers *trainers_tab;
    UiTabInns *inns_tab;
    Config *config;
    ConfigCheck *config_check;
    bool started;
    String combo_text;
    String typed_text;
    String edit_backup;
    TElTreeStringGrid *edit_grid;
    TElTreeItem *edit_row;
    unsigned char edit_column;
    bool caption_altered;

    __fastcall TFormMain(TComponent *owner);
    void ThemeComponents(TForm *owner);
    bool AnyChanged();
    void ClearChanged();
    void LoadPubs();
    void CheckPubs();
    void PubsLoaded();
    void AddMissingClasses();
    int NumberFromDigits(String text);
    int OutOfBoundsValue(String text);
    void EnableHotkeys(bool on);
    void ShowAllColumns(char all);
    void SetItemHints(char on);
    void FillSubGrids(char clear);
    void BeginUpdate();
    void EndUpdate();
    void SetQuickEdit(bool on);
    void SetRowSelect(bool on);
    void SetHeaderLock(char locked);
    void HotkeyStatus();
    void HotkeyDupe();
    void HotkeyEdit();
    void HotkeyImage();
    void HotkeyNew();
    void HotkeyCopy();
    void HotkeyPaste();
    void HotkeyTools();
    String PaddedNumber(int value);
    int NumberOr(String text, int default_value);
    String TrimLeadingZeros(String text);
    void ShowPubStatus(unsigned char tab);
    void ShowPopupMenu(TObject *sender, TElTreeStringGrid *grid, String kind);
    void DeleteRow(TElTreeStringGrid *grid, DataGridUI *grid_ui);
    void DestroyRow(TElTreeStringGrid *grid, DataGridUI *grid_ui);
    void InjectRow(TElTreeStringGrid *grid, DataGridUI *grid_ui);
};

extern PACKAGE TFormMain *FormMain;

// Applies the dark theme when chosen, keeps the name box's font colour once, and clears the quick-save cell.
// Shared body: a class defines its FormShow with DEFINE_T_FORM_ITEM_EDIT_FORM_SHOW(TheClass); each class has its own
// imageOperation, font_color_saved, name_font_color, editName, edit_grid.
#define DEFINE_T_FORM_ITEM_EDIT_FORM_SHOW(Class) \
void __fastcall Class::FormShow(TObject *Sender) \
{ \
    if (FormMain->config->GetTheme() == "dark") \
    { \
        FormMain->ThemeComponents(this); \
        imageOperation->Picture->Bitmap = FormMain->imageDarkArrow->Picture->Bitmap; \
        Color = (TColor)0x525252; \
    } \
    if (!font_color_saved) \
    { \
        name_font_color = editName->Font->Color; \
        font_color_saved = true; \
    } \
    FormMain->combo_text = ""; \
    FormMain->edit_backup = ""; \
    FormMain->edit_grid = 0; \
    FormMain->edit_row = 0; \
    editName->SetFocus(); \
}

// Shared body: a class defines its FormKeyDown with DEFINE_FORM_KEY_DOWN(TheClass).
#define DEFINE_FORM_KEY_DOWN(Class) \
void __fastcall Class::FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift) \
{ \
    if (Key == VK_ESCAPE) \
        ((TCustomForm *)this)->Close(); \
    if (Shift.Contains(ssCtrl) && Key == 'A') \
        ((TEdit *)Sender)->SelectAll(); \
}

// Picks the row a duplicate goes to, a recycled or a new one, and shows the copy's from and to IDs.
// Shared body: a class defines its FindDupeId with DEFINE_FORM_FIND_DUPE_ID(TheClass); each class has its own pub_grid,
// record_index, record_id, save_mode, itemNameOriginalID, buttonOk.
#define DEFINE_FORM_FIND_DUPE_ID(Class) \
void Class::FindDupeId() \
{ \
    TCustomElTree *grid = pub_grid; \
    int source_index = record_index; \
    record_index = grid->Items->Count + 1; \
    record_id = IntToStr(grid->Items->Count + 1); \
    save_mode = 0; \
    int name_index = 0; \
    for (int i = 1; i < ((TElTreeStringGrid *)grid)->HeaderSections->Count; i++) \
    { \
        if (String(((TElTreeStringGrid *)grid)->HeaderSections->Item[i]->Text) == "Name") \
        { \
            name_index = i - 1; \
            break; \
        } \
    } \
    if (FormMain->config->GetCreateRecycled()) \
    { \
        for (int row = 0; grid->Items->Count > row; row++) \
        { \
            if (grid->Items->Item[row]->ColumnText->Strings[name_index].Length() < 2 || \
                LowerCase(grid->Items->Item[row]->ColumnText->Strings[name_index]) == "eof") \
            { \
                record_index = row; \
                save_mode = 1; \
                record_id = String(grid->Items->Item[row]->Text); \
                break; \
            } \
        } \
    } \
    itemNameOriginalID->Caption = "| ID: " + IntToStr(source_index + 1) + " > " + record_id; \
    buttonOk->Hint = WideString("Duplicate ID:" + IntToStr(source_index + 1) + " > " + record_id); \
    buttonOk->Caption = WideString("DUPE TO " + record_id); \
}

// Blanks a zero in the edited cell when zeros are hidden.
// Shared body: a class defines its editStringGridItemChange with DEFINE_EDIT_STRING_GRID_ITEM_CHANGE(TheClass).
#define DEFINE_EDIT_STRING_GRID_ITEM_CHANGE(Class) \
void __fastcall Class::editStringGridItemChange(TObject *Sender, TElTreeItem *Item, TItemChangeMode ItemChangeMode) \
{ \
    if (!FormMain->config->GetShowZeros() && Item->ColumnText->Count > 0) \
    { \
        if (String(Item->ColumnText->Strings[0]) == "0") \
        { \
            TElWideStrings *cells = Item->ColumnText; \
            cells->Strings[0] = WideString(""); \
        } \
    } \
}

// The word for how the save went: created, duplicated, recycled or updated.
// Shared body: a class defines its ResultText with DEFINE_RESULT_TEXT(TheClass); each class has its own save_mode,
// open_operation.
#define DEFINE_RESULT_TEXT(Class) \
String Class::ResultText() \
{ \
    if (!save_mode) \
        return "created"; \
    if (save_mode == 1) \
    { \
        if (open_operation == 2) \
            return "duplicated"; \
        if (!open_operation) \
            return "recycled"; \
        return "updated"; \
    } \
}

// Trims the name, capitalizes its first letter when auto capitalization is on, then checks it.
// Shared body: a class defines its editNameExit with DEFINE_EDIT_NAME_EXIT(TheClass); each class has its own
// CheckDuplicateName.
#define DEFINE_EDIT_NAME_EXIT(Class) \
void __fastcall Class::editNameExit(TObject *Sender) \
{ \
    String name = ((TEdit *)Sender)->Text; \
    name = name.Trim(); \
    ((TEdit *)Sender)->Text = name; \
    if (FormMain->config->GetAutoCapitalization() && ((TEdit *)Sender)->Text.Length() > 0) \
    { \
        String capitalized = ((TEdit *)Sender)->Text; \
        capitalized[1] = toupper(capitalized[1]); \
        ((TEdit *)Sender)->Text = capitalized; \
    } \
    CheckDuplicateName(); \
}

// Shows the name in yellow with a warning when another item already has it.
// Shared body: a class defines its CheckDuplicateName with DEFINE_FORM_CHECK_DUPLICATE_NAME(TheClass); each class has
// its own editName, imageWarnDupe, name_font_color, pub_grid, open_operation, name_column.
#define DEFINE_FORM_CHECK_DUPLICATE_NAME(Class) \
void Class::CheckDuplicateName() \
{ \
    TEdit *name_edit = editName; \
    imageWarnDupe->Visible = false; \
    String lower_name = LowerCase(name_edit->Text); \
    name_edit->Font->Color = (TColor)name_font_color; \
    name_edit->Hint = String(""); \
    try \
    { \
        int checked = 0; \
        for (int i = 0; i < pub_grid->Items->Count; i++) \
        { \
            TElTreeItem *row = pub_grid->Items->Item[i]; \
            if (!open_operation || i != pub_grid->Selected->Index) \
            { \
                if (LowerCase(row->ColumnText->Strings[name_column->text_index]) == lower_name) \
                { \
                    name_edit->Font->Color = (TColor)0x25BEF1; \
                    name_edit->Hint = name_edit->Text + " already exist ( ID: " + IntToStr(i + 1) + " ) "; \
                    imageWarnDupe->Hint = name_edit->Hint; \
                    imageWarnDupe->Visible = true; \
                    return; \
                } \
                checked++; \
                if (checked > 60000) \
                    break; \
            } \
        } \
    } \
    catch (...) \
    { \
    } \
}

// Settles the typed type text on a list entry, by partial name or number, or an out-of-bounds tag.
// Shared body: a class defines its comboEditItemTypeExit with DEFINE_COMBO_EDIT_ITEM_TYPE_EXIT(TheClass); each class
// has its own TypeChosen.
#define DEFINE_COMBO_EDIT_ITEM_TYPE_EXIT(Class) \
void __fastcall Class::comboEditItemTypeExit(TObject *Sender) \
{ \
    String text(((TElComboBox *)Sender)->Text); \
    String lower_text = LowerCase(text); \
    for (int i = 0; ((TElComboBox *)Sender)->Items->Count > i; i++) \
    { \
        if (((TElComboBox *)Sender)->Text.Length() > 2) \
        { \
            if (LowerCase(((TElComboBox *)Sender)->Items->Strings[i]).Pos(lower_text) > 0) \
            { \
                ((TElComboBox *)Sender)->ItemIndex = i; \
                ((TElComboBox *)Sender)->Text = ((TElComboBox *)Sender)->Items->Strings[i]; \
                TypeChosen(); \
                FormMain->combo_text = ((TElComboBox *)Sender)->Text; \
                return; \
            } \
        } \
        else if (((TElComboBox *)Sender)->Text == ((TElComboBox *)Sender)->Items->Strings[i]) \
        { \
            TypeChosen(); \
            FormMain->combo_text = ((TElComboBox *)Sender)->Text; \
            return; \
        } \
    } \
    int number = FormMain->NumberFromDigits(text); \
    if (number >= 0) \
    { \
        if (((TElComboBox *)Sender)->Items->Count > number) \
        { \
            ((TElComboBox *)Sender)->ItemIndex = number; \
            text = ((TElComboBox *)Sender)->Items->Strings[number]; \
        } \
        else \
        { \
            if (number > 252) \
                number = 252; \
            text = "[OOB-" + IntToStr(number) + "]"; \
        } \
        ((TElComboBox *)Sender)->Text = WideString(text); \
        FormMain->combo_text = ((TElComboBox *)Sender)->Text; \
    } \
    for (int j = 0; ((TElComboBox *)Sender)->Items->Count > j; j++) \
    { \
        if (((TElComboBox *)Sender)->Text == ((TElComboBox *)Sender)->Items->Strings[j]) \
        { \
            TypeChosen(); \
            FormMain->combo_text = ((TElComboBox *)Sender)->Text; \
            return; \
        } \
    } \
    int out_of_bounds = FormMain->OutOfBoundsValue(text); \
    if (out_of_bounds >= 0) \
    { \
        if (out_of_bounds > 252) \
            out_of_bounds = 252; \
        ((TElComboBox *)Sender)->Text = WideString("[OOB-" + IntToStr(out_of_bounds) + "]"); \
        TypeChosen(); \
        FormMain->combo_text = ((TElComboBox *)Sender)->Text; \
        return; \
    } \
    for (int k = 0; ((TElComboBox *)Sender)->Items->Count > k; k++) \
    { \
        if (((TElComboBox *)Sender)->Text == ((TElComboBox *)Sender)->Items->Strings[k]) \
        { \
            TypeChosen(); \
            FormMain->combo_text = ((TElComboBox *)Sender)->Text; \
            return; \
        } \
    } \
    ((TElComboBox *)Sender)->Text = WideString(FormMain->combo_text); \
    TypeChosen(); \
}

// Escape restores the combo's text from entry, Enter settles it; typing drops the list open.
// Shared body: a class defines its comboEditItemTypeKeyDown with DEFINE_COMBO_EDIT_ITEM_TYPE_KEY_DOWN(TheClass); each
// class has its own comboEditItemTypeExit.
#define DEFINE_COMBO_EDIT_ITEM_TYPE_KEY_DOWN(Class) \
void __fastcall Class::comboEditItemTypeKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift) \
{ \
    String text(((TElComboBox *)Sender)->Text); \
    if (Key == VK_ESCAPE) \
    { \
        if (((TElComboBox *)Sender)->Text != FormMain->combo_text) \
        { \
            ((TElComboBox *)Sender)->Text = WideString(FormMain->combo_text); \
            ((TElComboBox *)Sender)->SelectAll(); \
            if (!((TElComboBox *)Sender)->DroppedDown) \
                ((TElComboBox *)Sender)->DropDown(); \
        } \
    } \
    if (text.Length() < 1) \
        return; \
    if (((TElComboBox *)Sender)->DroppedDown) \
    { \
        if (Key == VK_OEM_4) \
            ((TElComboBox *)Sender)->CloseUp(false); \
    } \
    else \
        ((TElComboBox *)Sender)->DropDown(); \
    if (Key == VK_RETURN) \
    { \
        if (((TElComboBox *)Sender)->DroppedDown) \
            ((TElComboBox *)Sender)->CloseUp(false); \
        comboEditItemTypeExit(Sender); \
        FormMain->combo_text = ((TElComboBox *)Sender)->Text; \
        ((TElComboBox *)Sender)->SelectAll(); \
    } \
    if (!Key && !((TElComboBox *)Sender)->DroppedDown) \
        ((TElComboBox *)Sender)->DropDown(); \
}

// Enter opens the list on the first presses, then settles the remembered text; other keys remember it.
// Shared body: a class defines its comboEditItemTypeKeyUp with DEFINE_COMBO_EDIT_ITEM_TYPE_KEY_UP(TheClass); each class
// has its own comboEditItemTypeExit.
#define DEFINE_COMBO_EDIT_ITEM_TYPE_KEY_UP(Class) \
void __fastcall Class::comboEditItemTypeKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift) \
{ \
    String text(((TElComboBox *)Sender)->Text); \
    ((TElComboBox *)Sender)->Tag++; \
    if (Key == VK_RETURN) \
    { \
        if (((TElComboBox *)Sender)->Tag < 3) \
        { \
            if (!((TElComboBox *)Sender)->DroppedDown) \
                ((TElComboBox *)Sender)->DropDown(); \
            ((TElComboBox *)Sender)->SelectAll(); \
            return; \
        } \
        if (((TElComboBox *)Sender)->Text.Length() < 1) \
        { \
            if (FormMain->typed_text.Length() < 1) \
                return; \
            ((TElComboBox *)Sender)->Text = WideString(FormMain->typed_text); \
            comboEditItemTypeExit(Sender); \
            FormMain->combo_text = ((TElComboBox *)Sender)->Text; \
            FormMain->typed_text = ""; \
            ((TElComboBox *)Sender)->SelectAll(); \
        } \
        if (((TElComboBox *)Sender)->DroppedDown) \
            ((TElComboBox *)Sender)->CloseUp(false); \
        return; \
    } \
    FormMain->typed_text = text; \
}

// Escape puts back the cell value kept for quick save, when quick save is on.
// Shared body: a class defines its editStringGridKeyUp with DEFINE_EDIT_STRING_GRID_KEY_UP(TheClass); each class has
// its own edit_grid.
#define DEFINE_EDIT_STRING_GRID_KEY_UP(Class) \
void __fastcall Class::editStringGridKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift) \
{ \
    if (Key != VK_ESCAPE) \
        return; \
    if (!FormMain->config->GetMouseQuickSave()) \
        return; \
    if (!FormMain->edit_grid) \
        return; \
    if (!FormMain->edit_row) \
        return; \
    if (FormMain->edit_grid->Selected == NULL) \
        return; \
    if (((TElTreeStringGrid *)Sender)->Selected == NULL) \
        return; \
    TElWideStrings *cells = FormMain->edit_row->ColumnText; \
    cells->Strings[FormMain->edit_column] = WideString(FormMain->edit_backup); \
    FormMain->edit_backup = ""; \
    FormMain->edit_grid = 0; \
    FormMain->edit_row = 0; \
}

// Drops the number's leading zeros and caps it at the most the box allows.
// Shared body: a class defines its spinEditGfxExit with DEFINE_SPIN_EDIT_GFX_EXIT(TheClass).
#define DEFINE_SPIN_EDIT_GFX_EXIT(Class) \
void __fastcall Class::spinEditGfxExit(TObject *Sender) \
{ \
    ((TElSpinEdit *)Sender)->Text = WideString(FormMain->TrimLeadingZeros(((TElSpinEdit *)Sender)->Text)); \
    if (((TElSpinEdit *)Sender)->Tag < ((TElSpinEdit *)Sender)->Value) \
        ((TElSpinEdit *)Sender)->Value = ((TElSpinEdit *)Sender)->Tag; \
}

// Shared body: a class defines its NumberText with DEFINE_NUMBER_TEXT(TheClass).
#define DEFINE_NUMBER_TEXT(Class) \
String Class::NumberText(int value) \
{ \
    if (!FormMain->config->GetShowZeros() && value <= 0) \
        return ""; \
    return IntToStr(value); \
}

// Applies the dark theme when chosen, keeps the name box's font colour once, and focuses the name.
// Shared body: a class defines its FormShow with DEFINE_FORM_FORM_SHOW(TheClass); each class has its own
// imageOperation, font_color_saved, name_font_color, editName.
#define DEFINE_FORM_FORM_SHOW(Class) \
void __fastcall Class::FormShow(TObject *Sender) \
{ \
    if (FormMain->config->GetTheme() == "dark") \
    { \
        FormMain->ThemeComponents(this); \
        imageOperation->Picture->Bitmap = FormMain->imageDarkArrow->Picture->Bitmap; \
        Color = (TColor)0x525252; \
    } \
    if (!font_color_saved) \
    { \
        name_font_color = editName->Font->Color; \
        font_color_saved = true; \
    } \
    editName->SetFocus(); \
}

// Picks the row a duplicate goes to, a recycled or a new one, and shows the copy's from and to IDs.
// Shared body: a class defines its FindDupeId with DEFINE_FIND_DUPE_ID(TheClass); each class has its own pub_grid,
// record_index, record_id, save_mode, itemNameOriginalID, buttonOk.
#define DEFINE_FIND_DUPE_ID(Class) \
void Class::FindDupeId() \
{ \
    TCustomElTree *grid = pub_grid; \
    int source_index = record_index; \
    record_index = grid->Items->Count + 1; \
    record_id = IntToStr(grid->Items->Count + 1); \
    save_mode = 0; \
    int name_index = 0; \
    for (int i = 1; i < ((TElTreeStringGrid *)grid)->HeaderSections->Count; i++) \
    { \
        if (String(((TElTreeStringGrid *)grid)->HeaderSections->Item[i]->Text) == "Name") \
        { \
            name_index = i - 1; \
            break; \
        } \
    } \
    if (FormMain->config->GetCreateRecycled()) \
    { \
        for (int row = 0; grid->Items->Count > row; row++) \
        { \
            if (grid->Items->Item[row]->ColumnText->Strings[name_index].Length() < 2 || \
                LowerCase(grid->Items->Item[row]->ColumnText->Strings[name_index]) == "eof") \
            { \
                record_index = row; \
                save_mode = 1; \
                record_id = String(grid->Items->Item[row]->Text); \
                break; \
            } \
        } \
    } \
    itemNameOriginalID->Caption = "| ID: " + IntToStr(source_index + 1) + " > " + record_id; \
    buttonOk->Hint = WideString("Duplicate " + IntToStr(source_index + 1) + " > " + record_id); \
    buttonOk->Caption = WideString("DUPE TO " + record_id); \
}

// Drops the number's leading zeros and caps it at the most the box allows.
// Shared body: a class defines its spinEditStrExit with DEFINE_SPIN_EDIT_STR_EXIT(TheClass).
#define DEFINE_SPIN_EDIT_STR_EXIT(Class) \
void __fastcall Class::spinEditStrExit(TObject *Sender) \
{ \
    ((TElSpinEdit *)Sender)->Text = WideString(FormMain->TrimLeadingZeros(((TElSpinEdit *)Sender)->Text)); \
    if (((TElSpinEdit *)Sender)->Tag < ((TElSpinEdit *)Sender)->Value) \
        ((TElSpinEdit *)Sender)->Value = ((TElSpinEdit *)Sender)->Tag; \
}

// Shared body: a class defines its NumberOrZero with DEFINE_NUMBER_OR_ZERO(TheClass).
#define DEFINE_NUMBER_OR_ZERO(Class) \
int Class::NumberOrZero(String text) \
{ \
    if (text == "") \
        return 0; \
    return StrToInt(text); \
}

// Shared body: a class defines its HideZeroText with DEFINE_HIDE_ZERO_TEXT(TheClass).
#define DEFINE_HIDE_ZERO_TEXT(Class) \
String Class::HideZeroText(String text) \
{ \
    if (FormMain->config->GetShowZeros()) \
        return text; \
    if (text == "0") \
        return ""; \
    return text; \
}

// Shows the name in yellow with a warning when another class already has it.
// Shared body: a class defines its CheckDuplicateName with DEFINE_CHECK_DUPLICATE_NAME(TheClass); each class has its
// own editName, imageWarnDupe, name_font_color, names_grid, open_operation, name_column.
#define DEFINE_CHECK_DUPLICATE_NAME(Class) \
void Class::CheckDuplicateName() \
{ \
    TEdit *name_edit = editName; \
    imageWarnDupe->Visible = false; \
    String lower_name = LowerCase(name_edit->Text); \
    name_edit->Font->Color = (TColor)name_font_color; \
    name_edit->Hint = String(""); \
    try \
    { \
        int checked = 0; \
        for (int i = 0; i < names_grid->Items->Count; i++) \
        { \
            TElTreeItem *row = names_grid->Items->Item[i]; \
            if (!open_operation || i != names_grid->Selected->Index) \
            { \
                if (LowerCase(row->ColumnText->Strings[name_column->text_index]) == lower_name) \
                { \
                    name_edit->Font->Color = (TColor)0x25BEF1; \
                    name_edit->Hint = name_edit->Text + " already exist ( ID: " + IntToStr(i + 1) + " ) "; \
                    imageWarnDupe->Hint = name_edit->Hint; \
                    imageWarnDupe->Visible = true; \
                    return; \
                } \
                checked++; \
                if (checked > 60000) \
                    break; \
            } \
        } \
    } \
    catch (...) \
    { \
    } \
}

// Shared body: a class defines its FormShow with DEFINE_FORM_SHOW(TheClass); each class has its own buttonTabCatch.
#define DEFINE_FORM_SHOW(Class) \
void __fastcall Class::FormShow(TObject *Sender) \
{ \
    if (FormMain->config->GetTheme() == "dark") \
    { \
        FormMain->ThemeComponents(this); \
        Color = (TColor)0x525252; \
    } \
    buttonTabCatch->SetFocus(); \
}

// Shared body: a class defines its FormKeyDown with DEFINE_FORM_FORM_KEY_DOWN(TheClass).
#define DEFINE_FORM_FORM_KEY_DOWN(Class) \
void __fastcall Class::FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift) \
{ \
    if (Key == VK_ESCAPE || Key == VK_END) \
    { \
        ModalResult = mrOk; \
        ((TCustomForm *)this)->Close(); \
    } \
}

// Cleaning all values needs expert mode; without it the box is cleared with a warning.
// Shared body: a class defines its checkCleanerAllClick with DEFINE_CHECK_CLEANER_ALL_CLICK(TheClass).
#define DEFINE_CHECK_CLEANER_ALL_CLICK(Class) \
void __fastcall Class::checkCleanerAllClick(TObject *Sender) \
{ \
    if (!FormMain->config->GetExpertMode()) \
    { \
        TElCheckBox *check_box = (TElCheckBox *)Sender; \
        check_box->Checked = false; \
        FormWarning->ShowWarning(this, "Expert Needed", \
            "Cannot enable [all values] clean. Only expert users are allowed to reset the enitre column"); \
    } \
}

// The digits of the text as a number; 0 when it has none.
// Shared body: a class defines its DigitsOnly with DEFINE_DIGITS_ONLY(TheClass).
#define DEFINE_DIGITS_ONLY(Class) \
int Class::DigitsOnly(String text) \
{ \
    String digits = ""; \
    for (int i = 1; i <= text.Length(); i++) \
    { \
        if (text[i] == '0' || text[i] == '1' || text[i] == '2' || text[i] == '3' || text[i] == '4' || text[i] == '5' || \
            text[i] == '6' || text[i] == '7' || text[i] == '8' || text[i] == '9') \
            digits += text[i]; \
    } \
    if (digits == "") \
        return 0; \
    return StrToInt(digits); \
}

// An empty number becomes 0 and a number past the limit becomes the limit.
// Shared body: a class defines its SpinEditorAfter with DEFINE_SPIN_EDITOR_AFTER(TheClass).
#define DEFINE_SPIN_EDITOR_AFTER(Class) \
void __fastcall Class::SpinEditorAfter(TObject *sender, bool &changed) \
{ \
    TElTreeInplaceSpinEdit *editor = (TElTreeInplaceSpinEdit *)sender; \
    try \
    { \
        String text(editor->Editor->Text); \
        if (text.Length() <= 0 || text == "") \
            editor->Editor->Value = 0; \
        if (editor->Editor->Tag < editor->Editor->Value) \
            editor->Editor->Value = editor->Editor->Tag; \
    } \
    catch (...) \
    { \
        editor->Editor->Value = 0; \
    } \
}

// Cleans the chosen column and tells how many rows were set back, or that nothing was found.
// Shared body: a class defines its buttonOkClick with DEFINE_BUTTON_OK_CLICK(TheClass); each class has its own
// ElPageControl1, tabCleaner, CleanColumn, checkCleanerOOB, checkCleanerAll, cleaned_count.
#define DEFINE_BUTTON_OK_CLICK(Class) \
void __fastcall Class::buttonOkClick(TObject *Sender) \
{ \
    if (ElPageControl1->ActivePageIndex == tabCleaner->PageIndex) \
    { \
        if (!CleanColumn(checkCleanerOOB->Checked, checkCleanerAll->Checked)) \
        { \
            FormWarning->ShowWarning(this, "Nothing To Clean", \
                "No fields were found to be cleaned based on your current column and target."); \
            return; \
        } \
        String message = \
            IntToStr(cleaned_count) + " items set to default (zero). Please check the results before saving."; \
        FormWarning->ShowWarning(this, " Column Wiped", message); \
    } \
    ModalResult = mrOk; \
}

// Applies the dark theme when chosen, clears the quick-save cell and selects the first line.
// Shared body: a class defines its FormShow with DEFINE_T_FORM_NPC_EDIT_CHATS_FORM_SHOW(TheClass); each class has its
// own imageOperation, lineGrid, Panel1.
#define DEFINE_T_FORM_NPC_EDIT_CHATS_FORM_SHOW(Class) \
void __fastcall Class::FormShow(TObject *Sender) \
{ \
    if (FormMain->config->GetTheme() == "dark") \
    { \
        FormMain->ThemeComponents(this); \
        imageOperation->Picture->Bitmap = FormMain->imageDarkArrow->Picture->Bitmap; \
        Color = (TColor)0x525252; \
    } \
    FormMain->combo_text = ""; \
    FormMain->edit_backup = ""; \
    FormMain->edit_grid = 0; \
    FormMain->edit_row = 0; \
    if ((char)((int __fastcall (*)(int))FindDynaInst((void *)lineGrid, -68))((int)lineGrid) && \
        lineGrid->Items->Count > 0) \
    { \
        lineGrid->Items->Item[0]->Selected = true; \
        lineGrid->Items->Item[0]->Focused = true; \
        lineGrid->Selected = lineGrid->Items->Item[0]; \
        Panel1->SetFocus(); \
        lineGrid->SetFocus(); \
    } \
}

// Lines up the operation label, arrow, name and ID captions, and sets the form and save button captions.
// Shared body: a class defines its LayoutCaptions with DEFINE_LAYOUT_CAPTIONS(TheClass); each class has its own
// itemNameOriginalID, itemNameOriginal, labelOperation, imageOperation, buttonOk, record_index.
#define DEFINE_LAYOUT_CAPTIONS(Class) \
void Class::LayoutCaptions() \
{ \
    itemNameOriginalID->Left = itemNameOriginal->Left + itemNameOriginal->Width + 14; \
    Caption = " " + labelOperation->Caption + " : " + itemNameOriginal->Caption; \
    imageOperation->Left = labelOperation->Left + labelOperation->Width + 8; \
    itemNameOriginal->Left = imageOperation->Left + imageOperation->Width + 8; \
    itemNameOriginalID->Left = itemNameOriginal->Left + itemNameOriginal->Width + 8; \
    buttonOk->Caption = WideString("UPDATE | ID: " + IntToStr(record_index + 1)); \
}

// Numbers the lines by their position.
// Shared body: a class defines its Renumber with DEFINE_FORM_RENUMBER(TheClass); each class has its own lineGrid.
#define DEFINE_FORM_RENUMBER(Class) \
void Class::Renumber() \
{ \
    TElTreeItem *row; \
    for (int i = 0; i < lineGrid->Items->Count; i++) \
    { \
        row = lineGrid->Items->Item[i]; \
        row->Text = WideString(IntToStr(i + 1)); \
    } \
}

// Shows the npc's graphic from the npc library at double size, or an error caption when it is missing.
// Shared body: a class defines its ShowNpcGraphic with DEFINE_SHOW_NPC_GRAPHIC(TheClass); each class has its own
// imageNpc, panelImage, panelLabel.
#define DEFINE_SHOW_NPC_GRAPHIC(Class) \
void Class::ShowNpcGraphic(int gfx) \
{ \
    HINSTANCE__ *library; \
    int resource_step; \
    TImage *image = imageNpc; \
    TPanel *panel = panelImage; \
    int first_resource = 101; \
    TLabel *label = panelLabel; \
    resource_step = 40; \
    library = FormMain->gfx_vault->npcs_library; \
    if (gfx < 1) \
        gfx = 1; \
    int resource_id = first_resource + (gfx - 1) * resource_step; \
    image->Picture->Bitmap->TransparentColor = clBlack; \
    image->Picture->Bitmap->Transparent = true; \
    label->Caption = "res.id:" + String(resource_id); \
    try \
    { \
        image->Picture->Bitmap->LoadFromResourceID((unsigned int)library, resource_id); \
    } \
    catch (...) \
    { \
        if (panel) \
            panel->Caption = "(not found)"; \
        image->Visible = false; \
        label->Caption = "error: " + String(resource_id); \
        label->Font->Color = clRed; \
        return; \
    } \
    image->Visible = true; \
    label->Font->Color = (TColor)0x808040; \
    image->Width = 2 * image->Picture->Bitmap->Width; \
    image->Height = 2 * image->Picture->Bitmap->Height; \
    if (panel) \
    { \
        panel->Caption = ""; \
        image->Left = (panel->Width - image->Width) / 2; \
        image->Top = (panel->Height - image->Height) / 2; \
    } \
}

// Swaps the selected line's cells with the next line's.
// Shared body: a class defines its popupMoveDownClick with DEFINE_FORM_POPUP_MOVE_DOWN_CLICK(TheClass); each class has
// its own lineGrid.
#define DEFINE_FORM_POPUP_MOVE_DOWN_CLICK(Class) \
void __fastcall Class::popupMoveDownClick(TObject *Sender) \
{ \
    TElWideStrings *next_cells; \
    if (lineGrid->Selected == NULL) \
        return; \
    int row_index = lineGrid->Selected->Index; \
    if (lineGrid->Items->Count <= row_index + 1) \
        return; \
    for (int column = 0; column < lineGrid->HeaderSections->Count - 1; column++) \
    { \
        String current_text(lineGrid->Items->Item[row_index]->ColumnText->Strings[column]); \
        String next_text(lineGrid->Items->Item[row_index + 1]->ColumnText->Strings[column]); \
        TElWideStrings *current_cells = lineGrid->Items->Item[row_index]->ColumnText; \
        current_cells->Strings[column] = WideString(next_text); \
        next_cells = lineGrid->Items->Item[row_index + 1]->ColumnText; \
        next_cells->Strings[column] = WideString(current_text); \
    } \
}

// Swaps the selected line's cells with the previous line's.
// Shared body: a class defines its popupMoveUpClick with DEFINE_FORM_POPUP_MOVE_UP_CLICK(TheClass); each class has its
// own lineGrid.
#define DEFINE_FORM_POPUP_MOVE_UP_CLICK(Class) \
void __fastcall Class::popupMoveUpClick(TObject *Sender) \
{ \
    TElWideStrings *previous_cells; \
    if (lineGrid->Selected == NULL) \
        return; \
    int row_index = lineGrid->Selected->Index; \
    if (row_index < 1) \
        return; \
    for (int column = 0; column < lineGrid->HeaderSections->Count - 1; column++) \
    { \
        String current_text(lineGrid->Items->Item[row_index]->ColumnText->Strings[column]); \
        String previous_text(lineGrid->Items->Item[row_index - 1]->ColumnText->Strings[column]); \
        TElWideStrings *current_cells = lineGrid->Items->Item[row_index]->ColumnText; \
        current_cells->Strings[column] = WideString(previous_text); \
        previous_cells = lineGrid->Items->Item[row_index - 1]->ColumnText; \
        previous_cells->Strings[column] = WideString(current_text); \
    } \
}

// Deletes the selected row, notes the deletion for the save warning, and renumbers the rows.
// Shared body: a class defines its popupDeleteClick with DEFINE_FORM_POPUP_DELETE_CLICK(TheClass); each class has its
// own lineGrid, rows_deleted, Renumber, ShowCount.
#define DEFINE_FORM_POPUP_DELETE_CLICK(Class) \
void __fastcall Class::popupDeleteClick(TObject *Sender) \
{ \
    if (lineGrid->Selected != NULL) \
    { \
        if (lineGrid->Items->Count > 0) \
            rows_deleted = true; \
        lineGrid->IsUpdating = true; \
        lineGrid->Selected->Delete(); \
        Renumber(); \
        lineGrid->IsUpdating = false; \
        ShowCount(); \
    } \
}

// Shared body: a class defines its lineGridEnter with DEFINE_LINE_GRID_ENTER(TheClass); each class has its own
// lineGrid.
#define DEFINE_LINE_GRID_ENTER(Class) \
void __fastcall Class::lineGridEnter(TObject *Sender) \
{ \
    TElTreeStringGrid *grid = lineGrid; \
    if (grid->Selected == NULL && grid->Items->Count > 0) \
    { \
        grid->Items->Item[0]->Selected = true; \
        grid->Items->Item[0]->Focused = true; \
    } \
}

// Trims the edited text, and accepts it at once when quick save is on.
// Shared body: a class defines its ElTreeInplaceEdit1AfterOperation with
// DEFINE_EL_TREE_INPLACE_EDIT1_AFTER_OPERATION(TheClass).
#define DEFINE_EL_TREE_INPLACE_EDIT1_AFTER_OPERATION(Class) \
void __fastcall Class::ElTreeInplaceEdit1AfterOperation(TObject *Sender, bool &Accepted, bool &DefaultConversion) \
{ \
    String text(((TElTreeInplaceEdit *)Sender)->Editor->Text); \
    text = text.Trim(); \
    ((TElTreeInplaceEdit *)Sender)->Editor->Text = WideString(text); \
    if (FormMain->config->GetMouseQuickSave()) \
        Accepted = true; \
}

// Ctrl+Up and Ctrl+Down move the selected row; Escape closes the form unless a cell is being edited.
// Shared body: a class defines its lineGridKeyDown with DEFINE_LINE_GRID_KEY_DOWN(TheClass); each class has its own
// popupMoveUpClick, popupMoveDownClick, lineGrid.
#define DEFINE_LINE_GRID_KEY_DOWN(Class) \
void __fastcall Class::lineGridKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift) \
{ \
    if (Shift.Contains(ssCtrl)) \
    { \
        if (Key == VK_UP) \
            popupMoveUpClick((TObject *)this); \
        if (Key == VK_DOWN) \
            popupMoveDownClick((TObject *)this); \
    } \
    if (Key == VK_ESCAPE && !((TCustomElTree *)lineGrid)->IsEditing()) \
        ((TCustomForm *)this)->Close(); \
}

// Applies the dark theme when chosen, clears the quick-save cell and selects the first row.
// Shared body: a class defines its FormShow with DEFINE_T_FORM_NPC_EDIT_DROPS_FORM_SHOW(TheClass); each class has its
// own imageOperation, dropGrid, Panel1.
#define DEFINE_T_FORM_NPC_EDIT_DROPS_FORM_SHOW(Class) \
void __fastcall Class::FormShow(TObject *Sender) \
{ \
    if (FormMain->config->GetTheme() == "dark") \
    { \
        FormMain->ThemeComponents(this); \
        imageOperation->Picture->Bitmap = FormMain->imageDarkArrow->Picture->Bitmap; \
        Color = (TColor)0x525252; \
    } \
    FormMain->combo_text = ""; \
    FormMain->edit_backup = ""; \
    FormMain->edit_grid = 0; \
    FormMain->edit_row = 0; \
    if ((char)((int __fastcall (*)(int))FindDynaInst((void *)dropGrid, -68))((int)dropGrid) && \
        dropGrid->Items->Count > 0) \
    { \
        dropGrid->Items->Item[0]->Selected = true; \
        dropGrid->Items->Item[0]->Focused = true; \
        dropGrid->Selected = dropGrid->Items->Item[0]; \
        Panel1->SetFocus(); \
        dropGrid->SetFocus(); \
    } \
}

// Shared body: a class defines its Renumber with DEFINE_RENUMBER(TheClass); each class has its own dropGrid.
#define DEFINE_RENUMBER(Class) \
void Class::Renumber() \
{ \
    TElTreeItem *row; \
    for (int i = 0; i < dropGrid->Items->Count; i++) \
    { \
        row = dropGrid->Items->Item[i]; \
        row->Text = WideString(IntToStr(i + 1)); \
    } \
}

// Shared body: a class defines its FindItemRow with DEFINE_FIND_ITEM_ROW(TheClass).
#define DEFINE_FIND_ITEM_ROW(Class) \
TElTreeItem *Class::FindItemRow(String item_id) \
{ \
    TElTreeStringGrid *item_grid = FormMain->items_tab->main_grid; \
    for (int i = 0; i < item_grid->Items->Count; i++) \
    { \
        TElTreeItem *row = item_grid->Items->Item[i]; \
        if (row->Text == item_id) \
            return row; \
    } \
    return 0; \
}

// Shows the item graphic in the image, gold for 35, at triple size when it fits, else double.
// Shared body: a class defines its ShowItemGraphic with DEFINE_SHOW_ITEM_GRAPHIC(TheClass); each class has its own
// hintImagePanel.
#define DEFINE_SHOW_ITEM_GRAPHIC(Class) \
void Class::ShowItemGraphic(TImage *image, TPanel *panel, int gfx) \
{ \
    HINSTANCE__ *library; \
    int resource_id; \
    int image_top; \
    TImage *shown_image = image; \
    int resource_step = 2; \
    int first_resource = 101; \
    TPanel *image_panel = hintImagePanel; \
    library = FormMain->gfx_vault->items_library; \
    if (gfx < 1) \
        gfx = 1; \
    resource_id = first_resource + (gfx - 1) * resource_step; \
    shown_image->Picture->Bitmap->TransparentColor = clBlack; \
    shown_image->Picture->Bitmap->Transparent = true; \
    try \
    { \
        if (gfx == 35) \
            shown_image->Picture->Bitmap = FormMain->imageGold->Picture->Bitmap; \
        else \
            shown_image->Picture->Bitmap->LoadFromResourceID((unsigned int)library, resource_id); \
    } \
    catch (...) \
    { \
        if (image_panel) \
            image_panel->Caption = "(n/a)"; \
        shown_image->Visible = false; \
        return; \
    } \
    shown_image->Visible = true; \
    if (3 * shown_image->Picture->Bitmap->Width < image_panel->Width && \
        3 * shown_image->Picture->Bitmap->Height < image_panel->Height) \
    { \
        shown_image->Width = 3 * shown_image->Picture->Bitmap->Width; \
        shown_image->Height = 3 * shown_image->Picture->Bitmap->Height; \
    } \
    else \
    { \
        shown_image->Width = 2 * shown_image->Picture->Bitmap->Width; \
        shown_image->Height = 2 * shown_image->Picture->Bitmap->Height; \
    } \
    if (image_panel) \
    { \
        image_panel->Caption = ""; \
        shown_image->Left = (image_panel->Width - shown_image->Width) / 2; \
        shown_image->Top = (image_panel->Height - shown_image->Height) / 2; \
        image_top = shown_image->Top; \
        if (image_top < 0) \
            shown_image->Top = image_top / 3; \
    } \
}

// Shows the hint with the name, type and graphic of the drop's item under the mouse.
// Shared body: a class defines its ShowItemHint with DEFINE_SHOW_ITEM_HINT(TheClass); each class has its own hint_row,
// dropGrid, hintPanel, FindItemRow, Panel1, hintCaption, hintIDCaption, hintSubString, ShowItemGraphic, hintImage,
// hintImagePanel.
#define DEFINE_SHOW_ITEM_HINT(Class) \
void Class::ShowItemHint(int x, int y) \
{ \
    int hint_right; \
    int row_index; \
    if (hint_row == -2) \
    { \
        hint_row = -1; \
        return; \
    } \
    int hint_left = dropGrid->HeaderSections->Item[0]->Width; \
    hint_right = dropGrid->HeaderSections->Item[0]->Width; \
    hint_right += dropGrid->HeaderSections->Item[1]->Width; \
    hint_right += dropGrid->HeaderSections->Item[2]->Width; \
    int row_count = dropGrid->Items->Count; \
    if (x < hint_left) \
    { \
        hintPanel->Visible = false; \
        return; \
    } \
    if (hint_right < x) \
    { \
        hintPanel->Visible = false; \
        return; \
    } \
    if (y < 0) \
    { \
        hintPanel->Visible = false; \
        return; \
    } \
    if (24 * row_count < y) \
    { \
        hintPanel->Visible = false; \
        return; \
    } \
    int hover_column = 0; \
    row_index = y / dropGrid->DefaultRowHeight; \
    int visible_row = row_index; \
    row_index += dropGrid->TopIndex; \
    if (row_index >= row_count) \
    { \
        hintPanel->Visible = false; \
        return; \
    } \
    if (row_index == hint_row) \
    { \
        hintPanel->Visible = true; \
        return; \
    } \
    hint_row = row_index; \
    TElTreeItem *row = dropGrid->Items->Item[row_index]; \
    TElTreeItem *item_row = FindItemRow(row->ColumnText->Strings[0]); \
    if (dropGrid->HeaderSections->Item[0]->Width < x) \
        hover_column = 1; \
    if (dropGrid->HeaderSections->Item[0]->Width + dropGrid->HeaderSections->Item[1]->Width < x) \
        hover_column = 2; \
    int hint_x = Panel1->Left; \
    int hint_y = Panel1->Top; \
    hint_x += 20; \
    if (hover_column == 1) \
        hint_x += dropGrid->HeaderSections->Item[0]->Width; \
    if (hover_column == 2) \
    { \
        hint_x += dropGrid->HeaderSections->Item[0]->Width; \
        hint_x += dropGrid->HeaderSections->Item[1]->Width; \
    } \
    hint_y += dropGrid->Top + dropGrid->DefaultRowHeight + 4; \
    hint_y += dropGrid->DefaultRowHeight * visible_row; \
    DataGridCol *name_grid_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Name"); \
    DataGridCol *gfx_grid_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Gfx"); \
    DataGridCol *type_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Type"); \
    DataGridCol *gender_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Data.2"); \
    String item_type = "item file incomplete"; \
    String item_name = "[ITEM OOB]"; \
    String item_id(row->ColumnText->Strings[0]); \
    String gfx_text = "64001"; \
    if (item_row) \
    { \
        item_type = item_row->ColumnText->Strings[type_column->text_index]; \
        item_name = item_row->ColumnText->Strings[name_grid_column->text_index]; \
        item_id = item_row->Text; \
        if (item_row->ColumnText->Strings[gfx_grid_column->text_index].Length() > 0) \
            gfx_text = item_row->ColumnText->Strings[gfx_grid_column->text_index]; \
    } \
    hintCaption->Caption = item_name; \
    hintIDCaption->Caption = "| ID: " + item_id; \
    hintSubString->Caption = item_type; \
    hintIDCaption->Left = hintCaption->Left + hintCaption->Width + 8; \
    hintPanel->Width = 337; \
    int needed_width = hintIDCaption->Left + hintIDCaption->Width + 24; \
    if (needed_width > hintPanel->Width) \
        hintPanel->Width = needed_width; \
    if (item_row) \
    { \
        if (LowerCase(item_type).Pos("armor") > 0) \
        { \
            String gender_text(item_row->ColumnText->Strings[gender_column->text_index]); \
            if (gender_text == "1") \
                hintSubString->Caption = item_type + " (m)"; \
            else \
                hintSubString->Caption = item_type + " (f)"; \
        } \
    } \
    ShowItemGraphic(hintImage, hintImagePanel, StrToInt(gfx_text)); \
    hintPanel->Left = hint_x; \
    hintPanel->Top = hint_y; \
    hintPanel->Visible = true; \
}

// Deletes the selected row, notes the deletion for the save warning, and renumbers the rows.
// Shared body: a class defines its popupDeleteClick with DEFINE_POPUP_DELETE_CLICK(TheClass); each class has its own
// dropGrid, rows_deleted, Renumber, ShowCount.
#define DEFINE_POPUP_DELETE_CLICK(Class) \
void __fastcall Class::popupDeleteClick(TObject *Sender) \
{ \
    if (dropGrid->Selected != NULL) \
    { \
        if (dropGrid->Items->Count > 0) \
            rows_deleted = true; \
        dropGrid->IsUpdating = true; \
        dropGrid->Selected->Delete(); \
        Renumber(); \
        dropGrid->IsUpdating = false; \
        ShowCount(); \
    } \
}

// Swaps the selected row's cells with the previous row's.
// Shared body: a class defines its popupMoveUpClick with DEFINE_POPUP_MOVE_UP_CLICK(TheClass); each class has its own
// dropGrid, hintPanel, hint_row.
#define DEFINE_POPUP_MOVE_UP_CLICK(Class) \
void __fastcall Class::popupMoveUpClick(TObject *Sender) \
{ \
    TElWideStrings *previous_cells; \
    if (dropGrid->Selected == NULL) \
        return; \
    hintPanel->Visible = false; \
    hint_row = -2; \
    int row_index = dropGrid->Selected->Index; \
    if (row_index < 1) \
        return; \
    for (int column = 0; column < dropGrid->HeaderSections->Count - 1; column++) \
    { \
        String current_text(dropGrid->Items->Item[row_index]->ColumnText->Strings[column]); \
        String previous_text(dropGrid->Items->Item[row_index - 1]->ColumnText->Strings[column]); \
        TElWideStrings *current_cells = dropGrid->Items->Item[row_index]->ColumnText; \
        current_cells->Strings[column] = WideString(previous_text); \
        previous_cells = dropGrid->Items->Item[row_index - 1]->ColumnText; \
        previous_cells->Strings[column] = WideString(current_text); \
    } \
}

// Swaps the selected row's cells with the next row's.
// Shared body: a class defines its popupMoveDownClick with DEFINE_POPUP_MOVE_DOWN_CLICK(TheClass); each class has its
// own dropGrid, hintPanel, hint_row.
#define DEFINE_POPUP_MOVE_DOWN_CLICK(Class) \
void __fastcall Class::popupMoveDownClick(TObject *Sender) \
{ \
    TElWideStrings *next_cells; \
    if (dropGrid->Selected == NULL) \
        return; \
    hintPanel->Visible = false; \
    hint_row = -2; \
    int row_index = dropGrid->Selected->Index; \
    if (dropGrid->Items->Count <= row_index + 1) \
        return; \
    for (int column = 0; column < dropGrid->HeaderSections->Count - 1; column++) \
    { \
        String current_text(dropGrid->Items->Item[row_index]->ColumnText->Strings[column]); \
        String next_text(dropGrid->Items->Item[row_index + 1]->ColumnText->Strings[column]); \
        TElWideStrings *current_cells = dropGrid->Items->Item[row_index]->ColumnText; \
        current_cells->Strings[column] = WideString(next_text); \
        next_cells = dropGrid->Items->Item[row_index + 1]->ColumnText; \
        next_cells->Strings[column] = WideString(current_text); \
    } \
}

// Shared body: a class defines its dropGridEnter with DEFINE_DROP_GRID_ENTER(TheClass); each class has its own
// dropGrid.
#define DEFINE_DROP_GRID_ENTER(Class) \
void __fastcall Class::dropGridEnter(TObject *Sender) \
{ \
    TElTreeStringGrid *grid = dropGrid; \
    if (grid->Selected == NULL && grid->Items->Count > 0) \
    { \
        grid->Items->Item[0]->Selected = true; \
        grid->Items->Item[0]->Focused = true; \
    } \
}

// Ctrl+Up and Ctrl+Down move the selected row; Escape closes the form unless a cell is being edited.
// Shared body: a class defines its dropGridKeyDown with DEFINE_DROP_GRID_KEY_DOWN(TheClass); each class has its own
// popupMoveUpClick, popupMoveDownClick, dropGrid.
#define DEFINE_DROP_GRID_KEY_DOWN(Class) \
void __fastcall Class::dropGridKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift) \
{ \
    if (Shift.Contains(ssCtrl)) \
    { \
        if (Key == VK_UP) \
            popupMoveUpClick((TObject *)this); \
        if (Key == VK_DOWN) \
            popupMoveDownClick((TObject *)this); \
    } \
    if (Key == VK_ESCAPE && !((TCustomElTree *)dropGrid)->IsEditing()) \
        ((TCustomForm *)this)->Close(); \
}

// Centres the form on the owner, fills the list from the items tab and shows it modally.
// Shared body: a class defines its Open with DEFINE_OPEN(TheClass); each class has its own selected_id, hint_row,
// view_caption, searchEdit, ShowView, FillGrid, AlignColumns, Timer1.
#define DEFINE_OPEN(Class) \
void Class::Open(TForm *owner, String caption, int kind) \
{ \
    if (caption.Length() > 0) \
        Caption = "  " + caption; \
    Left = (owner->Width - Width) / 2 + owner->Left; \
    Top = (owner->Height - Height) / 2 + owner->Top; \
    Top = Top + 10; \
    selected_id = kind; \
    hint_row = -1; \
    view_caption = ""; \
    searchEdit->Text = view_caption; \
    ShowView(view_caption, searchEdit->Text); \
    FillGrid(); \
    AlignColumns(); \
    Timer1->Enabled = true; \
    ShowModal(); \
}

// Keeps the selected item's ID and name and closes the form.
// Shared body: a class defines its buttonOkClick with DEFINE_FORM_BUTTON_OK_CLICK(TheClass); each class has its own
// itemGrid, selected_id, selected_name.
#define DEFINE_FORM_BUTTON_OK_CLICK(Class) \
void __fastcall Class::buttonOkClick(TObject *Sender) \
{ \
    if (itemGrid->Selected == NULL) \
        return; \
    selected_id = StrToInt(itemGrid->Selected->Text); \
    selected_name = itemGrid->Selected->ColumnText->Strings[0]; \
    ModalResult = mrOk; \
}

// Escape closes the form and Enter takes the selected item, once the opening pause is over.
// Shared body: a class defines its buttonOkKeyUp with DEFINE_BUTTON_OK_KEY_UP(TheClass); each class has its own Timer1,
// itemGrid, buttonOkClick.
#define DEFINE_BUTTON_OK_KEY_UP(Class) \
void __fastcall Class::buttonOkKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift) \
{ \
    if (Timer1->Enabled) \
        Timer1->Enabled = false; \
    else \
    { \
        if (Key == VK_ESCAPE) \
            ((TCustomForm *)this)->Close(); \
        if (Key == VK_RETURN && itemGrid->Selected != NULL) \
            buttonOkClick((TObject *)this); \
    } \
}

// Shared body: a class defines its FillCombos with DEFINE_FILL_COMBOS(TheClass); each class has its own combos_filled,
// FillComboFromGrid, comboEditClass.
#define DEFINE_FILL_COMBOS(Class) \
void Class::FillCombos() \
{ \
    if (combos_filled) \
        return; \
    FillComboFromGrid(comboEditClass, FormMain->ElTreeStringGrid3, "Name"); \
    combos_filled = true; \
}

// Refills the combo with a dash and then the column's text from every row of the grid.
// Shared body: a class defines its FillComboFromGrid with DEFINE_FILL_COMBO_FROM_GRID(TheClass).
#define DEFINE_FILL_COMBO_FROM_GRID(Class) \
void Class::FillComboFromGrid(TElComboBox *combo, TElTreeStringGrid *grid, String column_name) \
{ \
    DataGridCol *column = FormMain->classes_tab->grid_ui->data_grid->FindColumn(column_name); \
    if (!column) \
        return; \
    combo->Items->Clear(); \
    TElWideStrings *combo_items = combo->Items; \
    int blank_index = combo_items->Add(WideString("-")); \
    for (int i = 0; i < grid->Items->Count; i++) \
    { \
        DataComboItem *combo_item = column->GetComboItem(i); \
        combo->Items->Add(grid->Items->Item[i]->ColumnText->Strings[column->text_index]); \
    } \
}

// Drops the number's leading zeros and caps it at the most the box allows.
// Shared body: a class defines its spinEditMinExit with DEFINE_SPIN_EDIT_MIN_EXIT(TheClass).
#define DEFINE_SPIN_EDIT_MIN_EXIT(Class) \
void __fastcall Class::spinEditMinExit(TObject *Sender) \
{ \
    ((TElSpinEdit *)Sender)->Text = WideString(FormMain->TrimLeadingZeros(((TElSpinEdit *)Sender)->Text)); \
    if (((TElSpinEdit *)Sender)->Tag < ((TElSpinEdit *)Sender)->Value) \
        ((TElSpinEdit *)Sender)->Value = ((TElSpinEdit *)Sender)->Tag; \
}

// Escape restores the combo's text from entry; typing drops the list open and [ closes it.
// Shared body: a class defines its comboEditClassKeyDown with DEFINE_COMBO_EDIT_CLASS_KEY_DOWN(TheClass).
#define DEFINE_COMBO_EDIT_CLASS_KEY_DOWN(Class) \
void __fastcall Class::comboEditClassKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift) \
{ \
    String text(((TElComboBox *)Sender)->Text); \
    if (Key == VK_ESCAPE) \
    { \
        if (((TElComboBox *)Sender)->Text != FormMain->combo_text) \
        { \
            ((TElComboBox *)Sender)->Text = WideString(FormMain->combo_text); \
            ((TElComboBox *)Sender)->SelectAll(); \
            if (!((TElComboBox *)Sender)->DroppedDown) \
                ((TElComboBox *)Sender)->DropDown(); \
        } \
    } \
    if (text.Length() < 1) \
        return; \
    if (((TElComboBox *)Sender)->DroppedDown) \
    { \
        if (Key == VK_OEM_4) \
            ((TElComboBox *)Sender)->CloseUp(false); \
    } \
    else \
        ((TElComboBox *)Sender)->DropDown(); \
    if (!Key && !((TElComboBox *)Sender)->DroppedDown) \
        ((TElComboBox *)Sender)->DropDown(); \
}

// Applies the theme and its shop picture, clears the quick-save cell and selects the first row.
// Shared body: a class defines its FormShow with DEFINE_T_FORM_SHOP_EDIT_ITEMS_FORM_SHOW(TheClass); each class has its
// own imageOperation, imageShop, dropGrid, Panel1.
#define DEFINE_T_FORM_SHOP_EDIT_ITEMS_FORM_SHOW(Class) \
void __fastcall Class::FormShow(TObject *Sender) \
{ \
    if (FormMain->config->GetTheme() == "dark") \
    { \
        FormMain->ThemeComponents(this); \
        imageOperation->Picture->Bitmap = FormMain->imageDarkArrow->Picture->Bitmap; \
        Color = (TColor)0x525252; \
        imageShop->Picture->Bitmap = FormMain->imageShopDark->Picture->Bitmap; \
    } \
    else \
        imageShop->Picture->Bitmap = FormMain->imageShopLight->Picture->Bitmap; \
    FormMain->combo_text = ""; \
    FormMain->edit_backup = ""; \
    FormMain->edit_grid = 0; \
    FormMain->edit_row = 0; \
    if ((char)((int __fastcall (*)(int))FindDynaInst((void *)dropGrid, -68))((int)dropGrid) && \
        dropGrid->Items->Count > 0) \
    { \
        dropGrid->Items->Item[0]->Selected = true; \
        dropGrid->Items->Item[0]->Focused = true; \
        dropGrid->Selected = dropGrid->Items->Item[0]; \
        Panel1->SetFocus(); \
        dropGrid->SetFocus(); \
    } \
}

// Fills the item name for a typed ID; refuses an ID with no item.
// Shared body: a class defines its ElTreeInplaceSpinEdit1AfterOperation with
// DEFINE_EL_TREE_INPLACE_SPIN_EDIT1_AFTER_OPERATION(TheClass); each class has its own dropGrid, edit_column,
// FindItemRow, hint_row, hintPanel.
#define DEFINE_EL_TREE_INPLACE_SPIN_EDIT1_AFTER_OPERATION(Class) \
void __fastcall Class::ElTreeInplaceSpinEdit1AfterOperation(TObject *Sender, bool &Accepted, bool &DefaultConversion) \
{ \
    if (((TElTreeInplaceSpinEdit *)Sender)->Editor->Text.Length() <= 0) \
        ((TElTreeInplaceSpinEdit *)Sender)->Editor->Value = 0; \
    if (((TElTreeInplaceSpinEdit *)Sender)->Editor->Tag > 0) \
    { \
        if (((TElTreeInplaceSpinEdit *)Sender)->Editor->Tag < ((TElTreeInplaceSpinEdit *)Sender)->Editor->Value) \
            ((TElTreeInplaceSpinEdit *)Sender)->Editor->Value = ((TElTreeInplaceSpinEdit *)Sender)->Editor->Tag; \
    } \
    if (FormMain->config->GetMouseQuickSave()) \
        Accepted = true; \
    if (dropGrid->Selected == NULL) \
        Accepted = false; \
    if (Accepted) \
    { \
        if (edit_column == 1) \
        { \
            TElTreeItem *item_row = FindItemRow((int)((TElTreeInplaceSpinEdit *)Sender)->Editor->Value); \
            if (!item_row) \
            { \
                Accepted = false; \
                return; \
            } \
            DataGridCol *name_grid_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Name"); \
            dropGrid->Selected->ColumnText->Strings[1] = item_row->ColumnText->Strings[name_grid_column->text_index]; \
            hint_row = -1; \
            hintPanel->Visible = false; \
        } \
    } \
}

// Blanks the zero cells of the changed row when zeros are hidden.
// Shared body: a class defines its dropGridItemChange with DEFINE_DROP_GRID_ITEM_CHANGE(TheClass).
#define DEFINE_DROP_GRID_ITEM_CHANGE(Class) \
void __fastcall Class::dropGridItemChange(TObject *Sender, TElTreeItem *Item, TItemChangeMode ItemChangeMode) \
{ \
    for (int column = 0; column < Item->ColumnText->Count; column++) \
    { \
        if (String(Item->ColumnText->Strings[column]) == "0" && !FormMain->config->GetShowZeros()) \
        { \
            TElWideStrings *cells = Item->ColumnText; \
            cells->Strings[column] = WideString(""); \
        } \
    } \
}

// Shared body: a class defines its FindSkillRow with DEFINE_FIND_SKILL_ROW(TheClass).
#define DEFINE_FIND_SKILL_ROW(Class) \
TElTreeItem *Class::FindSkillRow(String skill_id) \
{ \
    TElTreeStringGrid *skill_grid = FormMain->skills_tab->main_grid; \
    for (int i = 0; i < skill_grid->Items->Count; i++) \
    { \
        TElTreeItem *row = skill_grid->Items->Item[i]; \
        if (row->Text == skill_id) \
            return row; \
    } \
    return 0; \
}

// Shows the item graphic in the image, gold for 35, as large as the panel and the kind allow.
// Shared body: a class defines its ShowIcon with DEFINE_SHOW_ICON(TheClass).
#define DEFINE_SHOW_ICON(Class) \
void Class::ShowIcon(TImage *image, TPanel *panel, unsigned char kind, int id) \
{ \
    HINSTANCE__ *library; \
    int image_top; \
    TImage *shown_image = image; \
    int resource_step = 2; \
    int first_resource = 101; \
    TPanel *image_panel = panel; \
    library = FormMain->gfx_vault->items_library; \
    if (id < 1) \
        id = 1; \
    int resource_id = first_resource + (id - 1) * resource_step; \
    shown_image->Picture->Bitmap->TransparentColor = clBlack; \
    shown_image->Picture->Bitmap->Transparent = true; \
    try \
    { \
        if (id == 35) \
            shown_image->Picture->Bitmap = FormMain->imageGold->Picture->Bitmap; \
        else \
            shown_image->Picture->Bitmap->LoadFromResourceID((unsigned int)library, resource_id); \
    } \
    catch (...) \
    { \
        if (image_panel) \
            image_panel->Caption = "(n/a)"; \
        shown_image->Visible = false; \
        return; \
    } \
    shown_image->Visible = true; \
    if (3 * shown_image->Picture->Bitmap->Width < image_panel->Width && \
        3 * shown_image->Picture->Bitmap->Height < image_panel->Height && kind > 2) \
    { \
        shown_image->Width = 3 * shown_image->Picture->Bitmap->Width; \
        shown_image->Height = 3 * shown_image->Picture->Bitmap->Height; \
    } \
    else if (2 * shown_image->Picture->Bitmap->Width < image_panel->Width && \
        2 * shown_image->Picture->Bitmap->Height < image_panel->Height && kind > 1) \
    { \
        shown_image->Width = 2 * shown_image->Picture->Bitmap->Width; \
        shown_image->Height = 2 * shown_image->Picture->Bitmap->Height; \
    } \
    else if (image_panel->Width > shown_image->Picture->Bitmap->Width * 1.5 && \
        image_panel->Height > shown_image->Picture->Bitmap->Height * 1.5 && kind > 0) \
    { \
        shown_image->Width = (int)(shown_image->Picture->Bitmap->Width * 1.5); \
        shown_image->Height = (int)(shown_image->Picture->Bitmap->Height * 1.5); \
    } \
    else if (image_panel->Width > shown_image->Picture->Bitmap->Width * 1.2 && \
        image_panel->Height > shown_image->Picture->Bitmap->Height * 1.2 && kind > 0) \
    { \
        shown_image->Width = (int)(shown_image->Picture->Bitmap->Width * 1.2); \
        shown_image->Height = (int)(shown_image->Picture->Bitmap->Height * 1.2); \
    } \
    else \
    { \
        shown_image->Width = shown_image->Picture->Bitmap->Width; \
        shown_image->Height = shown_image->Picture->Bitmap->Height; \
    } \
    if (image_panel) \
    { \
        image_panel->Caption = ""; \
        shown_image->Left = (image_panel->Width - shown_image->Width) / 2; \
        shown_image->Top = (image_panel->Height - shown_image->Height) / 2; \
        image_top = shown_image->Top; \
        if (image_top < 0) \
            shown_image->Top = image_top / 3; \
    } \
}

// Delete removes the selected row and Ctrl+N adds a new one.
// Shared body: a class defines its dropGridKeyUp with DEFINE_DROP_GRID_KEY_UP(TheClass); each class has its own
// popupDeleteClick, buttonNewShopCraftClick.
#define DEFINE_DROP_GRID_KEY_UP(Class) \
void __fastcall Class::dropGridKeyUp(TObject *Sender, unsigned short &Key, TShiftState Shift) \
{ \
    if (Key == VK_DELETE) \
        popupDeleteClick((TObject *)this); \
    if (Shift.Contains(ssCtrl) && Key == 'N') \
        buttonNewShopCraftClick((TObject *)this); \
}

// Clears every craft once the warning is confirmed.
// Shared body: a class defines its buttonCleanListClick with DEFINE_BUTTON_CLEAN_LIST_CLICK(TheClass); each class has
// its own dropGrid, ShowCount.
#define DEFINE_BUTTON_CLEAN_LIST_CLICK(Class) \
void __fastcall Class::buttonCleanListClick(TObject *Sender) \
{ \
    String title = "Clear Craft List"; \
    String message = "The entire shop list will be cleared, are you sure?"; \
    String button_text = "CLEAR LIST"; \
    FormDeleteWarning->PlaceWarning(this, Width, title, message, button_text); \
    if (FormDeleteWarning->ShowModal() == mrOk) \
    { \
        dropGrid->IsUpdating = true; \
        dropGrid->Items->Clear(); \
        dropGrid->IsUpdating = false; \
    } \
    ShowCount(); \
}

// Lines up the operation label, arrow, name and ID captions, and sets the form and save button captions.
// Shared body: a class defines its LayoutCaptions with DEFINE_FORM_LAYOUT_CAPTIONS(TheClass); each class has its own
// itemNameOriginalID, itemNameOriginal, labelOperation, imageOperation.
#define DEFINE_FORM_LAYOUT_CAPTIONS(Class) \
void Class::LayoutCaptions() \
{ \
    itemNameOriginalID->Left = itemNameOriginal->Left + itemNameOriginal->Width + 14; \
    Caption = " " + labelOperation->Caption + " : " + itemNameOriginal->Caption; \
    imageOperation->Left = labelOperation->Left + labelOperation->Width + 8; \
    itemNameOriginal->Left = imageOperation->Left + imageOperation->Width + 8; \
    itemNameOriginalID->Left = itemNameOriginal->Left + itemNameOriginal->Width + 8; \
}

#endif
