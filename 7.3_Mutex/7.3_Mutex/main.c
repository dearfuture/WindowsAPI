#include<windows.h>
#include<stdio.h>

#define NUM_THREADS		4
DWORD dwCounter = 0;
HANDLE hMutex;
void UseMutex(void);
DWORD WINAPI MutexThread(LPVOID lpParam);

int main(void)
{
	UseMutex();
}

void UseMutex(void)
{
	INT i;
	HANDLE hThread;

	hMutex = CreateMutex(NULL, FALSE, NULL);
	if (NULL == hMutex)
	{
		printf("CreateMutex error: %d\n", GetLastError());
	}

	for (i = 0; i < NUM_THREADS; i++)
	{
		hThread = CreateThread(NULL, 0, MutexThread, NULL, 0, NULL);
		if (NULL == hThread)
		{
			printf("CreateThread failed (%d)\n", GetLastError());
			return ;
		}
	}

	Sleep(1000);
}

DWORD WINAPI MutexThread(LPVOID lpParam)
{
	DWORD dwWaitResult;
	dwWaitResult = WaitForSingleObject(hMutex, INFINITE);

	switch (dwWaitResult)
	{
	case WAIT_OBJECT_0:
		Sleep(rand() % 100);
		printf("counter: %d\n", dwCounter);
		++dwCounter;

		if (!ReleaseMutex(hMutex))
		{
			printf("Release Mutex error: %d\n", GetLastError());
		}
		break;

	default:
		printf("Wait error: %d\n", GetLastError());
		ExitThread(0);
	}

	return 1;
}

