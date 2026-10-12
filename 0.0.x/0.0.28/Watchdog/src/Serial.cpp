#include <vcl.h>
#pragma hdrstop

#include "Serial.h"
#include "Constants.h"

#pragma package(smart_init)

#define ENC_KEY_MKEY "T:b2"  // "mkey"
#define ENC_KEY_NAME "h2l1"  // "name"
#define ENC_KEY_SKEY "T:b,"  // "skey"
#define ENC_NOT_LICENCED "i:j1h<d3 +^1"  // "not licenced"
#define ENC_SERIAL_INI_PATH "d1dqa>d-h,B8d9_0jpC"  // "./config/serial.ini"

// Reads the licence ( name, skey and mkey ) from config/serial.ini and checks it.
SerialKey::SerialKey()
{
    counter = 10;
    ini_lines = new TStringList();
    SetIniPath(DecodeString(ENC_SERIAL_INI_PATH));
    key_base = ReadKey(DecodeString(ENC_KEY_NAME), key_base);
    unlock_code = ReadKey(DecodeString(ENC_KEY_MKEY), unlock_code);
    serial_code = ReadKey(DecodeString(ENC_KEY_SKEY), serial_code);
    CloseIni();
    Validate();
}

SerialKey::~SerialKey()
{
}

DEFINE_SET_INI_PATH(SerialKey)

// The value of a key of the loaded ini ( the name in any case ), or the default when it is missing or empty.
String SerialKey::ReadKey(String key, String default_value)
{
    String result = default_value;
    if (ini_lines->Count >= 1)
    {
        try
        {
            for (int i = 0; i < ini_lines->Count; i++)
            {
                char before_eq;
                String line = ini_lines->Strings[i];
                before_eq = 1;
                char skip_leading = 1;
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
                        else if (skip_leading)
                        {
                            if (line[j] != ' ' && line[j] != '=')
                            {
                                skip_leading = 0;
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
    ini_lines->Clear();
}

char SerialKey::IsValid()
{
    return valid;
}

// The skey of a valid licence, or "not licenced".
String SerialKey::GetDisplayCode()
{
    String result = DecodeString(ENC_NOT_LICENCED);
    if (serial_code.Length() >= 1 && valid)
        result = serial_code;
    return result;
}

// Checks the licence in two steps: the skey must fit the name, then the mkey must fit this PC's drive serial.
void SerialKey::Validate()
{
    char stage1_passed = 0;
    valid = 0;
    int group_sum1 = 400;
    int group_sum2 = 410;
    int group_sum3 = 413;
    for (int i = 1; i <= key_base.Length(); i++)
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
        DWORD name_size = 16;
        DWORD VolumeSerialNumber;
        DWORD MaximumComponentLength;
        DWORD FileSystemFlags;
        char computer_name[16];
        char VolumeNameBuffer[1000];
        char FileSystemNameBuffer[VOLUME_INFO_BUFFER_SIZE];
        computer_name[0] = 0;
        GetComputerNameA(computer_name, &name_size);
        String computer_name_text = computer_name;
        GetVolumeInformationA("c:\\", VolumeNameBuffer, 1000, &VolumeSerialNumber, &MaximumComponentLength,
            &FileSystemFlags, FileSystemNameBuffer, 1000);
        String volume_serial = VolumeSerialNumber;
        if (volume_serial.Length() >= 33)
            volume_serial = volume_serial.SubString(1, 32);
        int char_sum = 0;
        for (int j = 1; j <= volume_serial.Length(); j++)
            char_sum += (unsigned char)volume_serial[j];
        for (int serial_pos = 1; serial_pos <= serial_code.Length(); serial_pos++)
            char_sum += (unsigned char)serial_code[serial_pos];
        char_sum = volume_serial.Length() * char_sum;
        char_sum *= 135;
        key_base = IntToStr(char_sum);
        group_sum1 = 409;
        group_sum2 = 411;
        group_sum3 = 412;
        for (int key_pos = 1; key_pos <= key_base.Length(); key_pos++)
        {
            group_sum1 += (unsigned char)key_base[key_pos] % 22;
            group_sum2 += (unsigned char)key_base[key_pos] / 17;
            group_sum3 += (unsigned char)key_base[key_pos] % 18;
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
                valid = 1;
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
    for (int j = 1; j <= src.Length(); j++)
    {
        char ch = rev[j];
        unsigned char byte_value = ch;
        int code = byte_value;
        if (j % 2 == parity)
        {
            if (code >= 34 && code <= 125)
            {
                code = 125 - code + 34;
                byte_value = code;
                ch = byte_value;
                result = result + String((char)ch);
            }
            else
                result = result + String(rev[j]);
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
                result = result + String(rev[j]);
        }
    }
    return result;
}

void SerialKey::SetCounter(int value)
{
    counter = value;
}
