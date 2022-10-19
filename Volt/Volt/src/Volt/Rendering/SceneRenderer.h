#pragma once

#include "Volt/Asset/Asset.h"
#include "Volt/Core/Base.h"
#include "Volt/Rendering/RenderPass.h"
#include "Volt/Scene/Scene.h"

#include <functional>

namespace Volt
{
	class Camera;
	class Scene;
	class Framebuffer;
	class Mesh;
	class ComputePipeline;
	class Shader;
	class ConstantBuffer;

	class SceneRenderer
	{
	public:
		SceneRenderer(Ref<Scene> aScene);

		void OnRenderEditor(Ref<Camera> aCamera);
		void OnRenderRuntime();

		void OnUpdate(float aDeltaTime);

		void Resize(uint32_t width, uint32_t height);
		void AddForwardCallback(std::function<void(Ref<Scene>, Ref<Camera>)>&& callback);
		void AddExternalPassCallback(std::function<void(Ref<Scene>, Ref<Camera>)>&& callback);

		Ref<Framebuffer> GetFinalFramebuffer();
		Ref<Framebuffer> GetSelectionFramebuffer();

	private:
		void OnRender(Ref<Camera> aCamera);

		void CreatePasses();
		void ShadingPass(const SceneEnvironment& sceneEnv);

		void BloomPass(Ref<Image2D> sourceImage);
		void GTAOPass(Ref<Camera> aCamera);
		void CreateGTAODepthImage(uint32_t width, uint32_t height);

		Ref<Scene> myScene;
		RenderPass myForwardPass;
		RenderPass myDeferredPass;
		RenderPass myShadingPass;
		RenderPass myDirectionalShadowPass;

		///// Bloom /////
		const uint32_t myBloomMipCount = 6;
		std::vector<RenderPass> myBloomDownsamplePasses;
		std::vector<RenderPass> myBloomUpsamplePasses;
		RenderPass myBloomCompositePass;
		Ref<ComputePipeline> myBloomDownsamplePipeline;
		Ref<ComputePipeline> myBloomUpsamplePipeline;
		Ref<ConstantBuffer> myBloomUpsampleBuffer;
		/////////////////

		///// GTAO /////
		Ref<ConstantBuffer> myGTAOConstantBuffer;
		
		RenderPass myPreDepthPass;
		Ref<Image2D> myDepthPrefilterImage;
		Ref<ComputePipeline> myDepthPrefilterPipeline;

		Ref<ComputePipeline> myMainGTAOPipeline;
		RenderPass myMainGTAOPass;
		////////////////

		std::vector<std::function<void(Ref<Scene>, Ref<Camera>)>> myForwardRenderCallbacks;
		std::vector<std::function<void(Ref<Scene>, Ref<Camera>)>> myExternalPassRenderCallbacks;
	};
}