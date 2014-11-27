
#pragma once

namespace Pimp
{
	class EffectPass;

	class ScreenQuad
	{
	private:
		ID3D11Buffer* vertices;
		ID3D11InputLayout* inputLayout;

	public:
		ScreenQuad(const EffectPass& inputLayoutEffectPass);
		~ScreenQuad();

		void Bind();
		void Draw();
	};
}
