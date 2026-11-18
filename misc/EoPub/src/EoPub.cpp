#include <vcl.h>
#pragma hdrstop

#include <Forms.hpp>

USEFORM("UnitMain.cpp", FormMain);
USEFORM("UnitAbout.cpp", FormAbout);
USEFORM("UnitGfx.cpp", FormGfx);
USEFORM("UnitEditItem.cpp", FormItemEdit);
USEFORM("UnitPathConfig.cpp", FormPathConfig);
USEFORM("UnitEditClass.cpp", FormEditClass);
USEFORM("UnitConfiguration.cpp", FormConfiguration);
USEFORM("UnitExportForm.cpp", FormExport);
USEFORM("UnitExportResultForm.cpp", FormExportResult);
USEFORM("UnitWarning.cpp", FormWarning);
USEFORM("UnitWarningReload.cpp", FormReloadWarning);
USEFORM("UnitWarningClsCorruption.cpp", FormClassCorruption);
USEFORM("UnitWarningOutOfBound.cpp", FormWarningOutOfBound);
USEFORM("UnitWarningExit.cpp", FormWarningExit);
USEFORM("UnitToolsItems.cpp", FormToolsItems);
USEFORM("UnitWarningDelete.cpp", FormDeleteWarning);
USEFORM("UnitEditNpc.cpp", FormNpcEdit);
USEFORM("UnitEditSkill.cpp", FormSkillEdit);
USEFORM("UnitPubfileStatus.cpp", FormPubstatus);
USEFORM("UnitWarningExpertMode.cpp", FormWarningExpertMode);
USEFORM("UnitFolderSelect.cpp", FormFolderSelect);
USEFORM("UnitNewFolder.cpp", FormNewFolder);
USEFORM("UnitPathScan.cpp", FormPathScan);
USEFORM("UnitToolsNpcs.cpp", FormToolsNpcs);
USEFORM("UnitToolsSkills.cpp", FormToolsSkills);
USEFORM("UnitToolsClasses.cpp", FormToolsClasses);
USEFORM("UnitWarningCannotOpen.cpp", FormWarningCannotOpen);
USEFORM("UnitEditNpcChats.cpp", FormNpcEditChats);
USEFORM("UnitEditNpcDrops.cpp", FormNpcEditDrops);
USEFORM("UnitSelectItem.cpp", FormSelectItem);
USEFORM("UnitEditShop.cpp", FormEditShop);
USEFORM("UnitEditTrainer.cpp", FormEditTrainer);
USEFORM("UnitEditShopItems.cpp", FormShopEditItems);
USEFORM("UnitSelectSkill.cpp", FormSelectSkill);
USEFORM("UnitEditTrainerSkills.cpp", FormTrainerEditSkills);
USEFORM("UnitEditShopCrafts.cpp", FormShopEditCrafts);
USEFORM("UnitEditShopCraftsReq.cpp", FormShopEditCraftsReq);
USEFORM("UnitEditTrainerSkillsReq.cpp", FormTrainerEditSkillsReq);
USEFORM("UnitEditTrainerSkillsStats.cpp", FormTrainerEditSkillsStats);
USEFORM("UnitToolsShops.cpp", FormToolsShops);
USEFORM("UnitToolsTrainers.cpp", FormToolsTrainers);
USEFORM("UnitEditInn.cpp", FormEditInn);
USEFORM("UnitEditInnQuiz.cpp", FormEditInnQuiz);
USEFORM("UnitToolsInns.cpp", FormToolsInns);

// Creates the editor's windows and runs it; an exception is shown in a message box.
int WINAPI WinMain(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int n_cmd_show)
{
    try
    {
        Application->Initialize();
        Application->Title = "Endless Pub Editor";
        Application->CreateForm(__classid(TFormMain), &FormMain);
        Application->CreateForm(__classid(TFormAbout), &FormAbout);
        Application->CreateForm(__classid(TFormGfx), &FormGfx);
        Application->CreateForm(__classid(TFormItemEdit), &FormItemEdit);
        Application->CreateForm(__classid(TFormPathConfig), &FormPathConfig);
        Application->CreateForm(__classid(TFormEditClass), &FormEditClass);
        Application->CreateForm(__classid(TFormConfiguration), &FormConfiguration);
        Application->CreateForm(__classid(TFormExport), &FormExport);
        Application->CreateForm(__classid(TFormExportResult), &FormExportResult);
        Application->CreateForm(__classid(TFormWarning), &FormWarning);
        Application->CreateForm(__classid(TFormReloadWarning), &FormReloadWarning);
        Application->CreateForm(__classid(TFormClassCorruption), &FormClassCorruption);
        Application->CreateForm(__classid(TFormWarningOutOfBound), &FormWarningOutOfBound);
        Application->CreateForm(__classid(TFormWarningExit), &FormWarningExit);
        Application->CreateForm(__classid(TFormToolsItems), &FormToolsItems);
        Application->CreateForm(__classid(TFormDeleteWarning), &FormDeleteWarning);
        Application->CreateForm(__classid(TFormNpcEdit), &FormNpcEdit);
        Application->CreateForm(__classid(TFormSkillEdit), &FormSkillEdit);
        Application->CreateForm(__classid(TFormPubstatus), &FormPubstatus);
        Application->CreateForm(__classid(TFormWarningExpertMode), &FormWarningExpertMode);
        Application->CreateForm(__classid(TFormFolderSelect), &FormFolderSelect);
        Application->CreateForm(__classid(TFormNewFolder), &FormNewFolder);
        Application->CreateForm(__classid(TFormPathScan), &FormPathScan);
        Application->CreateForm(__classid(TFormToolsNpcs), &FormToolsNpcs);
        Application->CreateForm(__classid(TFormToolsSkills), &FormToolsSkills);
        Application->CreateForm(__classid(TFormToolsClasses), &FormToolsClasses);
        Application->CreateForm(__classid(TFormWarningCannotOpen), &FormWarningCannotOpen);
        Application->CreateForm(__classid(TFormNpcEditChats), &FormNpcEditChats);
        Application->CreateForm(__classid(TFormNpcEditDrops), &FormNpcEditDrops);
        Application->CreateForm(__classid(TFormSelectItem), &FormSelectItem);
        Application->CreateForm(__classid(TFormEditShop), &FormEditShop);
        Application->CreateForm(__classid(TFormEditTrainer), &FormEditTrainer);
        Application->CreateForm(__classid(TFormShopEditItems), &FormShopEditItems);
        Application->CreateForm(__classid(TFormSelectSkill), &FormSelectSkill);
        Application->CreateForm(__classid(TFormTrainerEditSkills), &FormTrainerEditSkills);
        Application->CreateForm(__classid(TFormShopEditCrafts), &FormShopEditCrafts);
        Application->CreateForm(__classid(TFormShopEditCraftsReq), &FormShopEditCraftsReq);
        Application->CreateForm(__classid(TFormTrainerEditSkillsReq), &FormTrainerEditSkillsReq);
        Application->CreateForm(__classid(TFormTrainerEditSkillsStats), &FormTrainerEditSkillsStats);
        Application->CreateForm(__classid(TFormToolsShops), &FormToolsShops);
        Application->CreateForm(__classid(TFormToolsTrainers), &FormToolsTrainers);
        Application->CreateForm(__classid(TFormEditInn), &FormEditInn);
        Application->CreateForm(__classid(TFormEditInnQuiz), &FormEditInnQuiz);
        Application->CreateForm(__classid(TFormToolsInns), &FormToolsInns);
        Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    return 0;
}
