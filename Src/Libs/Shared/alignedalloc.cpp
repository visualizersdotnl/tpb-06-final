#include "stdafx.h"
#include "alignedalloc.h"
#include "misc.h"
#include "assert.h"

void* AlignedAlloc(int size, int alignment)
{
	return _aligned_malloc(size, alignment);
}

void AlignedFree(void* alignedPtr)
{
	_aligned_free(alignedPtr);
}
