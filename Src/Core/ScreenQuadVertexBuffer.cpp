
#include "Platform.h"
#include "D3D.h"
#include "ScreenQuadVertexBuffer.h"
#include "Effect.h"
#include "EffectPass.h"

namespace Pimp
{
	static const ScreenQuadVertex screenQuad[] = 
	{
		{ Vector3(-1, 1, 0) }, // 0
		{ Vector3( 1, 1, 0) }, // 1
		{ Vector3( 1,-1, 0) }, // 3
		{ Vector3(-1, 1, 0) }, // 0
		{ Vector3( 1,-1, 0) }, // 3
		{ Vector3(-1,-1, 0) }, // 2						
	};

	ScreenQuadVertexBuffer::ScreenQuadVertexBuffer(EffectPass* inputLayoutEffectPass)
	{
		ASSERT(inputLayoutEffectPass != NULL);

		vertices = gD3D->CreateVertexBuffer(6*sizeof(ScreenQuadVertex), (void*)screenQuad, false);	

		unsigned char* signature;
		int signatureLength;
		inputLayoutEffectPass->GetVSInputSignature(&signature, &signatureLength);

		static const D3D10_INPUT_ELEMENT_DESC layoutDesc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};

		inputLayout = gD3D->CreateInputLayout(layoutDesc, sizeof(layoutDesc)/sizeof(D3D10_INPUT_ELEMENT_DESC), signature, signatureLength);

		delete [] signature;
	}

	ScreenQuadVertexBuffer::~ScreenQuadVertexBuffer()
	{
		SAFE_RELEASE(inputLayout);
		SAFE_RELEASE(vertices);
	}

	void ScreenQuadVertexBuffer::Bind()
	{
		gD3D->BindVertexBuffer(0, vertices, sizeof(ScreenQuadVertex));
		gD3D->BindInputLayout(inputLayout);
	}
}
