#pragma once

#include "stdafx.h"


struct FloatColor
{
	float r,g,b,a;

	FloatColor()
	{
	}

	FloatColor(float r, float g, float b, float a)
		: r(r),g(g),b(b),a(a)
	{
	}
};