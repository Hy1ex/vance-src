#include "cbase.h"
#include "gamerules.h"
#include "entities/vance_player.h"
#include "items.h"
#include "in_buttons.h"
#include "engine/IEngineSound.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Small health kit. Heals the player when picked up.
//-----------------------------------------------------------------------------
class CTourniquet : public CItem
{
public:
	DECLARE_CLASS(CTourniquet, CItem);

	void Spawn( void );
	void Precache( void );
	bool MyTouch( CBasePlayer *pPlayer );
};

LINK_ENTITY_TO_CLASS(item_tourniquet, CTourniquet);
PRECACHE_REGISTER(item_tourniquet);


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTourniquet::Spawn( void )
{
	Precache();
	SetModel( "models/items/healthkit.mdl" );

	BaseClass::Spawn();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTourniquet::Precache( void )
{
	PrecacheModel("models/items/healthkit.mdl");
	PrecacheScriptSound( "HealthKit.Touch" );
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPlayer - 
// Output : 
//-----------------------------------------------------------------------------
bool CTourniquet::MyTouch( CBasePlayer *pPlayer )
{
	CVancePlayer *pVancePlayer = (CVancePlayer *)pPlayer;

	if (!pVancePlayer->GiveTourniquet())
	{
		UTIL_Remove(this);
		return true;
	}

	return false;
}