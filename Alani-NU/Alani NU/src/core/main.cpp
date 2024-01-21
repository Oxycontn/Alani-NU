#pragma warning( disable : 4244 4312 4305 4005 )

#include <Windows.h>
#include <iostream> // debug
#include <thread> // threading
#include <string> // std::to_string
#include <fstream>

//for debug console
#include "../overlay/overlay.hpp"

//for threading
#include "../hacks/entity.h" 
#include "../hacks/esp.h"
#include "../hacks/aimbot.h"
#include "../hacks/rcs.h"

// read modules
#include "../classes/global.hpp"
#include "../mem/memory.h"

//disableing DSE
#include "..\dse\disable dse.h"

void StartThreads()
{
    std::thread EspThread(&CEntityLoop::EspThread, &entityloop);
    std::thread AimbotThread(&CAimbot::AimbotThread, &aimbot);

    EspThread.join();
    AimbotThread.join();
}

int main()
{
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
            return 0;

        //now we can disable dse and load our driver!
        status = dse.DisableDSE(g_CiOptionAddress, ghDriver);
        if (!NT_SUCCESS(status))
            return 0;

        //now load hook driver
        results = dse.LoadHookDriver();
        if (!results)
            return 0;

        //now enable dse
        status = dse.EnableDSE(g_CiOptionAddress, ghDriver);
        if (!NT_SUCCESS(status))
            return 0;

        //close handle to gig driver
        CloseHandle(ghDriver);
    }

    //now since the driver is loaded open up cs2!
    printf("[CS2]You may now load CS2");
    MessageBoxA(NULL, "Driver loaded Properly! You may now open up CS2.", "Alani-NU", MB_OK | MB_ICONQUESTION);

    //wait for cs2 to open
    while (FindWindowW(NULL, L"Counter-Strike 2") == NULL)
        continue;

    //get pid and call for dlls
    pid = GetProcesByName("cs2.exe");
    printf("[CS2]PID : %d\n", pid);

    global.modules.client = GetDllBase("Client.dll");
    printf("[CS2]Cleint : %p\n", global.modules.client);
    global.modules.engine = GetDllBase("Engine2.dll");
    printf("[CS2]Engine : %p\n", global.modules.engine);

    StartThreads();

    //exit calls
    printf("Exiting\n");

    overlay.DestroyOverlay();
    overlay.DestroyImGui();
    overlay.DestroyOverlay();

	return 0;
}