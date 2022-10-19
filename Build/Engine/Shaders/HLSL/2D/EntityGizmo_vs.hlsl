struct Input
{
	float4 position : POSITION;
	float4 color : COLOR;
	float3 scale : SCALE;
	uint textureIndex : TEXTUREINDEX;
	uint id : ID;
};

Input main(Input input)
{
	Input output;
	output.position = input.position;
	output.color = input.color;
	output.scale = input.scale;
	output.textureIndex = input.textureIndex;
	output.id = input.id;

	return output;
}