#include "../Common.hlslh"
#include "../Buffers.hlslh"
#include "../MatrixFunctions.hlslh"

struct Input
{
	float4 position : POSITION;
	float4 color : COLOR;
	float2 texCoords : TEXCOORD;
	uint textureIndex : TEXTUREINDEX;
	uint id : ID;
};

struct Output
{
	float4 position : SV_Position;
	float3 nearPoint : NEARPOINT;
	float3 farPoint : FARPOINT;
};

float3 UnprojectPoint(float x, float y, float z)
{
	float4x4 viewInv = inverse(u_cameraData.view);
	float4x4 projInv = inverse(u_cameraData.proj);

	float4 unprojectedPoint = mul(viewInv, mul(projInv, float4(x, y, z, 1.f)));
	return unprojectedPoint.xyz / unprojectedPoint.w;
}

Output main(Input input)
{
	float4 position = input.position;
	position.x *= 2.f;
	position.y *= 2.f;

	Output output;
	output.position = position;
	output.nearPoint = UnprojectPoint(position.x, position.y, 0.f);
	output.farPoint = UnprojectPoint(position.x, position.y, 1.f);
	return output;
}