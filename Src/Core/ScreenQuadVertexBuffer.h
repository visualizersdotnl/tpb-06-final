
#pragma once

namespace Pimp
{
	class EffectPass;

	struct ScreenQuadVertex
	{
		Vector3 position;
	};

	class ScreenQuadVertexBuffer
	{
	private:
		ID3D11Buffer* vertices;
		ID3D11InputLayout* inputLayout;

	public:
		ScreenQuadVertexBuffer(EffectPass* inputLayoutEffectPass);
		virtual ~ScreenQuadVertexBuffer();

		void Bind();
	};
}
