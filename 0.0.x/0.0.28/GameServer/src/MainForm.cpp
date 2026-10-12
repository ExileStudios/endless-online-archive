#include <vcl.h>
#pragma hdrstop

#include "MainForm.h"
#include "Chestcontrol.h"
#include "Classvalues.h"
#include "Doorcontrol.h"
#include "Effectcontrol.h"
#include "Eventcontrol.h"
#include "Gamecontrol.h"
#include "Innvalues.h"
#include "Itemvalues.h"
#include "Jukeboxcontrol.h"
#include "Learnvalues.h"
#include "Logins.h"
#include "Mapcontrol.h"
#include "Msgboardcontrol.h"
#include "Mysqlcontrols.h"
#include "Newscontrol.h"
#include "Npccontrol.h"
#include "Npcvalues.h"
#include "Packets.h"
#include "Players.h"
#include "Questengine.h"
#include "Serial.h"
#include "Settings.h"
#include "Shopvalues.h"
#include "Skillvalues.h"
#include "Weddings.h"

#pragma package(smart_init)

#include <Dialogs.hpp>
#include <stdio.h>

#pragma resource "*.dfm"
TGUI *GUI;

__fastcall TGUI::TGUI(Classes::TComponent *Owner)
    : Forms::TForm(Owner)
{
    version_major = 0;
    version_minor = 0;
    version_patch = 28;
}

// Starts the server: the boot log, the database, the data files, the controllers, the arenas, then the socket.
void __fastcall TGUI::FormCreate(System::TObject *Sender)
{
    FILE *boot;
    String line = DateToStr(Now());
    line.Insert(" ", line.Length() + 1);
    line.Insert(TimeToStr(Now()), line.Length() + 1);
    line.Insert(" EndlServ started\n", line.Length() + 1);
    boot = fopen(".\\logs\\boot.log", "a");
    fprintf(boot, "%s", line.c_str());
    fclose(boot);
    mysql_controls = new mySQLdb();
    if (!mysql_controls->TestConnection())
    {
        MessageDlg("No mysql database was found (115)", mtError, TMsgDlgButtons() << mbOK, 0);
        delete mysql_controls;
        Application->Terminate();
    }
    settings = new Settings();
    serial = new SerialKey();
    mysql_controls->InitServerData(version_major, version_minor, version_patch);
    serial->SetCounter(100);
    item_values = new ItemValues();
    npc_values = new NpcValues();
    skill_values = new SkillValues();
    learn_values = new LearnValues();
    shop_values = new ShopValues();
    inn_values = new InnValues();
    class_values = new ClassValues();
    news_control = new NewsTopics();
    jukebox_control = new JukeBoxController();
    players = new Players(settings, mysql_controls);
    map_control = new MapContainer(settings);
    quest_engine = new QuestContainer(settings);
    logins = new Logins(mysql_controls);
    server_ctrl = new Packets(map_control, quest_engine, players, settings, mysql_controls, logins, version_major,
        version_minor, version_patch);
    npc_control = new NpcController(map_control, players, server_ctrl, settings);
    chest_control = new ChestController(map_control, players, server_ctrl, settings);
    effect_control = new EffectController(map_control, players, server_ctrl, settings);
    event_control = new EventController(map_control, players, server_ctrl, settings);
    weddings = new WeddingController(players, server_ctrl);
    door_control = new DoorController(map_control);
    msgboard_control = new MsgBoardController();
    game_control = new Game();
    // .. the arenas' spawn tiles: map, the tile a player waits on, and the tile in the arena
    map_control->AddArenaSpawn(MapId_PvpMainArena, 11, 44, 12, 24);
    map_control->AddArenaSpawn(MapId_PvpMainArena, 13, 44, 12, 17);
    map_control->AddArenaSpawn(MapId_PvpMainArena, 15, 44, 12, 10);
    map_control->AddArenaSpawn(MapId_PvpMainArena, 17, 44, 18, 24);
    map_control->AddArenaSpawn(MapId_PvpMainArena, 19, 44, 18, 10);
    map_control->AddArenaSpawn(MapId_PvpMainArena, 21, 44, 24, 24);
    map_control->AddArenaSpawn(MapId_PvpMainArena, 23, 44, 24, 17);
    map_control->AddArenaSpawn(MapId_PvpMainArena, 25, 44, 24, 10);
    map_control->AddArenaSpawn(MapId_PvpRoom1, 17, 9, 14, 10);
    map_control->AddArenaSpawn(MapId_PvpRoom1, 17, 11, 6, 10);
    map_control->SetArenaBlock(MapId_PvpRoom1, 2);
    map_control->AddArenaSpawn(MapId_PvpRoom2, 17, 9, 14, 10);
    map_control->AddArenaSpawn(MapId_PvpRoom2, 17, 11, 6, 10);
    map_control->SetArenaBlock(MapId_PvpRoom2, 2);
    map_control->AddArenaSpawn(MapId_SmallArena2, 18, 39, 16, 12);
    map_control->AddArenaSpawn(MapId_SmallArena2, 20, 39, 16, 28);
    map_control->AddArenaSpawn(MapId_SmallArena2, 22, 39, 22, 20);
    map_control->AddArenaSpawn(MapId_SmallArena2, 24, 39, 28, 28);
    map_control->AddArenaSpawn(MapId_SmallArena2, 26, 39, 28, 12);
    map_control->AddArenaSpawn(MapId_SmallArena1, 18, 39, 16, 12);
    map_control->AddArenaSpawn(MapId_SmallArena1, 20, 39, 16, 28);
    map_control->AddArenaSpawn(MapId_SmallArena1, 22, 39, 22, 20);
    map_control->AddArenaSpawn(MapId_SmallArena1, 24, 39, 28, 28);
    map_control->AddArenaSpawn(MapId_SmallArena1, 26, 39, 28, 12);
    // .. the socket opens only with a valid licence
    if (serial->IsValid())
    {
        server->Port = settings->GetPort();
        try
        {
            server->Active = true;
        }
        catch (...)
        {
            Application->Terminate();
        }
    }
    if (serial->GetCounter() > 20)
    {
        tick_counter = 0;
        timer->Interval = 10;  // 10 ms
        timer->Enabled = true;
    }
    Caption = settings->GetServerName();
}

Packets * TGUI::Mainform_GetServer()
{
    return server_ctrl;
}

// A new connection: refused past maxconnections + 5, from a blocked ip, or when no player slot is left.
void __fastcall TGUI::serverClientConnect(System::TObject *Sender, Scktcomp::TCustomWinSocket *Socket)
{
    if (Socket->SocketHandle < 1)
    {
        Socket->Close();
        return;
    }
    else if (server->Socket->ActiveConnections > settings->GetMaxConnections() + 5)
    {
        Socket->Close();
        return;
    }
    else
    {
        if (!logins->HandleAddress(Socket->RemoteAddress))
        {
            Socket->Close();
            return;
        }
        if (!players->Add(Socket))
        {
            Socket->Close();
            return;
        }
    }
}

// A socket error removes the player, without a message.
void __fastcall TGUI::serverClientError(System::TObject *Sender, Scktcomp::TCustomWinSocket *Socket,
    Scktcomp::TErrorEvent ErrorEvent, int &ErrorCode)
{
    ErrorCode = 0;
    if (Socket->SocketHandle < 1 || Socket->SocketHandle >= MAX_PLAYER_ID)
        Socket->Close();
    else
        players->MarkRemoving(Socket);
}

// A closed socket takes its player out of the game.
void __fastcall TGUI::serverClientDisconnect(System::TObject *Sender, Scktcomp::TCustomWinSocket *Socket)
{
    if (Socket->SocketHandle >= 1 && Socket->SocketHandle < MAX_PLAYER_ID)
    {
        server_ctrl->Server_RemovePlayer(Socket);
        players->Remove(Socket);
    }
}

void __fastcall TGUI::serverClientRead(System::TObject *Sender, Scktcomp::TCustomWinSocket *Socket)
{
    if (Socket->SocketHandle < 1 || Socket->SocketHandle >= MAX_PLAYER_ID)
    {
        Socket->Close();
        return;
    }
    else
    {
        server_ctrl->Server_ClientRead(Socket, Socket->ReceiveText());
        return;
    }
}

// The game clock, every 10 ms; each part of the server ticks at its own rate:
// every 100 ms   the game and the players
// every 200 ms   the npcs
// every second   the logins, doors, effects, events and weddings
// every 10 s     the server status ( database and panels )
// every 50 s     the chests
void __fastcall TGUI::timerTimer(System::TObject *Sender)
{
    tick_counter++;
    if (!(tick_counter % 10))
        server_ctrl->Game_Tick();
    if (!(tick_counter % 10))
        players->Tick();
    if (!(tick_counter % 20))
        npc_control->Tick();
    if (!(tick_counter % 100))
    {
        logins->Tick();
        door_control->Tick();
        effect_control->Tick();
        event_control->Tick();
        weddings->Tick();
    }
    if (!(tick_counter % 5000))
        chest_control->Tick();
    if (!(tick_counter % 1000))
    {
        mysql_controls->UpdateServerStatus(settings->GetRefreshSeconds(), server->Socket->ActiveConnections,
            players->GetOnlineCount(), players->GetPeakOnline(), server_ctrl->Server_FormatSentTraffic(),
            server_ctrl->Server_FormatReceivedTraffic());
        if (Visible)
        {
            String caption = IntToStr(server->Socket->ActiveConnections) + " con / ";
            caption.Insert(IntToStr(players->GetOnlineCount()) + " players", caption.Length() + 1);
            panel_buffer->Caption = IntToStr((DWORD)mysql_controls->Db_GetQueuedQueryCount()) + " sql";
            panel_send->Caption = server_ctrl->Server_FormatSentTraffic();
            panel_received->Caption = server_ctrl->Server_FormatReceivedTraffic();
            panel_connections->Caption = caption;
        }
    }
    // .. without a valid licence the socket closes
    if (!(tick_counter % 10000) && serial->GetCounter() < 20)
        server->Active = false;
    // .. every 1000 seconds, with players online, the registered name must still match the licence
    if (tick_counter > 100000)
    {
        if (players->GetActiveCount() > 1)
        {
            String expected = Mainform_ComputeRegName(serial->GetKeyBaseCopy(), serial->GetDisplayCode(),
                serial->GetUnlockCode());
            if (serial->GetRegName() != expected)
                server->Active = false;
        }
        tick_counter = 0;
    }
}

// The registered name a licence's key base, display code and unlock code must give.
String TGUI::Mainform_ComputeRegName(String key_base, String display_code, String unlock_code)
{
    int sum;
    char again;
    if (key_base.Length() < 1 || display_code.Length() < 1 || unlock_code.Length() < 1)
        return "";
    String result = "";
    String part = "";
    sum = 13;
    again = 1;
    do
    {
        for (int i = 1; i <= key_base.Length(); i++)
            sum += (unsigned char)key_base[i];
        for (int i = 1; i <= display_code.Length(); i++)
            sum += (unsigned char)display_code[i];
        for (int i = 1; i <= unlock_code.Length(); i++)
            sum += (unsigned char)unlock_code[i];
        sum = sum + 2;
        if (!key_base.Length() && !display_code.Length())
            again = 0;
        if (key_base.Length() > 0)
            key_base.Delete(1, 1);
        if (display_code.Length() > 0)
            display_code.Delete(1, 1);
        if (unlock_code.Length() > 0)
            unlock_code.Delete(1, 1);
        sum = sum + 3;
    } while (again);
    part = IntToHex(4160 * sum, 2);
    if (part.Length() > 2)
        part = part.SubString(part.Length() - 2, 2);
    result = result + part;
    sum = sum + 2;
    part = IntToHex(3473 * sum, 3);
    if (part.Length() > 2)
        part = part.SubString(part.Length() - 3, 3);
    result = result + part;
    sum = sum + 3;
    part = IntToHex(2162 * sum, 4);
    if (part.Length() > 2)
        part = part.SubString(part.Length() - 4, 4);
    result = result + part;
    sum = sum + 4;
    part = IntToHex(5502 * sum, 3);
    if (part.Length() > 2)
        part = part.SubString(part.Length() - 3, 3);
    return result + part;
}

// Closing the window shuts the server down; the form stays.
void __fastcall TGUI::FormClose(System::TObject *Sender, Forms::TCloseAction &Action)
{
    server_ctrl->Server_Shutdown();
    Action = (TCloseAction)0;  // caNone
}

// Writes every unhandled exception to logs/error.log, with the action and family of the last packet.
void __fastcall TGUI::ApplicationEvents1Exception(System::TObject *Sender, Sysutils::Exception *E)
{
    FILE *fp;
    String line = DateToStr(Now());
    line.Insert(" ", line.Length() + 1);
    line.Insert(TimeToStr(Now()), line.Length() + 1);
    line.Insert(" EndlServ ", line.Length() + 1);
    line.Insert(E->Message, line.Length() + 1);
    line.Insert(" ", line.Length() + 1);
    line.Insert(IntToStr(last_action), line.Length() + 1);
    line.Insert(",", line.Length() + 1);
    line.Insert(IntToStr(last_family), line.Length() + 1);
    line.Insert("\n", line.Length() + 1);
    fp = fopen(".\\logs\\error.log", "a");
    fprintf(fp, "%s", line.c_str());
    fclose(fp);
}
