#include<windows.h>
#include<stdio.h>
#include<Psapi.h>
#include<tchar.h>

#pragma comment(lib, "psapi.lib")

#include "env.h"

__declspec(dllexport) DWORD ExportExample(LPTSTR szMsg, DWORD dwCode);

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		{
			TCHAR lpMainModuleName[MAX_PATH];
			TCHAR lpMessage[MAX_PATH + 64];
			DWORD dwPID = GetCurrentProcessId();

			GetModuleBaseName(GetCurrentProcess(), NULL, lpMainModuleName, MAX_PATH);
			_stprintf(lpMessage, _T("Process Name: %s, PID: %u"), lpMainModuleName, dwPID);
			MessageBox(NULL, lpMessage, _T("msg.dll"), MB_OK);

			break;
		}
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

DWORD WINAPI EnumEnviromentVariable(void)
{
	PVOID pEv = GetEnvironmentStrings();
	LPTSTR szEnvs;

	for (szEnvs = (LPTSTR)pEv; 0 != *szEnvs;)
	{
		_tprintf(_T("%s\n"), szEnvs);

		szEnvs += _tcslen(szEnvs);
		++szEnvs;
	}
	FreeEnvironmentStrings(pEv);

	return 0;
}

DWORD WINAPI ChangeEnviromentVariables(LPTSTR szName, LPTSTR szNewValue, DWORD dwFlag)
{
	DWORD dwErr;
	PVOID szVal;
	DWORD dwReturn;
	DWORD dwNewValSize;

	if (VARIABLES_APPEND == dwFlag)
	{
		dwNewValSize = _tcslen(szNewValue) + 1;
		szVal = HeapAlloc(GetProcessHeap(), 0, BUFSIZE + dwNewValSize);
		dwReturn = GetEnvironmentVariable(szName, szVal, BUFSIZE);

		if (0 == dwReturn)
		{
			dwErr = GetLastError();
			if (ERROR_ENVVAR_NOT_FOUND == dwErr)
			{
				_tprintf(_T("Enviroment variable %s does not exist.\n"), szName);
			}
			else
			{
				printf("error: %d", dwErr);
			}
			return FALSE;
		}
		else if (BUFSIZE < dwReturn)
		{
			szVal = (LPTSTR)HeapReAlloc(GetProcessHeap(), 0, szVal, dwReturn + dwNewValSize);
			if (NULL == szVal)
			{
				printf("Memory error\n");
				return FALSE;
			}

			dwReturn = GetEnvironmentVariable(szName, szVal, dwReturn);
			if (!dwReturn)
			{
				printf("GetEnviromentVariable failed(%d)\n", GetLastError());
				return FALSE;
			}
		}

		_tcscat(szVal, _T(";"));
		_tcscat(szVal, szNewValue);
		if (!SetEnvironmentVariable(szName, szVal))
		{
			printf("Set Value Error %d", GetLastError());
		}

		HeapFree(GetProcessHeap(), 0, szVal);
		return TRUE;
	}
	else if (VARIABLES_RESET == dwFlag)
	{
		if (!SetEnvironmentVariable(szName, szNewValue))
		{
			printf("Set value error %d", GetLastError());
		}
	}
	else if (VARIABLES_NULL == dwFlag)
	{
		if (!SetEnvironmentVariable(szName, NULL))
		{
			printf("Set value error %d", GetLastError());
		}
	}

	return TRUE;
}

DWORD ExportExample(LPTSTR szMsg, DWORD dwCode)
{
	LPVOID lpShowOut = HeapAlloc(GetProcessHeap(), 0, _tcslen(szMsg) * sizeof(TCHAR) + 100);
	_stprintf(lpShowOut, _T("%s, %d"), szMsg, dwCode);
	MessageBox(NULL, lpShowOut, _T("popped message by export function"), MB_OK);
	HeapFree(GetProcessHeap(), 0, lpShowOut);

	return 0;
}