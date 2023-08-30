//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		357 - hand gun
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "vance_baseweapon_shared.h"
#include "basecombatcharacter.h"
#include "ai_basenpc.h"
#include "player.h"
#include "gamerules.h"
#include "in_buttons.h"
#include "soundent.h"
#include "game.h"
#include "vstdlib/random.h"
#include "engine/IEngineSound.h"
#include "te_effect_dispatch.h"
#include "gamestats.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// CWeapon357
//-----------------------------------------------------------------------------

class CWeapon357 : public CBaseVanceWeapon
{
	DECLARE_CLASS(CWeapon357, CBaseVanceWeapon);
public:

	CWeapon357( void );

	void		PrimaryAttack();
	void		Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );
	void		ItemPostFrame();
	bool		Reload();

	float		WeaponAutoAimScale()	{ return 0.6f; }

	void		UpdateAds(bool inAds);
	Activity	GetPrimaryAttackActivity(void) { return m_bScoped.Get() ? ACT_VM_FIRE_EXTENDED : ACT_VM_PRIMARYATTACK; };
	Activity	GetIdleActivity() { return m_bScoped.Get() ? ACT_VM_IDLE_EXTENDED : ACT_VM_IDLE; };
	Activity	GetWalkActivity() { return m_bScoped.Get() ? ACT_VM_WALK_EXTENDED : ACT_VM_WALK; };


	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();
	DECLARE_ACTTABLE();
private:

	CNetworkVar(bool, m_bScoped);

};

LINK_ENTITY_TO_CLASS( weapon_357, CWeapon357 );

acttable_t CWeapon357::m_acttable[] =
{
	{ ACT_HL2MP_IDLE,                    ACT_HL2MP_IDLE_PISTOL,                    false },
	{ ACT_HL2MP_RUN,                    ACT_HL2MP_RUN_PISTOL,                    false },
	{ ACT_HL2MP_IDLE_CROUCH,            ACT_HL2MP_IDLE_CROUCH_PISTOL,            false },
	{ ACT_HL2MP_WALK_CROUCH,            ACT_HL2MP_WALK_CROUCH_PISTOL,            false },
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,    ACT_HL2MP_GESTURE_RANGE_ATTACK_PISTOL,    false },
	{ ACT_HL2MP_GESTURE_RELOAD,            ACT_HL2MP_GESTURE_RELOAD_PISTOL,        false },
	{ ACT_HL2MP_JUMP,                    ACT_HL2MP_JUMP_PISTOL,                    false },
	{ ACT_RANGE_ATTACK1,                ACT_RANGE_ATTACK_PISTOL,                false },
};

IMPLEMENT_ACTTABLE(CWeapon357);

PRECACHE_WEAPON_REGISTER( weapon_357 );

IMPLEMENT_SERVERCLASS_ST( CWeapon357, DT_Weapon357 )
SendPropBool(SENDINFO(m_bScoped)),
END_SEND_TABLE()

BEGIN_DATADESC( CWeapon357 )
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeapon357::CWeapon357( void )
{
	m_bReloadsSingly	= false;
	m_bFiresUnderwater	= false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeapon357::Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	switch( pEvent->event )
	{
		case EVENT_WEAPON_RELOAD:
		{
			CEffectData data;

			// Emit six spent shells
			for ( int i = 0; i < 6; i++ )
			{
				data.m_vOrigin = pOwner->WorldSpaceCenter() + RandomVector( -4, 4 );
				data.m_vAngles = QAngle( 90, random->RandomInt( 0, 360 ), 0 );
				data.m_nEntIndex = entindex();

				DispatchEffect( "ShellEject", data );
			}
			break;
		}
		default:
			CBaseCombatWeapon::Operator_HandleAnimEvent(pEvent, pOperator);
			break;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeapon357::PrimaryAttack( void )
{
	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if ( !pPlayer )
	{
		return;
	}

	if ( m_iClip1 <= 0 )
	{
		if ( !m_bFireOnEmpty )
		{
			Reload();
		}
		else
		{
			WeaponSound( EMPTY );
			m_flNextPrimaryAttack = 0.15;
		}

		return;
	}

	m_iPrimaryAttacks++;
	gamestats->Event_WeaponFired( pPlayer, true, GetClassname() );

	WeaponSound( SINGLE );
	pPlayer->DoMuzzleFlash();

	if (m_bScoped.Get()){
		SendWeaponAnim(ACT_VM_FIRE_EXTENDED);
	} else {
		SendWeaponAnim(ACT_VM_PRIMARYATTACK);
	}

	m_flNextPrimaryAttack = gpGlobals->curtime + 0.75;
	m_flNextSecondaryAttack = gpGlobals->curtime + 0.75;

	m_iClip1--;

#ifdef VANCE
	EmitAmmoIndicationSound( m_iClip1, GetMaxClip1() );
#endif

	Vector vecSrc		= pPlayer->Weapon_ShootPosition();
	Vector vecAiming	= pPlayer->GetAutoaimVector( AUTOAIM_SCALE_DEFAULT );	

	FireBulletProjectiles( 1, vecSrc, vecAiming, vec3_origin, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 1 );

	pPlayer->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );

	//Disorient the player
	QAngle angles = pPlayer->GetLocalAngles();

	angles.x += random->RandomInt( -1, 1 );
	angles.y += random->RandomInt( -1, 1 );
	angles.z = 0;

	pPlayer->SnapEyeAngles( angles );

	pPlayer->ViewPunch( QAngle( -8, random->RandomFloat( -2, 2 ), 0 ) );

	CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), 600, 0.2, GetOwner() );

	if ( !m_iClip1 && pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) <= 0 )
	{
		// HEV suit - indicate out of ammo condition
		pPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 ); 
	}
}

//scope stuff

void CWeapon357::ItemPostFrame() {
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
	if (pPlayer) {
		if ((pPlayer->m_afButtonPressed & IN_ATTACK2)) { //it doesnt like if it i try and set the condition directly
			UpdateAds(true);
		} else if ((pPlayer->m_afButtonReleased & IN_ATTACK2)) {
			UpdateAds(false);
		} else {
			UpdateAds(m_bScoped);
		}
	}
	//bandaid fix for it not updating idle properly
	if (m_bScoped.Get() && GetActivity() != ACT_VM_RELOAD &&  GetActivity() != ACT_VM_PRIMARYATTACK &&  GetActivity() != ACT_VM_FIRE_EXTENDED)
		WeaponIdle();
	if (pPlayer && pPlayer->GetAmmoCount(m_iPrimaryAmmoType) <= 0 && (pPlayer->m_nButtons & IN_ATTACK))
		WeaponIdle();
	if (m_bScoped.Get() && GetActivity() == ACT_VM_FIDGET) {
		SendWeaponAnim(ACT_VM_IDLE_EXTENDED);
	}
	BaseClass::ItemPostFrame();
}

bool CWeapon357::Reload() {
	UpdateAds(false);
	return BaseClass::Reload();
}

void CWeapon357::UpdateAds(bool inAds) {
	//check if we are playing any activities where we shouldnt be scoped in
	if (GetActivity() == ACT_VM_HOLSTER || GetActivity() == ACT_VM_CLIMB_HIGH || GetActivity() == ACT_VM_KICK) {
		inAds = false;
	}
	if (inAds != m_bScoped.Get()) {
		CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
		if (inAds) {
			if (pPlayer->SetFOV(this, pPlayer->GetDefaultFOV() - 15.0f, 0.1f)) {
				m_bScoped = true;
				m_bStopSprintSecondary = true;
				if (GetActivity() != ACT_VM_RELOAD &&  GetActivity() != ACT_VM_PRIMARYATTACK &&  GetActivity() != ACT_VM_FIRE_EXTENDED  &&  GetActivity() != ACT_VM_CLIMB_HIGH){
					SendWeaponAnim(ACT_VM_IDLE_EXTENDED);
				}
			}
		}
		else {
			if (pPlayer->SetFOV(this, 0, 0.2f)) {
				m_bScoped = false;
				m_bStopSprintSecondary = false;
				if (GetActivity() != ACT_VM_RELOAD &&  GetActivity() != ACT_VM_PRIMARYATTACK &&  GetActivity() != ACT_VM_FIRE_EXTENDED  &&  GetActivity() != ACT_VM_CLIMB_HIGH){
					SendWeaponAnim(ACT_VM_IDLE);
				}
			}
		}
	}
}