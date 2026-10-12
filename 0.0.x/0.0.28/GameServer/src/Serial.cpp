#include <vcl.h>
#pragma hdrstop

#include "Serial.h"

#pragma package(smart_init)

#define ENC_KEY_MKEY "T:b2"  // "mkey"
#define ENC_KEY_NAME "h2l1"  // "name"
#define ENC_KEY_SKEY "T:b,"  // "skey"
#define ENC_NOT_LICENCED "i:j1h<d3 +^1"  // "not licenced"
#define ENC_PUB_DVF_PATH "g)hq@oA9W;B=X/Bq"  // "./pub/dvf001.evf"
#define ENC_SERIAL_INI_PATH "d1dqa>d-h,B8d9_0jpC"  // "./config/serial.ini"

// Reads and checks the licence, then the registered name ( the first 12 characters of pub/dvf001.evf ).
SerialKey::SerialKey()
{
    counter = 10;
    ini_file = new TStringList();
    LoadIni(DecodeString(ENC_SERIAL_INI_PATH));
    key_base = ReadKey(DecodeString(ENC_KEY_NAME), key_base);
    unlock_code = ReadKey(DecodeString(ENC_KEY_MKEY), unlock_code);
    serial_code = ReadKey(DecodeString(ENC_KEY_SKEY), serial_code);
    CloseIni();
    Validate();
    LoadIni(DecodeString(ENC_PUB_DVF_PATH));
    if (ini_file->Text.Length() >= 12)
        reg_name = ini_file->Text.SubString(1, 12);
    CloseIni();
}

SerialKey::~SerialKey()
{
}

// Loads a file, its path taken from the folder the exe is in.
void SerialKey::LoadIni(String path)
{
    ini_file->Clear();
    try
    {
        int last_slash_pos;
        String exe = Application->ExeName;
        last_slash_pos = 0;
        if (exe.Length() >= 1)
        {
            int i = exe.Length();
            while (i >= 1)
            {
                if (exe[i] == '\\')
                {
                    last_slash_pos = i;
                    break;
                }
                i--;
            }
        }
        String dir = "";
        if (exe.Length() >= 1 && last_slash_pos >= 1)
        {
            for (int p = 1; p < last_slash_pos; p++)
                dir = dir + exe[p];
        }
        exe = dir;
        String tail;
        if (exe[exe.Length()] != '\\')
            tail = "\\" + path;
        else
            tail = path;
        ini_file->LoadFromFile(exe + tail);
    }
    catch (...)
    {
    }
}

// The value of a key of the loaded file ( the name in any case ), or the default when it is missing or empty.
String SerialKey::ReadKey(String key, String default_value)
{
    String result = default_value;
    if (ini_file->Count >= 1)
    {
        try
        {
            for (int i = 0; i < ini_file->Count; i++)
            {
                char before_eq;
                String line = ini_file->Strings[i];
                before_eq = 1;
                char after_eq = 1;
                String ini_name = "";
                String value = "";
                if (line.Length() >= 1)
                {
                    for (int j = 1; j <= line.Length(); j++)
                    {
                        if (line[j] == '=')
                            before_eq = 0;
                        if (before_eq)
                        {
                            if (line[j] != ' ' && line[j] != '=')
                                ini_name = ini_name + line[j];
                        }
                        else if (after_eq)
                        {
                            if (line[j] != ' ' && line[j] != '=')
                            {
                                after_eq = 0;
                                value = value + line[j];
                            }
                        }
                        else
                            value = value + line[j];
                    }
                    if (LowerCase(ini_name) == LowerCase(key))
                    {
                        if (value.Length() >= 1)
                            result = value;
                        break;
                    }
                }
            }
        }
        catch (...)
        {
        }
    }
    return result;
}

void SerialKey::CloseIni()
{
    ini_file->Clear();
}

String SerialKey::GetKeyBaseCopy()
{
    return key_base_copy;
}

String SerialKey::GetUnlockCode()
{
    return unlock_code;
}

String SerialKey::GetRegName()
{
    return reg_name;
}

// The skey of a valid licence, or "not licenced".
String SerialKey::GetDisplayCode()
{
    String result = DecodeString(ENC_NOT_LICENCED);
    if (serial_code.Length() >= 1 && valid)
        result = serial_code;
    return result;
}

bool SerialKey::IsValid()
{
    return valid;
}

// Checks the licence in two steps: the skey must fit the name, then the mkey must fit this PC's drive serial.
void SerialKey::Validate()
{
    char stage1_passed;
    int group_sum1;
    int group_sum2;
    int group_sum3;
    int i;
    stage1_passed = 0;
    valid = false;
    group_sum1 = 400;
    group_sum2 = 410;
    group_sum3 = 413;
    for (i = 1; i <= key_base.Length(); i++)
    {
        group_sum1 += (unsigned char)key_base[i] % 17;
        group_sum2 += (unsigned char)key_base[i] / 22;
        group_sum3 += (unsigned char)key_base[i] % 22;
    }
    String expected1 = IntToHex(32 * (2 * group_sum1 % 100), 3);
    String expected2 = IntToHex(37 * (11 * group_sum2 % 88), 3);
    String expected3 = IntToHex(31 * (3 * group_sum3 % 112), 3);
    // the skey's groups at 5, 9 and 13 must be the three values of the name
    if (serial_code.Length() >= 15)
    {
        String serial_group1 = serial_code.SubString(5, 3);
        String serial_group2 = serial_code.SubString(9, 3);
        String serial_group3 = serial_code.SubString(13, 3);
        if (expected1 == serial_group1 && expected2 == serial_group2 && expected3 == serial_group3)
            stage1_passed = 1;
    }
    // .. step 2: the mkey belongs to one PC
    if (stage1_passed)
    {
        DWORD csize = 16;
        DWORD volserial;
        DWORD maxcomp;
        DWORD flags;
        char computer_name[16];
        char volume_name[1000];
        char filesystem_name[1000];
        computer_name[0] = 0;
        GetComputerNameA(computer_name, &csize);
        String cn = computer_name;
        GetVolumeInformationA("c:\\", volume_name, 1000, &volserial, &maxcomp, &flags, filesystem_name, 1000);
        String vs = volserial;
        if (vs.Length() >= 33)
            vs = vs.SubString(1, 32);
        int char_sum = 0;
        for (int i = 1; i <= vs.Length(); i++)
            char_sum += (unsigned char)vs[i];
        for (int i = 1; i <= serial_code.Length(); i++)
            char_sum += (unsigned char)serial_code[i];
        char_sum = vs.Length() * char_sum;
        char_sum *= 135;
        key_base_copy = key_base;
        key_base = IntToStr(char_sum);
        group_sum1 = 409;
        group_sum2 = 411;
        group_sum3 = 412;
        for (int i = 1; i <= key_base.Length(); i++)
        {
            group_sum1 += (unsigned char)key_base[i] % 22;
            group_sum2 += (unsigned char)key_base[i] / 17;
            group_sum3 += (unsigned char)key_base[i] % 18;
        }
        expected1 = IntToHex(32 * (2 * group_sum1 % 100), 3);
        expected2 = IntToHex(37 * (11 * group_sum2 % 88), 3);
        expected3 = IntToHex(31 * (3 * group_sum3 % 112), 3);
        if (serial_code.Length() >= 15)
        {
            String unlock_group1 = unlock_code.SubString(5, 3);
            String unlock_group2 = unlock_code.SubString(9, 3);
            String unlock_group3 = unlock_code.SubString(13, 3);
            if (expected1 == unlock_group1 && expected2 == unlock_group2 && expected3 == unlock_group3)
                valid = true;
        }
    }
}

// Decodes a hidden string: reversed, then each character mirrored inside its range ( 34..125, or 34..79 and 80..125 ).
String SerialKey::DecodeString(String src)
{
    int parity;
    int i;
    String rev = "";
    String result = "";
    parity = (src.Length() + 1) % 2;
    for (i = src.Length(); i >= 1; i--)
        rev = rev + src[i];
    for (int i = 1; i <= src.Length(); i++)
    {
        char ch = rev[i];
        unsigned char byte_value = ch;
        int code = byte_value;
        if (i % 2 == parity)
        {
            if (code >= 34 && code <= 125)
            {
                code = 125 - code + 34;
                byte_value = code;
                ch = byte_value;
                result = result + String((char)ch);
            }
            else
                result = result + String(rev[i]);
        }
        else
        {
            char done = 0;
            if (code >= 34 && code <= 79)
            {
                done = 1;
                code = 79 - code + 34;
                byte_value = code;
                ch = byte_value;
                result = result + String((char)ch);
            }
            if (code >= 80 && code <= 125)
            {
                done = 1;
                code = 125 - code + 80;
                byte_value = code;
                ch = byte_value;
                result = result + String((char)ch);
            }
            if (!done)
                result = result + String(rev[i]);
        }
    }
    return result;
}

int SerialKey::GetCounter()
{
    return counter;
}

void SerialKey::SetCounter(int value)
{
    counter = value;
}
