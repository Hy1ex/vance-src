#pragma once
#include "shaderlib/cshader.h"
class sdk_eyeglint_ps30_Static_Index
{
public:
	sdk_eyeglint_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_sdk_eyeglint_ps30 1


class sdk_eyeglint_ps30_Dynamic_Index
{
public:
	sdk_eyeglint_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_sdk_eyeglint_ps30 1

