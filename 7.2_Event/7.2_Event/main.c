#include<windows.h>
#include<stdio.h>
#include<tchar.h>

#define NUMTHREADS 3
#define BUFFER_SIZE 16
#define FOR_TIMES 5

HANDLE hWriteEvents[NUMTHREADS];
HANDLE hReadEvents[NUMTHREADS];
BYTE lpSharedBuffer[BUFFER_SIZE] = {0};

DWORD MultiEvents(void);
VOID WriteToBuffer(void);
DWORD WINAPI ThreadFunction(LPVOID lpParam);

int main(void)
{
	MultiEvents();
}

DWORD MultiEvents(void)
{
	HANDLE hThread; 
	DWORD i;

	for (i = 0; i < NUMTHREADS; i++)
	{
		hWriteEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (NULL == hWriteEvents[i])
		{
			printf("CreateEvent Failed(%d)\n", GetLastError());
			return 0;
		}

		hReadEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (NULL == hReadEvents[i])
		{
			printf("CreateEvent failed (%d)\n", GetLastError());
			return 0;
		}

		hThread = CreateThread(NULL, 0, ThreadFunction, (LPVOID)i, 0, NULL);
		if (NULL == hThread)
		{
			printf("CreateThread failed (%d)\n", GetLastError());
			return 0;
		}
	}
	WriteToBuffer();

	return 1;
}

VOID WriteToBuffer(VOID)
{
	DWORD dwWaitResult, j, i;
	for (j = 0; j < FOR_TIMES; j++)
	{
		Sleep(rand() % 100);
		sprintf(lpSharedBuffer, "shared %d", j);

		for (i = 0; i < NUMTHREADS; i++)
		{
			if (!SetEvent(hWriteEvents[i]))
			{
				printf("SetEvent failed (%d)\n", GetLastError());
				return ;
			}
		}

		dwWaitResult = WaitForMultipleObjects(NUMTHREADS,
			hReadEvents,
			TRUE,
			INFINITE);
		if (WAIT_OBJECT_0 != dwWaitResult)
		{
			printf("Wait error: %d\n", GetLastError());
			ExitProcess(0);
		}
	}
}

DWORD WINAPI ThreadFunction(LPVOID lpParam)
{
	DWORD dwWaitResult;
	BYTE lpRead[16];
	DWORD j = 0;
	DWORD dwThreadIndex = (DWORD)lpParam;

	for (; j < FOR_TIMES; j++)
	{
		dwWaitResult = WaitForSingleObject(hWriteEvents[dwThreadIndex], INFINITE);
		switch (dwWaitResult)
		{
		case WAIT_OBJECT_0:
			Sleep(rand() % 10);
			CopyMemory(lpRead, lpSharedBuffer, 16);
			break;
		default:
			printf("Wait error: %d\n", GetLastError());
			ExitThread(0);
		}

		if (!SetEvent(hReadEvents[dwThreadIndex]))
		{
			printf("SetEvent failed (%d)\n", GetLastError());
			return 0;
		}
		printf("Thread %u\t the %d times reads, content: %s\n", dwThreadIndex, j, (LPSTR)lpRead);
	}

	return 1;
}