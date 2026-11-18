#include <vcl.h>
#pragma hdrstop

#include "Datagridcol.h"
#include "Datacomboitem.h"
#include "Datalinkitem.h"

#pragma package(smart_init)

// A column with its header, caption, width, kind, hint, limits, group, alignment and text format.
DataGridCol::DataGridCol(String column_name, String column_caption, short column_width, char column_kind,
    String column_hint, int column_min, int column_max, char column_group, char column_alignment, char column_format)
{
    header_text = column_name;
    caption_text = column_caption;
    kind = column_kind;
    alignment = column_alignment;
    hint_text = column_hint;
    max_value = column_max;
    text_format = 1;
    width = column_width;
    min_value = column_min;
    group = column_group;
    text_format = column_format;
    is_out_of_bounds = false;
    is_link_out_of_bounds = false;
    sub_name = "";
    parent = 0;
    disabled = false;
}

DataGridCol::~DataGridCol()
{
}

unsigned char DataGridCol::ComboItemCount()
{
    return combo_items.size();
}

// Adds a choice to the column's list; the choice's limit is the column's highest value.
DataComboItem *DataGridCol::AddComboItem(int id, String text)
{
    DataComboItem *item = new DataComboItem(combo_items.size());
    item->text = text;
    item->max_value = max_value;
    item->id = id;
    item->group_count = 0;
    combo_items.insert(combo_items.end(), item);
    return item;
}

// Adds a choice that shows the columns of one group.
DataComboItem *DataGridCol::AddComboItemWithGroup(int id, String text, int first_group)
{
    DataComboItem *item = new DataComboItem(combo_items.size());
    item->text = text;
    item->max_value = max_value;
    item->id = id;
    item->groups[0] = first_group;
    item->group_count = 1;
    combo_items.insert(combo_items.end(), item);
    return item;
}

DataComboItem *DataGridCol::AddComboItemWithTwoGroups(int id, String text, int first_group, int second_group)
{
    DataComboItem *item = new DataComboItem(combo_items.size());
    item->text = text;
    item->max_value = max_value;
    item->id = id;
    item->groups[0] = first_group;
    item->second_group = second_group;
    item->group_count = 2;
    combo_items.insert(combo_items.end(), item);
    return item;
}

// Adds a choice that shows the columns of three groups.
DataComboItem *DataGridCol::AddComboItemWithThreeGroups(int id, String text, int first_group, int second_group,
    int third_group)
{
    DataComboItem *item = new DataComboItem(combo_items.size());
    item->text = text;
    item->max_value = max_value;
    item->id = id;
    item->groups[0] = first_group;
    item->second_group = second_group;
    item->third_group = third_group;
    item->group_count = 3;
    combo_items.insert(combo_items.end(), item);
    return item;
}

// Adds a choice that shows the columns of four groups.
DataComboItem *DataGridCol::AddComboItemWithFourGroups(int id, String text, int first_group, int second_group,
    int third_group,
    int fourth_group)
{
    DataComboItem *item = new DataComboItem(combo_items.size());
    item->text = text;
    item->max_value = max_value;
    item->id = id;
    item->groups[0] = first_group;
    item->second_group = second_group;
    item->third_group = third_group;
    item->fourth_group = fourth_group;
    item->group_count = 4;
    combo_items.insert(combo_items.end(), item);
    return item;
}

// Adds a choice that shows the columns of five groups.
DataComboItem *DataGridCol::AddComboItemWithFiveGroups(int id, String text, int first_group, int second_group,
    int third_group,
    int fourth_group, int fifth_group)
{
    DataComboItem *item = new DataComboItem(combo_items.size());
    item->text = text;
    item->max_value = max_value;
    item->id = id;
    item->groups[0] = first_group;
    item->second_group = second_group;
    item->third_group = third_group;
    item->fourth_group = fourth_group;
    item->fifth_group = fifth_group;
    item->group_count = 5;
    combo_items.insert(combo_items.end(), item);
    return item;
}

// Adds a choice that shows the columns of seven groups.
DataComboItem *DataGridCol::AddComboItemWithSevenGroups(int id, String text, int first_group, int second_group,
    int third_group,
    int fourth_group, int fifth_group, int sixth_group, int seventh_group)
{
    DataComboItem *item = new DataComboItem(combo_items.size());
    item->text = text;
    item->max_value = max_value;
    item->id = id;
    item->groups[0] = first_group;
    item->second_group = second_group;
    item->third_group = third_group;
    item->fourth_group = fourth_group;
    item->fifth_group = fifth_group;
    item->sixth_group = sixth_group;
    item->seventh_group = seventh_group;
    item->group_count = 7;
    combo_items.insert(combo_items.end(), item);
    return item;
}

// Adds a choice that shows the columns of eight groups.
DataComboItem *DataGridCol::AddComboItemWithEightGroups(int id, String text, int first_group, int second_group,
    int third_group,
    int fourth_group, int fifth_group, int sixth_group, int seventh_group, int eighth_group)
{
    DataComboItem *item = new DataComboItem(combo_items.size());
    item->text = text;
    item->max_value = max_value;
    item->id = id;
    item->groups[0] = first_group;
    item->second_group = second_group;
    item->third_group = third_group;
    item->fourth_group = fourth_group;
    item->fifth_group = fifth_group;
    item->sixth_group = sixth_group;
    item->seventh_group = seventh_group;
    item->eighth_group = eighth_group;
    item->group_count = 8;
    combo_items.insert(combo_items.end(), item);
    return item;
}

// Adds a choice that shows the columns of nine groups.
DataComboItem *DataGridCol::AddComboItemWithNineGroups(int id, String text, int first_group, int second_group,
    int third_group,
    int fourth_group, int fifth_group, int sixth_group, int seventh_group, int eighth_group, int ninth_group)
{
    DataComboItem *item = new DataComboItem(combo_items.size());
    item->text = text;
    item->max_value = max_value;
    item->id = id;
    item->groups[0] = first_group;
    item->second_group = second_group;
    item->third_group = third_group;
    item->fourth_group = fourth_group;
    item->fifth_group = fifth_group;
    item->sixth_group = sixth_group;
    item->seventh_group = seventh_group;
    item->eighth_group = eighth_group;
    item->ninth_group = ninth_group;
    item->group_count = 9;
    combo_items.insert(combo_items.end(), item);
    return item;
}

DataComboItem *DataGridCol::GetComboItem(short index)
{
    if (index < 0)
        return 0;
    if (combo_items.size() <= index)
        return 0;
    return combo_items[index];
}

// Finds a choice by its text in any case: the same text, then one holding it, then by the number in it.
DataComboItem *DataGridCol::FindComboItem(String text)
{
    text = LowerCase(text);
    vector<DataComboItem *>::iterator exact_entry;
    for (exact_entry = combo_items.begin(); exact_entry != combo_items.end(); exact_entry++)
    {
        DataComboItem *exact_item = *exact_entry;
        String exact_text = LowerCase(exact_item->text);
        if (exact_text == text)
            return exact_item;
    }
    vector<DataComboItem *>::iterator partial_entry;
    for (partial_entry = combo_items.begin(); partial_entry != combo_items.end(); partial_entry++)
    {
        DataComboItem *partial_item = *partial_entry;
        String partial_text = LowerCase(partial_item->text);
        if (partial_text.Pos(text) > 0)
            return partial_item;
    }
    String digits = "";
    for (int i = 1; i <= text.Length(); i++)
    {
        if (text[i] >= '0' && text[i] <= '9')
            digits = digits + text[i];
    }
    if (digits.Length() > 0)
        return GetComboItem(StrToInt(digits));
    return 0;
}

// The index of the choice the text names; an [OOB-n] mark gives its number, kept to the highest value; else 0.
int DataGridCol::ComboIndexOf(String text)
{
    int out_of_bounds = ParseOutOfBounds(text);
    if (out_of_bounds >= 0)
    {
        if ((unsigned int)max_value < out_of_bounds)
            out_of_bounds = max_value;
        return out_of_bounds;
    }
    text = LowerCase(text);
    vector<DataComboItem *>::iterator entry;
    for (entry = combo_items.begin(); entry != combo_items.end(); entry++)
    {
        DataComboItem *item = *entry;
        String item_text = LowerCase(item->text);
        if (item_text == text)
            return item->index;
        if (item_text.Pos(text) > 0 && text.Length() > 2)
            return item->index;
    }
    return 0;
}

// The number of an [OOB-n] mark; -1 when the text is not one.
int DataGridCol::ParseOutOfBounds(String text)
{
    if (text.Length() <= 6)
        return -1;
    if (text[1] != '[')
        return -1;
    if (text.Length() >= 15)
        return -1;
    if (text.Pos("]") <= 0)
        return -1;
    if (text.Pos("[OOB-") <= 0)
        return -1;
    int i = 6;
    int length = text.Length();
    String digits = "";
    for (; i < length; i++)
    {
        if (text[i] >= '0' && text[i] <= '9')
            digits += text[i];
    }
    if (!digits.Length())
        return -1;
    return StrToInt(digits);
}

// When asked, widens the column and puts each choice's number before its text.
void DataGridCol::WidenForComboItems(char wider)
{
    DataComboItem *item;
    if (!wider)
        return;
    alignment = 0;
    width += 25;
    for (vector<DataComboItem *>::iterator entry = combo_items.begin(); entry != combo_items.end(); entry++)
    {
        item = *entry;
        item->text = NumberedText(item->index, item->text, combo_items.size());
    }
}

unsigned char DataGridCol::LinkCount()
{
    return links.size();
}

// Links the column to a column of another grid, named by its header.
DataLinkItem *DataGridCol::AddLink(TElTreeStringGrid *grid, String column_name)
{
    DataLinkItem *link = new DataLinkItem(grid, column_name);
    links.insert(links.end(), link);
    return link;
}

DataLinkItem *DataGridCol::GetLink(short index)
{
    if (index < 0)
        return 0;
    if (links.size() <= index)
        return 0;
    return links[index];
}

// The row of the linked grid whose linked column holds the text, plus the start; 0 when none does.
int DataGridCol::LinkedText(String id_text, unsigned char link_index)
{
    if (!LinkCount())
        return 0;
    DataLinkItem *link = GetLink(0);
    TElTreeStringGrid *grid = link->grid;
    for (int row = 0; row < grid->Items->Count; row++)
    {
        int item = (int)grid->Items->Item[row];
        if (((TElTreeItem *)item)->ColumnText->Strings[link->FindColumnIndex()] == id_text)
            return link_index + row;
    }
    return 0;
}

char DataGridCol::SubColumnCount()
{
    return sub_columns.size();
}

// Adds a variant of the column for one index value, with its own caption.
DataGridCol *DataGridCol::AddSubColumn(String name, String caption)
{
    return AddSubColumnWithHint(name, caption, "");
}

DataGridCol *DataGridCol::AddSubColumnWithHint(String name, String caption, String hint)
{
    DataGridCol *sub_column =
        new DataGridCol(header_text, caption_text, width, kind, hint_text, min_value, max_value, group, alignment,
            text_format);
    sub_column->sub_name = name;
    sub_column->index = index;
    sub_column->parent = this;
    if (caption.Length() > 0)
        sub_column->caption_text = caption;
    if (hint.Length() > 0)
        sub_column->hint_text = hint;
    sub_columns.insert(sub_columns.end(), sub_column);
    return sub_column;
}

// The column's variant for the index value; none when it has no such variant.
DataGridCol *DataGridCol::FindSubColumn(String name)
{
    vector<DataGridCol *>::iterator entry;
    if (!SubColumnCount())
        return 0;
    if (name == "")
        return 0;
    for (entry = sub_columns.begin(); entry != sub_columns.end(); entry++)
    {
        if (name == (*entry)->sub_name)
            return *entry;
    }
    return 0;
}

// The choice's text after its number, the number padded with zeros to the width of the count.
String DataGridCol::NumberedText(int number, String text, int count)
{
    if (count < 10)
        return IntToStr(number) + ": " + text;
    if (count < 100)
    {
        if (number < 10)
            return "0" + IntToStr(number) + ": " + text;
        if (number < 100)
            return IntToStr(number) + ": " + text;
    }
    if (count < 1000)
    {
        if (number < 10)
            return "00" + IntToStr(number) + ": " + text;
        if (number < 100)
            return "0" + IntToStr(number) + ": " + text;
        if (number < 1000)
            return IntToStr(number) + ": " + text;
    }
    return "err: ";
}
