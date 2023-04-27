//========= Copyright (C) 2021, CSProMod Team, All rights reserved. =========//
// Written: November 2011
// Author: Saul Rennison

#include "cbase.h"
#include "bspfile.h"
#include "eiface.h"
#include "filesystem.h"
#include "worldlight.h"
#include "vis.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static float Engine_WorldLightDistanceFalloff( const dworldlight_t *wl, const Vector &delta )
{
	float falloff;

	switch ( wl->type )
	{
	case emit_surface:
		// Cull out stuff that's too far
		if ( wl->radius != 0 )
		{
			if ( DotProduct( delta, delta ) > ( wl->radius * wl->radius ) )
				return 0.0f;
		}

		return InvRSquared( delta );

	case emit_skylight:
		return 1.0f;

	case emit_quakelight:
		// X - r;
		falloff = wl->linear_attn - FastSqrt( DotProduct( delta, delta ) );
		if ( falloff < 0 )
			return 0.0f;

		return falloff;

	case emit_skyambient:
		return 1.0f;

	case emit_point:
	case emit_spotlight: // Directional & positional
		{
			float dist2, dist;

			dist2 = DotProduct( delta, delta );
			dist = FastSqrt( dist2 );

			// Cull out stuff that's too far
			if ( wl->radius != 0 && dist > wl->radius )
				return 0.0f;

			return 1.0f / ( wl->constant_attn + wl->linear_attn * dist + wl->quadratic_attn * dist2 );
		}
	}

	return 1.0f;
}

CWorldLights::CWorldLights() : CAutoGameSystem( "World lights" )
{
	m_nWorldLights = 0;
	m_pWorldLights = nullptr;
}

void CWorldLights::Clear()
{
	m_nWorldLights = 0;

	if ( m_pWorldLights )
	{
		delete[] m_pWorldLights;
		m_pWorldLights = nullptr;
	}
}

bool CWorldLights::Init()
{
	return true;
}

void CWorldLights::LevelInitPreEntity()
{
	// Get the map path
	const char *pszMapName = modelinfo->GetModelName( modelinfo->GetModel( 1 ) );

	// Open map
	FileHandle_t hFile = g_pFullFileSystem->Open( pszMapName, "rb" );
	if ( !hFile )
	{
		Warning( "CWorldLights: Unable to open map\n" );
		return;
	}

	// Read the BSP header. We don't need to do any version checks, etc. as we
	// can safely assume that the engine did this for us
	dheader_t hdr;
	g_pFullFileSystem->Read( &hdr, sizeof( hdr ), hFile );

	// Grab the light lump and seek to it
	lump_t &lightLump = hdr.lumps[LUMP_WORLDLIGHTS];

	// If the worldlights lump is empty, that means theres no normal, LDR lights to extract
	// This can happen when, for example, the map is compiled in HDR mode only
	// So move on to the HDR worldlights lump
	if ( lightLump.filelen == 0 )
	{
		lightLump = hdr.lumps[LUMP_WORLDLIGHTS_HDR];
	}

	// If we can't divide the lump data into a whole number of worldlights,
	// then the BSP format changed and we're unaware
	if ( lightLump.filelen % sizeof( dworldlight_t ) )
	{
		Warning( "CWorldLights: Unknown world light lump\n" );

		// Close file
		g_pFullFileSystem->Close( hFile );
		return;
	}

	g_pFullFileSystem->Seek( hFile, lightLump.fileofs, FILESYSTEM_SEEK_HEAD );

	// Allocate memory for the worldlights
	m_nWorldLights = lightLump.filelen / sizeof( dworldlight_t );
	m_pWorldLights = new dworldlight_t[m_nWorldLights];

	// Read worldlights then close
	g_pFullFileSystem->Read( m_pWorldLights, lightLump.filelen, hFile );
	g_pFullFileSystem->Close( hFile );

	DevMsg( "CWorldLights: Load successful (%d lights at 0x%p)\n", m_nWorldLights, m_pWorldLights );
}

bool CWorldLights::GetBrightestLightSource( const Vector &vecPosition, Vector &vecLightPos, Vector &vecLightBrightness )
{
	if ( !m_nWorldLights || !m_pWorldLights )
		return false;

	// Default light position and brightness to zero
	vecLightBrightness.Init();
	vecLightPos.Init();

	// Find the size of the PVS for our current position
	int nCluster = GetClusterForOrigin( vecPosition );
	int nPVSSize = GetPVSForCluster( nCluster, 0, nullptr );

	// Get the PVS at our position
	byte *pvs = new byte[nPVSSize];
	GetPVSForCluster( nCluster, nPVSSize, pvs );

	// Iterate through all the worldlights
	for ( int i = 0; i < m_nWorldLights; ++i )
	{
		dworldlight_t *light = &m_pWorldLights[i];

		// Skip skyambient
		if ( light->type == emit_skyambient )
		{
			continue;
		}

		// Handle sun
		if ( light->type == emit_skylight )
		{
			// Calculate sun position
			Vector vecAbsStart = vecPosition + Vector( 0, 0, 30 );
			Vector vecAbsEnd = vecAbsStart - ( light->normal * MAX_TRACE_LENGTH );

			trace_t tr;
			UTIL_TraceLine( vecPosition, vecAbsEnd, MASK_OPAQUE, nullptr, COLLISION_GROUP_NONE, &tr );

			// If we didn't hit anything then we have a problem
			if ( !tr.DidHit() )
			{
				continue;
			}

			// If we did hit something, and it wasn't the skybox, then skip
			// this worldlight
			if ( !( tr.surface.flags & SURF_SKY ) && !( tr.surface.flags & SURF_SKY2D ) )
			{
				continue;
			}

			// Act like we didn't find any valid worldlights, so the shadow
			// manager uses the default shadow direction instead (should be the
			// sun direction)

			delete[] pvs;

			return false;
		}

		// Calculate square distance to this worldlight
		Vector vecDelta = light->origin - vecPosition;
		float flDistSqr = vecDelta.LengthSqr();
		float flRadiusSqr = light->radius * light->radius;

		// Skip lights that are out of our radius
		if ( flRadiusSqr > 0 && flDistSqr >= flRadiusSqr )
		{
			continue;
		}

		// Is it out of our PVS?
		if ( !CheckOriginInPVS( light->origin, pvs, nPVSSize ) )
		{
			continue;
		}

		// Calculate intensity at our position
		float flRatio = Engine_WorldLightDistanceFalloff( light, vecDelta );
		Vector vecIntensity = light->intensity * flRatio;

		// Is this light more intense than the one we already found?
		if ( vecIntensity.LengthSqr() <= vecLightBrightness.LengthSqr() )
		{
			continue;
		}

		// Can we see the light?
		trace_t tr;
		Vector vecAbsStart = vecPosition + Vector( 0, 0, 30 );
		UTIL_TraceLine( vecAbsStart, light->origin, MASK_OPAQUE, nullptr, COLLISION_GROUP_NONE, &tr );

		if ( tr.DidHit() )
		{
			continue;
		}

		vecLightPos = light->origin;
		vecLightBrightness = vecIntensity;
	}

	delete[] pvs;

	return !vecLightBrightness.IsZero();
}

static CWorldLights s_WorldLights;
CWorldLights *g_pWorldLights = &s_WorldLights;
