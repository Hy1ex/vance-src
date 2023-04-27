#pragma once
#include "shaderlib/cshader.h"
class test_ps30_Static_Index
{
public:
	test_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_test_ps30 1


class test_ps30_Dynamic_Index
{
public:
	test_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_test_ps30 1

