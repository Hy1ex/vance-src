#ifndef VANCE_PLAYER_H
#define VANCE_PLAYER_H

#include "vance_shareddefs.h"
#include "hl2_player.h"
#include "singleplayer_animstate.h"
#include "vance_viewmodel.h"

#define P_PLAYER_ALYX	"models/player/alyx.mdl"
#define P_PLAYER_HEV	"models/player/hev.mdl"
#define P_PLAYER_SYNTH	"models/player/synth.mdl"

#define P_PLAYER_LEGS_ALYX	"models/player/hev_fpp.mdl" //"models/player/alyx_fpp.mdl"
#define P_PLAYER_LEGS_HEV	"models/player/hev_fpp.mdl"
#define P_PLAYER_LEGS_SYNTH	"models/player/hev_fpp.mdl" //"models/player/synth_fpp.mdl"

#define C_ARMS_ALYX		"models/weapons/v_arms_nosuit.mdl"
#define C_ARMS_HEV		"models/weapons/v_arms_suit.mdl"
#define C_ARMS_SYNTH	"models/weapons/v_arms_synth.mdl"

#define V_KICK_ALYX		"models/weapons/v_kick_nosuit.mdl"
#define V_KICK_HEV		"models/weapons/v_kick_suit.mdl"
#define V_KICK_SYNTH	"models/weapons/v_kick_synth.mdl"

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
	virtual void			UpdateStepSound( surfacedata_t *psurface, const Vector &vecOrigin, const Vector &vecVelocity );

	void					SetAnimation(PLAYER_ANIM playerAnim);

	// Parkour abilities
	void					SlideTick();
	void					TrySlide();

	void					LedgeClimbTick();
	void					TryLedgeClimb();

	void					Think();

	void					StartHealing();

	inline bool CanBreathUnderwater() const { return (IsSuitEquipped() || IsSynthEquipped()) && m_HL2Local.m_flSuitPower > 0.0f ;}

	inline const char* GetPlayerWorldModel() const {
		if (GetObserverMode() == 0 || GetObserverMode() == 4) {
			if (IsSynthEquipped())
				return P_PLAYER_LEGS_SYNTH;
			else if (IsSuitEquipped())
				return P_PLAYER_LEGS_HEV;
			return P_PLAYER_LEGS_ALYX;
		} else {
			if (IsSynthEquipped())
				return P_PLAYER_SYNTH;
			else if (IsSuitEquipped())
				return P_PLAYER_HEV;
			return P_PLAYER_ALYX;
		}
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

	bool IsSliding()
	{
		return m_ParkourAction.Get() == ParkourAction::Slide;
	}

	bool IsClimbing()
	{
		return m_ParkourAction.Get() == ParkourAction::Climb;
	}

	Vector GetVaultCameraAdjustment(){
		return m_vecVaultCameraAdjustment;
	}

	bool IsVaulting()
	{
		return m_ParkourAction.Get() == ParkourAction::Climb && m_bVaulting;
	}

	bool CanSwitchViewmodelSequence() {
		CVanceViewModel *pViewModel = (CVanceViewModel *)GetViewModel();
		if (!pViewModel)
			return false;
		Activity act = pViewModel->GetSequenceActivity(pViewModel->GetSequence());
		if (act == ACT_VM_HOLSTER || act == ACT_VM_HOLSTER_EXTENDED
			|| act == ACT_VM_EXTEND || act == ACT_VM_RETRACT
			|| pViewModel->IsPlayingReloadActivity()
			|| act == ACT_VM_PRIMARYATTACK || act == ACT_VM_FIRE_EXTENDED)
			return false;
		return true;
	}

	CBaseCombatWeapon* m_UnarmedWeapon;
	CBaseCombatWeapon* m_HolsteredWeapon;
	
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
	bool		WpnCanSprint();

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
	CNetworkVar(int, m_iNumTourniquets);

	// Stims
	CNetworkVar(int, m_iNumStims);
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
	float		m_fNextThrowTime = 0.0f;

	// Parkour
	CNetworkVar( ParkourAction, m_ParkourAction );
	Vector		m_vecClimbDesiredOrigin;
	Vector		m_vecClimbCurrentOrigin;
	Vector		m_vecClimbStartOrigin;
	Vector		m_vecClimbOutVelocity;
	float		m_flClimbFraction;
	bool		m_bBigClimb;
	bool		m_bVault;
	Vector		m_vecClimbStartOriginHull;
	CNetworkVar( bool, m_bVaulting );
	CNetworkVar( Vector, m_vecVaultCameraAdjustment);

	CNetworkVar(float, m_flSlideEndTime);
	CNetworkVar(float, m_flSlideFrictionScale);
	Vector		m_vecSlideDirection;
	float		m_fMidairSlideWindowTime = 0.0f;
	bool		m_bAllowMidairSlide = true;
	float		m_fNextSlideTime = 0.0f;
	float		m_fSprintTime = 0.0f;
	bool		m_bSlideWaitingForCrouchDebounce;

	// vehicle
	bool		m_bWasInAVehicle;

	friend class CVanceGameMovement;
};

#endif // VANCE_PLAYER_H