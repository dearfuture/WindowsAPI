#include<windows.h>
#include<stdio.h>
#include<tchar.h>
#include<locale.h>

DWORD EnumerateFileInDirectory(LPCTSTR szPath)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hListFile;
	TCHAR szFilePath[MAX_PATH];
	setlocale(LC_ALL, "chs");

	_tcscpy(szFilePath, szPath);
	_tcscat(szFilePath, _T("*.*"));
	hListFile = FindFirstFile(szFilePath, &FindFileData);
	if (INVALID_HANDLE_VALUE == hListFile)
	{
		printf("Error: %d\n", GetLastError());
		return -1;
	}
	else
	{
		do 
		{
			if (_tcscmp(FindFileData.cFileName, _T(".")) == 0
				|| _tcscmp(FindFileData.cFileName, _T("..")) == 0)
			{
				continue;
			}
			_tprintf(_T("%s\t\t"), FindFileData.cFileName);

			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED)
			{
				printf("<Encrypted>");
			}
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
			{
				printf("<Hidden>");
			}
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				printf("<DIR>");
			}
			printf("\n");
		} while (FindNextFile(hListFile, &FindFileData));
	}

	return 0;
}

int main(void)
{
	EnumerateFileInDirectory(_T("E:\\"));
}