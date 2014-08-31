#include "stdafx.h"
#include "alignedalloc.h"
#include "misc.h"
#include "assert.h"

#define PIMP_ALIGNED_ALLOC_ANALYSIS 0

void* AlignedAlloc(int size, int alignment)
{
	ASSERT( alignment > 0 );
	ASSERT( IsPowerOfTwo(alignment) );
	
	if (size == 0)
	{
		return NULL;
	}

	void* unalignedPtr = HeapAlloc( GetProcessHeap(), 0, size + alignment + sizeof(void*) );
	
	ASSERT( unalignedPtr != NULL );

	//
	// +---------------------------+-----+-----------------+
	// |   alignment padding       | ptr |      size       |
	// +---------------------------+-----+-----------------+
	// ^                                 ^
	// | allocated ptr                   | returned ptr
	//
	// and 'ptr' contains a pointer to the allocated ptr.


	char* alignedData = (char*)unalignedPtr + sizeof(void*);

	int misAlign = ((unsigned int)alignedData) & (alignment-1); // should be 0

	if (misAlign != 0)
	{
		// Enforce alignment, by offsetting ourself a bit
		alignedData += alignment - misAlign;
	}

	char* ptr = alignedData - sizeof(void*);
	ASSERT( ptr >= unalignedPtr );

	ASSERT( sizeof(void*) == 4 );
	*(DWORD*)ptr  = (DWORD)unalignedPtr;

//	CopySomeMemory(ptr, &unalignedPtr, sizeof(void*));

	return alignedData;
}

void AlignedFree(void* alignedPtr)
{
	if (alignedPtr == NULL)
		return;

	char* dataPtr = (char*)alignedPtr - sizeof(void*);

	DWORD unalignedPtr = 0;

	ASSERT( sizeof(void*) == 4 );
	unalignedPtr = *(DWORD*)dataPtr;

//	CopySomeMemory(&unalignedPtr, dataPtr, sizeof(void*));

	ASSERT( unalignedPtr != 0 );
	ASSERT( (char*)unalignedPtr <= dataPtr );

	HeapFree( GetProcessHeap(), 0, (void*)unalignedPtr );
}


struct AllocInfo
{
	void* ptr;
	int sizeBytes;
};


#if PIMP_ALIGNED_ALLOC_ANALYSIS

#define PIMP_NUM_ALLOCS 1024

static int gNumAllocs = 0;
static AllocInfo gAllocs[PIMP_NUM_ALLOCS];
static int gTotalAllocedMem = 0;

#endif


void* MyVirtualAlloc(int sizeBytes)
{
#ifdef _DEBUG
	void* ptr = VirtualAlloc(
		NULL, 
		sizeBytes, 
		MEM_COMMIT | MEM_RESERVE,
		PAGE_READWRITE);

	if (!ptr)
		throw Win32Exception();

#else
	void* ptr = VirtualAlloc(
		NULL, 
		sizeBytes, 
		MEM_COMMIT | MEM_RESERVE,
		PAGE_READWRITE);
#endif

#if PIMP_ALIGNED_ALLOC_ANALYSIS
	gAllocs[gNumAllocs].sizeBytes = sizeBytes;
	gAllocs[gNumAllocs].ptr = ptr;
	gNumAllocs++;
	gTotalAllocedMem += sizeBytes;
#endif

	return ptr;
}


void MyVirtualFree(void* ptr)
{
	ASSERT( ptr != NULL );

#if PIMP_ALIGNED_ALLOC_ANALYSIS
	for (int i=0; i<gNumAllocs; ++i)
	{
		if (gAllocs[i].ptr == ptr)
		{
			gTotalAllocedMem -= gAllocs[i].sizeBytes;
			gAllocs[i] = gAllocs[gNumAllocs-1];
			gNumAllocs--;			
			break;
		}
	}
#endif

#ifdef _DEBUG
	bool result = (VirtualFree(ptr, 0, MEM_RELEASE) == TRUE);

	if (!result)
		throw Win32Exception();
#else
	VirtualFree(ptr, 0, MEM_RELEASE);
#endif
}


