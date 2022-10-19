// From https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom

#include "../Common.hlslh"

struct Input
{
	float4 position : SV_Position;
	float2 texCoords : TEXCOORD;
};

Texture2D u_colorSource : register(t0);

float4 main(Input input) : SV_Target
{
	const float2 texCoords = input.texCoords;

	float2 srcTexelSize;
	u_colorSource.GetDimensions(srcTexelSize.x, srcTexelSize.y);

	srcTexelSize = 1.f / srcTexelSize;
	float x = srcTexelSize.x;
	float y = srcTexelSize.y;

	// Take 13 samples around current texel:
	// a - b - c
	// - j - k -
	// d - e - f
	// - l - m -
	// g - h - i
	// === ('e' is the current texel) ===

	const float3 a = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x - 2.f * x, texCoords.y + 2.f * y)).rgb;
	const float3 b = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x, texCoords.y + 2.f * y)).rgb;
	const float3 c = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x + 2.f * x, texCoords.y + 2.f * y)).rgb;

	const float3 d = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x - 2.f * x, texCoords.y)).rgb;
	const float3 e = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x, texCoords.y)).rgb;
	const float3 f = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x + 2.f * x, texCoords.y)).rgb;

	const float3 g = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x - 2.f * x, texCoords.y - 2.f * y)).rgb;
	const float3 h = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x, texCoords.y - 2.f * y)).rgb;
	const float3 i = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x + 2.f * x, texCoords.y - 2.f * y)).rgb;

	const float3 j = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x - x, texCoords.y + y)).rgb;
	const float3 k = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x + x, texCoords.y + y)).rgb;
	const float3 l = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x - x, texCoords.y - y)).rgb;
	const float3 m = u_colorSource.Sample(u_linearSamplerClamp, float2(texCoords.x + x, texCoords.y - y)).rgb;

	// Apply weighted distribution:
	// 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
	// a,b,d,e * 0.125
	// b,c,e,f * 0.125
	// d,e,g,h * 0.125
	// e,f,h,i * 0.125
	// j,k,l,m * 0.5
	// This shows 5 square areas that are being sampled. But some of them overlap,
	// so to have an energy preserving downsample we need to make some adjustments.
	// The weights are the distributed, so that the sum of j,k,l,m (e.g.)
	// contribute 0.5 to the final color output. The code below is written
	// to effectively yield this sum. We get:
	// 0.125*5 + 0.03125*4 + 0.0625*4 = 1

	float3 result = e * 0.125f;
	result += (a + c + g + i) * 0.03125f;
	result += (b + d + f + h) * 0.0625f;
	result += (j + k + l + m) * 0.125;

	result = max(result, 0.0001f);

	return float4(result, 1.f);
}