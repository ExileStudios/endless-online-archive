#include <vcl.h>
#pragma hdrstop

#include "Banned.h"
#include "Mysqlcontrols.h"
#include "Constants.h"

#pragma package(smart_init)

#define IP_OCTET_UNSET 256

// Loads the bans of endl_banlist.
Banned::Banned(mySQLdb *db_handle)
{
    this->ban_list = new TList;
    this->db_handle = db_handle;
    this->db_handle->Query("SELECT bandate, permanent, ipaddress, serial_h FROM endl_banlist");
    if (this->db_handle->GetResultCount() > 0)
    {
        while (!this->db_handle->ResultAtEnd())
        {
            if (this->db_handle->Db_GetInt("permanent") > 0)
                this->AddBan(this->db_handle->Db_GetString("ipaddress"), this->db_handle->Db_GetString("serial_h"),
                    true, 750);
            else
                this->AddBan(this->db_handle->Db_GetString("ipaddress"), this->db_handle->Db_GetString("serial_h"),
                    false, 750);
            this->db_handle->NextResultRecord();
        }
    }
}

Banned::~Banned()
{
}

// Adds a ban; an ip part written * matches any value.
void Banned::AddBan(String ip, String serial, bool permanent, int duration)
{
    try
    {
        int p1 = ip.Pos(".");
        ip[p1] = 'x';
        int p2 = ip.Pos(".");
        ip[p2] = 'x';
        int p3 = ip.Pos(".");
        ip[p3] = 'x';
        short octet1 = IP_OCTET_UNSET;
        short octet2 = IP_OCTET_UNSET;
        short octet3 = IP_OCTET_UNSET;
        short octet4 = IP_OCTET_UNSET;
        Asocketban *entry;
        if (ip.SubString(1, p1 - 1) != "*")
            octet1 = StrToInt(ip.SubString(1, p1 - 1));
        if (ip.SubString(p1 + 1, p2 - 1 - p1) != "*")
            octet2 = StrToInt(ip.SubString(p1 + 1, p2 - 1 - p1));
        if (ip.SubString(p2 + 1, p3 - 1 - p2) != "*")
            octet3 = StrToInt(ip.SubString(p2 + 1, p3 - 1 - p2));
        if (ip.SubString(p3 + 1, 3) != "*")
            octet4 = StrToInt(ip.SubString(p3 + 1, 3));
        entry = new Asocketban;
        entry->hdid = serial;
        entry->octet[0] = octet1;
        entry->octet[1] = octet2;
        entry->octet[2] = octet3;
        entry->octet[3] = octet4;
        entry->ban_date = DateTimeToTimeStamp(Now());
        entry->ban_type = permanent;
        entry->duration = duration;
        ban_list->Add(entry);
    }
    catch (...)
    {
    }
}

// Adds a ban on the ip's first three parts ( any last part ) and the hdid.
void Banned::AddBan(String ip, String hdid, char ban_type, int duration)
{
    try
    {
        int p1 = ip.Pos(".");
        ip[p1] = 'x';
        int p2 = ip.Pos(".");
        ip[p2] = 'x';
        int p3 = ip.Pos(".");
        ip[p3] = 'x';
        short octet1 = IP_OCTET_UNSET;
        short octet2 = IP_OCTET_UNSET;
        short octet3 = IP_OCTET_UNSET;
        short lv6c = 256;
        Asocketban *entry;
        if (ip.SubString(1, p1 - 1) != "*")
            octet1 = StrToInt(ip.SubString(1, p1 - 1));
        if (ip.SubString(p1 + 1, p2 - 1 - p1) != "*")
            octet2 = StrToInt(ip.SubString(p1 + 1, p2 - 1 - p1));
        if (ip.SubString(p2 + 1, p3 - 1 - p2) != "*")
            octet3 = StrToInt(ip.SubString(p2 + 1, p3 - 1 - p2));
        entry = new Asocketban;
        entry->hdid = hdid;
        entry->octet[0] = octet1;
        entry->octet[1] = octet2;
        entry->octet[2] = octet3;
        entry->octet[3] = lv6c;
        entry->ban_date = DateTimeToTimeStamp(Now());
        entry->ban_type = ban_type;
        entry->duration = duration;
        ban_list->Add(entry);
    }
    catch (...)
    {
    }
}

// True when the ip or the hdid is banned; GetBanType and GetBanTime then give the ban's type and the minutes left.
char Banned::IsBanned(String ip, String hdid)
{
    char banned = 0;
    try
    {
        int first_dot = ip.Pos(".");
        ip[first_dot] = 'x';
        int second_dot = ip.Pos(".");
        ip[second_dot] = 'x';
        int third_dot = ip.Pos(".");
        ip[third_dot] = 'x';
        short octet1 = IP_OCTET_UNSET;
        short octet2 = IP_OCTET_UNSET;
        short octet3 = IP_OCTET_UNSET;
        short octet4 = IP_OCTET_UNSET;
        if (ip.SubString(1, first_dot - 1) != "*")
            octet1 = StrToInt(ip.SubString(1, first_dot - 1));
        if (ip.SubString(first_dot + 1, second_dot - 1 - first_dot) != "*")
            octet2 = StrToInt(ip.SubString(first_dot + 1, second_dot - 1 - first_dot));
        if (ip.SubString(second_dot + 1, third_dot - 1 - second_dot) != "*")
            octet3 = StrToInt(ip.SubString(second_dot + 1, third_dot - 1 - second_dot));
        if (ip.SubString(third_dot + 1, 3) != "*")
            octet4 = StrToInt(ip.SubString(third_dot + 1, 3));
        for (int i = ban_list->Count - 1; i >= 0; i--)
        {
            char match = 0;
            Asocketban *entry;
            TTimeStamp now;
            entry = (Asocketban *)ban_list->Items[i];
            now = DateTimeToTimeStamp(Now());
            int days = now.Date - entry->ban_date.Date;
            int ms = now.Time - entry->ban_date.Time;
            int elapsed = ms / MS_PER_SECOND + days * SECONDS_PER_DAY;
            // another first ip part skips the ban, even for the same hdid
            if (entry->octet[0] != octet1)
            {
                if (entry->octet[0] <= 255)
                    continue;
            }
            if (hdid == entry->hdid)
                match = 1;
            if (entry->octet[1] == octet2 || entry->octet[1] > 255)
            {
                if (entry->octet[2] == octet3 || entry->octet[2] > 255)
                {
                    if (entry->octet[3] == octet4 || entry->octet[3] > 255)
                        match = 1;
                }
            }
            if (match)
            {
                if (entry->duration > elapsed)
                {
                    ban_time = (entry->duration - elapsed) / 60 + 1;
                    ban_type = entry->ban_type;
                    banned = 1;
                }
            }
            // a timed ban that ran out is taken off the list
            if (!entry->ban_type)
            {
                if (entry->duration <= elapsed)
                {
                    ban_list->Delete(i);
                    delete entry;
                }
            }
        }
    }
    catch (...)
    {
        banned = 0;
    }
    return banned;
}

int Banned::GetBanType()
{
    return ban_type;
}

int Banned::GetBanTime()
{
    return ban_time;
}
