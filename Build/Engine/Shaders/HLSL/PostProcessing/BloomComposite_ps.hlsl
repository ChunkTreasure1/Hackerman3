// From https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom

#include "../Common.hlslh"
#include "ACES.hlslh"

struct Input
{
	float4 position : SV_Position;
	float2 texCoords : TEXCOORD;
};

Texture2D u_colorSource : register(t0);
Texture2D u_bloomSource : register(t1);

float3 LinearTosRGB(in float3 color)
{
	float3 x = color * 12.92f;
	float3 y = 1.055f * pow(saturate(color), 1.0f / 2.4f) - 0.055f;

	float3 clr = color;
	clr.r = color.r < 0.0031308f ? x.r : y.r;
	clr.g = color.g < 0.0031308f ? x.g : y.g;
	clr.b = color.b < 0.0031308f ? x.b : y.b;

	return clr;
}

float4 main(Input input) : SV_Target
{
	const float bloomStrength = 0.04f;
	const float3 srcColor = u_colorSource.Sample(u_linearSampler, input.texCoords).rgb;
	const float3 bloomColor = u_bloomSource.Sample(u_linearSampler, input.texCoords).rgb;

	float3 result = lerp(srcColor, bloomColor, bloomStrength);
	result = LinearTosRGB(ACESFitted(result) * 1.8f);

	return float4(result, 1.f);
}