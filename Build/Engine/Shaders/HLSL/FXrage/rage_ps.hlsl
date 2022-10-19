#include "../Common.hlslh"
#include "../Buffers.hlslh"

//////// Inputs ////////
/*
	input.position: The pixel position in projection space
	input.worldPosition: The pixel position in world space
	input.normal: The pixels interpolated vertex normal

	input.texCoords: The pixels interpolated texture coordinate in slot 0
	input.texCoords2: The pixels interpolated texture coordinate in slot 1
	input.texCoords3: The pixels interpolated texture coordinate in slot 2
	input.texCoords4: The pixels interpolated texture coordinate in slot 3

	input.color: The pixels interpolated vertex color in slot 0
	input.color2: The pixels interpolated vertex color in slot 1
	input.color3: The pixels interpolated vertex color in slot 2
	input.color4: The pixels interpolated vertex color in slot 3

	input.TBN: The pixels interpolated tangent, basis and normal matrix
*/

//////// Outputs ////////
/*
	output.color: The main output color
	output.luminance: The output luminance value (used in bloom calculations)
	output.id: The current objects entity ID (used for mouse picking in the editor, should normally be u_objectData.id)
*/

//////// Shader values ////////
/*
	u_cameraData.view: The current cameras view matrix
	u_cameraData.proj: The current cameras projection matrix
	u_cameraData.viewProj: The current cameras view and projection matrix combined
	u_cameraData.position: The current cameras position
	u_cameraData.ambianceMultiplier: The multiplier used on the ambient light
	u_cameraData.exposure: The exposure added to the scene

	u_objectData.transform: The current objects transform
	u_objectData.id: The current objects id
	u_objectData.isAnimated: Is the current object animated? (0 = false, 1 = true)
	u_objectData.timeSinceCreation: The time elapsed since object creation

	u_passData.targetSize: The current render size

	u_sceneData.timeSinceStart: The time since the scene started playing
	u_sceneData.deltaTime: The delta time of the current frame
*/

//////// Samplers ////////
/*
	u_linearSampler: Min, Mag, Mip Linear (Wrap) sampler
	u_linearPointSampler: Min Linear, Mag, Mip Point (Wrap) sampler
	
	u_pointSampler: Min, Mag, Mip Point (Wrap) sampler
	u_pointLinearSampler: Min Point, Mag, Mip (Wrap) Linear

	u_linearSamplerClamp: Min, Mag, Mip Linear (Clamp) sampler
	u_linearPointSamplerClamp: Min Linear, Mag, Mip Point (Clamp) sampler

	u_pointSamplerClamp: Min, Mag, Mip Point (Clamp) sampler
	u_pointLinearSamplerClamp: Min Point, Mag, Mip (Clamp) Linear

	u_anisotropicSampler: Anisotropic sampler
	u_shadowSampler: Min, Mag, Mip Point, Comparison Less sampler
*/

//////// Material Buffer ////////
/*
	To expose custom variables in the material editor, you can create
	a constant buffer in slot 2. The shader compiler will automatically 
	detect and set this up for you.

	Example:

	cbuffer MaterialBuffer : register(b2)
	{
		float u_emissiveStrength;
		float3 u_color;
	}
*/

//////// Textures ////////
/*
	To expose custom textures in the material editor, you only need to
	create a Texture2D or TextureCube in HLSL and give it a register between 0 and 10.
	This means you can have up to 10 custom textures per shader.

	Example:
		
	Texture2D u_customTexture1 : register(0);
	TextureCube u_customTexture2 : register(1);

	You also have to add these textures in your shader definition file.
	You can find an example of this i Engine/Shaders/Definitions/deferred.vtsdef

*/

struct Input
{
	float4 position : SV_Position;
	float4 worldPosition : POSITION;
	float3 normal : NORMAL;

	float2 texCoords : TEXCOORD0;
	float2 texCoords2 : TEXCOORD1;
	float2 texCoords3 : TEXCOORD2;
	float2 texCoords4 : TEXCOORD3;

	float4 color : COLOR0;
	float4 color2 : COLOR1;
	float4 color3 : COLOR2;
	float4 color4 : COLOR3;

	float3x3 TBN : TBN;
};

struct Output
{
	float4 color : SV_Target0;
	float4 luminance : SV_Target1;
	uint id : SV_Target2;
};


cbuffer MaterialBuffer : register(b2)
{
	float u_emissiveStrength;
	float3 u_color;
}


Texture2D u_albedo : register(t0);

Output main(Input input)
{
//
	
	float vCol = input.color.r; //strange result;
	
	float exp = 2;
	float timeloop = frac(u_objectData.timeSinceCreation*exp);

	float sinExp = sin(u_objectData.timeSinceCreation*3.14)*0.25;
	float cosAndSinExp = cos(sinExp*4)*0.1;

	//float alpha = u_albedo.Sample(u_pointSampler, Upanning).a;
	//
	float2 UV = input.texCoords;
	float2 Upanning = float2(UV.x, UV.y);
	float2 Vpanning = float2(UV.x, (UV.y+0.75)+sinExp);


	//float4 albedo = u_albedo.Sample(u_pointSampler, Upanning);
	float3 col = u_albedo.Sample(u_linearPointSampler, UV.y).rgb;
	float alpha = u_albedo.Sample(u_linearPointSampler, Vpanning).a;

	float op = alpha*vCol;


	Output output;
	output.color = float4 ((col), (alpha));
	output.luminance = 0.f;
	output.id = 0;

	
	return output;
}