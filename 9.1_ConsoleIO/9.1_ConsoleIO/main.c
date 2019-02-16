#include<windows.h>
#include<stdio.h>
#include<tchar.h>

VOID ScrollScreenBuffer(HANDLE, INT);
VOID HandleInput(LPTSTR szInput);

HANDLE hStdout, hStdin;
CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

VOID ChangeConsoleTitle(LPTSTR szNewTitle)
{
	SetConsoleTitle(szNewTitle);
}

VOID GetConsoleInfo(HANDLE hOutput)
{
	CONSOLE_FONT_INFO cfi;
	DWORD dwProcessList[32];
	DWORD dwAttachedProcess;
	TCHAR szOutputBuffer[1024];
	HWND hConsoleWindows;
	DWORD dwWritten;
	DWORD i;
	TCHAR szConsoleTitle[MAX_PATH];

	dwAttachedProcess = GetConsoleProcessList(dwProcessList, 32);
	if (0 == dwAttachedProcess)
	{
		perror("GetConsoleProcessList");
		ExitProcess(-1);
	}

	GetConsoleTitle(szConsoleTitle, MAX_PATH);
	hConsoleWindows = GetConsoleWindow();
	GetCurrentConsoleFont(hOutput, FALSE, &cfi);

	_stprintf(szOutputBuffer, _T("Now %u attached Processes: "), dwAttachedProcess);
	for (i = 0; i < dwAttachedProcess; i++)
	{
		_stprintf(szOutputBuffer + _tcslen(szOutputBuffer), _T("%u, "), dwProcessList[i]);
	}

	_stprintf(szOutputBuffer + _tcslen(szOutputBuffer),
		_T("\nConsoleTitle is %s, \nWindow Handle is 0x%.8X\n")
		_T("Font is %u, Font Size X = %u, Y = %u\n"),
		   szConsoleTitle, hConsoleWindows, cfi.nFont, cfi.dwFontSize.X, cfi.dwFontSize.Y);
	WriteConsole(hOutput, szOutputBuffer, _tcslen(szOutputBuffer), &dwWritten, NULL);
}

VOID ChangeTextColor(HANDLE hStdout, WORD wColor)
{
	SetConsoleTextAttribute(hStdout, wColor);
}

VOID ChangeBackgroundColor(HANDLE hConsole)
{
	COORD coord;
	WORD wColor;
	DWORD cWritten;
	BOOL fSuccess;
	coord.X = 0;
	coord.Y = 0;

	wColor = BACKGROUND_BLUE | BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	fSuccess = FillConsoleOutputAttribute(hConsole, wColor, 80 * 50, coord, &cWritten);
}

VOID ChangeUpCaseTextBackgroundColor(HANDLE hStdin, HANDLE hStdout)
{
	DWORD dwLen, i, dwRead, dwWritten;
	WORD wColors[3];
	COORD coord;
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	PTCHAR lpCharacters = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 2048);
	coord.X = 0;
	coord.Y = 0;
	ReadConsoleOutputCharacter(hStdout, lpCharacters, 2047, coord, &dwRead);
	GetConsoleScreenBufferInfo(hStdout, &csbi);
	dwLen = _tcslen(lpCharacters);

	wColors[0] = BACKGROUND_RED;
	wColors[1] = BACKGROUND_RED | BACKGROUND_GREEN;
	wColors[2] = BACKGROUND_GREEN;
	for (i = 0; i < dwLen; i++)
	{
		if (lpCharacters[i] >= 'A' && lpCharacters[i] <= 'Z')
		{
			coord.Y = (SHORT)(i / csbi.dwSize.X);
			coord.X = (SHORT)(i % csbi.dwSize.X);

			WriteConsoleOutputAttribute(hStdout, wColors, 3, coord, &dwWritten);
		}
	}
}

void cls(HANDLE hConsole)
{
	COORD coordScreen = {0, 0};
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return ;
	}

	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
	SetConsoleCursorPosition(hConsole, coordScreen);
}

VOID UseBlockIO(void)
{
	HANDLE hStdout, hNewScreenBuffer;
	SMALL_RECT srctReadRect;
	SMALL_RECT srctWriteRect;
	CHAR_INFO chiBuffer[160];
	COORD coordBufSize;
	COORD coordBufCoord;

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	hNewScreenBuffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL);
	SetConsoleActiveScreenBuffer(hNewScreenBuffer);
	
	srctReadRect.Top = 0;
	srctReadRect.Left = 0;
	srctReadRect.Bottom = 1;
	srctReadRect.Right = 79;

	coordBufSize.Y = 2;
	coordBufSize.X = 80;
	coordBufCoord.X = 0;
	coordBufCoord.Y = 0;
	ReadConsoleOutput(
		hStdout,
		chiBuffer,
		coordBufSize,
		coordBufCoord,
		&srctReadRect);

	srctWriteRect.Top = 10;
	srctWriteRect.Left = 0;
	srctWriteRect.Bottom = 11;
	srctWriteRect.Right = 79;
	WriteConsoleOutput(hNewScreenBuffer,
		chiBuffer,
		coordBufSize,
		coordBufCoord,
		&srctWriteRect);
	Sleep(10000);
	SetConsoleActiveScreenBuffer(hStdout);
}

void ScrollScreenBuffer(HANDLE h, INT x)
{
	SMALL_RECT srctScrollRect, srctClipRect;
	CHAR_INFO chiFill;
	COORD coordDest;

	srctScrollRect.Left = 0;
	srctScrollRect.Top = 1;
	srctScrollRect.Right = csbiInfo.dwSize.X - (SHORT)x; 
	srctScrollRect.Bottom = csbiInfo.dwSize.Y - (SHORT)x; 

	// The destination for the scroll rectangle is one row up. 
	coordDest.X = 0; 
	coordDest.Y = 0; 

	// The clipping rectangle is the same as the scrolling rectangle. 
	// The destination row is left unchanged. 
	srctClipRect = srctScrollRect; 

	// Set the fill character and attributes. 
	chiFill.Attributes = FOREGROUND_RED|FOREGROUND_INTENSITY; 
	chiFill.Char.AsciiChar = (char)' '; 

	// Scroll up one line. 
	ScrollConsoleScreenBuffer( 
		h,               // screen buffer handle 
		&srctScrollRect, // scrolling rectangle 
		&srctClipRect,   // clipping rectangle 
		coordDest,       // top left destination cell 
		&chiFill);       // fill character and color 
}

VOID NewLine(VOID)
{
	GetConsoleScreenBufferInfo(hStdout, &csbiInfo);
	csbiInfo.dwCursorPosition.X = 0;
	if (csbiInfo.dwCursorPosition.Y == (csbiInfo.dwSize.Y - 1))
	{
		ScrollScreenBuffer(hStdout, 1);
	}
	else
	{
		csbiInfo.dwCursorPosition.Y += 1;
	}

	SetConsoleCursorPosition(hStdout, csbiInfo.dwCursorPosition);
}

VOID ChangeMode(HANDLE hStdin, HANDLE hStdout)
{
	LPTSTR lpszPrompt = _T("Mode changed Type any key, or q to quit: ");
	TCHAR chBuffer[256];
	DWORD fdwMode, fdwOldMode;
	DWORD cRead, cWritten;

	GetConsoleMode(hStdin, &fdwOldMode);
	fdwMode = fdwOldMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
	SetConsoleMode(hStdin, fdwMode);

	NewLine();
	while (1)
	{
		WriteFile(hStdout, lpszPrompt, _tcslen(lpszPrompt), &cWritten, NULL);
		ReadFile(hStdin, chBuffer, 1, &cRead, NULL);
		if (chBuffer[0] == _T('\r'))
		{
			NewLine();
		}
		else
		{
			WriteFile(hStdout, chBuffer, cRead, &cWritten, NULL);
		}

		if (_T('q') == chBuffer[0])
		{
			break;
		}
	}

	SetConsoleMode(hStdin, fdwOldMode);
}

VOID KeyEventProc(KEY_EVENT_RECORD ker)
{
	TCHAR szMsg[256];
	_stprintf(szMsg, _T("KEY_EVENT_RECORD\n Char = %c"), ker.uChar);
	MessageBox(NULL, szMsg, _T("EVENT"), MB_OK);
}
VOID MouseEventProc(MOUSE_EVENT_RECORD mer)
{
	TCHAR szMsg[256];
	_stprintf(szMsg, _T("MOUSE_EVENT_RECORD\n button state: %d\nmouse position X=%u, Y=%u"),
		mer.dwButtonState, mer.dwMousePosition.X, mer.dwMousePosition.Y);
	MessageBox(NULL, szMsg, _T("EVENT"), MB_OK);
}
VOID ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD wbsr)
{
	TCHAR szMsg[256];
	_stprintf(szMsg, _T("WINDOW_BUFFER_SIZE_RECORD\nX = %u, Y = %u"),
		wbsr.dwSize.X, wbsr.dwSize.Y);
	MessageBox(NULL, szMsg, _T("EVENT"), MB_OK);
}

DWORD UseEvent(VOID)
{
	HANDLE hStdin;
	DWORD cNumRead, fdwMode, fdwSaveOldMode, i;
	INPUT_RECORD irInBuf[128];

	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(hStdin, &fdwSaveOldMode);
	fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	SetConsoleMode(hStdin, fdwMode);

	while (1)
	{
		ReadConsoleInput(hStdin, irInBuf, 128, &cNumRead);
		for (i = 0; i < cNumRead; i++)
		{
			switch (irInBuf[i].EventType)
			{
			case KEY_EVENT:
				KeyEventProc(irInBuf[i].Event.KeyEvent);
				break;
			case MOUSE_EVENT:
				MouseEventProc(irInBuf[i].Event.MouseEvent);
				break;
			case WINDOW_BUFFER_SIZE_EVENT:
				ResizeEventProc(irInBuf[i].Event.WindowBufferSizeEvent);
				break;
			case FOCUS_EVENT:
			case MENU_EVENT:
				break;
			default:
				break;
			}
		}
	}

	return 0;
}

int main(void)
{
	LPTSTR lpszPrompt = _T("Type a line and press Enter, 255 max \n\
		q to quit\n\
		i to Show console info\n\
		c to Change background color\n\
		s to Change text color\n\
		t to Find upper case character\n\
		b to Call UseBlockIO\n\
		u to Change console title\n\
		m to Show console event\n\
		r to Change mode\n\
		p to Clear up Console\n"
		);
	TCHAR chBuffer[256];
	DWORD cRead, cWritten;
	WORD wOldColorAttrs;

	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (INVALID_HANDLE_VALUE == hStdin || INVALID_HANDLE_VALUE == hStdout)
	{
		MessageBox(NULL, _T("GetStdHandle"), _T("Console Error"), MB_OK);
		return ;
	}

	if (!GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
	{
		MessageBox(NULL, _T("GetConsoleScreenBufferInfo"), _T("Console Error"), MB_OK);
		return;
	}

	wOldColorAttrs = csbiInfo.wAttributes;
	ChangeTextColor(hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY);
	if (!WriteConsole(hStdout, lpszPrompt, _tcslen(lpszPrompt), &cWritten, NULL))
	{
		MessageBox(NULL, _T("WriteFile"), _T("Consoel Error"), MB_OK);
		return ;
	}

	while (1)
	{
		ZeroMemory(chBuffer, 256);
		if (!ReadConsole(hStdin,
			chBuffer,
			255,
			&cRead,
			NULL))
		{
			break;
		}

		HandleInput(chBuffer);
	}
}

VOID HandleInput(LPTSTR szInput)
{
	switch (szInput[0])
	{
	case 'q':
		ExitProcess(0);
		break;
	case 'i':
		GetConsoleInfo(hStdout);
		break;
	case 'c':
		ChangeBackgroundColor(hStdout);
		break;
	case 's':
		ChangeTextColor(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case 't':
		ChangeUpCaseTextBackgroundColor(hStdin, hStdout);
		break;
	case 'b':
		UseBlockIO();
		break;
	case 'u':
		ChangeConsoleTitle(&szInput[2]);
		break;
	case 'm':
		UseEvent();
		break;
	case 'r':
		ChangeMode(hStdin, hStdout);
		break;
	case 'p':
		cls(hStdout);
		break;
	default:
		break ;
	}

	return ;
}