//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef NPC_ASSASSIN_H
#define NPC_ASSASSIN_H
#ifdef _WIN32
#pragma once
#endif

#include "ai_basenpc.h"
#include "Sprite.h"
#include "SpriteTrail.h"
#include "soundent.h"

//Eye states
enum eyeState_t
{
	ASSASSIN_EYE_SEE_TARGET = 0,		//Sees the target, bright and big
	ASSASSIN_EYE_SEEKING_TARGET,	//Looking for a target, blinking (bright)
	ASSASSIN_EYE_ACTIVE,			//Actively looking
	ASSASSIN_EYE_DORMANT,			//Not active
	ASSASSIN_EYE_DEAD,				//Completely invisible
};

//=========================================================
//=========================================================
class CNPC_Assassin : public CAI_BaseNPC
{
public:
	DECLARE_CLASS(CNPC_Assassin, CAI_BaseNPC);
	// DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CNPC_Assassin(void);

	Class_T		Classify(void)			{ return CLASS_COMBINE; }
	virtual float	HearingSensitivity(void) { return 1.0; };
	int				GetSoundInterests(void);
	virtual bool	QueryHearSound(CSound *pSound);

	int			SelectSchedule(void);
	int			SelectSchedObject();
	virtual int		SelectFailSchedule(int failedSchedule, int failedTask, AI_TaskFailureCode_t taskFailCode);
	int			MeleeAttack1Conditions(float flDot, float flDist);
	int			RangeAttack1Conditions(float flDot, float flDist);
	int			RangeAttack2Conditions(float flDot, float flDist);
	virtual bool	InnateWeaponLOSCondition(const Vector &ownerPos, const Vector &targetPos, bool bSetConditions);
	virtual float	InnateRange1MinRange(void) { return 0.0f; }
	virtual float	InnateRange1MaxRange(void) { return 724 * 12; }

	void		Precache(void);
	void		Spawn(void);
	virtual void	AimGun(void);
	void		PrescheduleThink(void);
	Activity	NPC_TranslateActivity(Activity eNewActivity);
	void		HandleAnimEvent(animevent_t *pEvent);
	void		StartTask(const Task_t *pTask);
	void		RunTask(const Task_t *pTask);
	void		OnScheduleChange(void);
	void		GatherEnemyConditions(CBaseEntity *pEnemy);
	void		BuildScheduleTestBits(void);
	void		Event_Killed(const CTakeDamageInfo &info);
	Vector		Weapon_ShootPosition();
	bool		FValidateHintType(CAI_Hint *pHint);
	bool		IsJumpLegal(const Vector &startPos, const Vector &apex, const Vector &endPos) const;

	float		GetJumpGravity() const		{ return 4.0f; } //4.0
	float		GetMaxJumpSpeed() const { return 640.0f; }
	//bool		MovementCost(int moveType, const Vector &vecStart, const Vector &vecEnd, float *pCost);

	float		MaxYawSpeed(void);

	const Vector &GetViewOffset(void);
	Vector		EyePosition(void);

private:

	void		SetEyeState(eyeState_t state);
	void		FirePistol(int hand);
	bool		CanFlip(int flipType, Activity &activity, const Vector *avoidPosition);

	int			m_nNumFlips;
	int			m_nLastFlipType;
	float		m_flNextFlipTime;	//Next earliest time the assassin can flip again
	float		m_flNextLungeTime;
	float		m_flNextShotTime;
	int			m_nShotCount;

	bool		m_bEvade;
	bool		m_bAggressive;		// Sets certain state, including whether or not her eye is visible
	bool		m_bBlinkState;

	CSprite				*m_pEyeSprite;
	CSpriteTrail		*m_pEyeTrail;

	DEFINE_CUSTOM_AI;
};


#endif // NPC_ASSASSIN_H
