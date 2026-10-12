#include <vcl.h>
#pragma hdrstop
USERES("Watchdog.res");
USEFORM("TMainForm.cpp", MainForm);
USEUNIT("Stats_server.cpp");
USEUNIT("Settingshtml.cpp");
USEUNIT("Serial.cpp");
USEUNIT("Settings.cpp");
USEUNIT("Packetproces.cpp");
USEUNIT("Packetcontrols.cpp");
USEUNIT("Packetoperations.cpp");
USEUNIT("Stats_players.cpp");
USEUNIT("Stats_top100.cpp");
USEUNIT("Stats_guilds.cpp");
USEUNIT("Stats_count.cpp");

#pragma argsused

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
        Application->Initialize();
        Application->Title = "Endless Watch";
        Application->ShowMainForm = false;  // the Watchdog lives in the tray
        Application->CreateForm(__classid(TMainForm), &MainForm);
        Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    return 0;
}
