#pragma warning( disable : 4244 4312 4305 4005 4477 )

#include "crtdbg.h"

#include <Windows.h>
#include <iostream> // debug
#include <thread> // threading
#include <string> // std::to_string
#include <fstream>
#include <Dbghelp.h>
#include <tchar.h>

//for debug console
#include "../overlay/overlay.hpp"

//for threadings
#include "../hacks/esp.h"
#include "../hacks/aimbot.h"

//read modules
#include "../classes/global.hpp"
#include "../mem/memory.h"

//disableing DSE
#include "..\dse\disable dse.h"

//for CtrlHandler
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
    case CTRL_CLOSE_EVENT:
        printf("[Console]Console Closing\n");
        dse.UnLoadIOCTLDriver();
        overlay.DestroyOverlay();
        overlay.DestroyDevice();
        return TRUE;
        break;

    default:
        return FALSE;
        break;
    }
}

void StartThreads()
{
    std::thread EspThread(&CEntityLoop::EspThread, &entityloop);
    std::thread AimbotThread(&CAimbot::AimbotThread, &aimbot);

    EspThread.detach();
    AimbotThread.detach();
}

int main()
{
    //control handler
    SetConsoleCtrlHandler(CtrlHandler, TRUE);

    //for debugging
    debug.ShowConsole();

    //driver needs to be loaded before cs2 is open
    if (FindWindow(NULL, "Counter-Strike 2") != NULL)
    {
        MessageBoxA(NULL, "CS2 Needs to be closed to load the driver Properly. Close CS2 and re-launch Alani-NU!", "Alani-NU", MB_OK | MB_ICONQUESTION);
        return 0;
    }

    //we need to disable DSE then we load the Driver!
    bool results = dse.LoadGigDriver();

    //check to see if gig driver was loaded before we move on!
    if (results)
    {
        //then get a handle to the driver. No need for a check here cause if the driver was loaded properly then a handle will always be granted to the driver.
        LPCSTR szDeviceNames = "\\\\.\\GIO";
        HANDLE ghDriver = CreateFile(szDeviceNames, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        //first thing we need to do i find what OS the user is uing, the reason for this is cause diffrent os versions find G_CiOptions diffrently.
        RTL_OSVERSIONINFOW versionInfo = dse.GetRealOSVersion();
        dse.buildNumber = versionInfo.dwBuildNumber;

        //after this we need to find the address to G_CiOptions
        PVOID g_CiOptionAddress = 0;
        NTSTATUS status = dse.FinalCalculation(&g_CiOptionAddress);
        if (!NT_SUCCESS(status))
            goto GoToPause;

        //now we can disable dse and load our driver!
        status = dse.DisableDSE(g_CiOptionAddress, ghDriver);
        if (!NT_SUCCESS(status))
            goto GoToPause;

        //now load hook driver
        results = dse.LoadIOCTLDriver();
        if (!results)
        {
            dse.UnLoadIOCTLDriver();
            dse.EnableDSE(g_CiOptionAddress, ghDriver);
            goto GoToPause;
        }

        //now enable dse
        status = dse.EnableDSE(g_CiOptionAddress, ghDriver);
        if (!NT_SUCCESS(status))
            goto GoToPause;

        //close handle to gig driver
        CloseHandle(ghDriver);

        //unload gig driver
        results = dse.UnLoadGigDriver();
        if (!results)
            goto GoToPause;
    }
    else
    {
        printf("[GDRV]Failed to load Gigabyte Driver\n");
        goto GoToPause;
    }

    //now since the driver is loaded open up cs2!
    printf("[CS2]You may now load CS2, in Fullscreen Window Mode\n");
    MessageBoxA(NULL, "Driver loaded Properly! You may now open up CS2.", "Alani-NU", MB_OK | MB_ICONQUESTION);

    //wait for cs2 to open
    while (FindWindowW(NULL, L"Counter-Strike 2") == NULL)
        continue;

    printf("[CS2]Waiting for CS2 to load\n");

    Sleep(4000);

    //get handle to driver
    driver.hDriver = CreateFile("\\\\.\\KernalDriver", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

    //get pid and call for dlls
    driver.pid = driver.GetProcessId();
    printf("[CS2]PID : %d\n", driver.pid);

    global.modules.engine = driver.GetEngineAddress();
    printf("[CS2]Engine : %p\n", global.modules.engine);
    global.modules.client = driver.GetClientAddress();
    printf("[CS2]Cleint : %p\n", global.modules.client);

    //start threads
    StartThreads();

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (FindWindowW(NULL, L"Counter-Strike 2") == NULL)
            break;
        else
            continue;
    }

    //exit calls
    printf("[CS2]Exiting\n");

    //unload ioctl driver
    dse.UnLoadIOCTLDriver();

    //destory overlay
    overlay.DestroyOverlay();
    overlay.DestroyDevice();

    return 0;

GoToPause:
    system("pause");
    return 0;
}