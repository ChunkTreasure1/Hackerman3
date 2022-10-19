#include "GTAO_Common.hlslh"

SamplerState u_defaultSampler : register(s0);
Texture2D u_sourceNDCDepth : register(t0);

RWTexture2D<lpfloat> o_depth0 : register(u0);
RWTexture2D<lpfloat> o_depth1 : register(u1);
RWTexture2D<lpfloat> o_depth2 : register(u2);
RWTexture2D<lpfloat> o_depth3 : register(u3);
RWTexture2D<lpfloat> o_depth4 : register(u4);

groupshared lpfloat g_scratchDepths[8][8];

lpfloat XeGTAO_ClampDepth(float depth)
{
	return (lpfloat)clamp(depth, 0.0, 65504.0);
}

// weighted average depth filter
lpfloat XeGTAO_DepthMIPFilter(lpfloat depth0, lpfloat depth1, lpfloat depth2, lpfloat depth3, const GTAOConstants consts)
{
	lpfloat maxDepth = max(max(depth0, depth1), max(depth2, depth3));

	const lpfloat depthRangeScaleFactor = 0.75; // found empirically :)
#if XE_GTAO_USE_DEFAULT_CONSTANTS != 0
	const lpfloat effectRadius = depthRangeScaleFactor * (lpfloat)consts.effectRadius * (lpfloat)XE_GTAO_DEFAULT_RADIUS_MULTIPLIER;
	const lpfloat falloffRange = (lpfloat)XE_GTAO_DEFAULT_FALLOFF_RANGE * effectRadius;
#else
	const lpfloat effectRadius = depthRangeScaleFactor * (lpfloat)consts.effectRadius * (lpfloat)consts.radiusMultiplier;
	const lpfloat falloffRange = (lpfloat)consts.effectFalloffRange * effectRadius;
#endif
	const lpfloat falloffFrom = effectRadius * ((lpfloat)1 - (lpfloat)consts.effectFalloffRange);
	// fadeout precompute optimisation
	const lpfloat falloffMul = (lpfloat)-1.0 / (falloffRange);
	const lpfloat falloffAdd = falloffFrom / (falloffRange)+(lpfloat)1.0;

	lpfloat weight0 = saturate((maxDepth - depth0) * falloffMul + falloffAdd);
	lpfloat weight1 = saturate((maxDepth - depth1) * falloffMul + falloffAdd);
	lpfloat weight2 = saturate((maxDepth - depth2) * falloffMul + falloffAdd);
	lpfloat weight3 = saturate((maxDepth - depth3) * falloffMul + falloffAdd);

	lpfloat weightSum = weight0 + weight1 + weight2 + weight3;
	return (weight0 * depth0 + weight1 * depth1 + weight2 * depth2 + weight3 * depth3) / weightSum;
}

[numthreads(8, 8, 1)]
void main(uint2 dispatchId : SV_DispatchThreadID, uint2 groupThreadID : SV_GroupThreadID)
{
	// MIP 0
	const uint2 baseCoord = dispatchId;
	const uint2 pixCoord = baseCoord * 2;

	float4 depths4 = u_sourceNDCDepth.GatherRed(u_defaultSampler, float2(pixCoord * u_constants.viewportPixelSize), int2(1, 1));
	lpfloat depth0 = XeGTAO_ClampDepth(XeGTAO_ScreenSpaceToViewSpaceDepth(depths4.w, u_constants));
	lpfloat depth1 = XeGTAO_ClampDepth(XeGTAO_ScreenSpaceToViewSpaceDepth(depths4.z, u_constants));
	lpfloat depth2 = XeGTAO_ClampDepth(XeGTAO_ScreenSpaceToViewSpaceDepth(depths4.x, u_constants));
	lpfloat depth3 = XeGTAO_ClampDepth(XeGTAO_ScreenSpaceToViewSpaceDepth(depths4.y, u_constants));

	o_depth0[pixCoord + uint2(0, 0)] = (lpfloat)depth0;
	o_depth0[pixCoord + uint2(1, 0)] = (lpfloat)depth1;
	o_depth0[pixCoord + uint2(0, 1)] = (lpfloat)depth2;
	o_depth0[pixCoord + uint2(1, 1)] = (lpfloat)depth3;

	// MIP 1
	lpfloat dm1 = XeGTAO_DepthMIPFilter(depth0, depth1, depth2, depth3, u_constants);
	o_depth1[baseCoord] = (lpfloat)dm1;
	g_scratchDepths[groupThreadID.x][groupThreadID.y] = dm1;

	GroupMemoryBarrierWithGroupSync();

	// MIP 2
	[branch]
	if (all((groupThreadID.xy % 2) == 0))
	{
		lpfloat inTL = g_scratchDepths[groupThreadID.x + 0][groupThreadID.y + 0];
		lpfloat inTR = g_scratchDepths[groupThreadID.x + 1][groupThreadID.y + 0];
		lpfloat inBL = g_scratchDepths[groupThreadID.x + 0][groupThreadID.y + 1];
		lpfloat inBR = g_scratchDepths[groupThreadID.x + 1][groupThreadID.y + 1];

		lpfloat dm2 = XeGTAO_DepthMIPFilter(inTL, inTR, inBL, inBR, u_constants);
		o_depth2[baseCoord / 2] = (lpfloat)dm2;
		g_scratchDepths[groupThreadID.x][groupThreadID.y] = dm2;
	}

	GroupMemoryBarrierWithGroupSync();

	// MIP 3
	[branch]
	if (all((groupThreadID.xy % 4) == 0))
	{
		lpfloat inTL = g_scratchDepths[groupThreadID.x + 0][groupThreadID.y + 0];
		lpfloat inTR = g_scratchDepths[groupThreadID.x + 2][groupThreadID.y + 0];
		lpfloat inBL = g_scratchDepths[groupThreadID.x + 0][groupThreadID.y + 2];
		lpfloat inBR = g_scratchDepths[groupThreadID.x + 2][groupThreadID.y + 2];

		lpfloat dm3 = XeGTAO_DepthMIPFilter(inTL, inTR, inBL, inBR, u_constants);
		o_depth3[baseCoord / 4] = (lpfloat)dm3;
		g_scratchDepths[groupThreadID.x][groupThreadID.y] = dm3;
	}

	GroupMemoryBarrierWithGroupSync();

	// MIP 4
	[branch]
	if (all((groupThreadID.xy % 8) == 0))
	{
		lpfloat inTL = g_scratchDepths[groupThreadID.x + 0][groupThreadID.y + 0];
		lpfloat inTR = g_scratchDepths[groupThreadID.x + 4][groupThreadID.y + 0];
		lpfloat inBL = g_scratchDepths[groupThreadID.x + 0][groupThreadID.y + 4];
		lpfloat inBR = g_scratchDepths[groupThreadID.x + 4][groupThreadID.y + 4];

		lpfloat dm4 = XeGTAO_DepthMIPFilter(inTL, inTR, inBL, inBR, u_constants);
		o_depth4[baseCoord / 8] = (lpfloat)dm4;
		//g_scratchDepths[groupThreadID.x][groupThreadID.y] = dm3;
	}
}