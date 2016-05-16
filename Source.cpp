#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib,"netapi32")

#include <windows.h>
#include <lm.h>
#include <stdio.h> 

TCHAR szClassName[] = TEXT("Window");

void GetMacAddress(HWND hList)
{
	unsigned short MACData[8];
	DWORD dwEntriesRead;
	DWORD dwTotalEntries;
	BYTE *pbBuffer;
	if (NetWkstaTransportEnum(
		NULL,
		0,
		&pbBuffer,
		MAX_PREFERRED_LENGTH,
		&dwEntriesRead,
		&dwTotalEntries,
		NULL) == NERR_Success)
	{
		WKSTA_TRANSPORT_INFO_0 *pwkti = (WKSTA_TRANSPORT_INFO_0 *)pbBuffer;
		for (DWORD i = 1; i < dwEntriesRead; i++)
		{
			swscanf_s((wchar_t *)pwkti[i].wkti0_transport_address, L"%2hx%2hx%2hx%2hx%2hx%2hx",
				&MACData[0], &MACData[1], &MACData[2], &MACData[3], &MACData[4], &MACData[5]);
			TCHAR szMacAddress[64];
			wsprintf(szMacAddress, TEXT("%02X-%02X-%02X-%02X-%02X-%02X"),
				MACData[0],
				MACData[1],
				MACData[2],
				MACData[3],
				MACData[4],
				MACData[5]);
			SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)szMacAddress);
		}
		NetApiBufferFree(pbBuffer);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton;
	static HWND hList;
	switch (msg)
	{
	case WM_CREATE:
		hList = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), 0, WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOINTEGRALHEIGHT, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("Macアドレスを取得"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		PostMessage(hWnd, WM_COMMAND, IDOK, 0);
		break;
	case WM_SIZE:
		MoveWindow(hList, 10, 10, 256, 128, TRUE);
		MoveWindow(hButton, 10, 148, 256, 32, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			SendMessage(hList, LB_RESETCONTENT, 0, 0);
			GetMacAddress(hList);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("NetWkstaTransportEnum 関数を使用してMacアドレスを取得する"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
