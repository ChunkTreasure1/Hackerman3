#include "../Common.hlslh"

#define LOCAL_SIZE 8
#define MAX_MIP_BATCH_SIZE 4

Texture2D u_inputDepth : register(t0);

RWTexture2D<float> o_output0 : register(u0);
RWTexture2D<float> o_output1 : register(u1);
RWTexture2D<float> o_output2 : register(u2);
RWTexture2D<float> o_output3 : register(u3);

cbuffer HZBBuffer : register(b13)
{
	float2 u_dispatchThreadIdToBufferUV;
	float2 u_inverseSize;
	float2 u_inputViewportMaxBound;
	int u_firstLod;
	int u_isFirstPass;
}

groupshared float g_closestDeviceZ[LOCAL_SIZE * LOCAL_SIZE];

float4 Gather4(float2 bufferUV, float lod)
{
	float2 uv[4];
	uv[0] = min(bufferUV + float2(-0.5f, -0.5f) * u_inverseSize, u_inputViewportMaxBound);
	uv[1] = min(bufferUV + float2( 0.5f, -0.5f) * u_inverseSize, u_inputViewportMaxBound);
	uv[2] = min(bufferUV + float2(-0.5f,  0.5f) * u_inverseSize, u_inputViewportMaxBound);
	uv[3] = min(bufferUV + float2( 0.5f,  0.5f) * u_inverseSize, u_inputViewportMaxBound);

	float4 depth;
	depth.x = u_inputDepth.SampleLevel(u_pointSampler, uv[0], lod).x;
	depth.y = u_inputDepth.SampleLevel(u_pointSampler, uv[1], lod).x;
	depth.z = u_inputDepth.SampleLevel(u_pointSampler, uv[2], lod).x;
	depth.w = u_inputDepth.SampleLevel(u_pointSampler, uv[3], lod).x;
	
	return depth;
}

uint SignedRightShift(uint x, const int bitshift)
{
	uint result = x;

	if (bitshift > 0)
	{
		result = x << uint(bitshift);
	}
	else if (bitshift < 0)
	{
		result = x >> uint(-bitshift);
	}

	return result;
}

int2 InitialTilePixelPositionForReduction2x2(const uint tileSizeLog2, uint sharedArrayId)
{
	uint x = 0;
	uint y = 0;

	for (uint i = 0; i < tileSizeLog2; i++)
	{
		const uint destBitId = tileSizeLog2 - 1 - i;
		const uint destBitMask = 1 << destBitId;

		x |= destBitMask & SignedRightShift(sharedArrayId, int(destBitId) - int(i * 2 + 0));
		y |= destBitMask & SignedRightShift(sharedArrayId, int(destBitId) - int(i * 2 + 1));
	}

	return int2(x, y);
}

[numthreads(LOCAL_SIZE, LOCAL_SIZE, 1)]
void main(const uint groupIndex : SV_GroupIndex, const uint groupId : SV_GroupID)
{
	int2 groupThreadId = InitialTilePixelPositionForReduction2x2(uint(MAX_MIP_BATCH_SIZE - 1), groupIndex);
	int2 dispatchThreadId = (LOCAL_SIZE * groupId) + groupThreadId;

	float2 bufferUV = ((float2)dispatchThreadId + 0.5f) * u_dispatchThreadIdToBufferUV;
	float4 deviceZ = Gather4(bufferUV, u_firstLod - 1.f);
	float closestDeviceZ = max(max(deviceZ.x, deviceZ.y), max(deviceZ.z, deviceZ.z));

	int2 outputPixelPos = dispatchThreadId;
	if (u_isFirstPass != 0)
	{
		o_output0[outputPixelPos] = u_inputDepth.SampleLevel(u_pointSampler, bufferUV, 0).x;
	}
	else
	{
		o_output0[outputPixelPos] = closestDeviceZ;
	}

	g_closestDeviceZ[groupIndex] = closestDeviceZ;

	GroupMemoryBarrierWithGroupSync();

	for (int mipLevel = 1; mipLevel < MAX_MIP_BATCH_SIZE; mipLevel++)
	{
		const int tileSize = ((uint)LOCAL_SIZE / (uint)(1 << mipLevel));
		const int reduceBankSize = tileSize * tileSize;

		if (groupIndex < (uint)reduceBankSize)
		{
			float4 parentDeviceZ;
			parentDeviceZ.x = closestDeviceZ;

			[unroll]
			for (int i = 1; i < MAX_MIP_BATCH_SIZE; i++)
			{
				parentDeviceZ[i] = g_closestDeviceZ[groupIndex + i * reduceBankSize];
			}

			closestDeviceZ = max(max(parentDeviceZ.x, parentDeviceZ.y), max(parentDeviceZ.z, parentDeviceZ.w));
			outputPixelPos = outputPixelPos >> 1;
			g_closestDeviceZ[groupIndex] = closestDeviceZ;
			
			switch (mipLevel)
			{
				case 1: o_output1[outputPixelPos] = closestDeviceZ; break;
				case 2: o_output2[outputPixelPos] = closestDeviceZ;	break;
				case 3: o_output3[outputPixelPos] = closestDeviceZ;	break;
			}
		}
	}
}