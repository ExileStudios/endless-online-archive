#include <vcl.h>
#pragma hdrstop

#include <Forms.hpp>

USEFORM("TMainForm.cpp", MainForm);

// The program's entry: two named mutexes let at most two copies run, then the application is set up, given its
// title and its main form, and run until it closes.
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    void *h_object = CreateMutexA(0, 1, "EOINST-1");
    if (GetLastError() == 183)
    {
        h_object = CreateMutexA(0, 1, "EOINST-2");
        if (GetLastError() == 183)
        {
            if (h_object)
                CloseHandle(h_object);
            return 0;
        }
    }
    try
    {
        Application->Initialize();
        Application->Title = "EndlessOnline";
        Application->CreateForm(__classid(TMainForm), &MainForm);
        Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    ReleaseMutex(h_object);
    CloseHandle(h_object);
    return 0;
}
