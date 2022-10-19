#include "vtpch.h"
#include "Mesh.h"

#include "Volt/Rendering/Buffer/VertexBuffer.h"
#include "Volt/Rendering/Buffer/IndexBuffer.h"

namespace Volt
{
	Mesh::Mesh(std::vector<Vertex> aVertices, std::vector<uint32_t> aIndices, Ref<Material>& aMaterial)
	{
		myVertices = aVertices;
		myIndices = aIndices;

		myMaterial = aMaterial;

		SubMesh subMesh;
		subMesh.indexCount = (uint32_t)aIndices.size();

		mySubMeshes.push_back(subMesh);

		Construct();
	}

	void Mesh::Construct()
	{
		myVertexBuffer = VertexBuffer::Create(myVertices.data(), sizeof(Vertex) * (uint32_t)myVertices.size(), sizeof(Vertex));
		myIndexBuffer = IndexBuffer::Create(myIndices, (uint32_t)myIndices.size());

		for (const auto& vertex : myVertices)
		{
			auto length = gem::distance(myBoundingSphere.center, vertex.position);
			if (length > myBoundingSphere.radius)
			{
				myBoundingSphere.radius = length;
			}
		}
	}
}

