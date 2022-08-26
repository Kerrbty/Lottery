#ifndef _DATABASE_HEADER_FILE_H_
#define _DATABASE_HEADER_FILE_H_
#include <Windows.h>
#include <tchar.h>

#ifdef _cplusplus
extern "C" {
#endif // _cplusplus 

// 从文件中读数据，返回buffer需要 FreeData 释放 
BOOL ReadFromFile(LPTSTR szFileName, unsigned char** buf, unsigned int* len);
// 将buffer写入指定文件 
BOOL WriteToFile(LPTSTR szFileName, unsigned char* buf, unsigned int len);
// 将文件加密存为数据库 
BOOL SaveFile2Database(LPTSTR lpFileName);
// 从数据库中读取buffer 
BOOL ReadDatabase(unsigned char** pb, unsigned int* size);
// 释放内存 
VOID FreeData(void* pb);


#ifdef _cplusplus
};
#endif // _cplusplus

#endif  // _DATABASE_HEADER_FILE_H_ 
