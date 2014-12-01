
#include "Platform.h"
#include <Math/math.h>
#include "D3D.h"
#include "ScreenQuad.h"
#include "Effect.h"
#include "EffectPass.h"

namespace Pimp
{
	static const Vector3 screenQuad[] = 
	{
		Vector3(-1, 1, 0), // 0
		Vector3( 1, 1, 0), // 1
		Vector3( 1,-1, 0), // 3
		Vector3(-1, 1, 0), // 0
		Vector3( 1,-1, 0), // 3
		Vector3(-1,-1, 0), // 2						
	};

	ScreenQuad::ScreenQuad(const EffectPass& inputLayoutEffectPass)
	{
		vertices = gD3D->CreateVertexBuffer(6*sizeof(Vector3), reinterpret_cast<const void*>(screenQuad), false);	

		unsigned char* signature;
		int signatureLength;
		inputLayoutEffectPass.GetVSInputSignature(&signature, &signatureLength);

		static const D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		inputLayout = gD3D->CreateInputLayout(layoutDesc, sizeof(layoutDesc)/sizeof(D3D11_INPUT_ELEMENT_DESC), signature, signatureLength);

		delete [] signature;
	}

	ScreenQuad::~ScreenQuad()
	{
		SAFE_RELEASE(inputLayout);
		SAFE_RELEASE(vertices);
	}

	void ScreenQuad::Bind()
	{
		gD3D->BindVertexBuffer(0, vertices, sizeof(Vector3));
		gD3D->BindInputLayout(inputLayout);
	}

	void ScreenQuad::Draw()
	{
		gD3D->DrawTriQuad(0);
	}
}
