#include <vcl.h>
#pragma hdrstop
USERES("EOSoda.res");
USEFORM("Unit1.cpp", Form1);

#pragma argsused

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
        Application->Initialize();
        Application->Title = "EO Soda";
        Application->CreateForm(__classid(TForm1), &Form1);
        Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    return 0;
}
