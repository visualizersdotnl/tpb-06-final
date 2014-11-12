
// Global platform include for Core and all depending projects.
// - Includes core OS, DirectX, CRT & STL header.
// - Defines often used macros, constants et cetera.

#pragma once

// COM release macro
#define SAFE_RELEASE(pX) if (NULL != (pX)) (pX)->Release()

// FIXME: work in progress!
