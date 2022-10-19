#include "../PBR.hlslh"

Texture2D u_albedo : register(t0);
Texture2D u_material : register(t1);
Texture2D u_normalAO : register(t2);
Texture2D u_vertexNormal : register(t3);
Texture2D u_worldPosition : register(t4);

struct Input
{
	float4 position : SV_Position;
	float2 texCoords : TEXCOORD0;
};

struct Output
{
	float4 color : SV_Target0;
	float4 luminance : SV_Target1;
};

Output main(Input input)
{
	///// Read GBuffer /////
	const float4 albedo = u_albedo.Sample(u_linearSampler, input.texCoords);
	const float4 material = u_material.Sample(u_linearSampler, input.texCoords);
	const float4 normalAO = u_normalAO.Sample(u_linearSampler, input.texCoords);
	const float4 worldPosition = u_worldPosition.Sample(u_linearSampler, input.texCoords);
	////////////////////////

	Output output;
	output.color = 0.f;
	output.luminance = 0.f;

	if (albedo.w == 0.f)
	{
		discard;
		return output;
	}

	float3 lightAccumulation = CalculatePBR(albedo, material, normalAO, worldPosition, input.position);
	output.color = float4(lightAccumulation, 1.f);

	const float brightness = dot(lightAccumulation, float3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.f)
	{
		output.luminance = float4(lightAccumulation, 1.f);
	}
	else
	{
		output.luminance = float4(0.f, 0.f, 0.f, 1.f);
	}

	return output;
}