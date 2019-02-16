#include<windows.h>
#include<tchar.h>

#define MAXGUIDESEGMENTS 1000
#define MyAlloc(dwSize)		HeapAlloc(GetProcessHeap(), 0, dwSize)
#define MyFree(lpMem)		HeapFree(GetProcessHeap(), 0, lpMem)

BOOL GetGuideLine(HWND, LPPOINT*, LPDWORD);
BOOL ShowGuide(HDC, LPPOINT, DWORD);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD GetLastErrorBox(HWND hWnd, LPTSTR lpTitle);

HINSTANCE g_hInst;
LPTSTR szAppName = _T("Curves");
LPTSTR szTitle = _T("Curves Application");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevIntance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HWND hWnd;
	WNDCLASS wc;

	g_hInst = hInstance;
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)WndProc;
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
		GetLastErrorBox(NULL, _T("Error in RegisterClass"));
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
		GetLastErrorBox(hWnd, _T("Error in CreateWindow"));
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)(msg.wParam);
	UNREFERENCED_PARAMETER(lpCmdLine);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL bOn = TRUE;
	static LPPOINT lpBlue = NULL;
	static LPPOINT lpRed = NULL;
	static DWORD dwBlue = 0;
	static DWORD dwRed = 0;
	static BOOL bOutLineOnly = FALSE;
	static BOOL bShowGuides = TRUE;
	static HPEN hPenBlue, hPenRed;

	switch (message)
	{
	case WM_CREATE:
		{
			HDC hDC = GetDC(hWnd);
			hPenBlue = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
			hPenRed = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		}
		GetLastErrorBox(hWnd, _T("Error in WM_CREATE"));
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hWnd, &ps);

			RECT rect;
			GetClientRect(hWnd, &rect);
			PatBlt(hDC, 0, 0, rect.right, rect.bottom, WHITENESS);
			if (dwBlue && dwRed)
			{
				if (dwBlue && bShowGuides)
				{
					SelectObject(hDC, hPenBlue);
					ShowGuide(hDC, lpBlue, dwBlue);
					SelectObject(hDC, GetStockObject(BLACK_PEN));
				}
				if (dwRed && bShowGuides)
				{
					SelectObject(hDC, hPenRed);
					ShowGuide(hDC, lpRed, dwRed);
					SelectObject(hDC, GetStockObject(BLACK_PEN));
				}
			}
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			HDC hDC = GetDC(hWnd);
			RECT rect;
			if (bOn)
			{
				GetClientRect(hWnd, &rect);
				PatBlt(hDC, 0, 0, rect.right, rect.bottom, WHITENESS);
				if (lpBlue)
				{
					MyFree(lpBlue);
				}
				if (lpRed)
				{
					MyFree(lpRed);
				}
				dwRed = 0;
				dwBlue = 0;
				
				SelectObject(hDC, hPenBlue);
				GetGuideLine(hWnd, &lpBlue, &dwBlue);
			}
			else
			{
				SelectObject(hDC, hPenRed);
				GetGuideLine(hWnd, &lpRed, &dwRed);
			}

			SelectObject(hDC, GetStockObject(BLACK_PEN));
			bOn = !bOn;
		}
		GetLastErrorBox(hWnd, _T("Error in WM_LBUTTONDOWN"));
		break;
	case WM_DESTROY:
		if (lpBlue)
		{
			MyFree(lpBlue);
		}
		if (lpRed)
		{
			MyFree(lpRed);
		}
		PostQuitMessage(0);
		return TRUE;
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

BOOL GetGuideLine(HWND hWnd, LPPOINT *lpPoint, LPDWORD lpdwNumPts)
{
	MSG msg;
	HDC hDC = GetDC(hWnd);
	BOOL bFirstTime = TRUE;
	DWORD dwPos = 0;
	RECT rect;

	SetCapture(hWnd);
	GetClientRect(hWnd, &rect);

	*lpPoint = (LPPOINT)MyAlloc(MAXGUIDESEGMENTS * sizeof(POINT));
	for (;;)
	{
		WaitMessage();
		if (PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE))
		{
			if ((LOWORD(msg.lParam) < rect.right) && (HIWORD(msg.lParam) < rect.bottom))
			{
				if (bFirstTime)
				{
					bFirstTime = FALSE;
					MoveToEx(hDC, LOWORD(msg.lParam), HIWORD(msg.lParam), NULL);
				}
				if (dwPos < MAXGUIDESEGMENTS)
				{
					(*lpPoint)[dwPos].x = LOWORD(msg.lParam);
					(*lpPoint)[dwPos].y = HIWORD(msg.lParam);
					LineTo(hDC, (*lpPoint)[dwPos].x, (*lpPoint)[dwPos].y);
					dwPos++;
				}
			}

			if (msg.message == WM_LBUTTONUP)
			{
				break;
			}
		}
	}

	*lpdwNumPts = dwPos;
	ReleaseDC(hWnd, hDC);
	ReleaseCapture();
	DeleteDC(hDC);

	return TRUE;
}

BOOL ShowGuide(HDC hDC, LPPOINT lpPoints, DWORD dwNumPts)
{
	Polyline(hDC, lpPoints, dwNumPts);
	return TRUE;
}

DWORD GetLastErrorBox(HWND hWnd, LPTSTR lpTitle)
{
	LPVOID lpv;
	DWORD dwRv;
	if (GetLastError() == 0)
	{
		return 0;
	}

	dwRv = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		(LPVOID)&lpv,
		0,
		NULL);
	MessageBox(hWnd, lpv, lpTitle, MB_OK);
	if (dwRv)
	{
		LocalFree(lpv);
	}
	SetLastError(0);

	return dwRv;
}

