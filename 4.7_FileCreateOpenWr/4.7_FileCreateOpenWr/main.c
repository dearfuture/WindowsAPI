#include<windows.h>
#include<stdio.h>
#include<tchar.h>

DWORD ReadFileContent(LPCTSTR szFilePath)
{
	HANDLE hFileRead;
	LARGE_INTEGER liFileSize;
	DWORD dwReadedSize;
	LONGLONG liTotalRead = 0;
	TCHAR lpFileDataBuffer[32];

	hFileRead = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFileRead)
	{
		printf("Open file fail: %d", GetLastError());
	}
	if (!GetFileSizeEx(hFileRead, &liFileSize))
	{
		printf("Obtain file size fail: %d", GetLastError());
	}
	else
	{
		printf("File size is : %d\n", liFileSize.QuadPart);
	}

	while (TRUE)
	{
		DWORD i;
		if (!ReadFile(hFileRead, lpFileDataBuffer, 32, &dwReadedSize, NULL))
		{
			printf("Read file fail: %d\n", GetLastError());
			break;
		}
		printf("Read %d bytes, and the content is: ", dwReadedSize);
		for (i = 0; i < dwReadedSize; i++)
		{
			printf("0x%x ", lpFileDataBuffer[i]);
		}
		printf("\n");
		liTotalRead += dwReadedSize;
		if (liTotalRead == liFileSize.QuadPart)
		{
			printf("Read file completed\n");
			break;
		}
	}

	CloseHandle(hFileRead);

	return 0;
}

DWORD SaveDataToFile(LPCTSTR szFilePath, const LPVOID lpData, DWORD dwDataSize)
{
	HANDLE hFileWrite;
	DWORD dwWritedDataSize;

	hFileWrite = CreateFile(szFilePath, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFileWrite)
	{
		printf("Open file fail: %d", GetLastError());
	}
	SetFilePointer(hFileWrite, 0, 0, FILE_END);
	if (!WriteFile(hFileWrite, lpData, dwDataSize, &dwWritedDataSize, NULL))
	{
		printf("Write file fail: %d\n", GetLastError());
	}
	else
	{
		printf("Write file success, write %d bytes\n", dwWritedDataSize);
	}

	CloseHandle(hFileWrite);
	return 0;
}

int main(void)
{
	TCHAR szFileData[] = _T("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	SaveDataToFile(_T("C:\\show.txt"), (const LPVOID)szFileData, sizeof(szFileData));
	ReadFileContent(_T("C:\\show.txt"));

	SaveDataToFile(_T("C:\\show.txt"), (const LPVOID)szFileData, sizeof(szFileData));
	SaveDataToFile(_T("C:\\show.txt"), (const LPVOID)szFileData, sizeof(szFileData));
	ReadFileContent(_T("C:\\show.txt"));

	//DeleteFile(_T("C:\\show.txt"));
}
