#include "Config.h"
#include <Shlwapi.h>
#include "ImportData.h"
#pragma comment(lib, "shlwapi.lib")

LPCTSTR GetExePath()
{
    if (gData.lpExePath == NULL)
    {
        gData.lpExePath = new TCHAR[MAX_PATH];
        if (gData.lpExePath)
        {
            GetModuleFileName(GetModuleHandle(NULL), gData.lpExePath, MAX_PATH);
            PathRemoveFileSpec(gData.lpExePath);
        }
    }
    return gData.lpExePath;
}

// 读取保存配置 
UINT GetConfig(LPCTSTR lpConfigName)
{ 
    UINT retcfg = 0;
    LPTSTR lpIniFileName = (LPTSTR)malloc(MAX_PATH*2*sizeof(TCHAR));
    if (lpIniFileName)
    {
        GetModuleFileName(GetModuleHandle(NULL), lpIniFileName, MAX_PATH);
        PathRenameExtension(lpIniFileName, TEXT(".ini"));
        retcfg = GetPrivateProfileInt(TEXT("CONFIG"), lpConfigName, 0, lpIniFileName);
        free(lpIniFileName);
    }
    return retcfg;
}

// 保存配置
VOID SetConfig(LPCTSTR lpConfigName, INT val)
{ 
    LPTSTR lpIniFileName = (LPTSTR)malloc(MAX_PATH*2*sizeof(TCHAR));
    if (lpIniFileName)
    {
        LPTSTR lpcfg = (LPTSTR)malloc(MAX_PATH*2*sizeof(TCHAR));
        if (lpcfg)
        {
            GetModuleFileName(GetModuleHandle(NULL), lpIniFileName, MAX_PATH);
            PathRenameExtension(lpIniFileName, TEXT(".ini"));
            wsprintf(lpcfg, TEXT("%u"), val);
            WritePrivateProfileString(TEXT("CONFIG"), lpConfigName, lpcfg, lpIniFileName);
            free(lpcfg);
        }
        free(lpIniFileName);
    }
}
