#include "../Common.hlslh"

struct Input
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 texCoords : UV;
	uint textureIndex : TEXTUREINDEX;
	uint id : ID;
};

struct Output
{
	float4 color : SV_Target0;
	uint id : SV_Target1;
};

Texture2D u_textures[32] : register(t0);

Output main(Input input)
{
	float4 result = 0.f;

	switch (input.textureIndex)
	{
		case 0: result = u_textures[0].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 1: result = u_textures[1].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 2: result = u_textures[2].Sample(u_linearSampler, input.texCoords) * input.color;	break;
		case 3: result = u_textures[3].Sample(u_linearSampler, input.texCoords) * input.color;	break;
		case 4: result = u_textures[4].Sample(u_linearSampler, input.texCoords) * input.color;	break;
		case 5: result = u_textures[5].Sample(u_linearSampler, input.texCoords) * input.color;	break;
		case 6: result = u_textures[6].Sample(u_linearSampler, input.texCoords) * input.color;	break;
		case 7: result = u_textures[7].Sample(u_linearSampler, input.texCoords) * input.color;	break;
		case 8: result = u_textures[8].Sample(u_linearSampler, input.texCoords) * input.color;	break;
		case 9: result = u_textures[9].Sample(u_linearSampler, input.texCoords) * input.color;	break;
		case 10: result = u_textures[10].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 11: result = u_textures[11].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 12: result = u_textures[12].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 13: result = u_textures[13].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 14: result = u_textures[14].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 15: result = u_textures[15].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 16: result = u_textures[16].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 17: result = u_textures[17].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 18: result = u_textures[18].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 19: result = u_textures[19].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 20: result = u_textures[20].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 21: result = u_textures[21].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 22: result = u_textures[22].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 23: result = u_textures[23].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 24: result = u_textures[24].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 25: result = u_textures[25].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 26: result = u_textures[26].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 27: result = u_textures[27].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 28: result = u_textures[28].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 29: result = u_textures[29].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 30: result = u_textures[30].Sample(u_linearSampler, input.texCoords) * input.color; break;
		case 31: result = u_textures[31].Sample(u_linearSampler, input.texCoords) * input.color; break;
	}

	Output output;
	output.color = 0.f;
	output.id = 0;

	output.color = result;
	output.id = input.id;

	return output;
}