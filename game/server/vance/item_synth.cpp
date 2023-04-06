//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
/*

===== item_suit.cpp ========================================================

  handling for the player's synth suit.
*/

#include "cbase.h"
#include "vance_player.h"
//#include "player.h"
#include "gamerules.h"
#include "items.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define SF_SUIT_SHORTLOGON		0x0001

class CItemSynth : public CItem
{
public:
	DECLARE_CLASS( CItemSynth, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/synthsuit.mdl" );
		BaseClass::Spawn( );
		
		CollisionProp()->UseTriggerBounds( false, 0 );
	}
	void Precache( void )
	{
		PrecacheModel ("models/items/synthsuit.mdl");
	}
	bool MyTouch( CBasePlayer *pPlayer)
	{
		if (((CVancePlayer*)pPlayer)->IsSynthEquipped())
			return FALSE;

		if ( m_spawnflags & SF_SUIT_SHORTLOGON )
			UTIL_EmitSoundSuit(pPlayer->edict(), "!HEV_A0");		// short version of suit logon,
		else
			UTIL_EmitSoundSuit(pPlayer->edict(), "!HEV_AAx");	// long version of suit logon

		((CVancePlayer*)pPlayer)->EquipSynth(true);
				
		return true;
	}
};

LINK_ENTITY_TO_CLASS(item_synth, CItemSynth);
