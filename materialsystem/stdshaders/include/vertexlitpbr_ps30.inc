// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ($CUBEMAP || FLASHLIGHT ) && $LIGHT_PREVIEW
// ($PIXELFOGTYPE == 0) && ($WRITEWATERFOGTODESTALPHA != 0)
// ( $FLASHLIGHT == 0 ) && ( $FLASHLIGHTSHADOWS == 1 )
// ( $CUBEMAP == 1 ) && ( $FLASHLIGHT == 1 )
// ( $FLASHLIGHT == 1) && ( $CSM == 1)
// $CUBEMAP_SPHERE_LEGACY && ($CUBEMAP == 0)
// ($CUBEMAP || FLASHLIGHT )
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
class vertexlitpbr_ps30_Static_Index
{
	unsigned int m_nFLASHLIGHT : 2;
	unsigned int m_nCUBEMAP : 2;
	unsigned int m_nCUBEMAP_SPHERE_LEGACY : 2;
	unsigned int m_nSMOOTHNESS : 2;
#ifdef _DEBUG
	bool m_bFLASHLIGHT : 1;
	bool m_bCUBEMAP : 1;
	bool m_bCUBEMAP_SPHERE_LEGACY : 1;
	bool m_bSMOOTHNESS : 1;
#endif	// _DEBUG
public:
	void SetFLASHLIGHT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFLASHLIGHT = i;
#ifdef _DEBUG
		m_bFLASHLIGHT = true;
#endif	// _DEBUG
	}

	void SetCUBEMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCUBEMAP = i;
#ifdef _DEBUG
		m_bCUBEMAP = true;
#endif	// _DEBUG
	}

	void SetCUBEMAP_SPHERE_LEGACY( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCUBEMAP_SPHERE_LEGACY = i;
#ifdef _DEBUG
		m_bCUBEMAP_SPHERE_LEGACY = true;
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

	vertexlitpbr_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nFLASHLIGHT = 0;
		m_nCUBEMAP = 0;
		m_nCUBEMAP_SPHERE_LEGACY = 0;
		m_nSMOOTHNESS = 0;
#ifdef _DEBUG
		m_bFLASHLIGHT = false;
		m_bCUBEMAP = false;
		m_bCUBEMAP_SPHERE_LEGACY = false;
		m_bSMOOTHNESS = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bFLASHLIGHT && m_bCUBEMAP && m_bCUBEMAP_SPHERE_LEGACY && m_bSMOOTHNESS );
		AssertMsg( !( ( m_nCUBEMAP == 1 ) && ( m_nFLASHLIGHT == 1 ) ), "Invalid combo combination ( ( CUBEMAP == 1 ) && ( FLASHLIGHT == 1 ) )" );
		AssertMsg( !( m_nCUBEMAP_SPHERE_LEGACY && ( m_nCUBEMAP == 0 ) ), "Invalid combo combination ( CUBEMAP_SPHERE_LEGACY && ( CUBEMAP == 0 ) )" );
		return ( 1440 * m_nFLASHLIGHT ) + ( 2880 * m_nCUBEMAP ) + ( 5760 * m_nCUBEMAP_SPHERE_LEGACY ) + ( 11520 * m_nSMOOTHNESS ) + 0;
	}
};

#define shaderStaticTest_vertexlitpbr_ps30 psh_forgot_to_set_static_FLASHLIGHT + psh_forgot_to_set_static_CUBEMAP + psh_forgot_to_set_static_CUBEMAP_SPHERE_LEGACY + psh_forgot_to_set_static_SMOOTHNESS


class vertexlitpbr_ps30_Dynamic_Index
{
	unsigned int m_nWRITEWATERFOGTODESTALPHA : 2;
	unsigned int m_nPIXELFOGTYPE : 2;
	unsigned int m_nNUM_LIGHTS : 3;
	unsigned int m_nWRITE_DEPTH_TO_DESTALPHA : 2;
	unsigned int m_nFLASHLIGHTSHADOWS : 2;
	unsigned int m_nCSM : 2;
	unsigned int m_nCSM_PERF : 2;
	unsigned int m_nLIGHT_PREVIEW : 2;
#ifdef _DEBUG
	bool m_bWRITEWATERFOGTODESTALPHA : 1;
	bool m_bPIXELFOGTYPE : 1;
	bool m_bNUM_LIGHTS : 1;
	bool m_bWRITE_DEPTH_TO_DESTALPHA : 1;
	bool m_bFLASHLIGHTSHADOWS : 1;
	bool m_bCSM : 1;
	bool m_bCSM_PERF : 1;
	bool m_bLIGHT_PREVIEW : 1;
#endif	// _DEBUG
public:
	void SetWRITEWATERFOGTODESTALPHA( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nWRITEWATERFOGTODESTALPHA = i;
#ifdef _DEBUG
		m_bWRITEWATERFOGTODESTALPHA = true;
#endif	// _DEBUG
	}

	void SetPIXELFOGTYPE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPIXELFOGTYPE = i;
#ifdef _DEBUG
		m_bPIXELFOGTYPE = true;
#endif	// _DEBUG
	}

	void SetNUM_LIGHTS( int i )
	{
		Assert( i >= 0 && i <= 4 );
		m_nNUM_LIGHTS = i;
#ifdef _DEBUG
		m_bNUM_LIGHTS = true;
#endif	// _DEBUG
	}

	void SetWRITE_DEPTH_TO_DESTALPHA( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nWRITE_DEPTH_TO_DESTALPHA = i;
#ifdef _DEBUG
		m_bWRITE_DEPTH_TO_DESTALPHA = true;
#endif	// _DEBUG
	}

	void SetFLASHLIGHTSHADOWS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFLASHLIGHTSHADOWS = i;
#ifdef _DEBUG
		m_bFLASHLIGHTSHADOWS = true;
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

	void SetCSM_PERF( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nCSM_PERF = i;
#ifdef _DEBUG
		m_bCSM_PERF = true;
#endif	// _DEBUG
	}

	void SetLIGHT_PREVIEW( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nLIGHT_PREVIEW = i;
#ifdef _DEBUG
		m_bLIGHT_PREVIEW = true;
#endif	// _DEBUG
	}

	vertexlitpbr_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nWRITEWATERFOGTODESTALPHA = 0;
		m_nPIXELFOGTYPE = 0;
		m_nNUM_LIGHTS = 0;
		m_nWRITE_DEPTH_TO_DESTALPHA = 0;
		m_nFLASHLIGHTSHADOWS = 0;
		m_nCSM = 0;
		m_nCSM_PERF = 0;
		m_nLIGHT_PREVIEW = 0;
#ifdef _DEBUG
		m_bWRITEWATERFOGTODESTALPHA = false;
		m_bPIXELFOGTYPE = false;
		m_bNUM_LIGHTS = false;
		m_bWRITE_DEPTH_TO_DESTALPHA = false;
		m_bFLASHLIGHTSHADOWS = false;
		m_bCSM = false;
		m_bCSM_PERF = false;
		m_bLIGHT_PREVIEW = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bWRITEWATERFOGTODESTALPHA && m_bPIXELFOGTYPE && m_bNUM_LIGHTS && m_bWRITE_DEPTH_TO_DESTALPHA && m_bFLASHLIGHTSHADOWS && m_bCSM && m_bCSM_PERF && m_bLIGHT_PREVIEW );
		AssertMsg( !( ( m_nPIXELFOGTYPE == 0 ) && ( m_nWRITEWATERFOGTODESTALPHA != 0 ) ), "Invalid combo combination ( ( PIXELFOGTYPE == 0 ) && ( WRITEWATERFOGTODESTALPHA != 0 ) )" );
		AssertMsg( !( 1 && ( 1 && ( ( m_nPIXELFOGTYPE != 1 ) && m_nWRITEWATERFOGTODESTALPHA ) ) ), "Invalid combo combination ( 1 && ( 1 && ( ( PIXELFOGTYPE != 1 ) && WRITEWATERFOGTODESTALPHA ) ) )" );
		AssertMsg( !( 1 && ( 1 && ( ( m_nPIXELFOGTYPE != 1 ) && m_nWRITEWATERFOGTODESTALPHA ) ) ), "Invalid combo combination ( 1 && ( 1 && ( ( PIXELFOGTYPE != 1 ) && WRITEWATERFOGTODESTALPHA ) ) )" );
		return ( 1 * m_nWRITEWATERFOGTODESTALPHA ) + ( 2 * m_nPIXELFOGTYPE ) + ( 4 * m_nNUM_LIGHTS ) + ( 20 * m_nWRITE_DEPTH_TO_DESTALPHA ) + ( 40 * m_nFLASHLIGHTSHADOWS ) + ( 80 * m_nCSM ) + ( 160 * m_nCSM_PERF ) + ( 480 * m_nLIGHT_PREVIEW ) + 0;
	}
};

#define shaderDynamicTest_vertexlitpbr_ps30 psh_forgot_to_set_dynamic_WRITEWATERFOGTODESTALPHA + psh_forgot_to_set_dynamic_PIXELFOGTYPE + psh_forgot_to_set_dynamic_NUM_LIGHTS + psh_forgot_to_set_dynamic_WRITE_DEPTH_TO_DESTALPHA + psh_forgot_to_set_dynamic_FLASHLIGHTSHADOWS + psh_forgot_to_set_dynamic_CSM + psh_forgot_to_set_dynamic_CSM_PERF + psh_forgot_to_set_dynamic_LIGHT_PREVIEW

