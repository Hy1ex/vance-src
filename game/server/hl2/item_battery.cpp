//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Handling for the suit batteries.
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "hl2_player.h"
#include "basecombatweapon.h"
#include "gamerules.h"
#include "items.h"
#include "engine/IEngineSound.h"

#ifdef VANCE
#include "vance_player.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CItemBattery : public CItem
{
public:
	DECLARE_CLASS( CItemBattery, CItem );

	void Spawn( void )
	{ 
		Precache( );
		SetModel( "models/items/battery.mdl" );
		BaseClass::Spawn( );
	}
	void Precache( void )
	{
		PrecacheModel ("models/items/battery.mdl");

#ifdef VANCE
		PrecacheScriptSound( "ItemBattery.Touch_Suitless" );
#endif

		PrecacheScriptSound( "ItemBattery.Touch" );
	}

	bool MyTouch( CBasePlayer *pPlayer )
	{
		CHL2_Player *pHL2Player = dynamic_cast<CHL2_Player *>( pPlayer );
		if ( !pHL2Player )
			return FALSE;

#ifdef VANCE
		return !pHL2Player->IsSuitEquipped() ? ((CVancePlayer *)pHL2Player)->ApplyBattery( 1.0f, true ) : pHL2Player->ApplyBattery();
#else
		return pHL2Player->ApplyBattery();
#endif
	}
};

LINK_ENTITY_TO_CLASS(item_battery, CItemBattery);
PRECACHE_REGISTER(item_battery);

