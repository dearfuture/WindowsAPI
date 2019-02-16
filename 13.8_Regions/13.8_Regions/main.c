#include<windows.h>
#include<stdio.h>
#include<tchar.h>

LPTSTR szAppName = _T("RGN");
LPTSTR szTitle = _T("RGN Sample");

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HRGN ElliRgns(HWND hWnd, POINT point)
{
	RECT rect, rectClient;
	HRGN hRgn;
	HBRUSH hBrushOld, hBrush;
	HDC hDC = GetDC(hWnd);

	GetClientRect(hWnd, &rectClient);
	rect.left = point.x - 40;
	rect.right = point.x + 40;
	rect.top = point.y - 30;
	rect.bottom = point.y + 30;

	hRgn = CreateEllipticRgnIndirect(&rect);
	hBrush = CreateSolidBrush(RGB(0, 255, 0));
	hBrushOld = (HBRUSH)SelectObject(hDC, hBrush);
	PaintRgn(hDC, hRgn);
	Sleep(3000);
	InvertRgn(hDC, hRgn);

	hBrush = SelectObject(hDC, hBrushOld);
	DeleteObject(hBrush);
	DeleteObject(hRgn);
	DeleteDC(hDC);

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HWND hWnd;
	WNDCLASS wc;
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppName;

	if (!RegisterClass(&wc))
	{
		return FALSE;
	}
	hWnd = CreateWindow(
		szAppName,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
		NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
	UNREFERENCED_PARAMETER(lpCmdLine);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		{
			POINT pt;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			ElliRgns(hWnd, pt);
			return 0;
		}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
