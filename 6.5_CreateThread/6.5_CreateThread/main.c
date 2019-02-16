#include<windows.h>
#include<stdio.h>

#define MAX_THREADS 5

typedef struct _THREAD_PARAM
{
	DWORD i;
	DWORD dwRandom;
	DWORD dwData;
}THREAD_PARAM, *LPTHREAD_PARAM;

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	LPTHREAD_PARAM pData;
	
	pData = (LPTHREAD_PARAM)lpParam;
	printf("TID = %u,\t Parameters = %u, %u, %u\n", GetCurrentThreadId(), pData->i, pData->dwRandom, pData->dwData);

	return 0;
}

int main(void)
{
	LPTHREAD_PARAM pData[MAX_THREADS];
	DWORD dwThreadId[MAX_THREADS];
	HANDLE hThread[MAX_THREADS];
	int i;


	for (i = 0; i < MAX_THREADS; i++)
	{
		LPTHREAD_PARAM pDataTemp = (LPTHREAD_PARAM)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(THREAD_PARAM));
		if (NULL == pData)
		{
			printf("HeapAlloc error;\n");
			ExitProcess(2);
		}

		pDataTemp->i = i;
		pDataTemp->dwRandom = rand();
		pDataTemp->dwData = 100;
		pData[i] = pDataTemp;

		hThread[i] = CreateThread(
			NULL,
			0,
			ThreadProc,
			pData[i],
			0,
			&dwThreadId[i]);
		if (NULL == hThread[i])
		{
			ExitProcess(i);
		}
	}

	WaitForMultipleObjects(MAX_THREADS, hThread, TRUE, INFINITE);
	for (i = 0; i < MAX_THREADS; i++)
	{
		HeapFree(GetProcessHeap(), 0, pData[i]);
		CloseHandle(hThread[i]);
	}

	return 0;
}