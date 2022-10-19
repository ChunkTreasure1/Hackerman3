#include "../PBR.hlslh"

struct Input
{
	float4 position : SV_Position;
	float4 worldPosition : POSITION;
	float3 normal : NORMAL;

	float2 texCoords : TEXCOORD0;
	float2 texCoords2 : TEXCOORD1;
	float2 texCoords3 : TEXCOORD2;
	float2 texCoords4 : TEXCOORD3;

	float4 color : COLOR0;
	float4 color2 : COLOR1;
	float4 color3 : COLOR2;
	float4 color4 : COLOR3;

	float3x3 TBN : TBN;
};

struct Output
{
	float4 color : SV_Target0;
	float4 luminance : SV_Target1;
	uint id : SV_Target2;
};

cbuffer MaterialBuffer : register(b2)
{
	float u_emissiveStrength;
	float3 u_color;
}

Texture2D u_albedo : register(t0);
Texture2D u_normal : register(t1);
Texture2D u_material : register(t2);

float3 CalculateNormal(float3 normal, float3x3 TBN)
{
	float3 tangentNormal = normal * 2.f - 1.f;
	tangentNormal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));

	return normalize(mul(TBN, tangentNormal));
}

Output main(Input input)
{
	float4 albedo = u_albedo.Sample(u_linearSampler, input.texCoords);
	float4 material = u_material.Sample(u_linearSampler, input.texCoords);
	float3 normal = u_normal.Sample(u_linearSampler, input.texCoords).wyz;
	float ao = normal.z;

	normal = CalculateNormal(normal, input.TBN);
	material.z *= u_emissiveStrength;
	albedo.xyz *= u_color;

	float3 lightAccumulation = CalculatePBR(albedo, material, float4(normal, ao), input.worldPosition, input.position);

	Output output;
	output.color = float4(lightAccumulation, albedo.w) * u_cameraData.exposure;

	const float brightness = dot(lightAccumulation, float3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.f)
	{
		output.luminance = float4(lightAccumulation, 1.f);
	}
	else
	{
		output.luminance = float4(0.f, 0.f, 0.f, 1.f);
	}
	output.id = u_objectData.id;

	return output;
}