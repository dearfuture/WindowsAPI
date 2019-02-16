#include<windows.h>
#include<stdio.h>
#include<tchar.h>

HANDLE hSlot;
LPTSTR lpszSlotName = _T("\\\\.\\mailslot\\sample_mailslot");
LPTSTR Message = _T("Message for mailslot in primary domain.");

int main(void)
{
	DWORD cbmessage, cMessage, cbRead;
	BOOL fResult;
	LPTSTR lpszBuffer;
	TCHAR achID[80];
	DWORD cAllMessages;
	cbmessage = cMessage = cbRead = 0;

	hSlot = CreateMailslot(lpszSlotName, 0, MAILSLOT_WAIT_FOREVER, (LPSECURITY_ATTRIBUTES)NULL);
	while (1)
	{
		fResult = GetMailslotInfo(hSlot, (LPDWORD)NULL, &cbmessage, &cMessage, (LPDWORD)NULL);
		if (cbmessage == MAILSLOT_NO_MESSAGE)
		{
			Sleep(20000);
			continue;
		}

		cAllMessages = cMessage;
		while (0 != cMessage)
		{
			_stprintf((LPTSTR)achID, _T("\nMessage #%d of %d\n"),
				cAllMessages - cMessage + 1, cAllMessages);
			lpszBuffer = (LPTSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
				_tcslen((LPTSTR)achID) * sizeof(TCHAR) + cbmessage);
			fResult = ReadFile(hSlot, lpszBuffer, cbmessage, &cbRead, NULL);
			_tcscat(lpszBuffer, (LPTSTR)achID);
			_tprintf(_T("Contents of the mailslot: %s\n"), lpszBuffer);
			HeapFree(GetProcessHeap(), 0, lpszBuffer);
			fResult = GetMailslotInfo(hSlot, (LPDWORD)NULL, &cbmessage, &cMessage, (LPDWORD)NULL);
		}
	}
}

