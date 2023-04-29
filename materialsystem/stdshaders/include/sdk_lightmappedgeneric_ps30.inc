// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// $BUMPMAP2 && $WARPLIGHTING
// $WARPLIGHTING && $DETAILTEXTURE
// $ENVMAPMASK && $BUMPMAP
// $NORMALMAPALPHAENVMAPMASK && $BASEALPHAENVMAPMASK
// $NORMALMAPALPHAENVMAPMASK && $ENVMAPMASK
// $BASEALPHAENVMAPMASK && $ENVMAPMASK
// $BASEALPHAENVMAPMASK && $SELFILLUM
// !$FASTPATH && $FASTPATHENVMAPCONTRAST
// !$FASTPATH && $FASTPATHENVMAPTINT
// !$BUMPMAP && $DIFFUSEBUMPMAP
// !$BUMPMAP && $BUMPMAP2
// !$BUMPMAP2 && $BUMPMASK
// $ENVMAPMASK && $BUMPMAP2
// $BASETEXTURENOENVMAP && ( !$BASETEXTURE2 || !$CUBEMAP )
// $BASETEXTURE2NOENVMAP && ( !$BASETEXTURE2 || !$CUBEMAP )
// $BASEALPHAENVMAPMASK && $BUMPMAP
// $SEAMLESS && $DETAILTEXTURE
// $SEAMLESS && $MASKEDBLENDING
// $BUMPMASK && ( $SEAMLESS || $DETAILTEXTURE || $SELFILLUM || $BASETEXTURENOENVMAP || $BASETEXTURE2 )
// $FANCY_BLENDING && $BUMPMAP && $DETAILTEXTURE
// $BASETEXTURETRANSFORM2 && !$BASETEXTURE2
// $BASETEXTURETRANSFORM2 && $SEAMLESS
// !$FANCY_BLENDING && $MASKEDBLENDING
// $PARALLAXCORRECT && !$CUBEMAP
// $WARPLIGHTING
// $FASTPATH
// $FASTPATHENVMAPCONTRAST
// $BASETEXTURETRANSFORM2
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
// $LIGHTING_PREVIEW

#pragma once
#include "shaderlib/cshader.h"
class sdk_lightmappedgeneric_ps30_Static_Index
{
	unsigned int m_nMASKEDBLENDING : 2;
	unsigned int m_nBASETEXTURE2 : 2;
	unsigned int m_nDETAILTEXTURE : 2;
	unsigned int m_nBUMPMAP : 2;
	unsigned int m_nBUMPMAP2 : 2;
	unsigned int m_nCUBEMAP : 2;
	unsigned int m_nENVMAPMASK : 2;
	unsigned int m_nBASEALPHAENVMAPMASK : 2;
	unsigned int m_nSELFILLUM : 2;
	unsigned int m_nNORMALMAPALPHAENVMAPMASK : 2;
	unsigned int m_nDIFFUSEBUMPMAP : 2;
	unsigned int m_nBASETEXTURENOENVMAP : 2;
	unsigned int m_nBASETEXTURE2NOENVMAP : 2;
	unsigned int m_nWARPLIGHTING : 2;
	unsigned int m_nFANCY_BLENDING : 2;
	unsigned int m_nSEAMLESS : 2;
	unsigned int m_nBUMPMASK : 2;
	unsigned int m_nBASETEXTURETRANSFORM2 : 2;
	unsigned int m_nPARALLAXCORRECT : 2;
#ifdef _DEBUG
	bool m_bMASKEDBLENDING : 1;
	bool m_bBASETEXTURE2 : 1;
	bool m_bDETAILTEXTURE : 1;
	bool m_bBUMPMAP : 1;
	bool m_bBUMPMAP2 : 1;
	bool m_bCUBEMAP : 1;
	bool m_bENVMAPMASK : 1;
	bool m_bBASEALPHAENVMAPMASK : 1;
	bool m_bSELFILLUM : 1;
	bool m_bNORMALMAPALPHAENVMAPMASK : 1;
	bool m_bDIFFUSEBUMPMAP : 1;
	bool m_bBASETEXTURENOENVMAP : 1;
	bool m_bBASETEXTURE2NOENVMAP : 1;
	bool m_bWARPLIGHTING : 1;
	bool m_bFANCY_BLENDING : 1;
	bool m_bSEAMLESS : 1;
	bool m_bBUMPMASK : 1;
	bool m_bBASETEXTURETRANSFORM2 : 1;
	bool m_bPARALLAXCORRECT : 1;
#endif	// _DEBUG
public:
	void SetMASKEDBLENDING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMASKEDBLENDING = i;
#ifdef _DEBUG
		m_bMASKEDBLENDING = true;
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

	void SetDETAILTEXTURE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDETAILTEXTURE = i;
#ifdef _DEBUG
		m_bDETAILTEXTURE = true;
#endif	// _DEBUG
	}

	void SetBUMPMAP( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nBUMPMAP = i;
#ifdef _DEBUG
		m_bBUMPMAP = true;
#endif	// _DEBUG
	}

	void SetBUMPMAP2( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPMAP2 = i;
#ifdef _DEBUG
		m_bBUMPMAP2 = true;
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

	void SetENVMAPMASK( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nENVMAPMASK = i;
#ifdef _DEBUG
		m_bENVMAPMASK = true;
#endif	// _DEBUG
	}

	void SetBASEALPHAENVMAPMASK( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBASEALPHAENVMAPMASK = i;
#ifdef _DEBUG
		m_bBASEALPHAENVMAPMASK = true;
#endif	// _DEBUG
	}

	void SetSELFILLUM( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSELFILLUM = i;
#ifdef _DEBUG
		m_bSELFILLUM = true;
#endif	// _DEBUG
	}

	void SetNORMALMAPALPHAENVMAPMASK( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nNORMALMAPALPHAENVMAPMASK = i;
#ifdef _DEBUG
		m_bNORMALMAPALPHAENVMAPMASK = true;
#endif	// _DEBUG
	}

	void SetDIFFUSEBUMPMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDIFFUSEBUMPMAP = i;
#ifdef _DEBUG
		m_bDIFFUSEBUMPMAP = true;
#endif	// _DEBUG
	}

	void SetBASETEXTURENOENVMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBASETEXTURENOENVMAP = i;
#ifdef _DEBUG
		m_bBASETEXTURENOENVMAP = true;
#endif	// _DEBUG
	}

	void SetBASETEXTURE2NOENVMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBASETEXTURE2NOENVMAP = i;
#ifdef _DEBUG
		m_bBASETEXTURE2NOENVMAP = true;
#endif	// _DEBUG
	}

	void SetWARPLIGHTING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nWARPLIGHTING = i;
#ifdef _DEBUG
		m_bWARPLIGHTING = true;
#endif	// _DEBUG
	}

	void SetFANCY_BLENDING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFANCY_BLENDING = i;
#ifdef _DEBUG
		m_bFANCY_BLENDING = true;
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

	void SetBUMPMASK( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPMASK = i;
#ifdef _DEBUG
		m_bBUMPMASK = true;
#endif	// _DEBUG
	}

	void SetBASETEXTURETRANSFORM2( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBASETEXTURETRANSFORM2 = i;
#ifdef _DEBUG
		m_bBASETEXTURETRANSFORM2 = true;
#endif	// _DEBUG
	}

	void SetPARALLAXCORRECT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPARALLAXCORRECT = i;
#ifdef _DEBUG
		m_bPARALLAXCORRECT = true;
#endif	// _DEBUG
	}

	sdk_lightmappedgeneric_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nMASKEDBLENDING = 0;
		m_nBASETEXTURE2 = 0;
		m_nDETAILTEXTURE = 0;
		m_nBUMPMAP = 0;
		m_nBUMPMAP2 = 0;
		m_nCUBEMAP = 0;
		m_nENVMAPMASK = 0;
		m_nBASEALPHAENVMAPMASK = 0;
		m_nSELFILLUM = 0;
		m_nNORMALMAPALPHAENVMAPMASK = 0;
		m_nDIFFUSEBUMPMAP = 0;
		m_nBASETEXTURENOENVMAP = 0;
		m_nBASETEXTURE2NOENVMAP = 0;
		m_nWARPLIGHTING = 0;
		m_nFANCY_BLENDING = 0;
		m_nSEAMLESS = 0;
		m_nBUMPMASK = 0;
		m_nBASETEXTURETRANSFORM2 = 0;
		m_nPARALLAXCORRECT = 0;
#ifdef _DEBUG
		m_bMASKEDBLENDING = false;
		m_bBASETEXTURE2 = false;
		m_bDETAILTEXTURE = false;
		m_bBUMPMAP = false;
		m_bBUMPMAP2 = false;
		m_bCUBEMAP = false;
		m_bENVMAPMASK = false;
		m_bBASEALPHAENVMAPMASK = false;
		m_bSELFILLUM = false;
		m_bNORMALMAPALPHAENVMAPMASK = false;
		m_bDIFFUSEBUMPMAP = false;
		m_bBASETEXTURENOENVMAP = false;
		m_bBASETEXTURE2NOENVMAP = false;
		m_bWARPLIGHTING = false;
		m_bFANCY_BLENDING = false;
		m_bSEAMLESS = false;
		m_bBUMPMASK = false;
		m_bBASETEXTURETRANSFORM2 = false;
		m_bPARALLAXCORRECT = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bMASKEDBLENDING && m_bBASETEXTURE2 && m_bDETAILTEXTURE && m_bBUMPMAP && m_bBUMPMAP2 && m_bCUBEMAP && m_bENVMAPMASK && m_bBASEALPHAENVMAPMASK && m_bSELFILLUM && m_bNORMALMAPALPHAENVMAPMASK && m_bDIFFUSEBUMPMAP && m_bBASETEXTURENOENVMAP && m_bBASETEXTURE2NOENVMAP && m_bWARPLIGHTING && m_bFANCY_BLENDING && m_bSEAMLESS && m_bBUMPMASK && m_bBASETEXTURETRANSFORM2 && m_bPARALLAXCORRECT );
		AssertMsg( !( m_nBUMPMAP2 && m_nWARPLIGHTING ), "Invalid combo combination ( BUMPMAP2 && WARPLIGHTING )" );
		AssertMsg( !( m_nWARPLIGHTING && m_nDETAILTEXTURE ), "Invalid combo combination ( WARPLIGHTING && DETAILTEXTURE )" );
		AssertMsg( !( m_nENVMAPMASK && m_nBUMPMAP ), "Invalid combo combination ( ENVMAPMASK && BUMPMAP )" );
		AssertMsg( !( m_nNORMALMAPALPHAENVMAPMASK && m_nBASEALPHAENVMAPMASK ), "Invalid combo combination ( NORMALMAPALPHAENVMAPMASK && BASEALPHAENVMAPMASK )" );
		AssertMsg( !( m_nNORMALMAPALPHAENVMAPMASK && m_nENVMAPMASK ), "Invalid combo combination ( NORMALMAPALPHAENVMAPMASK && ENVMAPMASK )" );
		AssertMsg( !( m_nBASEALPHAENVMAPMASK && m_nENVMAPMASK ), "Invalid combo combination ( BASEALPHAENVMAPMASK && ENVMAPMASK )" );
		AssertMsg( !( m_nBASEALPHAENVMAPMASK && m_nSELFILLUM ), "Invalid combo combination ( BASEALPHAENVMAPMASK && SELFILLUM )" );
		AssertMsg( !( !m_nBUMPMAP && m_nDIFFUSEBUMPMAP ), "Invalid combo combination ( !BUMPMAP && DIFFUSEBUMPMAP )" );
		AssertMsg( !( !m_nBUMPMAP && m_nBUMPMAP2 ), "Invalid combo combination ( !BUMPMAP && BUMPMAP2 )" );
		AssertMsg( !( !m_nBUMPMAP2 && m_nBUMPMASK ), "Invalid combo combination ( !BUMPMAP2 && BUMPMASK )" );
		AssertMsg( !( m_nENVMAPMASK && m_nBUMPMAP2 ), "Invalid combo combination ( ENVMAPMASK && BUMPMAP2 )" );
		AssertMsg( !( m_nBASETEXTURENOENVMAP && ( !m_nBASETEXTURE2 || !m_nCUBEMAP ) ), "Invalid combo combination ( BASETEXTURENOENVMAP && ( !BASETEXTURE2 || !CUBEMAP ) )" );
		AssertMsg( !( m_nBASETEXTURE2NOENVMAP && ( !m_nBASETEXTURE2 || !m_nCUBEMAP ) ), "Invalid combo combination ( BASETEXTURE2NOENVMAP && ( !BASETEXTURE2 || !CUBEMAP ) )" );
		AssertMsg( !( m_nBASEALPHAENVMAPMASK && m_nBUMPMAP ), "Invalid combo combination ( BASEALPHAENVMAPMASK && BUMPMAP )" );
		AssertMsg( !( m_nSEAMLESS && m_nDETAILTEXTURE ), "Invalid combo combination ( SEAMLESS && DETAILTEXTURE )" );
		AssertMsg( !( m_nSEAMLESS && m_nMASKEDBLENDING ), "Invalid combo combination ( SEAMLESS && MASKEDBLENDING )" );
		AssertMsg( !( m_nBUMPMASK && ( m_nSEAMLESS || ( m_nDETAILTEXTURE || ( m_nSELFILLUM || ( m_nBASETEXTURENOENVMAP || m_nBASETEXTURE2 ) ) ) ) ), "Invalid combo combination ( BUMPMASK && ( SEAMLESS || ( DETAILTEXTURE || ( SELFILLUM || ( BASETEXTURENOENVMAP || BASETEXTURE2 ) ) ) ) )" );
		AssertMsg( !( m_nFANCY_BLENDING && ( m_nBUMPMAP && m_nDETAILTEXTURE ) ), "Invalid combo combination ( FANCY_BLENDING && ( BUMPMAP && DETAILTEXTURE ) )" );
		AssertMsg( !( m_nBASETEXTURETRANSFORM2 && !m_nBASETEXTURE2 ), "Invalid combo combination ( BASETEXTURETRANSFORM2 && !BASETEXTURE2 )" );
		AssertMsg( !( m_nBASETEXTURETRANSFORM2 && m_nSEAMLESS ), "Invalid combo combination ( BASETEXTURETRANSFORM2 && SEAMLESS )" );
		AssertMsg( !( !m_nFANCY_BLENDING && m_nMASKEDBLENDING ), "Invalid combo combination ( !FANCY_BLENDING && MASKEDBLENDING )" );
		AssertMsg( !( m_nPARALLAXCORRECT && !m_nCUBEMAP ), "Invalid combo combination ( PARALLAXCORRECT && !CUBEMAP )" );
		AssertMsg( !m_nWARPLIGHTING, "Invalid combo combination WARPLIGHTING" );
		AssertMsg( !m_nBASETEXTURETRANSFORM2, "Invalid combo combination BASETEXTURETRANSFORM2" );
		return ( 384 * m_nMASKEDBLENDING ) + ( 768 * m_nBASETEXTURE2 ) + ( 1536 * m_nDETAILTEXTURE ) + ( 3072 * m_nBUMPMAP ) + ( 9216 * m_nBUMPMAP2 ) + ( 18432 * m_nCUBEMAP ) + ( 36864 * m_nENVMAPMASK ) + ( 73728 * m_nBASEALPHAENVMAPMASK ) + ( 147456 * m_nSELFILLUM ) + ( 294912 * m_nNORMALMAPALPHAENVMAPMASK ) + ( 589824 * m_nDIFFUSEBUMPMAP ) + ( 1179648 * m_nBASETEXTURENOENVMAP ) + ( 2359296 * m_nBASETEXTURE2NOENVMAP ) + ( 4718592 * m_nWARPLIGHTING ) + ( 9437184 * m_nFANCY_BLENDING ) + ( 18874368 * m_nSEAMLESS ) + ( 37748736 * m_nBUMPMASK ) + ( 75497472 * m_nBASETEXTURETRANSFORM2 ) + ( 150994944 * m_nPARALLAXCORRECT ) + 0;
	}
};

#define shaderStaticTest_sdk_lightmappedgeneric_ps30 psh_forgot_to_set_static_MASKEDBLENDING + psh_forgot_to_set_static_BASETEXTURE2 + psh_forgot_to_set_static_DETAILTEXTURE + psh_forgot_to_set_static_BUMPMAP + psh_forgot_to_set_static_BUMPMAP2 + psh_forgot_to_set_static_CUBEMAP + psh_forgot_to_set_static_ENVMAPMASK + psh_forgot_to_set_static_BASEALPHAENVMAPMASK + psh_forgot_to_set_static_SELFILLUM + psh_forgot_to_set_static_NORMALMAPALPHAENVMAPMASK + psh_forgot_to_set_static_DIFFUSEBUMPMAP + psh_forgot_to_set_static_BASETEXTURENOENVMAP + psh_forgot_to_set_static_BASETEXTURE2NOENVMAP + psh_forgot_to_set_static_WARPLIGHTING + psh_forgot_to_set_static_FANCY_BLENDING + psh_forgot_to_set_static_SEAMLESS + psh_forgot_to_set_static_BUMPMASK + psh_forgot_to_set_static_BASETEXTURETRANSFORM2 + psh_forgot_to_set_static_PARALLAXCORRECT


class sdk_lightmappedgeneric_ps30_Dynamic_Index
{
	unsigned int m_nFASTPATHENVMAPCONTRAST : 2;
	unsigned int m_nFASTPATH : 2;
	unsigned int m_nWRITEWATERFOGTODESTALPHA : 2;
	unsigned int m_nPIXELFOGTYPE : 2;
	unsigned int m_nWRITE_DEPTH_TO_DESTALPHA : 2;
	unsigned int m_nVANCE_CSM : 2;
	unsigned int m_nVANCE_CSM_PERF : 2;
	unsigned int m_nLIGHTING_PREVIEW : 2;
#ifdef _DEBUG
	bool m_bFASTPATHENVMAPCONTRAST : 1;
	bool m_bFASTPATH : 1;
	bool m_bWRITEWATERFOGTODESTALPHA : 1;
	bool m_bPIXELFOGTYPE : 1;
	bool m_bWRITE_DEPTH_TO_DESTALPHA : 1;
	bool m_bVANCE_CSM : 1;
	bool m_bVANCE_CSM_PERF : 1;
#endif	// _DEBUG
public:
	void SetFASTPATHENVMAPCONTRAST( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFASTPATHENVMAPCONTRAST = i;
#ifdef _DEBUG
		m_bFASTPATHENVMAPCONTRAST = true;
#endif	// _DEBUG
	}

	void SetFASTPATH( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFASTPATH = i;
#ifdef _DEBUG
		m_bFASTPATH = true;
#endif	// _DEBUG
	}

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

	void SetWRITE_DEPTH_TO_DESTALPHA( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nWRITE_DEPTH_TO_DESTALPHA = i;
#ifdef _DEBUG
		m_bWRITE_DEPTH_TO_DESTALPHA = true;
#endif	// _DEBUG
	}

	void SetVANCE_CSM( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVANCE_CSM = i;
#ifdef _DEBUG
		m_bVANCE_CSM = true;
#endif	// _DEBUG
	}

	void SetVANCE_CSM_PERF( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nVANCE_CSM_PERF = i;
#ifdef _DEBUG
		m_bVANCE_CSM_PERF = true;
#endif	// _DEBUG
	}

	void SetLIGHTING_PREVIEW( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nLIGHTING_PREVIEW = i;
	}

	sdk_lightmappedgeneric_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nFASTPATHENVMAPCONTRAST = 0;
		m_nFASTPATH = 0;
		m_nWRITEWATERFOGTODESTALPHA = 0;
		m_nPIXELFOGTYPE = 0;
		m_nWRITE_DEPTH_TO_DESTALPHA = 0;
		m_nVANCE_CSM = 0;
		m_nVANCE_CSM_PERF = 0;
		m_nLIGHTING_PREVIEW = pShaderAPI->GetIntRenderingParameter( INT_RENDERPARM_ENABLE_FIXED_LIGHTING ) != 0 ;
#ifdef _DEBUG
		m_bFASTPATHENVMAPCONTRAST = false;
		m_bFASTPATH = false;
		m_bWRITEWATERFOGTODESTALPHA = false;
		m_bPIXELFOGTYPE = false;
		m_bWRITE_DEPTH_TO_DESTALPHA = false;
		m_bVANCE_CSM = false;
		m_bVANCE_CSM_PERF = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bFASTPATHENVMAPCONTRAST && m_bFASTPATH && m_bWRITEWATERFOGTODESTALPHA && m_bPIXELFOGTYPE && m_bWRITE_DEPTH_TO_DESTALPHA && m_bVANCE_CSM && m_bVANCE_CSM_PERF );
		AssertMsg( !( !m_nFASTPATH && m_nFASTPATHENVMAPCONTRAST ), "Invalid combo combination ( !FASTPATH && FASTPATHENVMAPCONTRAST )" );
		AssertMsg( !m_nFASTPATH, "Invalid combo combination FASTPATH" );
		AssertMsg( !m_nFASTPATHENVMAPCONTRAST, "Invalid combo combination FASTPATHENVMAPCONTRAST" );
		AssertMsg( !( 1 && ( 1 && ( ( m_nPIXELFOGTYPE != 1 ) && m_nWRITEWATERFOGTODESTALPHA ) ) ), "Invalid combo combination ( 1 && ( 1 && ( ( PIXELFOGTYPE != 1 ) && WRITEWATERFOGTODESTALPHA ) ) )" );
		AssertMsg( !( 1 && ( 1 && ( m_nLIGHTING_PREVIEW && m_nFASTPATHENVMAPCONTRAST ) ) ), "Invalid combo combination ( 1 && ( 1 && ( LIGHTING_PREVIEW && FASTPATHENVMAPCONTRAST ) ) )" );
		AssertMsg( !( 1 && ( 1 && ( m_nLIGHTING_PREVIEW && m_nFASTPATH ) ) ), "Invalid combo combination ( 1 && ( 1 && ( LIGHTING_PREVIEW && FASTPATH ) ) )" );
		AssertMsg( !( 1 && ( 1 && ( ( m_nPIXELFOGTYPE != 1 ) && m_nWRITEWATERFOGTODESTALPHA ) ) ), "Invalid combo combination ( 1 && ( 1 && ( ( PIXELFOGTYPE != 1 ) && WRITEWATERFOGTODESTALPHA ) ) )" );
		AssertMsg( !( 1 && ( 1 && ( m_nLIGHTING_PREVIEW && m_nFASTPATHENVMAPCONTRAST ) ) ), "Invalid combo combination ( 1 && ( 1 && ( LIGHTING_PREVIEW && FASTPATHENVMAPCONTRAST ) ) )" );
		AssertMsg( !( 1 && ( 1 && ( m_nLIGHTING_PREVIEW && m_nFASTPATH ) ) ), "Invalid combo combination ( 1 && ( 1 && ( LIGHTING_PREVIEW && FASTPATH ) ) )" );
		AssertMsg( !m_nLIGHTING_PREVIEW, "Invalid combo combination LIGHTING_PREVIEW" );
		return ( 1 * m_nFASTPATHENVMAPCONTRAST ) + ( 2 * m_nFASTPATH ) + ( 4 * m_nWRITEWATERFOGTODESTALPHA ) + ( 8 * m_nPIXELFOGTYPE ) + ( 16 * m_nWRITE_DEPTH_TO_DESTALPHA ) + ( 32 * m_nVANCE_CSM ) + ( 64 * m_nVANCE_CSM_PERF ) + ( 192 * m_nLIGHTING_PREVIEW ) + 0;
	}
};

#define shaderDynamicTest_sdk_lightmappedgeneric_ps30 psh_forgot_to_set_dynamic_FASTPATHENVMAPCONTRAST + psh_forgot_to_set_dynamic_FASTPATH + psh_forgot_to_set_dynamic_WRITEWATERFOGTODESTALPHA + psh_forgot_to_set_dynamic_PIXELFOGTYPE + psh_forgot_to_set_dynamic_WRITE_DEPTH_TO_DESTALPHA + psh_forgot_to_set_dynamic_VANCE_CSM + psh_forgot_to_set_dynamic_VANCE_CSM_PERF
