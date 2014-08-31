#pragma once

#include <d3d10_1.h>
#include <math/math.h>
#include "EffectPass.h"

namespace Pimp
{
	struct ScreenQuadVertex
	{
		Vector3 position;
	};

	class ScreenQuadVertexBuffer
	{
	private:
		ID3D10Buffer* vertices;
		ID3D10InputLayout* inputLayout;

	public:
		ScreenQuadVertexBuffer(EffectPass* inputLayoutEffectPass);
		virtual ~ScreenQuadVertexBuffer();

		void Bind();
	};

}
