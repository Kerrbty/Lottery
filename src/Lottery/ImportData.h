#ifndef _IMPORT_EXECL_DATA_HEADER_H_
#define _IMPORT_EXECL_DATA_HEADER_H_
#include <string>
#include <vector>

#define SHOWITEMS   18

#ifdef _UNICODE
#define _tstring std::wstring
#else
#define _tstring std::string
#endif // _UNICODE 

// ��ȡ������excel���ṹ 
typedef std::vector<_tstring> Elements;
typedef std::vector<Elements> BookLines;
typedef struct _BookInfo{
    _tstring name;
    BookLines lines; 
}BookInfo;
typedef std::vector<BookInfo> Book;

typedef struct _ExcelData{
    LPTSTR lpExePath;
    Book book;
    BYTE bSelect[10];   // ѡ��� 
    DWORD ulBookCount[10];  // ÿ�δӱ���ѡ������� 
    DWORD ulRoundCount;  // �ܳ�ȡ���� 
    BOOL bRepeatRount;   // �Ƿ�����ظ���ȡ 

    // ��������ű��й�ͬ�е������ڵ�һ�ű��е����� 
    unsigned int uiShowItems[SHOWITEMS+1]; 
    unsigned int nShowItemCount;   // ����п�����ʾ������Ŀ�� 
    DWORD dwResultCount;  // ���ѡ����ʾ����Ŀ��   
    BYTE bShowItems[SHOWITEMS+1];
    LPTSTR lpStatusTxt;
}ExcelData, *pExcelData;

extern ExcelData gData;

/**
  * ����Excel���� 
  * @data: excel ����ȡ���ڴ������ 
  * @uilen: excel ������ݳ��� 
  * @books: �����������ҳ�� 
  * >0 �Ƿ����ܹ�������Ŀ��ÿҳ����ѵ�һ�������е��ɱ����У� 
  */
int ImportFromXls(const char* data, unsigned int uilen, Book &books);

LPTSTR GetStatusText();

#endif // _IMPORT_EXECL_DATA_HEADER_H_