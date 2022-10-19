#include "vtpch.h"
#include "SceneRenderer.h"

#include "Volt/Asset/AssetManager.h"
#include "Volt/Asset/Animation/Animation.h"
#include "Volt/Asset/Animation/AnimatedCharacter.h"
#include "Volt/Asset/Mesh/Material.h"

#include "Volt/Asset/Text/Font.h"

#include "Volt/Animation/AnimationManager.h"

#include "Volt/Rendering/Renderer.h"
#include "Volt/Rendering/Framebuffer.h"
#include "Volt/Rendering/RendererStructs.h"
#include "Volt/Rendering/ComputePipeline.h"
#include "Volt/Rendering/Shader/ShaderRegistry.h"
#include "Volt/Rendering/Shader/Shader.h"
#include "Volt/Rendering/Camera/Camera.h"
#include "Volt/Rendering/Texture/Texture2D.h"
#include "Volt/Rendering/Buffer/ConstantBuffer.h"

#include "Volt/Core/Profiling.h"
#include "Volt/Scene/Scene.h"

#include "Volt/Components/Components.h"
#include "Volt/Scene/Entity.h"
#include "Volt/Particles/ParticleSystem.h"

#include "Volt/Utility/Math.h"

#include <GEM/gem.h>

#include <ranges>

namespace Volt
{
	SceneRenderer::SceneRenderer(Ref<Scene> aScene)
		: myScene(aScene)
	{
		CreatePasses();
	}

	void SceneRenderer::OnRenderEditor(Ref<Camera> aCamera)
	{
		OnRender(aCamera);
	}

	void SceneRenderer::OnRender(Ref<Camera> aCamera)
	{
		VT_PROFILE_FUNCTION();

		auto& registry = myScene->GetRegistry();

		registry.ForEach<MeshComponent, TransformComponent, EntityDataComponent>([&](Wire::EntityId id, const MeshComponent& meshComp, TransformComponent& transformComp, const EntityDataComponent& dataComp)
			{
				if (meshComp.handle != Asset::Null() && transformComp.visible)
				{
					auto mesh = AssetManager::GetAsset<Mesh>(meshComp.handle);
					if (!mesh || !mesh->IsValid())
					{
						return;
					}

					Ref<Material> material;
					if (meshComp.overrideMaterial != Asset::Null())
					{
						auto overrideMat = AssetManager::GetAsset<Material>(meshComp.overrideMaterial);
						if (overrideMat && overrideMat->IsValid())
						{
							material = overrideMat;
						}
					}

					const gem::mat4 transform = myScene->GetWorldSpaceTransform(Entity(id, myScene.get()));

					// #TODO: Add ability to have override materials on single submesh draws
					if (meshComp.subMeshIndex != -1)
					{
						Renderer::Submit(mesh, (uint32_t)meshComp.subMeshIndex, transform, id, dataComp.timeSinceCreation);
					}
					else if (material)
					{
						if (meshComp.subMaterialIndex != -1)
						{
							Renderer::Submit(mesh, material->GetSubMaterials().at(meshComp.subMaterialIndex), id, dataComp.timeSinceCreation);
						}
						else
						{
							Renderer::Submit(mesh, material, transform, id, dataComp.timeSinceCreation);
						}
					}
					else
					{
						if (meshComp.subMaterialIndex != -1)
						{
							Renderer::Submit(mesh, mesh->GetMaterial()->GetSubMaterials().at(meshComp.subMaterialIndex), id, dataComp.timeSinceCreation);
						}
						else
						{
							Renderer::Submit(mesh, transform, id, dataComp.timeSinceCreation);
						}
					}
				}
			});

		registry.ForEach<PointLightComponent, TransformComponent>([&](Wire::EntityId id, const PointLightComponent& pointLightComp, const TransformComponent& transformComp)
			{
				if (transformComp.visible)
				{
					PointLight light{};
					light.color = pointLightComp.color;
					light.falloff = pointLightComp.falloff;
					light.farPlane = pointLightComp.farPlane;
					light.intensity = pointLightComp.intensity;
					light.radius = pointLightComp.radius;

					auto trs = myScene->GetWorldSpaceTRS(Entity{ id, myScene.get() });

					gem::vec3 p, s, r;
					Math::DecomposeTransform(myScene->GetWorldSpaceTransform(Volt::Entity{ id, myScene.get() }), p, r, s);

					light.position = p;
					Renderer::SubmitLight(light);
				}
			});

		Ref<Camera> dirLightCamera = nullptr; // We probably don't want to heap allocate every frame

		registry.ForEach<DirectionalLightComponent, TransformComponent>([&](Wire::EntityId id, const DirectionalLightComponent& dirLightComp, const TransformComponent& transformComp)
			{
				if (transformComp.visible)
				{
					DirectionalLight light{};
					light.colorIntensity = gem::vec4(dirLightComp.color.x, dirLightComp.color.y, dirLightComp.color.z, dirLightComp.intensity);

					const gem::vec3 dir = gem::normalize(gem::mat3(transformComp.GetTransform()) * gem::vec3(1.f)) * -1.f;
					light.direction = gem::vec4(dir.x, dir.y, dir.z, 1.f);
					light.castShadows = static_cast<uint32_t>(dirLightComp.castShadows);

					if (dirLightComp.castShadows)
					{
						constexpr float size = 4000.f;
						const gem::vec3 camPos = { aCamera->GetPosition().x, 0.f, aCamera->GetPosition().z };

						const gem::mat4 view = gem::lookAtLH((dir * size) + camPos, gem::vec3{ 0.f } + camPos, {0.f, 1.f, 0.f});
						const gem::mat4 projection = gem::ortho(-size, size, -size, size, 1.f, 2000000.f);

						light.viewProjection = projection * view;

						dirLightCamera = CreateRef<Camera>();
						dirLightCamera->SetView(view);
						dirLightCamera->SetProjection(projection);
					}

					Renderer::SubmitLight(light);
				}
			});

		registry.ForEach<AnimatedCharacterComponent, TransformComponent, EntityDataComponent>([&](Wire::EntityId id, const AnimatedCharacterComponent& animCharComp, const TransformComponent& transformComp, const EntityDataComponent& dataComp)
			{
				if (animCharComp.animatedCharacter != Asset::Null() && transformComp.visible)
				{
					auto character = AssetManager::GetAsset<AnimatedCharacter>(animCharComp.animatedCharacter);
					if (character && character->IsValid())
					{
						const gem::mat4 transform = myScene->GetWorldSpaceTransform(Entity(id, myScene.get()));
						Renderer::Submit(character->GetSkin(), transform, character->SampleAnimation(animCharComp.currentAnimation, animCharComp.currentStartTime), id, dataComp.timeSinceCreation);
					}
				}
			});

		SceneEnvironment sceneEnvironment{ Renderer::GetDefaultData().blackCubeImage, Renderer::GetDefaultData().blackCubeImage };

		registry.ForEach<SkylightComponent>([&](Wire::EntityId id, SkylightComponent& skylightComp)
			{
				if (skylightComp.environmentHandle != Asset::Null())
				{
					if (skylightComp.environmentHandle != skylightComp.lastEnvironmentHandle)
					{
						skylightComp.lastEnvironmentHandle = skylightComp.environmentHandle;
						skylightComp.currentSceneEnvironment = Renderer::GenerateEnvironmentMap(skylightComp.environmentHandle);
					}

					auto asset = Volt::AssetManager::GetAsset<Texture2D>(skylightComp.environmentHandle);
					if (asset && asset->IsValid())
					{
						sceneEnvironment = skylightComp.currentSceneEnvironment;
					}
				}

				Renderer::SetAmbianceMultiplier(skylightComp.intensity);
			});

		registry.ForEach<TextRendererComponent>([&](Wire::EntityId id, const TextRendererComponent& textComp) 
			{
				Ref<Font> fontAsset = AssetManager::GetAsset<Font>(textComp.fontHandle);
				if (fontAsset && fontAsset->IsValid())
				{
					Renderer::SubmitString(textComp.text, fontAsset, myScene->GetWorldSpaceTransform(Entity{ id, myScene.get() }), textComp.maxWidth);
				}
			});

		Volt::Renderer::SetDepthState(DepthState::ReadWrite);
		Volt::Renderer::ResetStatistics();
		Volt::Renderer::Begin();

		// Directional Shadow Pass
		{
			VT_PROFILE_SCOPE("SceneRenderer::DirectionalLight");

			if (dirLightCamera)
			{
				Volt::Renderer::BeginPass(myDirectionalShadowPass, dirLightCamera);
				Volt::Renderer::DispatchRenderCommands();
				Volt::Renderer::EndPass();
			}

		}

		// Pre depth
		//{
		//	Volt::Renderer::BeginPass(myPreDepthPass, aCamera);
		//	Volt::Renderer::DispatchRenderCommands();
		//	Volt::Renderer::EndPass();
		//}

		//GTAOPass(aCamera);

		// Deferred Pass
		{
			VT_PROFILE_SCOPE("SceneRenderer::Deferred");

			Volt::Renderer::BeginPass(myDeferredPass, aCamera);
			Volt::Renderer::DispatchRenderCommands();
			Volt::Renderer::EndPass();
		}

		ShadingPass(sceneEnvironment);

		// Forward Pass
		{
			VT_PROFILE_SCOPE("SceneRenderer::Forward");

			Volt::Renderer::BeginPass(myForwardPass, aCamera);
			Volt::Renderer::DispatchRenderCommands();

			{
				VT_PROFILE_SCOPE("SceneRenderer::ForwardCallbacks");
				for (const auto& callback : myForwardRenderCallbacks)
				{
					callback(myScene, aCamera);
				}
			}

			Volt::Renderer::SetDepthState(DepthState::Read);
			myScene->myParticleSystem->RenderParticles();
			Volt::Renderer::DispatchText();
			Volt::Renderer::EndPass();
		}

		BloomPass(myForwardPass.framebuffer->GetColorAttachment(1));

		for (const auto& callback : myExternalPassRenderCallbacks)
		{
			callback(myScene, aCamera);
		}

		Volt::Renderer::End();
		Volt::Renderer::SetDepthState(DepthState::ReadWrite);
	}

	void SceneRenderer::OnRenderRuntime()
	{
		Ref<Camera> camera = nullptr;
		uint32_t lowestPrio = UINT_MAX;

		myScene->GetRegistry().ForEach<CameraComponent>([&](Wire::EntityId id, const CameraComponent& camComp)
			{
				if (camComp.priority < lowestPrio)
				{
					lowestPrio = camComp.priority;
					camera = camComp.camera;
				}
			});

		if (camera)
		{
			OnRender(camera);
		}
	}

	void SceneRenderer::OnUpdate(float aDeltaTime)
	{}

	void SceneRenderer::Resize(uint32_t width, uint32_t height)
	{
		myDeferredPass.framebuffer->Resize(width, height);
		myShadingPass.framebuffer->Resize(width, height);
		myForwardPass.framebuffer->Resize(width, height);
		myBloomCompositePass.framebuffer->Resize(width, height);

		gem::vec2ui mipSize = { width, height };

		for (auto& pass : myBloomDownsamplePasses)
		{
			mipSize /= 2;
			mipSize = gem::max(mipSize, 1u);

			pass.framebuffer->Resize(mipSize.x, mipSize.y);
		}

		mipSize = { width, height };

		for (auto& myBloomUpsamplePass : std::ranges::reverse_view(myBloomUpsamplePasses))
		{
			mipSize /= 2;
			mipSize = gem::max(mipSize, 1u);

			myBloomUpsamplePass.framebuffer->Resize(mipSize.x, mipSize.y);
		}

		myPreDepthPass.framebuffer->Resize(width, height);
		myMainGTAOPass.framebuffer->Resize(width, height);
		CreateGTAODepthImage(width, height);
	}

	void SceneRenderer::AddForwardCallback(std::function<void(Ref<Scene>, Ref<Camera>)>&& callback)
	{
		myForwardRenderCallbacks.emplace_back(callback);
	}

	void SceneRenderer::AddExternalPassCallback(std::function<void(Ref<Scene>, Ref<Camera>)>&& callback)
	{
		myExternalPassRenderCallbacks.emplace_back(callback);
	}

	Ref<Framebuffer> SceneRenderer::GetFinalFramebuffer()
	{
		return myBloomCompositePass.framebuffer;
	}

	Ref<Framebuffer> SceneRenderer::GetSelectionFramebuffer()
	{
		return myDeferredPass.framebuffer;
	}

	void SceneRenderer::CreatePasses()
	{
		// GBuffer
		{
			Volt::FramebufferSpecification spec{};

			spec.attachments =
			{
				{ ImageFormat::RGBA, { 0.1f, 0.1f, 0.1f, 0.f }, TextureBlend::Alpha, "Albedo" }, // Albedo
				{ ImageFormat::RGBA, { 0.f, 0.f, 0.f, 0.f }, TextureBlend::None, "Material" }, // Material
				{ ImageFormat::RGBAS, { 0.f, 0.f, 0.f, 0.f }, TextureBlend::None, "Normal AO" }, // Normal AO
				{ ImageFormat::RGBAS, { 0.f, 0.f, 0.f, 0.f }, TextureBlend::None, "Vertex Normal" }, // Vertex normal
				{ ImageFormat::RGBA32F, { 0.f, 0.f, 0.f, 0.f }, TextureBlend::None, "World Position" }, // World position
				{ ImageFormat::R32UI, { 0.f, 0.f, 0.f, 0.f }, TextureBlend::None, "ID" }, // ID
				{ ImageFormat::DEPTH32F }
			};

			spec.width = 1920;
			spec.height = 1080;

			myDeferredPass.framebuffer = Volt::Framebuffer::Create(spec);
			myDeferredPass.exclusiveShaderHash = ShaderRegistry::Get("Deferred")->GetHash();
			myDeferredPass.debugName = "Deferred";
		}

		// Shading
		{
			Volt::FramebufferSpecification spec{};

			spec.attachments =
			{
				{ Volt::ImageFormat::RGBA32F, { 0.05f, 0.05f, 0.05f, 1.f }, TextureBlend::Alpha, "Main Color" },
				{ Volt::ImageFormat::RGBA32F, { 0.f, 0.f, 0.f, 1.f }, TextureBlend::None, "Luminance" }
			};

			spec.width = 1920;
			spec.height = 1080;

			myShadingPass.framebuffer = Volt::Framebuffer::Create(spec);
			myShadingPass.debugName = "Shading";
		}

		// Forward
		{
			Volt::FramebufferSpecification spec{};

			spec.attachments =
			{
				{ Volt::ImageFormat::RGBA32F },
				{ Volt::ImageFormat::RGBA32F },
				{ Volt::ImageFormat::R32UI },
				{ Volt::ImageFormat::DEPTH32F }
			};

			spec.existingImages =
			{
				{ 0, myShadingPass.framebuffer->GetColorAttachment(0) },
				{ 1, myShadingPass.framebuffer->GetColorAttachment(1) },
				{ 2, myDeferredPass.framebuffer->GetColorAttachment(5) },
			};

			spec.existingDepth = myDeferredPass.framebuffer->GetDepthAttachment();

			spec.width = 1920;
			spec.height = 1080;

			myForwardPass.framebuffer = Volt::Framebuffer::Create(spec);
			myForwardPass.excludedShaderHashes.emplace_back(myDeferredPass.exclusiveShaderHash);
			myForwardPass.debugName = "Forward";
		}

		// Directional Shadow
		{
			Volt::FramebufferSpecification spec{};

			spec.attachments =
			{
				{ Volt::ImageFormat::R32Typeless, { 1.f, 1.f, 1.f, 1.f }, TextureBlend::None, "Directional Shadow Depth"}
			};

			spec.width = 4096;
			spec.height = 4096;

			myDirectionalShadowPass.framebuffer = Volt::Framebuffer::Create(spec);
			myDirectionalShadowPass.overrideShader = Volt::ShaderRegistry::Get("DirectionalShadow");
			myDirectionalShadowPass.debugName = "Directional Shadow";
		}

		// Bloom
		{
			Volt::FramebufferSpecification spec{};

			spec.attachments =
			{
				{ Volt::ImageFormat::RGBA32F, { 1.f, 1.f, 1.f, 1.f }, TextureBlend::Alpha, "Bloom" }
			};

			spec.width = 1920;
			spec.height = 1080;

			myBloomCompositePass.framebuffer = Framebuffer::Create(spec);

			spec.attachments[0].storageCompatible = true;

			for (uint32_t i = 0; i < myBloomMipCount; i++)
			{
				spec.width /= 2;
				spec.height /= 2;

				myBloomDownsamplePasses.emplace_back(Framebuffer::Create(spec));
			}

			spec.attachments[0].blending = TextureBlend::Add;
			spec.width = 1920;
			spec.height = 1080;

			for (uint32_t i = 0; i < myBloomMipCount; i++)
			{
				spec.width /= 2;
				spec.height /= 2;

				spec.existingImages =
				{
					{ 0, myBloomDownsamplePasses[i].framebuffer->GetColorAttachment(0) }
				};

				myBloomUpsamplePasses.emplace_back(Framebuffer::Create(spec));
			}

			std::reverse(myBloomUpsamplePasses.begin(), myBloomUpsamplePasses.end());

			myBloomDownsamplePipeline = ComputePipeline::Create(ShaderRegistry::Get("BloomDownsample"));
			myBloomUpsamplePipeline = ComputePipeline::Create(ShaderRegistry::Get("BloomUpsample"));
			myBloomUpsampleBuffer = ConstantBuffer::Create(nullptr, sizeof(gem::vec4), ShaderStage::Pixel);
		}

		// GTAO
		{
			Volt::FramebufferSpecification spec{};

			spec.attachments =
			{
				{ Volt::ImageFormat::R32Typeless, { 1.f, 1.f, 1.f, 1.f }, TextureBlend::None, "PreDepth" }
			};

			spec.width = 1920;
			spec.height = 1080;

			myPreDepthPass.framebuffer = Volt::Framebuffer::Create(spec);
			myPreDepthPass.overrideShader = ShaderRegistry::Get("DepthOnly");
			myPreDepthPass.debugName = "Pre Depth";

			myDepthPrefilterPipeline = ComputePipeline::Create(ShaderRegistry::Get("GTAO_DepthPrefilter"));

			CreateGTAODepthImage(1920, 1080);
			myGTAOConstantBuffer = ConstantBuffer::Create(nullptr, sizeof(GTAOConstants), ShaderStage::Compute);

			spec.attachments =
			{
				{ Volt::ImageFormat::R32UI, { 0.f, 0.f, 0.f, 0.f }, TextureBlend::None, "Working AO", true },
				{ Volt::ImageFormat::R8U, { 0.f, 0.f, 0.f, 0.f }, TextureBlend::None, "Working Edges", true },
			};

			myMainGTAOPass.framebuffer = Volt::Framebuffer::Create(spec);
			myMainGTAOPass.debugName = "Main GTAO";
			myMainGTAOPipeline = ComputePipeline::Create(ShaderRegistry::Get("GTAO_MainPass"));
		}
	}

	void SceneRenderer::ShadingPass(const SceneEnvironment& sceneEnv)
	{
		VT_PROFILE_FUNCTION();

		auto context = GraphicsContext::GetContext();

		Renderer::BeginSection("Deferred Shading");

		context->PSSetShaderResources(0, 1, myDeferredPass.framebuffer->GetColorAttachment(0)->GetSRV().GetAddressOf());
		context->PSSetShaderResources(1, 1, myDeferredPass.framebuffer->GetColorAttachment(1)->GetSRV().GetAddressOf());
		context->PSSetShaderResources(2, 1, myDeferredPass.framebuffer->GetColorAttachment(2)->GetSRV().GetAddressOf());
		context->PSSetShaderResources(3, 1, myDeferredPass.framebuffer->GetColorAttachment(3)->GetSRV().GetAddressOf());
		context->PSSetShaderResources(4, 1, myDeferredPass.framebuffer->GetColorAttachment(4)->GetSRV().GetAddressOf());

		context->PSSetShaderResources(11, 1, Renderer::GetDefaultData().brdfLut->GetSRV().GetAddressOf());
		context->PSSetShaderResources(12, 1, sceneEnv.irradianceMap->GetSRV().GetAddressOf());
		context->PSSetShaderResources(13, 1, sceneEnv.radianceMap->GetSRV().GetAddressOf());
		context->PSSetShaderResources(14, 1, myDirectionalShadowPass.framebuffer->GetDepthAttachment()->GetSRV().GetAddressOf());

		myShadingPass.framebuffer->Clear();
		myShadingPass.framebuffer->Bind();
		Renderer::DrawFullscreenTriangleWithShader(ShaderRegistry::Get("Shading"));
		myShadingPass.framebuffer->Unbind();

		ID3D11ShaderResourceView* nullSRV[15] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
		context->PSSetShaderResources(0, 15, nullSRV);

		Renderer::EndSection("Deferred Shading");
	}

	void SceneRenderer::BloomPass(Ref<Image2D> sourceImage)
	{
		VT_PROFILE_FUNCTION();

		Renderer::BeginSection("Bloom");

		constexpr uint32_t threadCount = 32;
		Ref<Image2D> lastSource = sourceImage;
		auto context = GraphicsContext::GetContext();

		// Downsample
		{

			for (auto& pass : myBloomDownsamplePasses)
			{
				context->PSSetShaderResources(0, 1, lastSource->GetSRV().GetAddressOf());
				pass.framebuffer->Clear();
				pass.framebuffer->Bind();
				Renderer::DrawFullscreenTriangleWithShader(ShaderRegistry::Get("BloomDownsamplePS"));
				pass.framebuffer->Unbind();
				lastSource = pass.framebuffer->GetColorAttachment(0);
			}
		}

		// Upsample
		{
			struct BloomUpsampleData
			{
				float filterRadius;
				gem::vec3 padding;
			} data{};

			data.filterRadius = 0.005f;
			myBloomUpsampleBuffer->SetData(&data, sizeof(BloomUpsampleData));
			myBloomUpsampleBuffer->Bind(13);

			for (uint32_t i = 0; i < (uint32_t)myBloomUpsamplePasses.size() - 1; i++)
			{
				auto currMip = myBloomUpsamplePasses[i].framebuffer;
				auto nextMip = myBloomUpsamplePasses[i + 1].framebuffer;

				context->PSSetShaderResources(0, 1, currMip->GetColorAttachment(0)->GetSRV().GetAddressOf());
				nextMip->Bind();
				Renderer::DrawFullscreenTriangleWithShader(ShaderRegistry::Get("BloomUpsamplePS"));
				nextMip->Unbind();
			}
		}

		// Composite
		{
			context->PSSetShaderResources(0, 1, myForwardPass.framebuffer->GetColorAttachment(0)->GetSRV().GetAddressOf());
			context->PSSetShaderResources(1, 1, myBloomUpsamplePasses.back().framebuffer->GetColorAttachment(0)->GetSRV().GetAddressOf());

			myBloomCompositePass.framebuffer->Clear();
			myBloomCompositePass.framebuffer->Bind();
			Renderer::DrawFullscreenTriangleWithShader(ShaderRegistry::Get("BloomComposite"));
			myBloomCompositePass.framebuffer->Unbind();

			ID3D11ShaderResourceView* nullSRV[2] = { nullptr, nullptr };
			context->PSSetShaderResources(0, 2, nullSRV);
		}

		Renderer::EndSection("Bloom");
	}

	void SceneRenderer::GTAOPass(Ref<Camera> aCamera)
	{
		constexpr uint32_t THREAD_COUNT = 8;

		auto context = GraphicsContext::GetContext();

		// Update constants
		{
			GTAOConstants constants{};
			constants.viewportSize = { myDepthPrefilterImage->GetWidth(), myDepthPrefilterImage->GetHeight() };
			constants.viewportPixelSize = { 1.f / constants.viewportSize.x, 1.f / constants.viewportSize.y };

			const gem::mat4& projectionMat = aCamera->GetProjection();
			float depthLinearizeMul = -projectionMat[2].w;
			float depthLinearizeAdd = projectionMat[2].z;

			if (depthLinearizeMul * depthLinearizeAdd < 0.f)
			{
				depthLinearizeAdd = -depthLinearizeAdd;
			}

			constants.depthUnpackConsts = { depthLinearizeMul, depthLinearizeAdd };

			float tanHalfFovY = 1.f / projectionMat[1].y;
			float tanHalfFovX = 1.f / projectionMat[0].x;

			constants.cameraTanHalfFov = { tanHalfFovX, tanHalfFovY };

			constants.NDCToViewMul = { constants.cameraTanHalfFov.x * 2.f, constants.cameraTanHalfFov.y * -2.f };
			constants.NDCToViewAdd = { constants.cameraTanHalfFov.x * -1.f, constants.cameraTanHalfFov.y * 1.f };

			constants.NDCToViewMul_x_PixelSize = { constants.NDCToViewMul.x * constants.viewportPixelSize.x, constants.NDCToViewMul.y * constants.viewportPixelSize.y };
			constants.effectRadius = 1.457f;
			constants.effectFalloffRange = 0.615f;
			constants.sampleDistributionPower = 2.f;
			constants.thinOccluderCompensation = 0.f;
			constants.finalValuePower = 2.2f;
			constants.depthMIPSamplingOffset = 3.3f;
			constants.noiseIndex = 0;
			constants.denoiseBlurBeta = 1e4f;
			constants.padding0 = 0.f;

			myGTAOConstantBuffer->SetData(&constants, sizeof(GTAOConstants));
			myGTAOConstantBuffer->Bind(13);
		}

		// Prefilter Depth
		{
			myDepthPrefilterPipeline->SetImage(myPreDepthPass.framebuffer->GetDepthAttachment(), 0);

			std::array<ID3D11UnorderedAccessView*, 5> mipViews =
			{
				myDepthPrefilterImage->GetUAV(0).Get(),
				myDepthPrefilterImage->GetUAV(1).Get(),
				myDepthPrefilterImage->GetUAV(2).Get(),
				myDepthPrefilterImage->GetUAV(3).Get(),
				myDepthPrefilterImage->GetUAV(4).Get()
			};

			context->CSSetUnorderedAccessViews(0, 5, mipViews.data(), nullptr);
			myDepthPrefilterPipeline->Execute((myPreDepthPass.framebuffer->GetWidth() + 16 - 1) / 16, (myPreDepthPass.framebuffer->GetHeight() + 16 - 1) / 16, 1);
			myDepthPrefilterPipeline->Clear();

			mipViews = { nullptr, nullptr, nullptr, nullptr, nullptr };
			context->CSSetUnorderedAccessViews(0, 5, mipViews.data(), nullptr);
		}

		// Main Pass
		{
			myMainGTAOPipeline->SetTarget(myMainGTAOPass.framebuffer->GetColorAttachment(0), 0);
			myMainGTAOPipeline->SetTarget(myMainGTAOPass.framebuffer->GetColorAttachment(1), 1);
			myMainGTAOPipeline->SetImage(myDepthPrefilterImage, 0);

			myMainGTAOPipeline->Execute((myMainGTAOPass.framebuffer->GetWidth() + THREAD_COUNT - 1) / THREAD_COUNT, (myMainGTAOPass.framebuffer->GetHeight() + THREAD_COUNT - 1) / THREAD_COUNT, 1);
			myMainGTAOPipeline->Clear();
		}
	}

	void SceneRenderer::CreateGTAODepthImage(uint32_t width, uint32_t height)
	{
		ImageSpecification spec{};
		spec.format = ImageFormat::R16F;
		spec.usage = ImageUsage::AttachmentStorage;
		spec.width = width;
		spec.height = height;
		spec.mips = 5;

		myDepthPrefilterImage = Image2D::Create(spec);
		myDepthPrefilterImage->CreateMipUAVs();
	}

	auto DinMamma()
	{
		struct Mother
		{
			void SetSize(uint64_t val) { mySize = val; }
			void Clear() { mySize = 0; }
			uint64_t mySize;
		} myMother{};

		struct Pizza
		{
			void SetContent(Mother aMother) { myMother = aMother; }
			Mother myMother;
		} myPizza{};

		myMother.SetSize(UINT64_MAX);
		myPizza.SetContent(myMother);
		myMother.Clear();

		return myPizza;
	}
}
