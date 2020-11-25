//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "ammodef.h"
#include "AI_Hint.h"
#include "AI_Navigator.h"
#include "npc_Assassin.h"
#include "game.h"
#include "npcevent.h"
#include "engine/IEngineSound.h"
#include "ai_squad.h"
#include "AI_SquadSlot.h"
#include "ai_moveprobe.h"
#include "ai_scheduleobject.h"
#include "ai_tacticalservices.h"
#include "ai_motor.h"
#include "ai_squadslot.h"
#include "ai_squad.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar	sk_assassin_health("sk_assassin_health", "150");
ConVar	g_debug_assassin("g_debug_assassin", "0");

//=========================================================
// Anim Events	
//=========================================================
#define	ASSASSIN_AE_FIRE_PISTOL_RIGHT	1
#define	ASSASSIN_AE_FIRE_PISTOL_LEFT	2
#define	ASSASSIN_AE_KICK_HIT			3

int AE_ASSASIN_FIRE_PISTOL_RIGHT;
int AE_ASSASIN_FIRE_PISTOL_LEFT;
int AE_ASSASIN_KICK_HIT;

//=========================================================
// Assassin activities
//=========================================================
int ACT_ASSASSIN_FLIP_LEFT;
int ACT_ASSASSIN_FLIP_RIGHT;
int ACT_ASSASSIN_FLIP_BACK;
int ACT_ASSASSIN_FLIP_FORWARD;
int ACT_ASSASSIN_PERCH;

//=========================================================
// Flip types
//=========================================================
enum
{
	FLIP_LEFT,
	FLIP_RIGHT,
	FLIP_FORWARD,
	FLIP_BACKWARD,
	NUM_FLIP_TYPES,
};

//=========================================================
// Private conditions
//=========================================================
enum Assassin_Conds
{
	COND_ASSASSIN_ENEMY_TARGETTING_ME = LAST_SHARED_CONDITION,
};

//=========================================================
// Assassin schedules
//=========================================================
enum
{
	SCHED_ASSASSIN_FIND_VANTAGE_POINT = LAST_SHARED_SCHEDULE,
	SCHED_ASSASSIN_EVADE,
	SCHED_ASSASSIN_STALK_ENEMY,
	SCHED_ASSASSIN_LUNGE,
	SCHED_ASSASSIN_TAKE_COVER,
	SCHED_ASSASSIN_FIRE_PISTOLS,
	SCHED_ASSASSIN_FLANK_COVER,
	SCHED_ASSASSIN_ESTABLISH_LOS,
};

//=========================================================
// Assassin tasks
//=========================================================
enum
{
	TASK_ASSASSIN_GET_PATH_TO_VANTAGE_POINT = LAST_SHARED_TASK,
	TASK_ASSASSIN_EVADE,
	TASK_ASSASSIN_SET_EYE_STATE,
	TASK_ASSASSIN_LUNGE,
	TASK_ASSASSIN_BEGIN_FLANK,
	TASK_ASSASSIN_FLANK_COVER,
};


//-----------------------------------------------------------------------------
// Purpose: Class Constructor
//-----------------------------------------------------------------------------
CNPC_Assassin::CNPC_Assassin(void)
{
}

//-----------------------------------------------------------------------------

LINK_ENTITY_TO_CLASS(npc_assassin, CNPC_Assassin);

#if 0
//---------------------------------------------------------
// Custom Client entity
//---------------------------------------------------------
IMPLEMENT_SERVERCLASS_ST(CNPC_Assassin, DT_NPC_Assassin)
END_SEND_TABLE()

#endif

//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC(CNPC_Assassin)
DEFINE_FIELD(m_nNumFlips, FIELD_INTEGER),
DEFINE_FIELD(m_nLastFlipType, FIELD_INTEGER),
DEFINE_FIELD(m_flNextFlipTime, FIELD_TIME),
DEFINE_FIELD(m_flNextLungeTime, FIELD_TIME),
DEFINE_FIELD(m_flNextShotTime, FIELD_TIME),
DEFINE_FIELD(m_bEvade, FIELD_BOOLEAN),
DEFINE_FIELD(m_bAggressive, FIELD_BOOLEAN),
DEFINE_FIELD(m_bBlinkState, FIELD_BOOLEAN),
DEFINE_FIELD(m_pEyeSprite, FIELD_CLASSPTR),
DEFINE_FIELD(m_pEyeTrail, FIELD_CLASSPTR),
DEFINE_FIELD(m_nShotCount, FIELD_INTEGER),
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//
//
//-----------------------------------------------------------------------------
void CNPC_Assassin::Precache(void)
{
	PrecacheModel("models/fassassin/fassassin.mdl");

	PrecacheScriptSound("Weapon_Pistol.NPC_Single");
	PrecacheScriptSound("Zombie.AttackHit");
	PrecacheScriptSound("Assassin.AttackMiss");
	PrecacheScriptSound("NPC_Assassin.Footstep");

	PrecacheModel("sprites/redglow1.vmt");

	BaseClass::Precache();
}


//-----------------------------------------------------------------------------
// Purpose: 
//
//
//-----------------------------------------------------------------------------
void CNPC_Assassin::Spawn(void)
{
	Precache();

	SetModel("models/fassassin/fassassin.mdl");

	SetHullType(HULL_HUMAN);
	SetHullSizeNormal();

	SetSolid(SOLID_BBOX);
	AddSolidFlags(FSOLID_NOT_STANDABLE);
	SetMoveType(MOVETYPE_STEP);
	SetBloodColor(BLOOD_COLOR_RED);

	m_iHealth = sk_assassin_health.GetFloat();
	m_flFieldOfView = 0.1;
	m_NPCState = NPC_STATE_NONE;
	m_nShotCount = 0;

	CapabilitiesClear();
	CapabilitiesAdd(bits_CAP_MOVE_CLIMB | bits_CAP_MOVE_GROUND | bits_CAP_MOVE_JUMP);
	CapabilitiesAdd(bits_CAP_SQUAD | bits_CAP_AIM_GUN | bits_CAP_INNATE_RANGE_ATTACK1 | bits_CAP_INNATE_RANGE_ATTACK2 | bits_CAP_INNATE_MELEE_ATTACK1);

	//Turn on our guns
	SetBodygroup(1, 1);

	int attachment = LookupAttachment("Eye");

	// Start up the eye glow
	m_pEyeSprite = CSprite::SpriteCreate("sprites/redglow1.vmt", GetLocalOrigin(), false);

	if (m_pEyeSprite != NULL)
	{
		m_pEyeSprite->SetAttachment(this, attachment);
		m_pEyeSprite->SetTransparency(kRenderTransAdd, 255, 255, 255, 200, kRenderFxNone);
		m_pEyeSprite->SetScale(0.25f);
	}

	// Start up the eye trail
	m_pEyeTrail = CSpriteTrail::SpriteTrailCreate("sprites/bluelaser1.vmt", GetLocalOrigin(), false);

	if (m_pEyeTrail != NULL)
	{
		m_pEyeTrail->SetAttachment(this, attachment);
		m_pEyeTrail->SetTransparency(kRenderTransAdd, 255, 0, 0, 200, kRenderFxNone);
		m_pEyeTrail->SetStartWidth(8.0f);
		m_pEyeTrail->SetLifeTime(0.75f);
	}

	NPCInit();

	m_bEvade = false;
	m_bAggressive = false;
}

//-----------------------------------------------------------------------------
// Purpose: Returns true if a reasonable jumping distance
// Input  :
// Output :
//-----------------------------------------------------------------------------
bool CNPC_Assassin::IsJumpLegal(const Vector &startPos, const Vector &apex, const Vector &endPos) const
{
	const float MAX_JUMP_RISE = 256.0f;
	const float MAX_JUMP_DISTANCE = 256.0f;
	const float MAX_JUMP_DROP = 512.0f;

	return BaseClass::IsJumpLegal(startPos, apex, endPos, MAX_JUMP_RISE, MAX_JUMP_DROP, MAX_JUMP_DISTANCE);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : flDot - 
//			flDist - 
// Output : int CNPC_Assassin::MeleeAttack1Conditions
//-----------------------------------------------------------------------------
int CNPC_Assassin::MeleeAttack1Conditions(float flDot, float flDist)
{
	if (flDist > 84)
		return COND_TOO_FAR_TO_ATTACK;

	if (flDot < 0.7f)
		return 0;

	if (GetEnemy() == NULL)
		return 0;

	return COND_CAN_MELEE_ATTACK1;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : flDot - 
//			flDist - 
// Output : int CNPC_Assassin::RangeAttack1Conditions
//-----------------------------------------------------------------------------
int CNPC_Assassin::RangeAttack1Conditions(float flDot, float flDist)
{
	if (GetEnemy() == NULL)
		return COND_NONE;

	if (gpGlobals->curtime < m_flNextAttack)
		return COND_NONE;


	if (flDist < 84)
		return COND_TOO_CLOSE_TO_ATTACK;

	if (flDist > 1024)
		return COND_TOO_FAR_TO_ATTACK;

	if (flDot < 0.5f)
		return COND_NOT_FACING_ATTACK;

	if (HasCondition(COND_WEAPON_SIGHT_OCCLUDED))
		return COND_NONE;

	return COND_CAN_RANGE_ATTACK1;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : flDot - 
//			flDist - 
// Output : int CNPC_Assassin::RangeAttack1Conditions
//-----------------------------------------------------------------------------
int CNPC_Assassin::RangeAttack2Conditions(float flDot, float flDist)
{
	if (m_flNextLungeTime > gpGlobals->curtime)
		return 0;

	float lungeRange = GetSequenceMoveDist(SelectWeightedSequence((Activity)ACT_ASSASSIN_FLIP_FORWARD));

	if (flDist < lungeRange * 0.25f)
		return COND_TOO_CLOSE_TO_ATTACK;

	if (flDist > lungeRange * 1.5f)
		return COND_TOO_FAR_TO_ATTACK;

	if (flDot < 0.75f)
		return COND_NOT_FACING_ATTACK;

	if (GetEnemy() == NULL)
		return 0;

	// Check for a clear path
	trace_t	tr;
	UTIL_TraceHull(GetAbsOrigin(), GetEnemy()->GetAbsOrigin(), GetHullMins(), GetHullMaxs(), MASK_NPCSOLID, this, COLLISION_GROUP_NONE, &tr);

	if (tr.fraction == 1.0f || tr.m_pEnt == GetEnemy())
		return COND_CAN_RANGE_ATTACK2;

	return 0;
}

bool CNPC_Assassin::InnateWeaponLOSCondition(const Vector &ownerPos, const Vector &targetPos, bool bSetConditions)
{

	// --------------------
	// Check for occlusion
	// --------------------
	// Base class version assumes innate weapon position is at eye level
	Vector barrelPos = Weapon_ShootPosition();
	trace_t tr;

	CBaseEntity *pTargetEnt = GetEnemy();
	if (pTargetEnt == NULL)
		return false;

	AI_TraceLine(barrelPos, pTargetEnt->WorldSpaceCenter() + Vector(0, 0, 16), MASK_SHOT, this, COLLISION_GROUP_NONE, &tr);


	//if (GunHasLOS(pTargetEnt->WorldSpaceCenter()) == false)
	//return false;

	if (tr.fraction == 1.0)
	{
		//DevMsg(1, "Weapon free to fire\n");
		return true;
	}

	CBaseEntity *pBE = tr.m_pEnt;
	CBaseCombatCharacter *pBCC = ToBaseCombatCharacter(pBE);
	if (pBE == GetEnemy())
	{
		return true;
	}
	else if (pBCC)
	{
		if (IRelationType(pBCC) == D_HT)
		{
			return true;
		}
		else if (bSetConditions)
		{
			//DevMsg(1, "Weapon blocked by friend\n");
			SetCondition(COND_WEAPON_BLOCKED_BY_FRIEND);
		}
	}
	else if (bSetConditions)
	{
		//DevMsg(1, "Weapon occluded\n");
		SetCondition(COND_WEAPON_SIGHT_OCCLUDED);
		SetEnemyOccluder(pBE);
	}

	return BaseClass::InnateWeaponLOSCondition(ownerPos, targetPos, bSetConditions);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : hand - 
//-----------------------------------------------------------------------------
void CNPC_Assassin::FirePistol(int hand)
{
	//if (m_flNextShotTime > gpGlobals->curtime)
	//	return;

	//m_flNextShotTime = gpGlobals->curtime + random->RandomFloat(0.05f, 0.15f);
	m_nShotCount--;

	Vector	muzzlePos;
	QAngle	muzzleAngle;

	const char *handName = (hand) ? "LeftMuzzle" : "RightMuzzle";

	GetAttachment(handName, muzzlePos, muzzleAngle);

	Vector	muzzleDir;

	if (GetEnemy() == NULL)
	{
		AngleVectors(muzzleAngle, &muzzleDir);
	}
	else
	{
		muzzleDir = GetEnemy()->BodyTarget(muzzlePos) - muzzlePos;
		VectorNormalize(muzzleDir);
	}

	int bulletType = GetAmmoDef()->Index("Pistol");

	FireBullets(1, muzzlePos, muzzleDir, VECTOR_CONE_5DEGREES, 1024, bulletType, 2);

	UTIL_MuzzleFlash(muzzlePos, muzzleAngle, 0.5f, 1);

	CPASAttenuationFilter filter(this);
	EmitSound(filter, entindex(), "Weapon_Pistol.NPC_Single");
}

//---------------------------------------------------------
//---------------------------------------------------------
void CNPC_Assassin::HandleAnimEvent(animevent_t *pEvent)
{

	if (pEvent->event == ASSASSIN_AE_FIRE_PISTOL_RIGHT)
	{
		FirePistol(0);
		return;
	}

	if (pEvent->event == ASSASSIN_AE_FIRE_PISTOL_LEFT)
	{
		FirePistol(1);
		return;
	}

	if (pEvent->event == ASSASSIN_AE_KICK_HIT)
	{
		Vector	attackDir = BodyDirection2D();
		Vector	attackPos = WorldSpaceCenter() + (attackDir * 64.0f);

		trace_t	tr;
		UTIL_TraceHull(WorldSpaceCenter(), attackPos, -Vector(8, 8, 8), Vector(8, 8, 8), MASK_SHOT_HULL, this, COLLISION_GROUP_NONE, &tr);

		if ((tr.m_pEnt != NULL) && (tr.DidHitWorld() == false))
		{
			if (tr.m_pEnt->m_takedamage != DAMAGE_NO)
			{
				CTakeDamageInfo info(this, this, 5, DMG_CLUB);
				CalculateMeleeDamageForce(&info, (tr.endpos - tr.startpos), tr.endpos);
				tr.m_pEnt->TakeDamage(info);

				CBasePlayer	*pPlayer = ToBasePlayer(tr.m_pEnt);

				if (pPlayer != NULL)
				{
					//Kick the player angles
					pPlayer->ViewPunch(QAngle(-30, 40, 10));
				}

				EmitSound("Zombie.AttackHit");
				//EmitSound( "Assassin.AttackHit" );
			}
		}
		else
		{
			EmitSound("Assassin.AttackMiss");
			//EmitSound( "Assassin.AttackMiss" );
		}

		return;
	}

	BaseClass::HandleAnimEvent(pEvent);
}

int CNPC_Assassin::GetSoundInterests(void)
{
	return	SOUND_WORLD | SOUND_COMBAT | SOUND_PLAYER | SOUND_DANGER | SOUND_PHYSICS_DANGER | SOUND_BULLET_IMPACT | SOUND_MOVE_AWAY;
}

//-----------------------------------------------------------------------------
// Purpose: Return true if this NPC can hear the specified sound
//-----------------------------------------------------------------------------
bool CNPC_Assassin::QueryHearSound(CSound *pSound)
{
	if (pSound->SoundContext() & SOUND_CONTEXT_COMBINE_ONLY)
		return true;

	if (pSound->SoundContext() & SOUND_CONTEXT_EXCLUDE_COMBINE)
		return false;

	return BaseClass::QueryHearSound(pSound);
}


//---------------------------------------------------------
//---------------------------------------------------------
int CNPC_Assassin::SelectSchedule(void)
{

	switch (m_NPCState)
	{
	case NPC_STATE_IDLE:
	case NPC_STATE_ALERT:
	{
		if (HasCondition(COND_HEAR_DANGER))
			return SCHED_TAKE_COVER_FROM_BEST_SOUND;

		if (HasCondition(COND_HEAR_COMBAT))
			return SCHED_INVESTIGATE_SOUND;
	}
	break;

	case NPC_STATE_COMBAT:
	{
		int nSched = SelectSchedObject();
		if (nSched != SCHED_NONE)
			return nSched;
	}
	break;
	}

	return BaseClass::SelectSchedule();
}

int CNPC_Assassin::SelectSchedObject()
{
	//Check health 
	/*
	float healthperc = (float)GetHealth() / GetMaxHealth();
	healthperc = clamp(healthperc, 0.0f, 1.0f);
	Vector vecEnemy = GetEnemy()->GetAbsOrigin();
	float flDist = (vecEnemy - GetAbsOrigin()).Length();

	bool IsClosest = true;
	if (m_pSquad)
	{
	if (m_pSquad->GetSquadMemberNearestTo(vecEnemy) != this)
	IsClosest = false;
	}
	*/

	CScheduleObject ShootGun;
	ShootGun.m_iSchedType = SCHED_ASSASSIN_FIRE_PISTOLS;
	//ShootGun.m_iSchedType = SCHED_RANGE_ATTACK1;
	ShootGun.m_bCondition = HasCondition(COND_CAN_RANGE_ATTACK1) && (!HasCondition(COND_ASSASSIN_ENEMY_TARGETTING_ME) || !HasCondition(COND_LIGHT_DAMAGE));
	ShootGun.m_bSquadslot = OccupyStrategySlotRange(SQUAD_SLOT_ATTACK1, SQUAD_SLOT_ATTACK2);
	ShootGun.m_fSchedPriority = 10.0f;

	CScheduleObject Kick;
	Kick.m_iSchedType = SCHED_MELEE_ATTACK1;
	Kick.m_bCondition = HasCondition(COND_CAN_MELEE_ATTACK1);
	Kick.m_fSchedPriority = 20.0f;

	CScheduleObject Lunge;
	Lunge.m_iSchedType = SCHED_ASSASSIN_LUNGE;
	Lunge.m_bCondition = HasCondition(COND_CAN_RANGE_ATTACK2);
	Lunge.m_fSchedPriority = 15.0f;


	CScheduleObject EstablishLOS;
	EstablishLOS.m_iSchedType = SCHED_ASSASSIN_ESTABLISH_LOS;
	EstablishLOS.m_bCondition = !HasCondition(COND_SEE_ENEMY) || HasCondition(COND_WEAPON_SIGHT_OCCLUDED);
	EstablishLOS.m_bSquadslot = OccupyStrategySlotRange(SQUAD_SLOT_ATTACK1, SQUAD_SLOT_ATTACK2);
	EstablishLOS.m_fSchedPriority = 12.0f;

	CScheduleObject FaceEnemy;
	FaceEnemy.m_iSchedType = SCHED_COMBAT_FACE;
	FaceEnemy.m_bCondition = HasCondition(COND_SEE_ENEMY) && !HasCondition(COND_CAN_RANGE_ATTACK1) && gpGlobals->curtime > m_flNextAttack;
	FaceEnemy.m_fSchedPriority = 5.0f;


	CScheduleObject TakeCover;
	TakeCover.m_iSchedType = SCHED_ASSASSIN_TAKE_COVER;
	TakeCover.m_bCondition = !HasMemory(bits_MEMORY_INCOVER) || HasCondition(COND_SEE_ENEMY);
	TakeCover.m_fSchedPriority = (HasCondition(COND_LIGHT_DAMAGE) || HasCondition(COND_HEAVY_DAMAGE)) ? 15.0f : 5.0f;

	CScheduleObject Evade;
	Evade.m_iSchedType = SCHED_ASSASSIN_EVADE;
	Evade.m_bCondition = HasCondition(COND_SEE_ENEMY) && m_nNumFlips <= 0;
	Evade.m_fSchedPriority = 10.0f;

	CScheduleObject FindVantagePoint;
	FindVantagePoint.m_iSchedType = SCHED_ASSASSIN_FIND_VANTAGE_POINT;
	FindVantagePoint.m_bSquadslot = !HasCondition(COND_SEE_ENEMY);
	FindVantagePoint.m_fSchedPriority = 15.0f;

	CScheduleObject TakeFlankCover;
	TakeFlankCover.m_iSchedType = SCHED_ASSASSIN_FLANK_COVER;
	TakeFlankCover.m_bCondition = !HasCondition(COND_SEE_ENEMY);
	TakeFlankCover.m_fSchedPriority = random->RandomFloat(9.0f, 12.0f);


	int FinalSched = SCHED_NONE;
	int SchedPriority = 0;


	struct GoalTable
	{
		bool GoalSet;
		CScheduleObject * GoalArray;
		int ArraySize;
	};

	CScheduleObject GoalEngageEnemy[] = { ShootGun, Kick, Lunge };
	CScheduleObject GoalSeekEnemy[] = { EstablishLOS, FindVantagePoint };
	CScheduleObject GoalEvadeEnemy[] = { TakeCover, Evade, };

	bool CanSeek = m_flNextAttack < gpGlobals->curtime;

	GoalTable AvailableGoals[] =
	{
		true, GoalEngageEnemy, ARRAYSIZE(GoalEngageEnemy),
		CanSeek, GoalSeekEnemy, ARRAYSIZE(GoalSeekEnemy),
		true, GoalEvadeEnemy, ARRAYSIZE(GoalEvadeEnemy),
	};


	for (int i = 0; i < ARRAYSIZE(AvailableGoals); i++)
	{

		if (AvailableGoals[i].GoalSet == true)
		{

			for (int v = 0; v < AvailableGoals[i].ArraySize; v++)
			{
				CScheduleObject GoalSched = AvailableGoals[i].GoalArray[v];

				if (GoalSched.m_bCondition && GoalSched.m_fSchedPriority > SchedPriority && GoalSched.m_bSquadslot)
				{
					SchedPriority = GoalSched.m_fSchedPriority;
					FinalSched = GoalSched.m_iSchedType;
				}
			}
		}

		if (FinalSched > 0)
		{
			break;
		}
	}

	return FinalSched;

	/*
	for (CScheduleObject n : Schedules)
	{
	if (n.m_pCondition == true && n.m_fSchedPriority > SchedPriority)
	{
	FinalSched = n.m_iSchedType;
	SchedPriority = n.m_fSchedPriority;
	}
	}
	*/
}

int CNPC_Assassin::SelectFailSchedule(int failedSchedule, int failedTask, AI_TaskFailureCode_t taskFailCode)
{
	if (failedSchedule == SCHED_ASSASSIN_FIND_VANTAGE_POINT)
	{
		float flDistToEnemy = (GetEnemy()->GetAbsOrigin() - GetAbsOrigin()).Length();
		if (HasCondition(COND_SEE_ENEMY))
		{
			if (OccupyStrategySlotRange(SQUAD_SLOT_ATTACK1, SQUAD_SLOT_ATTACK2) && flDistToEnemy <= 128.0f)
			{
				return SCHED_ASSASSIN_LUNGE;
			}
		}

		return SCHED_ASSASSIN_ESTABLISH_LOS;

	}

	return BaseClass::SelectFailSchedule(failedSchedule, failedTask, taskFailCode);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Assassin::AimGun(void)
{

	// Aim at our target
	if (GetEnemy())
	{
		Vector vecShootOrigin;

		vecShootOrigin = Weapon_ShootPosition();
		Vector vecShootDir;

		// Aim where it is
		vecShootDir = GetShootEnemyDir(vecShootOrigin, false);

		SetAim(vecShootDir);
	}
	else
	{
		RelaxAim();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Assassin::PrescheduleThink(void)
{
	if (GetActivity() == ACT_RUN || GetActivity() == ACT_WALK)
	{
		CPASAttenuationFilter filter(this);

		static int iStep = 0;
		iStep = !iStep;
		if (iStep)
		{
			EmitSound(filter, entindex(), "NPC_Assassin.Footstep");
		}
	}

	if (m_flNextAttack > gpGlobals->curtime)
	{
		m_nNumFlips = random->RandomInt(1, 2);
	}
}

// Purpose: Translate base class activities into combot activites
//-----------------------------------------------------------------------------
Activity CNPC_Assassin::NPC_TranslateActivity(Activity eNewActivity)
{

	if (eNewActivity == ACT_IDLE || eNewActivity == ACT_CROUCH)
	{
		if (m_NPCState == NPC_STATE_COMBAT || m_NPCState == NPC_STATE_ALERT)
		{
			return (Activity)ACT_ASSASSIN_PERCH;
		}
	}

	if (eNewActivity == ACT_ASSASSIN_FLIP_FORWARD)
	{
		m_flNextLungeTime = gpGlobals->curtime + 2.0f;
		m_nLastFlipType = FLIP_FORWARD;
	}



	return BaseClass::NPC_TranslateActivity(eNewActivity);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : right - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CNPC_Assassin::CanFlip(int flipType, Activity &activity, const Vector *avoidPosition)
{
	Vector		testDir;
	Activity	act = ACT_INVALID;

	switch (flipType)
	{
	case FLIP_RIGHT:
		GetVectors(NULL, &testDir, NULL);
		act = NPC_TranslateActivity((Activity)ACT_ASSASSIN_FLIP_RIGHT);
		break;

	case FLIP_LEFT:
		GetVectors(NULL, &testDir, NULL);
		testDir.Negate();
		act = NPC_TranslateActivity((Activity)ACT_ASSASSIN_FLIP_LEFT);
		break;

	case FLIP_FORWARD:
		GetVectors(&testDir, NULL, NULL);
		act = NPC_TranslateActivity((Activity)ACT_ASSASSIN_FLIP_FORWARD);
		break;

	case FLIP_BACKWARD:
		GetVectors(&testDir, NULL, NULL);
		testDir.Negate();
		act = NPC_TranslateActivity((Activity)ACT_ASSASSIN_FLIP_BACK);
		break;

	default:
		assert(0); //NOTENOTE: Invalid flip type
		activity = ACT_INVALID;
		return false;
		break;
	}

	// Make sure we don't flip towards our avoidance position/
	if (avoidPosition != NULL)
	{
		Vector	avoidDir = (*avoidPosition) - GetAbsOrigin();
		VectorNormalize(avoidDir);

		if (DotProduct(avoidDir, testDir) > 0.0f)
			return false;
	}

	int seq = SelectWeightedSequence(act);

	// Find out the length of this sequence
	float	testDist = GetSequenceMoveDist(seq);

	// Find the resulting end position from the sequence's movement
	Vector	endPos = GetAbsOrigin() + (testDir * testDist);

	trace_t	tr;

	if ((flipType != FLIP_BACKWARD) && (avoidPosition != NULL))
	{
		UTIL_TraceLine((*avoidPosition), endPos, MASK_SHOT, this, COLLISION_GROUP_NONE, &tr);

		if (tr.fraction == 1.0f)
			return false;
	}

	/*
	UTIL_TraceHull( GetAbsOrigin(), endPos, NAI_Hull::Mins(m_eHull) + Vector( 0, 0, StepHeight() ), NAI_Hull::Maxs(m_eHull), MASK_NPCSOLID, this, COLLISION_GROUP_NONE, &tr );

	// See if we're hit an obstruction in that direction
	if ( tr.fraction < 1.0f )
	{
	if ( g_debug_assassin.GetBool() )
	{
	NDebugOverlay::BoxDirection( GetAbsOrigin(), NAI_Hull::Mins(m_eHull) + Vector( 0, 0, StepHeight() ), NAI_Hull::Maxs(m_eHull) + Vector( testDist, 0, StepHeight() ), testDir, 255, 0, 0, true, 2.0f );
	}

	return false;
	}

	#define NUM_STEPS 2

	float	stepLength = testDist / NUM_STEPS;

	for ( int i = 1; i <= NUM_STEPS; i++ )
	{
	endPos = GetAbsOrigin() + ( testDir * (stepLength*i) );

	// Also check for a cliff edge
	UTIL_TraceHull( endPos, endPos - Vector( 0, 0, StepHeight() * 4.0f ), NAI_Hull::Mins(m_eHull) + Vector( 0, 0, StepHeight() ), NAI_Hull::Maxs(m_eHull), MASK_NPCSOLID, this, COLLISION_GROUP_NONE, &tr );

	if ( tr.fraction == 1.0f )
	{
	if ( g_debug_assassin.GetBool() )
	{
	NDebugOverlay::BoxDirection( endPos, NAI_Hull::Mins(m_eHull) + Vector( 0, 0, StepHeight() ), NAI_Hull::Maxs(m_eHull) + Vector( StepHeight() * 4.0f, 0, StepHeight() ), Vector(0,0,-1), 255, 0, 0, true, 2.0f );
	}

	return false;
	}
	}

	if ( g_debug_assassin.GetBool() )
	{
	NDebugOverlay::BoxDirection( GetAbsOrigin(), NAI_Hull::Mins(m_eHull) + Vector( 0, 0, StepHeight() ), NAI_Hull::Maxs(m_eHull) + Vector( testDist, 0, StepHeight() ), testDir, 0, 255, 0, true, 2.0f );
	}
	*/

	AIMoveTrace_t moveTrace;
	GetMoveProbe()->TestGroundMove(GetAbsOrigin(), endPos, MASK_NPCSOLID, AITGM_DEFAULT, &moveTrace);

	if (moveTrace.fStatus != AIMR_OK)
		return false;

	// Return the activity to use
	activity = (Activity)act;

	return true;
}

//---------------------------------------------------------
// Purpose: 
//---------------------------------------------------------
void CNPC_Assassin::StartTask(const Task_t *pTask)
{
	switch (pTask->iTask)
	{
	case TASK_ASSASSIN_SET_EYE_STATE:
	{
		SetEyeState((eyeState_t)((int)pTask->flTaskData));
		TaskComplete();
	}
	break;

	case TASK_ASSASSIN_EVADE:
	{
		Activity flipAct = ACT_INVALID;

		const Vector *avoidPos = (GetEnemy() != NULL) ? &(GetEnemy()->GetAbsOrigin()) : NULL;

		for (int i = FLIP_LEFT; i < NUM_FLIP_TYPES; i++)
		{
			if (CanFlip(i, flipAct, avoidPos))
			{
				// Don't flip back to where we just were
				if (((i == FLIP_LEFT) && (m_nLastFlipType == FLIP_RIGHT)) ||
					((i == FLIP_RIGHT) && (m_nLastFlipType == FLIP_LEFT)) ||
					((i == FLIP_FORWARD) && (m_nLastFlipType == FLIP_BACKWARD)) ||
					((i == FLIP_BACKWARD) && (m_nLastFlipType == FLIP_FORWARD)))
				{
					flipAct = ACT_INVALID;
					continue;
				}

				m_nNumFlips--;
				ResetIdealActivity(flipAct);
				m_flNextFlipTime = gpGlobals->curtime + 2.0f;
				m_nLastFlipType = i;
				break;
			}
		}

		if (flipAct == ACT_INVALID)
		{
			m_nNumFlips = 0;
			m_nLastFlipType = -1;
			m_flNextFlipTime = gpGlobals->curtime + 2.0f;
			TaskFail("Unable to find flip evasion direction!\n");
		}
	}
	break;

	case TASK_ASSASSIN_GET_PATH_TO_VANTAGE_POINT:
	{
		assert(GetEnemy() != NULL);
		if (GetEnemy() == NULL)
			break;

		Vector	goalPos;

		CHintCriteria	hint;

		// Find a disadvantage node near the player, but away from ourselves

		Vector VecEnemyOrigin = GetEnemy()->GetAbsOrigin();

		hint.AddIncludePosition(GetAbsOrigin(), 640);
		hint.SetHintType(HINT_TACTICAL_ENEMY_DISADVANTAGED);
		//hint.AddExcludePosition(GetAbsOrigin(), 256);
		hint.AddExcludePosition(VecEnemyOrigin, 128);

		if ((m_pSquad != NULL) && (m_pSquad->NumMembers() > 1))
		{
			AISquadIter_t iter;
			for (CAI_BaseNPC *pSquadMember = m_pSquad->GetFirstMember(&iter); pSquadMember; pSquadMember = m_pSquad->GetNextMember(&iter))
			{
				if (pSquadMember == NULL)
					continue;

				hint.AddExcludePosition(pSquadMember->GetAbsOrigin(), 128);
			}
		}

		hint.SetFlag(bits_HINT_NODE_NEAREST);

		CAI_Hint *pHint = CAI_HintManager::FindHint(this, GetEnemy()->GetAbsOrigin(), hint);

		if (pHint == NULL)
		{
			TaskFail("Unable to find vantage point!\n");
			break;
		}

		pHint->GetPosition(this, &goalPos);

		AI_NavGoal_t goal(goalPos);

		//Try to run directly there
		if (GetNavigator()->SetGoal(goal) == false)
		{
			TaskFail("Unable to find path to vantage point!\n");
			break;
		}

		TaskComplete();
	}
	break;

	case TASK_RANGE_ATTACK1:
	{
		m_nShotCount = random->RandomInt(4, 6);
	}
	break;

	case TASK_ASSASSIN_BEGIN_FLANK:
	{
		if (IsInSquad() && GetSquad()->NumMembers() > 1)
		{
			// Flank relative to the other shooter in our squad.
			// If there's no other shooter, just flank relative to any squad member.
			AISquadIter_t iter;
			CAI_BaseNPC *pNPC = GetSquad()->GetFirstMember(&iter);
			while (pNPC == this)
			{
				pNPC = GetSquad()->GetNextMember(&iter);
			}

			m_vSavePosition = pNPC->GetAbsOrigin();
		}
		else
		{
			// Flank relative to our current position.
			m_vSavePosition = GetAbsOrigin();
		}

		//DevMsg(1, "Began flanking\n");

		TaskComplete();
		break;
	}

	case TASK_ASSASSIN_FLANK_COVER:
	{

		if (!GetEnemy())
		{
			TaskFail(FAIL_NO_ENEMY);
			return;
		}


		Vector vecEnemy = GetEnemies()->LastSeenPosition(GetEnemy());
		Vector vecEnemyEye = vecEnemy + GetEnemy()->GetViewOffset();

		//float flDist = (vecSquadmate - GetAbsOrigin()).Length();
		float flMinRange = 320;
		float flMaxRange = 1024;

		Vector posLos;
		bool found = false;

		if (!found)
		{
			FlankType_t eFlankType = FLANKTYPE_ARC;
			Vector vecFlankRefPos = m_vSavePosition;
			float flFlankParam = pTask->flTaskData;

			if (GetTacticalServices()->FindLos(vecEnemy, vecEnemyEye, flMinRange, flMaxRange, 1.0, eFlankType, vecFlankRefPos, flFlankParam, &posLos))
			{
				found = true;
			}
		}

		if (!found)
		{
			TaskFail(FAIL_NO_SHOOT);
		}


		Vector coverPos;

		if (FindCoverPosInRadius(GetEnemy(), posLos, 192.0f, &coverPos))
		{
			AI_NavGoal_t goal(coverPos, ACT_RUN, AIN_HULL_TOLERANCE);
			GetNavigator()->SetGoal(goal);

			m_vInterruptSavePosition = coverPos;

			m_flMoveWaitFinished = gpGlobals->curtime; //+ pTask->flTaskData;
		}
		else
		{
			// no coverwhatsoever.
			TaskFail(FAIL_NO_COVER);
		}
	}

	break;

	case TASK_WALK_PATH:
	case TASK_RUN_PATH:
		// grunt no longer assumes he is covered if he moves
		Forget(bits_MEMORY_INCOVER);
		BaseClass::StartTask(pTask);
		break;

	default:
		BaseClass::StartTask(pTask);
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//
//
//-----------------------------------------------------------------------------
float CNPC_Assassin::MaxYawSpeed(void)
{
	switch (GetActivity())
	{
	case ACT_TURN_LEFT:
	case ACT_TURN_RIGHT:
		return 160;
		break;
	case ACT_RUN:
		return 900;
		break;
	case ACT_RANGE_ATTACK1:
		return 0;
		break;
	default:
		return 60;
		break;
	}
}


//---------------------------------------------------------
//---------------------------------------------------------
void CNPC_Assassin::RunTask(const Task_t *pTask)
{
	switch (pTask->iTask)
	{
	case TASK_ASSASSIN_EVADE:

		AutoMovement();

		if (IsActivityFinished())
		{
			TaskComplete();
		}

		break;

	case TASK_RANGE_ATTACK1:

		if (m_nShotCount > 0)
		{
			// DevMsg("ACT_RANGE_ATTACK1\n");
			ResetIdealActivity(ACT_RANGE_ATTACK1);
			m_flLastAttackTime = gpGlobals->curtime;
		}

		if (m_nShotCount <= 0)
		{
			DevMsg("Out of shots!\n");
			m_flNextAttack = gpGlobals->curtime + random->RandomFloat(1.25f, 1.75f);
			TaskComplete();
		}

		if (GetTaskInterrupt() > 0)
		{
			m_flNextAttack = gpGlobals->curtime + random->RandomFloat(1.25f, 1.75f);
			TaskInterrupt();
		}
		break;

	case TASK_ASSASSIN_FLANK_COVER:
	{
		//		CBaseEntity *pEnemy = GetEnemy();

		//CBaseEntity *pEnemy = GetEnemy();
		if (!GetEnemy())
		{
			TaskFail(FAIL_NO_ENEMY);
			return;
		}

		if (GetTaskInterrupt() > 0)
		{
			ClearTaskInterrupt();

			Vector vecEnemy = GetEnemies()->LastSeenPosition(GetEnemy());

			AI_NavGoal_t goal(m_vInterruptSavePosition, ACT_RUN, AIN_HULL_TOLERANCE);

			GetNavigator()->SetGoal(goal, AIN_CLEAR_TARGET);
			GetNavigator()->SetArrivalDirection(vecEnemy - goal.dest);
		}
		else
			TaskInterrupt();
	}
	break;

	default:
		BaseClass::RunTask(pTask);
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Get shoot position of BCC at an arbitrary position
// Input  :
// Output :
//-----------------------------------------------------------------------------
Vector CNPC_Assassin::Weapon_ShootPosition()
{
	/*
	Vector	muzzlePos;
	QAngle	muzzleAngle;


	const char *handName = "RightMuzzle";

	GetAttachment(handName, muzzlePos, muzzleAngle);

	return muzzlePos;
	*/

	return GetAbsOrigin() + Vector(0, 0, 36);
}


//---------------------------------------------------------
//---------------------------------------------------------
bool CNPC_Assassin::FValidateHintType(CAI_Hint *pHint)
{
	switch (pHint->HintType())
	{
	case HINT_TACTICAL_ENEMY_DISADVANTAGED:
	{
		Vector	hintPos;
		pHint->GetPosition(this, &hintPos);

		// Verify that we can see the target from that position
		hintPos += GetViewOffset();

		trace_t	tr;
		UTIL_TraceLine(hintPos, GetEnemy()->BodyTarget(hintPos, true), MASK_SHOT, this, COLLISION_GROUP_NONE, &tr);

		// Check for seeing our target at the new location
		if ((tr.fraction == 1.0f) || (tr.m_pEnt != GetEnemy()))
			return false;

		if (GetEnemy() && fabs(GetEnemy()->GetAbsOrigin().z - hintPos.z) < 128)
		{
			return false;
		}

		if (m_pSquad)
		{
			if (m_pSquad->SquadMemberInRange(hintPos, 256.0f))
			{
				return false;
			}
		}

		return true;
		break;
	}

	default:
		return false;
		break;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : const Vector
//-----------------------------------------------------------------------------
const Vector &CNPC_Assassin::GetViewOffset(void)
{
	static Vector eyeOffset = Vector(0, 0, 24.0f);

	//FIXME: Use eye attachment?
	// If we're crouching, offset appropriately
	/*
	if ((GetActivity() == ACT_ASSASSIN_PERCH) ||
	(GetActivity() == ACT_RANGE_ATTACK1))
	{
	eyeOffset = Vector(0, 0, 24.0f);
	}
	else
	{
	eyeOffset = BaseClass::GetViewOffset();
	}
	*/

	return eyeOffset;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Vector
//-----------------------------------------------------------------------------
Vector CNPC_Assassin::EyePosition(void)
{

	return GetAbsOrigin() + Vector(0, 0, 24);

	/*
	Vector m_EyePos;
	GetAttachment( "eyes", m_EyePos );
	return m_EyePos;
	*/
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Assassin::OnScheduleChange(void)
{
	//TODO: Change eye state?

	BaseClass::OnScheduleChange();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : state - 
//-----------------------------------------------------------------------------
void CNPC_Assassin::SetEyeState(eyeState_t state)
{
	//Must have a valid eye to affect
	if ((m_pEyeSprite == NULL) || (m_pEyeTrail == NULL))
		return;

	//Set the state
	switch (state)
	{
	default:
	case ASSASSIN_EYE_SEE_TARGET: //Fade in and scale up
		m_pEyeSprite->SetColor(255, 0, 0);
		m_pEyeSprite->SetBrightness(164, 0.1f);
		m_pEyeSprite->SetScale(0.4f, 0.1f);

		m_pEyeTrail->SetColor(255, 0, 0);
		m_pEyeTrail->SetScale(8.0f);
		m_pEyeTrail->SetBrightness(164);

		break;

	case ASSASSIN_EYE_SEEKING_TARGET: //Ping-pongs

		//Toggle our state
		m_bBlinkState = !m_bBlinkState;
		m_pEyeSprite->SetColor(255, 128, 0);

		if (m_bBlinkState)
		{
			//Fade up and scale up
			m_pEyeSprite->SetScale(0.25f, 0.1f);
			m_pEyeSprite->SetBrightness(164, 0.1f);
		}
		else
		{
			//Fade down and scale down
			m_pEyeSprite->SetScale(0.2f, 0.1f);
			m_pEyeSprite->SetBrightness(64, 0.1f);
		}

		break;

	case ASSASSIN_EYE_DORMANT: //Fade out and scale down
		m_pEyeSprite->SetScale(0.5f, 0.5f);
		m_pEyeSprite->SetBrightness(64, 0.5f);

		m_pEyeTrail->SetScale(2.0f);
		m_pEyeTrail->SetBrightness(64);
		break;

	case ASSASSIN_EYE_DEAD: //Fade out slowly
		m_pEyeSprite->SetColor(255, 0, 0);
		m_pEyeSprite->SetScale(0.1f, 5.0f);
		m_pEyeSprite->SetBrightness(0, 5.0f);

		m_pEyeTrail->SetColor(255, 0, 0);
		m_pEyeTrail->SetScale(0.1f);
		m_pEyeTrail->SetBrightness(0);
		break;

	case ASSASSIN_EYE_ACTIVE:
		m_pEyeSprite->SetColor(255, 0, 0);
		m_pEyeSprite->SetScale(0.1f);
		m_pEyeSprite->SetBrightness(0);
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Assassin::GatherEnemyConditions(CBaseEntity *pEnemy)
{
	ClearCondition(COND_ASSASSIN_ENEMY_TARGETTING_ME);

	BaseClass::GatherEnemyConditions(pEnemy);

	// See if we're being targetted specifically
	if (HasCondition(COND_ENEMY_FACING_ME))
	{
		Vector	enemyDir = GetAbsOrigin() - pEnemy->GetAbsOrigin();
		VectorNormalize(enemyDir);

		Vector	enemyBodyDir;
		CBasePlayer	*pPlayer = ToBasePlayer(pEnemy);

		if (pPlayer != NULL)
		{
			enemyBodyDir = pPlayer->BodyDirection3D();
		}
		else
		{
			AngleVectors(pEnemy->GetAbsAngles(), &enemyBodyDir);
		}

		float	enemyDot = DotProduct(enemyBodyDir, enemyDir);

		//FIXME: Need to refine this a bit
		if (enemyDot > 0.97f)
		{
			SetCondition(COND_ASSASSIN_ENEMY_TARGETTING_ME);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNPC_Assassin::BuildScheduleTestBits(void)
{
	SetNextThink(gpGlobals->curtime + 0.05);

	//Don't allow any modifications when scripted
	if (m_NPCState == NPC_STATE_SCRIPT)
		return;

	if (IsCurSchedule(SCHED_ASSASSIN_FIND_VANTAGE_POINT))
	{
		if ((HasCondition(COND_LIGHT_DAMAGE) || HasCondition(COND_HEAVY_DAMAGE)) && HasCondition(COND_SEE_ENEMY))
		{
			SetCustomInterruptCondition(COND_SEE_ENEMY);
		}
	}

	//Become interrupted if we're targetted when shooting an enemy
	/*
	if (IsCurSchedule(SCHED_RANGE_ATTACK1))
	{
	SetCustomInterruptCondition(COND_ASSASSIN_ENEMY_TARGETTING_ME);
	}
	*/

}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &info - 
//-----------------------------------------------------------------------------
void CNPC_Assassin::Event_Killed(const CTakeDamageInfo &info)
{
	BaseClass::Event_Killed(info);

	// Turn off the eye
	SetEyeState(ASSASSIN_EYE_DEAD);

	// Turn off the pistols
	SetBodygroup(1, 0);

	// Spawn her guns
}

//-----------------------------------------------------------------------------
//
// Schedules
//
//-----------------------------------------------------------------------------

AI_BEGIN_CUSTOM_NPC(npc_assassin, CNPC_Assassin)

DECLARE_ACTIVITY(ACT_ASSASSIN_FLIP_LEFT)
DECLARE_ACTIVITY(ACT_ASSASSIN_FLIP_RIGHT)
DECLARE_ACTIVITY(ACT_ASSASSIN_FLIP_BACK)
DECLARE_ACTIVITY(ACT_ASSASSIN_FLIP_FORWARD)
DECLARE_ACTIVITY(ACT_ASSASSIN_PERCH)

//Adrian: events go here
DECLARE_ANIMEVENT(AE_ASSASIN_FIRE_PISTOL_RIGHT)
DECLARE_ANIMEVENT(AE_ASSASIN_FIRE_PISTOL_LEFT)
DECLARE_ANIMEVENT(AE_ASSASIN_KICK_HIT)

DECLARE_TASK(TASK_ASSASSIN_GET_PATH_TO_VANTAGE_POINT)
DECLARE_TASK(TASK_ASSASSIN_EVADE)
DECLARE_TASK(TASK_ASSASSIN_SET_EYE_STATE)
DECLARE_TASK(TASK_ASSASSIN_LUNGE)
DECLARE_TASK(TASK_ASSASSIN_BEGIN_FLANK)
DECLARE_TASK(TASK_ASSASSIN_FLANK_COVER)


DECLARE_CONDITION(COND_ASSASSIN_ENEMY_TARGETTING_ME)

//=========================================================
// ASSASSIN_STALK_ENEMY
//=========================================================

DEFINE_SCHEDULE
(
SCHED_ASSASSIN_STALK_ENEMY,

"	Tasks"
"		TASK_STOP_MOVING						0"
"		TASK_PLAY_SEQUENCE_FACE_ENEMY			ACTIVITY:ACT_ASSASSIN_PERCH"
"		TASK_WAIT					1.2"
"	"
"	Interrupts"
"		COND_ASSASSIN_ENEMY_TARGETTING_ME"
"		COND_CAN_RANGE_ATTACK1"
"		COND_SEE_ENEMY"
"		COND_LIGHT_DAMAGE"
"		COND_HEAVY_DAMAGE"
)

//=========================================================
// > ASSASSIN_FIND_VANTAGE_POINT
//=========================================================

DEFINE_SCHEDULE
(
SCHED_ASSASSIN_FIND_VANTAGE_POINT,

"	Tasks"
"		TASK_SET_FAIL_SCHEDULE					SCHEDULE:SCHED_ASSASSIN_ESTABLISH_LOS"
"		TASK_STOP_MOVING						0"
"		TASK_ASSASSIN_GET_PATH_TO_VANTAGE_POINT	0"
"		TASK_RUN_PATH							0"
"		TASK_WAIT_FOR_MOVEMENT					0"
"		TASK_SET_SCHEDULE						SCHEDULE:SCHED_COMBAT_FACE"
"	"
"	Interrupts"

"		COND_LIGHT_DAMAGE"
"		COND_HEAVY_DAMAGE"
"		COND_CAN_MELEE_ATTACK1"
"		COND_CAN_MELEE_ATTACK2"
"		COND_TASK_FAILED"
)

//=========================================================
// Assassin needs to avoid the player
//=========================================================
DEFINE_SCHEDULE
(
SCHED_ASSASSIN_EVADE,

"	Tasks"
"		TASK_SET_FAIL_SCHEDULE		SCHEDULE:SCHED_ASSASSIN_TAKE_COVER"
"		TASK_STOP_MOVING			0"
"		TASK_ASSASSIN_EVADE			0"
"	"
"	Interrupts"
"		COND_TASK_FAILED"
)

//=========================================================
// Assassin needs to avoid the player
//=========================================================
DEFINE_SCHEDULE
(
SCHED_ASSASSIN_LUNGE,

"	Tasks"
"		TASK_SET_FAIL_SCHEDULE		SCHEDULE:SCHED_ASSASSIN_TAKE_COVER"
"		TASK_STOP_MOVING			0"
"		TASK_FACE_ENEMY				0"
"		TASK_PLAY_SEQUENCE			ACTIVITY:ACT_ASSASSIN_FLIP_FORWARD"
"	"
"	Interrupts"
"		COND_TASK_FAILED"
)

DEFINE_SCHEDULE
(
SCHED_ASSASSIN_TAKE_COVER,

"	Tasks"
"		TASK_STOP_MOVING				0"
"		TASK_WAIT					0.2"
"		TASK_FIND_FAR_NODE_COVER_FROM_ENEMY 320.0"
"		TASK_RUN_PATH				0"
"		TASK_WAIT_FOR_MOVEMENT		0"
"		TASK_WAIT_FACE_ENEMY		0.5"
"		TASK_REMEMBER				MEMORY:INCOVER"
"		TASK_SET_SCHEDULE			SCHEDULE:SCHED_COMBAT_FACE"
"	"
"	Interrupts"
"		COND_CAN_MELEE_ATTACK1"
"		COND_CAN_MELEE_ATTACK2"
)

//=========================================================
// SCHED_COMBINE_RANGE_ATTACK1
//=========================================================
DEFINE_SCHEDULE
(
SCHED_ASSASSIN_FIRE_PISTOLS,

"	Tasks"
"		TASK_STOP_MOVING				0"
"		TASK_FACE_ENEMY					0"
"		TASK_WAIT_RANDOM				0.3" //.3
"		TASK_RANGE_ATTACK1				0"
//"		TASK_COMBINE_IGNORE_ATTACKS		0.5"
//"		TASK_SET_SCHEDULE				SCHEDULE:SCHED_COMBINE_POST_FIRE"
""
"	Interrupts"
"		COND_NEW_ENEMY"
"		COND_ENEMY_DEAD"
"		COND_HEAVY_DAMAGE"
"		COND_LIGHT_DAMAGE"
"		COND_CAN_MELEE_ATTACK1"
"		COND_CAN_MELEE_ATTACK2"
"		COND_WEAPON_BLOCKED_BY_FRIEND"
"		COND_TOO_CLOSE_TO_ATTACK"
"		COND_GIVE_WAY"
"		COND_HEAR_DANGER"
"		COND_HEAR_MOVE_AWAY"
""
// Enemy_Occluded				Don't interrupt on this.  Means
//								comibine will fire where player was after
//								he has moved for a little while.  Good effect!!
// WEAPON_SIGHT_OCCLUDED		Don't block on this! Looks better for railings, etc.
)

DEFINE_SCHEDULE
(
SCHED_ASSASSIN_FLANK_COVER,

"	Tasks"
"		TASK_SET_FAIL_SCHEDULE					SCHEDULE:SCHED_ASSASSIN_TAKE_COVER"
"		TASK_ANNOUNCE_ATTACK				3"
"		TASK_STOP_MOVING						0"
"		TASK_ASSASSIN_BEGIN_FLANK					0"
"		TASK_SET_TOLERANCE_DISTANCE				96"
"		TASK_SET_ROUTE_SEARCH_TIME				1"	// Spend 1 second trying to build a path if stuck"
"		TASK_ASSASSIN_FLANK_COVER				60"
"		TASK_RUN_PATH							0"
"		TASK_WAIT_FOR_MOVEMENT					0"
"		TASK_FACE_ENEMY							0"
""
"	Interrupts"
"		COND_NEW_ENEMY"
//"		COND_CAN_RANGE_ATTACK1"
//"		COND_CAN_RANGE_ATTACK2"
"		COND_CAN_MELEE_ATTACK1"
"		COND_CAN_MELEE_ATTACK2"
"		COND_ENEMY_DEAD"
//"		COND_ENEMY_UNREACHABLE"
"		COND_TOO_CLOSE_TO_ATTACK"
"		COND_LOST_ENEMY"
"		COND_HEAR_DANGER"
"		COND_HEAR_MOVE_AWAY"
)

DEFINE_SCHEDULE
(
SCHED_ASSASSIN_ESTABLISH_LOS,

"	Tasks "
"		TASK_SET_FAIL_SCHEDULE			SCHEDULE:SCHED_FAIL_ESTABLISH_LINE_OF_FIRE"
"		TASK_SET_TOLERANCE_DISTANCE		48"
"		TASK_GET_PATH_TO_ENEMY_LKP_LOS	0"
"		TASK_RUN_PATH					0"
"		TASK_WAIT_FOR_MOVEMENT			0"
"		TASK_SET_SCHEDULE				SCHEDULE:SCHED_COMBAT_FACE"
"	"
"	Interrupts "
"		COND_NEW_ENEMY"
"		COND_ENEMY_DEAD"
//"		COND_CAN_RANGE_ATTACK1"
//"		COND_CAN_RANGE_ATTACK2"
"		COND_CAN_MELEE_ATTACK1"
"		COND_CAN_MELEE_ATTACK2"
"		COND_HEAR_DANGER"
"		COND_HEAR_MOVE_AWAY"
"		COND_HEAVY_DAMAGE"
)


AI_END_CUSTOM_NPC()
