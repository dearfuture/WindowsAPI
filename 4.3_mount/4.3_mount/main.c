#include<windows.h>
#include<stdio.h>
#include<tchar.h>

#define BUFSIZE MAX_PATH
#define FILESYSNAMEBUFSIZE MAX_PATH

BOOL ProcessVolumeMountPoint(HANDLE hPt, TCHAR* ptBuf, DWORD dwPtBufSize, TCHAR* pBuf)
{
	BOOL bFlag;
	TCHAR path[BUFSIZE];
	TCHAR target[BUFSIZE];
	TCHAR volumeName[MAX_PATH + 1] = {0};
	DWORD charCnt = MAX_PATH + 1;

	_tprintf(_T("\tVolume mount point found is \"%s\"\n"), ptBuf);
	_tcscpy(path, pBuf);
	_tcscat(path, ptBuf);
	GetVolumePathNamesForVolumeName(pBuf, volumeName, charCnt, &charCnt);
	_tprintf(_T("Name is: %s%s\n"), volumeName, ptBuf);

	bFlag = GetVolumeNameForVolumeMountPoint(path, target, BUFSIZE);
	if (!bFlag)
	{
		printf("\tAttempt to get volume name for %s failed. \n", path);
	}
	else
	{
		_tprintf(_T("\tTarget of the volume mount point is %s. \n"), target);
		GetVolumePathNamesForVolumeName(target, volumeName, charCnt, &charCnt);
		_tprintf(_T("Name is: %s\n"), volumeName);
	}

	bFlag = FindNextVolumeMountPoint(hPt, ptBuf, dwPtBufSize);

	return bFlag;
}

BOOL ProcessVolume(HANDLE hVol, TCHAR* buf, DWORD iBufSize)
{
	BOOL bFlag;
	HANDLE hPt;
	TCHAR ptBuf[BUFSIZE];
	DWORD dwSysFlags;
	TCHAR fileSysNameBuf[FILESYSNAMEBUFSIZE];
	TCHAR volumeName[MAX_PATH + 1] = {0};
	DWORD charCnt = MAX_PATH + 1;

	_tprintf(_T("Volume found is \"%s\".\n"), buf);
	GetVolumePathNamesForVolumeName(buf, volumeName, charCnt, &charCnt);
	_tprintf(_T("Name is: %s\n"), volumeName);
	GetVolumeInformation(buf, NULL, 0, NULL, NULL, &dwSysFlags, fileSysNameBuf, FILESYSNAMEBUFSIZE);

	if (!(dwSysFlags & FILE_SUPPORTS_REPARSE_POINTS))
	{
		printf("\tThis file system does not support volume mount points.\n");
	}
	else
	{
		hPt = FindFirstVolumeMountPoint(buf, ptBuf, BUFSIZE);
		if (INVALID_HANDLE_VALUE == hPt)
		{
			printf("\tNo volume mount points found!\n");
		}
		else
		{
			bFlag = ProcessVolumeMountPoint(hPt, ptBuf, BUFSIZE, buf);
			while (bFlag)
			{
				bFlag = ProcessVolumeMountPoint(hPt, ptBuf, BUFSIZE, buf);
			}
			FindVolumeMountPointClose(hPt);
		}
	}

	bFlag = FindNextVolume(hVol, buf, iBufSize);

	return bFlag;
}

int GetMountPoint(void)
{
	TCHAR buf[BUFSIZE];
	HANDLE hVol;
	BOOL bFlag;

	printf("Volume mount points info of this computer:\n\n");
	hVol = FindFirstVolume(buf, BUFSIZE);
	if (INVALID_HANDLE_VALUE == hVol)
	{
		printf("No volumes found!\n");
		return -1;
	}

	bFlag = ProcessVolume(hVol, buf, BUFSIZE);
	while (bFlag)
	{
		bFlag = ProcessVolume(hVol, buf, BUFSIZE);
	}
	bFlag = FindVolumeClose(hVol);

	return bFlag;
}

int main(void)
{
	GetMountPoint();

	return 0;
}