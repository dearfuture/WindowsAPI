#include<windows.h>
#include<stdio.h>
#include<tchar.h>
#include<CommCtrl.h>
#include "resource.h"

HINSTANCE hInst;
HWND hWndMain;
HWND hWndTreeView;
HWND hWndListView;
HWND hWndEdit;

void OnWindowResize(void)
{

}
VOID APIENTRY DisplayContextMenu(HWND hwnd, POINT pt)
{
	HMENU hmenu;
	HMENU hmenuTrackPopup;

	if ((hmenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU_POPUP))) == NULL)
	{
		return ;
	}

	hmenuTrackPopup = GetSubMenu(hmenu, 0);
	TrackPopupMenuEx(hmenuTrackPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, hwnd, NULL);

	DestroyMenu(hmenu);
}
BOOL OnRclickTree(LPNMHDR lpParam)
{
	POINT point;
	TVHITTESTINFO thti;
	HTREEITEM htItem;

	GetCursorPos(&point);

	ScreenToClient(hWndTreeView, &point);
	thti.pt = point;
	thti.flags = TVHT_TORIGHT;

	htItem = TreeView_HitTest(hWndTreeView, &thti);
	if (NULL != htItem)
	{
		ClientToScreen(hWndTreeView, &point);
		TreeView_SelectItem(hWndTreeView, htItem);
		DisplayContextMenu(hWndMain, point);

		return TRUE;
	}

	return FALSE;
}
BOOL OnChildWindowsNotify(PVOID lpParam)
{
	LPNMHDR phdr = (LPNMHDR)lpParam;
	if (hWndTreeView != phdr->hwndFrom)
	{
		return FALSE;
	}
	switch (((LPNMHDR)lpParam)->code)
	{
	case NM_RCLICK:
		OnRclickTree((LPNMHDR)lpParam);
		break;
	default:
		break;
	}

	return TRUE;
}

INT_PTR About(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hwnd, LOWORD(wParam));
			return TRUE;
		}
		if (LOWORD(wParam) == IDCANCEL)
		{
			ExitProcess(0);
		}
		break;
	}

	return FALSE;
}

LRESULT CALLBACK MainWndProc(
	_In_ HWND   hwnd,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	switch (uMsg)
	{
	case WM_CREATE:
		break;
	case WM_PAINT:
		break;
	case WM_SIZE:
		OnWindowResize();
		break;
	case WM_NOTIFY:
		OnChildWindowsNotify((PVOID)lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case ID_HELP_ABOUT:
				{
					DialogBox(hInst, (LPCTSTR)IDD_DIALOG_ABOUT, hwnd, (DLGPROC)About);
					return 0;
				}
			default:
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
		}
	default:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

BOOL InitApplication(HINSTANCE hInstance)
{
	WNDCLASSEX wcx;
	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = MainWndProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = hInstance;
	wcx.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON_MAIN));
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcx.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_MAIN);
	wcx.lpszClassName = _T("MainWClass");
	wcx.hIconSm = (HICON)LoadImage(hInstance, 
		MAKEINTRESOURCE(IDI_ICON_MAIN),
		IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON),
		LR_DEFAULTCOLOR);

	return RegisterClassEx(&wcx);
}

BOOL InitTreeViewImageLists(HWND hwndTV)
{
	return TRUE;
}
HTREEITEM AddItemToTree(HWND hwndTV, LPTSTR lpszItem, HTREEITEM hParent, BOOL bFolder)
{
	TVITEM tvi;
	TVINSERTSTRUCT tvins;
	HTREEITEM hme;

	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvi.pszText = lpszItem;
	tvi.cchTextMax = _tcslen(tvi.pszText);

	if (bFolder)
	{
		tvi.iImage = -1;
		tvi.iSelectedImage = 2;
		tvi.cChildren = I_CHILDRENCALLBACK;
	}
	else
	{
		tvi.iImage = 3;
		tvi.iSelectedImage = 3;
		tvi.cChildren = 0;
	}

	tvins.item = tvi;
	tvins.hInsertAfter = TVI_SORT;
	if (NULL == hParent)
	{
		tvins.hParent = TVI_ROOT;
	}
	else
	{
		tvins.hParent = hParent;
	}

	hme = TreeView_InsertItem(hwndTV, &tvins);

	return hme;
}
BOOL InitTreeViewItems(HWND hwndTV)
{
	HTREEITEM hRoot = AddItemToTree(hwndTV, _T("type1"), NULL, TRUE);
	HTREEITEM hP = AddItemToTree(hwndTV, _T("name1"), hRoot, FALSE);
	hP = AddItemToTree(hwndTV, _T("name2"), hRoot, TRUE);

	AddItemToTree(hwndTV, _T("sub1"), hP, FALSE);
	AddItemToTree(hwndTV, _T("sub2"), hP, FALSE);

	hRoot = AddItemToTree(hwndTV, _T("type2"), NULL, TRUE);
	hP = AddItemToTree(hwndTV, _T("name3"), hRoot, TRUE);
	hP = AddItemToTree(hwndTV, _T("sub3"), hP, FALSE);

	hRoot = AddItemToTree(hwndTV, _T("type3"), NULL, TRUE);

	return TRUE;
}

HWND CreateTreeView(HWND hWnd, LPTSTR lpszName)
{
	HWND hwndTV;
	hwndTV = CreateWindowEx(0,
		WC_TREEVIEW,
		lpszName,
		WS_VISIBLE | WS_CHILD | WS_SIZEBOX | WS_TILED | TVS_HASBUTTONS | TVS_LINESATROOT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		hWnd,
		(HMENU)NULL,
		hInst,
		NULL);
	if (!InitTreeViewImageLists(hwndTV) || !InitTreeViewItems(hwndTV))
	{
		DestroyWindow(hwndTV);
		return NULL;
	}

	return hwndTV;
}

BOOL InitListViewImageLists(HWND hWndListView)
{
	return TRUE;
}
BOOL InitListViewColumns(HWND hWndListView)
{
	TCHAR szText[256];
	LVCOLUMN lvc;
	DWORD i;

	LPTSTR ColNames[] = {_T("name"), _T("type"), _T("size"), _T("time")};
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.pszText = szText;
	lvc.cx = 100;
	lvc.iImage = 1;
	lvc.fmt = LVCFMT_LEFT;
	
	for (i = 0; i < 3; i++)
	{
		lvc.pszText = ColNames[i];
		lvc.iSubItem = i;
		if (ListView_InsertColumn(hWndListView, i, &lvc) == -1)
		{
			return FALSE;
		}
	}

	lvc.cx = 200;
	lvc.fmt = LVCFMT_RIGHT;
	lvc.iSubItem = 3;
	lvc.pszText = ColNames[3];
	if (ListView_InsertColumn(hWndListView, 3, &lvc) == -1)
	{
		return FALSE;
	}

	return TRUE;

	return TRUE;
}
BOOL AddListViewItems(HWND hWndListView)
{
	LVITEM lvI;
	DWORD index;

	ZeroMemory(&lvI, sizeof(lvI));
	lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvI.state = 0;
	lvI.stateMask = 0;

	for (index = 0; index < 3; index++)
	{
		lvI.iItem = index;
		lvI.iImage = 0;
		lvI.iSubItem = 0;
		lvI.pszText = _T("name1");
		lvI.cchTextMax = _tcslen(lvI.pszText) + 1;

		if (ListView_InsertItem(hWndListView, &lvI) == -1)
		{
			return FALSE;
		}
		ListView_SetItemText(hWndListView, index, 2, _T("size a"));
	}

	return TRUE;
}

HWND CreateListView(HWND hWnd, LPTSTR lpszName)
{
	HWND hWndListView;
	hWndListView = CreateWindow(
		WC_LISTVIEW,
		lpszName,
		WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		hWnd,
		(HMENU)NULL,
		hInst,
		0);
	if (NULL == hWndListView)
	{
		return NULL;
	}

	if (InitListViewImageLists(hWndListView)
		&& InitListViewColumns(hWndListView)
		&& AddListViewItems(hWndListView))
	{
		return hWndListView;
	}
	else
	{
		DestroyWindow(hWndListView);
		return NULL;
	}
}
HWND CreateEdit(HWND hWnd, LPTSTR lpszName)
{
	HWND hwnd;

	hwnd = CreateWindow(
		WC_EDIT,
		NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		hWnd,
		(HMENU)NULL,
		hInst,
		(LPVOID)NULL);
	if (!hWnd)
	{
		return NULL;
	}
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	return hwnd;
}
VOID SetWindows(LPRECT lpRect)
{
	SetWindowPos(hWndTreeView, HWND_TOP, lpRect->left, lpRect->top, (int)(lpRect->right * 0.3), lpRect->bottom, SWP_SHOWWINDOW);
	SetWindowPos(hWndListView, HWND_TOP, (int)(lpRect->right * 0.3), (int)(lpRect->bottom * 0.7), (int)(lpRect->right * 0.7), (int)(lpRect->bottom * 0.3), SWP_SHOWWINDOW);
	SetWindowPos(hWndEdit, HWND_TOP, (int)(lpRect->right * 0.3), lpRect->top, (int)(lpRect->right * 0.7), (int)(lpRect->bottom * 0.7), SWP_SHOWWINDOW);
}

LPTSTR lpszLatin = _T("Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
void ShowTextOnEdit(LPTSTR lpszText)
{
	SendMessage(hWndEdit, WM_SETTEXT, 0, (LPARAM)lpszText);
}
void WINAPI InitCommonControls(void)
{

}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hwnd;
	RECT rect;
	
	hInst = hInstance;
	hwnd = CreateWindow(_T("mainWClass"),
		_T("Let's Smile"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUPWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600,
		NULL, (HMENU)LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU_MAIN)),
		hInstance, (LPVOID)NULL);
	hWndMain = hwnd;

	InitCommonControls();
	hWndTreeView = CreateTreeView(hWndMain, _T("files"));
	hWndListView = CreateListView(hWndMain, _T("processing"));
	hWndEdit = CreateEdit(hWndMain, _T("texta"));

	GetClientRect(hWndMain, &rect);
	SetWindows(&rect);
	ShowTextOnEdit(lpszLatin);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	BOOL fGotMessage;

	InitApplication(hInstance);
	InitInstance(hInstance, nCmdShow);

	while ((fGotMessage = GetMessage(&msg, (HWND)NULL, 0, 0)) != 0 && -1 != fGotMessage)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
	UNREFERENCED_PARAMETER(lpCmdLine);
}