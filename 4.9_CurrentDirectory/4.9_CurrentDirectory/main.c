#include<windows.h>
#include<stdio.h>
#include<tchar.h>

int main(void)
{
	TCHAR szCurrentDir[MAX_PATH];
	TCHAR szModulePath[MAX_PATH];
	LPTSTR szKernel32 = _T("kernel32.dll");
	HMODULE hKernel32;
	DWORD dwCurDirPathLen;

	dwCurDirPathLen = GetCurrentDirectory(MAX_PATH, szCurrentDir);
	if (0 == dwCurDirPathLen)
	{
		printf ("Obtain current directory fail. \n");
		return -1;
	}
	_tprintf(_T("Current directory of task is %s \n"), szCurrentDir);

	_tcscpy(szCurrentDir, _T("C:\\"));
	if (!SetCurrentDirectory(szCurrentDir))
	{
		printf("Set current directory fail. \n");
		return -1;
	}

	system("dir");
	dwCurDirPathLen = GetCurrentDirectory(MAX_PATH, szCurrentDir);
	if (0 == dwCurDirPathLen)
	{
		printf ("Obtain current directory fail. \n");
		return -1;
	}
	_tprintf(_T("Current directory of task is %s \n"), szCurrentDir);

	if (!GetModuleFileName(NULL, szModulePath, MAX_PATH))
	{
		printf("Obtain module path fail. \n");
		return -1;
	}
	_tprintf(_T("Path of this module: %s\n"), szModulePath);

	hKernel32 = LoadLibrary(szKernel32);
	if (!GetModuleFileName(hKernel32, szModulePath, MAX_PATH))
	{
		printf("Obtain module path fail. \n");
		return -1;
	}
	_tprintf(_T("Path of kernel32 module is %s\n"), szModulePath);

	getchar();
	return 0;
}