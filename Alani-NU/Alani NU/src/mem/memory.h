#pragma once
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

#define IO_GET_CLIENTADDRESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x666, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#define IO_READ_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x667, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#define IO_WRITE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x668, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#define IO_REQUEST_PROCESSID CTL_CODE(FILE_DEVICE_UNKNOWN, 0x669, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#define IO_GET_ENGINEADDRESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x670, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

class Driver
{
public:
	HANDLE hDriver;
	DWORD pid;

	ULONG64 GetClientAddress()
	{
		ULONG64 Address;
		DWORD Bytes;

		if (DeviceIoControl(hDriver, IO_GET_CLIENTADDRESS, &Address, sizeof(Address), &Address, sizeof(Address), &Bytes, NULL))
		{
			return Address;
		}

		return false;
	}

	ULONG64 GetEngineAddress()
	{
		ULONG64 Address;
		DWORD Bytes;

		if (DeviceIoControl(hDriver, IO_GET_ENGINEADDRESS, &Address, sizeof(Address), &Address, sizeof(Address), &Bytes, NULL))
		{
			return Address;
		}

		return false;
	}

	DWORD GetProcessId()
	{
		int ProcessID;
		DWORD Bytes;

		if (DeviceIoControl(hDriver, IO_REQUEST_PROCESSID, &ProcessID, sizeof(ProcessID), &ProcessID, sizeof(ProcessID), &Bytes, NULL))
		{
			return ProcessID;
		}

		return 0;
	}

	template <typename type>
	type Read(uintptr_t ReadAddress)
	{
		type Buffer;

		KERNAL_READ_REQUEST Readrequest;

		Readrequest.ProcessId = pid;
		Readrequest.Address = ReadAddress;
		Readrequest.pBuff = &Buffer;
		Readrequest.Size = sizeof(type);

		if (DeviceIoControl(hDriver, IO_READ_REQUEST, &Readrequest, sizeof(Readrequest), &Readrequest, sizeof(Readrequest), 0, 0))
		{
			return Buffer;
		}

		return Buffer;
	}

	template <typename type>
	bool Write(uintptr_t WriteAddress, type WriteValue)
	{
		DWORD Bytes;

		KERNAL_WRITE_REQUEST Writerequest;

		Writerequest.ProcessId = pid;
		Writerequest.Address = WriteAddress;
		Writerequest.pBuff = &WriteValue;
		Writerequest.Size = sizeof(type);

		if (DeviceIoControl(hDriver, IO_WRITE_REQUEST, &Writerequest, sizeof(Writerequest), 0, 0, &Bytes, NULL))
		{
			return true;
		}

		return false;
	}

private:
	typedef struct _KERNAL_READ_REQUEST
	{
		int ProcessId;
		uintptr_t Address;
		PVOID pBuff;
		SIZE_T Size;

	} KERNAL_READ_REQUEST, * PKERNAL_READ_REQUEST;

	typedef struct _KERNAL_WRITE_REQUEST
	{
		int ProcessId;
		uintptr_t Address;
		PVOID pBuff;
		SIZE_T Size;

	} KERNAL_WRITE_REQUEST, * PKERNAL_WRITE_REQUEST;

};
inline Driver driver;