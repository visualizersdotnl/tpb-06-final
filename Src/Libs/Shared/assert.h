#pragma once

#ifdef _DEBUG
	#define ASSERT(condition) if (!(condition)) __debugbreak();
	#define VERIFY(condition) ASSERT(condition)
	#define ASSERT_MSG(condition, message) ASSERT(condition) // FIXME: Add message output.
#else
	#define ASSERT(condition)
	#define VERIFY(condition) (condition)
	#define ASSERT_MSG(condition, message)
#endif
