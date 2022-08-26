#include "DataBase.h"
#include "zlib/zlib.h"
#include "Config.h"

#define DATA_BASE_FILE TEXT("Data.bin")

BOOL ReadFromFile(LPTSTR szFileName, unsigned char** buf, unsigned int* len)
{
    BOOL bRet = FALSE;
    HANDLE hFile = CreateFile( 
        szFileName,
        GENERIC_READ, 
        FILE_SHARE_READ, 
        NULL, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return bRet;
    }

    // 读取整个文件 
    DWORD dwSize = GetFileSize(hFile, NULL);
    unsigned char* pibuf = new unsigned char[dwSize];
    if (pibuf)
    {
        DWORD dwBytes = 0;
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        ReadFile(hFile, pibuf, dwSize, &dwBytes, NULL);
        
        *buf = pibuf;
        *len = dwBytes;
        bRet = TRUE;
    }
    CloseHandle(hFile);

    return bRet;
}

BOOL WriteToFile(LPTSTR szFileName, unsigned char* buf, unsigned int len)
{
    HANDLE hFile = CreateFile( 
        szFileName,
        GENERIC_WRITE, 
        FILE_SHARE_READ, 
        NULL, 
        CREATE_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    // 读取整个文件 
    DWORD dwBytes = 0;
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    WriteFile(hFile, buf, len, &dwBytes, NULL);
    CloseHandle(hFile);

    return TRUE;
}

VOID FreeData(void* pb)
{
    delete []pb;
}

BOOL SaveFile2Database(LPTSTR lpFileName)
{
    BOOL bRet = FALSE;
    unsigned char* pibuf = NULL;
    unsigned int len = 0;
    if (ReadFromFile(lpFileName, &pibuf, &len))
    {
        DWORD dwCompressLen = len;
        unsigned char* pobuf = new unsigned char[dwCompressLen];
        if (pobuf)
        {
            // 压缩 
            if( compress(pobuf, &dwCompressLen, pibuf, len) == Z_OK)
            {
                // 保存文件 
                LPTSTR lpName = new TCHAR[MAX_PATH*2];
                if (lpName)
                {
                    wsprintf(lpName, TEXT("%s\\") DATA_BASE_FILE, GetExePath());
                    bRet = WriteToFile(lpName, pobuf, dwCompressLen);
                    delete []lpName;
                }
            }
            delete []pobuf;
        }
        FreeData(pibuf);
    }  
    return bRet;
}

BOOL ReadDatabase(unsigned char** pb, unsigned int* size)
{
    BOOL bRet = FALSE;
    unsigned char* pibuf = NULL;
    unsigned int len = 0;

    LPTSTR lpName = new TCHAR[MAX_PATH*2];
    if (lpName == NULL)
    {
        return bRet;
    }

    wsprintf(lpName, TEXT("%s\\") DATA_BASE_FILE, GetExePath());
    if (ReadFromFile(lpName, &pibuf, &len))
    {
        int n = 1;
        DWORD dwLen = len*2*n;
        unsigned char* pobuf = new unsigned char[dwLen];
        do 
        {
            if (pobuf == NULL)
            {
                break;
            }

            // 压缩 
            int res = uncompress(pobuf, &dwLen, pibuf, len);
            if(res == Z_OK)
            {
                *pb = pobuf;
                *size = dwLen;
                bRet = TRUE;
                break;
            }
            else if (res == Z_BUF_ERROR)
            {
                delete []pobuf;

                n++;
                dwLen = len*2*n;
                pobuf = new unsigned char[dwLen];
            }
            else
            {
                break;
            }
        } while (TRUE);
        FreeData(pibuf);

        if (!bRet)
        {
            delete []pobuf;
        }
    } 

    delete []lpName;
    return bRet;
}

#ifdef _TEST_DATABASE_HEADER_H_

int main()
{
    // 保存数据库 
    SaveFile2Database(TEXT("名单.xls"));

    // 读取数据库 
    unsigned char* pibuf = NULL;
    unsigned int len = 0;
    ReadDatabase(&pibuf, &len);
    FreeData(pibuf);

    return 0;
}

#endif // _TEST_DATABASE_HEADER_H_