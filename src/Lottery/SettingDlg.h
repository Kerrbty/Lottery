#ifndef _SETTING_DLALOG_HEADER_H_
#define _SETTING_DLALOG_HEADER_H_
#include <Windows.h>
#include <tchar.h>
#include "ImportData.h"

typedef struct {
    HINSTANCE hInstance; 
    HWND hMainWnd;

    HWND hSelectGroupWnd;  // 选择表格页文本 
    HWND hBookWnd[MAX_BOOKS];     // 各表格选择框 
    HWND hBookSetTextWnd[MAX_BOOKS];   // 各表格数量框前面的文本 
    HWND hBookSetCountWnd[MAX_BOOKS];  // 各表格数量选择框 
    HWND hResultSelectGroupWnd;  // 结果列表选择宽文本 
    HWND hBookShowItemWnd[SHOWITEMS];  // 结果显示列 
    HWND hRoundCountWnd;   // 总选择组数 

    HWND hTopMoustWnd;  // 顶端显示选项 
    HWND hStatusWnd;  // 状态提示信息 
}WndFrame;

extern WndFrame gFrame;

#endif  // _SETTING_DLALOG_HEADER_H_ 
