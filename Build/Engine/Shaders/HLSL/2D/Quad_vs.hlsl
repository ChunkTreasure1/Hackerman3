#include "../Common.hlslh"
#include "../Buffers.hlslh"

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
	float4 color : COLOR;
	float2 texCoords : TEXCOORD;
	uint textureIndex : TEXTUREINDEX;
	uint id : ID;
};

Output main(Input input)
{
	Output output;
	output.position = mul(u_cameraData.viewProj, input.position);
	output.textureIndex = input.textureIndex;
	output.texCoords = input.texCoords;
	output.id = input.id;
	output.color = input.color;

	return output;
}