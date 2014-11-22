
#pragma once

// FIXME:
// - In _DEBUG add message output if wanted.
// - For both builds add a D3D exception throw (with descriptive message) to be catched by Player.

#ifdef _DEBUG
	#define D3D_ASSERT(condition) if (true == (condition != S_OK)) __debugbreak();
	#define D3D_VERIFY(condition) D3D_ASSERT(condition)
	#define D3D_ASSERT_MSG(condition, message) D3D_ASSERT(condition)
#else
	#define D3D_ASSERT(condition)
	#define D3D_VERIFY(condition) (condition)
	#define D3D_ASSERT_MSG(condition, message)
#endif
