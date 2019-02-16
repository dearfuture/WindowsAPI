#include<windows.h>
#include<commctrl.h>
#include<tchar.h>
#include "resource.h"

HINSTANCE g_hInst;
UINT uFormat = (UINT)-1;
BOOL fAuto = TRUE;
HWND g_hwnd;
LPTSTR szFormatUnicodeText = _T("CF_UNICODETEXT");
LPTSTR szFormatText = _T("CF_TEXT");
LPTSTR szFormatOwnerDisplay = _T("CF_OWNERDISPLAY");
LPTSTR szFormatEnhmetafile = _T("CF_ENHMETAFILE");
LPTSTR szFormatBitMap = _T("CF_BITMAP");

VOID WINAPI InitMenu(HWND hwnd, HMENU hmenu);
BOOL WINAPI IsDisplayableFormat(UINT uFormat);
void WINAPI SetAutoView(HWND hwnd);
BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
LPTSTR GetPredefinedClipboardFormatName(UINT);
LRESULT CALLBACK MainWindProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	if (!InitApplication(hInstance))
	{
		return FALSE;
	}
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	while (GetMessage(&msg, (HWND)NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
	UNREFERENCED_PARAMETER(lpCmdLine);
}

BOOL InitApplication(HINSTANCE hInstance)
{
	WNDCLASSEX wcx;

	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = MainWindProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = hInstance;
	wcx.hIcon = NULL;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcx.lpszMenuName = NULL;
	wcx.lpszClassName = _T("MainWClass");
	wcx.hIconSm = NULL;

	return RegisterClassEx(&wcx);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInst = hInstance;
	g_hwnd = CreateWindow(
		_T("MainWClass"),
		_T("ClipBoardViewer"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUPWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		800,
		600,
		(HWND)NULL,
		(HMENU)LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU_COMMAND)),
		hInstance,
		(LPVOID)NULL);
	if (!g_hwnd)
	{
		return FALSE;
	}
	ShowWindow(g_hwnd, nCmdShow);
	UpdateWindow(g_hwnd);
	return TRUE;
}

LRESULT CALLBACK MainWindProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndNextViewer;
	HDC hdc;
	HDC hdcMem;
	PAINTSTRUCT ps;
	LPPAINTSTRUCT lpps;
	RECT rc;
	LPRECT lprc;
	HGLOBAL hglb;
	LPTSTR lpstr;
	LPSTR szText;
	HBITMAP hbm;
	HENHMETAFILE hemf;
	HWND hwndOwner;

	switch (uMsg)
	{
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		switch (uFormat)
		{
		case CF_OWNERDISPLAY:
			hwndOwner = GetClipboardOwner();
			hglb = GlobalAlloc(GMEM_MOVEABLE, sizeof(PAINTSTRUCT));
			lpps = GlobalLock(hglb);
			memcpy(lpps, &ps, sizeof(PAINTSTRUCT));
			GlobalUnlock(hglb);
			SendMessage(hwndOwner, WM_PAINTCLIPBOARD, (WPARAM)hwnd, (LPARAM)hglb);
			GlobalFree(hglb);
			break;
		case CF_BITMAP:
			hdcMem = CreateCompatibleDC(hdc);
			if (NULL != hdcMem)
			{
				if (OpenClipboard(hwnd))
				{
					hbm = (HBITMAP)GetClipboardData(uFormat);
					SelectObject(hdcMem, hbm);
					GetClientRect(hwnd, &rc);
					BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);
					CloseClipboard();
				}
				DeleteDC(hdcMem);
			}
			break;
		case CF_UNICODETEXT:
			if (OpenClipboard(hwnd))
			{
				hglb = GetClipboardData(uFormat);
				lpstr = GlobalLock(hglb);
				GetClientRect(hwnd, &rc);
				DrawText(hdc, lpstr, -1, &rc, DT_LEFT | DT_EXPANDTABS);
				GlobalUnlock(hglb);
				CloseClipboard();
			}
			break;
		case CF_TEXT:
			if (OpenClipboard(hwnd))
			{
				TCHAR str[4096];
				hglb = GetClipboardData(uFormat);
				szText = GlobalLock(hglb);
				MultiByteToWideChar (CP_ACP, 0, szText, -1, str, 4096);
				GetClientRect(hwnd, &rc);
				DrawText(hdc, str, -1, &rc, DT_LEFT | DT_EXPANDTABS);
				GlobalUnlock(hglb);
				CloseClipboard();
			}
			break;
		case CF_ENHMETAFILE:
			if (OpenClipboard(hwnd))
			{
				hemf = GetClipboardData(uFormat);
				GetClientRect(hwnd, &rc);
				PlayEnhMetaFile(hdc, hemf, &rc);
				CloseClipboard();
			}
			break;
		case 0:
			GetClientRect(hwnd, &rc);
			DrawText(hdc, _T("The clipboard is empty."), -1, &rc, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			break;
		default:
			GetClientRect(hwnd, &rc);
			DrawText(hdc, _T("Unable to display format."), -1, &rc, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
		EndPaint(hwnd, &ps);
		break;
	case WM_SIZE:
		if (CF_OWNERDISPLAY == uFormat)
		{
			hwndOwner = GetClipboardOwner();
			hglb = GlobalAlloc(GMEM_MOVEABLE, sizeof(RECT));
			lprc = GlobalLock(hglb);
			GetClientRect(hwnd, lprc);
			GlobalUnlock(hglb);
			SendMessage(hwndOwner, WM_SIZECLIPBOARD, (WPARAM)hwnd, (LPARAM)hglb);
			GlobalFree(hglb);
		}
		break;
	case WM_CREATE:
		hwndNextViewer = SetClipboardViewer(hwnd);
		break;
	case WM_CHANGECBCHAIN:
		if ((HWND)wParam == hwndNextViewer)
		{
			hwndNextViewer = (HWND)lParam;
		}
		else if (hwndNextViewer != NULL)
		{
			SendMessage(hwndNextViewer, uMsg, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		ChangeClipboardChain(hwnd, hwndNextViewer);
		PostQuitMessage(0);
		return TRUE;
	case WM_DRAWCLIPBOARD:
		SetAutoView(hwnd);
		SendMessage(hwndNextViewer, uMsg, wParam, lParam);
		break;
	case WM_INITMENUPOPUP:
		if (!HIWORD(lParam))
		{
			InitMenu(hwnd, (HMENU)wParam);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_NULL:
			break;
		case ID_AUTO:
			SetAutoView(hwnd);
			break;
		default:
			fAuto = FALSE;
			uFormat = LOWORD(wParam);
			InvalidateRect(hwnd, NULL, TRUE);
		}
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void WINAPI SetAutoView(HWND hwnd)
{
	static UINT auPriorityList[] = {
		CF_OWNERDISPLAY,
		CF_UNICODETEXT,
		CF_TEXT,
		CF_ENHMETAFILE,
		CF_BITMAP
	};

	uFormat = GetPriorityClipboardFormat(auPriorityList, 5);
	fAuto = TRUE;
	InvalidateRect(hwnd, NULL, TRUE);
	UpdateWindow(hwnd);
}

void WINAPI InitMenu(HWND hwnd, HMENU hMenu)
{
	UINT uFormat;
	TCHAR szFormatName[80];
	LPCTSTR lpFormatName;
	UINT fuFlags;
	UINT idMenuItem;

	if (GetMenuItemID(hMenu, 0) != ID_AUTO)
	{
		return ;
	}

	while (GetMenuItemCount(hMenu) > 1)
	{
		DeleteMenu(hMenu, 1, MF_BYPOSITION);
	}
	fuFlags = fAuto ? MF_BYCOMMAND | MF_CHECKED : MF_BYCOMMAND | MF_UNCHECKED;
	CheckMenuItem(hMenu, ID_AUTO, fuFlags);

	if (CountClipboardFormats() == 0)
	{
		return ;
	}
	if (!OpenClipboard(hwnd))
	{
		return ;
	}

	AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
	uFormat = EnumClipboardFormats(0);

	while (uFormat)
	{
		lpFormatName = GetPredefinedClipboardFormatName(uFormat);
		if (NULL == lpFormatName)
		{
			if (GetClipboardFormatName(uFormat, szFormatName, sizeof(szFormatName)))
			{
				lpFormatName = szFormatName;
			}
			else
			{
				lpFormatName = _T("(unknown)");
			}
		}
		if (IsDisplayableFormat(uFormat))
		{
			fuFlags = MF_STRING;
			idMenuItem = uFormat;
		}
		else
		{
			fuFlags = MF_STRING | MF_GRAYED;
			idMenuItem = 0;
		}
		AppendMenu(hMenu, fuFlags, idMenuItem, lpFormatName);
		uFormat = EnumClipboardFormats(uFormat);
	}

	CloseClipboard();
}

LPTSTR GetPredefinedClipboardFormatName(UINT uFormat)
{
	switch (uFormat)
	{
	case CF_OWNERDISPLAY:
		return szFormatOwnerDisplay;
	case CF_UNICODETEXT:
		return szFormatUnicodeText;
	case CF_TEXT:
		return szFormatText;
	case CF_ENHMETAFILE:
		return szFormatEnhmetafile;
	case CF_BITMAP:
		return szFormatBitMap;
	default:
		return NULL;
	}
}

BOOL WINAPI IsDisplayableFormat(UINT uFormat)
{
	switch (uFormat)
	{
	case CF_OWNERDISPLAY:
	case CF_UNICODETEXT:
	case CF_TEXT:
	case CF_ENHMETAFILE:
	case CF_BITMAP:
		return TRUE;
	}

	return FALSE;
}