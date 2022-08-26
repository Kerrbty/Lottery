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

    HWND hSelectGroupWnd;  // ѡ����ҳ�ı� 
    HWND hBookWnd[10];     // �����ѡ��� 
    HWND hBookSetTextWnd[10];   // �����������ǰ����ı� 
    HWND hBookSetCountWnd[10];  // ���������ѡ��� 
    HWND hBookShowItemWnd[10];  // �����ʾ�� 
    HWND hRoundCountWnd;   // ��ѡ������ 

    HWND hTopMoustWnd;  // ������ʾѡ�� 
    HWND hStatusWnd;  // ״̬��ʾ��Ϣ 
}WndFrame;

WndFrame gFrame;

void move_Middle(HWND hWnd)
{
    RECT rect;
    int x,y;
    GetWindowRect(hWnd, &rect);//�õ���ǰ���ڴ�С��Ϣ
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

            // ����ͼ�� 
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

            // ������ʾ 
            move_Middle(hWnd);

            SetWindowText(gFrame.hSelectGroupWnd, TEXT("ѡ���(0/10)"));
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
    //��ʾ����
    DialogBoxParam(hInstance, MAKEINTRESOURCE(CONDIG_DIALOG), NULL, (DLGPROC)ProcWinMain, NULL);
#if USE_SKIN
    SkinH_Detach();
#endif
    return TRUE;
}
