#include<windows.h>
#include<stdio.h>
#include<stdint.h>
#include<tchar.h>

BOOL GetDiskSpaceInfo(LPCTSTR pszDrive)
{
	DWORD64 qwFreeBytesToCaller, qwTotalBytes, qwFreeBytes;
	DWORD dwSectPerClust, dwBytesPerSect, dwFreeClusters, dwTotalClusters;
	BOOL bResult;

	bResult = GetDiskFreeSpaceEx(pszDrive, 
		(PULARGE_INTEGER)&qwFreeBytesToCaller,
		(PULARGE_INTEGER)&qwTotalBytes,
		(PULARGE_INTEGER)&qwFreeBytes);
	if (bResult)
	{
		printf("Using GetDiskFreeSpaceEx to get disk space info\n");
		printf("Available free space(bytes):\t\t%I64d\n", qwFreeBytesToCaller);
		printf("Free space(bytes):\t\t%I64d\n", qwFreeBytes);
		printf("Total disk space(bytes):\t\t%I64d\n", qwTotalBytes);
	}

	bResult = GetDiskFreeSpace(pszDrive, &dwSectPerClust, &dwBytesPerSect, &dwFreeClusters, &dwTotalClusters);
	if (bResult)
	{
		printf("\nUsing GetDiskFreeSpace to get disk space info\n");
		printf("Free clusters: \t\t%d\n", dwFreeClusters);
		printf("Total clusters: \t\t%d\n", dwTotalClusters);
		printf("Sections per cluster: \t\t%d\n", dwSectPerClust);
		printf("Space per sections(Bytes): \t\t%d\n", dwBytesPerSect);
		printf("Free space(Bytes): \t\t%I64d\n", (DWORD64)dwFreeClusters * dwSectPerClust * dwBytesPerSect);
		printf("Total space on disk(Bytes): \t\t%I64d", (DWORD64)dwTotalClusters * dwSectPerClust * dwBytesPerSect);
	}

	return bResult;
}

int main(void)
{
	GetDiskSpaceInfo(_T("C:\\"));
	return 0;
}