#include<windows.h>

void GdiOut(HDC hdc);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HDC hdc = GetDC(NULL);
	GdiOut(hdc);
	ReleaseDC(NULL, hdc);
}

void GdiOut(HDC hdc)
{
	HPEN hpen, hpenOld;
	HBRUSH hbrush, hbrushOld;
	BYTE bRed = 0, bGreen = 0, bBlue = 0;
	COLORREF cPen = RGB(bRed, bGreen, bBlue);
	COLORREF cBrush = RGB(233, GetGValue(cPen), 255);

	hpen = CreatePen(PS_SOLID, 10, cPen);
	hbrush = CreateSolidBrush(cBrush);
	hpenOld = SelectObject(hdc, hpen);
	hbrushOld = SelectObject(hdc, hbrush);

	LineTo(hdc, 500, 500);
	SelectObject(hdc, hpenOld);

	Rectangle(hdc, 200, 200, 500, 500);
	SelectObject(hdc, hbrushOld);

	DeleteObject(hpen);
	DeleteObject(hbrush);
}