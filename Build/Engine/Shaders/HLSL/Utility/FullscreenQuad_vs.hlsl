struct Output
{
	float4 position : SV_Position;
	float2 texCoords : TEXCOORD;
};

struct Input
{
	float3 position : POSITION;
	float2 texCoords : TEXCOORD;
};

Output main(Input input)
{
	Output output;
	output.position = float4(input.position, 1.f);
	output.texCoords = input.texCoords;

	return output;
}