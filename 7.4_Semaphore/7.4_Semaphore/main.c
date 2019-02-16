#include<windows.h>
#include<stdio.h>
#include<conio.h>

#define NUMTHREADS		4

HANDLE hSemaphore;

void UseSemaphore(void);
DWORD WINAPI SemaphoreThread(LPVOID lpParam);

int main(void)
{
	UseSemaphore();

	return 0;
}

DWORD WaitForAllThread(HANDLE hThread[], DWORD dwNumThread)
{
	DWORD dwWaitResult = WaitForMultipleObjects(dwNumThread, hThread, TRUE, INFINITE);
	switch (dwWaitResult)
	{
	case WAIT_OBJECT_0:
		printf("\nAll thread exit\n");
		break;
	default:
		printf("\nWait error: %u", GetLastError());
	}

	return 0;
}

VOID UseSemaphore(VOID)
{
	HANDLE hThread[NUMTHREADS];
	INT i;
	LONG lMax;
	CHAR cMax;

	printf("Will create %d threads. Thread that gets sempahore can print to screen.\n"
		"Please input max count of semaphore 1 ~ %d: ", NUMTHREADS, NUMTHREADS);
	cMax = _getch();
	printf("%c\n", cMax);
	lMax = cMax & 0xF;
	if (lMax < 0 || lMax > NUMTHREADS)
	{
		printf("Please input 1 - %d\n", NUMTHREADS);
	}

	hSemaphore = CreateSemaphore(NULL, lMax, lMax, NULL);
	if (NULL == hSemaphore)
	{
		printf("CreateSemaphore error: %d\n", GetLastError());
	}

	for (i = 0; i < NUMTHREADS; i++)
	{
		hThread[i] = CreateThread(NULL, 0, SemaphoreThread, &i, 0, NULL);
		if (NULL == hThread[i])
		{
			printf("CreateThread failed (%d)\n", GetLastError());
			return ;
		}
	}

	WaitForAllThread(hThread, NUMTHREADS);
}

DWORD WINAPI SemaphoreThread(LPVOID lpParam)
{
	DWORD dwWaitResult;
	DWORD dwPreviousCount;
	DWORD j = 0;

	for (; j < 3; j++)
	{
		Sleep(rand() % 1000);
		dwWaitResult = WaitForSingleObject(hSemaphore, INFINITE);
		switch (dwWaitResult)
		{
		case WAIT_OBJECT_0:
			printf("\nProcess %d Gets Semaphore", GetCurrentThreadId());
			break;
		default:
			printf("\nProcess %u Wait Error: %u", GetCurrentThreadId(), GetLastError());
		}

		Sleep(rand() % 1000);
		if (!ReleaseSemaphore(hSemaphore, 1, &dwPreviousCount))
		{
			printf("\nprocess %u ReleaseSemaphore error: %d", GetCurrentThreadId(), GetLastError());
		}
		else
		{
			printf("\nProcess %u release Semaphore, previous count is %u", GetCurrentThreadId(), dwPreviousCount);
		}
	}

	return 1;
}