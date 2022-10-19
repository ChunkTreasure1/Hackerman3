#include "../Common.hlslh"
#include "../Buffers.hlslh"
#include "../PBRCommon.hlslh"

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
	float4 color : SV_Target0;
	float4 luminance : SV_Target1;
	uint id : SV_Target2;
};

Output main(Input input)
{
	Output output;
	output.color = float4(1.f, 0.f, 0.f, 1.f);
	output.luminance = float4(15.f, 0.f, 0.f, 1.f);
	output.id = 0;

	return output;
}