#include <vcl.h>
#pragma hdrstop

#include "Configcheck.h"
#include <io.h>

#pragma package(smart_init)

ConfigCheck::ConfigCheck()
{
}

ConfigCheck::~ConfigCheck()
{
}

// Checks that the folder and its four gfx files exist; true only when all of them are there.
bool ConfigCheck::CheckGfxPath(String folder)
{
    gfx_folder_found = false;
    gfx021_found = false;
    gfx023_found = false;
    gfx025_found = false;
    gfx024_found = false;
    gfx021_path = folder + "gfx021.egf";
    gfx023_path = folder + "gfx023.egf";
    gfx024_path = folder + "gfx024.egf";
    gfx025_path = folder + "gfx025.egf";
    if (!access(folder.c_str(), 0))
        gfx_folder_found = true;
    if (!access(gfx021_path.c_str(), 0))
        gfx021_found = true;
    if (!access(gfx023_path.c_str(), 0))
        gfx023_found = true;
    if (!access(gfx025_path.c_str(), 0))
        gfx025_found = true;
    if (!access(gfx024_path.c_str(), 0))
        gfx024_found = true;
    if (!gfx_folder_found || !gfx023_found || !gfx021_found || !gfx025_found || !gfx024_found)
        return false;
    return true;
}

// Checks that the folder and the item, class, npc and skill pub files exist; true only when all are there.
char ConfigCheck::CheckPubPath(String folder)
{
    pub_folder_found = false;
    item_pub_found = false;
    class_pub_found = false;
    npc_pub_found = false;
    skill_pub_found = false;
    item_pub_path = folder + "dat001.eif";
    class_pub_path = folder + "dat001.ecf";
    npc_pub_path = folder + "dtn001.enf";
    skill_pub_path = folder + "dsl001.esf";
    if (!access(folder.c_str(), 0))
        pub_folder_found = true;
    if (!access(item_pub_path.c_str(), 0))
        item_pub_found = true;
    if (!access(class_pub_path.c_str(), 0))
        class_pub_found = true;
    if (!access(npc_pub_path.c_str(), 0))
        npc_pub_found = true;
    if (!access(skill_pub_path.c_str(), 0))
        skill_pub_found = true;
    if (!pub_folder_found || !item_pub_found || !class_pub_found || !npc_pub_found || !skill_pub_found)
        return false;
    return true;
}

// Whether the export folder exists, kept for the status icons.
char ConfigCheck::CheckExportPath(String folder)
{
    export_folder_found = false;
    if (!access(folder.c_str(), 0))
        export_folder_found = true;
    else
        return false;
    return true;
}

// Whether the backup folder exists, kept for the status icons.
char ConfigCheck::CheckBackupPath(String folder)
{
    backup_folder_found = false;
    if (!access(folder.c_str(), 0))
        backup_folder_found = true;
    else
        return false;
    return true;
}

bool ConfigCheck::PathExists(String path)
{
    if (!access(path.c_str(), 0))
        return true;
    return false;
}

bool ConfigCheck::GetGfxFolderFound()
{
    return gfx_folder_found;
}

bool ConfigCheck::GetGfx023Found()
{
    return gfx023_found;
}

String ConfigCheck::GetGfx023Path()
{
    return gfx023_path;
}

bool ConfigCheck::GetGfx021Found()
{
    return gfx021_found;
}

String ConfigCheck::GetGfx021Path()
{
    return gfx021_path;
}

bool ConfigCheck::GetGfx025Found()
{
    return gfx025_found;
}

String ConfigCheck::GetGfx025Path()
{
    return gfx025_path;
}

bool ConfigCheck::GetGfx024Found()
{
    return gfx024_found;
}

String ConfigCheck::GetGfx024Path()
{
    return gfx024_path;
}

bool ConfigCheck::GetPubFolderFound()
{
    return pub_folder_found;
}

bool ConfigCheck::GetItemPubFound()
{
    return item_pub_found;
}

String ConfigCheck::GetItemPubPath()
{
    return item_pub_path;
}

bool ConfigCheck::GetClassPubFound()
{
    return class_pub_found;
}

String ConfigCheck::GetClassPubPath()
{
    return class_pub_path;
}

bool ConfigCheck::GetNpcPubFound()
{
    return npc_pub_found;
}

String ConfigCheck::GetNpcPubPath()
{
    return npc_pub_path;
}

bool ConfigCheck::GetSkillPubFound()
{
    return skill_pub_found;
}

String ConfigCheck::GetSkillPubPath()
{
    return skill_pub_path;
}
