#include<windows.h>
#include<stdio.h>
#include<tchar.h>

#define ONE_SECOND		10000000

typedef struct _APC_PROC_ARG
{
	TCHAR *szText;
	DWORD dwValue;
}APC_PROC_ARG;

VOID CALLBACK TimerAPCProc(LPVOID lpArg, DWORD dwTimerLowValue, DWORD dwTimerHighValue)
{
	APC_PROC_ARG *pApcData = (APC_PROC_ARG*)lpArg;
	_tprintf(_T("Message: %s\nValue: %d\n\n"), pApcData->szText, pApcData->dwValue);
	MessageBeep(MB_OK);
}

int main(void)
{
	HANDLE				hTimer;
	BOOL				bSuccess;
	INT64				qwDueTime;
	LARGE_INTEGER		liDeuTime;
	APC_PROC_ARG		ApcData;
	ApcData.szText = _T("Message to apc proc.");
	ApcData.dwValue = 1;

	hTimer = CreateWaitableTimer(
		NULL,
		FALSE,
		_T("MyTimer"));
	if (!hTimer)
	{
		printf("CreateWaitableTimer failed with Error %d.", GetLastError());
		return 0;
	}
	else
	{
		__try
		{
			qwDueTime = -5 * ONE_SECOND;
			liDeuTime.LowPart = (DWORD)(qwDueTime & 0xFFFFFFFF);
			liDeuTime.HighPart = (LONG)(qwDueTime >> 32);

			bSuccess = SetWaitableTimer(
				hTimer,
				&liDeuTime,
				2000,
				TimerAPCProc,
				&ApcData,
				FALSE);
			if (bSuccess)
			{
				for (; ApcData.dwValue < 10; ApcData.dwValue++)
				{
					SleepEx(INFINITE, TRUE);
				}
			}
			else
			{
				printf("SetWaitableTimer failed with error %d.", GetLastError());
			}
		}
		__finally
		{
			CloseHandle(hTimer);
		}
	}
}