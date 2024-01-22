#include "memory.h"

DWORD Driver::GetProcesByName(const char* pName)
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