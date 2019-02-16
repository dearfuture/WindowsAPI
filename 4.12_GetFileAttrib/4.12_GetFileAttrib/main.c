#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<tchar.h>



DWORD ShowFileTime(PFILETIME lpTime)
{
	FILETIME ftLocal;
	SYSTEMTIME st;
	FileTimeToLocalFileTime(lpTime, &ftLocal);
	FileTimeToSystemTime(&ftLocal, &st);
	printf("%4dyear - %#02dmonth - %02dday, %#02d:%#02d:%#02d\n", 
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	return 0;
}

DWORD ShowFileSize(DWORD dwFileSizeHigh, DWORD dwFileSizeLow)
{
	ULONGLONG liFileSize;
	liFileSize = dwFileSizeHigh;

	liFileSize <<= sizeof(DWORD) * 8;
	liFileSize |= dwFileSizeLow;

	printf("File Size: \t%I64u bytes\n", liFileSize);

	return 0;
}

DWORD ShowFileAttrInfo(DWORD dwAttribute)
{
	printf("File Attr: \t");
	if (dwAttribute & FILE_ATTRIBUTE_ARCHIVE)
	{
		printf("<ARCHIVE> ");
	}
	if (dwAttribute & FILE_ATTRIBUTE_COMPRESSED)
	{
		printf("<COMPRESSED> ");
	}
	if (dwAttribute & FILE_ATTRIBUTE_DIRECTORY)
	{
		printf("<DIRECTORY> ");
	}
	if (dwAttribute & FILE_ATTRIBUTE_ENCRYPTED)
	{
		printf("<ENCRYPTED> ");
	}
	if (dwAttribute & FILE_ATTRIBUTE_HIDDEN)
	{
		printf("<HIDDEND> ");
	}
	if (dwAttribute & FILE_ATTRIBUTE_NORMAL)
	{
		printf("<NORMAL> ");
	}
	if (dwAttribute & FILE_ATTRIBUTE_OFFLINE)
	{
		printf("<OFFLINE> ");
	}
	if (dwAttribute & FILE_ATTRIBUTE_READONLY)
	{
		printf("<READONLY> ");
	}
	if (dwAttribute & FILE_ATTRIBUTE_SPARSE_FILE)
	{
		printf("<SPARSE> ");
	}
	if (dwAttribute & FILE_ATTRIBUTE_SYSTEM)
	{
		printf("<SYSTEM> ");
	}
	if (dwAttribute & FILE_ATTRIBUTE_TEMPORARY)
	{
		printf("<TEMPORARY> ");
	}
	printf("\n");

	return 0;
}

DWORD ShowFileAttributes(LPCTSTR szPath)
{
	WIN32_FILE_ATTRIBUTE_DATA wfad;
	_tprintf(_T("File: %s\n"), szPath);

	if (!GetFileAttributesEx(szPath, GetFileExInfoStandard, &wfad))
	{
		printf("GetFileAttribute Error: %d\n", GetLastError());
		return -1;
	}

	printf("Created time: \t");
	ShowFileTime(&(wfad.ftCreationTime));
	printf("Last access time: \t");
	ShowFileTime(&(wfad.ftLastAccessTime));
	printf("Last write time: \t");
	ShowFileTime(&(wfad.ftLastWriteTime));
	ShowFileSize(wfad.nFileSizeHigh, wfad.nFileSizeLow);
	ShowFileAttrInfo(wfad.dwFileAttributes);

	return 0;
}

int main(int argc, char* argv[])
{
	TCHAR szFilePath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, argv[0], -1, szFilePath, MAX_PATH);

	ShowFileAttributes(szFilePath);

	return 0;
}