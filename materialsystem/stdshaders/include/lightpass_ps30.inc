// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// defined $HDRTYPE && defined $HDRENABLED && !$HDRTYPE && $HDRENABLED
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $HDRTYPE && $LIGHTING_PREVIEW && $HDRTYPE != 0
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW
// defined $HDRTYPE && defined $HDRENABLED && !$HDRTYPE && $HDRENABLED
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $HDRTYPE && $LIGHTING_PREVIEW && $HDRTYPE != 0
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW

#pragma once
#include "shaderlib/cshader.h"
class lightpass_ps30_Static_Index
{
	unsigned int m_nCONVERT_TO_SRGB : 1;
	unsigned int m_nBUMPMAP : 2;
	unsigned int m_nBASETEXTURE2 : 2;
	unsigned int m_nSEAMLESS : 2;
	unsigned int m_nDECAL : 2;
	unsigned int m_nMODEL : 2;
	unsigned int m_nSMOOTHNESS : 2;
#ifdef _DEBUG
	bool m_bCONVERT_TO_SRGB : 1;
	bool m_bBUMPMAP : 1;
	bool m_bBASETEXTURE2 : 1;
	bool m_bSEAMLESS : 1;
	bool m_bDECAL : 1;
	bool m_bMODEL : 1;
	bool m_bSMOOTHNESS : 1;
#endif	// _DEBUG
public:
	void SetCONVERT_TO_SRGB( int i )
	{
		Assert( i >= 0 && i <= 0 );
		m_nCONVERT_TO_SRGB = i;
#ifdef _DEBUG
		m_bCONVERT_TO_SRGB = true;
#endif	// _DEBUG
	}

	void SetBUMPMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPMAP = i;
#ifdef _DEBUG
		m_bBUMPMAP = true;
#endif	// _DEBUG
	}

	void SetBASETEXTURE2( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBASETEXTURE2 = i;
#ifdef _DEBUG
		m_bBASETEXTURE2 = true;
#endif	// _DEBUG
	}

	void SetSEAMLESS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSEAMLESS = i;
#ifdef _DEBUG
		m_bSEAMLESS = true;
#endif	// _DEBUG
	}

	void SetDECAL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDECAL = i;
#ifdef _DEBUG
		m_bDECAL = true;
#endif	// _DEBUG
	}

	void SetMODEL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMODEL = i;
#ifdef _DEBUG
		m_bMODEL = true;
#endif	// _DEBUG
	}

	void SetSMOOTHNESS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSMOOTHNESS = i;
#ifdef _DEBUG
		m_bSMOOTHNESS = true;
#endif	// _DEBUG
	}

	lightpass_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nCONVERT_TO_SRGB = 0;
		m_nBUMPMAP = 0;
		m_nBASETEXTURE2 = 0;
		m_nSEAMLESS = 0;
		m_nDECAL = 0;
		m_nMODEL = 0;
		m_nSMOOTHNESS = 0;
#ifdef _DEBUG
		m_bCONVERT_TO_SRGB = false;
		m_bBUMPMAP = false;
		m_bBASETEXTURE2 = false;
		m_bSEAMLESS = false;
		m_bDECAL = false;
		m_bMODEL = false;
		m_bSMOOTHNESS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bCONVERT_TO_SRGB && m_bBUMPMAP && m_bBASETEXTURE2 && m_bSEAMLESS && m_bDECAL && m_bMODEL && m_bSMOOTHNESS );
		return ( 3 * m_nCONVERT_TO_SRGB ) + ( 3 * m_nBUMPMAP ) + ( 6 * m_nBASETEXTURE2 ) + ( 12 * m_nSEAMLESS ) + ( 24 * m_nDECAL ) + ( 48 * m_nMODEL ) + ( 96 * m_nSMOOTHNESS ) + 0;
	}
};

#define shaderStaticTest_lightpass_ps30 psh_forgot_to_set_static_CONVERT_TO_SRGB + psh_forgot_to_set_static_BUMPMAP + psh_forgot_to_set_static_BASETEXTURE2 + psh_forgot_to_set_static_SEAMLESS + psh_forgot_to_set_static_DECAL + psh_forgot_to_set_static_MODEL + psh_forgot_to_set_static_SMOOTHNESS


class lightpass_ps30_Dynamic_Index
{
	unsigned int m_nPERF : 2;
#ifdef _DEBUG
	bool m_bPERF : 1;
#endif	// _DEBUG
public:
	void SetPERF( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nPERF = i;
#ifdef _DEBUG
		m_bPERF = true;
#endif	// _DEBUG
	}

	lightpass_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nPERF = 0;
#ifdef _DEBUG
		m_bPERF = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bPERF );
		return ( 1 * m_nPERF ) + 0;
	}
};

#define shaderDynamicTest_lightpass_ps30 psh_forgot_to_set_dynamic_PERF

