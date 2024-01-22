#pragma once
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

typedef struct _NULL_MEMORY
{
	void* buffer_address;
	UINT_PTR address;
	ULONGLONG size;
	ULONG pid;
	BOOLEAN write;
	BOOLEAN read;
	BOOLEAN requestBase;
	void* output;
	const char* moduleName;
	ULONG64 baseAddress;
}NULL_MEMORY;

inline DWORD pid;

inline DWORD GetProcesByName(const char* pName)
{
	DWORD pID = 0;
	HANDLE snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pInfo;
	pInfo.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(snapShot, &pInfo))
	{
		while (Process32Next(snapShot, &pInfo))
		{
			if (_stricmp(pName, pInfo.szExeFile) == 0)
			{
				pID = pInfo.th32ProcessID;
				return pID;
				CloseHandle(snapShot);
				break;
			}
		}
	}
	CloseHandle(snapShot);
	return 0;
}

template<typename ... Arg>
inline int64_t CallHook(const Arg ... args)
{
	LoadLibrary(TEXT("user32.dll"));

	void* FunctionPTR = GetProcAddress(LoadLibrary(TEXT("win32u.dll")), "NtQueryCompositionSurfaceStatistics");

	auto function = static_cast<uint64_t(_stdcall*)(Arg...)>(FunctionPTR);

	return function(args ...);
}

inline ULONG64 GetDllBase(const char* moduleName)
{
	NULL_MEMORY instructions = { 0 };

	instructions.pid = pid;
	instructions.moduleName = moduleName;
	instructions.requestBase = TRUE;

	CallHook(&instructions);

	uintptr_t base;
	base = instructions.baseAddress;

	return base;
}

template<class type>
inline type Read(uintptr_t ReadAddress)
{
	type Buffer;

	NULL_MEMORY instructions;

	instructions.pid = pid;
	instructions.address = ReadAddress;
	instructions.output = &Buffer;
	instructions.size = sizeof(type);
	instructions.read = TRUE;

	CallHook(&instructions);

	return Buffer;
}