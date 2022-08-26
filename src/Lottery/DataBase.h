#ifndef _DATABASE_HEADER_FILE_H_
#define _DATABASE_HEADER_FILE_H_
#include <Windows.h>
#include <tchar.h>

#ifdef _cplusplus
extern "C" {
#endif // _cplusplus 

// ���ļ��ж����ݣ�����buffer��Ҫ FreeData �ͷ� 
BOOL ReadFromFile(LPTSTR szFileName, unsigned char** buf, unsigned int* len);
// ��bufferд��ָ���ļ� 
BOOL WriteToFile(LPTSTR szFileName, unsigned char* buf, unsigned int len);
// ���ļ����ܴ�Ϊ���ݿ� 
BOOL SaveFile2Database(LPTSTR lpFileName);
// �����ݿ��ж�ȡbuffer 
BOOL ReadDatabase(unsigned char** pb, unsigned int* size);
// �ͷ��ڴ� 
VOID FreeData(void* pb);


#ifdef _cplusplus
};
#endif // _cplusplus

#endif  // _DATABASE_HEADER_FILE_H_ 
