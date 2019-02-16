#include<windows.h>
#include<math.h>
#include<tchar.h>
#include "resource.h"

CHOOSEFONT g_cf;
LOGFONT g_lf;
HFONT g_hfont;
HFONT g_hfontOld;
HDC g_hdc;
int g_nXStart, g_nYStart;
RECT g_rc;
double g_aflSin[90];
double g_aflCos[90];
double g_flRadius, g_a;
int g_iMode;
HRGN g_hrgn;
COLORREF g_crOld;

LRESULT CALLBACK MainWndproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;

	switch (uMsg)
	{
	case WM_PAINT:
		g_hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDM_VANISH:
			g_hdc = GetDC(hWnd);
			GetClientRect(hWnd, &g_rc);
			FillRect(g_hdc, &g_rc, GetStockObject(WHITE_BRUSH));
			ReleaseDC(hWnd, g_hdc);
			DeleteDC(g_hdc);
			break;
		case IDM_AND:
			g_iMode = RGN_AND;
			break;
		case IDM_COPY:
			g_iMode = RGN_COPY;
			break;
		case IDM_DIFF:
			g_iMode = RGN_DIFF;
			break;
		case IDM_OR:
			g_iMode = RGN_OR;
			break;
		case IDM_XOR:
			g_iMode = RGN_XOR;
			break;
		case IDM_CLIP:
			{
				POINT points[5] = {{0, 200}, {600, 200}, {100, 600}, {300, 0}, {500, 600}};
				LPTSTR szOut = _T("Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
				RECT rect;
				int i;

				g_hdc = GetDC(hWnd);
				g_hfont = CreateFontIndirect(g_cf.lpLogFont);
				g_hfontOld = SelectObject(g_hdc, g_hfont);

				g_hrgn = CreatePolygonRgn(points, 5, WINDING);
				SelectClipRgn(g_hdc, g_hrgn);
				GetClientRect(hWnd, &rect);
				SetBkMode(g_hdc, TRANSPARENT);

				BeginPath(g_hdc);
				TextOut(g_hdc, 0, rect.bottom / 2, szOut, _tcslen(szOut));
				EndPath(g_hdc);

				SelectClipPath(g_hdc, g_iMode);

				for (i = 0; i < 90; i++)
				{
					g_aflSin[i] = sin(i * 1.0 / 180.0 * 3.14159);
					g_aflCos[i] = cos(i * 1.0 / 180.0 * 3.14159);
				}

				g_flRadius = 1000;
				for (i = 0; i < 90; i++)
				{
					MoveToEx(g_hdc, g_nXStart, g_nYStart, NULL);
					LineTo(g_hdc, g_nXStart + (int)(g_flRadius * g_aflCos[i]), g_nYStart + (int)(g_flRadius * g_aflSin[i]));
				}
				SelectObject(g_hdc, g_hfontOld);
				DeleteObject(g_hfont);
				ReleaseDC(hWnd, g_hdc);

				UpdateWindow(hWnd);
			}
			break;
		case IDM_FONT:
			g_cf.lStructSize = sizeof(CHOOSEFONT);
			g_cf.hwndOwner = hWnd;
			g_cf.lpLogFont = &g_lf;
			g_cf.Flags = CF_SCREENFONTS | CF_EFFECTS;
			g_cf.rgbColors = RGB(0, 255, 255);
			g_cf.nFontType = SCREEN_FONTTYPE;
			ChooseFont(&g_cf);
			break;
		default:
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HWND hWnd;
	WNDCLASS wc;

	wc.style = CS_OWNDC;
	wc.lpfnWndProc = MainWndproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_CLIP);
	wc.lpszClassName = _T("clip");
	RegisterClass(&wc);

	hWnd = CreateWindow(
		wc.lpszClassName,
		_T("clip"),
		WS_OVERLAPPEDWINDOW,
		100, 100, 600, 650,
		NULL, NULL, hInstance, NULL);
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