#include <stdio.h>
#include <locale.h>
#include <Windows.h>
#include <tchar.h>
#include "ImportData.h"
#include "include/libxl.h"
#pragma comment(lib, "./lib/libxl.lib")

#define REG_BOOK(_book_xls_)  xlBookSetKey(_book_xls_, TEXT("jha334201553355102433ahj"), TEXT("windows-2c25240709c0e60565b36d6ca9r1p5kf"))  //ע��

// ��excel�ļ� 
static BOOL OpenXlsFile(const char* data, unsigned int uilen, BookHandle &ByReadXlsBook)
{
    if (ByReadXlsBook == NULL)
    {
        return FALSE;
    }

    _tsetlocale(LC_ALL, TEXT("chs"));

    // ��book��ʽ���� 
    ByReadXlsBook = xlCreateBook();
    if (ByReadXlsBook)
    {
        REG_BOOK(ByReadXlsBook);   //ע��
        if(xlBookLoadRaw(ByReadXlsBook, data, uilen)) 
        {
            return TRUE;        
        } 
    }
    xlBookRelease(ByReadXlsBook);
    
    // ��xml��ʽ���� 
    ByReadXlsBook = xlCreateXMLBook();
    if (ByReadXlsBook)
    {
        REG_BOOK(ByReadXlsBook);   //ע��
        if(xlBookLoadRaw(ByReadXlsBook, data, uilen))
        {
            return TRUE;
        }
    }
    xlBookRelease(ByReadXlsBook);

    // �޷����� 
    MessageBox(GetDesktopWindow(), TEXT("����ʽ������ת����ʽ������"), TEXT("��ʽ����"), MB_OK|MB_ICONSTOP);
    return FALSE;
}

// ��ҳ����excel 
static unsigned int GetBookDatas(BookHandle ByReadXlsBook, int index, BookInfo &book)
{
    LPCTSTR szName = NULL;
    unsigned int nSpaceSkip = 0;
    unsigned int nMaxRows = 0;  // ����� 
    unsigned int nMaxCols = 0;  // ����� 
    TCHAR data[MAX_PATH];

    SheetHandle sheet = xlBookGetSheet(ByReadXlsBook, index); 
    book.name = xlSheetName(sheet);

    // �ȼ��������к������ 
    for (unsigned int row=0; row<65536; row++) // �� 
    {
        unsigned int nColumns = 0;
        for (unsigned int col=0; col<256; col++)  // �� 
        {
            CellType nType = (CellType)xlSheetCellType(sheet, row, col);
            if (nType == CELLTYPE_EMPTY)
            {
                continue;
            }
            nColumns++;
        }

        if (nColumns == 0)
        {
            nSpaceSkip++;
        }
        else
        {
            nMaxRows = nMaxRows + nSpaceSkip + 1; // �������� 
            // ����ȡ���ֵ 
            if (nMaxCols<nColumns)
            {
                nMaxCols = nColumns;
            }
            nSpaceSkip = 0;
        }

        // ����5�пհף����� 
        if (nSpaceSkip>5)
        {
            break;
        }
    }

    // ������ȡ���� 
    for (unsigned int i=0; i<nMaxRows; i++)
    {
        Elements celes;
        for (unsigned int j=0; j<nMaxCols; j++)
        {
            CellType nType = (CellType)xlSheetCellType(sheet, i, j);
            switch (nType)
            {
            case CELLTYPE_EMPTY:
                szName = TEXT("");
                break;
            case CELLTYPE_NUMBER:
                {
                    double num = xlSheetReadNum(sheet, i, j, NULL);
                    int nLen = _stprintf(data, TEXT("%.02lf"), num);
                    for (int i=nLen-1; i>0; i--)
                    {
                        if (data[i] == TEXT('0'))
                        {
                            data[i] = TEXT('\0');
                        }
                        else
                        {
                            if (data[i] == TEXT('.'))
                            {
                                data[i] = TEXT('\0');
                            }
                            break;
                        }
                    }
                    szName = data;
                }
                break;
            case CELLTYPE_STRING:
                szName = xlSheetReadStr(sheet, i, j, NULL);
                break;
            case CELLTYPE_BOOLEAN:
                {
                    int b = xlSheetReadBool(sheet, i, j, NULL);
                    if (b==0)
                    {
                        szName = TEXT("True");
                    }
                    else
                    {
                        szName = TEXT("False");
                    }
                }
                break;
            case CELLTYPE_BLANK:
                // xlSheetReadBlank() 
                szName = TEXT(""); 
                break;
            case CELLTYPE_ERROR:
                break;
            default:
                break;
            }

            celes.push_back(szName);
        }

        // ������ 
        if ( celes.size()<nMaxCols )
        {
            continue;
        }

        // ���㼸���ж��ٵ�Ԫ�ǿյ� 
        unsigned int nNotSpaceLine = 0;
        for (unsigned int j=0; j<nMaxCols; j++)
        {
            if (celes[j].length()>0)
            {
                nNotSpaceLine++;
            }
        }
        // ���������ǿհף�����Ϊ������ 
        if ((nNotSpaceLine+nNotSpaceLine) > nMaxCols)
        {
            book.lines.push_back(celes);
        }
    }

    if (book.lines.size() > 0)
    {
        return (book.lines.size()-1);
    }
    return 0;
}

// ����excel�ļ� 
int ImportFromXls(const char* data, unsigned int uilen, Book &books)
{
    unsigned int nCount = 0;
    BookHandle ByReadXlsBook;

    books.clear();
    if ( !OpenXlsFile(data, uilen, ByReadXlsBook) )
    {
        return -1;
    }

    int nBooks = xlBookSheetCount(ByReadXlsBook);
#ifdef _DEBUG
    printf("��ȡ����� %u ��\n", nBooks);
#endif
    for (int i=0; i<nBooks; i++)
    {
#ifdef _DEBUG
        printf("������%u�ű�\n", i);
#endif
        BookInfo book;
        unsigned int nDatas = GetBookDatas(ByReadXlsBook, i, book);
        // ����������һ�б��⣬һ������ 
        if (book.lines.size()>1)
        {
            books.push_back(book);
        }
#ifdef _DEBUG
        printf("��ȡ������ %u ��\n", nDatas);
#endif
        nCount += nDatas;
    }

    xlBookRelease(ByReadXlsBook);
    return nCount;
}

LPTSTR GetStatusText()
{
    if (gData.lpStatusTxt == NULL)
    {
        gData.lpStatusTxt = new TCHAR[MAX_PATH*2];
    }
    return gData.lpStatusTxt;
}

//////////////////////////////////////////////////////////////////////////
// ���� 
#ifdef _TEST_IMPORT_DATA_

int main()
{
    Book books;

    HANDLE hFile = CreateFile( 
        TEXT("����.xls"),
        GENERIC_READ, 
        FILE_SHARE_READ, 
        NULL, 
        OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return -1;
    }
    DWORD dwSize = GetFileSize(hFile, NULL);
    char* pbuffer = new char[dwSize];
    if (pbuffer)
    {
        DWORD dwBytes = 0;
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        ReadFile(hFile, pbuffer, dwSize, &dwBytes, NULL);
        ImportFromXls(pbuffer, dwSize, books);
        delete []pbuffer;
    }
    CloseHandle(hFile);    
    return 0;
}

#endif // _TEST_IMPORT_DATA_ 
