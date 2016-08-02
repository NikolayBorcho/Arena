//
// memory.cpp
// craig
//

#include "memory.h"

typedef struct _MemoryHeader
{
	char cFile[128];
	i32 iLine;
	void *pMemory;
	u32 uSize;

	_MemoryHeader *pNext;
}MemoryHeader;

char cMemoryTemp[128];
i32 iMemoryHeaders;
MemoryHeader *pHeaders;
static u32 iMemUsage;
static CRITICAL_SECTION section;

//----------------------------------------------------------------

void Memory_Init()
{
	iMemoryHeaders=0;
	pHeaders=0;
	iMemUsage=0;

	InitializeCriticalSection(&section);
}

//----------------------------------------------------------------

void Memory_Exit()
{
#ifdef _DEBUG
	MemoryHeader *pHeader=pHeaders;
	MemoryHeader *pHeaderFree[300];
	i32 iDeleteCount=0;
	char cText[256];

	if(pHeader)
	{
		sprintf(cText,"----------- Memory not Freed -----------\n");
		OutputDebugString(cText);
		while(pHeader)
		{
			sprintf(cText,"File: %s Line: %d\n",pHeader->cFile,pHeader->iLine);
			OutputDebugString(cText);
			pHeaderFree[iDeleteCount]=pHeader;
			iDeleteCount++;
			ASSERT(iDeleteCount<300,"AARGGHH!! too much memory not freed");
			pHeader=pHeader->pNext;
		}
		while(iDeleteCount)
		{
			iDeleteCount--;
			Mem_Free2(pHeaderFree[iDeleteCount]->pMemory);
		}
	}
#endif
}

//----------------------------------------------------------------

u32 Memory_GetUsage()
{
	return iMemUsage;
}

//----------------------------------------------------------------

#ifdef _DEBUG

void *Mem_New2(size_t nSize,char *pFile,i32 iLine)
{
	EnterCriticalSection(&section);

	MemoryHeader *pHeader;

	ASSERT(_CrtCheckMemory(),"bad memory");

	pHeader=(MemoryHeader *)malloc(sizeof(MemoryHeader));

	ASSERT(pHeader,"Out of memory");

	if(!pHeaders)
	{
		pHeaders=pHeader;
		pHeaders->pNext=NULL;
	}
	else
	{
		pHeader->pNext=pHeaders;
		pHeaders=pHeader;
	}

	char *pFile2=pFile;
	char *pFileBuff=pHeaders->cFile;

	pHeaders->pMemory = malloc(nSize);

	ASSERT(_CrtCheckMemory(),"bad memory");

	ASSERT(pHeaders->pMemory,"Out of memory");
	
	while(*pFile2)
	{
		*pFileBuff=*pFile2;
		if(*pFile2=='/' || *pFile2=='\\')
		{
			pFileBuff=pHeaders->cFile;
		}
		else
		{
			pFileBuff++;
		}
		pFile2++;
		
	}
	*pFileBuff=0;
	pHeaders->iLine=iLine;
	pHeaders->uSize=(u32)nSize;
	iMemUsage+=(u32)nSize;

	ASSERT(_CrtCheckMemory(),"bad memory");

	LeaveCriticalSection(&section);

	return(pHeaders->pMemory);
}

//----------------------------------------------------------------

void Mem_Free2(void *pAddress)
{
	MemoryHeader *pHeader=pHeaders;
	MemoryHeader *pHeaderLast=0;
	MemoryHeader *pHeaderDelete=0;

	while(pHeader)
	{
		if(pHeader->pMemory==pAddress)
		{
			iMemUsage-=pHeader->uSize;
			free(pHeader->pMemory);
			pHeaderDelete=pHeader;
			if(pHeaderLast)
			{
				pHeaderLast->pNext=pHeader->pNext;
			}
			else
			{
				pHeaders=pHeaders->pNext;
			}
			free(pHeaderDelete);

			ASSERT(_CrtCheckMemory(),"bad memory");

			return;
		}
		
		pHeaderLast=pHeader;
		pHeader=pHeader->pNext;
	}
	ASSERT(0,"Freeing memory twice");
}

#endif // _DEBUG