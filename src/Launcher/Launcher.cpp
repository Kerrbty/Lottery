#include <Windows.h>
#include <tchar.h>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

LPCTSTR szFileList[] = {
    _T("bin/Lottery.exe"),
    _T("bin/libxl.dll"),
    _T("bin/SkinH.dll"),
    _T("bin/SkinHu.dll"),
    _T("bin/Aero.she"),
    _T("bin/QQ2008.she"),
    _T("bin/skinh.she"),
};

void Launcher()
{
    // ��������ļ� 
    LPTSTR lpFile = new TCHAR[MAX_PATH*2];
    if (lpFile == NULL)
    {
        MessageBox(GetDesktopWindow(), TEXT("��������"), TEXT("����"), MB_OK);
        return ;
    }

    GetModuleFileName(GetModuleHandle(NULL), lpFile, MAX_PATH);
    LPTSTR lpName = PathFindFileName(lpFile);
    for (int i=0; i<sizeof(szFileList)/sizeof(szFileList[0]); i++)
    {
        _tcscpy(lpName, szFileList[i]);
        if ( !PathFileExists(lpFile) )
        {
            wsprintf(lpFile, _T("��ʧ�ļ� %s , �޷���������"), szFileList[i]);
            MessageBox(GetDesktopWindow(), lpFile, _T("������"), MB_OK);
            delete []lpFile;
            return ;
        }
    }
   
    // ������һ�� 
    _tcscpy(lpName, szFileList[0]);
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi = { 0 };
    if (CreateProcess(NULL, lpFile, NULL, NULL, FALSE, CREATE_NEW_CONSOLE|BELOW_NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
    {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Launcher();
    return 0;
}