// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH

#pragma once
#include "shaderlib/cshader.h"
class sdk_shatteredglass_vs30_Static_Index
{
	unsigned int m_nENVMAP_MASK : 2;
#ifdef _DEBUG
	bool m_bENVMAP_MASK : 1;
#endif	// _DEBUG
public:
	void SetENVMAP_MASK( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nENVMAP_MASK = i;
#ifdef _DEBUG
		m_bENVMAP_MASK = true;
#endif	// _DEBUG
	}

	sdk_shatteredglass_vs30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nENVMAP_MASK = 0;
#ifdef _DEBUG
		m_bENVMAP_MASK = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bENVMAP_MASK );
		return ( 1 * m_nENVMAP_MASK ) + 0;
	}
};

#define shaderStaticTest_sdk_shatteredglass_vs30 vsh_forgot_to_set_static_ENVMAP_MASK


class sdk_shatteredglass_vs30_Dynamic_Index
{
public:
	sdk_shatteredglass_vs30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_sdk_shatteredglass_vs30 1

