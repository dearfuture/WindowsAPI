#include<windows.h>
#include<stdio.h>
#include<tchar.h>

BOOL CALLBACK EnumFamCallBack(LPLOGFONT, LPNEWTEXTMETRIC, DWORD, LPVOID);
DWORD ListFont(HWND);

int main(void)
{
	ListFont(NULL);
}

DWORD ListFont(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	int aFontCount[] = {0, 0, 0};

	EnumFontFamilies(hdc, NULL, (FONTENUMPROC)EnumFamCallBack, (LPARAM)aFontCount);

	printf("Number of raster fonts: %d\n", aFontCount[0]);
	printf("Number of vector fonts: %d\n", aFontCount[1]);
	printf("Number of TrueType fonts: %d\n", aFontCount[2]);

	return 0;
}

BOOL CALLBACK EnumFamCallBack(LPLOGFONT lplf, LPNEWTEXTMETRIC lpntm, DWORD FontType, LPVOID aFontCount)
{
	PINT aiFontCount = (PINT)aFontCount;
	
	if (FontType & RASTER_FONTTYPE)
	{
		printf(" RASTER TYPE \t");
		aiFontCount[0]++;
	}
	else if (FontType & TRUETYPE_FONTTYPE)
	{
		printf(" TRUETYPE \t");
		aiFontCount[2]++;
	}
	else
	{
		printf(" VECTOR TYPE \t");
		aiFontCount[1]++;
	}

	_tprintf(_T("%s\tItalic = %d\n"), lplf->lfFaceName, lplf->lfItalic);
	if (aiFontCount[0] || aiFontCount[1] || aiFontCount[2])
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}