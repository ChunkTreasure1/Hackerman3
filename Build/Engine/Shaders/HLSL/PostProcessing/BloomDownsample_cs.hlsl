// From https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom

#include "../Common.hlslh"

RWTexture2D<float4> o_output : register(u0);
Texture2D u_inputTexture;

[numthreads(32, 32, 1)]
void main(uint2 threadId : SV_DispatchThreadID)
{
	float2 outputSize;
	o_output.GetDimensions(outputSize.x, outputSize.y);

	float2 inputSize;
	u_inputTexture.GetDimensions(inputSize.x, inputSize.y);

	const float2 texCoords = float2((float)threadId.x / outputSize.x, (float)threadId.y / outputSize.y);
	const float2 texelSize = 1.f / inputSize;

	const float x = texelSize.x;
	const float y = texelSize.y;

	// Take 13 samples around current texel:
	// a - b - c
	// - j - k -
	// d - e - f
	// - l - m -
	// g - h - i
	// === ('e' is the current texel) ===

	const float3 a = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x - 2.f * x, texCoords.y + 2.f * y), 0).rgb;
	const float3 b = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x,		   texCoords.y + 2.f * y), 0).rgb;
	const float3 c = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x + 2.f * x, texCoords.y + 2.f * y), 0).rgb;

	const float3 d = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x - 2.f * x, texCoords.y), 0).rgb;
	const float3 e = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x,		   texCoords.y), 0).rgb;
	const float3 f = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x + 2.f * x, texCoords.y), 0).rgb;

	const float3 g = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x - 2.f * x, texCoords.y - 2.f * y), 0).rgb;
	const float3 h = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x,		   texCoords.y - 2.f * y), 0).rgb;
	const float3 i = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x + 2.f * x, texCoords.y - 2.f * y), 0).rgb;

	const float3 j = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x - x, texCoords.y + y), 0).rgb;
	const float3 k = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x + x, texCoords.y + y), 0).rgb;
	const float3 l = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x - x, texCoords.y - y), 0).rgb;
	const float3 m = u_inputTexture.SampleLevel(u_linearSamplerClamp, float2(texCoords.x + x, texCoords.y - y), 0).rgb;

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

	float3 result = e * 0.124f;
	result += (a + c + g + i) * 0.03125f;
	result += (b + d + f + h) * 0.0625f;
	result += (j + k + l + m) * 0.125;

	result = max(result, 0.0001f);

	o_output[threadId] = float4(result, 1.f);
}