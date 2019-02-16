#include<windows.h>
#include<stdio.h>
#include<tchar.h>

HFONT ChooseNewFont(void)
{
	CHOOSEFONT cf;
	LOGFONT lf;
	HFONT hfont;

	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = (HWND)NULL;
	cf.hDC = (HDC)NULL;
	cf.lpLogFont = &lf;
	cf.iPointSize = 0;
	cf.Flags = CF_SCREENFONTS;
	cf.rgbColors = RGB(0, 0, 0);
	cf.lCustData = 0;
	cf.lpfnHook = NULL;
	cf.lpTemplateName = NULL;
	cf.hInstance = NULL;
	cf.lpszStyle = NULL;
	cf.nFontType = SCREEN_FONTTYPE;
	cf.nSizeMin = 0;
	cf.nSizeMax = 0;

	ChooseFont(&cf);
	hfont = CreateFontIndirect(cf.lpLogFont);

	return hfont;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HDC hdc = GetDC(NULL);
	int xIncrement = 500;
	int YStart = 50;
	TEXTMETRIC tm;
	HFONT hFntDefault, hFntChoose;
	SIZE sz;
	UINT uAlignPrev;
	LPTSTR lpszString1 = _T("String One");
	LPTSTR lpszString2 = _T("String Two");
	LPTSTR lpszString3 = _T("String Three");
	DWORD dwStrLen1 = _tcslen(lpszString1);
	DWORD dwStrLen2 = _tcslen(lpszString2);
	DWORD dwStrLen3 = _tcslen(lpszString3);

	hFntChoose = ChooseNewFont();
	SetBkColor(hdc, RGB(255, 255, 255));
	SetTextColor(hdc, RGB(255, 0, 0));
	SetBkMode(hdc, TRANSPARENT);

	TextOut(hdc, xIncrement, YStart, lpszString1, dwStrLen1);

	GetTextExtentPoint32(hdc, lpszString1, dwStrLen1, &sz);
	xIncrement += sz.cx;
	GetTextMetrics(hdc, &tm);
	xIncrement -= tm.tmOverhang;

	YStart += sz.cy;
	hFntDefault = SelectObject(hdc, hFntChoose);
	TextOut(hdc, xIncrement, YStart, lpszString2, dwStrLen2);

	GetTextExtentPoint32(hdc, lpszString2, dwStrLen2, &sz);
	YStart += sz.cy;
	GetTextMetrics(hdc, &tm);
	xIncrement -= tm.tmOverhang;
	SelectObject(hdc, hFntDefault);

	uAlignPrev = SetTextAlign(hdc, TA_UPDATECP);
	MoveToEx(hdc, xIncrement, YStart, (LPPOINT)NULL);
	TextOut(hdc, 0, 0, lpszString3, dwStrLen3);
	SetTextAlign(hdc, uAlignPrev);

	DeleteObject(hFntChoose);
	SetBkMode(hdc, OPAQUE);
	DeleteDC(hdc);

	return 0;
}