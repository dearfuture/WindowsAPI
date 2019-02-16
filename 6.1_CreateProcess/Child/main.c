#include<windows.h>
#include<stdio.h>
#include<TCHAR.h>

#define MyAlloc(size)	HeapAlloc(GetProcessHeap(), 0, size)
#define MyFree(lpMem)	HeapFree(GetProcessHeap(), 0, lpMem)

typedef struct _PROCESS_INFO
{
	DWORD dwPid;
	HANDLE hProcess;
	DWORD dwPrioClass;
	DWORD dwHandleCount;
	DWORD dwAffinityMask;
	SIZE_T dwWorkingSetSizeMax;
	SIZE_T dwWorkingSetSizeMin;
	LPWSTR szwCommandLine;
	STARTUPINFO sti;
}PROCESS_INFO, *LPPROCESS_INFO;

HANDLE hMySelf;

DWORD GetProcessInfo(LPPROCESS_INFO lppi)
{
	lppi->dwPid = GetCurrentProcessId();
	lppi->hProcess = GetCurrentProcess();
	lppi->dwPrioClass = GetPriorityClass(hMySelf);
	GetProcessWorkingSetSize(hMySelf, &lppi->dwWorkingSetSizeMin, &lppi->dwWorkingSetSizeMax);
	lppi->szwCommandLine = GetCommandLine();
	GetStartupInfo(&lppi->sti);

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	PROCESS_INFO pi;
	INT argc;
	TCHAR **argv;
	INT i;
	DWORD dwBufferSize = strlen(lpCmdLine) + MAX_PATH + 1024;
	LPSTR szShowBuffer = (LPSTR)MyAlloc(dwBufferSize);
	TCHAR szTcharCmdLine[MAX_PATH + 1] = {0};

	hMySelf = hInstance;
	sprintf(szShowBuffer, "Startup parameter\nhInstance: %.8X, Cmdline parameter: %s, Show option: %.8X",
		hInstance, lpCmdLine, nShowCmd);
	MultiByteToWideChar(CP_ACP, 0, lpCmdLine, -1, szTcharCmdLine, MAX_PATH);
	MessageBox(NULL, szTcharCmdLine, _T("WinMain function parameter"), MB_OK);

	GetProcessInfo(&pi);
	argv = CommandLineToArgvW(pi.szwCommandLine, &argc);
	*szShowBuffer = '\0';
	for (i = 0; i < argc; i++)
	{
		DWORD dwBufferSize = _tcslen(*argv) + 1;
		LPSTR szMBArgv = MyAlloc(dwBufferSize);
		WideCharToMultiByte(CP_ACP, 0, *argv, -1, szMBArgv, dwBufferSize, NULL, NULL);
		argv++;

		strcat(szShowBuffer, "\n");
		strcat(szShowBuffer, szMBArgv);
		MyFree(szMBArgv);
	}

	MultiByteToWideChar(CP_ACP, 0, szShowBuffer, -1, szTcharCmdLine, MAX_PATH);
	MessageBox(NULL, szTcharCmdLine, _T("Paramter"), MB_OK);
	MyFree(szShowBuffer);

	return 0;
}

