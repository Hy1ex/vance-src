#pragma once
#include "materialsystem/ishaderapi.h"
#include "../../materialsystem/ishadersystem.h"

struct ShaderRenderState_t;

class IShaderSystemInternal : public IShaderInit, public IShaderSystem
{
public:
	virtual void		Init() = 0;
	virtual void		Shutdown() = 0;
	virtual void		ModInit() = 0;
	virtual void		ModShutdown() = 0;
	virtual bool		LoadShaderDLL( const char *pFullPath ) = 0;
	virtual void		UnloadShaderDLL( const char *pFullPath ) = 0;
	virtual IShader		*FindShader( char const* pShaderName ) = 0;
	virtual const char	*ShaderStateString( int i ) const = 0;
	virtual int			ShaderStateCount( ) const = 0;
	virtual void		CreateDebugMaterials() = 0;
	virtual void		CleanUpDebugMaterials() = 0;
	virtual void		InitShaderParameters( IShader *pShader, IMaterialVar **params, const char *pMaterialName ) = 0;
	virtual void		InitShaderInstance( IShader *pShader, IMaterialVar **params, const char *pMaterialName, const char *pTextureGroupName ) = 0;
	virtual bool		InitRenderState( IShader *pShader, int numParams, IMaterialVar **params, ShaderRenderState_t* pRenderState, char const* pMaterialName ) = 0;
	virtual void		CleanupRenderState( ShaderRenderState_t* pRenderState ) = 0;

	virtual void		DrawElements( IShader *pShader, IMaterialVar **params, ShaderRenderState_t* pShaderState, VertexCompressionType_t vertexCompression,
									  uint32 nMaterialVarTimeStamp ) = 0;

	virtual int			ShaderCount() const = 0;
	virtual int			GetShaders( int nFirstShader, int nCount, IShader **ppShaderList ) const = 0;
};

#define SHADERSYSTEM_INTERFACE_VERSION "ShaderSystem002"


