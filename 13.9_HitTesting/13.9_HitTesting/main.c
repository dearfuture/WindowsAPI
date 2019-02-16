#include<windows.h>
#include<stdio.h>
#include<tchar.h>

RECT rctGrid;
RECT rctBrush;
UINT bBrushBits[8];
RECT rect[64];
HBITMAP hbm;
HBRUSH hBrush;
HBRUSH hBrushGrid[8];
HBRUSH hBrushOld;


void InitGridBurshAndRect(void)
{
	int x, y, deltaX, deltaY, i;

	hBrushGrid[0] = CreateHatchBrush(HS_BDIAGONAL,		RGB(255, 0, 0));
	hBrushGrid[1] = CreateHatchBrush(HS_CROSS,			RGB(0, 255, 0));
	hBrushGrid[2] = CreateHatchBrush(HS_DIAGCROSS,		RGB(0, 0, 255));
	hBrushGrid[3] = CreateHatchBrush(HS_FDIAGONAL,		RGB(255, 0, 255));
	hBrushGrid[4] = CreateHatchBrush(HS_HORIZONTAL,		RGB(255, 255, 0));
	hBrushGrid[5] = CreateHatchBrush(HS_BDIAGONAL,		RGB(0, 255, 255));
	hBrushGrid[6] = CreateHatchBrush(HS_VERTICAL,		RGB(0, 0, 0));
	hBrushGrid[7] = CreateSolidBrush(RGB(200, 200, 200));

	rctGrid.top = rctGrid.left = 10;
	rctGrid.bottom = rctGrid.right = 330;
	rctBrush.top = 10;
	rctBrush.left = 340;
	rctBrush.bottom = 330;
	rctBrush.right = 660;

	deltaX = (rctGrid.right - rctGrid.left) / 8;
	deltaY = (rctGrid.bottom - rctGrid.top) / 8;
	for (y = rctGrid.top, i = 0; y < rctGrid.bottom; y += deltaY)
	{
		for( x = rctGrid.left; x < (rctGrid.right - 8) && i < 64; x += deltaX, i++)
		{
			rect[i].left = x;
			rect[i].top  = y;
			rect[i].right = x + deltaX;
			rect[i].bottom = y + deltaY;
		}
	}
}

VOID DrawGridLine(HWND hWnd)
{
	int i;
	HDC hDC = GetDC(hWnd);

	for (i = rctGrid.left; i <= rctGrid.right; i += (rctGrid.right - rctGrid.left) / 8)
	{
		MoveToEx(hDC, i, rctGrid.top, NULL);
		LineTo(hDC, i, rctGrid.bottom);
	}
	for (i = rctGrid.top; i <= rctGrid.bottom; i += (rctGrid.bottom - rctGrid.top) / 8)
	{
		MoveToEx(hDC, rctGrid.left, i, NULL);
		LineTo(hDC, rctGrid.right, i);
	}

	ReleaseDC(hWnd, hDC);
	DeleteDC(hDC);
}

VOID PaintGrid(HWND hWnd, POINTS ptlHit)
{
	int i;
	HRGN hRgnCell = CreateRectRgn(rctGrid.left, rctGrid.top, rctGrid.right, rctGrid.bottom);
	HDC hDC = GetDC(hWnd);
	SelectObject(hDC, hRgnCell);

	if (PtInRegion(hRgnCell, ptlHit.x, ptlHit.y))
	{
		for (i = 0; i < 8; i++)
		{
			bBrushBits[i] = 0xFF;
		}
		for (i = 0; i < 64; i++)
		{
			DeleteObject(hRgnCell);
			hRgnCell = CreateRectRgn(rect[i].left, rect[i].top, rect[i].right, rect[i].bottom);

			if (PtInRegion(hRgnCell, ptlHit.x, ptlHit.y))
			{
				FillRgn(hDC, hRgnCell, hBrushGrid[i / 8]);
			}
		}

		SelectObject(hDC, hBrushOld);
		DeleteObject(hBrush);
		DeleteObject(hbm);
		DeleteDC(hDC);
	}
}

VOID PaintPattern(HWND hWnd, POINTS ptlHit)
{
	int i;
	HDC hDC = GetDC(hWnd);

	i = ptlHit.x % 8;
	bBrushBits[i] += 0x50;
	hbm = CreateBitmap(8, 8, 1, 1, (LPBYTE)bBrushBits);
	hBrush = CreatePatternBrush(hbm);
	hBrushOld = SelectObject(hDC, hBrush);
	Rectangle(hDC, rctBrush.left, rctBrush.top, rctBrush.right, rctBrush.bottom);
	
	ReleaseDC(hWnd, hDC);
	DeleteDC(hDC);
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int i;

	switch (uMsg)
	{
	case WM_DESTROY:
		for (i = 0; i < 8; i++)
		{
			DeleteObject(hBrushGrid[i]);
		}
		PostQuitMessage(0);
	case WM_CREATE:
		InitGridBurshAndRect();
		break;
	case WM_PAINT:
		DrawGridLine(hWnd);
		break;
	case WM_LBUTTONDOWN:
		{
			POINTS ptlHit = MAKEPOINTS((LPPOINTS)lParam);
			PaintGrid(hWnd, ptlHit);
			PaintPattern(hWnd, ptlHit);
		}
		
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcx;
	HWND hWnd;
	MSG msg;
	WORD wport = 80;
	HWND hWndCap = NULL;

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
	wcx.lpszClassName = _T("MainWClass");
	wcx.hIconSm = NULL;
	RegisterClassEx(&wcx);

	hWnd = CreateWindow(
		wcx.lpszClassName,
		_T("Brush_Pen"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MAXIMIZE | WS_POPUPWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, 
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