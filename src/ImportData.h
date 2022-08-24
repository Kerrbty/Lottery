#ifndef _IMPORT_EXECL_DATA_HEADER_H_
#define _IMPORT_EXECL_DATA_HEADER_H_
#include <string>
#include <vector>

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

/**
  * 导入Excel数据 
  * @data: excel 表格读取到内存的数据 
  * @uilen: excel 表格数据长度 
  * @books: 输出解析到的页面 
  * 返回总共数据条目（每页表格会把第一个完整行当成标题行 
  */
unsigned int ImportFromXls(const char* data, unsigned int uilen, Book &books);

#endif // _IMPORT_EXECL_DATA_HEADER_H_