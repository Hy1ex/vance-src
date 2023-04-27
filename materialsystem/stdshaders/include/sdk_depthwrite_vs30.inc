// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// $DECAL && $MORPHING == 0
// $MORPHING

#pragma once
#include "shaderlib/cshader.h"
class sdk_depthwrite_vs30_Static_Index
{
	unsigned int m_nCOLOR_DEPTH : 2;
	unsigned int m_nTREESWAY : 2;
#ifdef _DEBUG
	bool m_bCOLOR_DEPTH : 1;
	bool m_bTREESWAY : 1;
#endif	// _DEBUG
public:
	void SetCOLOR_DEPTH( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCOLOR_DEPTH = i;
#ifdef _DEBUG
		m_bCOLOR_DEPTH = true;
#endif	// _DEBUG
	}

	void SetTREESWAY( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nTREESWAY = i;
#ifdef _DEBUG
		m_bTREESWAY = true;
#endif	// _DEBUG
	}

	sdk_depthwrite_vs30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nCOLOR_DEPTH = 0;
		m_nTREESWAY = 0;
#ifdef _DEBUG
		m_bCOLOR_DEPTH = false;
		m_bTREESWAY = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bCOLOR_DEPTH && m_bTREESWAY );
		return ( 8 * m_nCOLOR_DEPTH ) + ( 16 * m_nTREESWAY ) + 0;
	}
};

#define shaderStaticTest_sdk_depthwrite_vs30 vsh_forgot_to_set_static_COLOR_DEPTH + vsh_forgot_to_set_static_TREESWAY


class sdk_depthwrite_vs30_Dynamic_Index
{
	unsigned int m_nCOMPRESSED_VERTS : 2;
	unsigned int m_nSKINNING : 2;
	unsigned int m_nMORPHING : 2;
#ifdef _DEBUG
	bool m_bCOMPRESSED_VERTS : 1;
	bool m_bSKINNING : 1;
#endif	// _DEBUG
public:
	void SetCOMPRESSED_VERTS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCOMPRESSED_VERTS = i;
#ifdef _DEBUG
		m_bCOMPRESSED_VERTS = true;
#endif	// _DEBUG
	}

	void SetSKINNING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSKINNING = i;
#ifdef _DEBUG
		m_bSKINNING = true;
#endif	// _DEBUG
	}

	void SetMORPHING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMORPHING = i;
	}

	sdk_depthwrite_vs30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nCOMPRESSED_VERTS = 0;
		m_nSKINNING = 0;
		m_nMORPHING = pShaderAPI->IsHWMorphingEnabled() ;
#ifdef _DEBUG
		m_bCOMPRESSED_VERTS = false;
		m_bSKINNING = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bCOMPRESSED_VERTS && m_bSKINNING );
		AssertMsg( !m_nMORPHING, "Invalid combo combination MORPHING" );
		return ( 1 * m_nCOMPRESSED_VERTS ) + ( 2 * m_nSKINNING ) + ( 4 * m_nMORPHING ) + 0;
	}
};

#define shaderDynamicTest_sdk_depthwrite_vs30 vsh_forgot_to_set_dynamic_COMPRESSED_VERTS + vsh_forgot_to_set_dynamic_SKINNING

