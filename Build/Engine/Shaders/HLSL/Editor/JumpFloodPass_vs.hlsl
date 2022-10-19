struct Input
{
	float3 position : POSITION;
	float2 texCoords : TEXCOORD;
};

struct Output
{
	float4 position : SV_Position;
	float2 texCoords : TEXCOORD;
	float2 texelSize : TEXELSIZE;
	float2 UV[9] : UV0;
};

cbuffer Data : register(b0)
{
	float2 u_texelSize;
	int u_step;
	int u_padding;
};

Output main(Input input)
{
	Output output;
	output.texCoords = input.texCoords;
	output.texelSize = u_texelSize;

	float2 dx = float2(u_texelSize.x, 0.f) * u_step;
	float2 dy = float2(0.f, u_texelSize.y) * u_step;

	output.UV[0] = output.texCoords;

	// Create sample positions in a 3x3 grid
	output.UV[1] = output.texCoords + dx;
	output.UV[2] = output.texCoords - dx;
	output.UV[3] = output.texCoords + dy;
	output.UV[4] = output.texCoords - dy;
	output.UV[5] = output.texCoords + dx + dy;
	output.UV[6] = output.texCoords + dx - dy;
	output.UV[7] = output.texCoords - dx + dy;
	output.UV[8] = output.texCoords - dx - dy;

	output.position = float4(input.position, 1.f);
	return output;
}