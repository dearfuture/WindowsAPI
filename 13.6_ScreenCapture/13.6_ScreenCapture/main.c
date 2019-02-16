#include<windows.h>
#include<stdio.h>
#include<tchar.h>

#define PALVERSION 0x300
#define CAP_SHOW_MODE_STRTCH	1
#define CAP_SHOW_MODE_NOSTRTCH	0

HBITMAP ghBitmap = NULL;
RECT rectShow;
LPTSTR szCaptureWindowName = _T("Windows 任务管理器");

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
HBITMAP ScreenCapture(LPTSTR fileName, WORD Bitcount, LPRECT lpRect);
VOID DoPaint(HWND hWnd, DWORD dwMode);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcx;
	HWND hWnd;
	MSG msg;
	WORD wPort = 80;
	HWND hWndCap = NULL;

	if (NULL != szCaptureWindowName)
	{
		hWndCap = FindWindow(NULL, szCaptureWindowName);
		if (!GetWindowRect(hWndCap, &rectShow))
		{
			MessageBox(NULL, _T("Can not find windows to capture"), _T("error"), MB_OK);
			return 0;
		}
	}

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

	if (!RegisterClassEx(&wcx))
	{
		return FALSE;
	}

	hWnd = CreateWindow(
		wcx.lpszClassName,
		_T("CAP"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MAXIMIZE | WS_POPUPWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}

	ghBitmap = ScreenCapture(_T("x.bmp"), 32, &rectShow);

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

HBITMAP ScreenCapture(LPTSTR fileName, WORD Bitcount, LPRECT lpRect)
{
	HBITMAP hBitMap = NULL;
	HDC hScreenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	HDC hMemDC = CreateCompatibleDC(hScreenDC);
	
	int ScreenWidth = GetDeviceCaps(hScreenDC, HORZRES);
	int ScreenHeight = GetDeviceCaps(hScreenDC, VERTRES);
	HBITMAP hOldBM;
	PVOID lpvpxldata;
	int ixStart;
	int iyStart;
	int iX;
	int iY;
	DWORD dwBitmapArraySize;
	DWORD nBitsOffset;
	DWORD lImageSize;
	DWORD lFileSize;
	BITMAPINFO bmInfo;
	BITMAPFILEHEADER bmFileHeader;
	HANDLE hbmFile;
	DWORD dwWritten;

	if (NULL == lpRect)
	{
		ixStart = iyStart = 0;
		iX = ScreenWidth;
		iY = ScreenHeight;
	}
	else
	{
		ixStart = lpRect->left;
		iyStart = lpRect->top;
		iX = lpRect->right - lpRect->left;
		iY = lpRect->bottom - lpRect->top;
	}

	hBitMap = CreateCompatibleBitmap(hScreenDC, iX, iY);
	hOldBM = SelectObject(hMemDC, hBitMap);
	BitBlt(hMemDC, 0, 0, iX, iY, hScreenDC, ixStart, iyStart, SRCCOPY);
	hBitMap = SelectObject(hMemDC, hOldBM);

	if (NULL == fileName)
	{
		DeleteDC(hScreenDC);
		DeleteDC(hMemDC);
		return hBitMap;
	}

	dwBitmapArraySize = ((((iX * 32) + 31) & ~31) >> 3) * iY;
	lpvpxldata = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, dwBitmapArraySize);
	ZeroMemory(lpvpxldata, dwBitmapArraySize);

	ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = iX;
	bmInfo.bmiHeader.biHeight = iY;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = Bitcount;
	bmInfo.bmiHeader.biCompression = BI_RGB;

	ZeroMemory(&bmFileHeader, sizeof(BITMAPFILEHEADER));
	nBitsOffset = sizeof(BITMAPFILEHEADER) + bmInfo.bmiHeader.biSize;
	lImageSize = ((((bmInfo.bmiHeader.biWidth * bmInfo.bmiHeader.biBitCount) + 31) &~31) >> 3) * bmInfo.bmiHeader.biHeight;
	lFileSize = nBitsOffset + lImageSize;
	bmFileHeader.bfType = 'B' + ('M' << 8);
	bmFileHeader.bfSize = lFileSize;
	bmFileHeader.bfOffBits = nBitsOffset;

	GetDIBits(hMemDC, hBitMap, 0, bmInfo.bmiHeader.biHeight, lpvpxldata, &bmInfo, DIB_RGB_COLORS);
	hbmFile = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hbmFile, &bmFileHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(hbmFile, &bmInfo, sizeof(BITMAPINFO), &dwWritten, NULL);
	WriteFile(hbmFile, lpvpxldata, lImageSize, &dwWritten, NULL);
	CloseHandle(hbmFile);

	HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, lpvpxldata);
	ReleaseDC(0, hScreenDC);
	DeleteDC(hMemDC);

	return hBitMap;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
		DoPaint(hWnd, CAP_SHOW_MODE_STRTCH);
		break;
	case WM_DESTROY:
		DeleteObject(ghBitmap);
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void DoPaint(HWND hWnd, DWORD dwMode)
{
	PAINTSTRUCT ps;
	RECT rect;
	HDC hDcMem;
	BITMAP bm;
	DIBSECTION ds;

	HDC hDC = BeginPaint(hWnd, &ps);
	GetClientRect(hWnd, &rect);

	SetStretchBltMode(hDC, HALFTONE);
	hDcMem = CreateCompatibleDC(hDC);
	SelectObject(hDcMem, ghBitmap);
	if (ghBitmap)
	{
		if (GetObject(ghBitmap, sizeof(BITMAP), &bm))
		{
			if (dwMode == CAP_SHOW_MODE_STRTCH)
			{
				StretchBlt(hDC, 0, 0, rect.right, rect.bottom, 
					hDcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
			}
			else
			{
				int ixStart = (rect.right - rect.left - bm.bmWidth) / 2;
				int iyStart = (rect.bottom - rect.top - bm.bmHeight) / 2;
				ixStart = ixStart < 0 ? 0 : ixStart;
				iyStart = iyStart < 0 ? 0 : iyStart;
				BitBlt(hDC, 0, 0, rect.right, rect.bottom, hDcMem, -ixStart, -iyStart, SRCCOPY);
			}
			DeleteDC(hDcMem);
		}
	}
	else
	{
		PatBlt(hDC, 0, 0, rect.right, rect.bottom, BLACKNESS);
	}

	EndPaint(hWnd, &ps);
}