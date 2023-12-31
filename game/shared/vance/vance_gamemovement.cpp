#include "cbase.h"
#include "vance_gamemovement.h"
#include "in_buttons.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar vance_slide_time( "vance_slide_time", "2.0", FCVAR_CHEAT );
ConVar vance_slide_movescale( "vance_slide_movescale", "0.05", FCVAR_CHEAT );
ConVar vance_crouch_speed_scale("vance_duck_move_speed_scale", "0.5", FCVAR_CHEAT);
ConVar vance_duck_down_speed("vance_duck_down_speed", "0.35", FCVAR_CHEAT);
ConVar vance_duck_down_slide_speed("vance_duck_down_slide_speed", "0.2", FCVAR_CHEAT);
ConVar vance_duck_up_speed("vance_duck_up_speed", "0.2", FCVAR_CHEAT);

CVanceGameMovement::CVanceGameMovement()
{
	m_flFrictionScale = 1.0f;
}

void CVanceGameMovement::HandleSpeedCrop()
{
	if ( !( m_iSpeedCropped & SPEED_CROPPED_PARKOUR ) && ( GetVancePlayer()->IsSliding() ) )
	{
		float frac = vance_slide_movescale.GetFloat();
		mv->m_flForwardMove *= frac;
		mv->m_flSideMove *= frac;
		mv->m_flUpMove *= frac;
		m_iSpeedCropped |= SPEED_CROPPED_PARKOUR;
	}
}

void CVanceGameMovement::FullWalkMove()
{
	m_flFrictionScale = GetVancePlayer()->m_flSlideFrictionScale;

	HandleSpeedCrop();

	BaseClass::FullWalkMove();
}

bool CVanceGameMovement::CheckJumpButton( void )
{
	if ( GetVancePlayer()->IsSliding() )
		return false;
	else
		return BaseClass::CheckJumpButton();
}

void CVanceGameMovement::Duck( void )
{
	BaseClass::Duck();
}

bool CVanceGameMovement::CanUnduck()
{
	if ( GetVancePlayer()->IsSliding() )
		return false;

	return BaseClass::CanUnduck();
}

void CVanceGameMovement::HandleDuckingSpeedCrop()
{
	if (!(m_iSpeedCropped & SPEED_CROPPED_DUCK) && (player->GetFlags() & FL_DUCKING) && (player->GetGroundEntity() != NULL))
	{
		float frac = vance_crouch_speed_scale.GetFloat();
		mv->m_flForwardMove *= frac;
		mv->m_flSideMove *= frac;
		mv->m_flUpMove *= frac;
		m_iSpeedCropped |= SPEED_CROPPED_DUCK;
	}
}

//ducking logic replacement thats hopefully less fucked up
void CVanceGameMovement::PlayerMove() //just using this as a think function lol
{
	BaseClass::PlayerMove(); //very important
	
	//crouching logic

	//if the player tries to duck during a climb it fucks shit up, so if the player is climbing and the player presses duck we not only have to ignore it, we have to ignore that duck input until its let go of
	if (m_bWaitingForDuckDebounce && !(mv->m_nButtons & IN_DUCK)) {
		m_bWaitingForDuckDebounce = false;
	} else if ((mv->m_nButtons & IN_DUCK) && GetVancePlayer()->IsClimbing()) {
		m_bWaitingForDuckDebounce = true;
	}

	//figure out duck fraction and if we are actually crouching or not
	if (((player->m_Local.m_bDucked || player->m_Local.m_bDucking) && !(player->m_Local.m_bDucked && player->m_Local.m_bDucking) || (mv->m_nButtons & IN_DUCK) || (GetVancePlayer()->IsSliding() && (player->GetFlags() & FL_ONGROUND))) && !m_bWaitingForDuckDebounce && !GetVancePlayer()->IsVaulting()) {
		if (!(player->GetFlags() & FL_ONGROUND)){
			m_fDuckFraction = 1.0f;
		} else if (GetVancePlayer()->IsSliding()) {
			m_fDuckFraction += gpGlobals->frametime / vance_duck_down_slide_speed.GetFloat();
		} else {
			m_fDuckFraction += gpGlobals->frametime / vance_duck_down_speed.GetFloat();
		}
		m_bWasInDuckJump = !(player->GetFlags() & FL_ONGROUND);
	} else {
		if (m_bWasInDuckJump) {
			m_fDuckFraction = 0.0f;
			m_bWasInDuckJump = false;
		} else {
			m_fDuckFraction -= gpGlobals->frametime / vance_duck_up_speed.GetFloat();
		}
	}
	m_fDuckFraction = Clamp(m_fDuckFraction, 0.0f, 1.0f);
	Vector vecDuckOffset = GetDuckedEyeOffset(m_fDuckFraction < 0.5 ? 2 * m_fDuckFraction * m_fDuckFraction : 1 - powf(-2 * m_fDuckFraction + 2, 2) / 2); //easeInOutQuad
	//add in the correctional offset from vaulting
	m_vecVaultOffset = GetVancePlayer()->GetVaultCameraAdjustment();
	if (GetVancePlayer()->IsVaulting()){
		m_fVaultOFfsetBlend = 1.0f;
	} else {
		m_fVaultOFfsetBlend -= gpGlobals->frametime / 0.27f;
	}
	m_fVaultOFfsetBlend = Clamp(m_fVaultOFfsetBlend, 0.0f, 1.0f);
	vecDuckOffset.z += m_vecVaultOffset.z * (m_fVaultOFfsetBlend < 0.5 ? 2 * m_fVaultOFfsetBlend * m_fVaultOFfsetBlend : 1 - powf(-2 * m_fVaultOFfsetBlend + 2, 2) / 2); //easeInOutQuad
	player->SetViewOffset(vecDuckOffset);
}

Vector CVanceGameMovement::GetDuckedEyeOffset(float duckFraction)
{
	Vector vDuckHullMin = GetPlayerMins(true);
	Vector vStandHullMin = GetPlayerMins(false);

	float fMore = (vDuckHullMin.z - vStandHullMin.z);

	Vector vecDuckViewOffset = GetPlayerViewOffset(true);
	Vector vecStandViewOffset = GetPlayerViewOffset(false);
	Vector temp = player->GetViewOffset();
	temp.z = ((vecDuckViewOffset.z - fMore) * duckFraction) +
		(vecStandViewOffset.z * (1 - duckFraction));
	return temp;
}

// Expose our interface.
static CVanceGameMovement g_GameMovement;
IGameMovement *g_pGameMovement = (IGameMovement *)&g_GameMovement;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CGameMovement, IGameMovement, INTERFACENAME_GAMEMOVEMENT, g_GameMovement );