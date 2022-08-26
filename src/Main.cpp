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

    HWND hSelectGroupWnd;  // 选择表格页文本 
    HWND hBookWnd[10];     // 各表格选择框 
    HWND hBookSetTextWnd[10];   // 各表格数量框前面的文本 
    HWND hBookSetCountWnd[10];  // 各表格数量选择框 
    HWND hResultSelectGroupWnd;  // 结果列表选择宽文本 
    HWND hBookShowItemWnd[SHOWITEMS];  // 结果显示列 
    HWND hRoundCountWnd;   // 总选择组数 

    HWND hTopMoustWnd;  // 顶端显示选项 
    HWND hStatusWnd;  // 状态提示信息 
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
            // 把公共的列名提取出来 
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
        SetWindowText(gFrame.hStatusWnd, TEXT("数据加载错误,可能未设置数据 或者 数据已损坏\r\n请重新导入数据"));
        if (MessageBox(gFrame.hMainWnd, TEXT("是否现在重新导入数据?"), TEXT("数据加载错误"), MB_YESNO|MB_ICONERROR) == IDYES)
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

            // 居中显示 
            move_Middle(hWnd);

            SetWindowText(gFrame.hSelectGroupWnd, TEXT("选择表(0/0)"));

            CloseHandle(CreateThread(NULL, 0, InitDataThread, NULL, 0, NULL));
            SetWindowText(gFrame.hStatusWnd, TEXT("正在加载数据..."));
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
            case IDC_BOOK1:  // 参与赛选的表格 
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
                    swprintf(lpTmp, TEXT("选择表(%u/%u)"), nCount, gData.book.size());
                    SetWindowText(gFrame.hSelectGroupWnd, lpTmp);
                }
                break;
            case IDC_SHOW_ITEM1: // 最后显示的数据列表选择 
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
                    swprintf(lpTmp, TEXT("结果展示(%u)"), gData.dwResultCount);
                    SetWindowText(gFrame.hResultSelectGroupWnd, lpTmp);
                }
                break;
            case IDC_TOP_MOST: // 顶端显示 
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
            case IDC_EXTRACT: // 抽签 
                {
                    BOOL bStopLottery = FALSE;
                    LPTSTR lpTmp = GetStatusText();

                    // 总共抽多少次? 
                    gData.ulRoundCount = GetDlgItemInt(hWnd, IDC_ROUND_COUNT, NULL, FALSE);
                    if (gData.ulRoundCount == 0)
                    {
                        MessageBox(hWnd, _T("《抽取次数》设置错误，请重新设置"), _T("错误提示"), MB_OK|MB_ICONERROR);
                        bStopLottery = TRUE;
                    }
                    else
                    {
                        // 每个表中一次抽取多少人? 
                        for (int i=0; i<10; i++)
                        {
                            if (gData.bSelect[i])
                            {
                                // 读取选择的抽取人数 
                                int nIndex = SendMessage(gFrame.hBookSetCountWnd[i], CB_GETCURSEL, 0, 0);
                                if (nIndex != CB_ERR)
                                {
                                    SendMessage(gFrame.hBookSetCountWnd[i], CB_GETLBTEXT, nIndex, (LPARAM)lpTmp);
                                    gData.ulBookCount[i] = _tcstoul(lpTmp, NULL, 10); 
                                }
                                else
                                {
                                    bStopLottery = TRUE;
                                    wsprintf(lpTmp, TEXT("表格 《%s》 未选择《抽取人数》"), gData.book[i].name.c_str());
                                    MessageBox(hWnd, lpTmp, _T("错误提示"), MB_OK);
                                    break;
                                }

                                // 计算总抽取人数是否超过表格，提示是否重复抽取 
                                
                                if (!gData.bRepeatRount)
                                {
                                    DWORD nLines = gData.book[i].lines.size() - 1;
                                    if (gData.ulBookCount[i]*gData.ulRoundCount > nLines)
                                    {
                                        bStopLottery = TRUE;
                                        wsprintf(lpTmp, 
                                            TEXT("表格 《%s》 设置为: 抽 %u 次, 每次 %u 个, 已经超过表格数量 %u \r\n如果开启重复抽取，则所有表格都会重复抽取，但在单次里面不会重复\r\n是否《开启重复抽取》选项?"), 
                                            gData.book[i].name.c_str(),
                                            gData.ulRoundCount,
                                            gData.ulBookCount[i],
                                            nLines);
                                        if (MessageBox(hWnd, lpTmp, TEXT("设置错误"), MB_YESNO|MB_ICONQUESTION) == IDYES)
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

                        // 结果显示数据 
                        if (gData.dwResultCount == 0)
                        {
                            MessageBox(hWnd, _T("请选择合适的《结果展示》项"), _T("结果展示项未设置"), MB_OK|MB_ICONERROR);
                            bStopLottery = TRUE;
                        }
                    }

                    if (!bStopLottery)
                    {
                        // 抽奖 
                    }
                }
                break;
            case IDC_IMPORT_DATA:  // 导入excel表格 
                {
                    OPENFILENAME open_file_name;   
                    LPTSTR lpfile = new TCHAR[MAX_PATH*2];  
                    if (lpfile)
                    {
                        memset(lpfile, 0, MAX_PATH*2*sizeof(TCHAR)); // 必须清空，否则会出问题 
                        // 初始化选择文件对话框。       
                        ZeroMemory(&open_file_name, sizeof(OPENFILENAME));  
                        open_file_name.lStructSize = sizeof(OPENFILENAME);  
                        open_file_name.hwndOwner = hWnd;  
                        open_file_name.lpstrFile = lpfile;  
                        open_file_name.nMaxFile = MAX_PATH*2;  
                        open_file_name.lpstrFilter = TEXT("Excel 表格数据(*.*)\0*.xls\0Excel 2007(*.xlsx)\0*.xlsx\0\0");  
                        open_file_name.nFilterIndex = 1;  
                        open_file_name.lpstrFileTitle = 0;  
                        open_file_name.nMaxFileTitle = 0;  
                        open_file_name.lpstrInitialDir = 0;  
                        open_file_name.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;  
                        //ofn.lpTemplateName =  MAKEINTRESOURCE(ID_TEMP_DIALOG);  

                        // 显示打开选择文件对话框。  
                        if (GetOpenFileName(&open_file_name))  
                        {  
                            unsigned char* pibuf = NULL;
                            unsigned int len = 0;
                            if ( ReadFromFile(lpfile, &pibuf, &len) )
                            {
                                Book tmpbook;
                                if (ImportFromXls((char*)pibuf, len, tmpbook)> 0)
                                {
                                    // 加载正常，保存到数据库中 
                                    SaveFile2Database(lpfile);
                                    // 已经成功保存数据，是否加载新数据? 
                                    if (MessageBox(hWnd, TEXT("已成功保存数据，是否加载新数据?"), TEXT("提示"), MB_YESNO|MB_ICONASTERISK) == IDYES)
                                    {
                                        CloseHandle(CreateThread(NULL, 0, InitDataThread, NULL, 0, NULL));
                                        SetWindowText(gFrame.hStatusWnd, TEXT("正在加载数据..."));
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
                // 最大数据量10个 
                if (nCount>10)
                {
                    nCount = 10;
                }
                // 设置数据 
                for (int i=0; i<nCount; i++)
                {
                    ShowWindow(gFrame.hBookWnd[i], SW_SHOW);
                    ShowWindow(gFrame.hBookSetTextWnd[i], SW_SHOW);
                    ShowWindow(gFrame.hBookSetCountWnd[i], SW_SHOW);

                    gData.bSelect[i] = TRUE;

                    SetWindowText(gFrame.hBookWnd[i], gData.book[i].name.c_str());
                    SendMessage(gFrame.hBookWnd[i], BM_SETCHECK, BST_CHECKED, 0); // Button_SetCheck(gFrame.hBookWnd[i], BST_CHECKED); 
                    SetWindowText(gFrame.hBookSetTextWnd[i], gData.book[i].name.c_str());

                    // 清空 
                    while(SendMessage(gFrame.hBookSetCountWnd[i], LB_GETCOUNT, 0, 0))
                    {
                        SendMessage(gFrame.hBookSetCountWnd[i], CB_DELETESTRING, 0, 0); ;
                    }
                    // 重新放入数据 
                    int nLines = gData.book[i].lines.size() - 1;
                    for (int index=nLines; index>0; index--)
                    {
                        swprintf(lpTmp, TEXT("%u"), index);
                        SendMessage(gFrame.hBookSetCountWnd[i], CB_INSERTSTRING, 0, (LPARAM)lpTmp);
                    }
                    // 默认选择第一个 
                    SendMessage(gFrame.hBookSetCountWnd[i], CB_SETCURSEL, 0, 0);
                }

                // 结果显示表项 
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

                // 隐藏其他 
                for (int i=nCount; i<10; i++)
                {
                    gData.bSelect[i] = FALSE;
                    ShowWindow(gFrame.hBookWnd[i], SW_HIDE);
                    ShowWindow(gFrame.hBookSetTextWnd[i], SW_HIDE);
                    ShowWindow(gFrame.hBookSetCountWnd[i], SW_HIDE);
                }
                swprintf(lpTmp, TEXT("选择表(%u/%u)"), nCount, nCount);
                SetWindowText(gFrame.hSelectGroupWnd, lpTmp);
            }
            SetWindowText(gFrame.hStatusWnd, TEXT("数据加载完成!"));
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
