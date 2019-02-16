#include<windows.h>
#include<stdio.h>
#include<tchar.h>
#include<Psapi.h>
#include<TlHelp32.h>

#pragma comment(lib, "psapi.lib")

VOID WINAPI EnumProcess1(void)
{
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		return ;
	}

	cProcesses = cbNeeded / sizeof(sizeof(DWORD));
	for (i = 0; i < cProcesses; i++)
	{
		printf("\n\n**************************************************");
		printf("\nPROCESS : %u\n\n", aProcesses[i]);
		printf("\n****************************************************");
	}
}

int WINAPI EnumProcess2(void)
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		perror("CreateToolhelp32Snapshot (of processes)");
		return FALSE;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hProcessSnap, &pe32))
	{
		perror("Process32First");
		CloseHandle(hProcessSnap);
		return FALSE;
	}
	do 
	{
		printf("\n\n======================================================");
		_tprintf(_T("\nPROCESS NAME: %s"), pe32.szExeFile);
		printf("\n--------------------------------------------------------");

		dwPriorityClass = 0;
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		if (NULL == hProcess)
		{
			perror("OpenProcess");
		}
		else
		{
			dwPriorityClass = GetPriorityClass(hProcess);
			if (!dwPriorityClass)
			{
				perror("GetPriorityClass");
			}
			CloseHandle(hProcess);
		}

		printf("\n process Id = 0x%08X", pe32.th32ProcessID);
		printf("\n thread count = %d", pe32.cntThreads);
		printf("\n parent process ID = 0x%08X", pe32.th32ParentProcessID);
		printf("\n Priority Base = %d", pe32.pcPriClassBase);
		if (dwPriorityClass)
		{
			printf("\n Priority Class = %d", dwPriorityClass);
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return 0;
}

int main(void)
{
	EnumProcess1();
	EnumProcess2();
}