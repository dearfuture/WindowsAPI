#include<windows.h>
#include<stdio.h>
#include<tchar.h>

VOID WINAPI ReadFiberFunc(LPVOID lpParameter);
VOID WINAPI WriteFiberFunc(LPVOID lpParameter);

typedef struct _FIBERDATASTRUCT
{
	DWORD dwParameter;
	DWORD dwFiberResultCode;
	HANDLE hFile;
	DWORD dwByteProcessed;
}FIBERDATASTRUCT, *LPFIBERDATASTRUCT;

#define RTN_OK 0
#define RTN_USAGE 1
#define RTN_ERROR 2
#define BUFFER_SIZE 64
#define FIBER_COUNT 3
#define PRIMARY_FIBER 0
#define READ_FIBER 1
#define WRITE_FIBER 2

LPVOID g_lpFiber[FIBER_COUNT];
LPBYTE g_lpBuffer;
DWORD g_dwBytesRead;

int main(int argc, char *argv[])
{
	LPFIBERDATASTRUCT fs;
	
	fs = (LPFIBERDATASTRUCT)HeapAlloc(GetProcessHeap(), 0, sizeof(FIBERDATASTRUCT) * FIBER_COUNT);
	if (NULL == fs)
	{
		printf("HeapAlloc error! (rc = %lu)\n", GetLastError());
		return RTN_ERROR;
	}

	g_lpBuffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, BUFFER_SIZE);
	if (NULL == g_lpBuffer)
	{
		printf("HeapAlloc error! (rc = %lu)\n", GetLastError());
		return RTN_ERROR;
	}

	fs[READ_FIBER].hFile = CreateFile(_T("main.c"), GENERIC_READ, 
		FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (INVALID_HANDLE_VALUE == fs[READ_FIBER].hFile)
	{
		printf("CreateFile error! (rc = %lu)\n", GetLastError());
		return RTN_ERROR;
	}

	fs[WRITE_FIBER].hFile = CreateFile(_T("main.txt"), GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (INVALID_HANDLE_VALUE == fs[WRITE_FIBER].hFile)
	{
		printf("CreateFile error! (rc = %lu)\n", GetLastError());
		return RTN_ERROR;
	}

	g_lpFiber[PRIMARY_FIBER] = ConvertThreadToFiber(&fs[PRIMARY_FIBER]);
	if (NULL == g_lpFiber[PRIMARY_FIBER])
	{
		printf("ConvertThreadToFiber failed! rc = %lu\n", GetLastError());
		return RTN_ERROR;
	}

	fs[PRIMARY_FIBER].dwParameter = 0;
	fs[PRIMARY_FIBER].dwFiberResultCode = 0;
	fs[PRIMARY_FIBER].hFile = INVALID_HANDLE_VALUE;
	g_lpFiber[READ_FIBER] = CreateFiber(0, ReadFiberFunc, &fs[READ_FIBER]);
	if (NULL == g_lpFiber[READ_FIBER])
	{
		printf("CreateFiber error! (rc = %lu)\n", GetLastError());
		return RTN_ERROR;
	}
	fs[READ_FIBER].dwParameter = (DWORD)g_lpFiber[READ_FIBER];

	g_lpFiber[WRITE_FIBER] = CreateFiber(0, WriteFiberFunc, &fs[WRITE_FIBER]);
	if (NULL == g_lpFiber[WRITE_FIBER])
	{
		printf("CreateFiber error! (rc = %lu)\n", GetLastError());
		return RTN_ERROR;
	}
	fs[WRITE_FIBER].dwParameter = (DWORD)g_lpFiber[WRITE_FIBER];
	SwitchToFiber(g_lpFiber[READ_FIBER]);

	printf("ReadFiber result == %lu Bytes Processed == %lu\n",
		fs[READ_FIBER].dwFiberResultCode, fs[READ_FIBER].dwByteProcessed);
	printf("WriteFiber result == %lu Bytes Processed = %lu\n",
		fs[WRITE_FIBER].dwFiberResultCode, fs[WRITE_FIBER].dwByteProcessed);
	DeleteFiber(g_lpFiber[READ_FIBER]);
	DeleteFiber(g_lpFiber[WRITE_FIBER]);
	CloseHandle(fs[READ_FIBER].hFile);
	CloseHandle(fs[WRITE_FIBER].hFile);
	HeapFree(GetProcessHeap(), 0, g_lpBuffer);
	HeapFree(GetProcessHeap(), 0, fs);

	return RTN_OK;
}

VOID WINAPI ReadFiberFunc(LPVOID lpParameter)
{
	LPFIBERDATASTRUCT fds = (LPFIBERDATASTRUCT)lpParameter;
	
	if (NULL == fds)
	{
		printf("Passed NULL fiber data. Exiting current thread.\n");
		return ;
	}

	printf("Read Fiber (dwParamter == 0x%1x)\n", fds->dwParameter);
	fds->dwByteProcessed = 0;
	while (1)
	{
		if (!ReadFile(fds->hFile, g_lpBuffer, BUFFER_SIZE, &g_dwBytesRead, NULL))
		{
			break;
		}
		if (0 == g_dwBytesRead)
		{
			break;
		}

		fds->dwByteProcessed += g_dwBytesRead;

		printf("Switch to Write\n");
		SwitchToFiber(g_lpFiber[WRITE_FIBER]);
	}

	fds->dwFiberResultCode = GetLastError();
	SwitchToFiber(g_lpFiber[PRIMARY_FIBER]);
}

VOID WINAPI WriteFiberFunc(LPVOID lpParameter)
{
	LPFIBERDATASTRUCT fds = (LPFIBERDATASTRUCT)lpParameter;
	DWORD dwBytesWritten;

	if (NULL == fds)
	{
		printf("Passed NULL fiber data. Exiting current thread.\n");
		return ;
	}

	printf("Write Fiber(dwParamter == 0x%1x)\n", fds->dwParameter);
	fds->dwByteProcessed = 0;
	fds->dwFiberResultCode = ERROR_SUCCESS;

	while (1)
	{
		if (!WriteFile(fds->hFile, g_lpBuffer, g_dwBytesRead, &dwBytesWritten, NULL))
		{
			break;
		}

		fds->dwByteProcessed += dwBytesWritten;

		printf("Switch To Read\n");
		SwitchToFiber(g_lpFiber[READ_FIBER]);
	}

	fds->dwFiberResultCode = GetLastError();
	SwitchToFiber(g_lpFiber[PRIMARY_FIBER]);
}