#include "vtpch.h"
#include "Swapchain.h"

#include "Volt/Log/Log.h"
#include "Volt/Core/Graphics/GraphicsContext.h"
#include "Volt/Utility/DirectXUtils.h"
#include "Volt/Rendering/Renderer.h"
#include "Volt/Core/Profiling.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <chrono>

namespace Volt
{
	Swapchain::Swapchain(GLFWwindow* aWindow, Ref<GraphicsContext> aGraphicsContext)
		: myGraphicsContext(aGraphicsContext), myHeight(1), myWidth(1)
	{
		myWindowHandle = glfwGetWin32Window(aWindow);
		myViewport.TopLeftX = 0.f;
		myViewport.TopLeftY = 0.f;
		myViewport.Width = (float)myWidth;
		myViewport.Height = (float)myHeight;
		myViewport.MinDepth = 0.f;
		myViewport.MaxDepth = 1.f;
	}

	Swapchain::~Swapchain()
	{
		Release();
		mySwapchain = nullptr;
		myRasterizerState = nullptr;
		myGraphicsContext = nullptr;
	}

	void Swapchain::Release()
	{
		myRenderTarget = nullptr;
	}

	void Swapchain::Invalidate(uint32_t width, uint32_t height, bool aFullscreen)
	{
		VT_CORE_ASSERT(myGraphicsContext, "Graphics Context is null!");
		auto device = GraphicsContext::GetDevice();

		myWidth = width;
		myHeight = height;

		if (!mySwapchain)
		{
			DXGI_SWAP_CHAIN_DESC swapChainDesc{};
			swapChainDesc.BufferDesc.Width = width;
			swapChainDesc.BufferDesc.Height = height;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
			swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = 2;
			swapChainDesc.OutputWindow = myWindowHandle;
			swapChainDesc.Windowed = !aFullscreen;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.Flags = 0;

			IDXGIDevice* dxDevice = nullptr;
			IDXGIAdapter* adapter = nullptr;
			IDXGIFactory* factory = nullptr;
			VT_DX_CHECK(device->QueryInterface(IID_PPV_ARGS(&dxDevice)));
			VT_DX_CHECK(dxDevice->GetParent(IID_PPV_ARGS(&adapter)));
			VT_DX_CHECK(adapter->GetParent(IID_PPV_ARGS(&factory)));

			VT_DX_CHECK(factory->CreateSwapChain(dxDevice, &swapChainDesc, mySwapchain.GetAddressOf()));

			device->Release();
			adapter->Release();
			factory->Release();

			D3D11_RASTERIZER_DESC rasterizerDesc = {};
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			rasterizerDesc.FrontCounterClockwise = false;
			GraphicsContext::GetDevice()->CreateRasterizerState(&rasterizerDesc, myRasterizerState.GetAddressOf());
			GraphicsContext::GetContext()->RSSetState(myRasterizerState.Get());
		}

		Release();

		if (width == 0 || height == 0)
		{
			return;
		}

		if (myIsFullscreen != aFullscreen)
		{
			VT_DX_CHECK(mySwapchain->SetFullscreenState(aFullscreen, nullptr));
			myIsFullscreen = aFullscreen;
		}

		VT_DX_CHECK(mySwapchain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));


		ID3D11Texture2D* backBuffer;
		VT_DX_CHECK(mySwapchain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
		VT_DX_CHECK(device->CreateRenderTargetView(backBuffer, 0, myRenderTarget.GetAddressOf()));

		SAFE_RELEASE(backBuffer);

		Bind();

		myViewport.TopLeftX = 0.f;
		myViewport.TopLeftY = 0.f;
		myViewport.Width = (float)myWidth;
		myViewport.Height = (float)myHeight;
		myViewport.MinDepth = 0.f;
		myViewport.MaxDepth = 1.f;
	}

	void Swapchain::BeginFrame()
	{
		auto context = GraphicsContext::GetContext();

#ifdef VT_PROFILE_GPU
		Renderer::GetProfilingData().lastFrame = Renderer::GetProfilingData().currentFrame;
		Renderer::GetProfilingData().currentFrame = (Renderer::GetProfilingData().currentFrame + 1) % 2;
		const uint32_t currentFrame = Renderer::GetProfilingData().currentFrame;

		context->Begin(Renderer::GetProfilingData().disjointQuery[currentFrame].Get());
		context->End(Renderer::GetProfilingData().beginFrameQuery[currentFrame].Get());
#endif
		const float color[4] = { 0.f, 0.f, 0.f, 1.f };
		context->ClearRenderTargetView(myRenderTarget.Get(), color);
	}

	void Swapchain::Present(bool aUseVSync)
	{
		VT_PROFILE_FUNCTION();
		VT_DX_CHECK(mySwapchain->Present(aUseVSync ? 1 : 0, 0));
		Bind();

		auto context = GraphicsContext::GetContext();

#ifdef VT_PROFILE_GPU
		const uint32_t currentFrame = Renderer::GetProfilingData().currentFrame;
		context->End(Renderer::GetProfilingData().endFrameQuery[currentFrame].Get());
		context->End(Renderer::GetProfilingData().disjointQuery[currentFrame].Get());

		CalculateFrameGPUTimes();
#endif
	}

	void Swapchain::Bind() const
	{
		auto context = GraphicsContext::GetContext();
		context->OMSetRenderTargets(1, myRenderTarget.GetAddressOf(), nullptr);
		context->RSSetViewports(1, &myViewport);
	}

	void Swapchain::Resize(uint32_t width, uint32_t height, bool aFullscreen)
	{
		Invalidate(width, height, aFullscreen);
	}

	Ref<Swapchain> Swapchain::Create(GLFWwindow* aWindow, Ref<GraphicsContext> aGraphicsContext)
	{
		return CreateScope<Swapchain>(aWindow, aGraphicsContext);
	}

	void Swapchain::CalculateFrameGPUTimes()
	{
		VT_PROFILE_FUNCTION();

		using namespace std::chrono_literals;

		auto context = GraphicsContext::GetContext();
		auto& profilingData = Renderer::GetProfilingData();

		const uint32_t lastFrame = Renderer::GetProfilingData().lastFrame;

		{
			VT_PROFILE_SCOPE("Wait for GPU");
			while (context->GetData(profilingData.disjointQuery.at(lastFrame).Get(), nullptr, 0, 0) == S_FALSE)
			{
			}
		}

		D3D10_QUERY_DATA_TIMESTAMP_DISJOINT disjointData{};
		context->GetData(profilingData.disjointQuery.at(lastFrame).Get(), &disjointData, sizeof(disjointData), 0);
		if (disjointData.Disjoint)
		{
			return;
		}

		if (myFirstFrame)
		{
			myFirstFrame = false;
			return;
		}

		profilingData.sectionPipelineDatas.at(lastFrame).clear();
		profilingData.sectionTimes.at(lastFrame).clear();

		auto& frameTimeQueries = profilingData.sectionTimeQueries.at(lastFrame);
		auto& framePipelineQueries = profilingData.sectionPipelineQueries.at(lastFrame);

		uint64_t beginTimestamp;
		context->GetData(profilingData.beginFrameQuery.at(lastFrame).Get(), &beginTimestamp, sizeof(uint64_t), 0);
		uint64_t lastTimestamp = beginTimestamp;

		for (const auto& sectionName : profilingData.sectionNames.at(lastFrame))
		{
			uint64_t timestamp;
			context->GetData(frameTimeQueries.at(sectionName).Get(), &timestamp, sizeof(uint64_t), 0);

			float sectionTime = float(timestamp - lastTimestamp) / float(disjointData.Frequency) * 1000.f;
			profilingData.sectionTimes.at(lastFrame).emplace_back(sectionTime);
			profilingData.sectionTotalAverageTimes[sectionName] += sectionTime;
			lastTimestamp = timestamp;
		}

		for (const auto& sectionName : profilingData.sectionNames.at(lastFrame))
		{
			D3D11_QUERY_DATA_PIPELINE_STATISTICS stats{};
			context->GetData(framePipelineQueries.at(sectionName).Get(), &stats, sizeof(D3D11_QUERY_DATA_PIPELINE_STATISTICS), 0);

			auto& pipelineData = profilingData.sectionPipelineDatas.at(lastFrame)[sectionName];
			pipelineData.vertexCount = stats.IAVertices;
			pipelineData.primitiveCount = stats.IAPrimitives;
			pipelineData.vsInvocations = stats.VSInvocations;
			pipelineData.psInvocations = stats.PSInvocations;
		}

		uint64_t endTimestamp;
		context->GetData(profilingData.endFrameQuery.at(lastFrame).Get(), &endTimestamp, sizeof(uint64_t), 0);
		profilingData.frameGPUTime.at(lastFrame) = float(endTimestamp - beginTimestamp) / float(disjointData.Frequency) * 1000.f;
		profilingData.frameTotalAverageGPUTime += profilingData.frameGPUTime.at(lastFrame);

		++myAverageGPUTimingFrames;
		if ((float)glfwGetTime() > myAverageGPUTimingStart + 0.5f)
		{
			profilingData.frameAverageGPUTime = profilingData.frameTotalAverageGPUTime / myAverageGPUTimingFrames;
			profilingData.frameTotalAverageGPUTime = 0.f;

			for (const auto& sectionName : profilingData.sectionNames.at(lastFrame))
			{
				profilingData.sectionAverageTimes[sectionName] = profilingData.sectionTotalAverageTimes.at(sectionName) / myAverageGPUTimingFrames;
				profilingData.sectionTotalAverageTimes.at(sectionName) = 0.f;
			}

			myAverageGPUTimingFrames = 0;
			myAverageGPUTimingStart = (float)glfwGetTime();
		}

		profilingData.sectionNames.at(lastFrame).clear();
	}
}