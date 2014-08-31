#pragma once

void* AlignedAlloc(int size, int alignment);
void AlignedFree(void* alignedPtr);

// Allocate virtual memory:
//	- minimum page size is always 4kb
//	- always aligned to 4kb
//	- initialized to zeroes.
void* MyVirtualAlloc(int sizeBytes);
void MyVirtualFree(void* ptr);
