#include "FinalLayer.h"

#include <Volt/Core/Application.h>
#include <Volt/Core/Graphics/Swapchain.h>

#include <Volt/Rendering/Renderer.h>
#include <Volt/Rendering/SceneRenderer.h>
#include <Volt/Rendering/Framebuffer.h>
#include <Volt/Rendering/Shader/ShaderRegistry.h>

FinalLayer::FinalLayer(Ref<Volt::SceneRenderer>& aSceneRenderer)
	: mySceneRenderer(aSceneRenderer)
{}

void FinalLayer::OnAttach()
{
	myCopyToScreenShader = Volt::ShaderRegistry::Get("CopyTextureToTarget");
}

void FinalLayer::OnDetach()
{}

void FinalLayer::OnEvent(Volt::Event & e)
{
	Volt::EventDispatcher dispatcher{ e };
	dispatcher.Dispatch<Volt::AppRenderEvent>(VT_BIND_EVENT_FN(FinalLayer::OnRenderEvent));
}

bool FinalLayer::OnRenderEvent(Volt::AppRenderEvent & e)
{
	Volt::Application::Get().GetWindow().GetSwapchain().Bind();

	auto context = Volt::GraphicsContext::GetContext();

	context->PSSetShaderResources(0, 1, mySceneRenderer->GetFinalFramebuffer()->GetColorAttachment(0)->GetSRV().GetAddressOf());
	Volt::Renderer::DrawFullscreenTriangleWithShader(myCopyToScreenShader);
	
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);

	return false;
}
