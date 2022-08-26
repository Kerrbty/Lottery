#include "./res/resource.h"
#include <Windows.h>
#include <tchar.h>
#include <algorithm>
#include "Config.h"
#include "DataBase.h"
#include "ImportData.h"

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
    HWND hResultSelectGroupWnd;  // ����б�ѡ����ı� 
    HWND hBookShowItemWnd[SHOWITEMS];  // �����ʾ�� 
    HWND hRoundCountWnd;   // ��ѡ������ 

    HWND hTopMoustWnd;  // ������ʾѡ�� 
    HWND hStatusWnd;  // ״̬��ʾ��Ϣ 
}WndFrame;

WndFrame gFrame;
ExcelData gData;

DWORD WINAPI InitDataThread(LPVOID lparam)
{
    unsigned char* pibuf = NULL;
    unsigned int len = 0;
    if (ReadDatabase(&pibuf, &len))
    {
        
        memset(&gData.bSelect, 0, sizeof(gData.bSelect));
        memset(&gData.uiShowItems, 0, sizeof(gData.uiShowItems));
        memset(&gData.bShowItems, 0, sizeof(gData.bShowItems));
        gData.nShowItemCount = 0;
        gData.dwResultCount = 0;
        if ( ImportFromXls((char*)pibuf, len, gData.book)> 0 && gData.book.size()>0) 
        {
            // �ѹ�����������ȡ���� 
            Elements* FirstBookTitle = &gData.book[0].lines[0];
            int nCount = gData.book.size();
            int nCols = FirstBookTitle->size();
            if (nCols > SHOWITEMS)
            {
                nCols = SHOWITEMS;
            }
            gData.nShowItemCount = 0;
            for (int i=0; i<nCols; i++)
            {
                int n=1;
                for (n=1; n<nCount; n++)
                {
                    Elements* pCurElement = &gData.book[n].lines[0];
                    if ( 
                        std::find( pCurElement->begin(), 
                                    pCurElement->end(),
                                    (*FirstBookTitle)[i]) == pCurElement->end()
                       )
                    {
                        break;;
                    }
                }
                if (n == nCount)
                {
                    gData.uiShowItems[gData.nShowItemCount++] = i;
                }
            }
            PostMessage(gFrame.hMainWnd, WM_UPDATE_DLG, 0, 0);
        }
        FreeData(pibuf);
    }
    else
    {
        SetWindowText(gFrame.hStatusWnd, TEXT("���ݼ��ش���,����δ�������� ���� ��������\r\n�����µ�������"));
        if (MessageBox(gFrame.hMainWnd, TEXT("�Ƿ��������µ�������?"), TEXT("���ݼ��ش���"), MB_YESNO|MB_ICONERROR) == IDYES)
        {
            PostMessage(gFrame.hMainWnd, WM_COMMAND, IDC_IMPORT_DATA, 0);
        }
    }
    return 0;
}

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
                ShowWindow(gFrame.hBookWnd[i], FALSE);
                gFrame.hBookSetTextWnd[i] = GetDlgItem(hWnd, IDC_TEXT_BOOK1+i*2);
                ShowWindow(gFrame.hBookSetTextWnd[i], FALSE);
                gFrame.hBookSetCountWnd[i] = GetDlgItem(hWnd, IDC_BOOK_COUNT1+i*2);
                ShowWindow(gFrame.hBookSetCountWnd[i], FALSE);
            }
            gFrame.hResultSelectGroupWnd = GetDlgItem(hWnd, IDC_RESULT_SELECT);
            for (unsigned int i=0; i<SHOWITEMS; i++)
            {
                gFrame.hBookShowItemWnd[i] = GetDlgItem(hWnd, IDC_SHOW_ITEM1+i);
                ShowWindow(gFrame.hBookShowItemWnd[i], FALSE);
            }
            gFrame.hRoundCountWnd = GetDlgItem(hWnd, IDC_ROUND_COUNT);
            gFrame.hTopMoustWnd = GetDlgItem(hWnd, IDC_TOP_MOST);
            gFrame.hStatusWnd = GetDlgItem(hWnd, IDC_TEXT_MSG);

            // ������ʾ 
            move_Middle(hWnd);

            SetWindowText(gFrame.hSelectGroupWnd, TEXT("ѡ���(0/0)"));

            CloseHandle(CreateThread(NULL, 0, InitDataThread, NULL, 0, NULL));
            SetWindowText(gFrame.hStatusWnd, TEXT("���ڼ�������..."));
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
            switch (nCtrlId)
            {
            case IDC_BOOK1:  // ������ѡ�ı�� 
            case IDC_BOOK2:
            case IDC_BOOK3:
            case IDC_BOOK4:
            case IDC_BOOK5:
            case IDC_BOOK6:
            case IDC_BOOK7:
            case IDC_BOOK8:
            case IDC_BOOK9:
            case IDC_BOOK10:
                {
                    int index = nCtrlId-IDC_BOOK1;
                    if ( IsDlgButtonChecked(hWnd, nCtrlId) )
                    {
                        gData.bSelect[index] = TRUE;
                        EnableWindow(gFrame.hBookSetTextWnd[index], TRUE);
                        EnableWindow(gFrame.hBookSetCountWnd[index], TRUE);
                    }
                    else
                    {
                        gData.bSelect[index] = FALSE;
                        EnableWindow(gFrame.hBookSetTextWnd[index], FALSE);
                        EnableWindow(gFrame.hBookSetCountWnd[index], FALSE);
                    }

                    unsigned int nCount = 0;
                    for (int i=0; i<10; i++)
                    {
                        if (gData.bSelect[i])
                        {
                            nCount++;
                        }
                    }
                    LPTSTR lpTmp = GetStatusText();
                    swprintf(lpTmp, TEXT("ѡ���(%u/%u)"), nCount, gData.book.size());
                    SetWindowText(gFrame.hSelectGroupWnd, lpTmp);
                }
                break;
            case IDC_SHOW_ITEM1: // �����ʾ�������б�ѡ�� 
            case IDC_SHOW_ITEM2:
            case IDC_SHOW_ITEM3:
            case IDC_SHOW_ITEM4:
            case IDC_SHOW_ITEM5:
            case IDC_SHOW_ITEM6:
            case IDC_SHOW_ITEM7:
            case IDC_SHOW_ITEM8:
            case IDC_SHOW_ITEM9:
            case IDC_SHOW_ITEM10:
            case IDC_SHOW_ITEM11:
            case IDC_SHOW_ITEM12:
            case IDC_SHOW_ITEM13:
            case IDC_SHOW_ITEM14:
            case IDC_SHOW_ITEM15:
            case IDC_SHOW_ITEM16:
            case IDC_SHOW_ITEM17:
            case IDC_SHOW_ITEM18:
                {
                    int index = nCtrlId-IDC_SHOW_ITEM1;
                    if ( IsDlgButtonChecked(hWnd, nCtrlId) )
                    {
                        gData.dwResultCount++;
                        gData.bShowItems[index] = TRUE;
                    }
                    else
                    {
                        gData.dwResultCount--;
                        gData.bShowItems[index] = FALSE;
                    }

                    LPTSTR lpTmp = GetStatusText();
                    swprintf(lpTmp, TEXT("���չʾ(%u)"), gData.dwResultCount);
                    SetWindowText(gFrame.hResultSelectGroupWnd, lpTmp);
                }
                break;
            case IDC_TOP_MOST: // ������ʾ 
                {
                    if ( IsDlgButtonChecked(hWnd, IDC_TOP_MOST) )
                    {
                        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
                        SetConfig(TEXT("TopMost"), 1);
                    }
                    else
                    {
                        SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
                        SetConfig(TEXT("TopMost"), 0);
                    }
                }
                break;
            case IDC_SELECT_REPEAT:
                {
                    if ( IsDlgButtonChecked(hWnd, IDC_SELECT_REPEAT) )
                    {
                        SetConfig(TEXT("RepeatLottery"), 1);
                        gData.bRepeatRount = TRUE;
                    }
                    else
                    {
                        SetConfig(TEXT("RepeatLottery"), 0);
                        gData.bRepeatRount = FALSE;
                    }
                }
                break;
            case IDC_EXTRACT: // ��ǩ 
                {
                    BOOL bStopLottery = FALSE;
                    LPTSTR lpTmp = GetStatusText();

                    // �ܹ�����ٴ�? 
                    gData.ulRoundCount = GetDlgItemInt(hWnd, IDC_ROUND_COUNT, NULL, FALSE);
                    if (gData.ulRoundCount == 0)
                    {
                        MessageBox(hWnd, _T("����ȡ���������ô�������������"), _T("������ʾ"), MB_OK|MB_ICONERROR);
                        bStopLottery = TRUE;
                    }
                    else
                    {
                        // ÿ������һ�γ�ȡ������? 
                        for (int i=0; i<10; i++)
                        {
                            if (gData.bSelect[i])
                            {
                                // ��ȡѡ��ĳ�ȡ���� 
                                int nIndex = SendMessage(gFrame.hBookSetCountWnd[i], CB_GETCURSEL, 0, 0);
                                if (nIndex != CB_ERR)
                                {
                                    SendMessage(gFrame.hBookSetCountWnd[i], CB_GETLBTEXT, nIndex, (LPARAM)lpTmp);
                                    gData.ulBookCount[i] = _tcstoul(lpTmp, NULL, 10); 
                                }
                                else
                                {
                                    bStopLottery = TRUE;
                                    wsprintf(lpTmp, TEXT("��� ��%s�� δѡ�񡶳�ȡ������"), gData.book[i].name.c_str());
                                    MessageBox(hWnd, lpTmp, _T("������ʾ"), MB_OK);
                                    break;
                                }

                                // �����ܳ�ȡ�����Ƿ񳬹������ʾ�Ƿ��ظ���ȡ 
                                
                                if (!gData.bRepeatRount)
                                {
                                    DWORD nLines = gData.book[i].lines.size() - 1;
                                    if (gData.ulBookCount[i]*gData.ulRoundCount > nLines)
                                    {
                                        bStopLottery = TRUE;
                                        wsprintf(lpTmp, 
                                            TEXT("��� ��%s�� ����Ϊ: �� %u ��, ÿ�� %u ��, �Ѿ������������ %u \r\n��������ظ���ȡ�������б�񶼻��ظ���ȡ�����ڵ������治���ظ�\r\n�Ƿ񡶿����ظ���ȡ��ѡ��?"), 
                                            gData.book[i].name.c_str(),
                                            gData.ulRoundCount,
                                            gData.ulBookCount[i],
                                            nLines);
                                        if (MessageBox(hWnd, lpTmp, TEXT("���ô���"), MB_YESNO|MB_ICONQUESTION) == IDYES)
                                        {
                                            bStopLottery = FALSE;
                                            gData.bRepeatRount = TRUE;
                                            SendMessage((HWND)GetDlgItem(hWnd, IDC_SELECT_REPEAT), BM_SETCHECK, 1, 0);
                                            SetConfig(TEXT("RepeatLottery"), 1);
                                        }
                                        break;
                                    }
                                }
                            }
                        }

                        // �����ʾ���� 
                        if (gData.dwResultCount == 0)
                        {
                            MessageBox(hWnd, _T("��ѡ����ʵġ����չʾ����"), _T("���չʾ��δ����"), MB_OK|MB_ICONERROR);
                            bStopLottery = TRUE;
                        }
                    }

                    if (!bStopLottery)
                    {
                        // �齱 
                    }
                }
                break;
            case IDC_IMPORT_DATA:  // ����excel��� 
                {
                    OPENFILENAME open_file_name;   
                    LPTSTR lpfile = new TCHAR[MAX_PATH*2];  
                    if (lpfile)
                    {
                        memset(lpfile, 0, MAX_PATH*2*sizeof(TCHAR)); // ������գ����������� 
                        // ��ʼ��ѡ���ļ��Ի���       
                        ZeroMemory(&open_file_name, sizeof(OPENFILENAME));  
                        open_file_name.lStructSize = sizeof(OPENFILENAME);  
                        open_file_name.hwndOwner = hWnd;  
                        open_file_name.lpstrFile = lpfile;  
                        open_file_name.nMaxFile = MAX_PATH*2;  
                        open_file_name.lpstrFilter = TEXT("Excel �������(*.*)\0*.xls\0Excel 2007(*.xlsx)\0*.xlsx\0\0");  
                        open_file_name.nFilterIndex = 1;  
                        open_file_name.lpstrFileTitle = 0;  
                        open_file_name.nMaxFileTitle = 0;  
                        open_file_name.lpstrInitialDir = 0;  
                        open_file_name.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;  
                        //ofn.lpTemplateName =  MAKEINTRESOURCE(ID_TEMP_DIALOG);  

                        // ��ʾ��ѡ���ļ��Ի���  
                        if (GetOpenFileName(&open_file_name))  
                        {  
                            unsigned char* pibuf = NULL;
                            unsigned int len = 0;
                            if ( ReadFromFile(lpfile, &pibuf, &len) )
                            {
                                Book tmpbook;
                                if (ImportFromXls((char*)pibuf, len, tmpbook)> 0)
                                {
                                    // �������������浽���ݿ��� 
                                    SaveFile2Database(lpfile);
                                    // �Ѿ��ɹ��������ݣ��Ƿ����������? 
                                    if (MessageBox(hWnd, TEXT("�ѳɹ��������ݣ��Ƿ����������?"), TEXT("��ʾ"), MB_YESNO|MB_ICONASTERISK) == IDYES)
                                    {
                                        CloseHandle(CreateThread(NULL, 0, InitDataThread, NULL, 0, NULL));
                                        SetWindowText(gFrame.hStatusWnd, TEXT("���ڼ�������..."));
                                    }
                                }
                                FreeData(pibuf);
                            }
                        } 
                        delete []lpfile;
                    }
                }
                break;
            }
        }
        break;
    case WM_UPDATE_DLG:
        {
            if (GetConfig(TEXT("TopMost")) == 1)
            {
                SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
                SendMessage((HWND)GetDlgItem(hWnd, IDC_TOP_MOST), BM_SETCHECK, 1, 0);
            }
            if (GetConfig(TEXT("RepeatLottery")) == 1)
            {
                gData.bRepeatRount = TRUE;
                SendMessage((HWND)GetDlgItem(hWnd, IDC_SELECT_REPEAT), BM_SETCHECK, 1, 0);
            }

            LPTSTR lpTmp = GetStatusText();
            if (lpTmp)
            {
                int nCount = gData.book.size();
                // ���������10�� 
                if (nCount>10)
                {
                    nCount = 10;
                }
                // �������� 
                for (int i=0; i<nCount; i++)
                {
                    ShowWindow(gFrame.hBookWnd[i], SW_SHOW);
                    ShowWindow(gFrame.hBookSetTextWnd[i], SW_SHOW);
                    ShowWindow(gFrame.hBookSetCountWnd[i], SW_SHOW);

                    gData.bSelect[i] = TRUE;

                    SetWindowText(gFrame.hBookWnd[i], gData.book[i].name.c_str());
                    SendMessage(gFrame.hBookWnd[i], BM_SETCHECK, BST_CHECKED, 0); // Button_SetCheck(gFrame.hBookWnd[i], BST_CHECKED); 
                    SetWindowText(gFrame.hBookSetTextWnd[i], gData.book[i].name.c_str());

                    // ��� 
                    while(SendMessage(gFrame.hBookSetCountWnd[i], LB_GETCOUNT, 0, 0))
                    {
                        SendMessage(gFrame.hBookSetCountWnd[i], CB_DELETESTRING, 0, 0); ;
                    }
                    // ���·������� 
                    int nLines = gData.book[i].lines.size() - 1;
                    for (int index=nLines; index>0; index--)
                    {
                        swprintf(lpTmp, TEXT("%u"), index);
                        SendMessage(gFrame.hBookSetCountWnd[i], CB_INSERTSTRING, 0, (LPARAM)lpTmp);
                    }
                    // Ĭ��ѡ���һ�� 
                    SendMessage(gFrame.hBookSetCountWnd[i], CB_SETCURSEL, 0, 0);
                }

                // �����ʾ���� 
                Elements* FirstBookTitle = &gData.book[0].lines[0];
                for (int i=0; i<SHOWITEMS; i++)
                {
                    if (i >= gData.nShowItemCount)
                    {
                        ShowWindow(gFrame.hBookShowItemWnd[i], SW_HIDE);
                    }
                    else
                    {
                        ShowWindow(gFrame.hBookShowItemWnd[i], SW_SHOW);
                        SetWindowText(gFrame.hBookShowItemWnd[i], (*FirstBookTitle)[gData.uiShowItems[i]].c_str());
                    }
                    
                }

                // �������� 
                for (int i=nCount; i<10; i++)
                {
                    gData.bSelect[i] = FALSE;
                    ShowWindow(gFrame.hBookWnd[i], SW_HIDE);
                    ShowWindow(gFrame.hBookSetTextWnd[i], SW_HIDE);
                    ShowWindow(gFrame.hBookSetCountWnd[i], SW_HIDE);
                }
                swprintf(lpTmp, TEXT("ѡ���(%u/%u)"), nCount, nCount);
                SetWindowText(gFrame.hSelectGroupWnd, lpTmp);
            }
            SetWindowText(gFrame.hStatusWnd, TEXT("���ݼ������!"));
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
