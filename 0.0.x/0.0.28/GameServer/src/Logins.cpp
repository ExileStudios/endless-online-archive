#include <vcl.h>
#pragma hdrstop

#include "Logins.h"
#include "Mysqlcontrols.h"

#pragma package(smart_init)

Logins::Logins(mySQLdb *mysql)
{
    mysql_controls = mysql;
    login_list = new TList;
    reserved_names = new TList;
    // these may connect again at once from their last ip
    AddReservedName("vult-r");
    AddReservedName("aengie");
    AddReservedName("angel");
}

void Logins::AddReservedName(String name)
{
    Asocketvip *entry = new Asocketvip;
    entry->name = name;
    entry->value = "new";
    reserved_names->Add(entry);
}

Logins::~Logins()
{
    delete login_list;
    delete reserved_names;
}

// Once a second: counts down every blocked ip and frees the ones that ran out.
void Logins::Tick()
{
    for (int i = login_list->Count - 1; i >= 0; i--)
    {
        Asocketblock *entry = (Asocketblock *)login_list->Items[i];
        entry->count--;
        if (entry->count < 1)
        {
            login_list->Delete(i);
            delete entry;
        }
    }
}

// True when the ip may connect now; it is then blocked for 12 seconds. Local connections always pass.
bool Logins::HandleAddress(String address)
{
    if (address == "127.0.0.1")
        return true;
    else
    {
        bool is_new = 1;
        for (int i = 0; i < login_list->Count; i++)
        {
            Asocketblock *entry = (Asocketblock *)login_list->Items[i];
            if (entry->address == address)
            {
                is_new = 0;
                break;
            }
        }
        if (!is_new)
        {
            for (int j = 0; j < reserved_names->Count; j++)
            {
                Asocketvip *reserved_entry = (Asocketvip *)reserved_names->Items[j];
                if (reserved_entry->value == address)
                {
                    is_new = 1;
                    break;
                }
            }
        }
        if (is_new)
        {
            Asocketblock *new_entry = new Asocketblock;
            new_entry->address = address;
            new_entry->count = 12;
            login_list->Add(new_entry);
        }
        return is_new;
    }
}

// Blocks an ip for 30 seconds.
void Logins::AddLogin(String address)
{
    Asocketblock *entry = new Asocketblock;
    entry->address = address;
    entry->count = 30;
    login_list->Add(entry);
}

// Keeps the ip a reserved name logged in from.
void Logins::SetReservedIp(String name, String ip)
{
    for (int i = 0; i < reserved_names->Count; i++)
    {
        Asocketvip *entry = (Asocketvip *)reserved_names->Items[i];
        if (entry->name == name)
        {
            entry->value = ip;
            break;
        }
    }
}

// True when the ip is one a reserved name logged in from.
bool Logins::IsReservedIp(String ip)
{
    for (int i = 0; i < reserved_names->Count; i++)
    {
        Asocketvip *entry = (Asocketvip *)reserved_names->Items[i];
        if (entry->value == ip)
            return true;
    }
    return false;
}
