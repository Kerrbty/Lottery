#include "./res/resource.h"
#include <Windows.h>
#include <tchar.h>
#define USE_SKIN   0
#if USE_SKIN
#include "SkinH/SkinH.h"
#pragma comment(lib, "SkinH/SkinH.lib")
#endif
#pragma comment(lib, "comctl32.lib")

typedef struct {
    HINSTANCE hInstance; 
    HWND hMainWnd;

    HWND hSelectGroupWnd;  // 选择表格页文本 
    HWND hBookWnd[10];     // 各表格选择框 
    HWND hBookSetTextWnd[10];   // 各表格数量框前面的文本 
    HWND hBookSetCountWnd[10];  // 各表格数量选择框 
    HWND hBookShowItemWnd[10];  // 结果显示列 
    HWND hRoundCountWnd;   // 总选择组数 

    HWND hTopMoustWnd;  // 顶端显示选项 
    HWND hStatusWnd;  // 状态提示信息 
}WndFrame;

WndFrame gFrame;

void move_Middle(HWND hWnd)
{
    RECT rect;
    int x,y;
    GetWindowRect(hWnd, &rect);//得到当前窗口大小信息
    x = rect.right - rect.left;
    y = rect.bottom - rect.top;
    MoveWindow(hWnd, (GetSystemMetrics(SM_CXSCREEN)- x)>>1, (GetSystemMetrics(SM_CYSCREEN)-y)>>1, x, y , TRUE);
}

INT_PTR CALLBACK ProcWinMain(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
    case WM_INITDIALOG:
        {
            gFrame.hMainWnd = hWnd;

            // 设置图标 
            HICON hIcon = LoadIcon( gFrame.hInstance, (LPCTSTR)IDI_MAINICON );
            SetClassLong( hWnd, GCL_HICON, (LONG)hIcon );

            gFrame.hSelectGroupWnd = GetDlgItem(hWnd, IDC_SELECT_BOOK);
            for (unsigned int i=0; i<10; i++)
            {
                gFrame.hBookWnd[i] = GetDlgItem(hWnd, IDC_BOOK1+i);
                gFrame.hBookSetTextWnd[i] = GetDlgItem(hWnd, IDC_TEXT_BOOK1+i*2);
                gFrame.hBookSetCountWnd[i] = GetDlgItem(hWnd, IDC_BOOK_COUNT1+i*2);
                gFrame.hBookShowItemWnd[i] = GetDlgItem(hWnd, IDC_SHOW_ITEM1+i);
            }
            gFrame.hRoundCountWnd = GetDlgItem(hWnd, IDC_ROUND_COUNT);
            gFrame.hTopMoustWnd = GetDlgItem(hWnd, IDC_TOP_MOST);
            gFrame.hStatusWnd = GetDlgItem(hWnd, IDC_TEXT_MSG);

            // 居中显示 
            move_Middle(hWnd);

            SetWindowText(gFrame.hSelectGroupWnd, TEXT("选择表(0/10)"));
            SetWindowText(gFrame.hStatusWnd, TEXT(""));
        }
        break;

    case WM_SHOWWINDOW:
        move_Middle(hWnd);
        break;

    case WM_CLOSE:
        EndDialog(hWnd, 0);
        break;

    case WM_COMMAND:
        {
            WORD nCode = HIWORD(wParam);
            WORD nCtrlId = LOWORD(wParam);
            HWND hSubWnd = (HWND)lParam;
        }
        break;
    default:
        return FALSE;
        break;
    }
    return 0;
} 

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#if USE_SKIN
    SkinH_AttachEx(TEXT("Aero.she"), NULL);
    SkinH_Attach();
#endif

    gFrame.hInstance = hInstance;
    //显示窗口
    DialogBoxParam(hInstance, MAKEINTRESOURCE(CONDIG_DIALOG), NULL, (DLGPROC)ProcWinMain, NULL);
#if USE_SKIN
    SkinH_Detach();
#endif
    return TRUE;
}
