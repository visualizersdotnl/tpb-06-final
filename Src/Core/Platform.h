
// Global platform include for Core and all depending projects.
// - Includes core OS, DirectX, CRT & STL headers.
// - Defines often used macros, constants et cetera.

#if !defined(CORE_PLATFORM_H)
#define CORE_PLATFORM_H

// APIs
#include <windows.h>
#include <dxgi.h>
#include <d3d11.h>

// CRT & STL
#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm>
#include <exception>

// Legacy
#include <Shared/assert.h>

// COM release macro
#define SAFE_RELEASE(pX) if (nullptr != (pX)) (pX)->Release()

#endif // CORE_PLATFORM_H
