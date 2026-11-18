#include <vcl.h>
#pragma hdrstop

#include "UnitMain.h"
#include "UnitFileExist.h"
#include "UnitFileNotFound.h"
#include <Filectrl.hpp>
#include <io.h>
#include <stdlib.h>

#pragma package(smart_init)
#pragma resource "*.dfm"

TMainForm *MainForm;

__fastcall TMainForm::TMainForm(TComponent *owner)
    : TForm(owner)
{
    TComponent *form_owner;
    pub_version = 2;
    dry_run = false;
}

// Creates the settings and the four lists, names the five pub files, then finds the import and output folders.
void __fastcall TMainForm::FormShow(TObject *Sender)
{
    config = new Config();
    config->LoadSettings();
    npcs = new NpcValues(String(""));
    shops = new ShopValues(String(""));
    trainers = new LearnValues(String(""));
    inns = new InnValues(String(""));
    drops_pub = "serv_drops.epf";
    chats_pub = "serv_chats.epf";
    shops_pub = "serv_shops.epf";
    trainers_pub = "serv_trainers.epf";
    inns_pub = "serv_inns.epf";
    if (PageControl1->ActivePageIndex != 1)
        PageControl1->ActivePageIndex = 1;
    read_path = ".";
    FindImportFolder();
    FindOutputFolder();
    PageControl1->ActivePageIndex = 0;
}

// Looks for the .ini files in data, then pub, then the program's folder; returns the folder found in.
String TMainForm::FindImportFolder()
{
    read_path = ".\\data\\";
    String folder = ".\\data\\";
    ScanImportFolder(folder);
    if (!files_found)
    {
        folder = ".\\pub\\";
        ScanImportFolder(folder);
    }
    if (!files_found)
    {
        folder = ".\\";
        ScanImportFolder(folder);
    }
    return read_path;
}

// Picks the output folder: the export path of the settings, else data\pub, else pub, the first that exists.
String TMainForm::FindOutputFolder()
{
    write_path = ".\\";
    write_path_caption = "[ current folder ]";
    String folder = config->GetExportPath();
    if (PathExists(folder))
    {
        write_path_caption = folder;
        write_path = AddBackslash(folder);
        UpdateWritePath();
        return write_path;
    }
    folder = ".\\data\\pub\\";
    if (PathExists(folder))
    {
        write_path_caption = folder;
        write_path = AddBackslash(folder);
        UpdateWritePath();
        return write_path;
    }
    folder = ".\\pub\\";
    if (PathExists(folder))
    {
        write_path_caption = folder;
        write_path = AddBackslash(folder);
        UpdateWritePath();
        return write_path;
    }
}

// Finds each .ini file the importer reads in the folder and reads the ones found into the lists.
void TMainForm::ScanImportFolder(String folder)
{
    files_found = false;
    drops_found = false;
    chats_found = false;
    shops_found = false;
    trainers_found = false;
    read_path_caption = "click here to browse import (.ini files) folder";
    if (folder[folder.Length()] != '\\')
        folder = folder + '\\';
    String ini_path = FindIniFile(folder, "drops.ini");
    if (ini_path.Length() > 0)
    {
        files_found = true;
        drops_found = true;
        drops_ini = ini_path;
    }
    ini_path = FindIniFile(folder, "chats.ini");
    if (ini_path.Length() > 0)
    {
        files_found = true;
        chats_found = true;
        chats_ini = ini_path;
    }
    if (!chats_found)
    {
        ini_path = FindIniFile(folder, "npcs.ini");
        if (ini_path.Length() > 0)
        {
            files_found = true;
            chats_found = true;
            chats_ini = ini_path;
        }
    }
    if (!chats_found)
    {
        ini_path = FindIniFile(folder, "npc_chat.ini");
        if (ini_path.Length() > 0)
        {
            files_found = true;
            chats_found = true;
            chats_ini = ini_path;
        }
    }
    ini_path = FindIniFile(folder, "shops.ini");
    if (ini_path.Length() > 0)
    {
        files_found = true;
        shops_found = true;
        shops_ini = ini_path;
    }
    ini_path = FindIniFile(folder, "skills.ini");
    if (ini_path.Length() > 0)
    {
        files_found = true;
        trainers_found = true;
        trainers_ini = ini_path;
    }
    ini_path = FindIniFile(folder, "home.ini");
    if (ini_path.Length() > 0)
    {
        files_found = true;
        inns_found = true;
        inns_ini = ini_path;
    }
    // inn.ini stands in for a missing home.ini.
    if (!inns_found)
    {
        ini_path = FindIniFile(folder, "inn.ini");
        if (ini_path.Length() > 0)
        {
            files_found = true;
            inns_found = true;
            inns_ini = ini_path;
        }
    }
    if (files_found)
    {
        read_path = folder;
        read_path_caption = folder;
        npcs->Clear();
        npcs->ClearHeaders();
        if (drops_found)
            drops_count = npcs->ImportIni(NpcIniFormat_Drops, drops_ini);
        if (chats_found)
            chats_count = npcs->ImportIni(NpcIniFormat_Chats, chats_ini);
        if (shops_found)
            shops_count = shops->ImportIni(0, shops_ini);
        if (trainers_found)
            trainers_count = trainers->ImportIni(0, trainers_ini);
        if (inns_found)
            inns_count = inns->ImportIni(0, inns_ini);
    }
    UpdateReadPage();
    FillExportGrid();
}

// The path of the file in the folder or in its data or extras folder; empty when none has it.
String TMainForm::FindIniFile(String folder, String file_name)
{
    String path = folder + file_name;
    if (PathExists(path))
        return path;
    path = folder + "data\\" + file_name;
    if (PathExists(path))
        return path;
    path = folder + "extras\\" + file_name;
    if (PathExists(path))
        return path;
    return "";
}

// Lets the user pick the import folder and reads it, or its data folder; says so when neither has a file.
void __fastcall TMainForm::buttonBrowseClick(TObject *Sender)
{
    String folder = RemoveBackslash(read_path);
    if (!SelectDirectory("Select Data folder (folder with .ini files inside)", WideString(""), folder))
        return;
    files_found = false;
    folder = AddBackslash(folder);
    files_found = false;
    drops_found = false;
    chats_found = false;
    shops_found = false;
    trainers_found = false;
    inns_found = false;
    ScanImportFolder(folder);
    if (!files_found)
        ScanImportFolder(folder + "data\\");
    if (!files_found)
        FormFileNotFound->ShowMissing(this, folder);
}

// Lets the user pick the output folder; the current folder stays when the one picked does not exist.
void __fastcall TMainForm::buttonBrowseWriteClick(TObject *Sender)
{
    String folder = RemoveBackslash(read_path);
    if (!SelectDirectory("Select Output folder:", WideString(""), folder))
        return;
    folder = AddBackslash(folder);
    write_path_picked = false;
    write_path = ".";
    write_path_caption = "[ current folder ]";
    if (PathExists(folder))
    {
        write_path_picked = true;
        write_path = folder;
        write_path_caption = folder;
        UpdateWritePath();
    }
}

// Shows on the first page whether import files were found, with the button and the tabs to match.
void TMainForm::UpdateReadPage()
{
    labelPath->Caption = read_path_caption;
    if (MainForm->Caption[2] != 'E')
        return;
    if (MainForm->Caption[3] != 'O')
        return;
    if (files_found)
    {
        TabSheet1->Caption = "  Path Found  ";
        TabSheet2->TabVisible = true;
        TabSheet3->TabVisible = false;
        buttonOk->Caption = "Continue";
        buttonOk->Hint = String("");
        buttonOk->ShowHint = false;
        ImageGfxFileOk->Visible = true;
        labelPath->Hint = String(" open this import (.ini files) folder ");
        labelPath->Cursor = crDrag;
        imageReady->Visible = true;
        ImageGfxFileOk->Visible = true;
        ImageGfxFileBad->Visible = false;
        buttonBrowse->PopupMenu = 0;
        return;
    }
    TabSheet1->Caption = "  Select Path  ";
    TabSheet2->TabVisible = false;
    TabSheet3->TabVisible = false;
    buttonOk->Caption = " Select Path ";
    buttonOk->Hint = String(" cannot import , input eoserv data folder first ");
    buttonOk->ShowHint = true;
    ImageGfxFileOk->Visible = false;
    labelPath->Hint = String(" browse a folder ");
    labelPath->Cursor = crHandPoint;
    imageReady->Visible = false;
    ImageGfxFileOk->Visible = false;
    ImageGfxFileBad->Visible = true;
}

// Shows the output folder on the second page.
void TMainForm::UpdateWritePath()
{
    labelPath->Caption = read_path_caption;
    if (MainForm->Caption[2] != 'E')
        return;
    if (MainForm->Caption[3] != 'O')
        return;
    labelWritePath->Caption = write_path_caption;
    labelWritePath->Hint = String(" open write folder \n");
    labelWritePath->Hint += " " + write_path_caption;
}

// Fills the list of the five files to import: a file found is ticked with its count of entries.
void TMainForm::FillExportGrid()
{
    if (drops_found)
    {
        exportGrid->Items->Item[0]->Caption = "  " + drops_ini;
        exportGrid->Items->Item[0]->Checked = true;
        exportGrid->Items->Item[0]->SubItems->Strings[0] = IntToStr(drops_count) + " drops";
    }
    else
    {
        exportGrid->Items->Item[0]->Caption = "  drops.ini";
        exportGrid->Items->Item[0]->Checked = false;
        exportGrid->Items->Item[0]->SubItems->Strings[0] = "not found";
    }
    if (chats_found)
    {
        exportGrid->Items->Item[1]->Caption = "  " + chats_ini;
        exportGrid->Items->Item[1]->Checked = true;
        exportGrid->Items->Item[1]->SubItems->Strings[0] = IntToStr(chats_count) + " chats";
    }
    else
    {
        exportGrid->Items->Item[1]->Caption = "  npcs.ini";
        exportGrid->Items->Item[1]->Checked = false;
        exportGrid->Items->Item[1]->SubItems->Strings[0] = "not found";
    }
    if (shops_found)
    {
        exportGrid->Items->Item[2]->Caption = "  " + shops_ini;
        exportGrid->Items->Item[2]->Checked = true;
        exportGrid->Items->Item[2]->SubItems->Strings[0] = IntToStr(shops_count) + " shops";
    }
    else
    {
        exportGrid->Items->Item[2]->Caption = "  shops.ini";
        exportGrid->Items->Item[2]->Checked = false;
        exportGrid->Items->Item[2]->SubItems->Strings[0] = "not found";
    }
    if (trainers_found)
    {
        exportGrid->Items->Item[3]->Caption = "  " + trainers_ini;
        exportGrid->Items->Item[3]->Checked = true;
        exportGrid->Items->Item[3]->SubItems->Strings[0] = IntToStr(trainers_count) + " trainer";
    }
    else
    {
        exportGrid->Items->Item[3]->Caption = "  skills.ini";
        exportGrid->Items->Item[3]->Checked = false;
        exportGrid->Items->Item[3]->SubItems->Strings[0] = "not found";
    }
    if (inns_found)
    {
        exportGrid->Items->Item[4]->Caption = "  " + inns_ini;
        exportGrid->Items->Item[4]->Checked = true;
        exportGrid->Items->Item[4]->SubItems->Strings[0] = IntToStr(inns_count) + " inns";
    }
    else
    {
        exportGrid->Items->Item[4]->Caption = "  home.ini";
        exportGrid->Items->Item[4]->Checked = false;
        exportGrid->Items->Item[4]->SubItems->Strings[0] = "not found";
    }
}

// Moves through the pages; on the list page writes each ticked pub file, asking before it overwrites one,
// then shows what was created.
void __fastcall TMainForm::buttonOkClick(TObject *Sender)
{
    if (!files_found)
    {
        buttonBrowseClick((TObject *)buttonBrowse);
        return;
    }
    if (PageControl1->ActivePageIndex == 2)
    {
        ((TCustomForm *)this)->Close();
        return;
    }
    if (PageControl1->ActivePageIndex != 1)
    {
        buttonOk->Caption = "Import Now";
        buttonOk->Hint = String("export pub files");
        PageControl1->ActivePageIndex = 1;
        return;
    }
    if (PageControl1->ActivePageIndex == 1)
    {
        files_created = 0;
        String created_files = "";
        if (exportGrid->Items->Item[0]->Checked && drops_found)
        {
            char write_drops = true;
            String drops_file = drops_pub;
            if (PathExists(write_path + drops_file))
            {
                write_drops = false;
                FormFileExist->AskOverwrite(this, drops_file + " (npc drops)");
                if (FormFileExist->ModalResult == mrOk)
                    write_drops = true;
            }
            if (write_drops)
                created_files += ExportDrops();
        }
        if (exportGrid->Items->Item[1]->Checked && chats_found)
        {
            char write_chats = true;
            String chats_file = chats_pub;
            if (PathExists(write_path + chats_file))
            {
                write_chats = false;
                FormFileExist->AskOverwrite(this, chats_file + " (chat lines)");
                if (FormFileExist->ModalResult == mrOk)
                    write_chats = true;
            }
            if (write_chats)
                created_files += ExportChats();
        }
        if (exportGrid->Items->Item[2]->Checked && shops_found)
        {
            char write_shops = true;
            String shops_file = shops_pub;
            if (PathExists(write_path + shops_file))
            {
                write_shops = false;
                FormFileExist->AskOverwrite(this, shops_file + " (shops)");
                if (FormFileExist->ModalResult == mrOk)
                    write_shops = true;
            }
            if (write_shops)
                created_files += ExportShops();
        }
        if (exportGrid->Items->Item[3]->Checked && trainers_found)
        {
            char write_trainers = true;
            String trainers_file = trainers_pub;
            if (PathExists(write_path + trainers_file))
            {
                write_trainers = false;
                FormFileExist->AskOverwrite(this, trainers_file + " (trainers)");
                if (FormFileExist->ModalResult == mrOk)
                    write_trainers = true;
            }
            if (write_trainers)
                created_files += ExportTrainers();
        }
        if (exportGrid->Items->Item[4]->Checked && inns_found)
        {
            char write_inns = true;
            String inns_file = inns_pub;
            if (PathExists(write_path + inns_file))
            {
                write_inns = false;
                FormFileExist->AskOverwrite(this, inns_file + " (inns)");
                if (FormFileExist->ModalResult == mrOk)
                    write_inns = true;
            }
            if (write_inns)
                created_files += ExportInns();
        }
        if (!files_created)
        {
            imageNoExport->Visible = true;
            imageOkExport->Visible = false;
            labelFilesTotal->Caption = "NOTHING WAS CREATED:";
            labelFilesCreated->Caption = "no pub files to import";
        }
        else
        {
            imageNoExport->Visible = false;
            imageOkExport->Visible = true;
            labelFilesTotal->Caption = IntToStr(files_created) + " PUB FILES CREATED:";
            // The comma after the last file name is taken off.
            created_files.Delete(created_files.Length() - 1, 1);
            labelFilesCreated->Caption = created_files;
        }
        buttonOk->Caption = "Exit";
        buttonOk->Hint = String("close importer");
        TabSheet3->TabVisible = true;
        PageControl1->ActivePageIndex = 2;
        TabSheet2->TabVisible = false;
        TabSheet1->TabVisible = false;
    }
}

bool TMainForm::PathExists(String path)
{
    if (!access(path.c_str(), 0))
        return true;
    return false;
}

// Opens the import folder in Explorer, or lets the user pick one when none was found.
void __fastcall TMainForm::labelPathClick(TObject *Sender)
{
    if (!files_found)
    {
        buttonBrowseClick((TObject *)buttonBrowse);
        return;
    }
    String folder = read_path;
    ShellExecuteA(Application->Handle, "OPEN", "EXPLORER.EXE", folder.c_str(), 0, SW_SHOWNORMAL);
}

void __fastcall TMainForm::labelWritePathClick(TObject *Sender)
{
    String folder = write_path;
    ShellExecuteA(Application->Handle, "OPEN", "EXPLORER.EXE", folder.c_str(), 0, SW_SHOWNORMAL);
}

// Reads the import folder again when the first page comes back.
void __fastcall TMainForm::PageControl1Change(TObject *Sender)
{
    if (PageControl1->ActivePageIndex == 0)
        ScanImportFolder(read_path);
    if (PageControl1->ActivePageIndex == 1)
        buttonOk->Caption = "Import Now";
}

// The path with a backslash at its end.
String TMainForm::AddBackslash(String path)
{
    if (path.Length() > 0)
    {
        if (path[path.Length()] != '\\')
            path = path + '\\';
    }
    return path;
}

// The path without the backslash at its end.
String TMainForm::RemoveBackslash(String path)
{
    if (path.Length() > 0)
    {
        if (path[path.Length()] == '\\')
            path.Delete(path.Length(), 1);
    }
    return path;
}

// Writes the chat timings and lines of each NPC that talks to serv_chats.epf; returns the file's name and a comma.
String TMainForm::ExportChats()
{
    String created = "";
    String header = "";
    header = header.Insert("ETF", header.Length() + 1);
    header = header.Insert(EncodeNumber(0, 2), header.Length() + 1);
    header = header.Insert(EncodeNumber(random(60000), 2), header.Length() + 1);
    header = header.Insert(EncodeNumber(chats_count, 2), header.Length() + 1);
    header = header.Insert(EncodeNumber(pub_version, 1), header.Length() + 1);
    String data = header;
    for (int npc_index = 0; npc_index < npcs->Count(); npc_index++)
    {
        NpcValue *npc = npcs->GetNpc(npc_index);
        // Only the NPCs with chat lines are written.
        if (npc->talk_lines.size() >= 1)
        {
            data.Insert(EncodeNumber(npc->npc_id, 2), data.Length() + 1);
            data.Insert(EncodeNumber(npc->talk_interval, 1), data.Length() + 1);
            data.Insert(EncodeNumber(npc->talk_rate, 1), data.Length() + 1);
            data.Insert(EncodeNumber(npc->talk_lines.size(), 1), data.Length() + 1);
            for (int line_index = 0; line_index < npc->talk_lines.size(); line_index++)
            {
                String line = npc->talk_lines[line_index];
                int line_length = line.Length();
                data.Insert(EncodeNumber(line_length, 1), data.Length() + 1);
                if (line_length > 0)
                    data.Insert(line, data.Length() + 1);
            }
        }
    }
    String file_name = chats_pub;
    String path = write_path + file_name;
    if (!dry_run)
    {
        basic_ofstream<char, char_traits<char> > file;
        file.open(path.c_str(), std::ios::binary);
        if ((char)file.is_open())
        {
            file << data.c_str();
            file.close();
            created += file_name + ", ";
            files_created++;
        }
        else
            write_failed = true;
    }
    return created;
}

// Writes the drops of each NPC that drops something to serv_drops.epf; returns the file's name and a comma.
String TMainForm::ExportDrops()
{
    String created = "";
    String header = "";
    header = header.Insert("EDF", header.Length() + 1);
    header = header.Insert(EncodeNumber(0, 2), header.Length() + 1);
    header = header.Insert(EncodeNumber(random(60000), 2), header.Length() + 1);
    header = header.Insert(EncodeNumber(drops_count, 2), header.Length() + 1);
    header = header.Insert(EncodeNumber(pub_version, 1), header.Length() + 1);
    String data = header;
    for (int npc_index = 0; npc_index < npcs->Count(); npc_index++)
    {
        NpcValue *npc = npcs->GetNpc(npc_index);
        // Only the NPCs with drops are written.
        if (npc->drops.size() >= 1)
        {
            data.Insert(EncodeNumber(npc->npc_id, 2), data.Length() + 1);
            data.Insert(EncodeNumber(npc->drops.size(), 2), data.Length() + 1);
            for (int drop_index = 0; drop_index < npc->drops.size(); drop_index++)
            {
                NpcDropItem drop = npc->drops[drop_index];
                data.Insert(EncodeNumber(drop.item_id, 2), data.Length() + 1);
                data.Insert(EncodeNumber(drop.min_amount, 3), data.Length() + 1);
                data.Insert(EncodeNumber(drop.max_amount, 3), data.Length() + 1);
                data.Insert(EncodeNumber(drop.rate, 2), data.Length() + 1);
            }
        }
    }
    String file_name = drops_pub;
    String path = write_path + file_name;
    if (!dry_run)
    {
        basic_ofstream<char, char_traits<char> > file;
        file.open(path.c_str(), std::ios::binary);
        if ((char)file.is_open())
        {
            file << data.c_str();
            file.close();
            created += file_name + ", ";
            files_created++;
        }
        else
            write_failed = true;
    }
    return created;
}

// Writes every shop with its trades and crafts to serv_shops.epf; returns the file's name and a comma.
String TMainForm::ExportShops()
{
    String created = "";
    String header = "";
    header = header.Insert("ESF", header.Length() + 1);
    header = header.Insert(EncodeNumber(0, 2), header.Length() + 1);
    header = header.Insert(EncodeNumber(random(60000), 2), header.Length() + 1);
    header = header.Insert(EncodeNumber(shops_count, 2), header.Length() + 1);
    header = header.Insert(EncodeNumber(pub_version, 1), header.Length() + 1);
    String data(header);
    for (int shop_index = 0; shop_index < (unsigned int)shops->Count(); shop_index++)
    {
        ShopValue *shop = shops->GetShop(shop_index);
        data.Insert(EncodeNumber(shop->behavior_id, 2), data.Length() + 1);
        String name = shop->name;
        int name_length = name.Length();
        data.Insert(EncodeNumber(name_length, 1), data.Length() + 1);
        data.Insert(name, data.Length() + 1);
        data.Insert(EncodeNumber(shop->min_level, 2), data.Length() + 1);
        data.Insert(EncodeNumber(shop->max_level, 2), data.Length() + 1);
        data.Insert(EncodeNumber(shop->class_requirement, 1), data.Length() + 1);
        data.Insert(EncodeNumber(shop->trades.size(), 2), data.Length() + 1);
        data.Insert(EncodeNumber(shop->crafts.size(), 1), data.Length() + 1);
        if (shop)
        {
            for (int trade_index = 0; trade_index < shop->trades.size(); trade_index++)
            {
                ShopItemVal trade = shop->trades[trade_index];
                data.Insert(EncodeNumber(trade.item_id, 2), data.Length() + 1);
                data.Insert(EncodeNumber(trade.buy_price, 3), data.Length() + 1);
                data.Insert(EncodeNumber(trade.sell_price, 3), data.Length() + 1);
                data.Insert(EncodeNumber(trade.max_amount, 1), data.Length() + 1);
            }
            for (int craft_index = 0; craft_index < shop->crafts.size(); craft_index++)
            {
                ShopCraftVal craft = shop->crafts[craft_index];
                data.Insert(EncodeNumber(craft.id, 2), data.Length() + 1);
                data.Insert(EncodeNumber(craft.ingredient_item_ids[0], 2), data.Length() + 1);
                data.Insert(EncodeNumber(craft.ingredient_amounts[0], 1), data.Length() + 1);
                data.Insert(EncodeNumber(craft.ingredient_item_ids[1], 2), data.Length() + 1);
                data.Insert(EncodeNumber(craft.ingredient_amounts[1], 1), data.Length() + 1);
                data.Insert(EncodeNumber(craft.ingredient_item_ids[2], 2), data.Length() + 1);
                data.Insert(EncodeNumber(craft.ingredient_amounts[2], 1), data.Length() + 1);
                data.Insert(EncodeNumber(craft.ingredient_item_ids[3], 2), data.Length() + 1);
                data.Insert(EncodeNumber(craft.ingredient_amounts[3], 1), data.Length() + 1);
            }
        }
    }
    String file_name = shops_pub;
    String path = write_path + file_name;
    if (!dry_run)
    {
        basic_ofstream<char, char_traits<char> > file;
        file.open(path.c_str(), std::ios::binary);
        if ((char)file.is_open())
        {
            file << data.c_str();
            file.close();
            created += file_name + ", ";
            files_created++;
        }
        else
            write_failed = true;
    }
    return created;
}

// Writes every skill master with the skills it teaches to serv_trainers.epf; returns the file's name and a comma.
String TMainForm::ExportTrainers()
{
    int name_length;
    String created = "";
    String header = "";
    header = header.Insert("ETF", header.Length() + 1);
    header = header.Insert(EncodeNumber(0, 2), header.Length() + 1);
    header = header.Insert(EncodeNumber(random(60000), 2), header.Length() + 1);
    header = header.Insert(EncodeNumber(trainers_count, 2), header.Length() + 1);
    header = header.Insert(EncodeNumber(pub_version, 1), header.Length() + 1);
    String data = header;
    for (int trainer_index = 0; trainer_index < (unsigned int)trainers->Count(); trainer_index++)
    {
        LearnValue *trainer = trainers->GetTrainer(trainer_index);
        data.Insert(EncodeNumber(trainer->behavior_id, 2), data.Length() + 1);
        String name = trainer->name;
        name_length = name.Length();
        data.Insert(EncodeNumber(name_length, 1), data.Length() + 1);
        data.Insert(name, data.Length() + 1);
        data.Insert(EncodeNumber(trainer->min_level, 2), data.Length() + 1);
        data.Insert(EncodeNumber(trainer->max_level, 2), data.Length() + 1);
        data.Insert(EncodeNumber(trainer->class_requirement, 1), data.Length() + 1);
        data.Insert(EncodeNumber(trainer->skills.size(), 2), data.Length() + 1);
        if (trainer)
        {
            for (int skill_index = 0; skill_index < trainer->skills.size(); skill_index++)
            {
                LearnItemVal skill = trainer->skills[skill_index];
                data.Insert(EncodeNumber(skill.id, 2), data.Length() + 1);
                data.Insert(EncodeNumber(skill.level_requirement, 2), data.Length() + 1);
                data.Insert(EncodeNumber(skill.class_requirement, 1), data.Length() + 1);
                data.Insert(EncodeNumber(skill.price, 3), data.Length() + 1);
                data.Insert(EncodeNumber(skill.skill_requirement_1, 2), data.Length() + 1);
                data.Insert(EncodeNumber(skill.skill_requirement_2, 2), data.Length() + 1);
                data.Insert(EncodeNumber(skill.skill_requirement_3, 2), data.Length() + 1);
                data.Insert(EncodeNumber(skill.skill_requirement_4, 2), data.Length() + 1);
                data.Insert(EncodeNumber(skill.str_requirement, 2), data.Length() + 1);
                data.Insert(EncodeNumber(skill.wis_requirement, 2), data.Length() + 1);
                data.Insert(EncodeNumber(skill.int_requirement, 2), data.Length() + 1);
                data.Insert(EncodeNumber(skill.agi_requirement, 2), data.Length() + 1);
                data.Insert(EncodeNumber(skill.con_requirement, 2), data.Length() + 1);
                data.Insert(EncodeNumber(skill.cha_requirement, 2), data.Length() + 1);
            }
        }
    }
    String file_name = trainers_pub;
    String path = write_path + file_name;
    if (!dry_run)
    {
        basic_ofstream<char, char_traits<char> > file;
        file.open(path.c_str(), std::ios::binary);
        if ((char)file.is_open())
        {
            file << data.c_str();
            file.close();
            created += file_name + ", ";
            files_created++;
        }
        else
            write_failed = true;
    }
    return created;
}

// Writes every inn with its places and questions to serv_inns.epf; returns the file's name and a comma.
String TMainForm::ExportInns()
{
    String created = "";
    String header = "";
    header = header.Insert("EID", header.Length() + 1);
    header = header.Insert(EncodeNumber(0, 2), header.Length() + 1);
    header = header.Insert(EncodeNumber(random(60000), 2), header.Length() + 1);
    header = header.Insert(EncodeNumber(inns_count, 2), header.Length() + 1);
    header = header.Insert(EncodeNumber(pub_version, 1), header.Length() + 1);
    String data = header;
    for (int inn_index = 0; inn_index < (unsigned int)inns->Count(); inn_index++)
    {
        InnValue *inn = inns->GetInn(inn_index);
        data.Insert(EncodeNumber(inn->behavior_id, 2), data.Length() + 1);
        String name = inn->name;
        int name_length = name.Length();
        data.Insert(EncodeNumber(name_length, 1), data.Length() + 1);
        data.Insert(name, data.Length() + 1);
        data.Insert(EncodeNumber(inn->sleep_cost, 3), data.Length() + 1);
        data.Insert(EncodeNumber(inn->sleep_map, 2), data.Length() + 1);
        data.Insert(EncodeNumber(inn->sleep_x, 1), data.Length() + 1);
        data.Insert(EncodeNumber(inn->sleep_y, 1), data.Length() + 1);
        data.Insert(EncodeNumber(inn->spawn_map, 2), data.Length() + 1);
        data.Insert(EncodeNumber(inn->spawn_x, 1), data.Length() + 1);
        data.Insert(EncodeNumber(inn->spawn_y, 1), data.Length() + 1);
        data.Insert(EncodeNumber(inn->alternate_spawn_level, 2), data.Length() + 1);
        data.Insert(EncodeNumber(inn->alternate_spawn_map, 2), data.Length() + 1);
        data.Insert(EncodeNumber(inn->alternate_spawn_x, 1), data.Length() + 1);
        data.Insert(EncodeNumber(inn->alternate_spawn_y, 1), data.Length() + 1);
        int question_count = inn->questions.size();
        data.Insert(EncodeNumber(inn->questions.size(), 1), data.Length() + 1);
        if (inn)
        {
            for (int question_index = 0; question_index < inn->questions.size(); question_index++)
            {
                InnQuizItem question = inn->questions[question_index];
                int text_length = question.question.Length();
                data.Insert(EncodeNumber(text_length, 1), data.Length() + 1);
                data.Insert(question.question, data.Length() + 1);
                text_length = question.answer.Length();
                data.Insert(EncodeNumber(text_length, 1), data.Length() + 1);
                data.Insert(question.answer, data.Length() + 1);
            }
        }
    }
    String file_name = inns_pub;
    String path = write_path + file_name;
    if (!dry_run)
    {
        basic_ofstream<char, char_traits<char> > file;
        file.open(path.c_str(), std::ios::binary);
        if ((char)file.is_open())
        {
            file << data.c_str();
            file.close();
            created += file_name + ", ";
            files_created++;
        }
        else
            write_failed = true;
    }
    return created;
}

// Writes a number given as its text the protocol's way; empty text is written as 0.
String TMainForm::EncodeNumber(String text, int width)
{
    if (text == "")
        text = "0";
    return EncodeNumber(StrToInt(text), width);
}

// Writes a number the protocol's way, base 253 and padded with 254; a one-byte number over 252 is written as 0.
String TMainForm::EncodeNumber(unsigned int value, int width)
{
    unsigned int quotient;
    int i;
    unsigned int digit;
    if (width == 1 && value > 252)
        value = 0;
    unsigned int remaining = value;
    String result = "";
    try
    {
        do
        {
            double scaled = remaining / 253.0;
            quotient = (int)scaled;
            digit = remaining % 253;
            value = digit;
            value++;
            result.Insert((char)value, result.Length() + 1);
            if (quotient >= 1)
                remaining = quotient;
        } while (quotient >= 1);
    }
    catch (...)
    {
        result = "";
    }
    // A number shorter than its width is padded with 254.
    if (result.Length() < width)
    {
        char pad = -2;
        int missing = width - result.Length();
        for (i = 0; i < missing; i++)
            result.Insert(pad, result.Length() + 1);
    }
    return result;
}
