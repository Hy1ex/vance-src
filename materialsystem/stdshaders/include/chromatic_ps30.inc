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
class chromatic_ps30_Static_Index
{
public:
	chromatic_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_chromatic_ps30 1


class chromatic_ps30_Dynamic_Index
{
	unsigned int m_nRADIAL : 2;
#ifdef _DEBUG
	bool m_bRADIAL : 1;
#endif	// _DEBUG
public:
	void SetRADIAL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nRADIAL = i;
#ifdef _DEBUG
		m_bRADIAL = true;
#endif	// _DEBUG
	}

	chromatic_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nRADIAL = 0;
#ifdef _DEBUG
		m_bRADIAL = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bRADIAL );
		return ( 1 * m_nRADIAL ) + 0;
	}
};

#define shaderDynamicTest_chromatic_ps30 psh_forgot_to_set_dynamic_RADIAL

