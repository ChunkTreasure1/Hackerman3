#include "../Common.hlslh"
#include "../Buffers.hlslh"

struct Input
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct Output
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

Output main(Input input)
{
	Output output;
	output.position = mul(u_cameraData.viewProj, input.position);
	output.color = input.color;

	return output;
}