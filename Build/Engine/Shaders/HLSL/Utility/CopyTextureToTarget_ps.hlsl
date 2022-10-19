#include "../Common.hlslh"

struct Input
{
	float4 position : SV_Position;
	float2 texCoords : TEXCOORD0;
};

Texture2D u_textureToCopy : register(t0);

float4 main(Input input) : SV_Target
{
	return u_textureToCopy.Sample(u_pointSampler, input.texCoords);
}