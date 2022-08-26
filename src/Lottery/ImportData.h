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

// 读取出来的excel表格结构 
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
    BYTE bSelect[10];   // 选择表 
    DWORD ulBookCount[10];  // 每次从表中选择的人数 
    DWORD ulRoundCount;  // 总抽取次数 
    BOOL bRepeatRount;   // 是否可以重复抽取 

    // 结果表，几张表中共同有的数据在第一张表中的索引 
    unsigned int uiShowItems[SHOWITEMS+1]; 
    unsigned int nShowItemCount;   // 结果中可以显示的总项目数 
    DWORD dwResultCount;  // 结果选择显示的项目数   
    BYTE bShowItems[SHOWITEMS+1];
    LPTSTR lpStatusTxt;
}ExcelData, *pExcelData;

extern ExcelData gData;

/**
  * 导入Excel数据 
  * @data: excel 表格读取到内存的数据 
  * @uilen: excel 表格数据长度 
  * @books: 输出解析到的页面 
  * >0 是返回总共数据条目（每页表格会把第一个完整行当成标题行） 
  */
int ImportFromXls(const char* data, unsigned int uilen, Book &books);

LPTSTR GetStatusText();

#endif // _IMPORT_EXECL_DATA_HEADER_H_