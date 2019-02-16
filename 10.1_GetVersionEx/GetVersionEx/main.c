#include<windows.h>
#include<stdio.h>
#include<tchar.h>
#include<LMCons.h>

void ShowVersionInfo(void)
{
	OSVERSIONINFO over;
	TCHAR szVersionInfo[1024] = {0};
	szVersionInfo[0] = '\0';

	over.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&over);

	if (over.dwMajorVersion == 5)
	{
		if (over.dwMinorVersion == 0)
		{
			_tcscat(szVersionInfo, _T("Windows 2000 "));
		}
		else if (over.dwMinorVersion == 1)
		{
			_tcscat(szVersionInfo, _T("Windows XP "));
		}
		else if (over.dwMinorVersion == 2)
		{
			_tcscat(szVersionInfo, _T("Windows Server 2003 "));
		}
	}
	else if (over.dwMajorVersion == 6)
	{
		_tcscat(szVersionInfo, _T("Windows Vista "));
	}
	else
	{
		_tcscat(szVersionInfo, _T("Windows NT 4.0 or other"));
	}

	_tcscat(szVersionInfo, over.szCSDVersion);

	_tprintf(_T("%s\n"), szVersionInfo);
}

void ShowSystemInfo(void)
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	_tprintf(_T("Memory page size: 0x%.8X, Available memory start: 0x%.8X, Available memory end: 0x%.8X, \n")
			 _T("CPU count: %d, CPU type: "),
				si.dwPageSize,
				si.lpMinimumApplicationAddress,
				si.lpMaximumApplicationAddress,
				si.dwNumberOfProcessors);
	switch (si.dwProcessorType)
	{
	case PROCESSOR_INTEL_386:
		printf("386");
		break;
	case PROCESSOR_INTEL_486:
		printf("486");
		break;
	case PROCESSOR_INTEL_PENTIUM:
		printf("pentium");
		printf(", cpu model 0x%.2X, Stepping 0x%.2X", (BYTE)(si.wProcessorRevision >> 8), (BYTE)si.wProcessorRevision);
		break;
	}

	printf("\nCPU Architecture: ");
	switch (si.wProcessorArchitecture)
	{
	case PROCESSOR_ARCHITECTURE_INTEL:
		printf("intel");
		printf(" CPU vendor is %d", si.wProcessorLevel);
		break;
	case PROCESSOR_ARCHITECTURE_IA64:
		printf("64 bits intel");
		break;
	case PROCESSOR_ARCHITECTURE_AMD64:
		printf("64 bits AMD");
		break;
	case PROCESSOR_ARCHITECTURE_UNKNOWN:
		printf("UNKNOWN");
		break;
	}

	printf("\n");
}

void GetFolders(void)
{
	TCHAR szSystemDirectory[MAX_PATH];
	TCHAR szWindowsDirectory[MAX_PATH];

	GetSystemDirectory(szSystemDirectory, MAX_PATH);
	GetWindowsDirectory(szWindowsDirectory, MAX_PATH);
	_tprintf(_T("System directory: \t%s\nWindows directory: \t%s\n"), szSystemDirectory, szWindowsDirectory);
}

void GetNames(void)
{
	DWORD dwComputerNameLen = MAX_COMPUTERNAME_LENGTH + 1;
	DWORD dwUserNameLen = UNLEN + 1;
	TCHAR szComputerName[MAX_COMPUTERNAME_LENGTH + 1];
	TCHAR szUserName[UNLEN + 1];

	GetComputerName(szComputerName, &dwComputerNameLen);
	_tprintf(_T("Computer Name: %s\n"), szComputerName);

	dwComputerNameLen = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerNameEx(ComputerNameDnsHostname, szComputerName, &dwComputerNameLen);
	_tprintf(_T("ComputerNameDnsHostName: %s\n"), szComputerName);

	dwComputerNameLen = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerNameEx(ComputerNamePhysicalNetBIOS, szComputerName, &dwComputerNameLen);

	GetUserName(szUserName, &dwUserNameLen);
	_tprintf(_T("UserName: %s\n"), szUserName);
}

void MouseSpeed(void)
{
	BOOL fResult;
	int aMouseInfo[3];

	fResult = SystemParametersInfo(
		SPI_GETMOUSE,
		0,
		&aMouseInfo,
		0);
	if (fResult)
	{
		aMouseInfo[2] = 2 * aMouseInfo[2];
		SystemParametersInfo(
			SPI_SETMOUSE,
			0,
			aMouseInfo,
			SPIF_SENDCHANGE);
	}
}

int main(void)
{
	ShowVersionInfo();
	ShowSystemInfo();
	GetFolders();
	MouseSpeed();
}
