#ifndef COMMON_MORPHING_VS_FXC_H
#define COMMON_MORPHING_VS_FXC_H

//  DYNAMIC: "MORPHING"				"0..1" [ = pShaderAPI->IsHWMorphingEnabled() ]
//	SKIP: $DECAL && $MORPHING == 0

//	Echoes; Disabled. We likely won't ever use HW morphs.
//	SKIP: $MORPHING

//-----------------------------------------------------------------------------
// Methods to sample morph data from a vertex texture
// NOTE: vMorphTargetTextureDim.x = width, cVertexTextureDim.y = height, cVertexTextureDim.z = # of float4 fields per vertex
// For position + normal morph for example, there will be 2 fields.
//-----------------------------------------------------------------------------
float4 SampleMorphDelta( sampler2D vt, const float3 vMorphTargetTextureDim, const float4 vMorphSubrect, const float flVertexID, const float flField )
{
	float flColumn = floor( flVertexID / vMorphSubrect.w );

	float4 t;
	t.x = vMorphSubrect.x + vMorphTargetTextureDim.z * flColumn + flField + 0.5f;
	t.y = vMorphSubrect.y + flVertexID - flColumn * vMorphSubrect.w + 0.5f;
	t.xy /= vMorphTargetTextureDim.xy;	
	t.z = t.w = 0.f;

	return tex2Dlod( vt, t );
}

// Optimized version which reads 2 deltas
void SampleMorphDelta2( sampler2D vt, const float3 vMorphTargetTextureDim, const float4 vMorphSubrect, const float flVertexID, out float4 delta1, out float4 delta2 )
{
	float flColumn = floor( flVertexID / vMorphSubrect.w );

	float4 t;
	t.x = vMorphSubrect.x + vMorphTargetTextureDim.z * flColumn + 0.5f;
	t.y = vMorphSubrect.y + flVertexID - flColumn * vMorphSubrect.w + 0.5f;
	t.xy /= vMorphTargetTextureDim.xy;	
	t.z = t.w = 0.f;

	delta1 = tex2Dlod( vt, t );
	t.x += 1.0f / vMorphTargetTextureDim.x;
	delta2 = tex2Dlod( vt, t );
}

bool ApplyMorph( sampler2D morphSampler, const float3 vMorphTargetTextureDim, const float4 vMorphSubrect, 
				const float flVertexID, const float3 vMorphTexCoord,
				inout float3 vPosition )
{
#if MORPHING

#if !DECAL
	// Flexes coming in from a separate stream
	float4 vPosDelta = SampleMorphDelta( morphSampler, vMorphTargetTextureDim, vMorphSubrect, flVertexID, 0 );
	vPosition	+= vPosDelta.xyz;
#else
	float4 t = float4( vMorphTexCoord.x, vMorphTexCoord.y, 0.0f, 0.0f );
	float3 vPosDelta = tex2Dlod( morphSampler, t );
	vPosition	+= vPosDelta.xyz * vMorphTexCoord.z;
#endif // DECAL

	return true;

#else // !MORPHING
	return false;
#endif
}
 
bool ApplyMorph( sampler2D morphSampler, const float3 vMorphTargetTextureDim, const float4 vMorphSubrect, 
				const float flVertexID, const float3 vMorphTexCoord, 
				inout float3 vPosition, inout float3 vNormal )
{
#if MORPHING

#if !DECAL
	float4 vPosDelta, vNormalDelta;
	SampleMorphDelta2( morphSampler, vMorphTargetTextureDim, vMorphSubrect, flVertexID, vPosDelta, vNormalDelta );
	vPosition	+= vPosDelta.xyz;
	vNormal		+= vNormalDelta.xyz;
#else
	float4 t = float4( vMorphTexCoord.x, vMorphTexCoord.y, 0.0f, 0.0f );
	float3 vPosDelta = tex2Dlod( morphSampler, t );
	t.x += 1.0f / vMorphTargetTextureDim.x;
	float3 vNormalDelta = tex2Dlod( morphSampler, t );
	vPosition	+= vPosDelta.xyz * vMorphTexCoord.z;
	vNormal		+= vNormalDelta.xyz * vMorphTexCoord.z;
#endif // DECAL

	return true;

#else // !MORPHING
	return false;
#endif
}

bool ApplyMorph( sampler2D morphSampler, const float3 vMorphTargetTextureDim, const float4 vMorphSubrect, 
				const float flVertexID, const float3 vMorphTexCoord, 
				inout float3 vPosition, inout float3 vNormal, inout float3 vTangent )
{
#if MORPHING

#if !DECAL
	float4 vPosDelta, vNormalDelta;
	SampleMorphDelta2( morphSampler, vMorphTargetTextureDim, vMorphSubrect, flVertexID, vPosDelta, vNormalDelta );
	vPosition	+= vPosDelta.xyz;
	vNormal		+= vNormalDelta.xyz;
	vTangent	+= vNormalDelta.xyz;
#else
	float4 t = float4( vMorphTexCoord.x, vMorphTexCoord.y, 0.0f, 0.0f );
	float3 vPosDelta = tex2Dlod( morphSampler, t );
	t.x += 1.0f / vMorphTargetTextureDim.x;
	float3 vNormalDelta = tex2Dlod( morphSampler, t );
	vPosition	+= vPosDelta.xyz * vMorphTexCoord.z;
	vNormal		+= vNormalDelta.xyz * vMorphTexCoord.z;
	vTangent	+= vNormalDelta.xyz * vMorphTexCoord.z;
#endif // DECAL

	return true;

#else // MORPHING

	return false;
#endif
}

bool ApplyMorph( sampler2D morphSampler, const float3 vMorphTargetTextureDim, const float4 vMorphSubrect,
	const float flVertexID, const float3 vMorphTexCoord,
	inout float3 vPosition, inout float3 vNormal, inout float3 vTangent, out float flWrinkle )
{
#if MORPHING

#if !DECAL
	float4 vPosDelta, vNormalDelta;
	SampleMorphDelta2( morphSampler, vMorphTargetTextureDim, vMorphSubrect, flVertexID, vPosDelta, vNormalDelta );
	vPosition	+= vPosDelta.xyz;
	vNormal		+= vNormalDelta.xyz;
	vTangent	+= vNormalDelta.xyz;
	flWrinkle = vPosDelta.w;
#else
	float4 t = float4( vMorphTexCoord.x, vMorphTexCoord.y, 0.0f, 0.0f );
	float4 vPosDelta = tex2Dlod( morphSampler, t );
	t.x += 1.0f / vMorphTargetTextureDim.x;
	float3 vNormalDelta = tex2Dlod( morphSampler, t );

	vPosition	+= vPosDelta.xyz * vMorphTexCoord.z;
	vNormal		+= vNormalDelta.xyz * vMorphTexCoord.z;
	vTangent	+= vNormalDelta.xyz * vMorphTexCoord.z;
	flWrinkle	= vPosDelta.w * vMorphTexCoord.z;
#endif // DECAL

	return true;

#else // MORPHING

	flWrinkle = 0.0f;
	return false;

#endif
}

#endif
