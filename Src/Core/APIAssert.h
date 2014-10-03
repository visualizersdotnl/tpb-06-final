
#pragma once

// @plek: Temporary placeholder for the D3D asserts.
#ifdef _DEBUG
	#define D3D_ASSERT(condition) if (true == (condition != S_OK)) __debugbreak();
	#define D3D_VERIFY(condition) D3D_ASSERT(condition)
	#define D3D_ASSERT_MSG(condition, message) D3D_ASSERT(condition) // FIXME: Add message output.
#else
	#define D3D_ASSERT(condition)
	#define D3D_VERIFY(condition) (condition)
	#define D3D_ASSERT_MSG(condition)
#endif
