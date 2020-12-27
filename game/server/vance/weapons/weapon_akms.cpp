//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		ResistanceGun - hand gun
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "weapons/vance_baseweapon_shared.h"
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

ConVar sk_akms_damage("sk_akms_damage", "15", FCVAR_REPLICATED);

class CWeaponAKMS : public CVanceMachineGun
{
	DECLARE_CLASS(CWeaponAKMS, CVanceMachineGun);
	
public:
	virtual void PrimaryAttack();
	virtual Activity GetPrimaryAttackActivity();
};

LINK_ENTITY_TO_CLASS(weapon_akms, CWeaponAKMS);
PRECACHE_WEAPON_REGISTER(weapon_akms);


void CWeaponAKMS::PrimaryAttack()
{
	if (m_iClip1 <= 0)
	{
		WeaponSound(EMPTY);
		SendWeaponAnim(ACT_VM_DRYFIRE);
		return;
	}

	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (!pOwner)
	{
		return;
	}

	pOwner->ViewPunchReset();

	CSoundEnt::InsertSound(SOUND_COMBAT | SOUND_CONTEXT_GUNFIRE, pOwner->GetAbsOrigin(), SOUNDENT_VOLUME_MACHINEGUN, 0.2, pOwner, SOUNDENT_CHANNEL_WEAPON, pOwner->GetEnemy());
	Vector vecShootOrigin = pOwner->Weapon_ShootPosition();
	QAngle angShootDir = pOwner->EyeAngles();
	Vector vecShootDir;
	AngleVectors(angShootDir, &vecShootDir);

	FireBulletProjectiles(1, vecShootOrigin, vecShootDir, VECTOR_CONE_2DEGREES, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 1, entindex(), -1, sk_akms_damage.GetInt());
	pOwner->DoMuzzleFlash();
	WeaponSound(SINGLE);
	SendWeaponAnim(GetPrimaryAttackActivity());
	//pOwner->ViewPunch(QAngle(-3, random->RandomFloat(-1, 1), 0));
	DoMachineGunKick(pOwner, 0.3f, 10.0f, max(1.0f, m_fFireDuration * 1.75), 5.0f);
	m_iClip1 = m_iClip1 - 1;

	m_nShotsFired++;
	m_iPrimaryAttacks++;
	gamestats->Event_WeaponFired(pOwner, true, GetClassname());

	m_flNextPrimaryAttack = gpGlobals->curtime + GetFireRate();
}

Activity CWeaponAKMS::GetPrimaryAttackActivity(void)
{
	if (m_nShotsFired < 3)
		return ACT_VM_PRIMARYATTACK;

	if (m_nShotsFired < 10)
		return ACT_VM_RECOIL1;

	if (m_nShotsFired < 20)
		return ACT_VM_RECOIL2;

	return ACT_VM_RECOIL3;
}