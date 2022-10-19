#pragma once

#include "Volt/Core/Base.h"
#include "Volt/Rendering/Vertex.h"

#include <vector>
#include <d3d11.h>

namespace Volt
{
	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, uint32_t aSize, uint32_t aStride, D3D11_USAGE usage);
		VertexBuffer(uint32_t aSize, uint32_t aStride);
		~VertexBuffer();

		void SetData(const void* aData, uint32_t aSize);
		void Bind(uint32_t aSlot = 0) const;

		static Ref<VertexBuffer> Create(const void* data, uint32_t aSize, uint32_t aStride, D3D11_USAGE usage = D3D11_USAGE_IMMUTABLE);
		static Ref<VertexBuffer> Create(uint32_t aSize, uint32_t aStride);
	
	private:
		ID3D11Buffer* myBuffer = nullptr;
		uint32_t myStride = 0;
	};
}