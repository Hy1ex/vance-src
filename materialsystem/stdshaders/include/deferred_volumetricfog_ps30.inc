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
class deferred_volumetricfog_ps30_Static_Index
{
	unsigned int m_nCONVERT_TO_SRGB : 2;
public:
	void SetCONVERT_TO_SRGB( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCONVERT_TO_SRGB = i;
	}

	deferred_volumetricfog_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nCONVERT_TO_SRGB = g_pHardwareConfig->NeedsShaderSRGBConversion();
	}

	int GetIndex() const
	{
		return ( 4 * m_nCONVERT_TO_SRGB ) + 0;
	}
};

#define shaderStaticTest_deferred_volumetricfog_ps30 1


class deferred_volumetricfog_ps30_Dynamic_Index
{
	unsigned int m_nFOG : 2;
	unsigned int m_nCSM : 2;
#ifdef _DEBUG
	bool m_bFOG : 1;
	bool m_bCSM : 1;
#endif	// _DEBUG
public:
	void SetFOG( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFOG = i;
#ifdef _DEBUG
		m_bFOG = true;
#endif	// _DEBUG
	}

	void SetCSM( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCSM = i;
#ifdef _DEBUG
		m_bCSM = true;
#endif	// _DEBUG
	}

	deferred_volumetricfog_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nFOG = 0;
		m_nCSM = 0;
#ifdef _DEBUG
		m_bFOG = false;
		m_bCSM = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bFOG && m_bCSM );
		return ( 1 * m_nFOG ) + ( 2 * m_nCSM ) + 0;
	}
};

#define shaderDynamicTest_deferred_volumetricfog_ps30 psh_forgot_to_set_dynamic_FOG + psh_forgot_to_set_dynamic_CSM

