#include "cbase.h"
#include "in_buttons.h"
#include "entities/vance_player.h"
#include "weapons/vance_baseweapon_shared.h"

#define SWING_DAMAGE 10.0f
#define SWING_COOLDOWN 0.4f
#define PARRY_COOLDOWN 0.3f

class CWeaponSocketWrench : public CBaseVanceWeapon
{
	DECLARE_CLASS(CWeaponSocketWrench, CBaseVanceWeapon);
public:

	CWeaponSocketWrench();

	virtual void PrimaryAttack();
	virtual void SecondaryAttack();

	virtual void ItemPostFrame();

private:

	float m_flNextPrimaryAttack;
	float m_flNextSecondaryAttack;
};

LINK_ENTITY_TO_CLASS(weapon_socketwrench, CWeaponSocketWrench);

CWeaponSocketWrench::CWeaponSocketWrench()
{
	m_flNextPrimaryAttack = 0.0f;
	m_flNextSecondaryAttack = 0.0f;
}

void CWeaponSocketWrench::PrimaryAttack()
{
	CVancePlayer *pPlayer = static_cast<CVancePlayer *>(GetOwner());
	if (!pPlayer)
		return;

	Vector vecPlayerForward;
	pPlayer->EyeVectors(&vecPlayerForward);

	Vector vecHullCenter = pPlayer->WorldSpaceCenter();
	vecHullCenter + vecPlayerForward * (VEC_HULL_MAX / 2);

	Vector hullMins(-36), hullMaxs(36);

	CTraceFilterHitAll traceFilter;

	trace_t tr;
	UTIL_TraceHull(vecHullCenter, vecHullCenter, hullMins, hullMaxs, MASK_SOLID, &traceFilter, &tr);

	if (tr.DidHit())
	{
		UTIL_ImpactTrace(&tr, DMG_CLUB);

		if (tr.m_pEnt)
		{
			SendWeaponAnim(ACT_VM_SWINGHIT);

			// this very likely is incorrect
			CTakeDamageInfo info(this, GetOwner(), vec3_origin, tr.endpos, SWING_DAMAGE, DMG_CLUB);
			TraceAttack(info, vecPlayerForward, &tr);
		}
		else
		{
			SendWeaponAnim(ACT_VM_SWINGMISS);
		}
	}

	m_flNextPrimaryAttack = gpGlobals->curtime + PARRY_COOLDOWN;
}

void CWeaponSocketWrench::SecondaryAttack()
{
	CVancePlayer *pPlayer = static_cast<CVancePlayer *>(GetOwner());
	if (!pPlayer)
		return;

//	SendWeaponAnim(ACT_VM_PARRY);
	pPlayer->SetParryState(true);

	m_flNextSecondaryAttack = gpGlobals->curtime + 0.3f;
}

void CWeaponSocketWrench::ItemPostFrame()
{
	BaseClass::ItemPostFrame();

	CVancePlayer *pPlayer = static_cast<CVancePlayer *>(GetOwner());
	if (!pPlayer)
		return;

	if (pPlayer->m_nButtons & IN_ATTACK && gpGlobals->curtime >= m_flNextPrimaryAttack)
	{
		PrimaryAttack();
	}
	else if (pPlayer->m_nButtons & IN_ATTACK2 && gpGlobals->curtime >= m_flNextSecondaryAttack)
	{
		SecondaryAttack();
	}
	else
	{
		pPlayer->SetParryState(false);
	}
}