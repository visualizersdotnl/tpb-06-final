#include "D3D.h"
#include "Material.h"
#include "World.h"


#define PIMP_MAX_NUM_BOUNDTEXTUREVARINDICES 256
#define PIMP_MAX_NUM_BOUNDMATERIALPARAMETERS 256

namespace Pimp 
{
	Material::Material(
		World* world, 
		const unsigned char* shaderCompiledText, int shaderCompiledTextLength,
		const std::string& shaderFileName)
		: effect(shaderCompiledText, shaderCompiledTextLength),
		effectTechnique(&effect, "Default"),
		effectPass(&effectTechnique, "Default"),
		world(world),
		varIndexViewInvMatrix(-1),
		varIndexSceneRenderLOD(0),
		varIndexSceneBuffer(0),
		blendMode(D3D::BM_None),
#ifdef _DEBUG
		shaderFileName(shaderFileName),
#endif
		boundTextureVariableIndices(PIMP_MAX_NUM_BOUNDTEXTUREVARINDICES),
		boundMaterialParameters(PIMP_MAX_NUM_BOUNDMATERIALPARAMETERS)
	{
		RefreshParameters();
	}

	Material::~Material()
	{
	}

	void Material::InitParameters()
	{
		varIndexViewInvMatrix = effect.RegisterVariable("viewInvMatrix", false);
		varIndexSceneRenderLOD = effect.RegisterVariable("sceneRenderLOD", false);
		varIndexSceneBuffer = effect.RegisterVariable("sceneBuffer", false);

		if (varIndexSceneRenderLOD >= 0)
			effect.SetVariableValue(varIndexSceneRenderLOD, Scene::GetSceneRenderLOD());
	}


	void Material::Bind(Camera* camera)
	{
		if (camera != NULL)
		{
			const Matrix4* viewInvMatrix = camera->GetViewInvMatrixPtr();

			if (varIndexViewInvMatrix >= 0)
				effect.SetVariableValue(varIndexViewInvMatrix, *viewInvMatrix);
		}


		for (int i=0; i<boundMaterialParameters.Size(); ++i)
			boundMaterialParameters[i].parameter->AssignValueToEffectVariable(&effect, boundMaterialParameters[i].varIndex);

#ifdef _DEBUG
		if (boundBalls.balls != NULL)
			boundBalls.balls->AssignValueToEffectVariables(&effect, boundBalls);
#endif

		effectPass.Apply();
	}


	void Material::RefreshParameters()
	{
		for (int i=0; i<boundTextureVariableIndices.Size(); ++i)
			effect.SetVariableValue(boundTextureVariableIndices[i], (ID3D10ShaderResourceView*)NULL);

		effect.ResetRegisteredVariables();
		boundTextureVariableIndices.Clear();
		boundMaterialParameters.Clear();

#ifdef _DEBUG
		boundBalls.balls = NULL;
#endif

		// Our own static parameters for this material
		InitParameters();

		const FixedSizeList<Texture*>& allTextures = world->GetTextures();

		// All of our world's material parameters and balls
		FixedSizeList<Element*>& elements = world->GetElements();

		for (int i=0; i<elements.Size(); ++i)
		{
			if (elements[i]->GetType() == ET_MaterialParameter)
			{
				MaterialParameter* matParam = (MaterialParameter*)elements[i];

				if (effect.HasVariable(matParam->GetName()))
				{
					BoundMaterialParameter bp;
					bp.varIndex = effect.RegisterVariable(matParam->GetName(), true);
					bp.parameter = matParam;
					boundMaterialParameters.Add(bp);
				}
			}
#ifdef _DEBUG
			else if (elements[i]->GetType() == ET_Balls)
			{
				Balls* balls = (Balls*)elements[i];

				char s[256];
				strcpy(s, balls->GetName());
				strcat(s, "OffsetsAndRadii");

				if (effect.HasVariable(s))
				{
					boundBalls.balls = balls;
					boundBalls.varIndexBallOffsetsAndRadii = effect.RegisterVariable(s, true);
				}
			}
#endif
		}

		// All of our world's textures
		for (int i=0; i<allTextures.Size(); ++i)
		{
			Texture* tex = allTextures[i];
			std::string texBindingName = std::string("texture_") + tex->GetName();

			if (effect.HasVariable(texBindingName.c_str()))
			{
				int varIndex = effect.RegisterVariable(texBindingName.c_str(), true);
				
				effect.SetVariableValue(varIndex, tex->GetShaderResourceView());
				boundTextureVariableIndices.Add(varIndex);
			}
		}
	}


	void Material::SetSceneBuffer(ID3D10ShaderResourceView* resourceView)
	{
		if (varIndexSceneBuffer >= 0)
			effect.SetVariableValue(varIndexSceneBuffer, resourceView);
	}
}
