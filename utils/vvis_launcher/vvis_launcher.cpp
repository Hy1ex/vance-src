//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
// vvis_launcher.cpp : Defines the entry point for the console application.
//

#ifdef _WIN32
#include "StdAfx.h"
#include <direct.h>
#endif

#include "tier1/strtools.h"
#include "tier0/icommandline.h"
#include "ilaunchabledll.h"
#include "interface.h"


char* GetLastErrorString()
{
#ifdef _WIN32
	static char err[2048];
	
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);

	strncpy( err, (char*)lpMsgBuf, sizeof( err ) );
	LocalFree( lpMsgBuf );

	err[ sizeof( err ) - 1 ] = 0;

	return err;
#else
	return "";
#endif
}


int main(int argc, char* argv[])
{
	CommandLine()->CreateCmdLine( argc, argv );
#ifdef _WIN32
	const char *pDLLName = "vvis_dll.dll";
#else
	const char *pDLLName = "vvis.so";
#endif

	CSysModule *pModule = Sys_LoadModule( pDLLName );
	if ( !pModule )
	{
		printf( "vvis launcher error: can't load %s\n%s", pDLLName, GetLastErrorString() );
		return 1;
	}

	CreateInterfaceFn fn = Sys_GetFactory( pModule );
	if( !fn )
	{
		printf( "vvis launcher error: can't get factory from %s\n", pDLLName );
		Sys_UnloadModule( pModule );
		return 2;
	}

	int retCode = 0;
	ILaunchableDLL *pDLL = (ILaunchableDLL*)fn( LAUNCHABLE_DLL_INTERFACE_VERSION, &retCode );
	if( !pDLL )
	{
		printf( "vvis launcher error: can't get IVVisDLL interface from %s\n", pDLLName );
		Sys_UnloadModule( pModule );
		return 3;
	}

	pDLL->main( argc, argv );
	Sys_UnloadModule( pModule );

	return 0;
}

