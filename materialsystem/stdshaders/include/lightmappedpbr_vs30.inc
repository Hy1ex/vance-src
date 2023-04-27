// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// !$BUMPMAP && $DIFFUSEBUMPMAP
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH

#pragma once
#include "shaderlib/cshader.h"
class lightmappedpbr_vs30_Static_Index
{
	unsigned int m_nBUMPMAP : 2;
	unsigned int m_nDIFFUSEBUMPMAP : 2;
	unsigned int m_nVERTEXCOLOR : 2;
	unsigned int m_nVERTEXALPHATEXBLENDFACTOR : 2;
#ifdef _DEBUG
	bool m_bBUMPMAP : 1;
	bool m_bDIFFUSEBUMPMAP : 1;
	bool m_bVERTEXCOLOR : 1;
	bool m_bVERTEXALPHATEXBLENDFACTOR : 1;
#endif	// _DEBUG
public:
	void SetBUMPMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPMAP = i;
#ifdef _DEBUG
		m_bBUMPMAP = true;
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

	void SetVERTEXCOLOR( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVERTEXCOLOR = i;
#ifdef _DEBUG
		m_bVERTEXCOLOR = true;
#endif	// _DEBUG
	}

	void SetVERTEXALPHATEXBLENDFACTOR( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nVERTEXALPHATEXBLENDFACTOR = i;
#ifdef _DEBUG
		m_bVERTEXALPHATEXBLENDFACTOR = true;
#endif	// _DEBUG
	}

	lightmappedpbr_vs30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nBUMPMAP = 0;
		m_nDIFFUSEBUMPMAP = 0;
		m_nVERTEXCOLOR = 0;
		m_nVERTEXALPHATEXBLENDFACTOR = 0;
#ifdef _DEBUG
		m_bBUMPMAP = false;
		m_bDIFFUSEBUMPMAP = false;
		m_bVERTEXCOLOR = false;
		m_bVERTEXALPHATEXBLENDFACTOR = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bBUMPMAP && m_bDIFFUSEBUMPMAP && m_bVERTEXCOLOR && m_bVERTEXALPHATEXBLENDFACTOR );
		AssertMsg( !( !m_nBUMPMAP && m_nDIFFUSEBUMPMAP ), "Invalid combo combination ( !BUMPMAP && DIFFUSEBUMPMAP )" );
		return ( 4 * m_nBUMPMAP ) + ( 8 * m_nDIFFUSEBUMPMAP ) + ( 16 * m_nVERTEXCOLOR ) + ( 32 * m_nVERTEXALPHATEXBLENDFACTOR ) + 0;
	}
};

#define shaderStaticTest_lightmappedpbr_vs30 vsh_forgot_to_set_static_BUMPMAP + vsh_forgot_to_set_static_DIFFUSEBUMPMAP + vsh_forgot_to_set_static_VERTEXCOLOR + vsh_forgot_to_set_static_VERTEXALPHATEXBLENDFACTOR


class lightmappedpbr_vs30_Dynamic_Index
{
	unsigned int m_nFASTPATH : 2;
	unsigned int m_nDOWATERFOG : 2;
#ifdef _DEBUG
	bool m_bFASTPATH : 1;
	bool m_bDOWATERFOG : 1;
#endif	// _DEBUG
public:
	void SetFASTPATH( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nFASTPATH = i;
#ifdef _DEBUG
		m_bFASTPATH = true;
#endif	// _DEBUG
	}

	void SetDOWATERFOG( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nDOWATERFOG = i;
#ifdef _DEBUG
		m_bDOWATERFOG = true;
#endif	// _DEBUG
	}

	lightmappedpbr_vs30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nFASTPATH = 0;
		m_nDOWATERFOG = 0;
#ifdef _DEBUG
		m_bFASTPATH = false;
		m_bDOWATERFOG = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bFASTPATH && m_bDOWATERFOG );
		return ( 1 * m_nFASTPATH ) + ( 2 * m_nDOWATERFOG ) + 0;
	}
};

#define shaderDynamicTest_lightmappedpbr_vs30 vsh_forgot_to_set_dynamic_FASTPATH + vsh_forgot_to_set_dynamic_DOWATERFOG

