#include "vtpch.h"
#include "StructuredBuffer.h"

namespace Volt
{
	StructuredBuffer::StructuredBuffer(uint32_t elementSize, uint32_t count, ShaderStage usageStage)
		: myElementSize(elementSize), myMaxCount(count)
	{
		VT_CORE_ASSERT(elementSize % 16 == 0, "Structure must be 16 byte aligned!");

		auto device = GraphicsContext::GetDevice();

		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = elementSize * count;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = elementSize;

		VT_DX_CHECK(device->CreateBuffer(&desc, nullptr, myBuffer.GetAddressOf()));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = count;

		VT_DX_CHECK(device->CreateShaderResourceView(myBuffer.Get(), &srvDesc, mySRV.GetAddressOf()));

		//D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
		//uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		//uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		//uavDesc.Buffer.FirstElement = 0;
		//uavDesc.Buffer.NumElements = count;

		//VT_DX_CHECK(device->CreateUnorderedAccessView(myBuffer.Get(), &uavDesc, myUAV.GetAddressOf()));
	}

	StructuredBuffer::~StructuredBuffer()
	{
		myBuffer.Reset();
	}

	void StructuredBuffer::Bind(uint32_t slot) const
	{
		auto context = GraphicsContext::GetContext();

		if ((myUsageStages & ShaderStage::Vertex) != ShaderStage::None)
		{
			context->VSSetShaderResources(slot, 1, mySRV.GetAddressOf());
		}

		if ((myUsageStages & ShaderStage::Pixel) != ShaderStage::None)
		{
			context->PSSetShaderResources(slot, 1, mySRV.GetAddressOf());
		}

		if ((myUsageStages & ShaderStage::Compute) != ShaderStage::None)
		{
			context->CSSetShaderResources(slot, 1, mySRV.GetAddressOf());
		}

		if ((myUsageStages & ShaderStage::Domain) != ShaderStage::None)
		{
			context->DSSetShaderResources(slot, 1, mySRV.GetAddressOf());
		}

		if ((myUsageStages & ShaderStage::Hull) != ShaderStage::None)
		{
			context->HSSetShaderResources(slot, 1, mySRV.GetAddressOf());
		}

		if ((myUsageStages & ShaderStage::Geometry) != ShaderStage::None)
		{
			context->GSSetShaderResources(slot, 1, mySRV.GetAddressOf());
		}
	}

	void StructuredBuffer::Unmap()
	{
		auto context = GraphicsContext::GetContext();
		context->Unmap(myBuffer.Get(), 0);
	}

	void StructuredBuffer::AddStage(ShaderStage stage)
	{
		myUsageStages = myUsageStages | stage;
	}

	Ref<StructuredBuffer> StructuredBuffer::Create(uint32_t elementSize, uint32_t count, ShaderStage usageStage)
	{
		return CreateRef<StructuredBuffer>(elementSize, count, usageStage);
	}
}