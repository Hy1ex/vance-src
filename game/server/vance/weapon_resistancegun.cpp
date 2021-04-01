//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		ResistanceGun - hand gun
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
#include "gamestats.h"
#include "saverestore.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define BURST_SIZE 2
#define BURST_SOONEST_REFIRE_RATE 0.4 // tap-shoot speed
#define BURST_AUTO_REFIRE_RATE 0.4; // hold-shoot speed
#define BURST_RATE 0.05
#define BURST_DAMAGE 5.0 // per-bullet of the burst damage

#define AUTO_ACCURACY_CONE 0.0 // 100% accurate
#define AUTO_DAMAGE 15.0

//-----------------------------------------------------------------------------
// CWeaponResistanceGun
//-----------------------------------------------------------------------------

class CWeaponResistanceGun : public CBaseVanceWeapon
{
	DECLARE_CLASS(CWeaponResistanceGun, CBaseVanceWeapon);
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
	DECLARE_ACTTABLE();
public:

	CWeaponResistanceGun();

	virtual bool		Deploy();
	virtual bool		Holster(CBaseCombatWeapon* pSwitchingTo);

	virtual void		Spawn();
	virtual Activity	GetDrawActivity();
	void				DryFire();
	
	void				BurstThink();

	virtual void		PrimaryAttack();
	virtual void		SecondaryAttack();

	virtual Activity	GetIdleActivity() { return !m_bFullAutoMode ? ACT_VM_IDLE : ACT_VM_IDLE_EXTENDED; };
	virtual Activity	GetWalkActivity() { return !m_bFullAutoMode ? ACT_VM_WALK : ACT_VM_WALK_EXTENDED; };
	virtual Activity	GetSprintActivity() { return !m_bFullAutoMode ? ACT_VM_SPRINT : ACT_VM_SPRINT_EXTENDED; };

	virtual void		ItemPostFrame();
	void				AddViewKick();

	virtual Activity	GetPrimaryAttackActivity();

	virtual bool		Reload();

private:
	float	m_flSoonestPrimaryAttack;
	float	m_flLastAttackTime;

	// VANCE
	bool	m_bFullAutoMode;
	float	m_flDoneSwitchingMode;

	bool	m_bInBurst;
	int		m_nBurstShot;
	bool	m_bInTransition;
};


IMPLEMENT_SERVERCLASS_ST(CWeaponResistanceGun, DT_WeaponResistanceGun)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_resistancegun, CWeaponResistanceGun);
PRECACHE_WEAPON_REGISTER(weapon_resistancegun);

BEGIN_DATADESC(CWeaponResistanceGun)
	DEFINE_FIELD(m_flSoonestPrimaryAttack, FIELD_TIME),
	DEFINE_FIELD(m_flLastAttackTime, FIELD_TIME),
	DEFINE_FIELD(m_bFullAutoMode, FIELD_BOOLEAN),
	DEFINE_THINKFUNC(BurstThink),
END_DATADESC()

acttable_t CWeaponResistanceGun::m_acttable[] =
{
	{ ACT_IDLE,						ACT_IDLE_PISTOL,				true },
	{ ACT_IDLE_ANGRY,				ACT_IDLE_ANGRY_PISTOL,			true },
	{ ACT_RANGE_ATTACK1,			ACT_RANGE_ATTACK_PISTOL,		true },
	{ ACT_RELOAD,					ACT_RELOAD_PISTOL,				true },
	{ ACT_WALK_AIM,					ACT_WALK_AIM_PISTOL,			true },
	{ ACT_RUN_AIM,					ACT_RUN_AIM_PISTOL,				true },
	{ ACT_GESTURE_RANGE_ATTACK1,	ACT_GESTURE_RANGE_ATTACK_PISTOL,true },
	{ ACT_RELOAD_LOW,				ACT_RELOAD_PISTOL_LOW,			false },
	{ ACT_RANGE_ATTACK1_LOW,		ACT_RANGE_ATTACK_PISTOL_LOW,	false },
	{ ACT_COVER_LOW,				ACT_COVER_PISTOL_LOW,			false },
	{ ACT_RANGE_AIM_LOW,			ACT_RANGE_AIM_PISTOL_LOW,		false },
	{ ACT_GESTURE_RELOAD,			ACT_GESTURE_RELOAD_PISTOL,		false },
	{ ACT_WALK,						ACT_WALK_PISTOL,				false },
	{ ACT_RUN,						ACT_RUN_PISTOL,					false },
	{ ACT_HL2MP_IDLE,                    ACT_HL2MP_IDLE_PISTOL,                    false },
	{ ACT_HL2MP_RUN,                    ACT_HL2MP_RUN_PISTOL,                    false },
	{ ACT_HL2MP_IDLE_CROUCH,            ACT_HL2MP_IDLE_CROUCH_PISTOL,            false },
	{ ACT_HL2MP_WALK_CROUCH,            ACT_HL2MP_WALK_CROUCH_PISTOL,            false },
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,    ACT_HL2MP_GESTURE_RANGE_ATTACK_PISTOL,    false },
	{ ACT_HL2MP_GESTURE_RELOAD,            ACT_HL2MP_GESTURE_RELOAD_PISTOL,        false },
	{ ACT_HL2MP_JUMP,                    ACT_HL2MP_JUMP_PISTOL,                    false },
	{ ACT_RANGE_ATTACK1,                ACT_RANGE_ATTACK_PISTOL,                false },
};
IMPLEMENT_ACTTABLE(CWeaponResistanceGun);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeaponResistanceGun::CWeaponResistanceGun(void)
{
	m_flSoonestPrimaryAttack = gpGlobals->curtime; // stupid hack to make the weapon not fire when you pick it up

	m_fMinRange1 = 24;
	m_fMaxRange1 = 1500;
	m_fMinRange2 = 24;
	m_fMaxRange2 = 200;

	m_bFiresUnderwater = false;

	// VANCE
	m_bFullAutoMode = false;
	m_bInTransition = false;
	m_flDoneSwitchingMode = 0.0f;

	m_bInBurst = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponResistanceGun::Spawn()
{
	m_bInTransition = false;
	m_flDoneSwitchingMode = 0.0f;

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Called when gun is drawn.
//-----------------------------------------------------------------------------
Activity CWeaponResistanceGun::GetDrawActivity(void)
{
	if (m_bFirstDraw && GetVanceWpnData().bHasFirstDrawAnim)
	{
		m_bFirstDraw = false;
		return ACT_VM_FIRSTDRAW;
	}
	else if (m_bFullAutoMode)
	{
		return ACT_VM_DRAW_EXTENDED;
	}
	else
	{
		return ACT_VM_DRAW;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponResistanceGun::DryFire(void)
{
	WeaponSound(EMPTY);
	SendWeaponAnim(ACT_VM_DRYFIRE);

	m_flSoonestPrimaryAttack = gpGlobals->curtime + 0.2f;
	m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();
}

//-----------------------------------------------------------------------------
// Purpose: Handles burst fire
//-----------------------------------------------------------------------------
void CWeaponResistanceGun::BurstThink(void)
{
	m_bInBurst = true;

	if (m_iClip1 <= 0)
	{
		m_nBurstShot = 0;

		DryFire();

		m_bInBurst = false;
		SetNextThink(TICK_NEVER_THINK); // We're out of ammo, end of burst.
		return;
	}

	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();

	Vector vecShootOrigin = pPlayer->Weapon_ShootPosition();
	QAngle angShootDir = pPlayer->EyeAngles();
	Vector vecShootDir;
	AngleVectors(angShootDir, &vecShootDir);
	Vector vecSpread = m_nBurstShot == 0 ? VECTOR_CONE_PRECALCULATED : VECTOR_CONE_4DEGREES;

	FireBulletProjectiles(1, vecShootOrigin, vecShootDir, vecSpread, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 1, -1, 1, BURST_DAMAGE);

	pPlayer->DoMuzzleFlash();

	m_iClip1 = m_iClip1 - 1;

	if (m_nBurstShot < BURST_SIZE)
	{
		m_nBurstShot++;
		SetNextThink(gpGlobals->curtime + BURST_RATE);
		return;
	}
	else
	{
		m_flSoonestPrimaryAttack = gpGlobals->curtime + BURST_SOONEST_REFIRE_RATE;
		m_flNextPrimaryAttack = gpGlobals->curtime + BURST_AUTO_REFIRE_RATE;

		m_nBurstShot = 0;
		m_bInBurst = false;
		SetNextThink(TICK_NEVER_THINK); // We've fired 3 bullets, end of burst.
		return;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponResistanceGun::PrimaryAttack(void)
{
	if (m_iClip1 <= 0)
	{
		DryFire();
		return;
	}

	CBasePlayer *pOwner = ToBasePlayer(GetOwner());

	if (!pOwner)
	{
		return;
	}
	
	pOwner->ViewPunchReset();

	CSoundEnt::InsertSound(SOUND_COMBAT | SOUND_CONTEXT_GUNFIRE, pOwner->GetAbsOrigin(), SOUNDENT_VOLUME_PISTOL, 0.2, pOwner, SOUNDENT_CHANNEL_WEAPON, pOwner->GetEnemy());

	if (m_bFullAutoMode)
	{
		Vector vecShootOrigin = pOwner->Weapon_ShootPosition();
		QAngle angShootDir = pOwner->EyeAngles();
		Vector vecShootDir;
		AngleVectors(angShootDir, &vecShootDir);
		
		SendWeaponAnim(ACT_VM_FIRE_EXTENDED);
		WeaponSound(SINGLE);
		FireBulletProjectiles(1, vecShootOrigin, vecShootDir, VECTOR_CONE_PRECALCULATED, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 1, entindex(), -1, AUTO_DAMAGE);
		pOwner->DoMuzzleFlash();
		m_iClip1 = m_iClip1 - 1;

		m_flNextPrimaryAttack = gpGlobals->curtime + GetFireRate();
		m_flSoonestPrimaryAttack = m_flNextPrimaryAttack;
	}
	else
	{
		// Burst fires are handle almost entirely by our think function
		if (m_bInBurst == false)
		{
			SendWeaponAnim(ACT_VM_PRIMARYATTACK);
			WeaponSound(BURST);
			SetThink(&CWeaponResistanceGun::BurstThink);
			SetNextThink(gpGlobals->curtime);
		}
	}

	m_iPrimaryAttacks++;
	gamestats->Event_WeaponFired(pOwner, true, GetClassname());
}

//-----------------------------------------------------------------------------
// Purpose: Used for switching between fire modes (Semi / Full Auto)
// TODO: Remove ClientPrints(...), used for debugging
//-----------------------------------------------------------------------------
void CWeaponResistanceGun::SecondaryAttack(void)
{
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer == NULL)
		return;

	if (m_bFullAutoMode)
	{
		m_bFullAutoMode = false;
		m_bInTransition = true;
		SendWeaponAnim(ACT_VM_RETRACT);
	}
	else
	{
		m_bFullAutoMode = true;
		m_bInTransition = true;
		SendWeaponAnim(ACT_VM_EXTEND);
	}

	m_flDoneSwitchingMode = gpGlobals->curtime + GetViewModelSequenceDuration();

	m_flNextSecondaryAttack = gpGlobals->curtime + GetViewModelSequenceDuration();
	m_flSoonestPrimaryAttack = m_flNextSecondaryAttack;
}

//-----------------------------------------------------------------------------
// Purpose: Allows firing as fast as button is pressed
//-----------------------------------------------------------------------------
void CWeaponResistanceGun::ItemPostFrame(void)
{
	//BaseClass::ItemPostFrame();

	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());
	if (!pPlayer)
		return;

	if (m_bInTransition && (m_flNextSecondaryAttack > gpGlobals->curtime))
		return;

	CheckReload();

	if ((((pPlayer->m_afButtonPressed & IN_RELOAD) && (m_iClip1 < GetMaxClip1())) || (m_iClip1 == 0)) && !m_bInReload && !m_bInBurst)
	{
		Reload();
		return;
	}

	if ((pPlayer->m_afButtonPressed & IN_ATTACK) && (m_flSoonestPrimaryAttack <= gpGlobals->curtime) && m_bFullAutoMode) // shoot as fast as the player can click
	{
		PrimaryAttack();
		return;
	}
	else if ((pPlayer->m_nButtons & IN_ATTACK) && (m_flNextPrimaryAttack <= gpGlobals->curtime) && !m_bFullAutoMode) // if we're holding shoot at a slower speed
	{
		PrimaryAttack();
		return;
	}
	else if ((pPlayer->m_afButtonPressed & IN_ATTACK2) && (m_flNextSecondaryAttack <= gpGlobals->curtime)) // switch modes
	{
		SecondaryAttack();
		return;
	}

	// We always return if we're doing something,
	// so here we're idling
	WeaponIdle();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : int
//-----------------------------------------------------------------------------
Activity CWeaponResistanceGun::GetPrimaryAttackActivity(void)
{
	return m_bFullAutoMode ? ACT_VM_FIRE_EXTENDED : ACT_VM_PRIMARYATTACK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CWeaponResistanceGun::Deploy()
{
	m_flDoneSwitchingMode = 0.0f;

	return BaseClass::Deploy();
}

bool CWeaponResistanceGun::Holster(CBaseCombatWeapon* pSwitchingTo)
{
	if (m_flDoneSwitchingMode > 0.0f && m_flDoneSwitchingMode > gpGlobals->curtime)
	{
		// Still switching mode. Cancel the transition.
		m_bFullAutoMode = !m_bFullAutoMode;
	}
	//hack to get proper holster animation if in secondary attack mode
	bool ret = BaseClass::Holster(pSwitchingTo);
	if (m_bFullAutoMode)
	{
		SendWeaponAnim(ACT_VM_HOLSTER_EXTENDED);
	}
	return ret;
}

bool CWeaponResistanceGun::Reload(void)
{
	bool fRet = m_bFullAutoMode ? 
		DefaultReload(GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD_EXTENDED) : 
		DefaultReload(GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD);

	if (fRet)
	{
		WeaponSound(RELOAD);
	}

	return fRet;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponResistanceGun::AddViewKick(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer == NULL)
		return;

	QAngle	viewPunch;

	viewPunch.x = random->RandomFloat(0.25f, 0.5f);
	viewPunch.y = random->RandomFloat(-.6f, .6f);
	viewPunch.z = 0.0f;

	//Add it to the view punch
	pPlayer->ViewPunch(viewPunch);
}