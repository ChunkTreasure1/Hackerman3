// From https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom

#include "../Common.hlslh"

RWTexture2D<float4> o_output : register(u0);
Texture2D u_inputTexture;

cbuffer ShaderData : register(b13)
{
	float u_filterRadius;
	float3 u_padding;
}

[numthreads(32, 32, 1)]
void main(uint2 threadId : SV_DispatchThreadID)
{
	float2 outputSize;
	o_output.GetDimensions(outputSize.x, outputSize.y);
	const float2 texCoords = float2(threadId.x / outputSize.x, threadId.y / outputSize.y);

	// The filter kernel is applied with a radius, specified in texture
	// coordinates, so that the radius will vary across mip resolutions.
	const float x = u_filterRadius;
	const float y = u_filterRadius;

	// Take 9 samples around current texel:
	// a - b - c
	// d - e - f
	// g - h - i
	// === ('e' is the current texel) ===
	const float3 a = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x - x, texCoords.y + y), 0).rgb;
	const float3 b = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x,	 texCoords.y + y), 0).rgb;
	const float3 c = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x + x, texCoords.y + y), 0).rgb;

	const float3 d = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x - x, texCoords.y), 0).rgb;
	const float3 e = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x,	 texCoords.y), 0).rgb;
	const float3 f = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x + x, texCoords.y), 0).rgb;

	const float3 g = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x - x, texCoords.y - y), 0).rgb;
	const float3 h = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x,	 texCoords.y - y), 0).rgb;
	const float3 i = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x + x, texCoords.y - y), 0).rgb;

	// Apply weighted distribution, by using a 3x3 tent filter:
	//  1   | 1 2 1 |
	// -- * | 2 4 2 |
	// 16   | 1 2 1 |

	float3 result = e * 4.f;
	result += (b + d + f + h) * 2.f;
	result += (a + c + g + i);
	result *= 1.f / 16.f;

	o_output[threadId] += float4(result, 1.f);
}