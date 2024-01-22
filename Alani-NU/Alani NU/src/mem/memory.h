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

class Driver
{
public:
	DWORD pid;

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

	template<typename ... Arg>
	int64_t CallHook(const Arg ... args)
	{
		LoadLibrary(TEXT("user32.dll"));

		void* FunctionPTR = GetProcAddress(LoadLibrary(TEXT("win32u.dll")), "NtQueryCompositionSurfaceStatistics");

		auto function = static_cast<uint64_t(_stdcall*)(Arg...)>(FunctionPTR);

		return function(args ...);
	}

	DWORD GetProcesByName(const char* pName);
};
inline Driver driver;