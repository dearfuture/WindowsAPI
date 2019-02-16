#include<windows.h>
#include<commctrl.h>
#include<tchar.h>
#include "resource.h"

HINSTANCE g_hInst;
HWND g_hWnd, g_hWndEdit;
LPTSTR lpszCopy = _T("You can input any text\n then click \'Send\' Item at Menu");

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASSEX wcx;

	g_hInst = hInstance;
	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = MainWndProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = hInstance;
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcx.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_COMMAND);
	wcx.lpszClassName = _T("CopyData");
	wcx.hIconSm = NULL;

	if (RegisterClassEx(&wcx) == 0)
	{
		return 0;
	}

	g_hWnd = CreateWindow(
		_T("CopyData"),
		_T("Sample"),
		WS_TILEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 400,
		(HWND)NULL,
		(HMENU)NULL,
		hInstance,
		(LPVOID)NULL);
	if (!g_hWnd)
	{
		return 0;
	}

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
	UNREFERENCED_PARAMETER(lpCmdLine);
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		{
			RECT rectMain;
			GetClientRect(hWnd, &rectMain);
			g_hWndEdit = CreateWindow(
				WC_EDIT,
				NULL,
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
				0,
				0,
				rectMain.right,
				rectMain.bottom,
				hWnd,
				NULL,
				(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				NULL);
			SendMessage(g_hWndEdit, WM_SETTEXT, 0, (LPARAM)lpszCopy);
		}
		
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_COMMAND_SEND)
		{
			DWORD dwTextLength = GetWindowTextLength(g_hWndEdit);
			LPTSTR szText = (LPTSTR)HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, (dwTextLength + 1) * sizeof(TCHAR));
			COPYDATASTRUCT cds;
			HWND hwnd2 = FindWindow(_T("CopyData2"), _T("RECV COPY DATA"));

			GetWindowText(g_hWndEdit, szText, dwTextLength + 1);
			szText[dwTextLength] = _T('\0');
			if (NULL == hwnd2)
			{
				MessageBox(hWnd, _T("cannot find window 2"), _T("ERROR"), MB_OK);
				break;
			}
			cds.dwData = 0x12345678;
			cds.cbData = (dwTextLength + 1) * sizeof(TCHAR);
			cds.lpData = szText;

			SendMessage(hwnd2, (UINT)WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
			HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, szText);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}