#include "cbase.h"
#include "IDeferredExt.h"
#include "shader_override.h"

#include "tier0/memdbgon.h"

CSysModule *__g_pDeferredShaderModule = NULL;
static IDeferredExtension *__g_defExt = NULL;
IDeferredExtension *GetDeferredExt()
{
	return __g_defExt;
}

CUtlString GetModulePath( const char *pszModuleName, const char *pszPathId );

bool ConnectDeferredExt()
{
	OverrideShaders();
	
	CUtlString modulePath = GetModulePath( "game_shader", "GAMEBIN" );

	if (!Sys_LoadInterface(modulePath, DEFERRED_EXTENSION_VERSION, &__g_pDeferredShaderModule, reinterpret_cast<void**>(&__g_defExt)))
		Warning("Unable to pull IDeferredExtension interface from game_shader_dx9.dll.\n");
	else
		return __g_defExt != NULL;

	return __g_defExt != NULL;
}

void ShutdownDeferredExt()
{
	if (!__g_defExt)
		return;

	//__g_defExt->CommitLightData_Common(NULL);

	__g_defExt = NULL;

	Sys_UnloadModule(__g_pDeferredShaderModule);
}