#pragma once

#ifdef _DEBUG

#include "conversions.h"
#include "exception.h"

#define ASSERT_MSG(cond, msg) if (!(cond)) { if (IsDebuggerPresent()) DebugBreak(); throw Exception(std::string("At ")+std::string(__FILE__)+std::string(":")+Int2Str(__LINE__)+std::string(":\n")+(msg)); }
#define ASSERT(cond) ASSERT_MSG(cond, #cond)

#else

#define ASSERT_MSG(x,y) {}
#define ASSERT(x) {}

#endif