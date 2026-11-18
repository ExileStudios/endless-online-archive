#ifndef ConfigcheckH
#define ConfigcheckH

// Checks the gfx, pub, export and backup folders and remembers which files are missing.
struct ConfigCheck
{
    bool gfx_folder_found;
    String gfx023_path;
    bool gfx023_found;
    String gfx021_path;
    bool gfx021_found;
    String gfx025_path;
    bool gfx025_found;
    String gfx024_path;
    bool gfx024_found;
    bool pub_folder_found;
    String item_pub_path;
    bool item_pub_found;
    String class_pub_path;
    bool class_pub_found;
    String npc_pub_path;
    bool npc_pub_found;
    String skill_pub_path;
    bool skill_pub_found;
    bool export_folder_found;
    bool backup_folder_found;

    ConfigCheck();
    ~ConfigCheck();
    bool CheckGfxPath(String folder);
    char CheckPubPath(String folder);
    char CheckExportPath(String folder);
    char CheckBackupPath(String folder);
    bool PathExists(String path);
    bool GetGfxFolderFound();
    bool GetGfx023Found();
    String GetGfx023Path();
    bool GetGfx021Found();
    String GetGfx021Path();
    bool GetGfx025Found();
    String GetGfx025Path();
    bool GetGfx024Found();
    String GetGfx024Path();
    bool GetPubFolderFound();
    bool GetItemPubFound();
    String GetItemPubPath();
    bool GetClassPubFound();
    String GetClassPubPath();
    bool GetNpcPubFound();
    String GetNpcPubPath();
    bool GetSkillPubFound();
    String GetSkillPubPath();
};

#endif
