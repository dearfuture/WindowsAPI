#include<windows.h>
#include<stdlib.h>
#include<stdio.h>
#include<tchar.h>

#define BUFSIZE 1024

BOOL GetDriverInfo(PTSTR szDrive)
{
	UINT uDriveType;
	DWORD dwVolumeSerialNumber;
	DWORD dwMaximumComponentLength;
	DWORD dwFileSystemFlags;
	TCHAR szFileSystemNameBuffer[BUFSIZE];
	TCHAR szDriveName[MAX_PATH];

	_tprintf(_T("\n%s\n"), szDrive);
	uDriveType = GetDriveType(szDrive);
	switch (uDriveType)
	{
	case DRIVE_UNKNOWN:
		printf("The drive type cannot be determnied.");
		break;
	case DRIVE_NO_ROOT_DIR:
		printf("The roto path is invalid, for example, no volume is mounted at the path. ");
		break;
	case DRIVE_REMOVABLE:
		printf("The drive is a type that has removable media, for example, a floppy drive or removable hard disk. ");
		break;
	case DRIVE_FIXED:
		printf("The drive is a type that cannot be removed, for example, a fixed hard drive. ");
		break;
	case DRIVE_REMOTE:
		printf("The drive is a remote(network) drive. ");
		break;
	case DRIVE_CDROM:
		printf("The drive is a CD-ROM drive. ");
		break;
	case DRIVE_RAMDISK:
		printf("The drive is a RAM disk. ");
		break;
	default:
		break;
	}

	if (!GetVolumeInformation(szDrive, szDriveName, MAX_PATH, 
		&dwVolumeSerialNumber, &dwMaximumComponentLength, &dwFileSystemFlags, szFileSystemNameBuffer, BUFSIZE))
	{
		return FALSE;
	}
	if (0 != _tcslen(szDriveName))
	{
		printf("\nDrive Name is %s\n", szDriveName);
	}
	printf("\nVolume Serial Number is %u", dwVolumeSerialNumber);
	printf("\nMaximum Component Length is %u", dwMaximumComponentLength);
	printf("\nSystem Type is %s\n", szFileSystemNameBuffer);
	if (dwFileSystemFlags & FILE_SUPPORTS_REPARSE_POINTS)
	{
		printf("The file system does not support volume mount points. \n");
	}
	if (dwFileSystemFlags & FILE_VOLUME_QUOTAS)
	{
		printf("The file system supports disk quotas. \n");
	}
	if (dwFileSystemFlags & FILE_CASE_SENSITIVE_SEARCH)
	{
		printf("The file system supports case-sensitive file names.\n");
	}
	printf("...\n");
	return TRUE;
}

int main(void)
{
	TCHAR szLogicalDriveStrings[BUFSIZE];
	PTSTR szDrive;

	ZeroMemory(szLogicalDriveStrings, BUFSIZE);
	GetLogicalDriveStrings(BUFSIZE - 1, szLogicalDriveStrings);
	szDrive = (PTSTR)szLogicalDriveStrings;

	do
	{
		if (!GetDriverInfo(szDrive))
		{
			printf("\nGet Volume Information Error: %d", GetLastError());
		}
		szDrive += _tcslen(szDrive) + 1;
	}while (*szDrive != '\x00');
}