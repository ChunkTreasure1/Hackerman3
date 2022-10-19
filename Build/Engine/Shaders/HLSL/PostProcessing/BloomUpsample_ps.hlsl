// From https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom

#include "../Common.hlslh"

struct Input
{
	float4 position : SV_Position;
	float2 texCoords : TEXCOORD;
};

cbuffer ShaderData : register(b13)
{
	float u_filterRadius;
	float3 u_padding;
}

Texture2D u_colorSource : register(t0);

float4 main(Input input) : SV_Target
{
	// The filter kernel is applied with a radius, specified in texture
	// coordinates, so that the radius will vary across mip resolutions.
	const float x = u_filterRadius;
	const float y = u_filterRadius;
	const float2 texCoords = input.texCoords;

	// Take 9 samples around current texel:
	// a - b - c
	// d - e - f
	// g - h - i
	// === ('e' is the current texel) ===
	const float3 a = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x - x, texCoords.y + y)).rgb;
	const float3 b = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x,	 texCoords.y + y)).rgb;
	const float3 c = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x + x, texCoords.y + y)).rgb;

	const float3 d = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x - x, texCoords.y)).rgb;
	const float3 e = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x, texCoords.y)).rgb;
	const float3 f = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x + x, texCoords.y)).rgb;

	const float3 g = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x - x, texCoords.y - y)).rgb;
	const float3 h = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x,	 texCoords.y - y)).rgb;
	const float3 i = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x + x, texCoords.y - y)).rgb;

	// Apply weighted distribution, by using a 3x3 tent filter:
	//  1   | 1 2 1 |
	// -- * | 2 4 2 |
	// 16   | 1 2 1 |

	float3 result = e * 4.f;
	result += (b + d + f + h) * 2.f;
	result += (a + c + g + i);
	result *= 1.f / 16.f;

	return float4(result, 1.f);
}