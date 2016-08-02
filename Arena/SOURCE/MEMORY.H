//
// memory.h
// craig
//

#ifndef _MEMORY_H
#define _MEMORY_H

#include "includeall.h"

void Memory_Init();
void Memory_Exit();
u32 Memory_GetUsage();
#ifdef _DEBUG

void *Mem_New2(size_t nSize,char *pFile,i32 iLine);
void Mem_Free2(void *pAddress);

#define Mem_New(nSize) Mem_New2(nSize,__FILE__,__LINE__);
#define Mem_Free(pAddress) Mem_Free2(pAddress);

#else
#define Mem_New(nSize) malloc(nSize);
#define Mem_Free(pAddress) free(pAddress);
#endif //_DEBUG

#endif //_MEMORY_H
