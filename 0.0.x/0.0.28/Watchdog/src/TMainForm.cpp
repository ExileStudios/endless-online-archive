#include <vcl.h>
#pragma hdrstop

#include "Tmainform.h"
#include "Packetcontrols.h"
#include "Packetoperations.h"
#include "Serial.h"
#include "Settings.h"
#include "Settingshtml.h"
#include "Stats_count.h"
#include "Stats_guilds.h"
#include "Stats_players.h"
#include "Stats_server.h"
#include "Stats_top100.h"

#pragma package(smart_init)

#include <process.h>
#include <stdio.h>

#pragma resource "*.dfm"
TMainForm *MainForm;

char *g_47447C = "endless advanced watchdog";

// Version 0.0.28: creates the settings, the licence and the pages, connects to the game server and adds the tray icon.
__fastcall TMainForm::TMainForm(Classes::TComponent *owner)
    : Forms::TForm(owner)
{
    version_major = 0;
    version_minor = 0;
    version_patch = 28;
    packet_controls = new Client();
    settings = new Settings();
    html_settings = new SettingsHTM();
    serial_key = new SerialKey();
    server_stats = new ServerStats(serial_key, settings, html_settings);
    player_stats = new PlayerStats(serial_key, settings, html_settings);
    top100_stats = new Top100Stats(serial_key, settings, html_settings);
    guild_stats = new GuildStats(serial_key, settings, html_settings);
    count_stats = new CountStats(serial_key, settings, html_settings);
    tick_count = 0;
    restart_counter = 0;
    generator_counter = 0;
    tray_icon = new TIcon();
    tray_icon->Handle = (HICON)LoadImageA(HInstance, "TRAYICON", 1, 0, 0, 0);
    if (serial_key->IsValid())
        serial_key->SetCounter(100);
    if (settings->GetWatchdog())
        watchdog1->Checked = true;
    if (settings->GetGenerator())
        generator1->Checked = true;
    if (settings->GetDowntimeLog())
        logdowntime1->Checked = true;
    client->Port = settings->GetPort();
    client->Active = true;
    AddTrayIcon(this);
    Application->OnMinimize = AppMinimize;
    Application->OnRestore = AppRestore;
}

// The tray icon's mouse messages.
void __fastcall TMainForm::TrayIconMessage(TMessage &Msg)
{
    POINT Point;
    switch (Msg.LParam)
    {
        case 517:  // WM_RBUTTONUP: the tray menu
            GetCursorPos(&Point);
            SetForegroundWindow(Handle);
            trayermenu->Popup(Point.x, Point.y);
            PostMessageA(Handle, 0, 0, 0);
            break;
        case 515:  // WM_LBUTTONDBLCLK: the window comes back
            Application->Restore();
            break;
    }
}

// Puts the Watchdog's icon in the tray.
void __fastcall TMainForm::AddTrayIcon(TMainForm *self)
{
    NOTIFYICONDATAA icon_data;
    icon_data.cbSize = 488;
    icon_data.uID = 1006;
    icon_data.hWnd = self->Handle;
    icon_data.uFlags = 7;  // NIF_MESSAGE, NIF_ICON and NIF_TIP
    icon_data.uCallbackMessage = 2025;  // WM_USER + 1001, see TrayIconMessage
    lstrcpyA(icon_data.szTip, g_47447C);
    icon_data.hIcon = self->tray_icon->Handle;
    Shell_NotifyIconA(0, &icon_data);
}

void __fastcall TMainForm::RemoveTrayIcon(TMainForm *self)
{
    NOTIFYICONDATAA icon_data;
    icon_data.cbSize = 488;
    icon_data.uID = 1006;
    icon_data.hWnd = self->Handle;
    icon_data.hIcon = self->tray_icon->Handle;
    Shell_NotifyIconA(2, &icon_data);
}

// Minimized, the Watchdog hides; only the tray icon stays.
void __fastcall TMainForm::AppMinimize(System::TObject *Sender)
{
    ShowWindow(Application->Handle, SW_HIDE);
    Visible = false;
}

void __fastcall TMainForm::AppRestore(System::TObject *Sender)
{
    ShowWindow(Application->Handle, SW_SHOW);
    Visible = true;
    SetForegroundWindow(Handle);
}

// Closing the window only hides it; the tray menu has the exit.
void __fastcall TMainForm::FormClose(System::TObject *Sender, Forms::TCloseAction &Action)
{
    Action = (TCloseAction)0;  // caNone
    Hide();
}

void __fastcall TMainForm::exitwatchdog1Click(System::TObject *Sender)
{
    RemoveTrayIcon(this);
    Application->Terminate();
}

// The Watchdog's clock, once a second: logs the downtime, starts the server again and writes the pages.
void __fastcall TMainForm::watchtimerTimer(System::TObject *Sender)
{
    tick_count++;
    server_stats->connected = client->Active;
    // .. once a minute while the socket is closed or the server is down: a line in error.log
    if (!(tick_count % 60) && settings->GetDowntimeLog() && (!server_stats->connected || !server_stats->server_running))
    {
        String message = "Lost connection with gameserver [s:";
        if (server_stats->connected)
            message = message + "1,p:";
        else
            message = message + "0,p:";
        message = message + server_stats->players;
        message = message + "/";
        message = message + server_stats->connections;
        message = message + "]";
        WriteLog(message);
    }
    if (!(tick_count % 60))
    {
        minute_count++;
        // the server runs when a window has the servername as its title
        HWND server_window = FindWindowA(0, settings->GetServerName().c_str());
        if (!server_window)
        {
            if (settings->GetDowntimeLog() && server_stats->server_running)
            {
                WriteLog("The gameserver was running but not anymore, last #players/con: " + server_stats->players +
                    "/" + server_stats->connections);
                server_stats->connected = client->Active;
                server_stats->GenerateHTML();
            }
            server_stats->server_running = false;
            server_stats->connections = 0;
            server_stats->players = 0;
            count_stats->players = 0;
        }
        else
            server_stats->server_running = true;
        if (settings->GetWatchdog())
        {
            restart_counter++;
            // .. every watchinterval minutes: start watchapp again if the server is not running
            if (settings->GetWatchInterval() < restart_counter)
            {
                if (!server_stats->server_running)
                {
                    try
                    {
                        spawnl(1, settings->GetWatchApp().c_str(), settings->GetWatchApp().c_str(), 0);  // P_NOWAIT
                    }
                    catch (...)
                    {
                    }
                }
                restart_counter = 0;
            }
        }
    }
    // .. the generator: ask the server, then write the page a few seconds later, over a 120 second cycle
    if (settings->GetGenerator())
    {
        if (tick_count == 10 && server_stats->has_template)
            packet_controls->packet_operator->RequestServerStats();
        if (tick_count == 11 && server_stats->has_template)
            server_stats->CheckConnectionLoad();
        if (tick_count == 15 && server_stats->has_template)
            server_stats->GenerateHTML();
        if (tick_count == 16 && count_stats->has_template)
            count_stats->GenerateHTML();
        if (tick_count == 40 && player_stats->has_template)
            packet_controls->packet_operator->RequestOnlineList();
        if (tick_count == 50 && player_stats->has_template)
            player_stats->GenerateHTML();
        if (tick_count == 51 && count_stats->has_template)
            count_stats->GenerateHTML();
        if (tick_count == 60 && player_stats->has_template)
            player_stats->GenerateExpHTML();
        if (tick_count == 70)
            packet_controls->packet_operator->RequestTopPlayers();
        if (tick_count == 80 && top100_stats->has_template)
            top100_stats->GenerateHTML();
        if (tick_count == 100)
            packet_controls->packet_operator->RequestTopGuilds();
        if (tick_count == 110 && guild_stats->has_template)
            guild_stats->GenerateHTML();
    }
    // .. the end of the cycle: an invalid licence closes the Watchdog, and a closed socket connects again
    if (tick_count > 120)
    {
        tick_count = 0;
        if (!serial_key->IsValid())
        {
            Application->Terminate();
            return;
        }
        if (!client->Active)
            client->Active = true;
    }
    if (minute_count > 1440)  // a day
        minute_count = 0;
}

int TMainForm_Length(String &list)
{
    return list.Length();
}

char &TMainForm_Slot(String &list, int index)
{
    return list[index];
}

String *TMainForm_New()
{
    return new String();
}

// Socket errors are not shown; the timer connects again.
void __fastcall TMainForm::clientError(System::TObject *Sender, Scktcomp::TCustomWinSocket *Socket,
    Scktcomp::TErrorEvent ErrorEvent, int &ErrorCode)
{
    ErrorCode = 0;
}

// Starts the init handshake as soon as the socket connects.
void __fastcall TMainForm::clientConnect(System::TObject *Sender, Scktcomp::TCustomWinSocket *Socket)
{
    packet_controls->SendInit();
}

void __fastcall TMainForm::clientRead(System::TObject *Sender, Scktcomp::TCustomWinSocket *Socket)
{
    packet_controls->HandleRead(Socket->RemoteAddress, Socket->SocketHandle, Socket->ReceiveText());
}

void TMainForm_UnusedLiterals1()
{
    "";
    "";
}

// Shows on the tray menu whether the socket is connected.
void __fastcall TMainForm::trayermenuPopup(System::TObject *Sender)
{
    if (client->Active)
        connected1->Caption = "connected";
    else
        connected1->Caption = "disconnected";
}

// Clicking "disconnected" connects again.
void __fastcall TMainForm::connected1Click(System::TObject *Sender)
{
    if (connected1->Caption == "disconnected")
        client->Active = true;
}

// Turns the watchdog on or off.
void __fastcall TMainForm::watchdog1Click(System::TObject *Sender)
{
    if (settings->GetWatchdog())
        settings->SetWatchdog(false);
    else
        settings->SetWatchdog(true);
    watchdog1->Checked = settings->GetWatchdog();
}

// Turns the page generator on or off.
void __fastcall TMainForm::generator1Click(System::TObject *Sender)
{
    if (settings->GetGenerator())
        settings->SetGenerator(false);
    else
        settings->SetGenerator(true);
    generator1->Checked = settings->GetGenerator();
}

// Turns the downtime log on or off.
void __fastcall TMainForm::logdowntime1Click(System::TObject *Sender)
{
    if (settings->GetDowntimeLog())
        settings->SetDowntimeLog(false);
    else
        settings->SetDowntimeLog(true);
    logdowntime1->Checked = settings->GetDowntimeLog();
}

// Writes every unhandled exception to error.log.
void __fastcall TMainForm::ApplicationEvents1Exception(System::TObject *Sender, Sysutils::Exception *E)
{
    FILE *log_file;
    String line = DateToStr(Now());
    line.Insert(" ", line.Length() + 1);
    line.Insert(TimeToStr(Now()), line.Length() + 1);
    line.Insert(" Watchdog ", line.Length() + 1);
    line.Insert(E->Message, line.Length() + 1);
    line.Insert("\n", line.Length() + 1);
    log_file = fopen("error.log", "a");
    fprintf(log_file, "%s", line.c_str());
    fclose(log_file);
}

// Adds a dated line to error.log.
void TMainForm::WriteLog(String message)
{
    FILE *log_file;
    String line = DateToStr(Now());
    line.Insert(" ", line.Length() + 1);
    line.Insert(TimeToStr(Now()), line.Length() + 1);
    line.Insert(" Watchdog ", line.Length() + 1);
    line.Insert(message, line.Length() + 1);
    line.Insert("\n", line.Length() + 1);
    log_file = fopen("error.log", "a");
    fprintf(log_file, "%s", line.c_str());
    fclose(log_file);
}
