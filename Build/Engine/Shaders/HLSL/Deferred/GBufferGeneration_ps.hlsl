#include "../Common.hlslh"
#include "../Buffers.hlslh"

struct Input
{
	float4 position : SV_Position;
	float4 worldPosition : POSITION;
	float3 normal : NORMAL;

	float3x3 TBN : TBN;
	float2 texCoords : TEXCOORD;
};

struct Output
{
	float4 albedo : SV_Target0;
	float4 material : SV_Target1;
	float4 normalAO : SV_Target2;
	float4 vertexNormal : SV_Target3;
	float4 worldPosition : SV_Target4;
	uint id : SV_Target5;
};

Texture2D u_albedo : register(t0);
Texture2D u_normal : register(t1);
Texture2D u_material : register(t2);

cbuffer MaterialBuffer : register(b2)
{
	float u_emissiveStrength;
	float3 u_color;
}

float3 CalculateNormal(float3 normal, float3x3 TBN)
{
	float3 tangentNormal = normal * 2.f - 1.f;
	tangentNormal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));

	return normalize(mul(TBN, tangentNormal));
}

Output main(Input input)
{
	float4 albedo = u_albedo.Sample(u_anisotropicSampler, input.texCoords);
	float4 material = u_material.Sample(u_linearSampler, input.texCoords);
	float3 normal = u_normal.Sample(u_linearSampler, input.texCoords).wyz;

	const float metallic = material.r;
	const float roughness = material.g;
	const float emissive = material.b * u_emissiveStrength;
	const float ao = normal.z;

	normal = CalculateNormal(normal, input.TBN);

	Output output;
	output.albedo = albedo;
	output.albedo.xyz *= u_color;

	output.material = float4(metallic, roughness, emissive, material.a);
	output.normalAO.xyz = normal;
	output.normalAO.w = ao;
	output.vertexNormal = float4(input.normal, 1.f);
	output.worldPosition = input.worldPosition;
	output.id = u_objectData.id;

	return output;
}