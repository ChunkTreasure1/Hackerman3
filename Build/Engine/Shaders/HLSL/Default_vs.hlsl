#include "Common.hlslh"
#include "Buffers.hlslh"

struct Input
{
	float3 position: POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;

	float2 texCoords : TEXCOORD0;
	float2 texCoords2 : TEXCOORD1;
	float2 texCoords3 : TEXCOORD2;
	float2 texCoords4 : TEXCOORD3;

	float4 color : COLOR0;
	float4 color2 : COLOR1;
	float4 color3 : COLOR2;
	float4 color4 : COLOR3;
	
	uint4 influences : INFLUENCES;
	float4 weights : WEIGHTS;
};

float4 main(Input input) : SV_Position
{
	float4x4 worldViewProjection = mul(u_cameraData.viewProj, u_objectData.transform);
	float4 position = mul(worldViewProjection, float4(input.position, 1.f));

	return position;
}