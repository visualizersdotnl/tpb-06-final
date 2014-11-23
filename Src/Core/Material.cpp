
#include "Platform.h"
#include "D3D.h"
#include "Material.h"
#include "World.h"

namespace Pimp 
{
	Material::Material(
		World* world
,		const unsigned char* shaderCompiledText, int shaderCompiledTextLength
,		const std::string& shaderFileName)
		: effect(shaderCompiledText, shaderCompiledTextLength)
,		effectTechnique(&effect, "Default")
,		effectPass(&effectTechnique, "Default")
,		world(world)
,		varIndexViewInvMatrix(-1)
,		varIndexSceneRenderLOD(0)
,		varIndexSceneBuffer(0)
,		blendMode(D3D::BM_None)
#ifdef _DEBUG
,		shaderFileName(shaderFileName)
#endif
	{
		RefreshParameters();
	}

	Material::~Material() {}

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
		if (nullptr != camera)
		{
			const Matrix4* viewInvMatrix = camera->GetViewInvMatrixPtr();

			if (varIndexViewInvMatrix >= 0)
				effect.SetVariableValue(varIndexViewInvMatrix, *viewInvMatrix);
		}

		for (auto &iParam : boundMaterialParameters)
			iParam.parameter->AssignValueToEffectVariable(&effect, iParam.varIndex);

		effectPass.Apply();
	}

	void Material::RefreshParameters()
	{
		for (auto varIndex : boundTextureVariableIndices)
			effect.SetVariableValue(varIndex, nullptr);

		effect.ResetRegisteredVariables();
		boundTextureVariableIndices.clear();
		boundMaterialParameters.clear();

		// Our own static parameters for this material
		InitParameters();

		// All of our world's material parameters
		const std::vector<Element*>& elements = world->GetElements();
		for (auto *element : elements)
		{
			if (ET_MaterialParameter == element->GetType())
			{
				MaterialParameter* matParam = static_cast<MaterialParameter*>(element);
				if (true == effect.HasVariable(matParam->GetName()))
				{
					BoundMaterialParameter boundMatParam;
					boundMatParam.varIndex = effect.RegisterVariable(matParam->GetName(), true);
					boundMatParam.parameter = matParam;
					boundMaterialParameters.push_back(std::move(boundMatParam));
				}
			}
		}

		// All of our world's textures
		const std::vector<Texture*>& textures = world->GetTextures();
		for (auto *texture : textures)
		{
			const std::string bindName = "texture_" + texture->GetName();
			if (true == effect.HasVariable(bindName.c_str()))
			{
				const int varIndex = effect.RegisterVariable(bindName.c_str(), true);
				effect.SetVariableValue(varIndex, texture->GetShaderResourceView());
				boundTextureVariableIndices.push_back(varIndex);
			}
		}
	}

	void Material::SetSceneBuffer(ID3D10ShaderResourceView* resourceView)
	{
		if (varIndexSceneBuffer >= 0)
			effect.SetVariableValue(varIndexSceneBuffer, resourceView);
	}
}
