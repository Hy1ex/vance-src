// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH

#pragma once
#include "shaderlib/cshader.h"
class sdk_windowimposter_vs30_Static_Index
{
	unsigned int m_nPARALLAXCORRECT : 2;
#ifdef _DEBUG
	bool m_bPARALLAXCORRECT : 1;
#endif	// _DEBUG
public:
	void SetPARALLAXCORRECT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPARALLAXCORRECT = i;
#ifdef _DEBUG
		m_bPARALLAXCORRECT = true;
#endif	// _DEBUG
	}

	sdk_windowimposter_vs30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nPARALLAXCORRECT = 0;
#ifdef _DEBUG
		m_bPARALLAXCORRECT = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bPARALLAXCORRECT );
		return ( 1 * m_nPARALLAXCORRECT ) + 0;
	}
};

#define shaderStaticTest_sdk_windowimposter_vs30 vsh_forgot_to_set_static_PARALLAXCORRECT


class sdk_windowimposter_vs30_Dynamic_Index
{
public:
	sdk_windowimposter_vs30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_sdk_windowimposter_vs30 1

