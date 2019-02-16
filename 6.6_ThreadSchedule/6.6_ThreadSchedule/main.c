#include<windows.h>
#include<stdio.h>
#include<tchar.h>

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	DWORD dwData;
	DWORD i = 0;

	dwData = (DWORD)lpParam;
	for (i = 0; i < 10; i++)
	{
		Sleep(100);
		printf("TID = %u,\t Parameters = %u\t i = %u\n", GetCurrentThreadId(), dwData, i);
	}
	ExitThread(i);

	return 0;
}

int main(void)
{
	DWORD dwData;
	DWORD dwThreadId[2];
	HANDLE hThread[2];

	dwData = 1;
	hThread[0] = CreateThread(
		NULL,
		0,
		ThreadProc,
		(LPVOID)dwData,
		CREATE_SUSPENDED,
		&dwThreadId[0]);
	if (NULL == hThread[0])
	{
		ExitProcess(0);
	}

	dwData = 2;
	hThread[1] = CreateThread(NULL,
		0,
		ThreadProc,
		(LPVOID)dwData,
		0,
		&dwThreadId[1]);
	if (NULL == hThread[1])
	{
		ExitProcess(1);
	}

	Sleep(200);
	ResumeThread(hThread[0]);
	SuspendThread(hThread[1]);

	Sleep(300);
	TerminateThread(hThread[0], 0);
	ResumeThread(hThread[1]);

	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
}