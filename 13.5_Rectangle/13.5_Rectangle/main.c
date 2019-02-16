#include<windows.h>
#include<stdio.h>
#include<tchar.h>

#include "resource.h"

HDC hDcCompat;
POINT pt;
RECT rcTarget;
RECT rcClient;

BOOL fSizeEllipse;
BOOL fDrawEllipse;
BOOL fDrawRectangle;
BOOL fSizeRectangle;
BOOL fSizeRoundRect;
BOOL fDrawRoundRect;

int nEllipseWidth;
int nEllipseHeight;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	POINT ptClientUL;
	POINT ptClientLR;

	switch (uMsg)
	{
	case WM_COMMAND:
		switch (wParam)
		{
		case ID_SHAPE_ELLIPSE:
			fSizeEllipse = TRUE;
			break;
		case ID_SHAPE_RECTANGLE:
			fSizeRectangle = TRUE;
			break;
		case ID_SHAPE_ROUNDRECT:
			fSizeRoundRect = TRUE;
			break;
		default:
			break;
		}
		break;
	case WM_CREATE:
		nEllipseWidth = 20;
		nEllipseHeight = 20;
		return 0;
	case WM_PAINT:
		BeginPaint(hWnd, &ps);

		SelectObject(ps.hdc, GetStockObject(GRAY_BRUSH));
		if (fDrawEllipse)
		{
			Ellipse(ps.hdc, rcTarget.left, rcTarget.top, rcTarget.right, rcTarget.bottom);
			fDrawEllipse = FALSE;
			rcTarget.left = rcTarget.right = 0;
			rcTarget.top = rcTarget.bottom = 0;
		}
		if (fDrawRectangle)
		{
			Rectangle(ps.hdc, rcTarget.left, rcTarget.top, rcTarget.right, rcTarget.bottom);
			fDrawRectangle = FALSE;
			rcTarget.left = rcTarget.right = 0;
			rcTarget.top = rcTarget.bottom = 0;
		}
		if (fDrawRoundRect)
		{
			RoundRect(ps.hdc, rcTarget.left, rcTarget.top, rcTarget.right, rcTarget.bottom, nEllipseWidth, nEllipseHeight);
			fDrawRectangle = FALSE;
			rcTarget.left = rcTarget.right = 0;
			rcTarget.top = rcTarget.bottom = 0;
		}
		EndPaint(hWnd, &ps);
		break;

	case WM_SIZE:
		GetClientRect(hWnd, &rcClient);
		ptClientUL.x = rcClient.left;
		ptClientUL.y = rcClient.top;
		ptClientLR.x = rcClient.right;
		ptClientLR.y = rcClient.bottom;
		ClientToScreen(hWnd, &ptClientUL);
		ClientToScreen(hWnd, &ptClientLR);
		SetRect(&rcClient, ptClientUL.x, ptClientUL.y, ptClientLR.x, ptClientLR.y);
		return 0;

	case WM_LBUTTONDOWN:
		ClipCursor(&rcClient);
		pt.x = (LONG)LOWORD(lParam);
		pt.y = (long)HIWORD(lParam);

		if (fDrawEllipse)
		{
			fSizeEllipse = TRUE;
		}
		return 0;

	case WM_MOUSEMOVE:
		if ((wParam && MK_LBUTTON) && (fSizeEllipse || fSizeRectangle || fSizeRoundRect))
		{
			hDC = GetDC(hWnd);
			if (!IsRectEmpty(&rcTarget))
			{
				Rectangle(hDC, rcTarget.left, rcTarget.top, rcTarget.right, rcTarget.bottom);
			}

			if ((pt.x < (LONG)LOWORD(lParam)) && (pt.y > (LONG)HIWORD(lParam)))
			{
				SetRect(&rcTarget, pt.x, HIWORD(lParam), LOWORD(lParam), pt.y);
			}
			else if ((pt.x > (LONG)LOWORD(lParam)) && (pt.y > (LONG)HIWORD(lParam)))
			{
				SetRect(&rcTarget, LOWORD(lParam), HIWORD(lParam), pt.x, pt.y);
			}
			else if ((pt.x > (LONG)LOWORD(lParam)) && (pt.y < (LONG)HIWORD(lParam)))
			{
				SetRect(&rcTarget, LOWORD(lParam), pt.y, pt.x, HIWORD(lParam));
			}
			else
			{
				SetRect(&rcTarget, pt.x, pt.y, LOWORD(lParam), HIWORD(lParam));
			}

			Rectangle(hDC, rcTarget.left, rcTarget.top, rcTarget.right, rcTarget.bottom);
			DeleteDC(hDC);
		}
		return 0;
	case WM_LBUTTONUP:
		if (fSizeEllipse)
		{
			fSizeEllipse = FALSE;
			fDrawEllipse = TRUE;
		}
		if (fSizeRectangle)
		{
			fSizeRectangle = FALSE;
			fDrawRectangle = TRUE;
		}
		if (fSizeRoundRect)
		{
			fSizeRoundRect = FALSE;
			fDrawRoundRect = TRUE;
		}

		if (fDrawEllipse || fDrawRectangle || fDrawRoundRect)
		{
			InvalidateRect(hWnd, &rcTarget, TRUE);
			UpdateWindow(hWnd);
		}

		ClipCursor(NULL);
		return 0;

	case WM_DESTROY:
		DeleteDC(hDcCompat);
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
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_SHAPE);
	wc.lpszClassName = _T("shape");

	if (!RegisterClass(&wc))
	{
		return FALSE;
	}

	hWnd = CreateWindow(
		_T("shape"),
		_T("shape"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		NULL,
		NULL,
		hInstance,
		NULL);
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