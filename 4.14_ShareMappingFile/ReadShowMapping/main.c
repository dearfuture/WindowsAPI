#include<windows.h>
#include<stdio.h>
#include<conio.h>
#include<tchar.h>

#define BUF_SIZE 256

LPTSTR szName = _T("SharedFileMappingObject");

int main(void)
{
	HANDLE hMapFile;
	LPTSTR pBuf;

	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, szName);
	if (NULL == hMapFile)
	{
		printf("OpenFileMapping error: %d.\n", GetLastError());
		return -1;
	}

	pBuf = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
	if (NULL == pBuf)
	{
		printf("MapViewOfFile error %d\n", GetLastError());
		return -1;
	}

	MessageBox(NULL, pBuf, _T("Process2"), MB_OK);

	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
}