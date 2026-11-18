#include <vcl.h>
#pragma hdrstop

#include "Filethemes.h"
#include <Classes.hpp>
#include "Configuration.h"
#include <ctype.h>
#include <io.h>

#pragma package(smart_init)

// Starts with the default colour of every text and box, for the theme read from folder for this client version.
FileThemes::FileThemes(String folder, int major_version, int minor_version)
{
    theme_folder = folder;
    client_major_version = major_version;
    client_minor_version = minor_version;
    is_loaded = false;
    unused_flag = false;
    second_unused_flag = false;
    color_menu = 0xDCDCDC;
    color_menu_version = 0x96AABE;
    color_edits = 0;
    color_chat_edit = 0;
    color_top_bar = 0xC8C8C8;
    color_bottom_bar = 0;
    color_dialog = 0xC8C8C8;
    color_dialog_soft = 0xB4B4B4;
    color_dialog_caption = 0xC8F0F0;
    color_dialog_link_over = 0xDCDCDC;
    color_dialog_link = 0xB4B4B4;
    color_popup_caption = 0xC8F0F0;
    color_popup = 0xC8DCDC;
    color_popup_edits = 0xB4C8DC;
    color_locker_item = 0x648282;
    color_locker_item_highlight = 0xBEBEC8;
    color_panel = 0xC8C8C8;
    color_panel_list = 0;
    color_chat_default = 0;
    color_chat_private = 0xA5C8CD;
    color_chat_system = 0xBBE3E3;
    color_chat_warning = 0x505AF0;
    color_chat_global = 0xBBC3C4;
    color_chat_admin = 0xBBD7E2;
    color_chat_tab = 0;
    color_chat_tab_active = 0xFFFFFF;
    color_chat_tab_highlight = 0xAAC8DC;
    String stem = "00";
    ini_file_name = stem + "0.ini";
    first_graphics_name = stem + "1.egf";
    second_graphics_name = stem + "2.egf";
}

FileThemes::~FileThemes()
{
}

FileTheme::~FileTheme()
{
}

void Filethemes_UnusedLiterals1()
{
    ".";
    "";
}

// The file name without the extension when it ends with it; empty otherwise, and for . and ..
String FileThemes::StripExtension(String file_name, String extension)
{
    if (file_name == ".")
        return "";
    if (file_name == "..")
        return "";
    if (file_name.Pos(extension) > 0)
    {
        if (file_name.SubString(file_name.Length() - (extension.Length() - 1), extension.Length()) == extension)
        {
            file_name.Delete(file_name.Length() - (extension.Length() - 1), extension.Length());
            return file_name;
        }
    }
    return "";
}

bool FileThemes::IsKey(String other_key)
{
    if (key != other_key)
        return false;
    return true;
}

// Reads the value of the key as a number of digits only; false for another key or value.
bool FileThemes::ReadIntValue(String name)
{
    if (key != name)
        return false;
    for (int i = 1; i <= upper_value.Length(); i++)
    {
        if (upper_value[i] < '0' || upper_value[i] > '9')
            return false;
    }
    int_value = StrToInt(upper_value);
    return true;
}

// Reads the colour of the named key, written as red, green and blue numbers split by commas or as six hex digits;
// false for another key.
bool FileThemes::ReadColorValue(String name)
{
    if (key != name)
        return false;
    unsigned char commas = 0;
    unsigned char red = 0;
    unsigned char green = 0;
    unsigned char blue = 0;
    // A colour is three numbers with commas, or six hex digits.
    if (upper_value.Pos(",") > 0)
    {
        String red_text = "";
        String green_text = "";
        String blue_text = "";
        for (int i = 1; i <= upper_value.Length(); i++)
        {
            if (upper_value[i] == ',')
                commas++;
            else if (upper_value[i] >= '0' && upper_value[i] <= '9')
            {
                if (!commas)
                    red_text += upper_value[i];
                if (commas == 1)
                    green_text += upper_value[i];
                if (commas == 2)
                    blue_text += upper_value[i];
                if (commas >= 3)
                    break;
            }
        }
        if (red_text.Length() > 0)
            red = StrToInt(red_text);
        else
            return false;
        if (green_text.Length() > 0)
            green = StrToInt(green_text);
        else
            return false;
        if (blue_text.Length() > 0)
            blue = StrToInt(blue_text);
        else
            return false;
    }
    else
    {
        String hex_text = "";
        for (int j = 1; j <= upper_value.Length() && j <= 8; j++)
        {
            if (upper_value[j] != '#')
            {
                if (upper_value[j] >= '0' && upper_value[j] <= '9')
                    hex_text += upper_value[j];
                if (upper_value[j] >= 'A' && upper_value[j] <= 'F')
                    hex_text += upper_value[j];
                if (upper_value[j] >= 'a' && upper_value[j] <= 'f')
                    hex_text += upper_value[j];
            }
        }
        if (hex_text.Length() != 6)
            return false;
        red = 16 * HexDigitValue(hex_text[1]);
        red += HexDigitValue(hex_text[2]);
        green = 16 * HexDigitValue(hex_text[3]);
        green += HexDigitValue(hex_text[4]);
        blue = 16 * HexDigitValue(hex_text[5]);
        blue += HexDigitValue(hex_text[6]);
    }
    color_value = red | ((unsigned short)green << 8) | (blue << 16);
    return true;
}

// The value of a hex digit in either case; 0 for any other character.
char FileThemes::HexDigitValue(char digit)
{
    if (digit >= '0' && digit <= '9')
    {
        digit -= 48;
        return digit;
    }
    if (digit >= 'A' && digit <= 'F')
    {
        digit -= 55;
        return digit;
    }
    if (digit >= 'a' && digit <= 'f')
    {
        digit -= 87;
        return digit;
    }
    return 0;
}

// Loads the named theme once its ini file and both graphics files are found in the theme folder; false with the
// error otherwise, or for the default theme.
bool FileThemes::LoadTheme(String file_name)
{
    is_loaded = false;
    error_message = "";
    first_graphics_path = "";
    second_graphics_path = "";
    theme_name = "";
    theme_description = "";
    theme_author = "";
    if (file_name.Length() < 4)
        return false;
    if (LowerCase(file_name) == "default")
        return false;
    String missing_text = "Missing theme ";
    String base_name = StripExtension(file_name, ini_file_name);
    if (base_name.Length() < 4)
    {
        error_message = "Wrong ini filename (" + file_name + ")";
        return false;
    }
    String ini_path = theme_folder + base_name + ini_file_name;
    if (access(ini_path.c_str(), 0))
    {
        error_message = missing_text + ini_file_name;
        return false;
    }
    first_graphics_path = theme_folder + base_name + first_graphics_name;
    if (0 != access(first_graphics_path.c_str(), 0))
    {
        error_message = missing_text + first_graphics_name;
        return false;
    }
    second_graphics_path = theme_folder + base_name + second_graphics_name;
    if (0 != access(second_graphics_path.c_str(), 0))
    {
        error_message = missing_text + second_graphics_name;
        return false;
    }
    return ReadThemeFile(file_name);
}

// Reads a theme file line by line: its name, description, authors and version, the client version it targets, and
// the colour of each part of the interface.
bool FileThemes::ReadThemeFile(String file_name)
{
    int handle;
    char *buffer;
    is_loaded = false;
    is_outdated = false;
    theme_name = "";
    co_author_1 = "";
    co_author_2 = "";
    co_author_3 = "";
    file_name = theme_folder + file_name;
    int size = 0;
    int pos = 0;
    String text;
    TStringList *lines = new TStringList();
    try
    {
        handle = FileOpen(file_name.c_str(), fmOpenRead);
        size = FileSeek(handle, 0, 2);
        FileSeek(handle, 0, 0);
        buffer = new char[size + 1];
        FileRead(handle, buffer, size);
        FileClose(handle);
        text = buffer;
        text.SetLength(size);
        delete[] buffer;
    }
    catch (...)
    {
        FileClose(handle);
        return false;
    }
    int header_count = 0;
    String color_prefix = "FONTCOLOR";
    try
    {
        String line = "";
        int line_start = 1;
        do
        {
            line = "";
            for (int i = line_start; i <= text.Length(); i++)
            {
                if (text[i] == '\n' || i == text.Length())
                {
                    if (i == text.Length())
                    {
                        if ((unsigned char)text[i] != '\n')
                        {
                            if ((unsigned char)text[i] != '\r')
                                line = line + text[i];
                        }
                    }
                    lines->Add(line);
                    line_start = i + 1;
                    break;
                }
                if ((unsigned char)text[i] != '\n' && (unsigned char)text[i] != '\r')
                    line = line + text[i];
            }
        } while (line_start <= text.Length());
        for (int line_index = 0; line_index < lines->Count; line_index++)
        {
            key = "";
            value = "";
            int unused_value = 0;
            pos = 1;
            int equals_count = 0;
            if (!lines->Strings[line_index].Length())
                continue;
            if (lines->Strings[line_index][1] == '#')
                continue;
            if (lines->Strings[line_index][1] == '[')
                continue;
            while (pos <= lines->Strings[line_index].Length())
            {
                if (lines->Strings[line_index][pos] == '=')
                    equals_count++;
                else
                {
                    if (!equals_count)
                    {
                        if (lines->Strings[line_index][pos] != ' ')
                            key += (char)lines->Strings[line_index][pos];
                    }
                    if (equals_count == 1)
                        value += (char)lines->Strings[line_index][pos];
                }
                pos++;
            }
            value = TrimSpaces(value);
            key = UpperCase(key);
            upper_value = UpperCase(value);
            if (upper_value.Length() > 0)
            {
                if (IsKey("THEMENAME"))
                {
                    theme_name = CleanThemeText(value);
                    header_count++;
                }
                else if (IsKey("THEMEDESC"))
                {
                    theme_description = CleanThemeText(value);
                    header_count++;
                }
                else if (IsKey("THEMEAUTHOR"))
                {
                    theme_author = CleanThemeText(value);
                    header_count++;
                }
                else if (IsKey("THEMEVERSION"))
                {
                    theme_version = CleanThemeText(value);
                    header_count++;
                }
                else if (ReadIntValue("TARGETMAJORVERSION"))
                {
                    target_major_version = int_value;
                    header_count++;
                }
                else if (ReadIntValue("TARGETMINORVERSION"))
                {
                    target_minor_version = int_value;
                    header_count++;
                }
                else
                {
                    if (IsKey("THEMECOAUTHOR"))
                    {
                        String co_author = CleanThemeText(value);
                        if (co_author.Length() < 4)
                            continue;
                        if (co_author_1.Length() < 4)
                            co_author_1 = co_author;
                        else if (co_author_2.Length() < 4)
                            co_author_2 = co_author;
                        else if (co_author_3.Length() < 4)
                            co_author_3 = co_author;
                        continue;
                    }
                    if (header_count >= 6)
                    {
                        if (ReadColorValue(color_prefix + "EDITS"))
                            color_edits = color_value;
                        else if (ReadColorValue(color_prefix + "POPUPEDITS"))
                            color_popup_edits = color_value;
                        else if (ReadColorValue(color_prefix + "CHATEDIT"))
                            color_chat_edit = color_value;
                        else if (ReadColorValue(color_prefix + "MENU"))
                            color_menu = color_value;
                        else if (ReadColorValue(color_prefix + "MENUVERSION"))
                            color_menu_version = color_value;
                        else if (ReadColorValue(color_prefix + "TOPBAR"))
                            color_top_bar = color_value;
                        else if (ReadColorValue(color_prefix + "BOTTOMBAR"))
                            color_bottom_bar = color_value;
                        else if (ReadColorValue(color_prefix + "PANEL"))
                            color_panel = color_value;
                        else if (ReadColorValue(color_prefix + "PANELLIST"))
                            color_panel_list = color_value;
                        else if (ReadColorValue(color_prefix + "DIALOGCAPTION"))
                            color_dialog_caption = color_value;
                        else if (ReadColorValue(color_prefix + "DIALOG"))
                            color_dialog = color_value;
                        else if (ReadColorValue(color_prefix + "DIALOGSOFT"))
                            color_dialog_soft = color_value;
                        else if (ReadColorValue(color_prefix + "DIALOGLINK"))
                            color_dialog_link = color_value;
                        else if (ReadColorValue(color_prefix + "DIALOGLINKOVER"))
                            color_dialog_link_over = color_value;
                        else if (ReadColorValue(color_prefix + "POPUPCAPTION"))
                            color_popup_caption = color_value;
                        else if (ReadColorValue(color_prefix + "POPUP"))
                            color_popup = color_value;
                        else if (ReadColorValue(color_prefix + "LOCKERITEM"))
                            color_locker_item = color_value;
                        else if (ReadColorValue(color_prefix + "LOCKERITEMHIGHLIGHT"))
                            color_locker_item_highlight = color_value;
                        else if (ReadColorValue(color_prefix + "CHATDEFAULT"))
                            color_chat_default = color_value;
                        else if (ReadColorValue(color_prefix + "CHATPRIVATE"))
                            color_chat_private = color_value;
                        else if (ReadColorValue(color_prefix + "CHATSYSTEM"))
                            color_chat_system = color_value;
                        else if (ReadColorValue(color_prefix + "CHATWARNING"))
                            color_chat_warning = color_value;
                        else if (ReadColorValue(color_prefix + "CHATGLOBAL"))
                            color_chat_global = color_value;
                        else if (ReadColorValue(color_prefix + "CHATADMIN"))
                            color_chat_admin = color_value;
                        else if (ReadColorValue(color_prefix + "CHATTAB"))
                            color_chat_tab = color_value;
                        else if (ReadColorValue(color_prefix + "CHATTABACTIVE"))
                            color_chat_tab_active = color_value;
                        else if (ReadColorValue(color_prefix + "CHATTABHIGHLIGHT"))
                            color_chat_tab_highlight = color_value;
                    }
                }
            }
        }
    }
    catch (...)
    {
    }
    unsigned char named_count = 0;
    theme_name = theme_name.Trim();
    theme_description = theme_description.Trim();
    theme_author = theme_author.Trim();
    if (theme_name.Length() > 3)
    {
        theme_name = theme_name.SubString(1, 24);
        theme_name = LowerCase(theme_name);
        named_count++;
    }
    if (theme_author.Length() > 3)
    {
        theme_author = theme_author.SubString(1, 24);
        named_count++;
    }
    if (co_author_1.Length() > 3)
        co_author_1 = co_author_1.SubString(1, 24);
    if (co_author_2.Length() > 3)
        co_author_2 = co_author_2.SubString(1, 24);
    if (co_author_3.Length() > 3)
        co_author_3 = co_author_3.SubString(1, 24);
    if (theme_description.Length() > 0)
    {
        theme_description = theme_description.SubString(1, 64);
        theme_description = LowerCase(theme_description);
    }
    if (client_major_version > target_major_version || client_minor_version > target_minor_version)
        is_outdated = true;
    if (theme_version.Length() != 4)
        theme_version = "0.01";
    if (named_count > 1 && header_count >= 6)
    {
        is_loaded = true;
        return true;
    }
    return false;
}

// The text without the spaces at its start and end.
DEFINE_TRIM_SPACES(FileThemes)

// Cleans a theme text: keeps letters, digits and a few signs, makes spaces and underscores one space, and evens out
// the letter case of each word.
String FileThemes::CleanThemeText(String text)
{
    String result = "";
    char after_space = false;
    for (int i = 1; i <= text.Length(); i++)
    {
        char keep = false;
        if ((text[i] == ' ' || text[i] == '_') && !after_space)
        {
            keep = true;
            after_space = true;
        }
        if (text[i] >= '-' && text[i] <= '9')
        {
            keep = true;
            after_space = false;
        }
        if (text[i] >= 'A' && text[i] <= 'Z')
        {
            keep = true;
            after_space = false;
        }
        if (text[i] >= 'a' && text[i] <= 'z')
        {
            keep = true;
            after_space = false;
        }
        if (keep)
            result += text[i];
    }
    if (result.Length() > 0)
        result[1] = toupper(result[1]);
    unsigned char letters_left = 3;
    char after_blank = false;
    for (int k = 1; k <= result.Length(); k++)
    {
        if (letters_left <= 0 || after_blank)
        {
            result[k] = tolower(result[k]);
            after_blank = false;
        }
        else if (result[k] >= 'A' && result[k] <= 'Z')
        {
            letters_left--;
            after_blank = true;
        }
    }
    return result;
}
