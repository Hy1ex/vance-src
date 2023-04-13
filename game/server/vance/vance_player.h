#ifndef VANCE_PLAYER_H
#define VANCE_PLAYER_H

#include "hl2_player.h"
#include "singleplayer_animstate.h"

#define P_PLAYER_ALYX	"models/player/alyx.mdl"
#define P_PLAYER_HEV	"models/player/hev.mdl"
#define P_PLAYER_SYNTH	"models/player/synth.mdl"

#define C_ARMS_ALYX		"models/weapons/v_arms_nosuit.mdl"
#define C_ARMS_HEV		"models/weapons/v_arms_suit.mdl"
#define C_ARMS_SYNTH		"models/weapons/v_arms_synth.mdl"

#define V_KICK_ALYX		"models/weapons/v_kick_nosuit.mdl"
#define V_KICK_HEV		"models/weapons/v_kick_suit.mdl"
#define V_KICK_SYNTH		"models/weapons/v_kick_synth.mdl"

enum class ParkourAction
{
	None,
	Slide,
	Climb
};

// Needs better name
enum class GestureAction
{
	None,
	InjectingStim,
	EquippingTourniquet,
	ThrowingGrenade
};

class CVancePlayer : public CHL2_Player
{
	DECLARE_CLASS(CVancePlayer, CHL2_Player);
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();
public:
	CVancePlayer();
	~CVancePlayer();

	static CVancePlayer*	Create(edict_t* pEdict);

	virtual void			Spawn();
	virtual void			Precache();
	virtual void			CheatImpulseCommands(int iImpulse);

	virtual bool			Weapon_CanUse(CBaseCombatWeapon* pWeapon);

	virtual int				OnTakeDamage(const CTakeDamageInfo &inputInfo);

	bool					IsSpawning() { return m_bSpawning; }

	virtual void			EquipSuit(bool bPlayEffects);
	virtual void			EquipSynth(bool bPlayEffects);
	virtual void			RemoveSuit();
	virtual void			RemoveSynth();


	bool	IsSynthEquipped() const	{ return m_Local.m_bWearingSynth; }
	bool					bHasSynth;

	virtual void			CreateViewModel(int iViewModel = 0);

	void					HandleSpeedChanges();
	void					ThrowGrenade( void );
	void					ThrowingGrenade( void );
	void					CreateGrenade( bool rollMe );

	void					Heal(int health); // move these to CBasePlayer at some point
	void					Damage(int damage);

	void					Bleed();

	virtual void 			PreThink();
	virtual void 			PostThink();

	void 					StartAdmireGlovesAnimation();

	bool 					CanSprint();
	void 					StartSprinting();
	void 					StopSprinting();
	void					StartWalking();
	void					StopWalking();

	void					SuitPower_Update();
	bool					ApplyBattery( float powerMultiplier );
	void					FlashlightTurnOn();

	void					Hit( trace_t &traceHit, Activity nHitActivity );
	void					KickAttack();
	float					GetKickAnimationLength() { return m_flKickAnimLength; }

	virtual void			PlayerUse();
	virtual void			UpdateClientData();
	virtual void			ItemPostFrame();
	void					SetAnimation(PLAYER_ANIM playerAnim);

	// Parkour abilities
	void					SlideTick();
	void					TrySlide();

	void					LedgeClimbTick();
	void					TryLedgeClimb();

	void					Think();

	inline bool CanBreathUnderwater() const { return (IsSuitEquipped() || IsSynthEquipped()) && m_HL2Local.m_flSuitPower > 0.0f ;}

	inline const char* GetPlayerWorldModel() const {
		if (IsSynthEquipped())
			return P_PLAYER_SYNTH;
		else if (IsSuitEquipped())
			return P_PLAYER_HEV;

		return P_PLAYER_ALYX;
	}

	inline const char* GetLegsViewModel() const {
		if (IsSynthEquipped())
			return V_KICK_SYNTH;
		else if (IsSuitEquipped())
			return V_KICK_HEV;

		return V_KICK_ALYX;
	}

	inline const char* GetArmsViewModel() const {
		if (IsSynthEquipped())
			return C_ARMS_SYNTH;
		else if (IsSuitEquipped())
			return C_ARMS_HEV;

		return C_ARMS_ALYX;
	}

	inline bool				IsBleeding() const { return m_bBleeding; }

	void					UseStimOrTourniquet();

	void					UseTourniquet();
	void					UseStim();

	bool					GiveTourniquet( int count = 1 );
	bool					GiveStim( int count = 1 );
	
private:

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

	// kick
	bool		bDropKick;

	// Gestures
	GestureAction m_PerformingGesture;
	float		m_fGestureFinishTime;

	// Bleed chance
	float		m_fLastDamageTime;
	float		m_fBleedChance;
	float		m_fNextBleedChanceDecay;

	// Bleeding
	bool		m_bBleeding;		// Are we bleeding?
	float		m_fNextBleedTime;	// When will we next take bleed damage?
	float		m_fBleedEndTime;	// When will bleeding stop?

	// Tourniquets
	int			m_iNumTourniquets;

	// Stims
	int			m_iNumStims;
	bool		m_bStimRegeneration;
	float		m_fStimRegenerationNextHealTime;
	float		m_fStimRegenerationEndTime;

	//QUick Grenade
	float		m_fTimeToReady = 0.0f;
	bool		m_bWantsToThrow = false;
	float		m_fNadeDetTime;
	float		m_fNextBlipTime;
	float		m_fWarnAITime;
	bool		m_bWarnedAI;
	bool		m_bRoll;
	CBaseCombatWeapon* m_CurrentWeapon;

	// Parkour
	ParkourAction m_ParkourAction;
	Vector		m_vecClimbDesiredOrigin;
	Vector		m_vecClimbCurrentOrigin;
	Vector		m_vecClimbStartOrigin;
	Vector		m_vecClimbOutVelocity;
	float		m_flClimbFraction;

	Vector		m_vecSlideStartPos;
	Vector		m_vecSlideEndPos;
};

#endif // VANCE_PLAYER_H