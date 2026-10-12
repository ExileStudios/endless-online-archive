#include <vcl.h>
#pragma hdrstop

#include "Stats_server.h"
#include "Serial.h"
#include "Settings.h"
#include "Settingshtml.h"
#include "Tmainform.h"

#pragma package(smart_init)

#include <io.h>

// Takes the server's name, limits and version, and loads templates/server.eht.
ServerStats::ServerStats(SerialKey *serial, Settings *server_settings, SettingsHTM *htm_settings)
{
    settings = server_settings;
    html_settings = htm_settings;
    serial_key = serial;
    players = "0";
    connections = "0";
    server_name = settings->GetServerName();
    max_connections = IntToStr(settings->GetMaxConnections());
    max_players = IntToStr(settings->GetMaxPlayers());
    connections = 0;
    version = IntToStr(MainForm->version_major) + ".";
    version = version + IntToStr(MainForm->version_minor) + ".";
    version = version + IntToStr(MainForm->version_patch);
    template_lines = new TStringList();
    page_lines = new TStringList();
    has_template = 0;
    if (!access("./templates/server.eht", 0))
    {
        has_template = 1;
        template_lines->LoadFromFile("./templates/server.eht");
    }
    CheckConnectionLoad();
}

ServerStats::~ServerStats()
{
}

void ServerStats::CheckConnectionLoad()
{
    if (connections > max_connections / 2)
        return;
}

// Writes server.html to the outputpath.
void ServerStats::GenerateHTML()
{
    page_lines->Clear();
    for (int i = 0; i < template_lines->Count; i++)
        page_lines->Add(template_lines->Strings[i]);
    for (int j = 0; j < page_lines->Count; j++)
        FillTemplateLine(j);
    String path = settings->GetOutputPath();
    if (path.Length() > 0)
    {
        if (path[path.Length()] != '\\')
        {
            if (path[path.Length()] != '/')
                path = path + (char)92;
        }
    }
    path = path + "server.html";
    try
    {
        page_lines->SaveToFile(path);
    }
    catch (...)
    {
    }
}

// Replaces the server's tags in one line of the page.
void ServerStats::FillTemplateLine(int line_index)
{
    char in_tag;
    char changed;
    int tag_start;
    int tag_length;
    int i;
    in_tag = 0;
    changed = 0;
    tag_start = 0;
    tag_length = 0;
    String line = page_lines->Strings[line_index];
    for (i = 1; i <= line.Length(); i++)
    {
        if (!in_tag)
        {
            if (line[i] == '{')
            {
                in_tag = 1;
                tag_length = 1;
                tag_start = i;
            }
        }
        else
        {
            tag_length++;
            if (line[i] == '}')
            {
                String tag = line.SubString(tag_start, tag_length);
                line.Delete(tag_start, tag_length);
                if (tag == "{PLAYERS}")
                    line.Insert(players, tag_start);
                if (tag == "{CONNECTIONS}")
                    line.Insert(connections, tag_start);
                if (tag == "{UPTIME}")
                    line.Insert(uptime, tag_start);
                if (tag == "{SEND}")
                    line.Insert(sent, tag_start);
                if (tag == "{RECEIVED}")
                    line.Insert(received, tag_start);
                if (tag == "{VERSION}")
                    line.Insert(version, tag_start);
                if (tag == "{CHARACTERS}")
                    line.Insert(characters, tag_start);
                if (tag == "{ACCOUNTS}")
                    line.Insert(accounts, tag_start);
                if (tag == "{GUILDS}")
                    line.Insert(guilds, tag_start);
                if (tag == "{MOSTPLAYERS}")
                    line.Insert(most_players, tag_start);
                if (tag == "{SERVERNAME}")
                    line.Insert(server_name, tag_start);
                if (tag == "{MAXPLAYERS}")
                    line.Insert(max_players, tag_start);
                if (tag == "{MAXCONNECTIONS}")
                    line.Insert(max_connections, tag_start);
                if (tag == "{GENDATE}")
                    line.Insert(DateToStr(Date()) + " " + TimeToStr(Time()), tag_start);
                // .. online or offline, in the colours of htmlgen.ini
                if (tag == "{STATUS}")
                {
                    if (connected)
                        line.Insert("<FONT COLOR='" + html_settings->GetFontColorOnline() + "'>" +
                            html_settings->CopyString() + "</FONT>", tag_start);
                    else
                        line.Insert("<FONT COLOR='" + html_settings->GetFontColorOffline() + "'>" +
                            html_settings->GetOfflineStatus() + "</FONT>", tag_start);
                }
                // after each tag the line is read again from the start
                i = 1;
                in_tag = 0;
                changed = 1;
                tag_start = 0;
                tag_length = 0;
            }
        }
    }
    if (changed)
        page_lines->Strings[line_index] = line;
}
