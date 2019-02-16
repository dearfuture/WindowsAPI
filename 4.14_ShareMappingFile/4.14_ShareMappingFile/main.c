#include<windows.h>
#include<stdio.h>
#include<conio.h>
#include<tchar.h>

#define BUF_SIZE 256

LPTSTR szName = _T("SharedFileMappingObject");
LPTSTR szMsg = _T("TaskMsg");

int main(int argc, char *argv[])
{
	HANDLE hMapFile;
	LPTSTR pBuf;

	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUF_SIZE, szName);
	if (NULL == hMapFile || INVALID_HANDLE_VALUE == hMapFile)
	{
		printf("CreateFileMapping error: %d\n", GetLastError());
		return -1;
	}

	pBuf = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
	if (NULL == pBuf)
	{
		printf("MapViewOfFile error %d\n", GetLastError());
		return -1;
	}

	if (1 == argc)
	{
		CopyMemory((PVOID)pBuf, szMsg, _tcslen(szMsg) * sizeof(TCHAR));
	}
	else
	{
		DWORD dwCopyLen = (strlen(argv[1]) < BUF_SIZE) ? strlen(argv[1]) : BUF_SIZE;
		CopyMemory((PVOID)pBuf, argv[1], dwCopyLen);
	}

	printf("Run program, press any ken to continue...");
	getch();

	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
}