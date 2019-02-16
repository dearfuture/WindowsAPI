#include<windows.h>
#include<stdio.h>
#include<tchar.h>

HANDLE hSlot;
LPTSTR lpszSlotName = _T("\\\\.\\mailslot\\sample_mailslot");
LPTSTR lpszMessage = _T("Test Message for mailslot ");

int main(void)
{
	BOOL fResult;
	HANDLE hFile;
	DWORD cbWritten;

	hFile = CreateFile(lpszSlotName,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		(LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);
	fResult = WriteFile(hFile, lpszMessage, (DWORD)((_tcslen(lpszMessage) + 1) * sizeof(TCHAR)), &cbWritten, (LPOVERLAPPED)NULL);
	printf("Slot written to successfully.\n");

	CloseHandle(hFile);

	return ;
}