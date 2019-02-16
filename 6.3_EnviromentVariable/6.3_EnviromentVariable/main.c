#include<windows.h>
#include<stdio.h>
#include<tchar.h>

#define BUFSIZE 4096

typedef enum
{
	VARIABLES_APPEND,
	VARIABLES_NULL,
	VARIABLES_RESET
}E_Variables_Opcode;

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

DWORD WINAPI ChangeEnviromentVariables(LPTSTR szName, LPTSTR szNewValue, E_Variables_Opcode dwFlag)
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

int main(void)
{
	EnumEnviromentVariable();
	ChangeEnviromentVariables(_T("PATH"), _T("C:\\"), VARIABLES_APPEND);

	EnumEnviromentVariable();
	ChangeEnviromentVariables(_T("PATH"), _T("C:\\"), VARIABLES_NULL);

	EnumEnviromentVariable();
	ChangeEnviromentVariables(_T("XX"), _T("C:\\"), VARIABLES_RESET);

	EnumEnviromentVariable();
}