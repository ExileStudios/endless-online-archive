#include <vcl.h>
#pragma hdrstop

#include "UnitExportForm.h"
#include "UnitMain.h"
#include "UnitAbout.h"
#include "UnitExportResultForm.h"
#include "Datagrid.h"
#include "Datagridcol.h"
#include <dir.h>
#include <Graphics.hpp>
#include "Innvalues.h"
#include <io.h>
#include "Learnvalue.h"
#include "Learnvalues.h"
#include "Npcdataadapter.h"
#include "Npcvalue.h"
#include "Npcvalues.h"
#include "Shopvalue.h"
#include "Shopvalues.h"
#include <stdlib.h>
#include <vector.h>

#pragma package(smart_init)
#pragma link "ElPopBtn"
#pragma link "ElBiProgr"
#pragma link "ElCheckCtl"
#pragma resource "*.dfm"

TFormExport *FormExport;

// Default limits: 900 rows and 64000 bytes to a file; every kind of pub is exported.
__fastcall TFormExport::TFormExport(TComponent *owner)
    : TForm(owner)
{
    TComponent *component;
    header_version = 2;
    rows_per_file = 900;
    file_size_limit = 64000;
    dry_run = false;
    shared_signature = true;
    export_items = true;
    export_npcs = true;
    export_skills = true;
    export_classes = true;
    export_shops = true;
    export_trainers = true;
    export_inns = true;
    export_drops = true;
    export_chats = true;
}

// Themes the form and counts the records to export; flags an export path other than the pub path.
void __fastcall TFormExport::FormShow(TObject *Sender)
{
    if (FormMain->config->GetTheme() == "dark")
    {
        FormMain->ThemeComponents(this);
        Color = (TColor)0x525252;
        panelProgress->BevelInner = bvNone;
        panelProgress->Color = (TColor)0x4F4F4F;
    }
    exportPathText->Caption = FormMain->config->GetExportPath();
    int record_count = CountExportRows();
    Caption = " Export ( " + IntToStr(record_count) + " records )";
    ProgressBar1->Scale = record_count;
    ProgressBar1->LightValue = 0;
    buttonCancel->Left = 146;
    buttonCancel->Caption = WideString("CANCEL");
    buttonOk->Visible = true;
    buttonOk->Enabled = true;
    panelWorking->Top = panelExportStatusHolder->Height;
    panelWorking->Height = panelExportStatusHolder->Height;
    panelComplete->Top = panelExportStatusHolder->Height;
    panelComplete->Height = panelExportStatusHolder->Height;
    buttonOk->Hint = WideString("Save pub files to :" + FormMain->config->GetExportPath());
    slide_stage = 0;
    slide_top = 0;
    Timer1->Enabled = true;
    Timer2->Enabled = false;
    header_version = 2;
    rows_per_file = 900;
    file_size_limit = 64000;
    checkWriteOversized->Checked = false;
    checkWriteOversized->Visible = FormMain->config->GetExpertMode();
    panelDifferent->Visible = false;
    if (FormMain->config->GetPubPath() != FormMain->config->GetExportPath())
    {
        panelDifferent->Visible = true;
        Timer3->Enabled = true;
    }
    flash_ticks = 0;
    buttonTabCatch->SetFocus();
}

// Counts the rows of every kind that is exported, and how often the progress bar repaints.
int TFormExport::CountExportRows()
{
    export_rows = 0;
    if (export_items)
        export_rows += FormMain->ElTreeStringGrid1->Items->Count;
    if (export_npcs)
        export_rows += FormMain->ElTreeStringGrid5->Items->Count;
    if (export_skills)
        export_rows += FormMain->ElTreeStringGrid7->Items->Count;
    if (export_classes)
        export_rows += FormMain->ElTreeStringGrid3->Items->Count;
    if (export_shops)
        export_rows += FormMain->ElTreeStringGrid9->Items->Count;
    if (export_trainers)
        export_rows += FormMain->ElTreeStringGrid11->Items->Count;
    if (export_inns)
        export_rows += FormMain->ElTreeStringGrid13->Items->Count;
    FormMain->npcs_tab->adapter->CountLists();
    if (export_drops)
        export_rows += FormMain->npcs_tab->adapter->message_count;
    if (export_chats)
        export_rows += FormMain->npcs_tab->adapter->drop_count;
    repaint_step = export_rows / 300;
    if (repaint_step < 1)
        repaint_step = 1;
    return export_rows;
}

// Repaints the progress bar now and then.
void TFormExport::StepProgress()
{
    if (ProgressBar1->LightValue % repaint_step == 0)
    {
        ProgressBar1->Update();
        Paint();
    }
}

// Slides the working panel up and then starts the export; slides the done panel up after it.
void __fastcall TFormExport::Timer2Timer(TObject *Sender)
{
    if (!Visible)
        Timer2->Enabled = false;
    if (FormAbout->Label5->Caption[2] != 'u')
        return;
    if (FormAbout->Label5->Left + 1 != 417)
        return;
    if (FormAbout->Label5->Top + 1 != 177)
        return;
    // Each tick slides a panel up; once the working panel is in, the backup starts.
    slide_top -= 3;
    if (slide_stage == 1)
    {
        panelWorking->Top = slide_top;
        if (slide_top <= 0)
        {
            panelWorking->Top = 0;
            Timer2->Enabled = false;
            BackupPubFiles();
            return;
        }
    }
    if (slide_stage == 2)
    {
        panelComplete->Top = slide_top;
        if (slide_top <= 0)
        {
            TFont *font = exportPathTxt->Font;
            font->Style = font->Style << fsUnderline;
            panelComplete->Top = 0;
            slide_top = 100;
            slide_stage = 3;
            return;
        }
    }
    if (slide_stage == 3 && slide_top <= 0)
    {
        TFont *path_font = exportPathTxt->Font;
        path_font->Style = path_font->Style >> fsUnderline;
        Timer2->Enabled = false;
        return;
    }
}

// Backs up the pub files with a readme when backups are on, then writes every kind and shows the result.
void TFormExport::BackupPubFiles()
{
    String written_files = "";
    export_error = 0;
    files_saved = 0;
    signature = random(63980) + 10;
    FormMain->ClearChanged();
    if (FormAbout->Label5->Caption[1] != 'v' && FormAbout->Label5->Caption[1] != 'V')
        return;
    String backup_folder = FormMain->config->GetBackupPath();
    if (FormMain->config->GetBackup() && !dry_run)
    {
        char folder_ready = true;
        backup_readme = "\n";
        if (!FormMain->config_check->CheckBackupPath(backup_folder))
        {
            try
            {
                mkdir(backup_folder.c_str());
            }
            catch (...)
            {
                folder_ready = false;
            }
        }
        // Each backup can go in a folder of its own, named after the date and time.
        if (FormMain->config->GetBackupFolders())
        {
            String dated_folder = DigitsOnly(DateToStr(Now())) + " " + DigitsOnly(TimeToStr(Now())) + "\\";
            backup_folder += dated_folder;
            try
            {
                mkdir(backup_folder.c_str());
            }
            catch (...)
            {
                folder_ready = false;
            }
        }
        if (folder_ready)
        {
            if (export_classes)
                BackupFileParts(backup_folder, "dat", ".ecf");
            if (export_items)
                BackupFileParts(backup_folder, "dat", ".eif");
            if (export_skills)
                BackupFileParts(backup_folder, "dsl", ".esf");
            if (export_npcs)
                BackupFileParts(backup_folder, "dtn", ".enf");
            if (export_shops)
                BackupFileParts(backup_folder, "sds", ".esf");
            if (export_trainers)
                BackupFileParts(backup_folder, "sdt", ".etf");
            if (export_inns)
                BackupFileParts(backup_folder, "sdi", ".eif");
            if (export_drops)
                BackupFileParts(backup_folder, "sdn", ".edf");
            if (export_chats)
                BackupFileParts(backup_folder, "sdn", ".etf");
            TStringList *readme = new TStringList();
            readme->Add("# Endless Pub Editor " + FormMain->version + " [ backup readme ]");
            readme->Add("");
            readme->Add(" auto-generated backup files");
            readme->Add(" date: " + DateToStr(Now()) + " " + TimeToStr(Now()));
            readme->Add(backup_readme);
            String readme_path = backup_folder + "readme.txt";
            readme->SaveToFile(readme_path);
        }
    }
    String export_path = FormMain->config->GetExportPath();
    if (access(export_path.c_str(), 0))
    {
        try
        {
            mkdir(export_path.c_str());
        }
        catch (...)
        {
        }
    }
    randomize();
    if (FormAbout->Label5->Caption.Length() != 6)
        return;
    if (FormAbout->Label5->Left != 416)
        return;
    if (FormAbout->Label5->Top != 176)
        return;
    // Each kind writes its files and gives back their names for the result.
    written_files += ExportClasses();
    written_files += ExportItems();
    written_files += ExportNpcs();
    written_files += ExportSkills();
    written_files += ExportShops();
    written_files += ExportTrainers();
    written_files += ExportInns();
    written_files += ExportChats();
    written_files += ExportDrops();
    panelCompleteTxt->Caption = IntToStr(files_saved) + " Files saved";
    if (files_saved > 0)
        labelExportResult->Caption = "click on the export path to locate the new pub files.";
    if (!files_saved)
        labelExportResult->Caption = "failed to write all pub files to the export path.";
    else if (export_error == 2)
        labelExportResult->Caption = "failed to sort pub dataset, aborted by critical error.";
    else if (export_error == 1)
        labelExportResult->Caption = "error to write pub files(s) to the export path";
    Timer2->Enabled = true;
    slide_stage = 2;
    slide_top = panelComplete->Top;
    FormExportResult->ShowResult(written_files);
    ModalResult = mrNone;
    ((TControl *)this)->BringToFront();
}

// Backs up each numbered part of the pub file, 001 on, while the next one exists.
void TFormExport::BackupFileParts(String backup_folder, String prefix, String extension)
{
    for (int part = 1; part < 9; part++)
    {
        String file_name = prefix;
        if (part < 10)
            file_name += "00" + IntToStr(part);
        else if (part < 100)
            file_name += "0" + IntToStr(part);
        file_name += extension;
        String source = FormMain->config->GetPubPath() + file_name;
        if (!access(source.c_str(), 0))
        {
            BackupFile(backup_folder, file_name);
            continue;
        }
        break;
    }
}

void TFormExport::BackupFile(String backup_folder, String file_name)
{
    String source = FormMain->config->GetPubPath() + file_name;
    String backup = backup_folder + file_name;
    backup_readme += "[" + file_name + "]\n";
    backup_readme += "  source: " + source + "\n";
    backup_readme += "  backup: " + backup + "\n\n";
    try
    {
        CopyFileA(source.c_str(), backup.c_str(), 1);
    }
    catch (...)
    {
    }
}

String TFormExport::DigitsOnly(String text)
{
    String digits = "";
    for (int i = 1; i <= text.Length(); i++)
    {
        if (text[i] == '0' || text[i] == '1' || text[i] == '2' || text[i] == '3' || text[i] == '4' || text[i] == '5' ||
            text[i] == '6' || text[i] == '7' || text[i] == '8' || text[i] == '9')
            digits += text[i];
    }
    return digits;
}

void __fastcall TFormExport::exportPathTextClick(TObject *Sender)
{
    if (FormMain->config_check->CheckExportPath(exportPathText->Caption))
    {
        String path = exportPathText->Caption;
        ShellExecuteA(Application->Handle, "OPEN", "EXPLORER.EXE", path.c_str(), 0, SW_SHOWNORMAL);
    }
}

// Writes the class rows to dat001.ecf on, starting a new file at the row or size limit.
String TFormExport::ExportClasses()
{
    String written_files = "";
    TElTreeStringGrid *grid = FormMain->ElTreeStringGrid3;
    if (grid->Items->Count <= 0 && !FormMain->config->GetExportEmpty())
        return written_files;
    char reversed = false;
    if (grid->Items->Count > 0)
    {
        if (String(grid->Items->Item[0]->Text) != "1")
            reversed = true;
    }
    DataGridCol *name_column = FormMain->classes_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *stat_group_column = FormMain->classes_tab->grid_ui->data_grid->FindColumn("Stat Group");
    DataGridCol *parent_column = FormMain->classes_tab->grid_ui->data_grid->FindColumn("Parent");
    DataGridCol *str_column = FormMain->classes_tab->grid_ui->data_grid->FindColumn("Str");
    DataGridCol *int_column = FormMain->classes_tab->grid_ui->data_grid->FindColumn("Int");
    DataGridCol *wis_column = FormMain->classes_tab->grid_ui->data_grid->FindColumn("Wis");
    DataGridCol *agi_column = FormMain->classes_tab->grid_ui->data_grid->FindColumn("Agi");
    DataGridCol *con_column = FormMain->classes_tab->grid_ui->data_grid->FindColumn("Con");
    DataGridCol *cha_column = FormMain->classes_tab->grid_ui->data_grid->FindColumn("Cha");
    // Each file starts with its kind, the signature, a random ID, the row count and the version.
    String header = "";
    header = header.Insert("ECF", header.Length() + 1);
    header = header.Insert(EncodeInteger(FileSignature(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(Random(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(grid->Items->Count, 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(header_version, 1), header.Length() + 1);
    int row_index = 0;
    int file_number = 0;
    for (; grid->Items->Count >= row_index; row_index++)
    {
        String file_text = header;
        int rows_in_file = 0;
        for (; grid->Items->Count > row_index; row_index++)
        {
            int row;
            if (!reversed)
                row = (int)grid->Items->Item[row_index];
            if (reversed)
                row = (int)grid->Items->Item[grid->Items->Count - row_index - 1];
            // IDs must follow on without a gap, or the export stops with a sort error.
            if (String(row_index + 1) != String(((TElTreeItem *)row)->Text))
            {
                export_error = 2;
                return written_files;
            }
            int name_length = ((TElTreeItem *)row)->ColumnText->Strings[name_column->text_index].Length();
            // A row that would pass the size limit starts the next file instead.
            if (file_text.Length() + name_length + 15 > file_size_limit)
            {
                row_index--;
                break;
            }
            file_text.Insert(EncodeInteger(name_length, 1), file_text.Length() + 1);
            file_text.Insert(((TElTreeItem *)row)->ColumnText->Strings[name_column->text_index],
                file_text.Length() + 1);
            int parent_id =
                parent_column->LinkedText(((TElTreeItem *)row)->ColumnText->Strings[parent_column->text_index], 1);
            file_text.Insert(EncodeInteger(parent_id, 1), file_text.Length() + 1);
            combo_index =
                stat_group_column->ComboIndexOf(((TElTreeItem *)row)->ColumnText->Strings[stat_group_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 1), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[str_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[int_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[wis_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[agi_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[con_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[cha_column->text_index], 2),
                file_text.Length() + 1);
            ProgressBar1->LightValue = ProgressBar1->LightValue + 1;
            StepProgress();
            rows_in_file++;
            if (rows_in_file >= rows_per_file)
                break;
        }
        file_number++;
        String file_name = AppendFileNumber("dat", file_number) + ".ecf";
        String path = FormMain->config->GetExportPath() + file_name;
        if (!dry_run)
        {
            basic_ofstream<char, char_traits<char> > file;
            file.open(path.c_str(), std::ios::binary);
            if ((char)file.is_open())
            {
                file << file_text.c_str();
                file.close();
                written_files += file_name + ", ";
                files_saved++;
            }
            else
                export_error = 1;
        }
        if (grid->Items->Count == row_index)
            break;
    }
    return written_files;
}

// Writes the item rows to dat001.eif on, starting a new file at the row or size limit.
String TFormExport::ExportItems()
{
    TElTreeItem *row;
    String written_files = "";
    TElTreeStringGrid *grid = FormMain->ElTreeStringGrid1;
    if (grid->Items->Count <= 0 && !FormMain->config->GetExportEmpty())
        return written_files;
    char reversed = false;
    if (grid->Items->Count > 0)
    {
        if (String(grid->Items->Item[0]->Text) != "1")
            reversed = true;
    }
    DataGridCol *name_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *gfx_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Gfx");
    DataGridCol *type_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Type");
    DataGridCol *subtype_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Subtype");
    DataGridCol *unique_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Unique");
    DataGridCol *weight_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Weight");
    DataGridCol *size_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Size");
    DataGridCol *first_data_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Data.1");
    DataGridCol *second_data_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Data.2");
    DataGridCol *third_data_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Data.3");
    DataGridCol *level_req_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("req:Lvl");
    DataGridCol *class_req_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("req:Cls");
    DataGridCol *str_req_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("req:Str");
    DataGridCol *int_req_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("req:Int");
    DataGridCol *wis_req_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("req:Wis");
    DataGridCol *agi_req_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("req:Agi");
    DataGridCol *con_req_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("req:Con");
    DataGridCol *cha_req_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("req:Cha");
    DataGridCol *hp_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Hp+");
    DataGridCol *tp_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Tp+");
    DataGridCol *low_damage_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Dmg-lo");
    DataGridCol *high_damage_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Dmg-hi");
    DataGridCol *hit_rate_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Hitrate");
    DataGridCol *evade_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Evade");
    DataGridCol *armor_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Armor");
    DataGridCol *r_damage_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("R-dmg");
    DataGridCol *element_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Element");
    DataGridCol *element_damage_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Element.Dmg");
    DataGridCol *light_res_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("res:Light");
    DataGridCol *dark_res_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("res:Dark");
    DataGridCol *earth_res_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("res:Earth");
    DataGridCol *air_res_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("res:Air");
    DataGridCol *water_res_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("res:Water");
    DataGridCol *fire_res_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("res:Fire");
    DataGridCol *str_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Str");
    DataGridCol *int_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Int");
    DataGridCol *wis_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Wis");
    DataGridCol *agi_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Agi");
    DataGridCol *con_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Con");
    DataGridCol *cha_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Cha");
    DataGridCol *range_column = FormMain->items_tab->grid_ui->data_grid->FindColumn("Range");
    String header = "";
    header = header.Insert("EIF", header.Length() + 1);
    header = header.Insert(EncodeInteger(FileSignature(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(Random(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(grid->Items->Count, 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(header_version, 1), header.Length() + 1);
    int row_index = 0;
    int file_number = 0;
    for (; grid->Items->Count >= row_index; row_index++)
    {
        String file_text = header;
        int rows_in_file = 0;
        for (; grid->Items->Count > row_index; row_index++)
        {
            if (!reversed)
                row = grid->Items->Item[row_index];
            if (reversed)
                row = grid->Items->Item[grid->Items->Count - row_index - 1];
            if (String(row_index + 1) != String(row->Text))
            {
                export_error = 2;
                return written_files;
            }
            int name_length = row->ColumnText->Strings[name_column->text_index].Length();
            // An item row takes its name and 59 bytes; past the limit it starts the next file.
            if (file_text.Length() + name_length + 59 > file_size_limit)
            {
                row_index--;
                break;
            }
            file_text.Insert(EncodeInteger(name_length, 1), file_text.Length() + 1);
            file_text.Insert(row->ColumnText->Strings[name_column->text_index], file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[gfx_column->text_index], 2), file_text.Length() + 1);
            combo_index = type_column->ComboIndexOf(row->ColumnText->Strings[type_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 1), file_text.Length() + 1);
            combo_index = subtype_column->ComboIndexOf(row->ColumnText->Strings[subtype_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 1), file_text.Length() + 1);
            combo_index = unique_column->ComboIndexOf(row->ColumnText->Strings[unique_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 1), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[hp_column->text_index], 2), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[tp_column->text_index], 2), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[low_damage_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[high_damage_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[hit_rate_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[evade_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[armor_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[r_damage_column->text_index], 1),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[str_column->text_index], 1), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[int_column->text_index], 1), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[wis_column->text_index], 1), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[agi_column->text_index], 1), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[con_column->text_index], 1), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[cha_column->text_index], 1), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[light_res_column->text_index], 1),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[dark_res_column->text_index], 1),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[earth_res_column->text_index], 1),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[air_res_column->text_index], 1),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[water_res_column->text_index], 1),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[fire_res_column->text_index], 1),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[first_data_column->text_index], 3),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[second_data_column->text_index], 1),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[third_data_column->text_index], 1),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[level_req_column->text_index], 2),
                file_text.Length() + 1);
            int class_id = class_req_column->LinkedText(row->ColumnText->Strings[class_req_column->text_index], 1);
            file_text.Insert(EncodeInteger(class_id, 2), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[str_req_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[int_req_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[wis_req_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[agi_req_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[con_req_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[cha_req_column->text_index], 2),
                file_text.Length() + 1);
            combo_index = element_column->ComboIndexOf(row->ColumnText->Strings[element_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 1), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[element_damage_column->text_index], 1),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(row->ColumnText->Strings[weight_column->text_index], 1),
                file_text.Length() + 1);
            combo_index = range_column->ComboIndexOf(row->ColumnText->Strings[range_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 1), file_text.Length() + 1);
            combo_index = size_column->ComboIndexOf(row->ColumnText->Strings[size_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 1), file_text.Length() + 1);
            ProgressBar1->LightValue = ProgressBar1->LightValue + 1;
            StepProgress();
            rows_in_file++;
            if (rows_per_file <= rows_in_file)
                break;
        }
        file_number++;
        String file_name = AppendFileNumber("dat", file_number) + ".eif";
        String path = FormMain->config->GetExportPath() + file_name;
        if (!dry_run)
        {
            basic_ofstream<char, char_traits<char> > file;
            file.open(path.c_str(), std::ios::binary);
            if ((char)file.is_open())
            {
                file << file_text.c_str();
                file.close();
                written_files += file_name + ", ";
                files_saved++;
            }
            else
                export_error = 1;
        }
        if (grid->Items->Count == row_index)
            break;
    }
    return written_files;
}

// Writes the npc rows to dtn001.enf on, starting a new file at the row or size limit.
String TFormExport::ExportNpcs()
{
    String written_files = "";
    TElTreeStringGrid *grid = FormMain->ElTreeStringGrid5;
    if (grid->Items->Count <= 0 && !FormMain->config->GetExportEmpty())
        return written_files;
    char reversed = false;
    if (grid->Items->Count > 0)
    {
        if (String(grid->Items->Item[0]->Text) != "1")
            reversed = true;
    }
    DataGridCol *name_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *gfx_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Gfx");
    DataGridCol *level_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Level");
    DataGridCol *experience_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Experience");
    DataGridCol *behaviour_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Behaviour");
    DataGridCol *behaviour_id_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Beh.ID");
    DataGridCol *hp_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Hp");
    DataGridCol *tp_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Tp");
    DataGridCol *low_damage_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Dmg-lo");
    DataGridCol *high_damage_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Dmg-hi");
    DataGridCol *hit_rate_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Hitrate");
    DataGridCol *evade_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Evade");
    DataGridCol *armor_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Armor");
    DataGridCol *r_damage_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("R-dmg");
    DataGridCol *race_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Race");
    DataGridCol *boss_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Boss");
    DataGridCol *add_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Add");
    DataGridCol *attack_element_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Elem-A");
    DataGridCol *attack_element_damage_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Elem-A.dmg ");
    DataGridCol *weak_element_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Elem-W");
    DataGridCol *weak_element_damage_column = FormMain->npcs_tab->grid_ui->data_grid->FindColumn("Elem-W.dmg ");
    String header = "";
    header = header.Insert("ENF", header.Length() + 1);
    header = header.Insert(EncodeInteger(FileSignature(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(Random(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(grid->Items->Count, 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(header_version, 1), header.Length() + 1);
    int row_index = 0;
    int file_number = 0;
    for (; grid->Items->Count >= row_index; row_index++)
    {
        String file_text = header;
        int rows_in_file = 0;
        for (; grid->Items->Count > row_index; row_index++)
        {
            int row;
            if (!reversed)
                row = (int)grid->Items->Item[row_index];
            if (reversed)
                row = (int)grid->Items->Item[grid->Items->Count - row_index - 1];
            if (String(row_index + 1) != String(((TElTreeItem *)row)->Text))
            {
                export_error = 2;
                return written_files;
            }
            int name_length = ((TElTreeItem *)row)->ColumnText->Strings[name_column->text_index].Length();
            if (file_text.Length() + name_length + 40 > file_size_limit)
            {
                row_index--;
                break;
            }
            file_text.Insert(EncodeInteger(name_length, 1), file_text.Length() + 1);
            if (name_length > 0)
                file_text.Insert(((TElTreeItem *)row)->ColumnText->Strings[name_column->text_index],
                    file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[gfx_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[race_column->text_index], 1),
                file_text.Length() + 1);
            combo_index = boss_column->ComboIndexOf(((TElTreeItem *)row)->ColumnText->Strings[boss_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 2), file_text.Length() + 1);
            combo_index = add_column->ComboIndexOf(((TElTreeItem *)row)->ColumnText->Strings[add_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 2), file_text.Length() + 1);
            combo_index =
                behaviour_column->ComboIndexOf(((TElTreeItem *)row)->ColumnText->Strings[behaviour_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 2), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[behaviour_id_column->text_index],
                2), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[hp_column->text_index], 3),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[tp_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[low_damage_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[high_damage_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[hit_rate_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[evade_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[armor_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[r_damage_column->text_index], 1),
                file_text.Length() + 1);
            combo_index =
                attack_element_column->ComboIndexOf(((TElTreeItem *)row)->ColumnText->Strings[attack_element_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 2), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[attack_element_damage_column->text_index],
                2), file_text.Length() + 1);
            combo_index =
                weak_element_column->ComboIndexOf(((TElTreeItem *)row)->ColumnText->Strings[weak_element_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 2), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[weak_element_damage_column->text_index],
                2), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[level_column->text_index], 1),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[experience_column->text_index], 3),
                file_text.Length() + 1);
            ProgressBar1->LightValue = ProgressBar1->LightValue + 1;
            StepProgress();
            rows_in_file++;
            if (rows_in_file >= rows_per_file)
                break;
        }
        file_number++;
        String file_name = AppendFileNumber("dtn", file_number) + ".enf";
        String path = FormMain->config->GetExportPath() + file_name;
        if (!dry_run)
        {
            basic_ofstream<char, char_traits<char> > file;
            file.open(path.c_str(), std::ios::binary);
            if ((char)file.is_open())
            {
                file << file_text.c_str();
                file.close();
                written_files += file_name + ", ";
                files_saved++;
            }
            else
                export_error = 1;
        }
        if (grid->Items->Count == row_index)
            break;
    }
    return written_files;
}

// Writes the skill rows to dsl001.esf on, starting a new file at the row or size limit.
String TFormExport::ExportSkills()
{
    String written_files = "";
    TElTreeStringGrid *grid = FormMain->ElTreeStringGrid7;
    if (grid->Items->Count <= 0 && !FormMain->config->GetExportEmpty())
        return written_files;
    char reversed = false;
    if (grid->Items->Count > 0)
    {
        if (String(grid->Items->Item[0]->Text) != "1")
            reversed = true;
    }
    DataGridCol *name_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *gfx_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Gfx");
    DataGridCol *effect_gfx_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Effect Gfx");
    DataGridCol *chant_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Chant");
    DataGridCol *nature_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Nature");
    DataGridCol *reserve_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Reserve");
    DataGridCol *type_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Type");
    DataGridCol *target_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Target");
    DataGridCol *target_type_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Target Type");
    DataGridCol *target_time_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Target Time");
    DataGridCol *tp_cost_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Tp Cost");
    DataGridCol *sp_cost_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Sp Cost");
    DataGridCol *delay_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Delay");
    DataGridCol *max_level_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Max Lvl");
    DataGridCol *hp_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Hp+");
    DataGridCol *tp_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Tp+");
    DataGridCol *sp_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Sp+");
    DataGridCol *low_damage_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Dmg-lo");
    DataGridCol *high_damage_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Dmg-hi");
    DataGridCol *hit_rate_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Hitrate");
    DataGridCol *evade_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Evade");
    DataGridCol *armor_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Armor");
    DataGridCol *r_damage_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("R-dmg");
    DataGridCol *str_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Str");
    DataGridCol *int_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Int");
    DataGridCol *wis_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Wis");
    DataGridCol *agi_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Agi");
    DataGridCol *con_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Con");
    DataGridCol *cha_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Cha");
    DataGridCol *element_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Element");
    DataGridCol *element_damage_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Element.Dmg");
    DataGridCol *range_column = FormMain->skills_tab->grid_ui->data_grid->FindColumn("Range");
    String header = "";
    header = header.Insert("ESF", header.Length() + 1);
    header = header.Insert(EncodeInteger(FileSignature(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(Random(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(grid->Items->Count, 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(header_version, 1), header.Length() + 1);
    int row_index = 0;
    int file_number = 0;
    for (; grid->Items->Count >= row_index; row_index++)
    {
        String file_text = header;
        int rows_in_file = 0;
        for (; grid->Items->Count > row_index; row_index++)
        {
            int row;
            if (!reversed)
                row = (int)grid->Items->Item[row_index];
            if (reversed)
                row = (int)grid->Items->Item[grid->Items->Count - row_index - 1];
            if (String(row_index + 1) != String(((TElTreeItem *)row)->Text))
            {
                export_error = 2;
                return written_files;
            }
            int name_length = ((TElTreeItem *)row)->ColumnText->Strings[name_column->text_index].Length();
            int chant_length = ((TElTreeItem *)row)->ColumnText->Strings[chant_column->text_index].Length();
            // The name and the chant both count toward the size limit of the file.
            if (file_text.Length() + name_length + chant_length + 53 > file_size_limit)
            {
                row_index--;
                break;
            }
            file_text.Insert(EncodeInteger(name_length, 1), file_text.Length() + 1);
            file_text.Insert(EncodeInteger(chant_length, 1), file_text.Length() + 1);
            String name_text(((TElTreeItem *)row)->ColumnText->Strings[name_column->text_index]);
            String skill_name(((TElTreeItem *)row)->ColumnText->Strings[name_column->text_index]);
            if (name_length > 0)
                file_text.Insert(((TElTreeItem *)row)->ColumnText->Strings[name_column->text_index],
                    file_text.Length() + 1);
            if (chant_length > 0)
                file_text.Insert(((TElTreeItem *)row)->ColumnText->Strings[chant_column->text_index],
                    file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[gfx_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[effect_gfx_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[tp_cost_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[sp_cost_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[delay_column->text_index], 1),
                file_text.Length() + 1);
            combo_index =
                nature_column->ComboIndexOf(((TElTreeItem *)row)->ColumnText->Strings[nature_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 1), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[reserve_column->text_index], 1),
                file_text.Length() + 1);
            combo_index = type_column->ComboIndexOf(((TElTreeItem *)row)->ColumnText->Strings[type_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 3), file_text.Length() + 1);
            combo_index =
                element_column->ComboIndexOf(((TElTreeItem *)row)->ColumnText->Strings[element_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 1), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[element_damage_column->text_index],
                2), file_text.Length() + 1);
            combo_index =
                target_type_column->ComboIndexOf(((TElTreeItem *)row)->ColumnText->Strings[target_type_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 1), file_text.Length() + 1);
            combo_index =
                target_column->ComboIndexOf(((TElTreeItem *)row)->ColumnText->Strings[target_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 1), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[target_time_column->text_index], 1),
                file_text.Length() + 1);
            combo_index =
                range_column->ComboIndexOf(((TElTreeItem *)row)->ColumnText->Strings[range_column->text_index]);
            file_text.Insert(EncodeInteger(combo_index, 1), file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[max_level_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[low_damage_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[high_damage_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[hit_rate_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[evade_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[armor_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[r_damage_column->text_index], 1),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[hp_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[tp_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[sp_column->text_index], 1),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[str_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[int_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[wis_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[agi_column->text_index], 2),
                file_text.Length() + 1);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[con_column->text_index], 2),
                file_text.Length() + 1);
            String cha_text(((TElTreeItem *)row)->ColumnText->Strings[cha_column->text_index]);
            file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[cha_column->text_index], 2),
                file_text.Length() + 1);
            ProgressBar1->LightValue = ProgressBar1->LightValue + 1;
            StepProgress();
            rows_in_file++;
            if (rows_in_file >= rows_per_file)
                break;
        }
        file_number++;
        String file_name = AppendFileNumber("dsl", file_number) + ".esf";
        String path = FormMain->config->GetExportPath() + file_name;
        if (!dry_run)
        {
            basic_ofstream<char, char_traits<char> > file;
            file.open(path.c_str(), std::ios::binary);
            if ((char)file.is_open())
            {
                file << file_text.c_str();
                file.close();
                written_files += file_name + ", ";
                files_saved++;
            }
            else
                export_error = 1;
        }
        if (grid->Items->Count == row_index)
            break;
    }
    return written_files;
}

// Writes the shops, with their items and crafts, to serv_shops.epf.
String TFormExport::ExportShops()
{
    ShopValue *shop;
    String written_files = "";
    TElTreeStringGrid *grid = FormMain->ElTreeStringGrid9;
    if (grid->Items->Count <= 0 && !FormMain->config->GetExportEmpty())
        return written_files;
    char reversed = false;
    if (grid->Items->Count > 0)
    {
        if (String(grid->Items->Item[0]->Text) != "1")
            reversed = true;
    }
    DataGridCol *name_column = FormMain->shops_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *tag_column = FormMain->shops_tab->grid_ui->data_grid->FindColumn("Tag");
    DataGridCol *level_from_column = FormMain->shops_tab->grid_ui->data_grid->FindColumn("Req:Lvl.min");
    DataGridCol *level_to_column = FormMain->shops_tab->grid_ui->data_grid->FindColumn("Req:Lvl.max");
    DataGridCol *class_req_column = FormMain->shops_tab->grid_ui->data_grid->FindColumn("Req:Cls");
    String header = "";
    header = header.Insert("ESF", header.Length() + 1);
    header = header.Insert(EncodeInteger(FileSignature(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(Random(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(grid->Items->Count, 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(header_version, 1), header.Length() + 1);
    String file_text = header;
    for (int row_index = 0; grid->Items->Count > row_index; row_index++)
    {
        int row;
        if (!reversed)
            row = (int)grid->Items->Item[row_index];
        if (reversed)
            row = (int)grid->Items->Item[grid->Items->Count - row_index - 1];
        if (String(row_index + 1) != String(((TElTreeItem *)row)->Text))
        {
            export_error = 2;
            return written_files;
        }
        file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[tag_column->text_index], 2),
            file_text.Length() + 1);
        int name_length = ((TElTreeItem *)row)->ColumnText->Strings[name_column->text_index].Length();
        file_text.Insert(EncodeInteger(name_length, 1), file_text.Length() + 1);
        file_text.Insert(((TElTreeItem *)row)->ColumnText->Strings[name_column->text_index], file_text.Length() + 1);
        file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[level_from_column->text_index], 2),
            file_text.Length() + 1);
        file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[level_to_column->text_index], 2),
            file_text.Length() + 1);
        int class_id =
            class_req_column->LinkedText(((TElTreeItem *)row)->ColumnText->Strings[class_req_column->text_index], 1);
        file_text.Insert(EncodeInteger(class_id, 1), file_text.Length() + 1);
        int item_count = 0;
        int craft_count = 0;
        shop = FormMain->shops_tab->shop_values->GetShop(row_index);
        // The shop's items, then its crafts with four ingredients each, follow its row.
        if (shop)
        {
            item_count = shop->trades.size();
            craft_count = shop->crafts.size();
        }
        file_text.Insert(EncodeNumber(item_count, 2), file_text.Length() + 1);
        file_text.Insert(EncodeNumber(craft_count, 1), file_text.Length() + 1);
        if (shop)
        {
            for (int item_index = 0; item_index < shop->trades.size(); item_index++)
            {
                ShopItemVal shop_item = shop->trades[item_index];
                file_text.Insert(EncodeNumber(shop_item.item_id, 2), file_text.Length() + 1);
                file_text.Insert(EncodeNumber(shop_item.buy_price, 3), file_text.Length() + 1);
                file_text.Insert(EncodeNumber(shop_item.sell_price, 3), file_text.Length() + 1);
                file_text.Insert(EncodeNumber(shop_item.max_amount, 1), file_text.Length() + 1);
            }
            for (int craft_index = 0; craft_index < shop->crafts.size(); craft_index++)
            {
                ShopCraftVal craft = shop->crafts[craft_index];
                file_text.Insert(EncodeNumber(craft.id, 2), file_text.Length() + 1);
                file_text.Insert(EncodeNumber(craft.ingredient_item_ids[0], 2), file_text.Length() + 1);
                file_text.Insert(EncodeNumber(craft.ingredient_amounts[0], 1), file_text.Length() + 1);
                file_text.Insert(EncodeNumber(craft.ingredient_item_ids[1], 2), file_text.Length() + 1);
                file_text.Insert(EncodeNumber(craft.ingredient_amounts[1], 1), file_text.Length() + 1);
                file_text.Insert(EncodeNumber(craft.ingredient_item_ids[2], 2), file_text.Length() + 1);
                file_text.Insert(EncodeNumber(craft.ingredient_amounts[2], 1), file_text.Length() + 1);
                file_text.Insert(EncodeNumber(craft.ingredient_item_ids[3], 2), file_text.Length() + 1);
                file_text.Insert(EncodeNumber(craft.ingredient_amounts[3], 1), file_text.Length() + 1);
            }
        }
        ProgressBar1->LightValue = ProgressBar1->LightValue + 1;
        StepProgress();
    }
    String file_name = "serv_shops.epf";
    String path = FormMain->config->GetExportPath() + file_name;
    if (!dry_run)
    {
        basic_ofstream<char, char_traits<char> > file;
        file.open(path.c_str(), std::ios::binary);
        if ((char)file.is_open())
        {
            file << file_text.c_str();
            file.close();
            written_files += file_name + ", ";
            files_saved++;
        }
        else
            export_error = 1;
    }
    return written_files;
}

// Writes the trainers, with their skills, to serv_trainers.epf.
String TFormExport::ExportTrainers()
{
    String created = "";
    TElTreeStringGrid *grid = FormMain->ElTreeStringGrid11;
    if (grid->Items->Count <= 0 && !FormMain->config->GetExportEmpty())
        return created;
    char trainer_index = 0;
    // A grid sorted from high to low IDs is written from its last row up.
    if (grid->Items->Count > 0)
    {
        if (String(grid->Items->Item[0]->Text) != "1")
            trainer_index = 1;
    }
    DataGridCol *name_length = FormMain->trainers_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *tag_column = FormMain->trainers_tab->grid_ui->data_grid->FindColumn("Tag");
    DataGridCol *level_from_column = FormMain->trainers_tab->grid_ui->data_grid->FindColumn("Req:Lvl.min");
    DataGridCol *level_to_column = FormMain->trainers_tab->grid_ui->data_grid->FindColumn("Req:Lvl.max");
    DataGridCol *class_req_column = FormMain->trainers_tab->grid_ui->data_grid->FindColumn("Req:Cls");
    String header = "";
    header = header.Insert("ETF", header.Length() + 1);
    header = header.Insert(EncodeInteger(FileSignature(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(Random(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(grid->Items->Count, 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(header_version, 1), header.Length() + 1);
    String data = header;
    for (int row_index = 0; grid->Items->Count > row_index; row_index++)
    {
        int row;
        if (!trainer_index)
            row = (int)grid->Items->Item[row_index];
        if (trainer_index)
            row = (int)grid->Items->Item[grid->Items->Count - row_index - 1];
        if (String(row_index + 1) != String(((TElTreeItem *)row)->Text))
        {
            export_error = 2;
            return created;
        }
        data.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[tag_column->text_index], 2),
            data.Length() + 1);
        int name_size = ((TElTreeItem *)row)->ColumnText->Strings[name_length->text_index].Length();
        data.Insert(EncodeInteger(name_size, 1), data.Length() + 1);
        data.Insert(((TElTreeItem *)row)->ColumnText->Strings[name_length->text_index], data.Length() + 1);
        data.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[level_from_column->text_index], 2),
            data.Length() + 1);
        data.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[level_to_column->text_index], 2),
            data.Length() + 1);
        int class_requirement =
            class_req_column->LinkedText(((TElTreeItem *)row)->ColumnText->Strings[class_req_column->text_index], 1);
        data.Insert(EncodeInteger(class_requirement, 1), data.Length() + 1);
        int skill_count = 0;
        LearnValue *trainer = FormMain->trainers_tab->trainer_values->Insert(row_index);
        if (trainer)
            skill_count = trainer->skills.size();
        data.Insert(EncodeNumber(skill_count, 2), data.Length() + 1);
        if (trainer)
        {
            for (int skill_index = 0; skill_index < trainer->skills.size(); skill_index++)
            {
                LearnItemVal skill = trainer->skills[skill_index];
                data.Insert(EncodeNumber(skill.id, 2), data.Length() + 1);
                data.Insert(EncodeNumber(String(skill.level_requirement), 2), data.Length() + 1);
                data.Insert(EncodeNumber(String(skill.class_requirement), 1), data.Length() + 1);
                data.Insert(EncodeNumber(skill.price, 3), data.Length() + 1);
                data.Insert(EncodeNumber(String(skill.skill_requirement_1), 2), data.Length() + 1);
                data.Insert(EncodeNumber(String(skill.skill_requirement_2), 2), data.Length() + 1);
                data.Insert(EncodeNumber(String(skill.skill_requirement_3), 2), data.Length() + 1);
                data.Insert(EncodeNumber(String(skill.skill_requirement_4), 2), data.Length() + 1);
                data.Insert(EncodeNumber(String(skill.str_requirement), 2), data.Length() + 1);
                data.Insert(EncodeNumber(String(skill.wis_requirement), 2), data.Length() + 1);
                data.Insert(EncodeNumber(String(skill.int_requirement), 2), data.Length() + 1);
                data.Insert(EncodeNumber(String(skill.agi_requirement), 2), data.Length() + 1);
                data.Insert(EncodeNumber(String(skill.con_requirement), 2), data.Length() + 1);
                data.Insert(EncodeNumber(String(skill.cha_requirement), 2), data.Length() + 1);
            }
        }
        ProgressBar1->LightValue = ProgressBar1->LightValue + 1;
        StepProgress();
    }
    String file_name = "serv_trainers.epf";
    String path = FormMain->config->GetExportPath() + file_name;
    if (!dry_run)
    {
        basic_ofstream<char, char_traits<char> > file;
        file.open(path.c_str(), std::ios::binary);
        if ((char)file.is_open())
        {
            file << data.c_str();
            file.close();
            created += file_name + ", ";
            files_saved++;
        }
        else
            export_error = 1;
    }
    return created;
}

// Writes the inns, with their quiz questions and answers, to serv_inns.epf.
String TFormExport::ExportInns()
{
    int answer_length;
    String written_files = "";
    TElTreeStringGrid *grid = FormMain->ElTreeStringGrid13;
    if (grid->Items->Count <= 0 && !FormMain->config->GetExportEmpty())
        return written_files;
    char reversed = false;
    if (grid->Items->Count > 0)
    {
        if (String(grid->Items->Item[0]->Text) != "1")
            reversed = true;
    }
    DataGridCol *name_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Name");
    DataGridCol *tag_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Tag");
    DataGridCol *cost_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Cost");
    DataGridCol *sleep_map_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Sleep Map");
    DataGridCol *sleep_x_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Sleep X");
    DataGridCol *sleep_y_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Sleep Y");
    DataGridCol *home_map_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Home Map");
    DataGridCol *home_x_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Home X");
    DataGridCol *home_y_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Home Y");
    DataGridCol *high_level_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Hi-Level");
    DataGridCol *high_map_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Hi Map");
    DataGridCol *high_x_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Hi X");
    DataGridCol *high_y_column = FormMain->inns_tab->grid_ui->data_grid->FindColumn("Hi Y");
    String header = "";
    header = header.Insert("EID", header.Length() + 1);
    header = header.Insert(EncodeInteger(FileSignature(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(Random(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(grid->Items->Count, 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(header_version, 1), header.Length() + 1);
    String file_text = header;
    for (int row_index = 0; grid->Items->Count > row_index; row_index++)
    {
        int row;
        if (!reversed)
            row = (int)grid->Items->Item[row_index];
        if (reversed)
            row = (int)grid->Items->Item[grid->Items->Count - row_index - 1];
        if (String(row_index + 1) != String(((TElTreeItem *)row)->Text))
        {
            export_error = 2;
            return written_files;
        }
        file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[tag_column->text_index], 2),
            file_text.Length() + 1);
        int name_length = ((TElTreeItem *)row)->ColumnText->Strings[name_column->text_index].Length();
        file_text.Insert(EncodeInteger(name_length, 1), file_text.Length() + 1);
        file_text.Insert(((TElTreeItem *)row)->ColumnText->Strings[name_column->text_index], file_text.Length() + 1);
        file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[cost_column->text_index], 3),
            file_text.Length() + 1);
        file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[sleep_map_column->text_index], 2),
            file_text.Length() + 1);
        file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[sleep_x_column->text_index], 1),
            file_text.Length() + 1);
        file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[sleep_y_column->text_index], 1),
            file_text.Length() + 1);
        file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[home_map_column->text_index], 2),
            file_text.Length() + 1);
        file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[home_x_column->text_index], 1),
            file_text.Length() + 1);
        file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[home_y_column->text_index], 1),
            file_text.Length() + 1);
        file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[high_level_column->text_index], 2),
            file_text.Length() + 1);
        file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[high_map_column->text_index], 2),
            file_text.Length() + 1);
        file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[high_x_column->text_index], 1),
            file_text.Length() + 1);
        file_text.Insert(EncodeNumber(((TElTreeItem *)row)->ColumnText->Strings[high_y_column->text_index], 1),
            file_text.Length() + 1);
        int question_count = 0;
        InnValue *inn = FormMain->inns_tab->inn_values->GetInn(row_index);
        // Each question and its answer are written trimmed, length first.
        if (inn)
            question_count = inn->questions.size();
        file_text.Insert(EncodeNumber(question_count, 1), file_text.Length() + 1);
        if (inn)
        {
            for (int question_index = 0; question_index < inn->questions.size(); question_index++)
            {
                InnQuizItem question = inn->questions[question_index];
                question.question = question.question.Trim();
                question.answer = question.answer.Trim();
                int question_length = question.question.Length();
                question_length = question_length;
                file_text.Insert(EncodeInteger(question_length, 1), file_text.Length() + 1);
                file_text.Insert(question.question, file_text.Length() + 1);
                answer_length = question.answer.Length();
                file_text.Insert(EncodeInteger(answer_length, 1), file_text.Length() + 1);
                file_text.Insert(question.answer, file_text.Length() + 1);
            }
        }
        ProgressBar1->LightValue = ProgressBar1->LightValue + 1;
        StepProgress();
    }
    String file_name = "serv_inns.epf";
    String path = FormMain->config->GetExportPath() + file_name;
    if (!dry_run)
    {
        basic_ofstream<char, char_traits<char> > file;
        file.open(path.c_str(), std::ios::binary);
        if ((char)file.is_open())
        {
            file << file_text.c_str();
            file.close();
            written_files += file_name + ", ";
            files_saved++;
        }
        else
            export_error = 1;
    }
    return written_files;
}

// Writes the chat lines of every npc that has any to serv_chats.epf.
String TFormExport::ExportChats()
{
    String created = "";
    if (FormMain->npcs_tab->adapter->chat_npc_count <= 0 && !FormMain->config->GetExportEmpty())
        return created;
    String header = "";
    header = header.Insert("ETF", header.Length() + 1);
    header = header.Insert(EncodeInteger(FileSignature(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(Random(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(FormMain->npcs_tab->adapter->chat_npc_count, 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(header_version, 1), header.Length() + 1);
    String data = header;
    for (int npc_index = 0; npc_index < (unsigned int)FormMain->npcs_tab->npc_values->Count(); npc_index++)
    {
        NpcValue *npc = FormMain->npcs_tab->npc_values->GetNpc(npc_index);
        if (npc->talk_lines.size() >= 1)
        {
            data.Insert(EncodeInteger(npc->npc_id, 2), data.Length() + 1);
            data.Insert(EncodeInteger(npc->talk_interval, 1), data.Length() + 1);
            data.Insert(EncodeInteger(npc->talk_rate, 1), data.Length() + 1);
            data.Insert(EncodeInteger(npc->talk_lines.size(), 1), data.Length() + 1);
            for (int line_index = 0; line_index < npc->talk_lines.size(); line_index++)
            {
                String line = npc->talk_lines[line_index].Trim();
                int line_length = line.Length();
                data.Insert(EncodeInteger(line_length, 1), data.Length() + 1);
                if (line_length > 0)
                    data.Insert(line, data.Length() + 1);
                ProgressBar1->LightValue = ProgressBar1->LightValue + 1;
                StepProgress();
            }
        }
    }
    String file_name = "serv_chats.epf";
    String path = FormMain->config->GetExportPath() + file_name;
    if (!dry_run)
    {
        basic_ofstream<char, char_traits<char> > file;
        file.open(path.c_str(), std::ios::binary);
        if ((char)file.is_open())
        {
            file << data.c_str();
            file.close();
            created += file_name + ", ";
            files_saved++;
        }
        else
            export_error = 1;
    }
    return created;
}

// Writes the drops of every npc that has any to serv_drops.epf.
String TFormExport::ExportDrops()
{
    String created = "";
    if (FormMain->npcs_tab->adapter->drop_npc_count <= 0 && !FormMain->config->GetExportEmpty())
        return created;
    String header = "";
    header = header.Insert("EDF", header.Length() + 1);
    header = header.Insert(EncodeInteger(FileSignature(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(Random(), 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(FormMain->npcs_tab->adapter->drop_npc_count, 2), header.Length() + 1);
    header = header.Insert(EncodeInteger(header_version, 1), header.Length() + 1);
    String data = header;
    for (int npc_index = 0; npc_index < (unsigned int)FormMain->npcs_tab->npc_values->Count(); npc_index++)
    {
        NpcValue *npc = FormMain->npcs_tab->npc_values->GetNpc(npc_index);
        if (npc->drops.size() >= 1)
        {
            data.Insert(EncodeInteger(npc->npc_id, 2), data.Length() + 1);
            data.Insert(EncodeInteger(npc->drops.size(), 2), data.Length() + 1);
            for (int drop_index = 0; drop_index < npc->drops.size(); drop_index++)
            {
                NpcDropItem drop = npc->drops[drop_index];
                data.Insert(EncodeInteger(drop.item_id, 2), data.Length() + 1);
                data.Insert(EncodeInteger(drop.min_amount, 3), data.Length() + 1);
                data.Insert(EncodeInteger(drop.max_amount, 3), data.Length() + 1);
                data.Insert(EncodeInteger(drop.rate, 2), data.Length() + 1);
                ProgressBar1->LightValue = ProgressBar1->LightValue + 1;
                StepProgress();
            }
        }
    }
    String file_name = "serv_drops.epf";
    String path = FormMain->config->GetExportPath() + file_name;
    if (!dry_run)
    {
        basic_ofstream<char, char_traits<char> > file;
        file.open(path.c_str(), std::ios::binary);
        if ((char)file.is_open())
        {
            file << data.c_str();
            file.close();
            created += file_name + ", ";
            files_saved++;
        }
        else
            export_error = 1;
    }
    return created;
}

String TFormExport::AppendFileNumber(String name, int number)
{
    if (number < 10)
        name += "00" + IntToStr(number);
    else if (number < 100)
        name += "0" + IntToStr(number);
    else if (number < 1000)
        name += IntToStr(number);
    return name;
}

// The number in the text as the bytes of a pub file; empty text counts as 0.
String TFormExport::EncodeNumber(String text, int width)
{
    if (text == "")
        text = "0";
    return EncodeInteger(StrToInt(text), width);
}

// The number as the bytes of a pub file, low digit first, padded to the width.
String TFormExport::EncodeInteger(unsigned int value, int width)
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
    // Missing digits are written as 254, which a reader takes as the end of the number.
    if (result.Length() < width)
    {
        char pad = -2;
        int missing = width - result.Length();
        for (i = 0; i < missing; i++)
            result.Insert(pad, result.Length() + 1);
    }
    return result;
}

// Escape or End closes the form; Enter starts the export.
void __fastcall TFormExport::FormKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE || Key == VK_END)
        ((TCustomForm *)this)->Close();
    if (Key == VK_RETURN)
    {
        if (buttonOk->Visible)
            buttonOkClick((TObject *)buttonOk);
        else
            buttonOkClick((TObject *)buttonOk);
    }
}

// Flashes the save button a few times.
void __fastcall TFormExport::Timer1Timer(TObject *Sender)
{
    flash_ticks++;
    if (flash_ticks % 2 == 0)
    {
        if (FormMain->config->GetTheme() == "dark")
            buttonOk->Font->Color = (TColor)0x222222;
        if (FormMain->config->GetTheme() != "dark")
            buttonOk->Font->Color = (TColor)0x2E2F13;
    }
    if (flash_ticks % 2 == 1)
    {
        if (FormMain->config->GetTheme() == "dark")
            buttonOk->Font->Color = (TColor)0xDDD164;
        if (FormMain->config->GetTheme() != "dark")
            buttonOk->Font->Color = (TColor)0x808040;
    }
    if (flash_ticks > 5 && FormMain->config->GetTheme() != "dark")
        Timer1->Enabled = false;
    if (flash_ticks > 6 && FormMain->config->GetTheme() == "dark")
    {
        buttonOk->Font->Color = (TColor)0xCCCCCC;
        Timer1->Enabled = false;
    }
}

// The number that signs every exported file, or a new random one.
int TFormExport::FileSignature()
{
    if (shared_signature)
        return signature;
    return random(63980) + 10;
}

// A random number that differs from the files' signature.
int TFormExport::Random()
{
    int number = random(63980) + 10;
    if (number == signature)
        number++;
    return number;
}

void __fastcall TFormExport::buttonCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

void UnitExportForm_UnusedLiterals1()
{
    "eof";
}

// Sorts every grid by ID and starts the export; expert mode can lift the file limits.
void __fastcall TFormExport::buttonOkClick(TObject *Sender)
{
    if (FormMain->config->GetExpertMode() && checkWriteOversized->Checked)
    {
        header_version = 0;
        rows_per_file = 99999;
        file_size_limit = 900000000;
    }
    buttonCancel->Left = 330;
    buttonCancel->Caption = WideString("DONE");
    buttonOk->Visible = false;
    buttonOk->Enabled = false;
    FormMain->items_tab->grid_ui->SortById();
    FormMain->npcs_tab->grid_ui->SortById();
    FormMain->skills_tab->grid_ui->SortById();
    FormMain->classes_tab->grid_ui->SortById();
    FormMain->shops_tab->grid_ui->SortById();
    FormMain->trainers_tab->grid_ui->SortById();
    FormMain->inns_tab->grid_ui->SortById();
    panelDifferent->Visible = false;
    Timer2->Enabled = true;
    slide_stage = 1;
    slide_top = panelWorking->Top;
}

// Escape or End closes the export.
void __fastcall TFormExport::buttonTabCatchKeyDown(TObject *Sender, unsigned short &Key, TShiftState Shift)
{
    if (Key == VK_ESCAPE || Key == VK_END)
        ((TCustomForm *)this)->Close();
}

// Hides the note that the export folder is not the pub folder.
void __fastcall TFormExport::Timer3Timer(TObject *Sender)
{
    panelDifferent->Visible = false;
    Timer3->Enabled = false;
}
