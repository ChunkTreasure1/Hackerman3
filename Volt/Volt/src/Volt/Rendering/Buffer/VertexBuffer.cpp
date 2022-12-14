#include "vtpch.h"
#include "VertexBuffer.h"

#include "Volt/Core/Graphics/GraphicsContext.h"
#include "Volt/Log/Log.h"
#include "Volt/Utility/DirectXUtils.h"

namespace Volt
{
	VertexBuffer::VertexBuffer(const void* aData, uint32_t aSize, uint32_t aStride, D3D11_USAGE usage)
		: myStride(aStride)
	{
		D3D11_BUFFER_DESC vertexBuffer{};
		vertexBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBuffer.Usage = usage;
		vertexBuffer.ByteWidth = aSize;
		vertexBuffer.CPUAccessFlags = usage == D3D11_USAGE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;
		vertexBuffer.MiscFlags = 0;
		vertexBuffer.StructureByteStride = aStride;

		D3D11_SUBRESOURCE_DATA data{};
		data.pSysMem = aData;

		auto device = GraphicsContext::GetDevice();
		VT_DX_CHECK(device->CreateBuffer(&vertexBuffer, &data, &myBuffer));
	}

	VertexBuffer::VertexBuffer(uint32_t aSize, uint32_t aStride)
		: myStride(aStride)
	{
		D3D11_BUFFER_DESC vertexBuffer{};
		vertexBuffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBuffer.Usage = D3D11_USAGE_DYNAMIC;
		vertexBuffer.ByteWidth = aSize;
		vertexBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBuffer.MiscFlags = 0;
		vertexBuffer.StructureByteStride = aStride;

		auto device = GraphicsContext::GetDevice();
		VT_DX_CHECK(device->CreateBuffer(&vertexBuffer, nullptr, &myBuffer));
	}

	VertexBuffer::~VertexBuffer()
	{
		SAFE_RELEASE(myBuffer);
	}

	void VertexBuffer::SetData(const void* aData, uint32_t aSize)
	{
		auto context = GraphicsContext::GetContext();

		D3D11_MAPPED_SUBRESOURCE data;
		context->Map(myBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		memcpy(data.pData, aData, aSize);
		context->Unmap(myBuffer, 0);
	}

	void VertexBuffer::Bind(uint32_t aSlot) const
	{
		auto context = GraphicsContext::GetContext();

		const uint32_t offset = 0;

		context->IASetVertexBuffers(aSlot, 1, &myBuffer, &myStride, &offset);
	}

	Ref<VertexBuffer> VertexBuffer::Create(const void* data, uint32_t aSize, uint32_t aStride, D3D11_USAGE usage)
	{
		return CreateRef<VertexBuffer>(data, aSize, aStride, usage);
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t aSize, uint32_t aStride)
	{
		return CreateRef<VertexBuffer>(aSize, aStride);
	}
}