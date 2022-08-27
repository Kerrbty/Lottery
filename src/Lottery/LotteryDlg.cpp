#include "LotteryDlg.h"
#include "SettingDlg.h"
#include "commctrl.h"
#include "Config.h"
#include "res/resource.h"
#include "SkinH/SkinH.h"
#include <time.h>
#include <vector>
#include <algorithm>
void move_Middle(HWND hWnd);

typedef struct _LotteryFrame{
    HWND hWnd;
    HWND hGroupText;  // 第几组 
    HWND hSysList;
    HWND hTopMost;

    DWORD* dwData[MAX_BOOKS];
    _tstring dwShow[SHOWITEMS];
    DWORD nShowCurrent;   // 当前显示的次数 
}LotteryFrame, *PLotteryFrame;
LotteryFrame gLottery;

static void SelectNumber(DWORD nStart, DWORD nTotal, DWORD nCount, DWORD* pDwSave)
{
    DWORD* data = new DWORD[nTotal];
    for (DWORD i=0; i<nTotal; i++)
    {
        data[i] = i+nStart;
    }

    DWORD nIndex = 0;
    while(nIndex < nCount)
    {
        // 选择一个数据 
        DWORD ipos = rand()%nTotal;
        pDwSave[nIndex] = data[ipos];
        nIndex++;

        // 剩余数据前移 
        memcpy(&data[ipos], &data[ipos+1], (nTotal-ipos-1)*sizeof(DWORD));
        nTotal--;
    }
    delete []data;
}

static DWORD WINAPI LotteryAllData(LPVOID lparam)
{
    if (gData.bRepeatRount)
    {
        // 可重复 
        for (int index=0; index<MAX_BOOKS; index++)
        {
            if (gLottery.dwData[index])
            {
                DWORD* pData = gLottery.dwData[index];
                for (DWORD iRound=0; iRound<gData.ulRoundCount; iRound++)
                {
                    // 每次抽取独立 
                    SelectNumber(1, 
                        gData.book[index].lines.size()-1, 
                        gData.ulBookCount[index], 
                        pData+iRound*gData.ulBookCount[index]
                    );
                }
            }
        }
    }
    else
    {
        for (int index=0; index<MAX_BOOKS; index++)
        {
            if (gLottery.dwData[index])
            {
                DWORD* pData = gLottery.dwData[index];
                // 每次抽取不独立 
                SelectNumber(1, 
                    gData.book[index].lines.size()-1, 
                    gData.ulBookCount[index]*gData.ulRoundCount, 
                    pData
                );
            }
        }
    }

    gLottery.nShowCurrent = 0;
    SendMessage(gLottery.hWnd, WM_UPDATE_DLG, 0, 0);
    return 0;
}

// 列表数字、英文是 10, 中文是 30 
static INT_PTR CALLBACK ResultWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
    case WM_INITDIALOG:
        {
            gLottery.hWnd = hWnd;
            gLottery.hGroupText = GetDlgItem(hWnd, IDC_GROUP_TEXT);
            gLottery.hSysList = GetDlgItem(hWnd, IDC_RESULT_LIST);
            gLottery.hTopMost = GetDlgItem(hWnd, IDC_TOPMOST);

            // 鼠标可以选择表格内容 
            SendDlgItemMessage(hWnd, IDC_RESULT_LIST, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
            
            if (GetConfig(TEXT("TopMost")) == 1)
            {
                SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
                SendMessage((HWND)GetDlgItem(hWnd, IDC_TOPMOST), BM_SETCHECK, 1, 0);
            }

            SetWindowText(gLottery.hGroupText, TEXT("抽取结果"));

            // 获取所有显示列 
            LPTSTR lpTmp = GetStatusText();
            int nLen = gData.nShowItemCount;
            if (nLen>SHOWITEMS)
            {
                nLen = SHOWITEMS;
            }
            gLottery.dwShow[0] = TEXT("-来源页-");
            for (int i=0, n=1; i<nLen; i++)
            {
                int index = IDC_SHOW_ITEM1 + i;
                if ( IsDlgButtonChecked(gFrame.hMainWnd, index) )
                {
                    GetWindowText(gFrame.hBookShowItemWnd[i], lpTmp, MAX_PATH);
                    gLottery.dwShow[n++] = lpTmp;
                }
            }

            // 显示 
            LVCOLUMN lvc;
            lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
            lvc.fmt = LVCFMT_CENTER;
            for (int i=0; i<=gData.dwResultCount; i++)
            {
                lvc.pszText = (LPWSTR)gLottery.dwShow[i].c_str();
                lvc.cx = gLottery.dwShow[i].length()*40;
                lvc.iSubItem = i;
                ListView_InsertColumn(gLottery.hSysList, i, &lvc);
            }

            LVITEM newItem = {0};
            for (int i=0, nIndex=0; i<MAX_BOOKS; i++)
            {
                if (gData.bSelect[i])
                {
                    for (int lines=0; lines<gData.ulBookCount[i]; lines++)
                    {
                        newItem.iItem = nIndex;
                        ListView_InsertItem(gLottery.hSysList, &newItem); 

                        ListView_SetItemText(gLottery.hSysList, nIndex, 0, (LPTSTR)gData.book[i].name.c_str());
                        for(unsigned column = 1; column <= gData.dwResultCount; column++) {
                            ListView_SetItemText(gLottery.hSysList, nIndex, column, _T("等待数据..."));
                        }
                        nIndex++;
                    }
                }
            }

            EnableWindow(GetDlgItem(hWnd, IDC_OUTPUT), FALSE);
            CloseHandle(CreateThread(NULL, 0, LotteryAllData, NULL, 0, NULL));
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
            switch(nCtrlId)
            {
            case IDC_PREVIOUS: // 上一个 
                if (gLottery.nShowCurrent > 0)
                {
                    gLottery.nShowCurrent--;
                    SendMessage(gLottery.hWnd, WM_UPDATE_DLG, 0, 0);
                }
                break;
            case IDC_NEXT: // 下一个 
                if (gLottery.nShowCurrent < gData.ulRoundCount-1)
                {
                    gLottery.nShowCurrent++;
                    SendMessage(gLottery.hWnd, WM_UPDATE_DLG, 0, 0);
                }
                break;
            case IDC_RESET: // 重新生成 
                {
                    for (int i=0, nIndex=0; i<MAX_BOOKS; i++)
                    {
                        if (gData.bSelect[i])
                        {
                            for (int lines=0; lines<gData.ulBookCount[i]; lines++)
                            {
                                for(unsigned column = 1; column <= gData.dwResultCount; column++) {
                                    ListView_SetItemText(gLottery.hSysList, nIndex, column, _T("等待数据..."));
                                }
                                nIndex++;
                            }
                        }
                    }
                    LotteryAllData(NULL);
                }
                break;
            case IDC_OUTPUT:  // 导出结果 
                break;
            case IDC_TOPMOST:  // 顶端显示 
                {
                    if ( IsDlgButtonChecked(hWnd, IDC_TOPMOST) )
                    {
                        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
                        SetConfig(TEXT("TopMost"), 1);
                        SendMessage(gFrame.hTopMoustWnd, BM_SETCHECK, 1, 0);
                    }
                    else
                    {
                        SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
                        SetConfig(TEXT("TopMost"), 0);
                        SendMessage(gFrame.hTopMoustWnd, BM_SETCHECK, 1, 0);
                    }
                }
                break;
            }
        }
        break;
    case WM_UPDATE_DLG:
        {
            if (gLottery.nShowCurrent == 0)
            {
                EnableWindow(GetDlgItem(hWnd, IDC_PREVIOUS), FALSE);
            }
            else
            {
                EnableWindow(GetDlgItem(hWnd, IDC_PREVIOUS), TRUE);
            }

            if (gLottery.nShowCurrent >= gData.ulRoundCount-1)
            {
                EnableWindow(GetDlgItem(hWnd, IDC_NEXT), FALSE);
            }
            else
            {
                EnableWindow(GetDlgItem(hWnd, IDC_NEXT), TRUE);
            }

            LPTSTR lpTmp = GetStatusText();
            wsprintf(lpTmp, TEXT("第 %u/%u 组"), gLottery.nShowCurrent+1, gData.ulRoundCount);
            SetWindowText(gLottery.hGroupText, lpTmp);

            // 反正每次抽签个数是固定的，不删除列表了，直接替换数据 
            int nShowDialogLines = 0; // 当前设置的哪一行数据 
            for (int iBookPage=0; iBookPage<MAX_BOOKS; iBookPage++)
            {
                if (gData.bSelect[iBookPage])
                {
                    // 先找到表格内的数据索引 
                    int nRowCount = 0;
                    int nRowValue[SHOWITEMS] = {0};
                    for (nRowCount=0; nRowCount<=gData.dwResultCount; nRowCount++)
                    {
                        if (gLottery.dwShow[nRowCount].length() == 0)
                        {
                            break;
                        }

                        Elements* pCurElement = &gData.book[iBookPage].lines[0];
                        Elements::iterator it = std::find(
                            pCurElement->begin(),
                            pCurElement->end(),
                            gLottery.dwShow[nRowCount]);
                        nRowValue[nRowCount] = it - pCurElement->begin();
                    }

                    // 设置数据 
                    DWORD* pData = gLottery.dwData[iBookPage];  // 当前页面总所有抽出的数据 
                    pData = pData + gLottery.nShowCurrent*gData.ulBookCount[iBookPage];  // 第 gLottery.nShowCurrent 抽取的数据 
                    BookLines &books = gData.book[iBookPage].lines;  // 被抽取的页 
                    // 一页抽取多少行 
                    for (int nSelectIndex=0; nSelectIndex<gData.ulBookCount[iBookPage]; nSelectIndex++)
                    {
                        Elements &ele = books[pData[nSelectIndex]]; // 第几行数据 
                        // 选择出这一行数据 
                        for (int n=1; n<=gData.dwResultCount; n++)
                        {
                            ListView_SetItemText(gLottery.hSysList, nShowDialogLines, n, (LPTSTR)ele[nRowValue[n]].c_str());
                        }
                        nShowDialogLines++;
                    }
                }
            }
        }
        break;
    default:
        return FALSE;
        break;
    }
    return 0;
} 


DWORD WINAPI LotteryData(LPVOID lparam)
{
    for (int i=0; i<MAX_BOOKS; i++)
    {
        if (gData.bSelect[i])
        {
            // 按次序排放 
            gLottery.dwData[i] = new DWORD[gData.ulBookCount[i]*gData.ulRoundCount];
        }
        else
        {
            gLottery.dwData[i] = NULL;
        }
    }
    DialogBoxParam(gFrame.hInstance, MAKEINTRESOURCE(IDD_RESULT_DLG), gFrame.hMainWnd, (DLGPROC)ResultWndProc, NULL);
    for (int i=0; i<MAX_BOOKS; i++)
    {
        if (gLottery.dwData[i] != NULL)
        {
            delete []gLottery.dwData[i];
            gLottery.dwData[i] = NULL;
        }
    }
    return 0;
}