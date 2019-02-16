#include<windows.h>
#include<stdio.h>
#include<tchar.h>
#include<Psapi.h>
#include<TlHelp32.h>

#pragma comment(lib, "psapi.lib")

BOOL GetProcessIdByName(LPTSTR szProcessName, LPDWORD lpPID)
{
	STARTUPINFO st;
	PROCESS_INFORMATION pi;
	PROCESSENTRY32 ps;
	HANDLE hSnapshot;

	ZeroMemory(&st, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	st.cb = sizeof(STARTUPINFO);
	ZeroMemory(&ps, sizeof(PROCESSENTRY32));
	ps.dwSize = sizeof(PROCESSENTRY32);

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return FALSE;
	}

	if (!Process32First(hSnapshot, &ps))
	{
		return FALSE;
	}
	do 
	{
		if (_tcscmp(ps.szExeFile, szProcessName) == 0)
		{
			*lpPID = ps.th32ProcessID;
			CloseHandle(hSnapshot);
			return TRUE;
		}
	} while (Process32Next(hSnapshot, &ps));

	CloseHandle(hSnapshot);
	return FALSE;
}

VOID ListProcessModules1(DWORD dwPID)
{
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;

	printf("\nListProcessModules1 Process ID %u\n", dwPID);
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE, dwPID);
	if (NULL == hProcess)
	{
		return;
	}

	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];
			if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
			{
				_tprintf(_T("\t%s (0x%08X)\n"), szModName, hMods[i]);
			}
		}
	}
	CloseHandle(hProcess);
}

int ListProcessModules2(DWORD dwPID)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;
	
	printf("\nListProcessModules2 Process ID %u\n", dwPID);
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (INVALID_HANDLE_VALUE == hModuleSnap)
	{
		perror("CreateToolhelp32Snapshot (of modules)");
		return FALSE;
	}

	me32.dwSize = sizeof(MODULEENTRY32);
	if (!Module32First(hModuleSnap, &me32))
	{
		perror("Module32First");
		CloseHandle(hModuleSnap);
		return FALSE;
	}
	do 
	{
		_tprintf(_T("\n\n MODULE NAME: %s"), me32.szModule);
		_tprintf(_T("\n executable = %s"), me32.szExePath);
		printf("\n process ID = 0x%08X", me32.th32ProcessID);
		printf("\n ref count(g) = 0x%04X", me32.GlblcntUsage);
		printf("\n ref coung(p) = 0x%04X", me32.ProccntUsage);
		printf("\n base address = 0x%08X", (DWORD)me32.modBaseAddr);
		printf("\n base size = %d", me32.modBaseSize);
	} while (Module32Next(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);
	return TRUE;
}

VOID PrintMemoryInfo(DWORD dwPID)
{
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	printf ("\nProcess ID: %u\n", dwPID);
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
	if (NULL == hProcess)
	{
		return ;
	}

	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
	{
		printf("\tPageFaultCount: 0x%08X\n", pmc.PageFaultCount);
		printf("\tPeakWorkingSetSize: 0x%08X\n", pmc.PeakWorkingSetSize);
		printf("\tWorkingSetSize: 0x%08X\n", pmc.WorkingSetSize);
		printf("\tQuotaPeakPagedPoolUsage: 0x%08X\n", pmc.QuotaPeakPagedPoolUsage);
		printf("\tQuotaPagedPoolUsage: 0x%08X\n", pmc.QuotaPagedPoolUsage);
		printf("\tQuotaPeakNonPagedPoolUsage: 0x%08X\n", pmc.QuotaPeakNonPagedPoolUsage);
		printf("\tQuotaNonPagedPoolUsage: 0x%08X\n", pmc.QuotaNonPagedPoolUsage);
		printf("\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage);
		printf("\tPeakPagefileUsage: 0x%08X\n", pmc.PeakPagefileUsage);
	}

	CloseHandle(hProcess);
}

VOID ListHeapInfo(DWORD dwPID)
{
	HEAPLIST32 hl;
	HEAPENTRY32 he;
	HANDLE hSnapshot = INVALID_HANDLE_VALUE;

	printf("\ListHeapInfo Process ID %u\n", dwPID);

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, dwPID);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		perror("CreateToolhelp32Snapshot (of heaplist)");
		return FALSE;
	}
	hl.dwSize = sizeof(HEAPLIST32);

	if (!Heap32ListFirst(hSnapshot, &hl))
	{
		perror("Heap32ListFirst");
		CloseHandle(hSnapshot);
		return FALSE;
	}
	do 
	{
		printf("\n\tHeap ID = %u", hl.th32HeapID);
		printf("\tHeap Flags = %u", hl.dwFlags);
		he.dwSize = sizeof(HEAPENTRY32);

		if (!Heap32First(&he, dwPID, hl.th32HeapID))
		{
			perror("Heap32First");
			CloseHandle(hSnapshot);
			return FALSE;
		}

		do 
		{
			printf("\n\t\t Heap Address\t= %u", he.dwAddress);
			printf("\t Heap Size\t= %u", he.dwBlockSize);
			printf("\t Heap Flags\t= %u", he.dwFlags);
			printf("\t Heap Handle\t = %u", he.hHandle);
		} while (Heap32Next(&he));
	} while (Heap32ListNext(hSnapshot, &hl));

	CloseHandle(hSnapshot);
	return TRUE;
}

int main(void)
{
	DWORD dwPid = 0;

	GetProcessIdByName(_T("cmd.exe"), &dwPid);
	ListProcessModules1(dwPid);
	ListProcessModules2(dwPid);
	PrintMemoryInfo(dwPid);
	ListHeapInfo(dwPid);
}