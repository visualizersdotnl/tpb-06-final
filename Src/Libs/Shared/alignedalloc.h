#pragma once

void* AlignedAlloc(int size, int alignment);
void AlignedFree(void* alignedPtr);
