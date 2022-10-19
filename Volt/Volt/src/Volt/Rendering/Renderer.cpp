#include "vtpch.h"
#include "Renderer.h"

#include "Volt/Asset/Mesh/Mesh.h"
#include "Volt/Asset/Mesh/Material.h"
#include "Volt/Asset/Mesh/SubMaterial.h"
#include "Volt/Asset/Text/Font.h"
#include "Volt/Asset/Text/MSDFData.h"
#include "Volt/Asset/AssetManager.h"

#include "Volt/Core/Profiling.h"

#include "Volt/Rendering/RendererStructs.h"

#include "Volt/Rendering/Shader/Shader.h"
#include "Volt/Rendering/Shader/ShaderRegistry.h"

#include "Volt/Rendering/Buffer/ConstantBuffer.h"
#include "Volt/Rendering/Buffer/StructuredBuffer.h"
#include "Volt/Rendering/Buffer/IndexBuffer.h"
#include "Volt/Rendering/Buffer/VertexBuffer.h"
#include "Volt/Rendering/Buffer/ConstantBufferRegistry.h"
#include "Volt/Rendering/Buffer/StructuredBufferRegistry.h"

#include "Volt/Rendering/Framebuffer.h"
#include "Volt/Rendering/Camera/Camera.h"
#include "Volt/Rendering/Texture/Texture2D.h"
#include "Volt/Rendering/SamplerState.h"
#include "Volt/Rendering/ComputePipeline.h"

#include <codecvt>

namespace Volt
{
	void Renderer::Initialize()
	{
		myRendererData = CreateScope<RendererData>();
		CreateDepthStates();
		CreateDefaultData();
		CreateSpriteData();
		CreateLineData();
		CreateBillboardData();
		CreateTextData();
		CreateProfilingData();
		CreateInstancingData();

		GenerateBRDFLut();
	}

	void Renderer::InitializeBuffers()
	{
		CreateDefaultBuffers();
	}

	void Renderer::Shutdown()
	{
		myDepthStates.clear();
		myDefaultData = nullptr;
		myRendererData = nullptr;
	}

	void Renderer::Submit(Ref<Mesh> aMesh, const gem::mat4& aTransform, uint32_t aId, float aTimeSinceCreation)
	{
		VT_PROFILE_FUNCTION();

		for (const auto& subMesh : aMesh->GetSubMeshes())
		{
			auto& cmd = myRendererData->renderCommands.emplace_back();
			cmd.mesh = aMesh;
			cmd.material = aMesh->GetMaterial()->GetSubMaterials().at(subMesh.materialIndex);
			cmd.subMesh = subMesh;
			cmd.transform = aTransform;
			cmd.id = aId;
			cmd.timeSinceCreation = aTimeSinceCreation;
		}
	}

	void Renderer::Submit(Ref<Mesh> aMesh, uint32_t subMeshIndex, const gem::mat4& aTransform, uint32_t aId, float aTimeSinceCreation)
	{
		VT_PROFILE_FUNCTION();

		if (subMeshIndex >= (uint32_t)aMesh->GetSubMeshes().size())
		{
			VT_CORE_WARN("Trying to submit submesh with invalid index!");
			return;
		}

		const auto& subMesh = aMesh->GetSubMeshes().at(subMeshIndex);

		auto& cmd = myRendererData->renderCommands.emplace_back();
		cmd.mesh = aMesh;
		cmd.material = aMesh->GetMaterial()->GetSubMaterials().at(subMesh.materialIndex);
		cmd.subMesh = subMesh;
		cmd.transform = aTransform;
		cmd.id = aId;
		cmd.timeSinceCreation = aTimeSinceCreation;
	}

	void Renderer::Submit(Ref<Mesh> aMesh, Ref<SubMaterial> aMaterial, const gem::mat4& aTransform, uint32_t aId /* = 0 */, float aTimeSinceCreation)
	{
		VT_PROFILE_FUNCTION();
		for (const auto& subMesh : aMesh->GetSubMeshes())
		{
			auto& cmd = myRendererData->renderCommands.emplace_back();
			cmd.mesh = aMesh;
			cmd.material = aMaterial;
			cmd.subMesh = subMesh;
			cmd.transform = aTransform;
			cmd.id = aId;
			cmd.timeSinceCreation = aTimeSinceCreation;
		}
	}

	void Renderer::Submit(Ref<Mesh> aMesh, Ref<Material> aMaterial, const gem::mat4& aTransform, uint32_t aId /* = 0 */, float aTimeSinceCreation)
	{
		VT_PROFILE_FUNCTION();
		for (const auto& subMesh : aMesh->GetSubMeshes())
		{
			auto it = aMaterial->GetSubMaterials().find(subMesh.materialIndex);

			auto& cmd = myRendererData->renderCommands.emplace_back();
			cmd.mesh = aMesh;
			cmd.material = it != aMaterial->GetSubMaterials().end() ? it->second : aMaterial->GetSubMaterials().at(0);
			cmd.subMesh = subMesh;
			cmd.transform = aTransform;
			cmd.id = aId;
			cmd.timeSinceCreation = aTimeSinceCreation;
		}
	}

	void Renderer::Submit(Ref<Mesh> aMesh, const gem::mat4& aTransform, const std::vector<gem::mat4>& aBoneTransforms, uint32_t aId, float aTimeSinceCreation)
	{
		VT_PROFILE_FUNCTION();

		for (const auto& subMesh : aMesh->GetSubMeshes())
		{
			auto& cmd = myRendererData->renderCommands.emplace_back();
			cmd.mesh = aMesh;
			cmd.material = aMesh->GetMaterial()->GetSubMaterials().at(subMesh.materialIndex);
			cmd.subMesh = subMesh;
			cmd.transform = aTransform;
			cmd.boneTransforms = aBoneTransforms;
			cmd.id = aId;
			cmd.timeSinceCreation = aTimeSinceCreation;
		}
	}

	void Renderer::SubmitSprite(const gem::mat4& aTransform, const gem::vec4& aColor, uint32_t id /* = 0 */)
	{
		SubmitSprite(nullptr, aTransform, id, aColor);
	}

	void Renderer::SubmitSprite(const SubTexture2D& aTexture, const gem::mat4& aTransform, uint32_t aId, const gem::vec4& aColor)
	{
		VT_PROFILE_FUNCTION();
		auto& spriteData = myRendererData->spriteData;

		uint32_t texIndex = 0;
		if (aTexture.GetTexture())
		{
			for (uint32_t slot = 1; slot < spriteData.textureSlotIndex; slot++)
			{
				if (spriteData.textureSlots[slot] == aTexture.GetTexture())
				{
					texIndex = slot;
					break;
				}
			}

			if (texIndex == 0)
			{
				texIndex = spriteData.textureSlotIndex;
				spriteData.textureSlots[texIndex] = aTexture.GetTexture();
				spriteData.textureSlotIndex++;
			}
		}

		for (uint32_t i = 0; i < 4; i++)
		{
			spriteData.vertexBufferPtr->position = aTransform * spriteData.vertices[i];
			spriteData.vertexBufferPtr->color = aColor;
			spriteData.vertexBufferPtr->texCoords = aTexture.GetTexCoords()[i];
			spriteData.vertexBufferPtr->textureIndex = texIndex;
			spriteData.vertexBufferPtr->id = aId;
			spriteData.vertexBufferPtr++;
		}

		spriteData.indexCount += 6;
	}

	void Renderer::SubmitSprite(Ref<Texture2D> aTexture, const gem::mat4& aTransform, uint32_t id, const gem::vec4& aColor)
	{
		VT_PROFILE_FUNCTION();
		auto& spriteData = myRendererData->spriteData;

		uint32_t texIndex = 0;
		if (aTexture)
		{
			for (uint32_t slot = 1; slot < spriteData.textureSlotIndex; slot++)
			{
				if (spriteData.textureSlots[slot] == aTexture)
				{
					texIndex = slot;
					break;
				}
			}

			if (texIndex == 0)
			{
				texIndex = spriteData.textureSlotIndex;
				spriteData.textureSlots[texIndex] = aTexture;
				spriteData.textureSlotIndex++;
			}
		}

		for (uint32_t i = 0; i < 4; i++)
		{
			spriteData.vertexBufferPtr->position = aTransform * spriteData.vertices[i];
			spriteData.vertexBufferPtr->color = aColor;
			spriteData.vertexBufferPtr->texCoords = spriteData.texCoords[i];
			spriteData.vertexBufferPtr->textureIndex = texIndex;
			spriteData.vertexBufferPtr->id = id;
			spriteData.vertexBufferPtr++;
		}

		spriteData.indexCount += 6;
	}

	void Renderer::SubmitBillboard(const gem::vec3& aPosition, const gem::vec3& aScale, const gem::vec4& aColor, uint32_t aId)
	{
		SubmitBillboard(nullptr, aPosition, aScale, aId, aColor);
	}

	void Renderer::SubmitBillboard(Ref<Texture2D> aTexture, const gem::vec3& aPosition, const gem::vec3& aScale, uint32_t aId, const gem::vec4& aColor)
	{
		VT_PROFILE_FUNCTION();
		auto& billboardData = myRendererData->billboardData;

		uint32_t texIndex = 0;
		if (aTexture)
		{
			for (uint32_t slot = 1; slot < billboardData.textureSlotIndex; slot++)
			{
				if (billboardData.textureSlots[slot] == aTexture)
				{
					texIndex = slot;
					break;
				}
			}

			if (texIndex == 0)
			{
				texIndex = billboardData.textureSlotIndex;
				billboardData.textureSlots[texIndex] = aTexture;
				billboardData.textureSlotIndex++;
			}
		}

		billboardData.vertexBufferPtr->postition = { aPosition.x, aPosition.y, aPosition.z, 1.f };
		billboardData.vertexBufferPtr->id = aId;
		billboardData.vertexBufferPtr->color = aColor;
		billboardData.vertexBufferPtr->textureIndex = texIndex;
		billboardData.vertexBufferPtr->scale = aScale;
		billboardData.vertexBufferPtr++;
		billboardData.vertexCount++;
	}

	void Renderer::SubmitLine(const gem::vec3& aStart, const gem::vec3& aEnd, const gem::vec4& aColor /* = */)
	{
		VT_PROFILE_FUNCTION();
		auto& lineData = myRendererData->lineData;

		lineData.vertexBufferPtr->position = { aStart.x, aStart.y, aStart.z, 1.f };
		lineData.vertexBufferPtr->color = aColor;
		lineData.vertexBufferPtr++;

		lineData.vertexBufferPtr->position = { aEnd.x, aEnd.y, aEnd.z, 1.f };
		lineData.vertexBufferPtr->color = aColor;
		lineData.vertexBufferPtr++;

		lineData.indexCount += 2;
	}

	void Renderer::SubmitLight(const PointLight& pointLight)
	{
		VT_PROFILE_FUNCTION();
		myRendererData->pointLights.push_back(pointLight);
	}

	void Renderer::SubmitLight(const DirectionalLight& dirLight)
	{
		VT_PROFILE_FUNCTION();
		myRendererData->directionalLight = dirLight;
	}

#pragma warning(disable : 4996)

	// From https://stackoverflow.com/questions/31302506/stdu32string-conversion-to-from-stdstring-and-stdu16string
	static std::u32string To_UTF32(const std::string& s)
	{
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
		return conv.from_bytes(s);
	}

#pragma warning(default : 4996)

	static bool NextLine(int32_t aIndex, const std::vector<int32_t>& aLines)
	{
		for (int32_t line : aLines)
		{
			if (line == aIndex)
			{
				return true;
			}
		}

		return false;
	}

	void Renderer::SubmitString(const std::string& aString, const Ref<Font> aFont, const gem::mat4& aTransform, float aMaxWidth, const gem::vec4& aColor)
	{
		if (aString.empty())
		{
			return;
		}

		auto& textData = myRendererData->textData;
		uint32_t textureIndex = 0;

		std::u32string utf32string = To_UTF32(aString);

		Ref<Texture2D> fontAtlas = aFont->GetAtlas();
		VT_CORE_ASSERT(fontAtlas, "Font Atlas was nullptr!");

		for (uint32_t i = 0; i < textData.textureSlotIndex; i++)
		{
			if (textData.textureSlots[i] == fontAtlas)
			{
				textureIndex = i;
				break;
			}
		}

		if (textureIndex == 0)
		{
			textureIndex = textData.textureSlotIndex;
			textData.textureSlots[textureIndex] = fontAtlas;
			textData.textureSlotIndex++;
		}

		auto& fontGeom = aFont->GetMSDFData()->fontGeometry;
		const auto& metrics = fontGeom.getMetrics();

		std::vector<int32_t> nextLines;
		{
			double x = 0.0;
			double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
			double y = -fsScale * metrics.ascenderY;

			int32_t lastSpace = -1;

			for (int32_t i = 0; i < utf32string.size(); i++)
			{
				char32_t character = utf32string[i];
				if (character == '\n')
				{
					x = 0.0;
					y -= fsScale * metrics.lineHeight;
					continue;
				}

				auto glyph = fontGeom.getGlyph(character);
				if (!glyph)
				{
					glyph = fontGeom.getGlyph('?');
				}

				if (!glyph)
				{
					continue;
				}

				if (character != ' ')
				{
					// Calculate geometry
					double pl, pb, pr, pt;
					glyph->getQuadPlaneBounds(pl, pb, pr, pt);
					gem::vec2 quadMin((float)pl, (float)pb);
					gem::vec2 quadMax((float)pl, (float)pb);

					quadMin *= (float)fsScale;
					quadMax *= (float)fsScale;
					quadMin += gem::vec2((float)x, (float)y);
					quadMax += gem::vec2((float)x, (float)y);

					if (quadMax.x > aMaxWidth && lastSpace != -1)
					{
						i = lastSpace;
						nextLines.emplace_back(lastSpace);
						lastSpace = -1;
						x = 0.0;
						y -= fsScale * metrics.lineHeight;
					}
				}
				else
				{
					lastSpace = i;
				}

				double advance = glyph->getAdvance();
				fontGeom.getAdvance(advance, character, utf32string[i + 1]);
				x += fsScale * advance;
			}
		}

		{
			double x = 0.0;
			double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
			double y = 0.0;
			for (int32_t i = 0; i < utf32string.size(); i++)
			{
				char32_t character = utf32string[i];
				if (character == '\n' || NextLine(i, nextLines))
				{
					x = 0.0;
					y -= fsScale * metrics.lineHeight;
					continue;
				}

				auto glyph = fontGeom.getGlyph(character);
				if (!glyph)
				{
					glyph = fontGeom.getGlyph('?');
				}

				if (!glyph)
				{
					continue;
				}

				double l, b, r, t;
				glyph->getQuadAtlasBounds(l, b, r, t);

				double pl, pb, pr, pt;
				glyph->getQuadPlaneBounds(pl, pb, pr, pt);

				pl *= fsScale, pb *= fsScale, pr *= fsScale, pt *= fsScale;
				pl += x, pb += y, pr += x, pt += y;

				double texelWidth = 1.0 / fontAtlas->GetWidth();
				double texelHeight = 1.0 / fontAtlas->GetHeight();

				l *= texelWidth, b *= texelHeight, r *= texelWidth, t *= texelHeight;

				textData.vertexBufferPtr->position = aTransform * gem::vec4{ (float)pl, (float)pb, 0.f, 1.f };
				textData.vertexBufferPtr->color = aColor;
				textData.vertexBufferPtr->texCoords = { (float)l, (float)b };
				textData.vertexBufferPtr->textureIndex = textureIndex;
				textData.vertexBufferPtr++;

				textData.vertexBufferPtr->position = aTransform * gem::vec4{ (float)pr, (float)pb, 0.f, 1.f };
				textData.vertexBufferPtr->color = aColor;
				textData.vertexBufferPtr->texCoords = { (float)r, (float)b };
				textData.vertexBufferPtr->textureIndex = textureIndex;
				textData.vertexBufferPtr++;

				textData.vertexBufferPtr->position = aTransform * gem::vec4{ (float)pr, (float)pt, 0.f, 1.f };
				textData.vertexBufferPtr->color = aColor;
				textData.vertexBufferPtr->texCoords = { (float)r, (float)t };
				textData.vertexBufferPtr->textureIndex = textureIndex;
				textData.vertexBufferPtr++;

				textData.vertexBufferPtr->position = aTransform * gem::vec4{ (float)pl, (float)pt, 0.f, 1.f };
				textData.vertexBufferPtr->color = aColor;
				textData.vertexBufferPtr->texCoords = { (float)l, (float)t };
				textData.vertexBufferPtr->textureIndex = textureIndex;
				textData.vertexBufferPtr++;

				textData.indexCount += 6;

				double advance = glyph->getAdvance();
				fontGeom.getAdvance(advance, character, utf32string[i + 1]);
				x += fsScale * advance;
			}
		}
	}

	void Renderer::SetAmbianceMultiplier(float multiplier)
	{
		myRendererData->settings.ambianceMultiplier = multiplier;
	}

	void Renderer::DrawFullscreenTriangleWithShader(Ref<Shader> aShader)
	{
		auto context = GraphicsContext::GetContext();

		aShader->Bind();

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
		context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

		SetDepthState(DepthState::None);

		context->Draw(3, 0);
		aShader->Unbind();
	}

	void Renderer::DrawFullscreenQuadWithShader(Ref<Shader> aShader)
	{
		auto context = GraphicsContext::GetContext();
		aShader->Bind();

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myRendererData->quadVertexBuffer->Bind();
		myRendererData->quadIndexBuffer->Bind();

		SetDepthState(DepthState::None);

		context->DrawIndexed(6, 0, 0);
		aShader->Unbind();
	}

	void Renderer::DrawMesh(Ref<Mesh> aMesh, const gem::mat4& aTransform)
	{
		for (uint32_t i = 0; const auto & subMesh : aMesh->GetSubMeshes())
		{
			DrawMesh(aMesh, i, aTransform);
			i++;
		}
	}

	void Renderer::DrawMesh(Ref<Mesh> aMesh, uint32_t subMeshIndex, const gem::mat4& aTransform)
	{
		auto& subMesh = aMesh->GetSubMeshes().at(subMeshIndex);

		auto material = aMesh->GetMaterial()->GetSubMaterials().at(subMesh.materialIndex);

		if (auto it = std::find(myRendererData->currentPass.excludedShaderHashes.begin(), myRendererData->currentPass.excludedShaderHashes.end(), material->GetShaderHash()); it != myRendererData->currentPass.excludedShaderHashes.end())
		{
			return;
		}

		if (myRendererData->currentPass.exclusiveShaderHash != 0 && myRendererData->currentPass.exclusiveShaderHash != material->GetShaderHash())
		{
			return;
		}

		if (myRendererData->currentPass.overrideShader)
		{
			myRendererData->currentPass.overrideShader->Bind();
		}

		material->Bind(myRendererData->currentPass.overrideShader == nullptr);

		myRendererData->statistics.materialBinds++;
		myRendererData->statistics.perSectionStatistics.at(myRendererData->currentPass.debugName).materialBinds++;

		aMesh->GetVertexBuffer()->Bind();
		aMesh->GetIndexBuffer()->Bind();
		myRendererData->statistics.vertexIndexBufferBinds++;
		myRendererData->statistics.perSectionStatistics.at(myRendererData->currentPass.debugName).vertexIndexBufferBinds++;

		// Update object buffer
		{
			ObjectData* mapped = ConstantBufferRegistry::Get(1)->Map<ObjectData>();

			mapped->transform = aTransform;
			//mapped->id = cmds[i].id;
			//mapped->isAnimated = !cmds[i].boneTransforms.empty();

			ConstantBufferRegistry::Get(1)->Unmap();
		}

		// Update animation buffer
		//if (!cmds[i].boneTransforms.empty())
		//{
		//	AnimationData* mapped = ConstantBufferRegistry::Get(6)->Map<AnimationData>();
		//	memcpy_s(mapped, sizeof(gem::mat4) * 128, cmds[i].boneTransforms.data(), sizeof(gem::mat4) * cmds[i].boneTransforms.size());
		//	ConstantBufferRegistry::Get(6)->Unmap();
		//}

		ConstantBufferRegistry::Get(1)->Bind(1);
		ConstantBufferRegistry::Get(6)->Bind(6);

		auto context = GraphicsContext::GetContext();

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->DrawIndexed((uint32_t)subMesh.indexCount, subMesh.indexStartOffset, subMesh.vertexStartOffset);
		myRendererData->statistics.drawCalls++;
		myRendererData->statistics.perSectionStatistics.at(myRendererData->currentPass.debugName).drawCalls++;
	}

	void Renderer::Begin()
	{
		VT_PROFILE_FUNCTION();

		GraphicsContext::GetAnnotations()->BeginEvent(L"Rendering Begin");

		UpdatePerFrameBuffers();
		SortRenderCommands();

		//UploadObjectData();

		// Bind samplers
		{
			myRendererData->samplers.linearWrap->Bind(0);
			myRendererData->samplers.linearPointWrap->Bind(1);

			myRendererData->samplers.pointWrap->Bind(2);
			myRendererData->samplers.pointLinearWrap->Bind(3);

			myRendererData->samplers.linearClamp->Bind(4);
			myRendererData->samplers.linearPointClamp->Bind(5);

			myRendererData->samplers.pointClamp->Bind(6);
			myRendererData->samplers.pointLinearClamp->Bind(7);

			myRendererData->samplers.anisotropic->Bind(8);
			myRendererData->samplers.shadow->Bind(9);
		}

		ConstantBufferRegistry::Get(1)->Bind(1);
		ConstantBufferRegistry::Get(4)->Bind(4);
		ConstantBufferRegistry::Get(5)->Bind(5);
		ConstantBufferRegistry::Get(6)->Bind(6);
	}

	void Renderer::End()
	{
		VT_PROFILE_FUNCTION();
		myRendererData->renderCommands.clear();
		myRendererData->pointLights.clear();
		myRendererData->directionalLight = DirectionalLight{};

		GraphicsContext::GetAnnotations()->EndEvent();
	}

	void Renderer::BeginPass(const RenderPass& aRenderPass, Ref<Camera> camera, bool shouldClear)
	{
		const std::string tag = "Begin " + aRenderPass.debugName;
		VT_PROFILE_SCOPE(tag.c_str());

		BeginSection(aRenderPass.debugName);

		myRendererData->currentPass = aRenderPass;
		myRendererData->currentPassCamera = camera;

		if (shouldClear)
		{
			myRendererData->currentPass.framebuffer->Clear();
		}

		myRendererData->currentPass.framebuffer->Bind();

		UpdatePerPassBuffers();
		//CollectRenderCommands();

		ConstantBufferRegistry::Get(0)->Bind(0);
		ConstantBufferRegistry::Get(3)->Bind(3);
	}

	void Renderer::EndPass()
	{
		const std::string tag = "End " + myRendererData->currentPass.debugName;
		VT_PROFILE_SCOPE(tag.c_str());

		if (myRendererData->currentPass.overrideShader)
		{
			myRendererData->currentPass.overrideShader->Unbind();
		}

		myRendererData->currentPass.framebuffer->Unbind();

		EndSection(myRendererData->currentPass.debugName);

		myRendererData->currentPass = {};
		myRendererData->currentPassCamera = nullptr;
		myRendererData->instancingData.passRenderCommands.clear();
	}

	void Renderer::BeginSection(const std::string& name)
	{
		GraphicsContext::GetAnnotations()->BeginEvent(std::filesystem::path(name).wstring().c_str());
		auto& profilingData = myRendererData->profilingData;

#ifdef VT_PROFILE_GPU
		auto& frameTimeQueries = profilingData.sectionTimeQueries.at(profilingData.currentFrame);
		auto& framePipelineQueries = profilingData.sectionPipelineQueries.at(profilingData.currentFrame);

		// Time queries
		{
			auto it = frameTimeQueries.find(name);
			if (it == frameTimeQueries.end())
			{
				// Create time query
				ComPtr<ID3D11Query> query0;
				ComPtr<ID3D11Query> query1;

				D3D11_QUERY_DESC desc{};
				desc.Query = D3D11_QUERY_TIMESTAMP;

				auto device = GraphicsContext::GetDevice();
				device->CreateQuery(&desc, query0.GetAddressOf());
				device->CreateQuery(&desc, query1.GetAddressOf());

				profilingData.sectionTimeQueries.at(0).emplace(name, query0);
				profilingData.sectionTimeQueries.at(1).emplace(name, query1);

				profilingData.sectionNames.at(profilingData.currentFrame).emplace_back(name);
			}
		}

		// Pipeline queries
		{
			auto it = framePipelineQueries.find(name);
			if (it == framePipelineQueries.end())
			{
				// Create time query
				ComPtr<ID3D11Query> query0;
				ComPtr<ID3D11Query> query1;

				D3D11_QUERY_DESC desc{};
				desc.Query = D3D11_QUERY_PIPELINE_STATISTICS;

				auto device = GraphicsContext::GetDevice();
				device->CreateQuery(&desc, query0.GetAddressOf());
				device->CreateQuery(&desc, query1.GetAddressOf());

				profilingData.sectionPipelineQueries.at(0).emplace(name, query0);
				profilingData.sectionPipelineQueries.at(1).emplace(name, query1);
			}

			auto context = GraphicsContext::GetContext();
			context->Begin(profilingData.sectionPipelineQueries.at(profilingData.currentFrame).at(name).Get());
		}
#endif
		profilingData.sectionNames.at(profilingData.currentFrame).emplace_back(name);
		myRendererData->statistics.perSectionStatistics.emplace(name, SectionStatistics{});
	}

	void Renderer::EndSection(const std::string& name)
	{
		auto context = GraphicsContext::GetContext();
		auto& profilingData = myRendererData->profilingData;

#ifdef VT_PROFILE_GPU
		context->End(profilingData.sectionTimeQueries.at(profilingData.currentFrame).at(name).Get());
		context->End(profilingData.sectionPipelineQueries.at(profilingData.currentFrame).at(name).Get());
#endif

		GraphicsContext::GetAnnotations()->EndEvent();
	}

	void Renderer::ResetStatistics()
	{
		myRendererData->statistics.Reset();
	}

	void Renderer::DispatchRenderCommands()
	{
		VT_PROFILE_FUNCTION();

		if (myRendererData->renderCommands.empty())
		{
			return;
		}

		auto context = GraphicsContext::GetContext();
		auto& cmds = myRendererData->renderCommands;

		for (size_t i = 0; i < myRendererData->renderCommands.size(); i++)
		{
			if (auto it = std::find(myRendererData->currentPass.excludedShaderHashes.begin(), myRendererData->currentPass.excludedShaderHashes.end(), cmds[i].material->GetShaderHash()); it != myRendererData->currentPass.excludedShaderHashes.end())
			{
				continue;
			}

			if (myRendererData->currentPass.exclusiveShaderHash != 0 && myRendererData->currentPass.exclusiveShaderHash != cmds[i].material->GetShaderHash())
			{
				continue;
			}

			if (i == 0 || (i > 0 && cmds[i].material != cmds[i - 1].material))
			{
				if (myRendererData->currentPass.overrideShader)
				{
					myRendererData->currentPass.overrideShader->Bind();
				}

				cmds[i].material->Bind(myRendererData->currentPass.overrideShader == nullptr);

				myRendererData->statistics.materialBinds++;
				myRendererData->statistics.perSectionStatistics.at(myRendererData->currentPass.debugName).materialBinds++;
			}

			if (i == 0 || (i > 0 && cmds[i].mesh != cmds[i - 1].mesh))
			{
				cmds[i].mesh->GetVertexBuffer()->Bind();
				cmds[i].mesh->GetIndexBuffer()->Bind();
				myRendererData->statistics.vertexIndexBufferBinds++;
				myRendererData->statistics.perSectionStatistics.at(myRendererData->currentPass.debugName).vertexIndexBufferBinds++;
			}

			// Update object buffer
			{
				ObjectData* mapped = ConstantBufferRegistry::Get(1)->Map<ObjectData>();

				mapped->transform = cmds[i].transform;
				mapped->id = cmds[i].id;
				mapped->isAnimated = !cmds[i].boneTransforms.empty();
				mapped->timeSinceCreation = cmds[i].timeSinceCreation;

				ConstantBufferRegistry::Get(1)->Unmap();
			}

			// Update animation buffer
			if (!cmds[i].boneTransforms.empty())
			{
				AnimationData* mapped = ConstantBufferRegistry::Get(6)->Map<AnimationData>();
				memcpy_s(mapped, sizeof(gem::mat4) * 128, cmds[i].boneTransforms.data(), sizeof(gem::mat4) * cmds[i].boneTransforms.size());
				ConstantBufferRegistry::Get(6)->Unmap();
			}

			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->DrawIndexed((uint32_t)cmds[i].subMesh.indexCount, cmds[i].subMesh.indexStartOffset, cmds[i].subMesh.vertexStartOffset);
			myRendererData->statistics.drawCalls++;
			myRendererData->statistics.perSectionStatistics.at(myRendererData->currentPass.debugName).drawCalls++;
		}
	}

	void Renderer::DispatchRenderCommandsInstanced()
	{
		VT_PROFILE_FUNCTION();

		// TODO: Add statistics stuff

		auto context = GraphicsContext::GetContext();
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (const auto& cmd : myRendererData->instancingData.passRenderCommands)
		{
			// Update instanced vertex buffer
			{
				myRendererData->instancingData.instancedVertexBuffer->SetData(cmd.objectDataIds.data(), sizeof(InstanceData) * (uint32_t)cmd.objectDataIds.size());
				myRendererData->instancingData.instancedVertexBuffer->Bind(1);
			}

			// Bind mesh data
			{
				cmd.mesh->GetVertexBuffer()->Bind(0);
				cmd.mesh->GetIndexBuffer()->Bind();
			
				if (myRendererData->currentPass.overrideShader)
				{
					myRendererData->currentPass.overrideShader->Bind();
				}

				cmd.material->Bind(myRendererData->currentPass.overrideShader == nullptr);
			}
			StructuredBufferRegistry::Get(100)->Bind(100);
			context->DrawIndexedInstanced(cmd.subMesh.indexCount, cmd.count, cmd.subMesh.indexStartOffset, cmd.subMesh.vertexStartOffset, 0);
		}
	}

	void Renderer::SetSceneData(const SceneData& aSceneData)
	{
		myRendererData->sceneData = aSceneData;
	}

	void Renderer::SetDepthState(DepthState aDepthState)
	{
		auto context = GraphicsContext::GetContext();
		context->OMSetDepthStencilState(myDepthStates[(uint32_t)aDepthState].Get(), 0);
	}

	SceneEnvironment Renderer::GenerateEnvironmentMap(AssetHandle aTextureHandle)
	{
		if (myRendererData->environmentCache.find(aTextureHandle) != myRendererData->environmentCache.end())
		{
			return myRendererData->environmentCache.at(aTextureHandle);
		}

		Ref<Texture2D> equirectangularTexture = AssetManager::GetAsset<Texture2D>(aTextureHandle);
		if (!equirectangularTexture || !equirectangularTexture->IsValid())
		{
			return {};
		}

		constexpr uint32_t cubeMapSize = 1024;
		constexpr uint32_t irradianceMapSize = 32;
		constexpr uint32_t conversionThreadCount = 32;

		auto device = GraphicsContext::GetDevice();
		auto context = GraphicsContext::GetContext();

		Ref<Image2D> environmentUnfiltered;
		Ref<Image2D> environmentFiltered;
		Ref<Image2D> irradianceMap;

		// Unfiltered - Conversion
		{
			ImageSpecification imageSpec{};
			imageSpec.format = ImageFormat::RGBA32F;
			imageSpec.width = cubeMapSize;
			imageSpec.height = cubeMapSize;
			imageSpec.usage = ImageUsage::Storage;
			imageSpec.layers = 6;
			imageSpec.isCubeMap = true;

			environmentUnfiltered = Image2D::Create(imageSpec);
			Ref<ComputePipeline> conversionPipeline = ComputePipeline::Create(ShaderRegistry::Get("EquirectangularToCubemap"));

			conversionPipeline->SetTexture(equirectangularTexture, 0);
			conversionPipeline->SetTarget(environmentUnfiltered, 0);
			conversionPipeline->Execute(cubeMapSize / conversionThreadCount, cubeMapSize / conversionThreadCount, 6);
			conversionPipeline->Clear();
		}

		// Filtered
		{
			struct IrradianceRougness
			{
				float roughness;
				float padding[3];
			} irradianceRoughness{};

			ImageSpecification imageSpec{};
			imageSpec.format = ImageFormat::RGBA32F;
			imageSpec.width = cubeMapSize;
			imageSpec.height = cubeMapSize;
			imageSpec.usage = ImageUsage::Storage;
			imageSpec.layers = 6;
			imageSpec.isCubeMap = true;
			imageSpec.mips = Utility::CalculateMipCount(cubeMapSize, cubeMapSize);

			environmentFiltered = Image2D::Create(imageSpec);
			environmentFiltered->CreateMipUAVs();

			Ref<ComputePipeline> filterPipeline = ComputePipeline::Create(ShaderRegistry::Get("EnvironmentMipFilter"));
			Ref<ConstantBuffer> constantBuffer = ConstantBuffer::Create(&irradianceRoughness, sizeof(irradianceRoughness), ShaderStage::Compute);
			constantBuffer->Bind(0);


			const float deltaRoughness = 1.f / gem::max((float)environmentFiltered->GetMipCount() - 1.f, 1.f);
			for (uint32_t i = 0, size = cubeMapSize; i < environmentFiltered->GetMipCount(); i++, size /= 2)
			{
				const uint32_t numGroups = gem::max(1u, size / 32);
				const float roughness = gem::max(i * deltaRoughness, 0.05f);

				irradianceRoughness.roughness = roughness;
				constantBuffer->SetData(&irradianceRoughness, sizeof(IrradianceRougness));

				filterPipeline->SetImage(environmentUnfiltered, 0);
				auto mipUAV = environmentFiltered->GetUAV(i);
				context->CSSetUnorderedAccessViews(0, 1, mipUAV.GetAddressOf(), nullptr);
				filterPipeline->Execute(numGroups, numGroups, 6);
				filterPipeline->Clear();
			}

			ID3D11UnorderedAccessView* nullUAV = nullptr;
			context->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
		}

		// Irradiance
		{
			ImageSpecification imageSpec{};
			imageSpec.format = ImageFormat::RGBA32F;
			imageSpec.width = irradianceMapSize;
			imageSpec.height = irradianceMapSize;
			imageSpec.usage = ImageUsage::Storage;
			imageSpec.layers = 6;
			imageSpec.isCubeMap = true;
			imageSpec.mips = 1;

			irradianceMap = Image2D::Create(imageSpec);
			Ref<ComputePipeline> irradiancePipeline = ComputePipeline::Create(ShaderRegistry::Get("EnvironmentIrradiance"));

			irradiancePipeline->SetImage(environmentFiltered, 0);
			irradiancePipeline->SetTarget(irradianceMap, 0);
			irradiancePipeline->Execute(irradianceMapSize / conversionThreadCount, irradianceMapSize / conversionThreadCount, 6);
			irradiancePipeline->Clear();
		}

		SceneEnvironment env{ irradianceMap, environmentFiltered };
		myRendererData->environmentCache.emplace(aTextureHandle, env);

		return env;
	}

	void Renderer::CreateDepthStates()
	{
		auto device = GraphicsContext::GetDevice();

		{
			D3D11_DEPTH_STENCIL_DESC depthDesc = {};
			depthDesc.DepthEnable = true;
			depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthDesc.DepthFunc = D3D11_COMPARISON_LESS;

			device->CreateDepthStencilState(&depthDesc, myDepthStates.emplace_back().GetAddressOf());
		}

		{
			D3D11_DEPTH_STENCIL_DESC depthDesc = {};
			depthDesc.DepthEnable = true;
			depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depthDesc.DepthFunc = D3D11_COMPARISON_LESS;

			device->CreateDepthStencilState(&depthDesc, myDepthStates.emplace_back().GetAddressOf());
		}

		{
			D3D11_DEPTH_STENCIL_DESC depthDesc = {};
			depthDesc.DepthEnable = false;
			depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depthDesc.DepthFunc = D3D11_COMPARISON_LESS;

			device->CreateDepthStencilState(&depthDesc, myDepthStates.emplace_back().GetAddressOf());
		}
	}

	void Renderer::CreateDefaultBuffers()
	{
		// Constant buffers
		{
			ConstantBufferRegistry::Register(0, ConstantBuffer::Create(nullptr, sizeof(CameraData), ShaderStage::Vertex));
			ConstantBufferRegistry::Register(1, ConstantBuffer::Create(nullptr, sizeof(ObjectData), ShaderStage::Vertex));
			ConstantBufferRegistry::Register(3, ConstantBuffer::Create(nullptr, sizeof(PassData), ShaderStage::Vertex));
			ConstantBufferRegistry::Register(4, ConstantBuffer::Create(nullptr, sizeof(DirectionalLight), ShaderStage::Pixel));
			ConstantBufferRegistry::Register(5, ConstantBuffer::Create(nullptr, sizeof(PointLightData), ShaderStage::Pixel));
			ConstantBufferRegistry::Register(6, ConstantBuffer::Create(nullptr, sizeof(AnimationData), ShaderStage::Pixel));
			ConstantBufferRegistry::Register(7, ConstantBuffer::Create(nullptr, sizeof(SceneData), ShaderStage::Pixel));
		}

		// Instance data
		{
			StructuredBufferRegistry::Register(100, StructuredBuffer::Create(sizeof(ObjectData), RendererData::MAX_OBJECTS_PER_FRAME, ShaderStage::Vertex));
		}
	}

	void Renderer::CreateSpriteData()
	{
		auto& spriteData = myRendererData->spriteData;

		// Vertex buffer
		{
			SpriteVertex* tempVertPtr = new SpriteVertex[SpriteData::MAX_VERTICES];
			spriteData.vertexBuffer = VertexBuffer::Create(tempVertPtr, sizeof(SpriteVertex) * SpriteData::MAX_VERTICES, sizeof(SpriteVertex), D3D11_USAGE_DYNAMIC);
			delete[] tempVertPtr;
		}

		spriteData.vertexBufferBase = new SpriteVertex[SpriteData::MAX_INDICES];
		spriteData.vertexBufferPtr = spriteData.vertexBufferBase;

		// Index buffer
		{
			uint32_t* indices = new uint32_t[SpriteData::MAX_INDICES];
			uint32_t offset = 0;

			for (uint32_t indice = 0; indice < SpriteData::MAX_INDICES; indice += 6)
			{
				indices[indice + 0] = offset + 0;
				indices[indice + 1] = offset + 3;
				indices[indice + 2] = offset + 2;

				indices[indice + 3] = offset + 2;
				indices[indice + 4] = offset + 1;
				indices[indice + 5] = offset + 0;

				offset += 4;
			}

			spriteData.indexBuffer = IndexBuffer::Create(indices, SpriteData::MAX_INDICES);
			delete[] indices;
		}

		spriteData.vertices[0] = { -0.5f, -0.5f, 0.f, 1.f };
		spriteData.vertices[1] = { 0.5f, -0.5f, 0.f, 1.f };
		spriteData.vertices[2] = { 0.5f,  0.5f, 0.f, 1.f };
		spriteData.vertices[3] = { -0.5f,  0.5f, 0.f, 1.f };

		spriteData.texCoords[0] = { 0.f, 1.f };
		spriteData.texCoords[1] = { 1.f, 1.f };
		spriteData.texCoords[2] = { 1.f, 0.f };
		spriteData.texCoords[3] = { 0.f, 0.f };

		spriteData.textureSlots[0] = myDefaultData->whiteTexture;
	}

	void Renderer::CreateBillboardData()
	{
		BillboardVertex* tempVertPtr = new BillboardVertex[BillboardData::MAX_BILLBOARDS];
		myRendererData->billboardData.vertexBuffer = VertexBuffer::Create(tempVertPtr, sizeof(BillboardVertex) * BillboardData::MAX_BILLBOARDS, sizeof(BillboardVertex), D3D11_USAGE_DYNAMIC);
		delete[] tempVertPtr;

		myRendererData->billboardData.vertexBufferBase = new BillboardVertex[BillboardData::MAX_BILLBOARDS];
		myRendererData->billboardData.vertexBufferPtr = myRendererData->billboardData.vertexBufferBase;
		myRendererData->billboardData.textureSlots[0] = myDefaultData->whiteTexture;
	}

	void Renderer::CreateLineData()
	{
		auto& lineData = myRendererData->lineData;

		LineVertex* tempVertPtr = new LineVertex[LineData::MAX_VERTICES];
		lineData.vertexBuffer = VertexBuffer::Create(tempVertPtr, sizeof(LineVertex) * LineData::MAX_VERTICES, sizeof(LineVertex), D3D11_USAGE_DYNAMIC);
		delete[] tempVertPtr;

		lineData.vertexBufferBase = new LineVertex[LineData::MAX_VERTICES];
		lineData.vertexBufferPtr = lineData.vertexBufferBase;
		lineData.shader = ShaderRegistry::Get("Line");

		// Indices
		{
			uint32_t* indices = new uint32_t[LineData::MAX_INDICES];
			uint32_t offset = 0;

			for (uint32_t indice = 0; indice < LineData::MAX_INDICES; indice += 2)
			{
				indices[indice + 0] = offset + 0;
				indices[indice + 1] = offset + 1;

				offset += 2;
			}

			lineData.indexBuffer = IndexBuffer::Create(indices, LineData::MAX_INDICES);
			delete[] indices;
		}
	}

	void Renderer::CreateTextData()
	{
		auto& textData = myRendererData->textData;
		textData.shader = ShaderRegistry::Get("Text");

		// Vertex buffer
		{
			TextVertex* tempVertPtr = new TextVertex[TextData::MAX_VERTICES];
			textData.vertexBuffer = VertexBuffer::Create(tempVertPtr, sizeof(TextVertex) * TextData::MAX_VERTICES, sizeof(TextVertex), D3D11_USAGE_DYNAMIC);
			delete[] tempVertPtr;
		}

		textData.vertexBufferBase = new TextVertex[TextData::MAX_INDICES];
		textData.vertexBufferPtr = textData.vertexBufferBase;

		// Index buffer
		{
			uint32_t* indices = new uint32_t[TextData::MAX_INDICES];
			uint32_t offset = 0;

			for (uint32_t indice = 0; indice < TextData::MAX_INDICES; indice += 6)
			{
				indices[indice + 0] = offset + 0;
				indices[indice + 1] = offset + 3;
				indices[indice + 2] = offset + 2;

				indices[indice + 3] = offset + 2;
				indices[indice + 4] = offset + 1;
				indices[indice + 5] = offset + 0;

				offset += 4;
			}

			textData.indexBuffer = IndexBuffer::Create(indices, SpriteData::MAX_INDICES);
			delete[] indices;
		}
	}

	void Renderer::CreateProfilingData()
	{
		auto device = GraphicsContext::GetDevice();

		// Disjoint
		{
			D3D11_QUERY_DESC desc{};
			desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
			VT_DX_CHECK(device->CreateQuery(&desc, myRendererData->profilingData.disjointQuery[0].GetAddressOf()));
			VT_DX_CHECK(device->CreateQuery(&desc, myRendererData->profilingData.disjointQuery[1].GetAddressOf()));
		}

		// Begin
		{
			D3D11_QUERY_DESC desc{};
			desc.Query = D3D11_QUERY_TIMESTAMP;
			VT_DX_CHECK(device->CreateQuery(&desc, myRendererData->profilingData.beginFrameQuery[0].GetAddressOf()));
			VT_DX_CHECK(device->CreateQuery(&desc, myRendererData->profilingData.beginFrameQuery[1].GetAddressOf()));
		}

		// End
		{
			D3D11_QUERY_DESC desc{};
			desc.Query = D3D11_QUERY_TIMESTAMP;
			VT_DX_CHECK(device->CreateQuery(&desc, myRendererData->profilingData.endFrameQuery[0].GetAddressOf()));
			VT_DX_CHECK(device->CreateQuery(&desc, myRendererData->profilingData.endFrameQuery[1].GetAddressOf()));
		}
	}

	void Renderer::CreateInstancingData()
	{
		myRendererData->instancingData.instancedVertexBuffer = VertexBuffer::Create(RendererData::MAX_OBJECTS_PER_FRAME * sizeof(InstanceData), sizeof(InstanceData));
	}

	void Renderer::GenerateBRDFLut()
	{
		constexpr uint32_t BRDFSize = 512;

		ImageSpecification spec{};
		spec.format = ImageFormat::RG16F;
		spec.usage = ImageUsage::Storage;
		spec.width = BRDFSize;
		spec.height = BRDFSize;

		myDefaultData->brdfLut = Image2D::Create(spec);

		Ref<ComputePipeline> brdfPipeline = ComputePipeline::Create(ShaderRegistry::Get("BRDFGeneration"));
		brdfPipeline->SetTarget(myDefaultData->brdfLut, 0);
		brdfPipeline->Execute(BRDFSize / 32, BRDFSize / 32, 1);
		brdfPipeline->Clear();
	}

	void Renderer::CreateDefaultData()
	{
		myDefaultData = CreateScope<DefaultData>();
		myDefaultData->defaultShader = ShaderRegistry::Get("Default");

		// White texture
		{
			constexpr uint32_t whiteTextureData = 0xffffffff;
			myDefaultData->whiteTexture = Texture2D::Create(ImageFormat::RGBA, 1, 1, &whiteTextureData);
			myDefaultData->whiteTexture->handle = Asset::Null();
		}

		// Empty normal
		{
			const gem::vec4 normalData = { 0.f, 0.5f, 1.f, 0.5f };
			myDefaultData->emptyNormal = Texture2D::Create(ImageFormat::RGBA32F, 1, 1, &normalData);
			myDefaultData->emptyNormal->handle = Asset::Null();
		}

		// Black cube
		{
			constexpr uint32_t blackCubeTextureData[6] = { 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000 };

			ImageSpecification imageSpec{};
			imageSpec.format = ImageFormat::RGBA;
			imageSpec.width = 1;
			imageSpec.height = 1;
			imageSpec.usage = ImageUsage::Texture;
			imageSpec.layers = 6;
			imageSpec.isCubeMap = true;

			myDefaultData->blackCubeImage = Image2D::Create(imageSpec, blackCubeTextureData);
		}

		// Samplers
		{
			myRendererData->samplers.linearWrap = SamplerState::Create(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);
			myRendererData->samplers.linearPointWrap = SamplerState::Create(D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);

			myRendererData->samplers.pointWrap = SamplerState::Create(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
			myRendererData->samplers.pointLinearWrap = SamplerState::Create(D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);

			myRendererData->samplers.linearClamp = SamplerState::Create(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
			myRendererData->samplers.linearPointClamp = SamplerState::Create(D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP);

			myRendererData->samplers.pointClamp = SamplerState::Create(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP);
			myRendererData->samplers.pointLinearClamp = SamplerState::Create(D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);

			myRendererData->samplers.anisotropic = SamplerState::Create(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);
			myRendererData->samplers.shadow = SamplerState::Create(D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_LESS);
		}

		// Fullscreen quad
		{
			float x = -1.f, y = -1.f;
			float width = 2.f, height = 2.f;

			struct QuadVertex
			{
				gem::vec3 position;
				gem::vec2 texCoord;
			};

			QuadVertex* data = new QuadVertex[4];

			data[0].position = { x, y, 0.f };
			data[0].texCoord = { 0.f, 1.f };

			data[1].position = { x, y + height, 0.f };
			data[1].texCoord = { 0.f, 0.f };

			data[2].position = { x + width, y, 0.f };
			data[2].texCoord = { 1.f, 1.f };

			data[3].position = { x + width, y + height, 0.f };
			data[3].texCoord = { 1.f, 0.f };

			myRendererData->quadVertexBuffer = VertexBuffer::Create(data, 4 * sizeof(QuadVertex), sizeof(QuadVertex));
			uint32_t indices[6] = { 0, 1, 2, 2, 1, 3 };
			myRendererData->quadIndexBuffer = IndexBuffer::Create(indices, 6);

			delete[] data;
		}
	}

	void Renderer::UpdatePerPassBuffers()
	{
		VT_PROFILE_FUNCTION();

		// Update camera buffer
		{
			CameraData* cameraData = ConstantBufferRegistry::Get(0)->Map<CameraData>();

			cameraData->proj = myRendererData->currentPassCamera->GetProjection();
			cameraData->view = myRendererData->currentPassCamera->GetView();
			cameraData->viewProj = cameraData->proj * cameraData->view;
			cameraData->ambianceMultiplier = myRendererData->settings.ambianceMultiplier;
			cameraData->exposure = myRendererData->settings.exposure;

			const auto pos = myRendererData->currentPassCamera->GetPosition();
			cameraData->position = gem::vec4(pos.x, pos.y, pos.z, 0.f);

			ConstantBufferRegistry::Get(0)->Unmap();
		}

		// Pass data
		{
			Volt::PassData* passData = ConstantBufferRegistry::Get(3)->Map<Volt::PassData>();

			passData->targetSize = { myRendererData->currentPass.framebuffer->GetWidth(), myRendererData->currentPass.framebuffer->GetHeight() };

			ConstantBufferRegistry::Get(3)->Unmap();
		}
	}

	void Renderer::UpdatePerFrameBuffers()
	{
		VT_PROFILE_FUNCTION();

		// Directional light
		{
			DirectionalLight* dirLight = ConstantBufferRegistry::Get(4)->Map<DirectionalLight>();

			*dirLight = myRendererData->directionalLight;

			ConstantBufferRegistry::Get(4)->Unmap();
		}

		// Point lights
		{
			uint32_t size = sizeof(PointLightData);

			PointLightData* data = ConstantBufferRegistry::Get(5)->Map<PointLightData>();

			memcpy_s(data->pointLights, sizeof(PointLight) * MAX_POINT_LIGHTS, myRendererData->pointLights.data(), sizeof(PointLight) * myRendererData->pointLights.size());
			data->count = (uint32_t)myRendererData->pointLights.size();

			ConstantBufferRegistry::Get(5)->Unmap();
		}

		// Scene data
		{
			SceneData* data = ConstantBufferRegistry::Get(7)->Map<SceneData>();
			*data = myRendererData->sceneData;
			ConstantBufferRegistry::Get(7)->Unmap();
		}
	}

	void Renderer::UploadObjectData()
	{
		VT_PROFILE_FUNCTION();

		std::vector<ObjectData> objData;
		objData.reserve(myRendererData->renderCommands.size());

		for (uint32_t i = 0; auto & cmd : myRendererData->renderCommands)
		{
			cmd.objectBufferId = i;

			ObjectData& data = objData.emplace_back();
			data.id = cmd.id;
			data.isAnimated = !cmd.boneTransforms.empty();
			data.timeSinceCreation = cmd.timeSinceCreation;
			data.transform = cmd.transform;

			++i;
		}

		StructuredBufferRegistry::Get(100)->SetData<ObjectData>(objData.data(), (uint32_t)objData.size());
	}

	void Renderer::SortRenderCommands()
	{
		VT_PROFILE_FUNCTION();

		auto& renderCommands = myRendererData->renderCommands;

		std::stable_sort(renderCommands.begin(), renderCommands.end(), [](const RenderCommand& lhs, const RenderCommand& rhs)
			{
				return lhs.mesh < rhs.mesh;
			});

		std::stable_sort(renderCommands.begin(), renderCommands.end(), [](const RenderCommand& lhs, const RenderCommand& rhs)
			{
				return lhs.material < rhs.material;
			});

	}

	void Renderer::CollectRenderCommands()
	{
		VT_PROFILE_FUNCTION();

		auto& allCmds = myRendererData->renderCommands;
		auto& passCmds = myRendererData->instancingData.passRenderCommands;
		const auto& currentPass = myRendererData->currentPass;

		for (size_t i = 0; i < allCmds.size(); ++i)
		{
			if (auto it = std::find(currentPass.excludedShaderHashes.begin(), currentPass.excludedShaderHashes.end(), allCmds[i].material->GetShaderHash()); it != currentPass.excludedShaderHashes.end())
			{
				continue;
			}

			if (currentPass.exclusiveShaderHash != 0 && currentPass.exclusiveShaderHash != allCmds[i].material->GetShaderHash())
			{
				continue;
			}

			if (i == 0 || passCmds.empty() || passCmds.back().subMesh != allCmds.at(i).subMesh)
			{
				auto& instancedCmd = passCmds.emplace_back();
				instancedCmd.subMesh = allCmds.at(i).subMesh;
				instancedCmd.boneTransforms = allCmds.at(i).boneTransforms;
				instancedCmd.mesh = allCmds.at(i).mesh;
				instancedCmd.material = allCmds.at(i).material;
				instancedCmd.objectDataIds.emplace_back(allCmds.at(i).objectBufferId);
				instancedCmd.count = 1;
			}
			else
			{
				passCmds.back().objectDataIds.emplace_back(allCmds.at(i).objectBufferId);
				passCmds.back().count++;
			}
		} 
	}

	void Renderer::DispatchSpritesWithShader(Ref<Shader> aShader)
	{
		VT_PROFILE_FUNCTION();
		auto& spriteData = myRendererData->spriteData;

		if (spriteData.indexCount == 0)
		{
			return;
		}

		uint32_t dataSize = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(spriteData.vertexBufferPtr) - reinterpret_cast<uint8_t*>(spriteData.vertexBufferBase));
		spriteData.vertexBuffer->SetData(spriteData.vertexBufferBase, dataSize);

		std::vector<ID3D11ShaderResourceView*> textures;
		for (uint32_t i = 0; i < spriteData.textureSlotIndex; i++)
		{
			if (!spriteData.textureSlots[i])
			{
				textures.emplace_back(GetDefaultData().whiteTexture->GetImage()->GetSRV().Get());
			}
			else
			{
				textures.emplace_back(spriteData.textureSlots[i]->GetImage()->GetSRV().Get());
			}
		}

		auto context = GraphicsContext::GetContext();
		aShader->Bind();
		context->PSSetShaderResources(0, (uint32_t)textures.size(), textures.data());
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		spriteData.vertexBuffer->Bind();
		spriteData.indexBuffer->Bind();
		context->DrawIndexed(spriteData.indexCount, 0, 0);

		spriteData.indexCount = 0;
		spriteData.vertexBufferPtr = spriteData.vertexBufferBase;
		spriteData.textureSlotIndex = 0;
		aShader->Unbind();
	}

	void Renderer::DispatchBillboardsWithShader(Ref<Shader> aShader)
	{
		VT_PROFILE_FUNCTION();

		auto& billboardData = myRendererData->billboardData;
		if (billboardData.vertexCount == 0)
		{
			return;
		}

		uint32_t dataSize = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(billboardData.vertexBufferPtr) - reinterpret_cast<uint8_t*>(billboardData.vertexBufferBase));
		billboardData.vertexBuffer->SetData(billboardData.vertexBufferBase, dataSize);

		std::vector<ID3D11ShaderResourceView*> textures;
		for (uint32_t i = 0; i < billboardData.textureSlotIndex; i++)
		{
			if (!billboardData.textureSlots[i])
			{
				textures.emplace_back(GetDefaultData().whiteTexture->GetImage()->GetSRV().Get());
			}
			else
			{
				textures.emplace_back(billboardData.textureSlots[i]->GetImage()->GetSRV().Get());
			}
		}

		auto context = GraphicsContext::GetContext();
		aShader->Bind();
		context->PSSetShaderResources(0, (uint32_t)textures.size(), textures.data());
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		billboardData.vertexBuffer->Bind();
		context->Draw(billboardData.vertexCount, 0);

		billboardData.vertexCount = 0;
		billboardData.vertexBufferPtr = billboardData.vertexBufferBase;
		aShader->Unbind();
	}

	void Renderer::ExecuteFullscreenPass(Ref<ComputePipeline> aComputePipeline, Ref<Framebuffer> aFramebuffer)
	{
		VT_PROFILE_FUNCTION();

		aFramebuffer->Clear();

		const uint32_t width = aFramebuffer->GetWidth();
		const uint32_t height = aFramebuffer->GetHeight();

		const uint32_t threadCountXY = 32;

		const uint32_t groupX = width / threadCountXY + 1;
		const uint32_t groupY = height / threadCountXY + 1;

		aComputePipeline->Execute(groupX, groupY, 1);
		aComputePipeline->Clear();
	}

	void Renderer::DispatchLines()
	{
		VT_PROFILE_FUNCTION();

		auto& lineData = myRendererData->lineData;
		if (lineData.indexCount == 0)
		{
			return;
		}

		uint32_t dataSize = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(lineData.vertexBufferPtr) - reinterpret_cast<uint8_t*>(lineData.vertexBufferBase));
		lineData.vertexBuffer->SetData(lineData.vertexBufferBase, dataSize);

		lineData.shader->Bind();
		lineData.vertexBuffer->Bind();
		lineData.indexBuffer->Bind();

		auto context = GraphicsContext::GetContext();
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		context->DrawIndexed(lineData.indexCount, 0, 0);

		lineData.vertexBufferPtr = lineData.vertexBufferBase;
		lineData.indexCount = 0;
		lineData.shader->Unbind();
	}

	void Renderer::DispatchText()
	{
		VT_PROFILE_FUNCTION();

		auto& textData = myRendererData->textData;

		if (textData.indexCount == 0)
		{
			return;
		}

		uint32_t dataSize = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(textData.vertexBufferPtr) - reinterpret_cast<uint8_t*>(textData.vertexBufferBase));
		textData.vertexBuffer->SetData(textData.vertexBufferBase, dataSize);

		std::vector<ID3D11ShaderResourceView*> textures;
		for (uint32_t i = 0; i < textData.textureSlotIndex; i++)
		{
			if (!textData.textureSlots[i])
			{
				textures.emplace_back(GetDefaultData().whiteTexture->GetImage()->GetSRV().Get());
			}
			else
			{
				textures.emplace_back(textData.textureSlots[i]->GetImage()->GetSRV().Get());
			}
		}

		auto context = GraphicsContext::GetContext();
		textData.shader->Bind();
		context->PSSetShaderResources(0, (uint32_t)textures.size(), textures.data());
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		textData.vertexBuffer->Bind();
		textData.indexBuffer->Bind();
		context->DrawIndexed(textData.indexCount, 0, 0);

		textData.indexCount = 0;
		textData.vertexBufferPtr = textData.vertexBufferBase;
		textData.textureSlotIndex = 0;

		textData.shader->Unbind();
	}
}