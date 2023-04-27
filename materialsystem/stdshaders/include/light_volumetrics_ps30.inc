// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// (CSM == 0) && PERF
// defined $HDRTYPE && defined $HDRENABLED && !$HDRTYPE && $HDRENABLED
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $HDRTYPE && $LIGHTING_PREVIEW && $HDRTYPE != 0
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW

#pragma once
#include "shaderlib/cshader.h"
class light_volumetrics_ps30_Static_Index
{
public:
	light_volumetrics_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_light_volumetrics_ps30 1


class light_volumetrics_ps30_Dynamic_Index
{
	unsigned int m_nCSM : 2;
	unsigned int m_nPERF : 2;
#ifdef _DEBUG
	bool m_bCSM : 1;
	bool m_bPERF : 1;
#endif	// _DEBUG
public:
	void SetCSM( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCSM = i;
#ifdef _DEBUG
		m_bCSM = true;
#endif	// _DEBUG
	}

	void SetPERF( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nPERF = i;
#ifdef _DEBUG
		m_bPERF = true;
#endif	// _DEBUG
	}

	light_volumetrics_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nCSM = 0;
		m_nPERF = 0;
#ifdef _DEBUG
		m_bCSM = false;
		m_bPERF = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bCSM && m_bPERF );
		return ( 1 * m_nCSM ) + ( 2 * m_nPERF ) + 0;
	}
};

#define shaderDynamicTest_light_volumetrics_ps30 psh_forgot_to_set_dynamic_CSM + psh_forgot_to_set_dynamic_PERF

