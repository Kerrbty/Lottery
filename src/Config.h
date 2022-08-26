#ifndef _PROGRAM_CONDFIG_HEADER_H_
#define _PROGRAM_CONDFIG_HEADER_H_
#include <windows.h>
#include <tchar.h>

LPCTSTR GetExePath();

// 读取保存配置 
UINT GetConfig(LPCTSTR lpConfigName);

// 保存配置
VOID SetConfig(LPCTSTR lpConfigName, INT val);

#endif // _PROGRAM_CONDFIG_HEADER_H_