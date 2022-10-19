struct Input
{
	float4 position : SV_Position;
};

cbuffer MaterialBuffer : register(b2)
{
	float4 u_color;
}

float4 main(Input input) : SV_Target
{
	return u_color;
}