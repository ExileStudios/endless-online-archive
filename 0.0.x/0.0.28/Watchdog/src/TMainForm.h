#ifndef TmainformH
#define TmainformH

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
#include <Graphics.hpp>
#include <Menus.hpp>

struct Client;
struct CountStats;
struct GuildStats;
struct PlayerStats;
struct SerialKey;
struct ServerStats;
struct Settings;
struct SettingsHTM;
struct Top100Stats;

// The Watchdog's hidden main form: the tray icon, the timer and the socket to the game server.
class TMainForm : public TForm
{
__published:
        TPopupMenu *trayermenu;
        TMenuItem *exitwatchdog1;
        TMenuItem *watchdog1;
        TMenuItem *generator1;
        TMenuItem *cancel1;
        TMenuItem *N1;
        TMenuItem *N2;
        TTimer *watchtimer;
        TClientSocket *client;
        TMenuItem *N3;
        TMenuItem *connected1;
        TMenuItem *logdowntime1;
        TApplicationEvents *ApplicationEvents1;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall exitwatchdog1Click(TObject *Sender);
        void __fastcall watchtimerTimer(TObject *Sender);
        void __fastcall clientError(TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode);
        void __fastcall clientConnect(TObject *Sender, TCustomWinSocket *Socket);
        void __fastcall trayermenuPopup(TObject *Sender);
        void __fastcall connected1Click(TObject *Sender);
        void __fastcall clientRead(TObject *Sender, TCustomWinSocket *Socket);
        void __fastcall watchdog1Click(TObject *Sender);
        void __fastcall generator1Click(TObject *Sender);
        void __fastcall logdowntime1Click(TObject *Sender);
        void __fastcall ApplicationEvents1Exception(TObject *Sender, Exception *E);
private:
public:
        __fastcall TMainForm(TComponent *owner);
        char pad_0x304[4];
        TIcon *tray_icon;
        Settings *settings;
        SettingsHTM *html_settings;
        SerialKey *serial_key;
        int tick_count;
        int minute_count;
        int restart_counter;
        int generator_counter;
        Client *packet_controls;
        ServerStats *server_stats;
        PlayerStats *player_stats;
        Top100Stats *top100_stats;
        GuildStats *guild_stats;
        CountStats *count_stats;
        int version_major;
        int version_minor;
        int version_patch;
        char pad_0x34c[4];

        void __fastcall TrayIconMessage(TMessage &Msg);
        static void __fastcall AddTrayIcon(TMainForm *self);
        static void __fastcall RemoveTrayIcon(TMainForm *self);
        void __fastcall AppMinimize(TObject *Sender);
        void __fastcall AppRestore(TObject *Sender);
        void WriteLog(String message);
        BEGIN_MESSAGE_MAP
        VCL_MESSAGE_HANDLER(WM_USER + 1001, TMessage, TrayIconMessage)
        END_MESSAGE_MAP(TForm)
};

extern PACKAGE TMainForm *MainForm;


#endif
