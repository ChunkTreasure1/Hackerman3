#include "../Common.hlslh"
#include "../Buffers.hlslh"

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

struct Output
{
	float4 position : SV_Position;
	float4 worldPosition : POSITION;
	float3 normal : NORMAL;

	float3x3 TBN : TBN;
	float2 texCoords : TEXCOORD;
};

Output main(Input input)
{
	float4x4 skinningMatrix = { 1, 0, 0, 0, 
								0, 1, 0, 0, 
								0, 0, 1, 0, 
								0, 0, 0, 1 };	

	if (u_objectData.isAnimated)
	{
		skinningMatrix = 0;
		skinningMatrix += mul(u_animationData.bones[input.influences.x], input.weights.x);
		skinningMatrix += mul(u_animationData.bones[input.influences.y], input.weights.y);
		skinningMatrix += mul(u_animationData.bones[input.influences.z], input.weights.z);
		skinningMatrix += mul(u_animationData.bones[input.influences.w], input.weights.w);
	}

	const float3x3 worldNormalRotation = (float3x3)u_objectData.transform;
	const float3x3 skinNormalRotation = (float3x3)skinningMatrix;

	const float3 T = normalize(mul(worldNormalRotation, mul(input.tangent, skinNormalRotation)));
	const float3 B = normalize(mul(worldNormalRotation, mul(input.bitangent, skinNormalRotation)));
	const float3 N = normalize(mul(worldNormalRotation, mul(input.normal, skinNormalRotation)));

	Output output;
	
	output.worldPosition = mul(u_objectData.transform, mul(skinningMatrix, float4(input.position, 1.f)));
	output.position = mul(u_cameraData.viewProj, output.worldPosition);
	output.TBN = transpose(float3x3(T, - B, N));

	output.texCoords = input.texCoords;
	output.normal = N;

	return output;
}