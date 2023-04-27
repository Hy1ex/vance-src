#include "cbase.h"
#include "IDeferredExt.h"
#include "shader_override.h"
#include "tier0/memdbgon.h"

CSysModule *g_pDeferredShaderModule = nullptr;
static IDeferredExtension *g_pDeferredExtension = nullptr;
IDeferredExtension *GetDeferredExt()
{
	return g_pDeferredExtension;
}

CUtlString GetModulePath( const char *pszModuleName, const char *pszPathId );

bool ConnectDeferredExt()
{
	OverrideShaders();
	
	CUtlString modulePath = GetModulePath( "game_shader", "GAMEBIN" );

	if (!Sys_LoadInterface(modulePath, DEFERRED_EXTENSION_VERSION, &g_pDeferredShaderModule, reinterpret_cast<void**>(&g_pDeferredExtension)))
		Warning("Unable to pull IDeferredExtension interface from game_shader_dx9.dll.\n");
	else
		return g_pDeferredExtension != NULL;

	return g_pDeferredExtension != NULL;
}

void ShutdownDeferredExt()
{
	if (!g_pDeferredExtension)
		return;

	//__g_defExt->CommitLightData_Common(NULL);

	g_pDeferredExtension = NULL;

	Sys_UnloadModule(g_pDeferredShaderModule);
}