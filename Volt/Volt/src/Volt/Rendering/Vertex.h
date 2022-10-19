#pragma once

#include <GEM/gem.h>

namespace Volt
{
	static bool AbsEqualVector(const gem::vec3& aFirst, const gem::vec3& aSecond)
	{
		return std::abs(aFirst.x - aSecond.x) < std::numeric_limits<float>::epsilon() &&
			std::abs(aFirst.y - aSecond.y) < std::numeric_limits<float>::epsilon() &&
			std::abs(aFirst.z - aSecond.z) < std::numeric_limits<float>::epsilon();
	}

	static bool AbsEqualVector(const gem::vec2& aFirst, const gem::vec2& aSecond)
	{
		return std::abs(aFirst.x - aSecond.x) < std::numeric_limits<float>::epsilon() &&
			std::abs(aFirst.y - aSecond.y) < std::numeric_limits<float>::epsilon();
	}

	struct Vertex
	{
		Vertex() = default;

		Vertex(const gem::vec3& aPosition)
			: position(aPosition)
		{}

		Vertex(const gem::vec3& aPosition, const gem::vec2& aTexCoords)
			: position(aPosition)
		{
			texCoords[0] = aTexCoords;
		}

		bool operator==(const Vertex& aVert) const
		{
			bool bPos = AbsEqualVector(position, aVert.position);
			bool bNorm = AbsEqualVector(normal, aVert.normal);
			bool bTex = AbsEqualVector(texCoords[0], aVert.texCoords[0]);

			return bPos && bNorm && bTex;
		}

		gem::vec3 position = gem::vec3(0.f);
		gem::vec3 normal = gem::vec3(0.f);
		gem::vec3 tangent = gem::vec3(0.f);
		gem::vec3 bitangent = gem::vec3(0.f);

		gem::vec2 texCoords[4] = { { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f } };
		gem::vec4 color[4] = { { 0.f, 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f, 0.f } };

		gem::vec4ui influences = { 0, 0, 0, 0 };
		gem::vec4 weights = { 0.f, 0.f, 0.f, 0.f };
	};

	struct SpriteVertex
	{
		gem::vec4 position = gem::vec4(0.f);
		gem::vec4 color = gem::vec4(1.f);
		gem::vec2 texCoords = gem::vec2(0.f);
		uint32_t textureIndex = 0;
		uint32_t id = 0;
	};

	struct BillboardVertex
	{
		gem::vec4 postition = gem::vec4(0.f);
		gem::vec4 color = gem::vec4(1.f);
		gem::vec3 scale = gem::vec3(1.f);
		uint32_t textureIndex = 0;
		uint32_t id = 0;
	};

	struct LineVertex
	{
		gem::vec4 position = gem::vec4(0.f);
		gem::vec4 color = gem::vec4(1.f);
	};

	struct TextVertex
	{
		gem::vec4 position = gem::vec4(0.f);
		gem::vec4 color = gem::vec4(1.f);
		gem::vec2 texCoords = gem::vec2(0.f);
		uint32_t textureIndex = 0;
	};
}