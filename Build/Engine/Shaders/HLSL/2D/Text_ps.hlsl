#include "../Common.hlslh"

struct Input
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 texCoords : TEXCOORD;
	uint textureIndex : TEXTUREINDEX;
};

Texture2D u_textures[32] : register(t0);

float Median(float r, float g, float b)
{
	return max(min(r, g), min(max(r, g), b));
}

float ScreenPxRange(float2 texSize, float2 texCoords)
{
	float pxRange = 2.f;
	float2 unitRange = pxRange / texSize;
	float2 screenTexSize = 1.f / fwidth(texCoords);

	return max(0.5f * dot(unitRange, screenTexSize), 1.f);
}

float4 main(Input input) : SV_Target
{
	float3 msd = 0.f;
	uint2 texSize = 0;

	switch (input.textureIndex)
	{
		case 0: 
			msd = u_textures[0].Sample(u_linearSampler, input.texCoords).xyz; 
			u_textures[0].GetDimensions(texSize.x, texSize.y);
			break;
		
		case 1:
			msd = u_textures[1].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[1].GetDimensions(texSize.x, texSize.y);
			break;

		case 2:
			msd = u_textures[2].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[2].GetDimensions(texSize.x, texSize.y);
			break;

		case 3:
			msd = u_textures[3].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[3].GetDimensions(texSize.x, texSize.y);
			break;

		case 4:
			msd = u_textures[4].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[4].GetDimensions(texSize.x, texSize.y);
			break;

		case 5:
			msd = u_textures[5].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[5].GetDimensions(texSize.x, texSize.y);
			break;

		case 6:
			msd = u_textures[6].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[6].GetDimensions(texSize.x, texSize.y);
			break;

		case 7:
			msd = u_textures[7].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[7].GetDimensions(texSize.x, texSize.y);
			break;
	
		case 8:
			msd = u_textures[8].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[8].GetDimensions(texSize.x, texSize.y);
			break;

		case 9:
			msd = u_textures[9].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[9].GetDimensions(texSize.x, texSize.y);
			break;

		case 10:
			msd = u_textures[10].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[10].GetDimensions(texSize.x, texSize.y);
			break;

		case 11:
			msd = u_textures[11].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[11].GetDimensions(texSize.x, texSize.y);
			break;

		case 12:
			msd = u_textures[12].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[12].GetDimensions(texSize.x, texSize.y);
			break;

		case 13:
			msd = u_textures[13].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[13].GetDimensions(texSize.x, texSize.y);
			break;

		case 14:
			msd = u_textures[14].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[14].GetDimensions(texSize.x, texSize.y);
			break;

		case 15:
			msd = u_textures[15].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[15].GetDimensions(texSize.x, texSize.y);
			break;

		case 16:
			msd = u_textures[16].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[16].GetDimensions(texSize.x, texSize.y);
			break;

		case 17:
			msd = u_textures[17].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[17].GetDimensions(texSize.x, texSize.y);
			break;

		case 18:
			msd = u_textures[18].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[18].GetDimensions(texSize.x, texSize.y);
			break;

		case 19:
			msd = u_textures[19].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[19].GetDimensions(texSize.x, texSize.y);
			break;

		case 20:
			msd = u_textures[20].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[20].GetDimensions(texSize.x, texSize.y);
			break;

		case 21:
			msd = u_textures[21].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[21].GetDimensions(texSize.x, texSize.y);
			break;

		case 22:
			msd = u_textures[22].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[22].GetDimensions(texSize.x, texSize.y);
			break;

		case 23:
			msd = u_textures[23].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[23].GetDimensions(texSize.x, texSize.y);
			break;

		case 24:
			msd = u_textures[24].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[24].GetDimensions(texSize.x, texSize.y);
			break;

		case 25:
			msd = u_textures[25].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[25].GetDimensions(texSize.x, texSize.y);
			break;

		case 26:
			msd = u_textures[26].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[26].GetDimensions(texSize.x, texSize.y);
			break;

		case 27:
			msd = u_textures[27].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[27].GetDimensions(texSize.x, texSize.y);
			break;

		case 28:
			msd = u_textures[28].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[28].GetDimensions(texSize.x, texSize.y);
			break;

		case 29:
			msd = u_textures[29].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[29].GetDimensions(texSize.x, texSize.y);
			break;

		case 30:
			msd = u_textures[30].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[30].GetDimensions(texSize.x, texSize.y);
			break;

		case 31:
			msd = u_textures[31].Sample(u_linearSampler, input.texCoords).xyz;
			u_textures[31].GetDimensions(texSize.x, texSize.y);
			break;
	}

	float4 bgColor = float4(input.color.xyz, 0.f);
	float4 fgColor = input.color;

	float sd = Median(msd.r, msd.g, msd.b);
	float screenPxDistance = ScreenPxRange((float2)texSize, input.texCoords) * (sd - 0.5f);
	float opacity = clamp(screenPxDistance + 0.5f, 0.f, 1.f);


	return lerp(bgColor, fgColor, opacity);
}