#include<windows.h>
#include<stdio.h>
#include<tchar.h>
#include<locale.h>

DWORD ListAllFileInDirectory(LPCTSTR szPath)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hListFile;
	TCHAR szFilePath[MAX_PATH];
	TCHAR szFullPath[MAX_PATH];
	DWORD dwTotalFileNum = 0;

	setlocale(LC_ALL, "chs");
	_tcscpy(szFilePath, szPath);
	_tcscat(szFilePath, _T("\\*.*"));
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
			_stprintf(szFullPath, _T("%s\\%s"), szPath, FindFileData.cFileName);
			dwTotalFileNum++;

			_tprintf(_T("\n%d\t%s\t"), dwTotalFileNum, szFullPath);

			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				printf("<DIR>");
				ListAllFileInDirectory(szFullPath);
			}
		} while (FindNextFile(hListFile, &FindFileData));
	}

	return 0;
}

int main(void)
{
	ListAllFileInDirectory(_T("E:"));
}