#include "../Common.hlslh"

struct Input
{
	float4 position : SV_Position;
	float2 texCoords : TEXCOORD;
};

Texture2D u_texture : register(t0);

float4 main(Input input) : SV_Target
{
	float4 pixel = u_texture.Sample(u_linearSampler, input.texCoords);

	float dist = sqrt(pixel.z);
	float alpha = smoothstep(0.004f, 0.002f, dist);
	if (alpha == 0.f)
	{
		discard;
		return pixel;
	}

	float3 outlineColor = float3(1.f, 0.5f, 0.f);
	return float4(outlineColor, alpha);
}