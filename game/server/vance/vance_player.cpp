//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:        Player for Vance.
//
//=============================================================================//

#include "cbase.h"
#include "hl2_player.h"
#include "ai_speech.h"
#include "ai_playerally.h"
#include "vance_viewmodel.h"
#include "singleplayer_animstate.h"
#include "datacache/imdlcache.h"
#include "in_buttons.h"
#include "trains.h"
#include "weapon_physcannon.h"
#include "gamestats.h"
#include "rumble_shared.h"
#include "npc_barnacle.h"
#include "globalstate.h"
#include "vance_player.h"
#include "vance_shareddefs.h"
#include "iservervehicle.h"
#include "vehicle_viewblend_shared.h"
#include "in_buttons.h"
#include "vehicle_viewblend_shared.h"
#include "gamemovement.h"
#include "in_buttons.h"
#include <stdarg.h>
#include "movevars_shared.h"
#include "engine/IEngineTrace.h"
#include "SoundEmitterSystem/isoundemittersystembase.h"
#include "decals.h"
#include "coordsize.h"
#include "rumble_shared.h"
#include "interpolatortypes.h"
#include "ammodef.h"
#include "grenade_frag.h"
#include "func_breakablesurf.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//stuff for emulating frag blips
#define FRAG_GRENADE_BLIP_FREQUENCY			1.0f
#define FRAG_GRENADE_BLIP_FAST_FREQUENCY	0.3f
#define FRAG_GRENADE_WARN_TIME 1.5f
#define GRENADE_RADIUS	4.0f // inches

extern ConVar player_showpredictedposition;
extern ConVar player_showpredictedposition_timestep;

extern ConVar sv_autojump;
extern ConVar sv_stickysprint;
extern ConVar sv_infinite_aux_power;

extern ConVar hl2_walkspeed;
extern ConVar hl2_normspeed;
extern ConVar hl2_sprintspeed;

extern ConVar sk_battery;

extern int gEvilImpulse101;

ConVar sk_max_tourniquets("sk_max_tourniquets", "3", FCVAR_NONE);
ConVar sk_max_stims("sk_max_stims", "3", FCVAR_NONE);

ConVar sk_bleed_chance("sk_bleed_chance", "1", FCVAR_NONE, "The chance (in percentage) that the Player will bleed upon taking damage.");
ConVar sk_bleed_chance_increment("sk_bleed_chance_increment", "0.2", FCVAR_NONE, "The amount of chance the Player will bleed will increment by when shot.");
ConVar sk_bleed_chance_decay_start_time("sk_bleed_chance_decay_start_time", "4", FCVAR_NONE, "How long after being shot the bleed chance will start to decay.");
ConVar sk_bleed_chance_decay_rate("sk_bleed_chance_decay_rate", "0.5", FCVAR_NONE);

ConVar sk_bleed_lifetime("sk_bleed_lifetime", "5", FCVAR_NONE);
ConVar sk_bleed_dmg_per_interval("sk_bleed_dmg_per_interval", "2", FCVAR_NONE);
ConVar sk_bleed_dmg_interval("sk_bleed_dmg_interval", "1.5", FCVAR_NONE);

ConVar sk_stim_regen_lifetime("sk_stim_regen_lifetime", "5", FCVAR_NONE);
ConVar sk_stim_health_per_interval("sk_stim_health_per_interval", "5", FCVAR_NONE);
ConVar sk_stim_heal_interval("sk_stim_heal_interval", "1", FCVAR_NONE);
ConVar sk_nextsprint_delay("sk_nextsprint_delay", "1.0", FCVAR_NONE);

#define	VANCE_WALK_SPEED hl2_normspeed.GetFloat()
#define	VANCE_SPRINT_SPEED hl2_sprintspeed.GetFloat()

#define	FLASH_DRAIN_TIME	 1.1111	// 100 units / 90 secs
#define	FLASH_CHARGE_TIME	 50.0f	// 100 units / 2 secs

#define SUITPOWER_CHARGE_RATE	12.5	// 100 units in 8 seconds

ConVar vance_climb_speed("vance_climb_speed", "0.025", FCVAR_CHEAT);
ConVar vance_climb_high_speed("vance_climb_high_speed", "0.01", FCVAR_CHEAT);
ConVar vance_climb_checkray_count("vance_climb_checkray_count", "5", FCVAR_CHEAT);
ConVar vance_climb_checkray_dist("vance_climb_checkray_dist", "64", FCVAR_CHEAT);
ConVar vance_climb_checkray_allowedheight( "vance_climb_checkray_allowedheight", "60", FCVAR_CHEAT );
// 10 is just enough to jump on the 64 unit tall wall
ConVar vance_climb_checkray_height( "vance_climb_checkray_height", "11", FCVAR_CHEAT );
ConVar vance_climb_debug("vance_climb_debug", "0");
#define CLIMB_TRACE_DIST		vance_climb_checkray_dist.GetFloat()
#define CLIMB_LERPSPEED			vance_climb_speed.GetFloat();
#define CLIMB_LERPSPEED_SLOW	vance_climb_high_speed.GetFloat();

extern ConVar vance_slide_time;
ConVar vance_slide_addvelocity( "vance_slide_addvelocity", "200.0", FCVAR_CHEAT );
ConVar vance_slide_frictionscale( "vance_slide_frictionscale", "0.1", FCVAR_CHEAT );
ConVar vance_slide_cooldown("vance_slide_cooldown", "0.5", FCVAR_CHEAT);
ConVar vance_slide_sprint_time("vance_slide_sprint_time", "0.3", FCVAR_CHEAT);
ConVar vance_slide_midair_window("vance_slide_midair_window", "0.4", FCVAR_CHEAT);

ConVar vance_kick_meleedamageforce( "kick_meleedamageforce", "2", FCVAR_ARCHIVE, "The default throw force of kick without player velocity." );
ConVar vance_kick_powerscale( "kick_powerscale", "1", FCVAR_ARCHIVE, "The default damage of kick without player velocity." );
ConVar vance_kick_time_adjust("kick_time_adjust", "0.1", FCVAR_CHEAT);
ConVar vance_kick_range("kick_range", "100", FCVAR_CHEAT);
ConVar vance_kick_firerate("kick_firerate", "1", FCVAR_CHEAT);
ConVar vance_kick_damage_mult_min("kick_damage_mult_min", "1.2", FCVAR_CHEAT);
ConVar vance_kick_damage_mult_max("kick_damage_mult_max", "2", FCVAR_CHEAT);
ConVar vance_kick_force_mult_min("kick_force_mult_min", "1", FCVAR_CHEAT);
ConVar vance_kick_force_mult_max("kick_force_mult_max", "2", FCVAR_CHEAT);
ConVar vance_kick_bounce_scale("kick_bounce_scale", "1", FCVAR_CHEAT);
ConVar vance_kick_knockback_scale("kick_knockback_scale", "1", FCVAR_CHEAT);

ConVar vance_frag_roll_angle("frag_roll_angle", "30", FCVAR_NONE);
ConVar vance_frag_redraw_time("frag_redraw_time", "1", FCVAR_NONE);

LINK_ENTITY_TO_CLASS( player, CVancePlayer );
PRECACHE_REGISTER( player );

BEGIN_DATADESC( CVancePlayer )
DEFINE_FIELD(m_ParkourAction, FIELD_INTEGER),
DEFINE_FIELD(m_flSlideEndTime, FIELD_TIME),
DEFINE_FIELD(m_flSlideFrictionScale, FIELD_FLOAT),
DEFINE_FIELD(m_iNumStims, FIELD_INTEGER),
DEFINE_FIELD(m_iNumTourniquets, FIELD_INTEGER),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CVancePlayer, DT_Vance_Player )
	SendPropFloat(SENDINFO(m_flKickAnimLength)),
	SendPropInt(SENDINFO(m_ParkourAction)),
	SendPropFloat(SENDINFO(m_flSlideEndTime)),
	SendPropFloat(SENDINFO(m_flSlideFrictionScale)),
	SendPropVector(SENDINFO(m_vecVaultCameraAdjustment)),
	SendPropBool(SENDINFO(m_bVaulting))
END_SEND_TABLE()

static void Cmd_UseStimOrTourniquet()
{
	CVancePlayer *pPlayer = static_cast<CVancePlayer *>( UTIL_GetCommandClient() );
	if ( pPlayer )
	{
		pPlayer->UseStimOrTourniquet();
	}
}
ConCommand use_stim_or_tourniquet( "use_stim_or_tourniquet", Cmd_UseStimOrTourniquet, "The Player injects an available stimulant to regenerate health.", FCVAR_CLIENTCMD_CAN_EXECUTE );

static void Cmd_Damage( const CCommand &args )
{
	if ( args.ArgC() < 1 || FStrEq( args.Arg( 1 ), "" ) )
	{
		Msg( "Usage: damage [amount of damage you want to take]\n" );
		return;
	}

	CVancePlayer *pPlayer = static_cast<CVancePlayer *>( UTIL_GetCommandClient() );
	if (pPlayer)
	{
		pPlayer->Damage(V_atoi(args.Arg(1)));
	}
}
ConCommand damage( "damage", Cmd_Damage, "Makes the executing Player take damage by a certain amount.", FCVAR_CHEAT );

static void Cmd_Bleed( const CCommand &args )
{
	CVancePlayer *pPlayer = static_cast<CVancePlayer *>( UTIL_GetCommandClient() );
	if ( pPlayer )
	{
		pPlayer->Bleed();
	}
}
ConCommand bleed( "bleed", Cmd_Bleed, "Makes the executing Player start bleeding.", FCVAR_CHEAT );

static void Cmd_Holster(const CCommand &args)
{
	CVancePlayer *pPlayer = static_cast<CVancePlayer *>(UTIL_GetCommandClient());
	if (pPlayer)
	{
		if (pPlayer->GetActiveWeapon() == pPlayer->m_UnarmedWeapon) {
			if (pPlayer->m_HolsteredWeapon) {
				pPlayer->GetActiveWeapon()->Holster(pPlayer->m_HolsteredWeapon);
			}
		} else {
			pPlayer->m_HolsteredWeapon = pPlayer->GetActiveWeapon();
			pPlayer->GetActiveWeapon()->Holster(pPlayer->m_UnarmedWeapon);
		}
	}
}
ConCommand holster("holster", Cmd_Holster, "Holsters the players weapon", FCVAR_NONE);

static void Cmd_Inspect(const CCommand &args)
{
	CVancePlayer *pPlayer = static_cast<CVancePlayer *>(UTIL_GetCommandClient());
	if (pPlayer->CanSwitchViewmodelSequence()) {
		CBaseVanceWeapon *pVanceWeapon = dynamic_cast<CBaseVanceWeapon *>(pPlayer->GetActiveWeapon());
		if (pVanceWeapon) {
			pVanceWeapon->SendWeaponAnim(ACT_VM_INSPECT);
		}
	}
}
ConCommand inspect("inspect", Cmd_Inspect, "plays inspect animation", FCVAR_NONE);

static void Cmd_GiveSuit(const CCommand &args)
{
	CVancePlayer *pPlayer = static_cast<CVancePlayer *>(UTIL_GetCommandClient());
	if (pPlayer)
	{
		pPlayer->EquipSuit(false);
	}
}
ConCommand GiveSuit("GiveSuit", Cmd_GiveSuit, "equips the hev suit", FCVAR_NONE);

static void Cmd_RemoveSuit(const CCommand &args)
{
	CVancePlayer *pPlayer = static_cast<CVancePlayer *>(UTIL_GetCommandClient());
	if (pPlayer)
	{
		pPlayer->RemoveSuit();
	}
}
ConCommand RemoveSuit("RemoveSuit", Cmd_RemoveSuit, "unequips the hev suit", FCVAR_NONE);

CVancePlayer::CVancePlayer()
{
	// Here we create and init the player animation state.
	m_pPlayerAnimState = CreatePlayerAnimationState(this);
	m_angEyeAngles.Init();

	// Code originally written by Valve.
	m_nNumMissPositions = 0;
	m_pPlayerAISquad = 0;
	m_bSprintEnabled = true;

	m_flNextSprint = 0.0f;

	m_fBleedChance = sk_bleed_chance.GetFloat();

	m_flSlideFrictionScale = 1.0f;
}

CVancePlayer::~CVancePlayer()
{
	// Clears the animation state.
	if (m_pPlayerAnimState != NULL)
	{
		m_pPlayerAnimState->Release();
		m_pPlayerAnimState = NULL;
	}
}

CVancePlayer *CVancePlayer::Create( edict_t *pEdict )
{
	CHL2_Player::s_PlayerEdict = pEdict;
	return static_cast<CVancePlayer *>( CreateEntityByName( "player" ) );
}

void CVancePlayer::Precache()
{
	BaseClass::Precache();

	PrecacheModel( P_PLAYER_ALYX );
	PrecacheModel( P_PLAYER_HEV );
	PrecacheModel( P_PLAYER_SYNTH );

	PrecacheModel(P_PLAYER_LEGS_ALYX);
	PrecacheModel(P_PLAYER_LEGS_HEV);
	PrecacheModel(P_PLAYER_LEGS_SYNTH);

	PrecacheModel( C_ARMS_ALYX );
	PrecacheModel( C_ARMS_HEV );
	PrecacheModel( C_ARMS_SYNTH );

	PrecacheModel( V_KICK_ALYX ); //SMOD KICK STUFF!
	PrecacheModel( V_KICK_HEV );
	PrecacheModel( V_KICK_SYNTH );

	PrecacheModel( "models/weapons/v_stim.mdl" );

	PrecacheScriptSound( "AlyxPlayer.KickHit" );
	PrecacheScriptSound( "AlyxPlayer.KickMiss" );
	
	PrecacheScriptSound("Grenade.Blip");
	PrecacheScriptSound("HL2Player.UseDeny");

	PrecacheScriptSound( "AlyxPlayer.PainHeavy" );

	PrecacheScriptSound("AlyxPlayer.InjectStim");

	PrecacheScriptSound( "AlyxPlayer.SprintPain" );
	PrecacheScriptSound( "AlyxPlayer.Sprint" );

	PrecacheScriptSound("AlyxPlayer.Slide_default_start");
	PrecacheScriptSound("AlyxPlayer.Slide_default_end");

	PrecacheScriptSound( "AlyxPlayer.Jump" );
	PrecacheScriptSound( "AlyxPlayer.JumpGear" );
	PrecacheScriptSound( "AlyxPlayer.Land" );

	PrecacheScriptSound( "AlyxPlayer.Die" );
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : iImpulse - 
//-----------------------------------------------------------------------------
void CVancePlayer::CheatImpulseCommands(int iImpulse)
{
	switch (iImpulse)
	{
	case 101:
		gEvilImpulse101 = true;

		EquipSuit(false);

		// Give the player everything!
		CBaseCombatCharacter::GiveAmmo(255, "Pistol");
		CBaseCombatCharacter::GiveAmmo(255, "AR2");
		CBaseCombatCharacter::GiveAmmo(5, "AR2AltFire");
		CBaseCombatCharacter::GiveAmmo(255, "SMG1");
		CBaseCombatCharacter::GiveAmmo(255, "Buckshot");
		CBaseCombatCharacter::GiveAmmo(3, "smg1_grenade");
		CBaseCombatCharacter::GiveAmmo(3, "rpg_round");
		CBaseCombatCharacter::GiveAmmo(5, "grenade");
		CBaseCombatCharacter::GiveAmmo(32, "357");
		CBaseCombatCharacter::GiveAmmo(16, "XBowBolt");
		CBaseCombatCharacter::GiveAmmo(5, "Hopwire");
		GiveNamedItem("weapon_smg1");
		GiveNamedItem("weapon_frag");
		GiveNamedItem("weapon_crowbar");
		GiveNamedItem("weapon_resistancegun");
		GiveNamedItem("weapon_ar2");
		GiveNamedItem("weapon_shotgun");
		//GiveNamedItem("weapon_physcannon");
		GiveNamedItem("weapon_bugbait");
		GiveNamedItem("weapon_rpg");
		GiveNamedItem("weapon_357");
		GiveNamedItem("weapon_sniper");
		GiveNamedItem("weapon_anastasia");
		GiveNamedItem("weapon_akms");
		GiveNamedItem("weapon_gauss");
		if (GetHealth() < 100)
		{
			TakeHealth(25, DMG_GENERIC);
		}

		GiveStim( 3 );
		GiveTourniquet( 3 );

		gEvilImpulse101 = false;

		break;
	default:
		BaseClass::CheatImpulseCommands(iImpulse);
	}
}

void CVancePlayer::EquipSuit(bool bPlayEffects)
{
	MDLCACHE_CRITICAL_SECTION();
	bool bHadSuit = IsSuitEquipped();

	BaseClass::EquipSuit();

	m_HL2Local.m_bDisplayReticle = true;

	// Force redeploy weapon to update the arm model.
	if (!bHadSuit && (GetActiveWeapon() && !IsInAVehicle()))
	{
		PhysCannonForceDrop(GetActiveWeapon(), NULL);
		GetActiveWeapon()->Deploy();
	}

	// Update that bod.
	if (IsSuitEquipped() != bHadSuit)
	{
		SetModel(GetPlayerWorldModel());

		CBaseViewModel* pLeg = GetViewModel(VM_LEGS);
		if (pLeg)
			pLeg->SetWeaponModel(GetLegsViewModel(), NULL);
	}

	if (bPlayEffects)
	{
		StartAdmireGlovesAnimation();
	}
}

void CVancePlayer::EquipSynth(bool bPlayEffects)
{
	MDLCACHE_CRITICAL_SECTION();
	bool bHadSynth = IsSynthEquipped();

	m_Local.m_bWearingSynth = true;

	m_HL2Local.m_bDisplayReticle = true;

	// Force redeploy weapon to update the arm model.
	if (!bHadSynth && (GetActiveWeapon() && !IsInAVehicle()))
	{
		PhysCannonForceDrop(GetActiveWeapon(), NULL);
		GetActiveWeapon()->Deploy();
	}

	// Update that bod.
	if (IsSynthEquipped() != bHadSynth)
	{
		SetModel(GetPlayerWorldModel());

		CBaseViewModel* pLeg = GetViewModel(VM_LEGS);
		if (pLeg)
			pLeg->SetWeaponModel(GetLegsViewModel(), NULL);
	}

	if (bPlayEffects)
	{
		StartAdmireGlovesAnimation();
	}
}

void CVancePlayer::RemoveSuit()
{
	bool bHadSuit = IsSuitEquipped();

	BaseClass::RemoveSuit();

	m_HL2Local.m_bDisplayReticle = false;

	// Force redeploy weapon to update the arm model.
	if (bHadSuit && (GetActiveWeapon() && !IsInAVehicle()))
	{
		PhysCannonForceDrop(GetActiveWeapon(), NULL);
		GetActiveWeapon()->Deploy();
	}

	// Update that bod.
	if (IsSuitEquipped() != bHadSuit)
	{
		SetModel(GetPlayerWorldModel());

		CBaseViewModel* pLeg = GetViewModel(VM_LEGS);
		if (pLeg)
			pLeg->SetWeaponModel(GetLegsViewModel(), NULL);
	}
}

void CVancePlayer::RemoveSynth()
{
	bool bHadSynth = IsSynthEquipped();

	m_Local.m_bWearingSynth = false;

	m_HL2Local.m_bDisplayReticle = false;

	// Force redeploy weapon to update the arm model.
	if (bHadSynth && (GetActiveWeapon() && !IsInAVehicle()))
	{
		PhysCannonForceDrop(GetActiveWeapon(), NULL);
		GetActiveWeapon()->Deploy();
	}

	// Update that bod.
	if (IsSynthEquipped() != bHadSynth)
	{
		SetModel(GetPlayerWorldModel());

		CBaseViewModel* pLeg = GetViewModel(VM_LEGS);
		if (pLeg)
			pLeg->SetWeaponModel(GetLegsViewModel(), NULL);
	}
}

void CVancePlayer::CreateViewModel(int iViewModel /*= 0*/)
{
	Assert(iViewModel >= 0 && iViewModel < MAX_VIEWMODELS);

	if (GetViewModel(iViewModel))
		return;

	CVanceViewModel *pViewModel = (CVanceViewModel *)CreateEntityByName("vance_viewmodel");
	if (pViewModel)
	{
		pViewModel->SetAbsOrigin(GetAbsOrigin());
		pViewModel->SetOwner(this);
		pViewModel->SetIndex(iViewModel);
		DispatchSpawn(pViewModel);
		pViewModel->FollowEntity(this, false);
		pViewModel->SetOwnerEntity(this);

		m_hViewModel.Set(iViewModel, pViewModel);
	}
}

void CVancePlayer::HandleSpeedChanges()
{
	int buttonsChanged = m_afButtonPressed | m_afButtonReleased;

	bool bCanSprint = CanSprint();
	bool bIsSprinting = IsSprinting();
	bool bWantSprint = (bCanSprint && (m_nButtons & IN_SPEED));
	if (bIsSprinting != bWantSprint && (buttonsChanged & IN_SPEED))
	{
		// If someone wants to sprint, make sure they've pressed the button to do so. We want to prevent the
		// case where a player can hold down the sprint key and burn tiny bursts of sprint as the suit recharges
		// We want a full debounce of the key to resume sprinting after the suit is completely drained
		if (bWantSprint)
		{
			if (sv_stickysprint.GetBool())
			{
				StartAutoSprint();
			}
			else
			{
				StartSprinting();
			}
		}
		else
		{
			if (!sv_stickysprint.GetBool())
			{
				StopSprinting();
			}

			// Reset key, so it will be activated post whatever is suppressing it.
			m_nButtons &= ~IN_SPEED;
		}
	}

	bool bIsWalking = IsWalking();
	bool bWantWalking;
	if (IsSuitEquipped())
	{
		bWantWalking = (m_nButtons & IN_WALK) && !IsSprinting() && !(m_nButtons & IN_DUCK);
	}
	else
	{
		bWantWalking = !IsSprinting() && !(m_nButtons & IN_DUCK);
	}

	if (bIsWalking != bWantWalking)
	{
		if (bWantWalking)
		{
			StartWalking();
		}
		else
		{
			StopWalking();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Allow pre-frame adjustments on the player
//-----------------------------------------------------------------------------
void CVancePlayer::PreThink()
{
	if (m_PerformingGesture != GestureAction::None)
	{
		m_nButtons &= ~(IN_ATTACK | IN_ALT1 | IN_ALT2 | IN_GRENADE1 | IN_GRENADE2 | IN_ATTACK2 | IN_RELOAD);
	}

	if (player_showpredictedposition.GetBool())
	{
		Vector	predPos;

		UTIL_PredictedPosition(this, player_showpredictedposition_timestep.GetFloat(), &predPos);

		NDebugOverlay::Box(predPos, NAI_Hull::Mins(GetHullType()), NAI_Hull::Maxs(GetHullType()), 0, 255, 0, 0, 0.01f);
		NDebugOverlay::Line(GetAbsOrigin(), predPos, 0, 255, 0, 0, 0.01f);
	}

	if (m_hLocatorTargetEntity != NULL)
	{
		// Keep track of the entity here, the client will pick up the rest of the work
		m_HL2Local.m_vecLocatorOrigin = m_hLocatorTargetEntity->WorldSpaceCenter();
	}
	else
	{
		m_HL2Local.m_vecLocatorOrigin = vec3_invalid; // This tells the client we have no locator target.
	}

	// Riding a vehicle?
	if (IsInAVehicle())
	{
		VPROF("CHL2_Player::PreThink-Vehicle");
		// make sure we update the client, check for timed damage and update suit even if we are in a vehicle
		UpdateClientData();
		CheckTimeBasedDamage();

		// Allow the suit to recharge when in the vehicle.
		SuitPower_Update();
		CheckSuitUpdate();
		CheckSuitZoom();

		WaterMove();
		return;
	}

	// This is an experiment of mine- autojumping! 
	// only affects you if sv_autojump is nonzero.
	if ((GetFlags() & FL_ONGROUND) && sv_autojump.GetFloat() != 0)
	{
		VPROF("CHL2_Player::PreThink-Autojump");
		// check autojump
		Vector vecCheckDir;

		vecCheckDir = GetAbsVelocity();

		float flVelocity = VectorNormalize(vecCheckDir);

		if (flVelocity > 200)
		{
			// Going fast enough to autojump
			vecCheckDir = WorldSpaceCenter() + vecCheckDir * 34 - Vector(0, 0, 16);

			trace_t tr;

			UTIL_TraceHull(WorldSpaceCenter() - Vector(0, 0, 16), vecCheckDir, NAI_Hull::Mins(HULL_TINY_CENTERED), NAI_Hull::Maxs(HULL_TINY_CENTERED), MASK_PLAYERSOLID, this, COLLISION_GROUP_PLAYER, &tr);

			//NDebugOverlay::Line( tr.startpos, tr.endpos, 0,255,0, true, 10 );

			if (tr.fraction == 1.0 && !tr.startsolid)
			{
				// Now trace down!
				UTIL_TraceLine(vecCheckDir, vecCheckDir - Vector(0, 0, 64), MASK_PLAYERSOLID, this, COLLISION_GROUP_NONE, &tr);

				//NDebugOverlay::Line( tr.startpos, tr.endpos, 0,255,0, true, 10 );

				if (tr.fraction == 1.0 && !tr.startsolid)
				{
					// !!!HACKHACK
					// I KNOW, I KNOW, this is definitely not the right way to do this,
					// but I'm prototyping! (sjb)
					Vector vecNewVelocity = GetAbsVelocity();
					vecNewVelocity.z += 250;
					SetAbsVelocity(vecNewVelocity);
				}
			}
		}
	}

	// Manually force us to sprint again if sprint is interrupted.
	if (m_flNextSprint != 0.0f && m_flNextSprint < gpGlobals->curtime)
	{
		m_flNextSprint = 0.0f;
		if (m_nButtons & IN_SPEED)
		{
			m_afButtonPressed |= IN_SPEED;
		}
	}

	if (m_flNextWalk != 0.0f && m_flNextWalk < gpGlobals->curtime)
	{
		m_flNextWalk = 0.0f;
	}

	VPROF_SCOPE_BEGIN("CHL2_Player::PreThink-Speed");
	HandleSpeedChanges();
	HandleArmorReduction();

	if (sv_stickysprint.GetBool() && m_bIsAutoSprinting)
	{
		// If we're ducked and not in the air
		if (IsDucked() && GetGroundEntity() != NULL)
		{
			StopSprinting();
		}
		// Stop sprinting if the player lets off the stick for a moment.
		else if (GetStickDist() == 0.0f)
		{
			if (gpGlobals->curtime > m_fAutoSprintMinTime)
			{
				StopSprinting();
			}
		}
		else
		{
			// Stop sprinting one half second after the player stops inputting with the move stick.
			m_fAutoSprintMinTime = gpGlobals->curtime + 0.5f;
		}
	}
	else if (IsSprinting())
	{
		// Disable sprint while ducked unless we're in the air (jumping)
		if (IsDucked() && (GetGroundEntity() != NULL))
		{
			StopSprinting();
		}
	}

	VPROF_SCOPE_END();

	if (g_fGameOver || IsPlayerLockedInPlace())
		return;         // finale

	VPROF_SCOPE_BEGIN("CHL2_Player::PreThink-ItemPreFrame");
	ItemPreFrame();
	VPROF_SCOPE_END();

	VPROF_SCOPE_BEGIN("CHL2_Player::PreThink-WaterMove");
	WaterMove();
	VPROF_SCOPE_END();

	if (g_pGameRules && g_pGameRules->FAllowFlashlight())
		m_Local.m_iHideHUD &= ~HIDEHUD_FLASHLIGHT;
	else
		m_Local.m_iHideHUD |= HIDEHUD_FLASHLIGHT;


	VPROF_SCOPE_BEGIN("CHL2_Player::PreThink-CommanderUpdate");
	CommanderUpdate();
	VPROF_SCOPE_END();

	// Operate suit accessories and manage power consumption/charge
	VPROF_SCOPE_BEGIN("CHL2_Player::PreThink-SuitPower_Update");
	SuitPower_Update();
	VPROF_SCOPE_END();

	// checks if new client data (for HUD and view control) needs to be sent to the client
	VPROF_SCOPE_BEGIN("CHL2_Player::PreThink-UpdateClientData");
	UpdateClientData();
	VPROF_SCOPE_END();

	VPROF_SCOPE_BEGIN("CHL2_Player::PreThink-CheckTimeBasedDamage");
	CheckTimeBasedDamage();
	VPROF_SCOPE_END();

	VPROF_SCOPE_BEGIN("CHL2_Player::PreThink-CheckSuitUpdate");
	CheckSuitUpdate();
	VPROF_SCOPE_END();

	VPROF_SCOPE_BEGIN("CHL2_Player::PreThink-CheckSuitZoom");
	CheckSuitZoom();
	VPROF_SCOPE_END();

	if (m_lifeState >= LIFE_DYING)
	{
		PlayerDeathThink();
		return;
	}

	CheckFlashlight();

	// So the correct flags get sent to client asap.
	//
	if (m_afPhysicsFlags & PFLAG_DIROVERRIDE)
		AddFlag(FL_ONTRAIN);
	else
		RemoveFlag(FL_ONTRAIN);

	// Train speed control
	if (m_afPhysicsFlags & PFLAG_DIROVERRIDE)
	{
		CBaseEntity *pTrain = GetGroundEntity();
		if ( pTrain && !( pTrain->ObjectCaps() & FCAP_DIRECTIONAL_USE ) )
		{
			pTrain = nullptr;
		}

		if (!pTrain)
		{
			if (GetActiveWeapon() && (GetActiveWeapon()->ObjectCaps() & FCAP_DIRECTIONAL_USE))
			{
				m_iTrain = TRAIN_ACTIVE | TRAIN_NEW;

				if (m_nButtons & IN_FORWARD)
				{
					m_iTrain |= TRAIN_FAST;
				}
				else if (m_nButtons & IN_BACK)
				{
					m_iTrain |= TRAIN_BACK;
				}
				else
				{
					m_iTrain |= TRAIN_NEUTRAL;
				}
				return;
			}
			else
			{
				trace_t trainTrace;
				// Maybe this is on the other side of a level transition
				UTIL_TraceLine(GetAbsOrigin(), GetAbsOrigin() + Vector(0, 0, -38),
					MASK_PLAYERSOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, &trainTrace);

				if (trainTrace.fraction != 1.0 && trainTrace.m_pEnt)
					pTrain = trainTrace.m_pEnt;


				if (!pTrain || !(pTrain->ObjectCaps() & FCAP_DIRECTIONAL_USE) || !pTrain->OnControls(this))
				{
					//					Warning( "In train mode with no train!\n" );
					m_afPhysicsFlags &= ~PFLAG_DIROVERRIDE;
					m_iTrain = TRAIN_NEW | TRAIN_OFF;
					return;
				}
			}
		}
		else if (!(GetFlags() & FL_ONGROUND) || pTrain->HasSpawnFlags(SF_TRACKTRAIN_NOCONTROL) || (m_nButtons & (IN_MOVELEFT | IN_MOVERIGHT)))
		{
			// Turn off the train if you jump, strafe, or the train controls go dead
			m_afPhysicsFlags &= ~PFLAG_DIROVERRIDE;
			m_iTrain = TRAIN_NEW | TRAIN_OFF;
			return;
		}

		SetAbsVelocity(vec3_origin);
		float vel = 0;
		if (m_afButtonPressed & IN_FORWARD)
		{
			vel = 1;
			pTrain->Use(this, this, USE_SET, (float)vel);
		}
		else if (m_afButtonPressed & IN_BACK)
		{
			vel = -1;
			pTrain->Use(this, this, USE_SET, (float)vel);
		}

		if (vel)
		{
			m_iTrain = TrainSpeed(pTrain->m_flSpeed, ((CFuncTrackTrain*)pTrain)->GetMaxSpeed());
			m_iTrain |= TRAIN_ACTIVE | TRAIN_NEW;
		}
	}
	else if (m_iTrain & TRAIN_ACTIVE)
	{
		m_iTrain = TRAIN_NEW; // turn off train
	}

	//
	// If we're not on the ground, we're falling. Update our falling velocity.
	//
	if (!(GetFlags() & FL_ONGROUND))
	{
		m_Local.m_flFallVelocity = -GetAbsVelocity().z;
	}

	if (m_afPhysicsFlags & PFLAG_ONBARNACLE)
	{
		bool bOnBarnacle = false;
		CNPC_Barnacle* pBarnacle = NULL;
		do
		{
			// FIXME: Not a good or fast solution, but maybe it will catch the bug!
			pBarnacle = (CNPC_Barnacle*)gEntList.FindEntityByClassname(pBarnacle, "npc_barnacle");
			if (pBarnacle)
			{
				if (pBarnacle->GetEnemy() == this)
				{
					bOnBarnacle = true;
				}
			}
		} while (pBarnacle);

		if (!bOnBarnacle)
		{
			Warning("Attached to barnacle?\n");
			Assert(0);
			m_afPhysicsFlags &= ~PFLAG_ONBARNACLE;
		}
		else
		{
			SetAbsVelocity(vec3_origin);
		}
	}

	// Update weapon's ready status
	UpdateWeaponPosture();

	if (m_nButtons & IN_ZOOM)
	{
		CBaseCombatWeapon *pWeapon = GetActiveWeapon();
		CBaseVanceWeapon *pVanceWeapon = dynamic_cast<CBaseVanceWeapon *>( pWeapon );
		bool canAttackWhileZoom = pVanceWeapon ? pVanceWeapon->CanAttackWhileZoomed() : false;

		// If we're using a mounted gun or holding a grenade
		// or the Alyx Gun, allow attacking

		// using entity or have visible weapon
		// clear buttons
		if ( ( !m_hUseEntity || ( pWeapon && pWeapon->IsWeaponVisible() ) ) && !canAttackWhileZoom )
		{
			m_nButtons &= ~( IN_ATTACK | IN_ATTACK2 );
		}
	}
}

bool CVancePlayer::Weapon_CanUse(CBaseCombatWeapon* pWeapon)
{
	if (pWeapon->ClassMatches("weapon_pistol"))
	{
		if (GiveAmmo(50, GetAmmoDef()->Index("Pistol"), false))
			UTIL_Remove(pWeapon);
		return false;
	}
	if (pWeapon->ClassMatches("weapon_frag"))
	{
		if (GiveAmmo(1, GetAmmoDef()->Index("Grenade"), false))
			UTIL_Remove(pWeapon);
		return false;
	}
	if (pWeapon->ClassMatches("weapon_physcannon"))
		return false;

	return BaseClass::Weapon_CanUse(pWeapon);
}

int CVancePlayer::OnTakeDamage(const CTakeDamageInfo &inputInfo)
{
	int bitsDamage = inputInfo.GetDamageType();
	CTakeDamageInfo info = inputInfo;

	if (m_debugOverlays & OVERLAY_BUDDHA_MODE && (m_iHealth - info.GetDamage()) <= 0)
	{
		m_iHealth = 1;
		return 0;
	}

	IServerVehicle *pVehicle = GetVehicle();
	if (pVehicle)
	{
		// Let the vehicle decide if we should take this damage or not
		if (pVehicle->PassengerShouldReceiveDamage(info) == false)
			return 0;
	}

	// Early out if there's no damage
	if (!info.GetDamage() || !IsAlive() || !g_pGameRules->FPlayerCanTakeDamage(this, info.GetAttacker(), inputInfo) || GetFlags() & FL_GODMODE)
		return 0;

	float flRatio = 0.2f;
	float flBonus = 1.0f;

	if ((info.GetDamageType() & DMG_BLAST) && g_pGameRules->IsMultiplayer())
		flBonus *= 2; // blasts damage armor more.

	// keep track of amount of damage last sustained
	m_lastDamageAmount = info.GetDamage();

	// Armor.
	int armorValue = m_PlayerInfo.GetArmorValue();
	if (armorValue && !(info.GetDamageType() & (DMG_FALL | DMG_DROWN | DMG_POISON | DMG_RADIATION)))
	{
		float flNew = info.GetDamage() * flRatio;
		float flArmor = (info.GetDamage() - flNew) * flBonus;
		if (flArmor < 1.0)
			flArmor = 1.0;

		// Does this use more armor than we have?
		if (flArmor > armorValue)
		{
			flArmor = armorValue;
			flArmor *= (1 / flBonus);
			flNew = info.GetDamage() - flArmor;
			m_DmgSave = armorValue;
			SetArmorValue(0);
		}
		else
		{
			m_DmgSave = flArmor;
			SetArmorValue(armorValue - flArmor);
		}

		info.SetDamage(flNew);
	}

	// Call up to the base class
	int fTookDamage = CBaseCombatCharacter::OnTakeDamage(info);

	// Early out if the base class took no damage
	if (!fTookDamage)
		return 0;

	// add to the damage total for clients, which will be sent as a single
	// message at the end of the frame
	// todo: remove after combining shotgun blasts?
	if (info.GetInflictor() && info.GetInflictor()->edict())
		m_DmgOrigin = info.GetInflictor()->GetAbsOrigin();

	m_DmgTake += (int)info.GetDamage();

	// Reset damage time countdown for each type of time based damage player just sustained
	for (int i = 0; i < CDMG_TIMEBASED; i++)
	{
		// Make sure the damage type is really time-based.
		// This is kind of hacky but necessary until we setup DamageType as an enum.
		int iDamage = (DMG_PARALYZE << i);
		if ((info.GetDamageType() & iDamage) && g_pGameRules->Damage_IsTimeBased(iDamage))
			m_rgbTimeBasedDamage[i] = 0;
	}

	// Display any effect associate with this damage type
	DamageEffect(info.GetDamage(), bitsDamage);

	// Severity of the damage.
	bool bTrivial = (m_iHealth > 75 || m_lastDamageAmount < 5);
	bool bMajor = (m_lastDamageAmount > 25);
	bool bHeavy = (m_iHealth < 50);
	bool bCritical = (m_iHealth < 30);

	if ( m_iHealth < 1 )
	{
		EmitSound( "AlyxPlayer.Die" ); // Death sound
	}
	else
	{
		// Don't make pain sounds for a pinch of damage
		if ( !bTrivial && m_flNextPainSound < gpGlobals->curtime )
		{
			if ( bHeavy )
				EmitSound( "AlyxPlayer.PainHeavy" ); // Heavy Pain sound
			else
				EmitSound( "Player.Death" ); // should be "Player.Pain" or something, bad name...

			m_flNextPainSound = gpGlobals->curtime + RandomFloat( 2.0f, 3.0f );
		}
	}


	m_bitsDamageType |= bitsDamage; // Save this so we can report it to the client
	m_bitsHUDDamage = -1;  // make sure the damage bits get resent

	#define MASK_CANBLEED ( DMG_SLASH | DMG_FALL | DMG_BULLET | DMG_CLUB )

	if ( ArmorValue() == 0 && bitsDamage & MASK_CANBLEED && !m_bStimRegeneration )
	{
		m_fLastDamageTime = gpGlobals->curtime;

		float chance = random->RandomFloat( 0.0f, 100.0f );
		if ( chance <= m_fBleedChance )
		{
			Bleed();
			SetSuitUpdate( "!HEV_DMG6", false, SUIT_NEXT_IN_30SEC ); // blood loss detected
			bitsDamage &= ~MASK_CANBLEED;
		}
		else
		{
			m_fBleedChance += sk_bleed_chance_increment.GetFloat();
		}
	}

	bool bFound = true;
	while ((!bTrivial || g_pGameRules->Damage_IsTimeBased(bitsDamage)) && bFound && bitsDamage)
	{
		bFound = false;

		if (bitsDamage & DMG_BLEED)
		{
			bitsDamage &= ~DMG_BLEED;
			bFound = true;
		}
		if (bitsDamage & DMG_CRUSH)
		{
			if (bMajor)
				SetSuitUpdate("!HEV_DMG5", false, SUIT_NEXT_IN_30SEC);	// major fracture
			else
				SetSuitUpdate("!HEV_DMG4", false, SUIT_NEXT_IN_30SEC);	// minor fracture

			bitsDamage &= ~DMG_CRUSH;
			bFound = true;
		}
		if (bitsDamage & DMG_SLASH)
		{
			if (bMajor)
				SetSuitUpdate("!HEV_DMG1", false, SUIT_NEXT_IN_30SEC);	// major laceration
			else
				SetSuitUpdate("!HEV_DMG0", false, SUIT_NEXT_IN_30SEC);	// minor laceration
		}
		if (bitsDamage & DMG_FALL)
		{
			if (bMajor)
				SetSuitUpdate("!HEV_DMG5", false, SUIT_NEXT_IN_30SEC);	// major fracture
			else
				SetSuitUpdate("!HEV_DMG4", false, SUIT_NEXT_IN_30SEC);	// minor fracture
		}
		if (bitsDamage & DMG_CLUB)
		{
			if (bMajor)
				SetSuitUpdate("!HEV_DMG4", false, SUIT_NEXT_IN_30SEC);	// minor fracture
		}
		if (bitsDamage & DMG_SLASH)
		{
			if (bMajor)
				SetSuitUpdate("!HEV_DMG1", false, SUIT_NEXT_IN_30SEC);	// major laceration
			else
				SetSuitUpdate("!HEV_DMG0", false, SUIT_NEXT_IN_30SEC);	// minor laceration

			bitsDamage &= ~DMG_SLASH;
			bFound = true;
		}
		if (bitsDamage & DMG_SONIC)
		{
			if (bMajor)
				SetSuitUpdate("!HEV_DMG2", false, SUIT_NEXT_IN_1MIN);	// internal bleeding
			bitsDamage &= ~DMG_SONIC;
			bFound = true;
		}
		if (bitsDamage & (DMG_POISON | DMG_PARALYZE) && IsSuitEquipped()
			&& g_pGameRules->Damage_IsTimeBased(bitsDamage))
		{
			if (bitsDamage & DMG_POISON)
			{
				m_nPoisonDmg += info.GetDamage();
				m_tbdPrev = gpGlobals->curtime;
				m_rgbTimeBasedDamage[itbd_PoisonRecover] = 0;
			}

			SetSuitUpdate("!HEV_DMG3", false, SUIT_NEXT_IN_1MIN);	// blood toxins detected
			bitsDamage &= ~(DMG_POISON | DMG_PARALYZE);
			bFound = true;
		}
		if (bitsDamage & DMG_ACID)
		{
			SetSuitUpdate("!HEV_DET1", false, SUIT_NEXT_IN_1MIN);	// hazardous chemicals detected
			bitsDamage &= ~DMG_ACID;
			bFound = true;
		}
		if (bitsDamage & DMG_NERVEGAS)
		{
			SetSuitUpdate("!HEV_DET0", false, SUIT_NEXT_IN_1MIN);	// biohazard detected
			bitsDamage &= ~DMG_NERVEGAS;
			bFound = true;
		}
		if (bitsDamage & DMG_RADIATION)
		{
			SetSuitUpdate("!HEV_DET2", false, SUIT_NEXT_IN_1MIN);	// radiation detected
			bitsDamage &= ~DMG_RADIATION;
			bFound = true;
		}
		if (bitsDamage & DMG_SHOCK)
		{
			bitsDamage &= ~DMG_SHOCK;
			bFound = true;
		}
	}

	float flPunch = -2;

	if (info.GetAttacker() && !FInViewCone(info.GetAttacker()))
	{
		if (info.GetDamage() > 10.0f)
			flPunch = -10;
		else
			flPunch = RandomFloat(-5, -7);
	}

	m_Local.m_vecPunchAngle.SetX(flPunch);

	float flHealthPrev = m_iHealth;
	if (!bTrivial && bMajor && flHealthPrev >= 75)
	{
		// first time we take major damage...
		// turn automedic on if not on
		SetSuitUpdate("!HEV_MED1", false, SUIT_NEXT_IN_30MIN);	// automedic on

		// give morphine shot if not given recently
		SetSuitUpdate("!HEV_HEAL7", false, SUIT_NEXT_IN_30MIN);	// morphine shot
	}

	if (!bTrivial && bCritical && flHealthPrev < 75)
	{
		// already took major damage, now it's critical...
		if (m_iHealth < 6)
			SetSuitUpdate("!HEV_HLTH3", false, SUIT_NEXT_IN_10MIN);	// near death
		else if (m_iHealth < 20)
			SetSuitUpdate("!HEV_HLTH2", false, SUIT_NEXT_IN_10MIN);	// health critical

		// give critical health warnings
		if (!random->RandomInt(0, 3) && flHealthPrev < 50)
			SetSuitUpdate("!HEV_DMG7", false, SUIT_NEXT_IN_5MIN); //seek medical attention
	}

	// if we're taking time based damage, warn about its continuing effects
	if (g_pGameRules->Damage_IsTimeBased(info.GetDamageType()) && flHealthPrev < 75)
	{
		if (flHealthPrev < 50)
		{
			if (!random->RandomInt(0, 3))
				SetSuitUpdate("!HEV_DMG7", false, SUIT_NEXT_IN_5MIN); //seek medical attention
		}
		else
		{
			SetSuitUpdate("!HEV_HLTH1", false, SUIT_NEXT_IN_10MIN);	// health dropping
		}
	}

	// Do special explosion damage effect
	if (bitsDamage & DMG_BLAST)
		OnDamagedByExplosion(info);

	return 1;
}

void CVancePlayer::Heal( int health )
{
	SetHealth( Clamp( GetHealth() + health, 0, GetMaxHealth() ) );
}

static void Cmd_Heal(const CCommand &args)
{
	if (args.ArgC() < 1 || FStrEq(args.Arg(1), ""))
	{
		Msg("Usage: heal [amount of damage you want to heal]\n");
		return;
	}

	CVancePlayer *pPlayer = (CVancePlayer *)UTIL_GetLocalPlayer();
	if (pPlayer)
		pPlayer->Heal(V_atoi(args.Arg(1)));
}
ConCommand heal("heal", Cmd_Heal, "Makes the executing Player heal damage by a certain amount.", FCVAR_CHEAT);

void CVancePlayer::Damage(int damage)
{
	CTakeDamageInfo info;
	info.SetInflictor(this);
	info.SetDamage(damage);
	info.SetDamageType(DMG_DIRECT);

	TakeDamage(info);
}

void CVancePlayer::Bleed()
{
	m_bBleeding = true;
	m_fNextBleedTime = gpGlobals->curtime + sk_bleed_dmg_interval.GetFloat();
	m_fBleedEndTime = gpGlobals->curtime + sk_bleed_dmg_interval.GetFloat() + sk_bleed_lifetime.GetFloat();
}

void CVancePlayer::UseStimOrTourniquet()
{
	//if we are in a vehicle then its gonna be the vehicles job to trigger the gestures, since it has to wait for the vehicle bodys hand to go offscreen
	//for now we just wont worry about it
	if (IsInAVehicle())
		return;

	if ( m_bBleeding && m_iNumTourniquets > 0 )
	{
		UseTourniquet();
		return;
	}
	else if ( GetHealth() < GetMaxHealth() && m_iNumStims > 0 )
	{
		UseStim();
		return;
	}
	m_bPlayUseDenySound = true;
}

void CVancePlayer::UseTourniquet()
{
	// Player must not be performing an action or
	// interrupt stim
	if ( m_PerformingGesture == GestureAction::None
		|| m_PerformingGesture == GestureAction::InjectingStim )
	{
		m_PerformingGesture = GestureAction::EquippingTourniquet;
	}
	else {
		return;
	}

	CVanceViewModel *pViewModel = static_cast<CVanceViewModel *>( GetViewModel() );
	if ( pViewModel )
	{
		pViewModel->SetWeaponModel( "models/weapons/v_stim.mdl", nullptr );

		int idealSequence = pViewModel->SelectWeightedSequence(ACT_VM_SPRINT);
		if ( idealSequence >= 0 )
		{
			pViewModel->SendViewModelMatchingSequence( idealSequence );
			m_fGestureFinishTime = gpGlobals->curtime + pViewModel->SequenceDuration();
		}
	}
	if (GetActiveWeapon())
	{
		GetActiveWeapon()->m_fDoNotDisturb = m_fGestureFinishTime;
		GetActiveWeapon()->AbortReload();
	}
}

void CVancePlayer::UseStim()
{
	// Player must not be performing an action or 
	// interrupt tourniquet
	if ( m_PerformingGesture == GestureAction::None
		|| m_PerformingGesture == GestureAction::EquippingTourniquet )
	{
		m_PerformingGesture = GestureAction::InjectingStim;
	}
	else {
		return;
	}

	CVanceViewModel *pViewModel = static_cast<CVanceViewModel *>( GetViewModel() );
	if ( pViewModel )
	{
		pViewModel->SetWeaponModel( "models/weapons/v_stim.mdl", nullptr );

		int idealSequence = pViewModel->SelectWeightedSequence(ACT_VM_IDLE);
		if ( idealSequence >= 0 )
		{
			pViewModel->SendViewModelMatchingSequence( idealSequence );
			m_fGestureFinishTime = gpGlobals->curtime + pViewModel->SequenceDuration();
		}
	}
	if (GetActiveWeapon())
	{
		GetActiveWeapon()->m_fDoNotDisturb = m_fGestureFinishTime;
		GetActiveWeapon()->AbortReload();
	}
}

//apply either the stim or tourniquette, depending on whats onscreen (AE_SV_STARTHEAL)
void CVancePlayer::StartHealing()
{
	if (m_PerformingGesture == GestureAction::InjectingStim) {
		m_bStimRegeneration = true;
		m_fStimRegenerationNextHealTime = gpGlobals->curtime + sk_stim_heal_interval.GetFloat();
		m_fStimRegenerationEndTime = gpGlobals->curtime + sk_stim_heal_interval.GetFloat() + sk_stim_regen_lifetime.GetFloat();
		m_iNumStims--;
		EmitSound("AlyxPlayer.InjectStim");
		color32 green = { 0, 255, 128, 50 };
		UTIL_ScreenFade(this, green, 0.5f, 0.0f, FFADE_IN);
	} else if (m_PerformingGesture == GestureAction::EquippingTourniquet) {
		m_bBleeding = false;
		m_iNumTourniquets--;
	}
}

void CVancePlayer::ThrowGrenade()
{
	//is it time..
	if (m_fNextThrowTime >= gpGlobals->curtime)
		return;

	// Player must not be performing an action
	if (m_PerformingGesture == GestureAction::None)
	{
		m_PerformingGesture = GestureAction::ThrowingGrenade;
	}
	else {
		return;
	}

	CVanceViewModel *pViewModel = static_cast<CVanceViewModel *>(GetViewModel());
	if (pViewModel)
	{
		pViewModel->SetWeaponModel("models/weapons/v_grenade.mdl", nullptr);

		int idealSequence = pViewModel->SelectWeightedSequence(ACT_VM_DRAW);
		if (idealSequence >= 0)
		{
			pViewModel->SendViewModelMatchingSequence(idealSequence);
			m_fGestureFinishTime = gpGlobals->curtime + pViewModel->SequenceDuration();
		}
	}

	m_CurrentWeapon = GetActiveWeapon();
	m_fTimeToReady = m_fGestureFinishTime;
	m_fNextBlipTime = m_fGestureFinishTime + 0.05f;
	m_fNadeDetTime = m_fGestureFinishTime + 3.0f;
	m_fWarnAITime = m_fNadeDetTime - FRAG_GRENADE_WARN_TIME;
	m_fGestureFinishTime += 0.5;
	StopSprinting();
	m_flNextSprint = m_fGestureFinishTime;
	m_bWarnedAI = false;
	m_bRoll = true;
	if (GetActiveWeapon())
	{
		GetActiveWeapon()->m_fDoNotDisturb = m_fGestureFinishTime;
		GetActiveWeapon()->AbortReload();
	}
}

void CVancePlayer::ThrowingGrenade()
{
	//if the player switched weapons cancel
	if (GetActiveWeapon() != m_CurrentWeapon)
	{
		m_fGestureFinishTime = gpGlobals->curtime;
		m_flNextSprint = m_fGestureFinishTime;
		if (GetActiveWeapon())
		{
			GetActiveWeapon()->m_fDoNotDisturb = m_fGestureFinishTime;
		}
		m_fTimeToReady = gpGlobals->curtime + 10.0f; //dont want this think function running again
		return;
	}

	//release the grenade if they held it too long
	if (m_fNadeDetTime - gpGlobals->curtime <= 0)
	{
		Fraggrenade_Create(GetAbsOrigin() + Vector(0,0,15), vec3_angle, Vector(0, 0, 0), Vector(0, 0, 0), this, 0.0f, false, true, gpGlobals->curtime + 10.0f);

		SetAmmoCount(Clamp(GetAmmoCount(GetAmmoDef()->Index("Grenade")) - 1, 0, 1000), GetAmmoDef()->Index("Grenade"));

		m_fGestureFinishTime = gpGlobals->curtime;
		m_flNextSprint = m_fGestureFinishTime;
		if (GetActiveWeapon())
		{
			GetActiveWeapon()->m_fDoNotDisturb = m_fGestureFinishTime;
		}
		m_fTimeToReady = gpGlobals->curtime + 10.0f; //dont want this think function running again
		return; 
	}

	//release grenade if the player happens to be dead
	if (!IsAlive())
	{
		Fraggrenade_Create(GetAbsOrigin() + Vector(0, 0, 32), vec3_angle, Vector(0, 0, 0), Vector(0, 0, 0), this, Clamp(m_fNadeDetTime - gpGlobals->curtime, 0.0f, 100.0f), false, true, m_fNextBlipTime);
		m_fTimeToReady = gpGlobals->curtime + 10.0f; //dont want this think function running again
		return;
	}

	//check roll
	Vector vForward;
	EyeVectors(&vForward);
	bool bRoll = UTIL_VecToPitch(vForward) >= vance_frag_roll_angle.GetFloat();
	if (bRoll && !(GetFlags() & FL_ONGROUND))
		bRoll = false;
	if (bRoll)
	{
		trace_t tr;
		UTIL_TraceLine(Weapon_ShootPosition(), Weapon_ShootPosition() + vForward * 200.0f, MASK_SHOT_HULL, this, COLLISION_GROUP_NONE, &tr);
		if (!tr.DidHit())
		{
			bRoll = false;
		}
	}

	//if they want to throw set the sequence, otherwise keep reseting the weapons donotdisturb
	if (m_bWantsToThrow)
	{
		CVanceViewModel *pViewModel = static_cast<CVanceViewModel *>(GetViewModel());
		if (pViewModel)
		{
			int idealSequence;
			if (bRoll)
			{
				idealSequence = pViewModel->SelectWeightedSequence(ACT_VM_SECONDARYATTACK);
			}
			else {
				idealSequence = pViewModel->SelectWeightedSequence(ACT_VM_THROW);
			}
			if (idealSequence >= 0)
			{
				pViewModel->SendViewModelMatchingSequence(idealSequence);
				m_fGestureFinishTime = gpGlobals->curtime + pViewModel->SequenceDuration();
			}
		}

		CreateGrenade(bRoll);
		SetAmmoCount(Clamp(GetAmmoCount(GetAmmoDef()->Index("Grenade")) - 1, 0, 1000), GetAmmoDef()->Index("Grenade"));

		if (GetActiveWeapon())
		{
			GetActiveWeapon()->m_fDoNotDisturb = m_fGestureFinishTime;
		}
		m_fTimeToReady = m_fGestureFinishTime + 10.0f;
		m_flNextSprint = m_fGestureFinishTime;

		return;
	}
	else {
		m_fGestureFinishTime = gpGlobals->curtime + 0.1;
		m_flNextSprint = m_fGestureFinishTime;
		if (GetActiveWeapon())
		{
			GetActiveWeapon()->m_fDoNotDisturb = m_fGestureFinishTime;
		}

		if (bRoll != m_bRoll)
		{
			m_bRoll = bRoll;

			CVanceViewModel *pViewModel = static_cast<CVanceViewModel *>(GetViewModel());
			if (pViewModel)
			{
				int idealSequence = -1;
				if (m_bRoll)
				{
					idealSequence = pViewModel->SelectWeightedSequence(ACT_VM_PULLBACK_LOW);
				}
				else {
					idealSequence = pViewModel->SelectWeightedSequence(ACT_VM_PULLBACK_HIGH);
				}
				if (idealSequence >= 0)
				{
					pViewModel->SendViewModelMatchingSequence(idealSequence);
					m_fGestureFinishTime = gpGlobals->curtime + pViewModel->SequenceDuration();
					m_flNextSprint = m_fGestureFinishTime;
				}
			}
		}
	}

	//update blips
	if (!m_bWarnedAI && gpGlobals->curtime >= m_fWarnAITime)
	{
		CSoundEnt::InsertSound(SOUND_DANGER, GetAbsOrigin(), 400, 1.5, this);
		m_bWarnedAI = true;
	}

	if (m_fNextBlipTime <= gpGlobals->curtime && gpGlobals->curtime <= m_fNadeDetTime - 0.2f)
	{
		EmitSound("Grenade.Blip");

		if (m_bWarnedAI)
		{
			m_fNextBlipTime = gpGlobals->curtime + FRAG_GRENADE_BLIP_FAST_FREQUENCY;
		}
		else
		{
			m_fNextBlipTime = gpGlobals->curtime + FRAG_GRENADE_BLIP_FREQUENCY;
		}
	}
}

void CVancePlayer::CreateGrenade(bool rollMe)
{
	if (!rollMe)
	{
		Vector vecEye = EyePosition();
		Vector vForward, vRight;

		EyeVectors(&vForward, &vRight, NULL);
		Vector vecSrc = vecEye + vForward * 18.0f + vRight * 8.0f;
		trace_t tr;

		UTIL_TraceHull(vecEye, vecSrc, -Vector(4.0f + 2, 4.0f + 2, 4.0f + 2), Vector(4.0f + 2, 4.0f + 2, 4.0f + 2), PhysicsSolidMaskForEntity(), this, GetCollisionGroup(), &tr);

		if (tr.DidHit())
		{
			vecSrc = tr.endpos;
		}
		vForward[2] += 0.1f;

		Vector vecThrow;
		GetVelocity(&vecThrow, NULL);
		vecThrow += vForward * 1200;

		gamestats->Event_WeaponFired(this, true, GetClassname());

		Fraggrenade_Create(vecSrc, vec3_angle, vecThrow, AngularImpulse(600, random->RandomInt(-1200, 1200), 0), this, Clamp(m_fNadeDetTime - gpGlobals->curtime, 0.0f, 100.0f), false, true, m_fNextBlipTime);
	}
	else
	{
		// BUGBUG: Hardcoded grenade width of 4 - better not change the model :)
		Vector vecSrc;
		CollisionProp()->NormalizedToWorldSpace(Vector(0.5f, 0.5f, 0.0f), &vecSrc);
		vecSrc.z += GRENADE_RADIUS;

		Vector vecFacing = BodyDirection2D();
		// no up/down direction
		vecFacing.z = 0;
		VectorNormalize(vecFacing);
		trace_t tr;
		UTIL_TraceLine(vecSrc, vecSrc - Vector(0, 0, 16), MASK_PLAYERSOLID, this, COLLISION_GROUP_NONE, &tr);
		if (tr.fraction != 1.0)
		{
			// compute forward vec parallel to floor plane and roll grenade along that
			Vector tangent;
			CrossProduct(vecFacing, tr.plane.normal, tangent);
			CrossProduct(tr.plane.normal, tangent, vecFacing);
		}
		vecSrc += (vecFacing * 18.0);

		//CheckThrowPosition( CBasePlayer *pPlayer, const Vector &vecEye, Vector &vecSrc )
		UTIL_TraceHull(WorldSpaceCenter(), vecSrc, -Vector(GRENADE_RADIUS + 2, GRENADE_RADIUS + 2, GRENADE_RADIUS + 2), Vector(GRENADE_RADIUS + 2, GRENADE_RADIUS + 2, GRENADE_RADIUS + 2),
			PhysicsSolidMaskForEntity(), this, GetCollisionGroup(), &tr);

		if (tr.DidHit())
		{
			vecSrc = tr.endpos;
		}

		Vector vecThrow;
		GetVelocity(&vecThrow, NULL);
		vecThrow += vecFacing * 700;
		// put it on its side
		QAngle orientation(0, GetLocalAngles().y, -90);
		// roll it
		AngularImpulse rotSpeed(0, 0, 720);
		gamestats->Event_WeaponFired(this, true, GetClassname());

		Fraggrenade_Create(vecSrc, orientation, vecThrow, rotSpeed, this, Clamp(m_fNadeDetTime - gpGlobals->curtime, 0.0f, 100.0f), false, true, m_fNextBlipTime);
	}
}

bool CVancePlayer::GiveTourniquet( int count )
{
	if ( m_iNumTourniquets != sk_max_tourniquets.GetInt() )
	{
		m_iNumTourniquets = Clamp( m_iNumTourniquets + count, 0, sk_max_tourniquets.GetInt() );
		return true;
	}
	else
	{
		return false;
	}
}

bool CVancePlayer::GiveStim( int count )
{
	if ( m_iNumStims != sk_max_stims.GetInt() )
	{
		m_iNumStims = Clamp( m_iNumStims + count, 0, sk_max_stims.GetInt() );
		return true;
	}
	else
	{
		return false;
	}
}

// might be better to use PreThink for this
void CVancePlayer::PostThink()
{
	BaseClass::PostThink();

	if ( gpGlobals->eLoadType != MapLoad_Background
		&& IsSuitEquipped() )
	{
		// Place-holder HUD for stims, tourniquets and bleeding notification 
		debugoverlay->AddScreenTextOverlay(0.02f, 0.71f, 0.0f, 255, 255, 255, 255, CFmtStr( "Gernades; %i", GetAmmoCount(GetAmmoDef()->Index("Grenade"))));
		debugoverlay->AddScreenTextOverlay( 0.02f, 0.75f, 0.0f, 255, 255, 255, 255, CFmtStr( "Stims: %i", m_iNumStims ) );
		debugoverlay->AddScreenTextOverlay( 0.02f, 0.79f, 0.0f, 255, 255, 255, 255, CFmtStr( "Tourniquets: %i", m_iNumTourniquets ) );
		if ( m_bStimRegeneration )
		{
			debugoverlay->AddScreenTextOverlay( 0.02f, 0.83f, 0.0f, 0, 180, 0, 255, "Regenerating!" );
		}
		if ( m_bBleeding )
		{
			debugoverlay->AddScreenTextOverlay( 0.02f, 0.87f, 0.0f, 180, 0, 0, 255, "Bleeding!" );
		}
	}
	
	// Gestures
	if (m_PerformingGesture != GestureAction::None && gpGlobals->curtime >= m_fGestureFinishTime && IsAlive())
	{
		switch ( m_PerformingGesture )
		{
			case GestureAction::InjectingStim:
				if (GetActiveWeapon())
				{
					GetActiveWeapon()->m_fDoNotDisturb = 0.0f;
					GetActiveWeapon()->Deploy();
				}
				break;
			case GestureAction::EquippingTourniquet:
				if (GetActiveWeapon())
				{
					GetActiveWeapon()->m_fDoNotDisturb = 0.0f;
					GetActiveWeapon()->Deploy();
				}
				break;
			case GestureAction::ThrowingGrenade:
				if (GetActiveWeapon())
				{
					GetActiveWeapon()->m_fDoNotDisturb = 0.0f;
					GetActiveWeapon()->Deploy();
				}
				m_fNextThrowTime = gpGlobals->curtime + vance_frag_redraw_time.GetFloat();

				break;
		}

		m_PerformingGesture = GestureAction::None;
	}

	// Stims, regenerates health on usage
	if ( m_bStimRegeneration )
	{
		if ( gpGlobals->curtime >= m_fStimRegenerationEndTime )
		{
			m_bStimRegeneration = false;
		}
		else if ( gpGlobals->curtime >= m_fStimRegenerationNextHealTime )
		{
			Heal( sk_stim_health_per_interval.GetInt() );
			m_fStimRegenerationNextHealTime = gpGlobals->curtime + sk_stim_heal_interval.GetFloat();
		}
	}

	// Taking damage increases the chance of bleeding
	if ( gpGlobals->curtime <= m_fNextBleedChanceDecay &&
		 gpGlobals->curtime - m_fLastDamageTime >= sk_bleed_chance_decay_start_time.GetFloat() )
	{
		m_fBleedChance = Clamp( m_fBleedChance - sk_bleed_chance_increment.GetFloat(), 5.0f, 100.0f );
		m_fNextBleedChanceDecay = gpGlobals->curtime + sk_bleed_chance_decay_rate.GetFloat();
	}

	// Bleeding
	if ( m_bBleeding )
	{
		if ( gpGlobals->curtime >= m_fBleedEndTime )
		{
			m_bBleeding = false;
		}
		else if ( gpGlobals->curtime >= m_fNextBleedTime )
		{
			EmitSound( "HL2_Player.UseDeny" );
			Damage( sk_bleed_dmg_per_interval.GetFloat() );
			m_fNextBleedTime = gpGlobals->curtime + sk_bleed_dmg_interval.GetFloat();
		}
	}

	// Assign our speed to this variable
	float playerSpeed = GetLocalVelocity().Length2D();

	// If sprinting and shot(s) fired, slow us down as a penalty.
	if (!WpnCanSprint())
	{
		if (playerSpeed >= 300)
			StopSprinting();
		m_flNextSprint = gpGlobals->curtime + sk_nextsprint_delay.GetFloat();
	}

	CBaseVanceWeapon *pWeapon = dynamic_cast<CBaseVanceWeapon *>(GetActiveWeapon());
	CBaseViewModel *pWeaponViewModel = GetViewModel();

	// This should be in the base weapon code
	if ( pWeapon && pWeaponViewModel )
	{
		// Implement a curve when we know what we want
		float playbackSpeed = 1.0f;

		Activity activity = pWeapon->GetActivity();
		if ( activity == pWeapon->GetWalkActivity() || activity == ACT_VM_SPRINT2 || activity == ACT_VM_SPRINT2_EXTENDED)
		{
			pWeaponViewModel->SetPlaybackRate( playbackSpeed );
		}
		else if ( activity == pWeapon->GetSprintActivity() && m_PerformingGesture == GestureAction::None)
		{
			pWeaponViewModel->SetPlaybackRate( GetFlags() & FL_ONGROUND ? playbackSpeed : 0.1f );
		}
		else
		{
			pWeaponViewModel->SetPlaybackRate( 1.0f );
		}
	}

	CBaseViewModel *pLegsViewModel = GetViewModel( VM_LEGS );

	//the weapon will have refused to deploy when the actual vehicle tells it to since it tells it to deploy way too early so we have to manually deploy once we are actually clear of the vehicle
	//i should just edit the actual code for deploying the weapon but i cant be bothered to lol
	if (IsInAVehicle() != m_bWasInAVehicle) {
		if (!IsInAVehicle()){ //just existed vehicle
			if (pWeapon)
				pWeapon->Deploy();
		}
		m_bWasInAVehicle = IsInAVehicle();
	}

	//grenade
	if (m_afButtonPressed & IN_THROWGRENADE && !IsInAVehicle())
	{
		if (GetAmmoCount(GetAmmoDef()->Index("Grenade")) <= 0)
		{
			EmitSound("HL2Player.UseDeny");
		}
		else {
			m_bWantsToThrow = false;
			ThrowGrenade();
		}
	}
	if (m_afButtonReleased & IN_THROWGRENADE && GetAmmoCount(GetAmmoDef()->Index("Grenade")) != 0)
	{
		m_bWantsToThrow = true;
	}
	if (m_fTimeToReady <= gpGlobals->curtime && m_PerformingGesture == GestureAction::ThrowingGrenade){
		ThrowingGrenade();
	}

	if (m_afButtonPressed & IN_ATTACK3 && m_flNextKick < gpGlobals->curtime && m_ParkourAction.Get() != ParkourAction::Climb && !IsInAVehicle())
	{
		// Play a kick animation for the legs
		if ( pLegsViewModel )
		{
			int idealSequence = 0;
			if ( GetGroundEntity() == nullptr )
			{
				idealSequence = pLegsViewModel->SelectWeightedSequence( ACT_VM_SECONDARYATTACK );
				bDropKick = true;
			}
			else
			{
				idealSequence = pLegsViewModel->SelectWeightedSequence( ACT_VM_PRIMARYATTACK );
				bDropKick = false;
			}

			if ( idealSequence >= 0 )
			{
				pLegsViewModel->SendViewModelMatchingSequence( idealSequence );
				m_flNextKickAttack = gpGlobals->curtime + pLegsViewModel->SequenceDuration( idealSequence ) - 0.5f;
				m_flKickAnimLength = gpGlobals->curtime + pLegsViewModel->SequenceDuration( idealSequence ) - 0.1f;
				m_flNextKick = gpGlobals->curtime + vance_kick_firerate.GetFloat();

				StopSprinting();
				StartWalking();
				m_flNextSprint = gpGlobals->curtime + 1.0f;
				m_flNextWalk = gpGlobals->curtime + 1.0f;
			}
		}

		// Play a special animation for weapons when kicking
		if (pWeapon && CanSwitchViewmodelSequence())
		{
			pWeapon->SendWeaponAnim( ACT_VM_KICK );
		}

		ViewPunch( QAngle( random->RandomFloat( 1.0f, 2.0f ), 0, 0 ) );

		float kickDuration = 0.0f;
		if ( pLegsViewModel )
		{
			kickDuration = pLegsViewModel->SequenceDuration();
		}

		// When to apply damage
		m_flKickTime = kickDuration / 4 + gpGlobals->curtime + vance_kick_time_adjust.GetFloat();
		m_bIsKicking = true;
	}

	if ( m_bIsKicking && m_flKickTime < gpGlobals->curtime )
	{
		KickAttack();
		m_bIsKicking = false;
	}

	/*if ( pLegsViewModel && m_flKickAnimLength < gpGlobals->curtime )
	{
		int idealSequence = pLegsViewModel->SelectWeightedSequence( ACT_VM_IDLE );
		if ( idealSequence >= 0 )
		{
			pLegsViewModel->SendViewModelMatchingSequence( idealSequence );
		}
	}*/

	m_angEyeAngles = EyeAngles();

	QAngle angles = GetLocalAngles();
	angles[PITCH] = 0;
	SetLocalAngles(angles);

	m_pPlayerAnimState->Update();
}

void CVancePlayer::StartAdmireGlovesAnimation()
{
	MDLCACHE_CRITICAL_SECTION();

	CVanceViewModel *vm = (CVanceViewModel *)(GetViewModel());
	if (vm && !GetActiveWeapon())
	{
		vm->SetWeaponModel(GetArmsViewModel(), NULL);
		ShowViewModel(true);

		int	idealSequence = vm->LookupSequence("seq_admire");
		if (idealSequence >= 0)
		{
			vm->SendViewModelMatchingSequence(idealSequence);
			m_flAdmireGlovesAnimTime = gpGlobals->curtime + vm->SequenceDuration(idealSequence);
		}
	}
}

void CVancePlayer::Spawn()
{
	BaseClass::Spawn();

	Precache();

	SetModel( GetPlayerWorldModel() );

	m_UnarmedWeapon = static_cast<CBaseCombatWeapon *>(GiveNamedItem("weapon_unarmed"));

	CreateViewModel( VM_LEGS );

	if ( CBaseViewModel *pLegViewModel = GetViewModel( VM_LEGS ) )
	{
		pLegViewModel->SetWeaponModel( GetLegsViewModel(), NULL );
	}

	SetTouch( &CVancePlayer::Touch );
	SetThink( &CVancePlayer::Think );
	SetNextThink( gpGlobals->curtime + 0.05f );
}

//-----------------------------------------------------------------------------
// Purpose: Returns whether or not we are allowed to sprint now.
//-----------------------------------------------------------------------------
bool CVancePlayer::CanSprint()
{
	if (m_flNextSprint >= gpGlobals->curtime)
		return false;

	return (m_bSprintEnabled &&										// Only if sprint is enabled
		!(m_Local.m_bDucked && !m_Local.m_bDucking) &&				// Nor if we're ducking
		(GetWaterLevel() != 3) &&									// Certainly not underwater
		(GlobalEntity_GetState("suit_no_sprint") != GLOBAL_ON));	// Out of the question without the sprint module
}

//reutrns whether the current weapon actions will allow us to sprint
bool CVancePlayer::WpnCanSprint()
{
	if (m_PerformingGesture != GestureAction::None) //if we are doing a gesture its not our job to not sprint
		return true;
	CBaseCombatWeapon *pWeapon = dynamic_cast<CBaseCombatWeapon *>(GetActiveWeapon());
	if (!pWeapon) //we dont even have a gun (which should never happen but might as well check)
		return true;
	if (pWeapon->m_bUseCustomStopSprint)
	{
		if ((m_nButtons & IN_ATTACK) && pWeapon->m_bStopSprintPrimary) //player is primary attacking and the weapon cares
			return false;
		if ((m_nButtons & IN_ATTACK2) && pWeapon->m_bStopSprintSecondary) //player is secondary attacking and the weapon cares
			return false;
	} else {
		if (m_nButtons & (IN_ATTACK | IN_ATTACK2))
			return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CVancePlayer::StartSprinting()
{
	CPASAttenuationFilter filter(this);
	filter.UsePredictionRules();
	if (GetHealth() < 50)
	{
		EmitSound(filter, entindex(), "AlyxPlayer.SprintPain");
	}
	else
	{
		EmitSound(filter, entindex(), "AlyxPlayer.Sprint");
	}
	if (IsSuitEquipped() && m_HL2Local.m_flSuitPower < 10)
	{
		// Don't sprint unless there's a reasonable
		// amount of suit power.

		// debounce the button for sound playing
		if (m_afButtonPressed & IN_SPEED)
		{
			CPASAttenuationFilter filter(this);
			filter.UsePredictionRules();
			EmitSound(filter, entindex(), "HL2Player.SprintNoPower");
		}

		return;
	}

	SetMaxSpeed(VANCE_SPRINT_SPEED);

	m_fIsSprinting = true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CVancePlayer::StopSprinting()
{
	SetMaxSpeed(VANCE_WALK_SPEED);

	m_fIsSprinting = false;

	StopSound("AlyxPlayer.SprintPain");
	StopSound("AlyxPlayer.Sprint");

	if (sv_stickysprint.GetBool())
	{
		m_bIsAutoSprinting = false;
		m_fAutoSprintMinTime = 0.0f;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CVancePlayer::StartWalking()
{
	m_fIsWalking = true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CVancePlayer::StopWalking()
{
	if (!IsSprinting())
	{
		if (!PhysCannonGetHeldEntity(GetActiveWeapon()))
		{
			SetMaxSpeed(VANCE_WALK_SPEED);
		}

		m_fIsWalking = false;
	}
}

extern CSuitPowerDevice SuitDeviceFlashlight;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CVancePlayer::SuitPower_Update()
{
	if (SuitPower_ShouldRecharge())
	{
		SuitPower_Charge(SUITPOWER_CHARGE_RATE * gpGlobals->frametime);
	}
	else if (m_HL2Local.m_bitsActiveDevices)
	{
		float flPowerLoad = m_flSuitPowerLoad;

		if (SuitPower_IsDeviceActive(SuitDeviceFlashlight))
		{
			float factor;

			factor = 1.0f / m_flFlashlightPowerDrainScale;

			flPowerLoad -= (SuitDeviceFlashlight.GetDeviceDrainRate() * (1.0f - factor));
		}

		if (!SuitPower_Drain(flPowerLoad * gpGlobals->frametime))
		{
			// TURN OFF ALL DEVICES!!
			if (IsSprinting())
			{
				StopSprinting();
			}
		}
	}
}

bool CVancePlayer::ApplyBattery( float powerMultiplier )
{
	const float MAX_NORMAL_BATTERY = 100;
	if ( ( ArmorValue() < MAX_NORMAL_BATTERY ) && IsSuitEquipped() )
	{
		int pct;
		char szcharge[64];

		IncrementArmorValue( sk_battery.GetFloat() * powerMultiplier, MAX_NORMAL_BATTERY );

		CPASAttenuationFilter filter( this, "ItemBattery.Touch" );
		EmitSound( filter, entindex(), "ItemBattery.Touch" );

		CSingleUserRecipientFilter user( this );
		user.MakeReliable();

		UserMessageBegin( user, "ItemPickup" );
		WRITE_STRING( "item_battery" );
		MessageEnd();

		// Suit reports new power level
		// For some reason this wasn't working in release build -- round it.
		pct = (int)( (float)( ArmorValue() * 100.0 ) * ( 1.0 / MAX_NORMAL_BATTERY ) + 0.5 );
		pct = ( pct / 5 );
		if ( pct > 0 )
			pct--;

		V_snprintf( szcharge, sizeof( szcharge ), "!HEV_%1dP", pct );

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CVancePlayer::FlashlightTurnOn()
{
	if ( m_bFlashlightDisabled )
		return;

	EmitSound( "HL2Player.FlashLightOn" );
	AddEffects( EF_DIMLIGHT );

	variant_t flashlighton;
	flashlighton.SetFloat( m_HL2Local.m_flSuitPower / 100.0f );
	FirePlayerProxyOutput( "OnFlashlightOn", flashlighton, this, this );
}

void CVancePlayer::Hit(trace_t& tr, Activity nHitActivity)
{
	// Make sound for the AI
	CSoundEnt::InsertSound( SOUND_BULLET_IMPACT, tr.endpos, 400, 0.2f, this );

	float flDamageMult = RemapValClamped(GetLocalVelocity().Length2D(), 0, hl2_sprintspeed.GetFloat(), vance_kick_damage_mult_min.GetFloat(), vance_kick_damage_mult_max.GetFloat());
	float flForceMult = RemapValClamped(GetLocalVelocity().Length2D(), 0, hl2_sprintspeed.GetFloat(), vance_kick_force_mult_min.GetFloat(), vance_kick_force_mult_max.GetFloat());

	// Apply damage to a hit target
	if ( tr.m_pEnt )
	{
		Vector vecForward;
		EyeVectors( &vecForward );

		CTakeDamageInfo info( this, this, 20 * vance_kick_powerscale.GetFloat() * flDamageMult, DMG_KICK );

		if ( tr.m_pEnt->IsNPC() )
		{
			// If bonking an NPC, adjust damage.
			info.AdjustPlayerDamageInflictedForSkillLevel();
		}

		CalculateMeleeDamageForce( &info, vecForward, tr.endpos );
		info.SetDamageForce( info.GetDamageForce() * vance_kick_meleedamageforce.GetFloat() * flForceMult );

		tr.m_pEnt->DispatchTraceAttack( info, vecForward, &tr );
		ApplyMultiDamage();

		// Now hit all triggers along the ray that... 
		TraceAttackToTriggers( info, tr.startpos, tr.endpos, vecForward );
	}
}

void CVancePlayer::KickAttack()
{

	float flDamageMult = RemapValClamped(GetLocalVelocity().Length2D(), 0, hl2_sprintspeed.GetFloat(), vance_kick_damage_mult_min.GetFloat(), vance_kick_damage_mult_max.GetFloat());
	float flForceMult = RemapValClamped(GetLocalVelocity().Length2D(), 0, hl2_sprintspeed.GetFloat(), vance_kick_force_mult_min.GetFloat(), vance_kick_force_mult_max.GetFloat());

	Vector vecForward, vecRight, vecUp;
	EyeVectors(&vecForward, &vecRight, &vecUp);

	Vector swingStart = Weapon_ShootPosition();
	Vector swingEnd = swingStart + vecForward * vance_kick_range.GetFloat();

	trace_t tr;
	UTIL_TraceLine( swingStart, swingEnd, MASK_SHOT_HULL, this, COLLISION_GROUP_NONE, &tr );

	//if the first trace for the crosshair didnt connect, do a second trace for the general area of the actual foot, its low on the screen so people (me) tend to aim high when kicking to have the foot visually connect
	if (!tr.DidHitNonWorldEntity())
	{
		Vector swingStart2 = Weapon_ShootPosition() + vecUp * -10 + vecRight * 7;
		Vector swingEnd2 = swingStart + vecForward * vance_kick_range.GetFloat() + vecUp * -30 + vecRight * 10;

		trace_t tr2;
		UTIL_TraceLine(swingStart2, swingEnd2, MASK_SHOT_HULL, this, COLLISION_GROUP_NONE, &tr2);

		if (tr2.DidHitNonWorldEntity())
		{
			tr = tr2; //overwrite the first line trace with the cooler one
		}
	}

	// Like bullets, bludgeon traces have to trace against triggers.
	CTakeDamageInfo triggerInfo(this, this, 20 * vance_kick_powerscale.GetFloat() * flDamageMult, DMG_KICK);
	triggerInfo.SetDamagePosition( tr.startpos );
	triggerInfo.SetDamageForce(triggerInfo.GetDamageForce() * vance_kick_meleedamageforce.GetFloat() * flForceMult);
	TraceAttackToTriggers( triggerInfo, tr.startpos, tr.endpos, vecForward );

	const float bludgeonHullDim = 16;
	const Vector bludgeonMins( -bludgeonHullDim, -bludgeonHullDim, -bludgeonHullDim );
	const Vector bludgeonMaxs( bludgeonHullDim, bludgeonHullDim, bludgeonHullDim );

	if ( tr.fraction == 1.0 )
	{
		float bludgeonHullRadius = 1.732f * bludgeonHullDim; // Hull is +/- 16, so use cuberoot of 2 to determine how big the hull is from center to the corner point
		swingEnd -= vecForward * bludgeonHullRadius; // Back off by hull "radius"

		UTIL_TraceHull( swingStart, swingEnd, bludgeonMins, bludgeonMaxs, MASK_SHOT_HULL, this, COLLISION_GROUP_NONE, &tr );
		if ( tr.fraction < 1.0 && tr.m_pEnt )
		{
			Vector vecToTarget = tr.m_pEnt->GetAbsOrigin() - swingStart;
			VectorNormalize(vecToTarget);

			float dot = DotProduct( vecToTarget, vecForward );

			// YWB:  Make sure they are sort of facing the guy at least...
			if ( dot < 0.70721f )
			{
				// Force amiss
				tr.fraction = 1.0f;
			}
		}
	}

	if (tr.DidHit())
	{
		EmitSound("AlyxPlayer.KickHit");
		UTIL_ScreenShake( GetAbsOrigin(), 4.0f, 10.0f, 0.25f, 1000, SHAKE_START, false );

		//kick off walls
		if (!(GetFlags() & FL_ONGROUND) && bDropKick)
		{
			if (tr.DidHitWorld())
			{
				SetAbsVelocity((GetAbsVelocity() 
					+ vecForward * -100 * vance_kick_bounce_scale.GetFloat() //boost backwards away from look direction
					+ tr.plane.normal * 150 * vance_kick_bounce_scale.GetFloat() //boost in direction of impact normal
					- GetAbsVelocity() * tr.plane.normal //nullify existing velocity that is aligned to impact normal
					)* Vector(1, 1, 1 - abs(tr.plane.normal.z)) //remove z velocity as impact normal z aproaches horizontal
					+ GetAbsVelocity() * Vector(0, 0, abs(tr.plane.normal.z)) //add back original z velocity to replace the modified z velocity we just removed
					+ Vector(0, 0, 30) * vance_kick_bounce_scale.GetFloat()); //boost upwards
			}
			else {
				Vector newVelocity = GetAbsVelocity() + vecForward * -120 * vance_kick_bounce_scale.GetFloat() + tr.plane.normal * 50 * vance_kick_bounce_scale.GetFloat();
				SetAbsVelocity(Vector(newVelocity.x,newVelocity.y,GetAbsVelocity().z));
			}
			bDropKick = false;
		}
		else if (tr.DidHitWorld())
		{
			SetAbsVelocity(GetAbsVelocity() + vecForward * -75 * vance_kick_knockback_scale.GetFloat());
		}
	}
	else //this can probably be removed since the normal cloth foley sounds are handled in the model as an anim event
	{
		EmitSound("AlyxPlayer.KickMiss");
	}

	ViewPunch( QAngle( random->RandomFloat( -1.0f, -2.0f ), 0, random->RandomFloat( -2.0f, 2.0f ) ) );

	if (tr.fraction != 1.0f)
	{
		Hit( tr, ACT_VM_PRIMARYATTACK );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : void CBasePlayer::PlayerUse
//-----------------------------------------------------------------------------
void CVancePlayer::PlayerUse()
{
	// Was use pressed or released?
	if (!((m_nButtons | m_afButtonPressed | m_afButtonReleased) & IN_USE))
		return;

	if (m_afButtonPressed & IN_USE)
	{
		// Currently using a latched entity?
		if (ClearUseEntity())
		{
			return;
		}
		else
		{
			if (m_afPhysicsFlags & PFLAG_DIROVERRIDE)
			{
				m_afPhysicsFlags &= ~PFLAG_DIROVERRIDE;
				m_iTrain = TRAIN_NEW | TRAIN_OFF;
				return;
			}
			else
			{	// Start controlling the train!
				CBaseEntity* pTrain = GetGroundEntity();
				if (pTrain && !(m_nButtons & IN_JUMP) && (GetFlags() & FL_ONGROUND) && (pTrain->ObjectCaps() & FCAP_DIRECTIONAL_USE) && pTrain->OnControls(this))
				{
					m_afPhysicsFlags |= PFLAG_DIROVERRIDE;
					m_iTrain = TrainSpeed(pTrain->m_flSpeed, ((CFuncTrackTrain*)pTrain)->GetMaxSpeed());
					m_iTrain |= TRAIN_NEW;
					EmitSound("HL2Player.TrainUse");
					return;
				}
			}
		}

		// Tracker 3926:  We can't +USE something if we're climbing a ladder
		if (GetMoveType() == MOVETYPE_LADDER)
		{
			return;
		}
	}

	if (m_flTimeUseSuspended > gpGlobals->curtime)
	{
		// Something has temporarily stopped us being able to USE things.
		// Obviously, this should be used very carefully.(sjb)
		return;
	}

	CBaseEntity* pUseEntity = FindUseEntity();

	bool usedSomething = false;

	// Found an object
	if (pUseEntity)
	{
		//!!!UNDONE: traceline here to prevent +USEing buttons through walls			
		int caps = pUseEntity->ObjectCaps();
		variant_t emptyVariant;

		if (IsSuitEquipped() && (m_afButtonPressed & IN_USE))
		{
			// Robin: Don't play sounds for NPCs, because NPCs will allow respond with speech.
			if (!pUseEntity->MyNPCPointer())
			{
				EmitSound("HL2Player.Use");
			}
		}

		if (((m_nButtons & IN_USE) && (caps & FCAP_CONTINUOUS_USE)) ||
			((m_afButtonPressed & IN_USE) && (caps & (FCAP_IMPULSE_USE | FCAP_ONOFF_USE))))
		{
			if (caps & FCAP_CONTINUOUS_USE)
				m_afPhysicsFlags |= PFLAG_USING;

			pUseEntity->AcceptInput("Use", this, this, emptyVariant, USE_TOGGLE);

			usedSomething = true;
		}
		// UNDONE: Send different USE codes for ON/OFF.  Cache last ONOFF_USE object to send 'off' if you turn away
		else if ((m_afButtonReleased & IN_USE) && (pUseEntity->ObjectCaps() & FCAP_ONOFF_USE))	// BUGBUG This is an "off" use
		{
			pUseEntity->AcceptInput("Use", this, this, emptyVariant, USE_TOGGLE);

			usedSomething = true;
		}
	}
	else if (m_afButtonPressed & IN_USE)
	{
		// Signal that we want to play the deny sound, unless the user is +USEing on a ladder!
		// The sound is emitted in ItemPostFrame, since that occurs after GameMovement::ProcessMove which
		// lets the ladder code unset this flag.
		m_bPlayUseDenySound = true;
	}

	// Debounce the use key
	if (usedSomething && pUseEntity)
	{
		m_Local.m_nOldButtons |= IN_USE;
		m_afButtonPressed &= ~IN_USE;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVancePlayer::UpdateClientData()
{
	if (m_DmgTake || m_DmgSave || m_bitsHUDDamage != m_bitsDamageType)
	{
		// Comes from inside me if not set
		Vector damageOrigin = GetLocalOrigin();
		// send "damage" message
		// causes screen to flash, and pain compass to show direction of damage
		damageOrigin = m_DmgOrigin;

		// only send down damage type that have hud art
		int iShowHudDamage = g_pGameRules->Damage_GetShowOnHud();
		int visibleDamageBits = m_bitsDamageType & iShowHudDamage;

		m_DmgTake = clamp(m_DmgTake, 0, 255);
		m_DmgSave = clamp(m_DmgSave, 0, 255);

		// If we're poisoned, but it wasn't this frame, don't send the indicator
		// Without this check, any damage that occured to the player while they were
		// recovering from a poison bite would register as poisonous as well and flash
		// the whole screen! -- jdw
		if (visibleDamageBits & DMG_POISON)
		{
			float flLastPoisonedDelta = gpGlobals->curtime - m_tbdPrev;
			if (flLastPoisonedDelta > 0.1f)
			{
				visibleDamageBits &= ~DMG_POISON;
			}
		}

		CSingleUserRecipientFilter user(this);
		user.MakeReliable();
		UserMessageBegin(user, "Damage");
		WRITE_BYTE(m_DmgSave);
		WRITE_BYTE(m_DmgTake);
		WRITE_LONG(visibleDamageBits);
		WRITE_FLOAT(damageOrigin.x);	//BUG: Should be fixed point (to hud) not floats
		WRITE_FLOAT(damageOrigin.y);	//BUG: However, the HUD does _not_ implement bitfield messages (yet)
		WRITE_FLOAT(damageOrigin.z);	//BUG: We use WRITE_VEC3COORD for everything else
		MessageEnd();

		m_DmgTake = 0;
		m_DmgSave = 0;
		m_bitsHUDDamage = m_bitsDamageType;

		// Clear off non-time-based damage indicators
		int iTimeBasedDamage = g_pGameRules->Damage_GetTimeBased();
		m_bitsDamageType &= iTimeBasedDamage;
	}

	// Update Flashlight
	if (FlashlightIsOn() && !sv_infinite_aux_power.GetBool())
	{
		m_HL2Local.m_flFlashBattery -= FLASH_DRAIN_TIME * gpGlobals->frametime;
		if (m_HL2Local.m_flFlashBattery < 0.0f)
		{
			FlashlightTurnOff();
			m_HL2Local.m_flFlashBattery = 0.0f;
		}
	}
	else
	{
		m_HL2Local.m_flFlashBattery += FLASH_CHARGE_TIME * gpGlobals->frametime;
		if ( IsSuitEquipped() && m_HL2Local.m_flFlashBattery > 100.0f )
		{
			m_HL2Local.m_flFlashBattery = 100.0f;
		}
	}

	BaseClass::UpdateClientData();
}

void CVancePlayer::ItemPostFrame()
{
	BaseClass::ItemPostFrame();

	if (m_bPlayUseDenySound)
	{
		m_bPlayUseDenySound = false;

		if (IsSuitEquipped())
		{
			EmitSound("HL2Player.UseDeny");
		}
	}
}

void CVancePlayer::UpdateStepSound( surfacedata_t *psurface, const Vector &vecOrigin, const Vector &vecVelocity )
{
	if( m_ParkourAction.Get() == ParkourAction::Slide )
	{
		// TODO: slide sounds go here
		return;
	}

	BaseClass::UpdateStepSound( psurface, vecOrigin, vecVelocity );
}

// Set the activity based on an event or current state
void CVancePlayer::SetAnimation(PLAYER_ANIM playerAnim)
{
	float speed = GetAbsVelocity().Length2D();

	if (GetFlags() & (FL_FROZEN | FL_ATCONTROLS))
	{
		speed = 0;
		playerAnim = PLAYER_IDLE;
	}

	Activity idealActivity = ACT_HL2MP_RUN;

	if (playerAnim == PLAYER_JUMP)
	{
		if (HasWeapons())
			idealActivity = ACT_HL2MP_JUMP;
		else
			idealActivity = ACT_JUMP;
	}
	else if (playerAnim == PLAYER_DIE)
	{
		if (m_lifeState == LIFE_ALIVE)
		{
			return;
		}
	}
	else if (playerAnim == PLAYER_ATTACK1)
	{
		if (GetActivity() == ACT_HOVER ||
			GetActivity() == ACT_SWIM ||
			GetActivity() == ACT_HOP ||
			GetActivity() == ACT_LEAP ||
			GetActivity() == ACT_DIESIMPLE)
		{
			idealActivity = GetActivity();
		}
		else
		{
			idealActivity = ACT_HL2MP_GESTURE_RANGE_ATTACK;
		}
	}
	else if (playerAnim == PLAYER_RELOAD)
	{
		idealActivity = ACT_HL2MP_GESTURE_RELOAD;
	}
	else if (playerAnim == PLAYER_IDLE || playerAnim == PLAYER_WALK)
	{
		if (!(GetFlags() & FL_ONGROUND) && (GetActivity() == ACT_HL2MP_JUMP || GetActivity() == ACT_JUMP))    // Still jumping
		{
			idealActivity = GetActivity();
		}
		else if (GetWaterLevel() > 1)
		{
			if (speed == 0)
			{
				if (HasWeapons())
					idealActivity = ACT_HL2MP_IDLE;
				else
					idealActivity = ACT_IDLE;
			}
			else
			{
				if (HasWeapons())
					idealActivity = ACT_HL2MP_RUN;
				else
					idealActivity = ACT_RUN;
			}
		}
		else
		{
			if (GetFlags() & FL_DUCKING)
			{
				if (speed > 0)
				{
					if (HasWeapons())
						idealActivity = ACT_HL2MP_WALK_CROUCH;
					else
						idealActivity = ACT_WALK_CROUCH;
				}
				else
				{
					if (HasWeapons())
						idealActivity = ACT_HL2MP_IDLE_CROUCH;
					else
						idealActivity = ACT_COVER_LOW;
				}
			}
			else
			{
				if (speed > 0)
				{
					if (HasWeapons())
						idealActivity = ACT_HL2MP_RUN;
					else
						idealActivity = ACT_WALK;
				}
				else
				{
					if (HasWeapons())
						idealActivity = ACT_HL2MP_IDLE;
					else
						idealActivity = ACT_IDLE;
				}
			}
		}

		//idealActivity = TranslateTeamActivity( idealActivity );
	}

	if (IsInAVehicle())
	{
		idealActivity = ACT_COVER_LOW;
	}

	int animDesired;
	if (idealActivity == ACT_HL2MP_GESTURE_RANGE_ATTACK)
	{
		RestartGesture(Weapon_TranslateActivity(idealActivity));

		// FIXME: this seems a bit wacked
		Weapon_SetActivity(Weapon_TranslateActivity(ACT_RANGE_ATTACK1), 0);

		return;
	}
	else if (idealActivity == ACT_HL2MP_GESTURE_RELOAD)
	{
		RestartGesture(Weapon_TranslateActivity(idealActivity));
		return;
	}
	else
	{
		SetActivity(idealActivity);

		animDesired = SelectWeightedSequence(Weapon_TranslateActivity(idealActivity));

		if (animDesired == -1)
		{
			animDesired = SelectWeightedSequence(idealActivity);

			if (animDesired == -1)
			{
				animDesired = 0;
			}
		}

		// Already using the desired animation?
		if (GetSequence() == animDesired)
			return;

		m_flPlaybackRate = 1.0;
		ResetSequence(animDesired);
		SetCycle(0);
		return;
	}

	// Already using the desired animation?
	if (GetSequence() == animDesired)
		return;

	//Msg( "Set animation to %d\n", animDesired );
	// Reset to first frame of desired animation
	ResetSequence(animDesired);
	SetCycle(0);
}

void CVancePlayer::SlideTick()
{
	//trace down
	//trace_t tr;
	//UTIL_TraceLine(GetAbsOrigin() + Vector(0, 0, 10), GetAbsOrigin() - Vector(0, 0, 64), MASK_SHOT_HULL, this, COLLISION_GROUP_NONE, &tr);

	//should we keep sliding
	bool ContinueSlide = true;

	if (gpGlobals->curtime >= m_flSlideEndTime) //out of time
		ContinueSlide = false;
	else if (GetLocalVelocity().Length2D() < 150) //too slow
		ContinueSlide = false;
	else if (!(GetFlags() & FL_ONGROUND)) //nothing under us
		ContinueSlide = false;

	if (ContinueSlide)
	{
		m_flSlideFrictionScale = vance_slide_frictionscale.GetFloat() + (5 * pow(1.0f - ((m_flSlideEndTime - gpGlobals->curtime) / vance_slide_time.GetFloat()), 2));
		m_fNextSlideTime = gpGlobals->curtime + vance_slide_cooldown.GetFloat();
	}
	else
	{
		m_flSlideFrictionScale = 1.0f;
		m_flNextSprint = gpGlobals->curtime + 0.11f;
		m_ParkourAction = ParkourAction::None;
		StopSound("AlyxPlayer.Slide_default_start");
		EmitSound("AlyxPlayer.Slide_default_end");
		UnforceButtons(IN_DUCK);
	}
}

void CVancePlayer::TrySlide()
{
	// i dont think sliding in vehicles even does anything but we shouldnt do it
	if (IsInAVehicle())
		return;

	if (m_ParkourAction.Get() != ParkourAction::None) {
		m_bSlideWaitingForCrouchDebounce = true;
		return;
	}

	if (m_bSlideWaitingForCrouchDebounce)
		return;

	// dont slide in air
	if (!(GetFlags() & FL_ONGROUND))
	{
		if (m_bAllowMidairSlide){
			m_fMidairSlideWindowTime = gpGlobals->curtime + vance_slide_midair_window.GetFloat();
			m_bAllowMidairSlide = false;
		}
		return;
	}

	//gotta be going quick enough
	if (GetLocalVelocity().Length2D() < 310)
		return;

	if (m_fSprintTime < vance_slide_sprint_time.GetFloat())
		return;

	// respect the cooldown
	if (m_fNextSlideTime > gpGlobals->curtime)
		return;

	//we are sliding now

	CBaseVanceWeapon *pWeapon = dynamic_cast<CBaseVanceWeapon *>(GetActiveWeapon());
	if (pWeapon && CanSwitchViewmodelSequence()) {
		int idealSequence = pWeapon->SelectWeightedSequence(ACT_VM_SLIDE);
		if (idealSequence >= 0)
		{
			pWeapon->SendWeaponAnim(ACT_VM_SLIDE);
			SetNextAttack(gpGlobals->curtime + pWeapon->SequenceDuration());
			pWeapon->m_flNextPrimaryAttack = gpGlobals->curtime + pWeapon->SequenceDuration();
			pWeapon->m_flNextSecondaryAttack = gpGlobals->curtime + pWeapon->SequenceDuration();
		}
	}
	EmitSound("AlyxPlayer.Slide_default_start");
	m_vecSlideDirection = EyeDirection2D();
	SetAbsVelocity(GetAbsVelocity() + m_vecSlideDirection * vance_slide_addvelocity.GetFloat());
	m_flSlideEndTime = gpGlobals->curtime + vance_slide_time.GetFloat();
	m_bSlideWaitingForCrouchDebounce = true;
	m_ParkourAction = ParkourAction::Slide;
	ForceButtons(IN_DUCK);
}

void CVancePlayer::LedgeClimbTick()
{
	if (m_flClimbFraction >= 1.0f)
	{
		if (m_bVault) {
			m_vecClimbOutVelocity = m_vecClimbDesiredOrigin - m_vecClimbStartOrigin;
			m_vecClimbOutVelocity.z = 0.0f;
			m_vecClimbOutVelocity = m_vecClimbOutVelocity.Normalized() * 320.0f;
		} else {
			Interpolator_CurveInterpolate(INTERPOLATE_CATMULL_ROM,
				m_vecClimbStartOrigin - Vector(0, 0, VEC_HULL_MAX.z) * 2, // pre
				m_vecClimbStartOrigin, m_vecClimbDesiredOrigin, // start end
				m_vecClimbStartOrigin - Vector(0, 0, VEC_HULL_MAX.z) * 2, // next
				0.9f,
				m_vecClimbOutVelocity);

			m_vecClimbOutVelocity = (m_vecClimbOutVelocity - m_vecClimbDesiredOrigin) * -10;
		}
		m_bVaulting = false;

		m_ParkourAction = ParkourAction::None;
		m_vecClimbStartOrigin = vec3_origin;
		m_vecClimbCurrentOrigin = vec3_origin;
		m_vecClimbDesiredOrigin = vec3_origin;

		SetSolid(SOLID_BBOX);
		SetMoveType(MOVETYPE_WALK);
		SetAbsVelocity(m_vecClimbOutVelocity);
	}
	else
	{
		if (m_bVault) {
			m_flClimbFraction += 0.05f; //we need to calculate a velocity here that would allow us to move laterally at running speed

			float fEasedClimbFraction = m_flClimbFraction;
			if (m_flClimbFraction < 0.5f){
				fEasedClimbFraction = 4 * pow(m_flClimbFraction, 3);
			}

			Interpolator_CurveInterpolate(INTERPOLATE_CATMULL_ROM,
				m_vecClimbStartOriginHull, // pre
				m_vecClimbStartOrigin, m_vecClimbDesiredOrigin, // start end
				m_vecClimbStartOriginHull, // next
				fEasedClimbFraction,
				m_vecClimbCurrentOrigin);

			float fEasedClimbCurrentOriginZ = m_vecClimbCurrentOrigin.z;

			Interpolator_CurveInterpolate(INTERPOLATE_CATMULL_ROM,
				m_vecClimbStartOriginHull, // pre
				m_vecClimbStartOrigin, m_vecClimbDesiredOrigin, // start end
				m_vecClimbStartOriginHull, // next
				m_flClimbFraction,
				m_vecClimbCurrentOrigin);

			m_vecClimbCurrentOrigin = Vector(m_vecClimbCurrentOrigin.x, m_vecClimbCurrentOrigin.y, fEasedClimbCurrentOriginZ);

			m_vecVaultCameraAdjustment = -0.8f * (m_vecClimbCurrentOrigin - m_vecClimbStartOrigin);
		} else {
			if (m_bBigClimb){
				m_flClimbFraction += CLIMB_LERPSPEED_SLOW;
			} else {
				m_flClimbFraction += CLIMB_LERPSPEED;
			}

			float fEasedClimbFraction = m_flClimbFraction;
			if (m_flClimbFraction < 0.5f){
				fEasedClimbFraction = 4 * pow(m_flClimbFraction, 3);
			}

			Interpolator_CurveInterpolate(INTERPOLATE_CATMULL_ROM,
				m_vecClimbStartOriginHull, // pre
				m_vecClimbStartOrigin, m_vecClimbDesiredOrigin, // start end
				m_vecClimbStartOriginHull, // next
				fEasedClimbFraction,
				m_vecClimbCurrentOrigin);
			
			if (!m_bBigClimb) {
				m_vecVaultCameraAdjustment = -0.2f * (m_vecClimbCurrentOrigin - m_vecClimbStartOrigin);
			}
		}

		if (vance_climb_debug.GetBool())
		{
			debugoverlay->AddBoxOverlay(m_vecClimbStartOrigin, VEC_HULL_MIN, VEC_HULL_MAX, vec3_angle, 255, 0, 0, 1, 10);
			debugoverlay->AddBoxOverlay(m_vecClimbDesiredOrigin, VEC_HULL_MIN, VEC_HULL_MAX, vec3_angle, 0, 255, 0, 1, 10);
		}
		//m_vecClimbCurrentOrigin = Lerp<Vector>(m_flClimbFraction, m_vecClimbStartOrigin, m_vecClimbDesiredOrigin);
		SetAbsOrigin(m_vecClimbCurrentOrigin);
		SetAbsVelocity(vec3_origin);
	}
}

void CVancePlayer::TryLedgeClimb()
{
	//dont even think about it if we are in a vehicle
	if (IsInAVehicle())
		return;

	auto checkClimbability = [this](Vector vecStart) -> bool {
		Vector vecForward;
		EyeVectors(&vecForward);

		trace_t tr;
		UTIL_TraceHull( vecStart + Vector( 0, 0, vance_climb_checkray_height.GetFloat() ),
				vecStart - Vector( 0, 0, vance_climb_checkray_height.GetFloat() ) * 2,
				VEC_HULL_MIN, VEC_HULL_MAX, (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_GRATE), this, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);

		UTIL_TraceHull(tr.endpos, tr.endpos, VEC_HULL_MIN, VEC_HULL_MAX, (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_GRATE), this, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);

		if (tr.DidHit())
			return true;

		if (vance_climb_debug.GetBool())
		{
			debugoverlay->AddBoxOverlay( vecStart + Vector( 0, 0, vance_climb_checkray_height.GetFloat() ), VEC_HULL_MIN, VEC_HULL_MAX, vec3_angle, 0, 255, 255, 100, 10 );
			debugoverlay->AddBoxOverlay( vecStart - Vector( 0, 0, vance_climb_checkray_height.GetFloat() ) * 2, VEC_HULL_MIN, VEC_HULL_MAX, vec3_angle, 255, 0, 255, 100, 10 );

			debugoverlay->AddLineOverlay(vecStart + Vector(0, 0, vance_climb_checkray_height.GetFloat()), vecStart - Vector(0, 0, vance_climb_checkray_height.GetFloat()) * 2, 0, 255, 255, true, 10);
		}

		m_vecClimbDesiredOrigin = tr.endpos + Vector(0, 0, 5.0f);
		m_vecClimbStartOrigin = GetAbsOrigin();
		m_vecClimbStartOriginHull = m_vecClimbStartOrigin - Vector(0, 0, VEC_HULL_MAX.z) * 2;
		m_flClimbFraction = 0.0f;
		m_vecVaultCameraAdjustment = Vector(0, 0, 0);
		m_ParkourAction = ParkourAction::Climb;

		//break through glass ceiling
		UTIL_TraceLine(m_vecClimbStartOrigin * Vector(1,1,0) + Vector(0, 0, m_vecClimbDesiredOrigin.z + 30), m_vecClimbDesiredOrigin + Vector(0, 0, 30) + ((m_vecClimbDesiredOrigin - m_vecClimbStartOrigin) * Vector(1,1,0)).Normalized() * 50.0, MASK_PLAYERSOLID, this, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);
		tr.endpos = m_vecClimbDesiredOrigin + Vector(0, 0, 30) + ((m_vecClimbDesiredOrigin - m_vecClimbStartOrigin) * Vector(1, 1, 0)).Normalized() * 50.0;
		if (tr.DidHit()) {
			CTakeDamageInfo triggerInfo(this, this, 40.0f, DMG_KICK);
			triggerInfo.SetDamagePosition(tr.startpos);
			triggerInfo.SetDamageForce(triggerInfo.GetDamageForce() * 5.0f);
			TraceAttackToTriggers(triggerInfo, tr.startpos, tr.endpos, vecForward);

			Hit(tr, ACT_VM_PRIMARYATTACK);
		}

		// Don't get stuck during this traversal since we'll just be slamming the player origin
		SetMoveType(MOVETYPE_NOCLIP);
		SetMoveCollide(MOVECOLLIDE_DEFAULT);
		SetSolid(SOLID_NONE);

		CBaseVanceWeapon *pWeapon = dynamic_cast<CBaseVanceWeapon *>(GetActiveWeapon());
		if (pWeapon) {
			pWeapon->AbortReload();
			if (abs(m_vecClimbDesiredOrigin.z - m_vecClimbStartOrigin.z) > 56.0f) {
				Activity act = pWeapon->GetSequenceActivity(pWeapon->GetSequence());
				if (!(act == ACT_VM_HOLSTER || act == ACT_VM_HOLSTER_EXTENDED))
					pWeapon->SendWeaponAnim(ACT_VM_CLIMB_HIGH);
				m_bBigClimb = true;
				m_bVault = false;
				m_bVaulting = false;
			} else {
				Activity act = pWeapon->GetSequenceActivity(pWeapon->GetSequence());
				if (!(act == ACT_VM_HOLSTER || act == ACT_VM_HOLSTER_EXTENDED))
					pWeapon->SendWeaponAnim(ACT_VM_CLIMB);
				//if we are going pretty quick and not sliding we can vault instead of climbing
				Vector pPosVel;
				GetVelocity(&pPosVel, NULL);
				if ((GetLocalVelocity().Length2D() >= 300.0f) && (m_nButtons & IN_SPEED) && (GetFlags() & FL_ONGROUND)) {
					m_bBigClimb = false;
					m_bVault = true;
					m_bVaulting = true;
					CBaseViewModel *pLegsViewModel = GetViewModel(VM_LEGS);
					if (pLegsViewModel)
					{
						float idealSequence = pLegsViewModel->SelectWeightedSequence(ACT_VM_CLIMB);
						if (idealSequence >= 0)
							pLegsViewModel->SendViewModelMatchingSequence(idealSequence);
					}
				} else {
					m_bBigClimb = false;
					m_bVault = false;
					m_bVaulting = false;
				}
			}
		}



		return false;
	};

	Vector vecForward = UTIL_YawToVector(GetAbsAngles()[YAW]);

	Vector vecAbsPos = GetAbsOrigin();
	Vector vecEyePos = EyePosition();
	Vector vecEyeDuck = GetAbsOrigin() + VEC_DUCK_VIEW;

	Vector vecStart = vecEyePos;
	Vector vecEnd = vecStart + (vecForward * CLIMB_TRACE_DIST);

	trace_t tr;

	// check for ceiling first, dont want to climb through the roof
	UTIL_TraceLine(vecStart, vecStart + Vector(0, 0, vance_climb_checkray_allowedheight.GetFloat()), (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_GRATE), this, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);
	if ( vance_climb_debug.GetBool() ) 
		debugoverlay->AddLineOverlay( EyePosition(), tr.endpos, 255, 0, 0, true, 10 );

	if ( tr.DidHitWorld() ) 
	{
		return;
		
	}

	 // check for a surface in front of the player, we do it from crouch eye level
	UTIL_TraceLine(vecEyeDuck, vecEyeDuck + (vecForward * CLIMB_TRACE_DIST), (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_GRATE), this, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);

	if (!tr.DidHitWorld())
	{
		return;
	}

	if (vance_climb_debug.GetBool())
	{
		debugoverlay->AddLineOverlay(vecStart, tr.endpos, 0, 0, 255, true, 10);
		debugoverlay->AddLineOverlay(vecStart, vecEnd, 255, 0, 0, true, 10);
	}

	m_flClimbFraction = tr.fraction;

	for (float posFraction = 0.0f; checkClimbability(tr.endpos) && posFraction <= 1.0f; posFraction += 1.0f / vance_climb_checkray_count.GetInt())
	{
		vecStart = vecAbsPos + ( vecEyePos - vecAbsPos ) * posFraction;
		vecEnd = vecStart + (vecForward * CLIMB_TRACE_DIST);
		UTIL_TraceLine(vecStart, vecEnd, (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_GRATE), this, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);

		if (vance_climb_debug.GetBool())
		{
			debugoverlay->AddLineOverlay(vecStart, tr.endpos, 0, 0, 255, true, 10);
		}
	}
}

void CVancePlayer::Think()
{
	if (IsSprinting()) {
		m_fSprintTime += gpGlobals->frametime;
	} else {
		m_fSprintTime = 0.0f;
	}

	// If we're on the ground, sprinting, holding the duck key and not sliding already
	if (m_bSlideWaitingForCrouchDebounce && !(m_nButtons & IN_DUCK)) {
		m_bSlideWaitingForCrouchDebounce = false;
	}
	if ((m_nButtons & IN_SPEED) && (!m_Local.m_bDucked || !(GetFlags() & FL_ONGROUND)) && (m_nButtons & IN_DUCK))
	{
		TrySlide();
	}
	if ((m_nButtons & IN_SPEED) && m_fMidairSlideWindowTime >= gpGlobals->curtime)
	{
		TrySlide();
	}

	if (m_ParkourAction.Get() == ParkourAction::None)
	{
		// Hold jump to climb, no special conditions required
		if (m_nButtons & IN_JUMP)
		{
			TryLedgeClimb();
		}
	}
	else
	{
		switch (m_ParkourAction)
		{
			case ParkourAction::Slide:
				SlideTick();
				break;
			case ParkourAction::Climb:
				LedgeClimbTick();
				break;
			default:
				Warning( "%s L%s: SHOULD NOT BE HERE\n", __FILE__, __LINE__ );
		}
	}
	if (!m_bAllowMidairSlide && (GetFlags() & FL_ONGROUND)){
		m_bAllowMidairSlide = true;
	}

	SetNextThink(gpGlobals->curtime);
	return;
}

CON_COMMAND_F(trace_hull_test, "Trace a hull in front of you and show the output in 3D", FCVAR_NONE)
{
	CBasePlayer* pPlayer = ToBasePlayer(UTIL_GetCommandClient());
	if (!pPlayer)
		return;

	Vector vecEyes, vecForward;
	pPlayer->EyePositionAndVectors(&vecEyes, &vecForward, nullptr, nullptr);

	trace_t tr;
	UTIL_TraceHull(vecEyes, vecEyes + vecForward * 64.0f,
		VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, MASK_SOLID_BRUSHONLY, pPlayer, COLLISION_GROUP_PLAYER_MOVEMENT, &tr);

	if (tr.DidHit())
	{
		DevMsg("trace_hull_test hit!\n");
		NDebugOverlay::Box(vecEyes + vecForward * 64.0f, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, 255, 0, 0, 100, 10.0f);
		NDebugOverlay::Cross3D(tr.endpos, 1.0f, 0, 0, 255, false, 10.0f);
	}
}