#include <vcl.h>
#pragma hdrstop
USERES("GameServer.res");
USEFORM("MainForm.cpp", GUI);
USEUNIT("Players.cpp");
USEUNIT("Player.cpp");
USEUNIT("Playerskill.cpp");
USEUNIT("Playerinventory.cpp");
USEUNIT("Serial.cpp");
USEUNIT("Settings.cpp");
USEUNIT("Logins.cpp");
USEUNIT("Packets.cpp");
USEUNIT("Mysqlcontrols.cpp");
USEUNIT("Itemvalue.cpp");
USEUNIT("Itemvalues.cpp");
USEUNIT("Npc.cpp");
USEUNIT("Mapchest.cpp");
USEUNIT("Mapcontrol.cpp");
USEUNIT("Mapobject.cpp");
USEUNIT("Mapwarp.cpp");
USEUNIT("Map.cpp");
USEUNIT("Itemground.cpp");
USEUNIT("Itemchest.cpp");
USEUNIT("Skillvalues.cpp");
USEUNIT("Npcvalues.cpp");
USEUNIT("Skillvalue.cpp");
USEUNIT("Npcvalue.cpp");
USEUNIT("Npcdrop.cpp");
USEUNIT("Jukeboxcontrol.cpp");
USEUNIT("Jukebox.cpp");
USEUNIT("Newscontrol.cpp");
USEUNIT("Doorcontrol.cpp");
USEUNIT("Msgboardcontrol.cpp");
USEUNIT("Msgboard.cpp");
USEUNIT("Npccontrol.cpp");
USEUNIT("Gamecontrol.cpp");
USEUNIT("Shopvalues.cpp");
USEUNIT("Shopitem.cpp");
USEUNIT("Shopvalue.cpp");
USEUNIT("Shopcraft.cpp");
USEUNIT("Chestcontrol.cpp");
USEUNIT("Weaponmap.cpp");
USEUNIT("Banned.cpp");
USEUNIT("Effectcontrol.cpp");
USEUNIT("Eventcontrol.cpp");
USEUNIT("Wedding.cpp");
USEUNIT("Weddings.cpp");
USEUNIT("Learnvalue.cpp");
USEUNIT("Learnvalues.cpp");
USEUNIT("Learnitem.cpp");
USEUNIT("MySQLthread.cpp");
USEUNIT("Mysqltask.cpp");
USEUNIT("Innvalues.cpp");
USEUNIT("Innvalue.cpp");
USEUNIT("Classvalues.cpp");
USEUNIT("Classvalue.cpp");
USEUNIT("Playerquest.cpp");
USEUNIT("Questtype.cpp");
USEUNIT("Quest.cpp");
USEUNIT("Questengine.cpp");
USEUNIT("Queststate.cpp");
USEUNIT("Filecache.cpp");
USEUNIT("Killcounter.cpp");
USEUNIT("Playercommand.cpp");
USEUNIT("Killcounters.cpp");
USEUNIT("Questcounter.cpp");
USEUNIT("Questcounterlist.cpp");
USEUNIT("Questcounters.cpp");

#pragma argsused

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
        Application->Initialize();
        Application->CreateForm(__classid(TGUI), &GUI);
        Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    return 0;
}
