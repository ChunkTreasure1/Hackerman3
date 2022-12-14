#include "vtpch.h"
#include "SubMaterial.h"

#include "Volt/Log/Log.h"

#include "Volt/Rendering/Shader/Shader.h"
#include "Volt/Rendering/Renderer.h"
#include "Volt/Rendering/Texture/Texture2D.h"

#include "Volt/Rendering/Buffer/ConstantBuffer.h"

namespace Volt
{
	SubMaterial::SubMaterial(const std::string& aName, uint32_t aIndex, Ref<Shader> aShader)
		: myName(aName), myIndex(aIndex), myShader(aShader)
	{
		myShader->AddReference(this);
		SetupMaterialFromShader();
	}

	SubMaterial::~SubMaterial()
	{
		if (myShader)
		{
			myShader->RemoveReference(this);
		}
	}

	void SubMaterial::Bind(bool aBindShader)
	{
		if (aBindShader)
		{
			myShader->Bind();

			if (myMaterialBuffer)
			{
				myMaterialBuffer->Bind(MATERIAL_BUFFER_BINDING);
			}
		}


		for (const auto& [binding, texture] : myTextures)
		{
			texture->Bind(binding);
		}
	}

	void SubMaterial::UpdateBuffer()
	{
		if (myMaterialBuffer)
		{
			myMaterialBuffer->SetData(myShaderResources.materialBuffer.data.data(), myShaderResources.materialBuffer.size);
		}
	}

	void SubMaterial::SetTexture(uint32_t binding, Ref<Texture2D> texture)
	{
		if (auto it = myTextures.find(binding); it == myTextures.end())
		{
			VT_CORE_ERROR("No texture exists on slot {0} in material {1}!", binding, myName.c_str());
			return;
		}

		myTextures[binding] = texture;
	}

	void SubMaterial::SetShader(Ref<Shader> aShader)
	{
		myShader = aShader;
		Invalidate();
	}

	void SubMaterial::Invalidate()
	{
		const auto originalTextures = myTextures;
		const auto originalBuffer = myShaderResources.materialBuffer;

		myTextures.clear();
		myMaterialBuffer = nullptr;

		SetupMaterialFromShader();

		for (auto& [binding, texture] : myTextures)
		{
			auto it = originalTextures.find(binding);
			if (it != originalTextures.end())
			{
				texture = it->second;
			}
		}

		// Move old data in to new buffer
		if (originalBuffer.exists && myShaderResources.materialBuffer.exists)
		{
			auto& newMaterialBuffer = myShaderResources.materialBuffer;

			for (const auto& [name, param] : originalBuffer.parameters)
			{
				auto it = newMaterialBuffer.parameters.find(name);
				if (it != newMaterialBuffer.parameters.end())
				{
					memcpy_s(&newMaterialBuffer.data[it->second.offset], BufferElement::GetSizeFromType(it->second.type), &originalBuffer.data[param.offset], BufferElement::GetSizeFromType(it->second.type));
				}
			}

			myMaterialBuffer->SetData(newMaterialBuffer.data.data(), newMaterialBuffer.size);
		}
	}

	Ref<SubMaterial> SubMaterial::Create(const std::string& aName, uint32_t aIndex, Ref<Shader> aShader)
	{
		return CreateRef<SubMaterial>(aName, aIndex, aShader);
	}

	Ref<SubMaterial> SubMaterial::Create()
	{
		return CreateRef<SubMaterial>();
	}

	void SubMaterial::SetupMaterialFromShader()
	{
		myShaderResources = myShader->GetResources();
		for (const auto& [binding, info] : myShaderResources.textures)
		{
			switch (info.dimension)
			{
				case ImageDimension::Dim2D:
				{
					if (myTextures.find(binding) == myTextures.end())
					{
						if (binding == 1)
						{
							myTextures.emplace(binding, Renderer::GetDefaultData().emptyNormal);
						}
						else
						{
							myTextures.emplace(binding, Renderer::GetDefaultData().whiteTexture);
						}

					}
					break;
				}

				default:
					VT_CORE_ERROR("Image dimension not implemented!");
			}
		}

		if (myShaderResources.materialBuffer.exists)
		{
			myMaterialBuffer = ConstantBuffer::Create(nullptr, myShaderResources.materialBuffer.size, ShaderStage::Pixel); // TODO: Maybe we want to use this data in other stages?
			myMaterialBuffer->SetData(myShaderResources.materialBuffer.data.data(), myShaderResources.materialBuffer.size);
		}
	}
}