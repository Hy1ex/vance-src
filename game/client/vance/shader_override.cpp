#include "cbase.h"
#include "cdll_client_int.h"
#include "filesystem.h"
#include "materialsystem/imaterialsystemhardwareconfig.h"
#include "materialsystem/materialsystem_config.h"
#include "shaderapi/IShaderDevice.h"
#include "hook/IShaderSystemInternal.h"
#include "hook/CHardwareConfig.h"


CUtlString GetModulePath( const char *pszModuleName, const char *pszPathId )
{
	CUtlString modulePath;
	modulePath.SetLength( MAX_PATH );
	filesystem->GetSearchPath( pszPathId, false, modulePath.Get(), MAX_PATH );
	
	V_AppendSlash( modulePath.Get(), MAX_PATH );
	V_strcat( modulePath.Get(), pszModuleName, MAX_PATH );
	V_strcat( modulePath.Get(), DLL_EXT_STRING, MAX_PATH );

	return modulePath;
}

IShaderDevice *ShaderDevice()
{
	static IShaderDevice *pShaderDevice = nullptr;
	if ( !pShaderDevice )
	{
		CSysModule *pModule = nullptr;
		Sys_LoadInterface( "shaderapidx9" DLL_EXT_STRING, SHADER_DEVICE_INTERFACE_VERSION,
			&pModule, reinterpret_cast<void **>( &pShaderDevice ) );
	}
	return pShaderDevice;
}

IShaderSystem *ShaderSystem()
{
	static IShaderSystem *pShaderSystem = nullptr;
	if ( !pShaderSystem )
	{
		CSysModule *pModule = nullptr;
		Sys_LoadInterface( "materialsystem" DLL_EXT_STRING, SHADERSYSTEM_INTERFACE_VERSION,
			&pModule, reinterpret_cast<void **>( &pShaderSystem ) );
	}
	return pShaderSystem;
}

// Get the private shader system interface.
// Needed to override shaders.
IShaderSystemInternal *ShaderSystemInternal()
{
	return static_cast<IShaderSystemInternal *>( ShaderSystem() );
}

// Special class to override any ConVar. It's named
// CCvar as ConVar is already friends with a class
// of such a name.
class CCvar : public ConVar
{
public:

	void Override( const char *pName, const char *pDefaultValue, int flags = 0,
				   const char *pHelpString = NULL, bool bMin = false, float fMin = 0.0,
				   bool bMax = false, float fMax = false, FnChangeCallback_t callback = NULL )
	{
		// Name should be static data
		SetDefault( pDefaultValue );

		// Deallocate what was previously allocated for
		// the cvar we're overriding.
		if ( m_pszString )
		{
			delete[] m_pszString;
		}

		m_StringLength = V_strlen( m_pszDefaultValue ) + 1;
		m_pszString = new char[m_StringLength];
		memcpy( m_pszString, m_pszDefaultValue, m_StringLength );
		
		m_bHasMin = bMin;
		m_fMinVal = fMin;
		m_bHasMax = bMax;
		m_fMaxVal = fMax;
		
		m_fnChangeCallback = callback;

		m_fValue = ( float )atof( m_pszString );
		m_nValue = atoi( m_pszString ); // dont convert from float to int and lose bits
	}

};

void OverrideDxLevel( MaterialSystem_Config_t &videoConfig )
{	
	ConVar *mat_dxlevel = cvar->FindVar( "mat_dxlevel" );
	if ( mat_dxlevel )
	{
		CCvar &override = reinterpret_cast<CCvar &>( *mat_dxlevel );
		override.Override( "mat_dxlevel", MAT_DXLEVEL_OVERRIDE_S,
						   FCVAR_UNREGISTERED, "", true, ABSOLUTE_MINIMUM_DXLEVEL,
						   true, MAT_DXLEVEL_OVERRIDE );

		override.SetValue( MAT_DXLEVEL_OVERRIDE );
	}

	videoConfig.dxSupportLevel = MAT_DXLEVEL_OVERRIDE;
	
	CommandLine()->AppendParm( "-dxlevel", MAT_DXLEVEL_OVERRIDE_S );
}

// - Sets the maximum number of pixel shader constants to 224
// - Force mat_dxlevel to 100
void OverrideHardwareCaps()
{
	if ( g_pMaterialSystemHardwareConfig )
	{
		CHardwareConfig *pHardwareConfig = static_cast<CHardwareConfig *>( g_pMaterialSystemHardwareConfig );
		CHardwareConfig::OverrideHardwareCaps( pHardwareConfig );
	}
		
	MaterialSystem_Config_t videoConfig = materials->GetCurrentConfigForVideoCard();

	// Regardless of whether -vulkan is used or not,
	// the engine still employs some stupid abstract
	// limitations on POSIX, like forcing mat_dxlevel
	// to be between 90 and 92.
	if ( IsPosix() )
	{
		OverrideDxLevel( videoConfig );
	}

	videoConfig.m_nReserved++;
	materials->OverrideConfig( videoConfig, true );
}

// Unloads Valve's shader DLL.
void UnloadShaderDLL()
{
	if ( ShaderSystemInternal() )
	{
		CUtlString modulePath = GetModulePath( "stdshader_dx9", "EXECUTABLE_PATH" );
		ShaderSystemInternal()->UnloadShaderDLL( modulePath );
	}
}

// Loads a shader DLL from gamebin. The loaded
// DLL will override any existing shaders.
void OverrideShaderDLL()
{
	if ( ShaderSystemInternal() )
	{
		CUtlString modulePath = GetModulePath( "game_shader", "GAMEBIN" );
		ShaderSystemInternal()->LoadShaderDLL( modulePath );
	}
}

// Handles all the shader overriding stuff. Ideally
// called before or within modemanager->Init().
void OverrideShaders()
{
	//Msg( "[Shaders] Overwrote stock shaders.\n" );

	//UnloadShaderDLL();

	OverrideHardwareCaps();

	OverrideShaderDLL();
}