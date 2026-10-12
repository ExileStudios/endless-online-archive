#ifndef MainFormH
#define MainFormH

#include <Classes.hpp>
#include <SysUtils.hpp>
#include <ScktComp.hpp>
#include <DBTables.hpp>
#include <string.h>
#include <stdio.h>
#include <vector.h>
#include <deque.h>
#include <list.h>
#include <stack.h>
#include <queue.h>
#include <Forms.hpp>
#include <AppEvnts.hpp>
#include <ExtCtrls.hpp>

struct ChestController;
struct ClassValues;
struct DoorController;
struct EffectController;
struct EventController;
struct Game;
struct InnValues;
struct ItemValues;
struct JukeBoxController;
struct LearnValues;
struct Logins;
struct MapContainer;
struct MsgBoardController;
struct NewsTopics;
struct NpcController;
struct NpcValues;
struct Packets;
struct Players;
struct QuestContainer;
struct SerialKey;
struct Settings;
struct ShopValues;
struct SkillValues;
struct WeddingController;
struct mySQLdb;

// The server's main form: the listening socket, the database, the 10 ms game timer and every controller.
class TGUI : public TForm
{
__published:
        TServerSocket *server;
        TDatabase *mysql;
        TQuery *myquery;
        TSession *mysession;
        TTimer *timer;
        TPanel *panel_connections;
        TPanel *panel_send;
        TPanel *panel_received;
        TPanel *panel_buffer;
        TApplicationEvents *ApplicationEvents1;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall serverClientError(TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
            int &ErrorCode);
        void __fastcall serverClientConnect(TObject *Sender, TCustomWinSocket *Socket);
        void __fastcall serverClientDisconnect(TObject *Sender, TCustomWinSocket *Socket);
        void __fastcall serverClientRead(TObject *Sender, TCustomWinSocket *Socket);
        void __fastcall timerTimer(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ApplicationEvents1Exception(TObject *Sender, Exception *E);
private:
public:
        __fastcall TGUI(TComponent *Owner);
        Settings *settings;
        SerialKey *serial;
        MapContainer *map_control;
        mySQLdb *mysql_controls;
        Players *players;
        Logins *logins;
        Packets *server_ctrl;
        NpcController *npc_control;
        ChestController *chest_control;
        DoorController *door_control;
        EffectController *effect_control;
        EventController *event_control;
        int version_major;
        int version_minor;
        int version_patch;
        int tick_counter;
        ItemValues *item_values;
        NpcValues *npc_values;
        SkillValues *skill_values;
        LearnValues *learn_values;
        ShopValues *shop_values;
        InnValues *inn_values;
        ClassValues *class_values;
        MsgBoardController *msgboard_control;
        JukeBoxController *jukebox_control;
        WeddingController *weddings;
        QuestContainer *quest_engine;
        NewsTopics *news_control;
        Game *game_control;
        int last_action;
        int last_family;
        char pad_0x374[4];

        Packets *Mainform_GetServer();
        String Mainform_ComputeRegName(String key_base, String display_code, String unlock_code);
};

extern PACKAGE TGUI *GUI;


#endif
