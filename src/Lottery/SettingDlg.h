#ifndef _SETTING_DLALOG_HEADER_H_
#define _SETTING_DLALOG_HEADER_H_
#include <Windows.h>
#include <tchar.h>
#include "ImportData.h"

typedef struct {
    HINSTANCE hInstance; 
    HWND hMainWnd;

    HWND hSelectGroupWnd;  // ѡ����ҳ�ı� 
    HWND hBookWnd[MAX_BOOKS];     // �����ѡ��� 
    HWND hBookSetTextWnd[MAX_BOOKS];   // �����������ǰ����ı� 
    HWND hBookSetCountWnd[MAX_BOOKS];  // ���������ѡ��� 
    HWND hResultSelectGroupWnd;  // ����б�ѡ����ı� 
    HWND hBookShowItemWnd[SHOWITEMS];  // �����ʾ�� 
    HWND hRoundCountWnd;   // ��ѡ������ 

    HWND hTopMoustWnd;  // ������ʾѡ�� 
    HWND hStatusWnd;  // ״̬��ʾ��Ϣ 
}WndFrame;

extern WndFrame gFrame;

#endif  // _SETTING_DLALOG_HEADER_H_ 
