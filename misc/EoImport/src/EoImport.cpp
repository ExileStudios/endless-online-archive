#include <vcl.h>
#pragma hdrstop

#include <Forms.hpp>

USEFORM("UnitMain.cpp", MainForm);
USEFORM("UnitFileExist.cpp", FormFileExist);
USEFORM("UnitFileNotFound.cpp", FormFileNotFound);

// Creates the importer's three windows and runs it; an exception is shown in a message box.
int WINAPI WinMain(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int n_cmd_show)
{
    try
    {
        Application->Initialize();
        Application->Title = "Eo Pub Import";
        Application->CreateForm(__classid(TMainForm), &MainForm);
        Application->CreateForm(__classid(TFormFileExist), &FormFileExist);
        Application->CreateForm(__classid(TFormFileNotFound), &FormFileNotFound);
        Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    return 0;
}
