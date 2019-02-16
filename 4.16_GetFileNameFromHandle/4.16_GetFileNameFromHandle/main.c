#include<windows.h>
#include<stdio.h>
#include<tchar.h>
#include<string.h>
#include<psapi.h>

#pragma comment(lib, "psapi.lib")

#define BUFSIZE 512

BOOL GetFileNameFromHandle(HANDLE hFile)
{
	TCHAR pszFileName[MAX_PATH + 1];
	HANDLE hFileMap;
	PVOID pMem;

	DWORD dwFileSizeHigh = 0;
	DWORD dwFileSizeLow = GetFileSize(hFile, &dwFileSizeHigh);

	TCHAR szTemp[BUFSIZE] = {0};
	TCHAR szName[MAX_PATH];
	TCHAR szDrive[3] = {0};
	BOOL bFound = FALSE;
	PTCHAR p = szTemp;

	if (0 == dwFileSizeLow && 0 == dwFileSizeHigh)
	{
		printf("Cannot map file of size 0.\n");
		return -1;
	}

	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 1, NULL);
	if (!hFileMap)
	{
		printf("CreateFileMapping error: %d\n", GetLastError());
		return -1;
	}

	pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);
	if (!pMem)
	{
		printf("MapViewOfFile error: %d", GetLastError());
		return -1;
	}

	if (0 == GetMappedFileName(GetCurrentProcess(), pMem, pszFileName, MAX_PATH))
	{
		printf("GetMappedFileName error: %d\n", GetLastError());
		return -1;
	}

	
	if (0 == GetLogicalDriveStrings(BUFSIZE - 1, szTemp))
	{
		printf("GetLogicalDriveStrings error: %d\n", GetLastError());
		return -1;
	}

	
	do 
	{
		UINT uNameLen;

		CopyMemory(szDrive, p, 2 * sizeof(TCHAR));
		if (!QueryDosDevice(szDrive, szName, BUFSIZE))
		{
			printf("QueryDosDrive error: %d", GetLastError());
			return -1;
		}

		uNameLen = _tcslen(szName);
		if (uNameLen < MAX_PATH)
		{
			bFound = (0 == _tcsncmp(pszFileName, szName, uNameLen));
			if (bFound)
			{
				TCHAR szTempFile[MAX_PATH];
				_stprintf(szTempFile, _T("%s%s"), szDrive, pszFileName + uNameLen);
				_tcscpy(pszFileName, szTempFile);
			}
		}

		while (*p++)
		{

		}
	} while (!bFound && *p);

	UnmapViewOfFile(pMem);
	CloseHandle(hFileMap);

	_tprintf(_T("File path is %s\n"), pszFileName);
	return -1;
}

int main(void)
{
	HANDLE hFile;
	HANDLE hFind;
	WIN32_FIND_DATA wfd;

	hFind = FindFirstFile(_T("*.c"), &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("can not find a file");
		return -1;
	}

	_tprintf(_T("Find %s at current dir\n"), wfd.cFileName);
	hFile = CreateFile(wfd.cFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		printf("Create file error, %d.\n", GetLastError());
	}
	else
	{
		GetFileNameFromHandle(hFile);
	}

	CloseHandle(hFile);

	return 0;
}

