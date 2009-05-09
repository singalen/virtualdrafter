//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("VirtualDrafter.res");
USEFORM("main.cpp", fmMain);
USEFORM("Pick.cpp", fmPick);
USEUNIT("CardData.cpp");
USEUNIT("db.cpp");
USEUNIT("MyStrUtil.cpp");
USEUNIT("Player.cpp");
USEUNIT("CardValues.cpp");
USEUNIT("DrawMTGC.cpp");
USEFORM("Options.cpp", fmOptions);
USEUNIT("NetPlayer.cpp");
USEFORM("Server.cpp", fmServer);
USEFORM("Client.cpp", fmClient);
USEFORM("About.cpp", AboutBox);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
        Application->Initialize();
        Application->Title = "VirtualDrafter";
        Application->CreateForm(__classid(TfmMain), &fmMain);
                 Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------
