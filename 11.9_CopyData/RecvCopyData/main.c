#include<windows.h>
#include<commctrl.h>
#include<stdio.h>
#include<tchar.h>

HINSTANCE g_hInst;
HWND g_hWnd, g_hWndEdit;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASSEX wcx;

	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = MainWndProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = hInstance;
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcx.lpszMenuName = NULL;
	wcx.lpszClassName = _T("CopyData2");
	wcx.hIconSm = NULL;

	if (0 == RegisterClassEx(&wcx))
	{
		return FALSE;
	}

	g_hWnd = CreateWindow(
		_T("CopyData2"),
		_T("RECV COPY DATA"),
		WS_TILEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 400,
		(HWND)NULL,
		(HMENU)NULL,
		hInstance,
		NULL);
	if (NULL == g_hWnd)
	{
		return FALSE;
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

	return 0;
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
		}
		break;
	case WM_COPYDATA:
		{
			PCOPYDATASTRUCT lpcds = (PCOPYDATASTRUCT)lParam;
			LPTSTR lpData = (LPTSTR)lpcds->lpData;
			lpData[lpcds->cbData / 2 - 1] = _T('\0');
			SetWindowText(g_hWndEdit, lpData);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}