#ifndef VANCE_PLAYER_H
#define VANCE_PLAYER_H

#include "hl2_player.h"
#include "anim/singleplayer_animstate.h"

#define P_PLAYER_ALYX	"models/player/alyx.mdl"
#define P_PLAYER_HEV	"models/player/hev.mdl"

#define C_ARMS_ALYX		"models/weapons/v_arms_nosuit.mdl"
#define C_ARMS_HEV		"models/weapons/v_arms_suit.mdl"

#define V_KICK_ALYX		"models/weapons/v_kick_nosuit.mdl"
#define V_KICK_HEV		"models/weapons/v_kick_suit.mdl"

extern ConVar sk_max_tourniquets;

typedef enum
{
	ACTION_NONE = 0,
	ACTION_SLIDE,
	ACTION_VAULT,
	ACTION_CLIMB
} ParkourAction_t;

class CVancePlayer : public CHL2_Player
{
	DECLARE_CLASS(CVancePlayer, CHL2_Player);
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();
public:
	CVancePlayer();
	~CVancePlayer();

	virtual void			Spawn(void);
	virtual void			Precache(void);
	virtual void			CheatImpulseCommands(int iImpulse);

	virtual bool			Weapon_CanUse(CBaseCombatWeapon* pWeapon);

	virtual int				OnTakeDamage(const CTakeDamageInfo &inputInfo);

	bool					IsSpawning() { return m_bSpawning; }

	virtual void			EquipSuit(bool bPlayEffects);
	virtual void			RemoveSuit();

	virtual void			CreateViewModel(int iViewModel = 0);

	void					HandleSpeedChanges(void);

	void					Heal(int health); // move these to CBasePlayer at some point
	void					Damage(int damage);

	void					Bleed();

	virtual void 			PreThink(void);
	virtual void 			PostThink(void);

	void 					StartAdmireGlovesAnimation(void);

	bool 					CanSprint();
	void 					StartSprinting(void);
	void 					StopSprinting(void);
	void					StartWalking(void);
	void					StopWalking(void);

	void					SuitPower_Update(void);
	bool					ApplyBattery(float powerMultiplier, bool bFlashlightPower = false);
	void					FlashlightTurnOn(void);

	void					Hit(trace_t& traceHit, Activity nHitActivity, bool bIsSecondary);
	void					SetKickTime(CBaseViewModel* pViewModel);
	void					KickAttack(void);
	float					GetKickAnimationLength() { return m_flKickAnimLength; }

	virtual void			PlayerUse(void);
	virtual void			UpdateClientData(void);
	virtual void			ItemPostFrame();
	void					SetAnimation(PLAYER_ANIM playerAnim);

	void					HandleSlide(void);
	void					HandleVault(void);
	void					HandleLedgeClimb(void);
	bool					StartLedgeClimb(Vector start);
	void					Think(void);

	virtual bool			CanBreatheUnderwater() const { return IsSuitEquipped() && m_HL2Local.m_flSuitPower > 0.0f; }

	inline const char		*GetPlayerWorldModel() { return IsSuitEquipped() ? P_PLAYER_HEV : P_PLAYER_ALYX; }
	inline const char		*GetLegsViewModel() { return IsSuitEquipped() ? V_KICK_HEV : V_KICK_ALYX; }
	inline const char		*GetArmsViewModel() { return IsSuitEquipped() ? C_ARMS_HEV : C_ARMS_ALYX; }
	
	inline bool				IsBleeding() { return m_bBleed; }

	void					UseTourniquet();
	void					InjectStim();

	inline unsigned int		MaxTourniquets() { return sk_max_tourniquets.GetInt(); }
	inline unsigned int		NumTourniquets() { return m_nNumTourniquets; }
	bool					GiveTourniquet()
	{
		if (m_nNumTourniquets == MaxTourniquets() - 1)
		{
			m_nNumTourniquets += 1;
			return true;
		}
		else
		{
			return false;
		}
	}

	inline void				SetBusy(float flBusyEndTime)
	{
		Assert(!m_bBusyInAnim); // we should not already be busy

		m_bBusyInAnim = true;
		m_flBusyAnimEndTime = flBusyEndTime;
	}
	
private:
	bool			m_bBusyInAnim;
	float			m_flBusyAnimEndTime;

	CAI_Expresser	*m_pExpresser;
	bool			m_bInAScript;

	CSinglePlayerAnimState *m_pPlayerAnimState;
	QAngle m_angEyeAngles;

	CNetworkVar(float, m_flNextKickAttack);
	CNetworkVar(float, m_flNextKick);
	CNetworkVar(float, m_flKickAnimLength);
	CNetworkVar(float, m_flKickTime);
	CNetworkVar(bool, m_bIsKicking);

	float		m_flNextSprint;			// Next time we're allowed to sprint
	float		m_flNextWalk;			// Next time we're allowed to walk.

	bool		m_bSpawning;
	float		m_flNextPainSound;

	float		m_flLastDamage;
	float		m_flBleedChance;
	float		m_flNextBleedChanceDecay;

	bool		m_bBleed;
	float		m_flBleedEndTime;
	float		m_flNextBleedTime;

	bool		m_bShouldRegenerate;
	float		m_flRegenerateEndTime;
	float		m_flNextHealTime;

	unsigned int m_nNumTourniquets;

	ParkourAction_t m_ParkourAction;
	Vector		m_vecClimbDesiredOrigin;
	Vector		m_vecClimbCurrentOrigin;
	Vector		m_vecClimbStartOrigin;
	Vector		m_vecClimbOutVelocity;
	float		m_flClimbFraction;
	float		m_flClimbTraceFraction;
};

#endif // VANCE_PLAYER_H