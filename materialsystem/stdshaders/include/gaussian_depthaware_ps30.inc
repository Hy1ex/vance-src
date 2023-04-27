// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// defined $HDRTYPE && defined $HDRENABLED && !$HDRTYPE && $HDRENABLED
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $HDRTYPE && $LIGHTING_PREVIEW && $HDRTYPE != 0
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW

#pragma once
#include "shaderlib/cshader.h"
class gaussian_depthaware_ps30_Static_Index
{
	unsigned int m_nHORIZONTAL : 2;
#ifdef _DEBUG
	bool m_bHORIZONTAL : 1;
#endif	// _DEBUG
public:
	void SetHORIZONTAL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nHORIZONTAL = i;
#ifdef _DEBUG
		m_bHORIZONTAL = true;
#endif	// _DEBUG
	}

	gaussian_depthaware_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nHORIZONTAL = 0;
#ifdef _DEBUG
		m_bHORIZONTAL = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bHORIZONTAL );
		return ( 1 * m_nHORIZONTAL ) + 0;
	}
};

#define shaderStaticTest_gaussian_depthaware_ps30 psh_forgot_to_set_static_HORIZONTAL


class gaussian_depthaware_ps30_Dynamic_Index
{
public:
	gaussian_depthaware_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_gaussian_depthaware_ps30 1

