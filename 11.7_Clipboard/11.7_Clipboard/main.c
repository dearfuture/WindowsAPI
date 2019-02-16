#include<windows.h>
#include<string.h>
#include<stdio.h>
#include<tchar.h>
#include "resource.h"

#define MY_CLIP_FORMAT		CF_UNICODETEXT

HINSTANCE hInst;
HWND hwnd;
LPTSTR lpszText = NULL;
LPTSTR lpData = NULL;
HANDLE hHeap;
LPTSTR szInitialClientAreaText =
	_T("This program demonstrates the use of the Edit menu to copy and \n")
	_T("paste text ot and from the clipboard. Try using the Copy command \n")
	_T("to move this text to the clipboard, and the Paste command to replace \n")
	_T("this text with data from another application. \r\n\r\n")
	_T("You might watn to try running Notepad and Clipbrd alongside this \n")
	_T("aplication so that you can watch the data exchanges take place. \n");

BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, INT);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
VOID OutOfMemory(VOID);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	BOOL fGotMessage;
	
	if (!InitApplication(hInstance))
	{
		return FALSE;
	}
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	while ((fGotMessage = GetMessage(&msg, (HWND)hwnd, 0, 0)) != 0 && fGotMessage != -1)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
	UNREFERENCED_PARAMETER(lpCmdLine);
}

BOOL InitApplication(HANDLE hInstance)
{
	WNDCLASS wc;

	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _T("CliptextWClass");
	wc.style = 0;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	return RegisterClass(&wc);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;
	hHeap = GetProcessHeap();
	lpszText = HeapAlloc(hHeap, 0, (_tcslen(szInitialClientAreaText) + 1) * sizeof(TCHAR));
	_tcscpy(lpszText, szInitialClientAreaText);

	hwnd = CreateWindow( _T("CliptextWClass"),
		_T("Cliptext Sample Application"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		NULL,
		(HMENU)LoadMenu(hInstance, MAKEINTRESOURCE(ID_CLIP_MENU)),
		hInstance,
		NULL);
	if (!hwnd)
	{
		return FALSE;
	}

	ShowWindow(hwnd, nCmdShow);
	return UpdateWindow(hwnd);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hDC = NULL;
	PAINTSTRUCT ps = {0};
	RECT rectClient = {0};
	LPTSTR lpClipData = NULL;

	switch (message)
	{
	case WM_INITMENU:
		if (wParam == (WPARAM)GetMenu(hWnd))
		{
			if (OpenClipboard(hWnd))
			{
				if (IsClipboardFormatAvailable(MY_CLIP_FORMAT) || IsClipboardFormatAvailable(CF_OEMTEXT))
				{
					EnableMenuItem((HMENU)wParam, IDM_PASTE, MF_ENABLED);
				}
				else
				{
					EnableMenuItem((HMENU)wParam, IDM_PASTE, MF_GRAYED);
				}

				CloseClipboard();
			}
		}
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_ABOUT:
			break;
		case IDM_NEW:
		case IDM_OPEN:
		case IDM_SAVE:
		case IDM_SAVEAS:
		case IDM_PRINT:
		case IDM_UNDO:
		case IDM_CLEAR:
			MessageBox(GetFocus(), _T("Command not implemented."), _T("ClipText Sample Application"), MB_ICONASTERISK | MB_OK);
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_CUT:
		case IDM_COPY:
			if (NULL != lpszText)
			{
				if (NULL != lpData)
				{
					HeapFree(hHeap, 0, lpData);
				}
				lpData = HeapAlloc(hHeap, 0, HeapSize(hHeap, 0, lpszText));
				_tcscpy(lpData, lpszText);
				if (OpenClipboard(hWnd))
				{
					EmptyClipboard();
					SetClipboardData(MY_CLIP_FORMAT, lpData);
					CloseClipboard();
				}

				if (LOWORD(wParam) == IDM_CUT)
				{
					HeapFree(hHeap, 0, lpszText);
					lpszText = NULL;
					EnableMenuItem(GetMenu(hWnd), IDM_CUT, MF_GRAYED);
					EnableMenuItem(GetMenu(hWnd), IDM_COPY, MF_GRAYED);
					InvalidateRect(hWnd, NULL, TRUE);
					UpdateWindow(hWnd);
				}
			}
			return TRUE;
		case IDM_PASTE:
			if (OpenClipboard(hWnd))
			{
				if (!(lpClipData = GetClipboardData(MY_CLIP_FORMAT)))
				{
					CloseClipboard();
					break;
				}
				if (NULL != lpszText)
				{
					HeapFree(hHeap, 0, lpszText);
					lpszText = NULL;
				}

				lpszText = HeapAlloc(hHeap, 0, (_tcslen(lpClipData) + 1) * sizeof(TCHAR));
				_tcscpy(lpszText, lpClipData);
				CloseClipboard();
				EnableMenuItem(GetMenu(hWnd), IDM_CUT, MF_ENABLED);
				EnableMenuItem(GetMenu(hWnd), IDM_COPY, MF_ENABLED);
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
				return TRUE;
			}
			else
			{
				return FALSE;
			}
			break;
		default:
			break;
		}
	case WM_SIZE:
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		if (NULL != lpszText)
		{
			GetClientRect(hWnd, &rectClient);
			DrawText(hDC, lpszText, -1, &rectClient, DT_EXTERNALLEADING | DT_NOPREFIX | DT_WORDBREAK | DT_EXPANDTABS);
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		if (NULL != lpszText)
		{
			HeapFree(hHeap, 0, lpszText);
		}
		if (NULL != lpData)
		{
			HeapFree(hHeap, 0, lpData);
		}
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}