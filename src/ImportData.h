#ifndef _IMPORT_EXECL_DATA_HEADER_H_
#define _IMPORT_EXECL_DATA_HEADER_H_
#include <string>
#include <vector>

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

/**
  * ����Excel���� 
  * @data: excel ����ȡ���ڴ������ 
  * @uilen: excel ������ݳ��� 
  * @books: �����������ҳ�� 
  * �����ܹ�������Ŀ��ÿҳ����ѵ�һ�������е��ɱ����� 
  */
unsigned int ImportFromXls(const char* data, unsigned int uilen, Book &books);

#endif // _IMPORT_EXECL_DATA_HEADER_H_