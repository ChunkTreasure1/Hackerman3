#pragma once

#include "Volt/Core/Base.h"
#include "Volt/Rendering/Shader/Shader.h"

#include <string>

namespace Volt
{
	class Texture2D;
	class ConstantBuffer;
	class SubMaterial
	{
	public:
		SubMaterial() = default;
		SubMaterial(const std::string& aName, uint32_t aIndex, Ref<Shader> aShader);
		~SubMaterial();

		void Bind(bool aBindShader = true);
		void SetTexture(uint32_t binding, Ref<Texture2D> texture);
		void Invalidate();
		void UpdateBuffer();

		inline const size_t GetShaderHash() const { return myShader->GetHash(); }
		inline const Ref<Shader> GetShader() const { return myShader; }
		void SetShader(Ref<Shader> aShader);

		inline const std::string& GetName() const { return myName; }
		inline const Shader::ShaderResources& GetResources() const { return myShaderResources; }
		inline const std::map<uint32_t, Ref<Texture2D>>& GetTextures() const { return myTextures; }

		static Ref<SubMaterial> Create(const std::string& aName, uint32_t aIndex, Ref<Shader> aShader);
		static Ref<SubMaterial> Create();

	private:
		friend class MaterialImporter;

		void SetupMaterialFromShader();

		std::map<uint32_t, Ref<Texture2D>> myTextures;

		Ref<Shader> myShader;
		Ref<ConstantBuffer> myMaterialBuffer;

		Shader::ShaderResources myShaderResources;
		uint32_t myIndex = 0;
		std::string myName;
	};
}