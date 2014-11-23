
// Global platform include for Core and all depending projects.
// - Includes core OS, DirectX, CRT & STL headers.
// - Defines often used macros, constants et cetera.

#pragma once

// APIs
#include <windows.h>
#include <DXGI.h>
#include <d3d10_1.h>

// CRT & STL
#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm>
#include <exception>

// ** Must be implemented by host! **
void SetLastError(const std::string &message);

// COM release macro
#define SAFE_RELEASE(pX) if (NULL != (pX)) (pX)->Release()
