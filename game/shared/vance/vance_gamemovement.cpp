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
	BaseClass::PlayerMove(); //very important lol
	
	//crouching logic
	//crouching: we are ducked or ducking, but we arent ducked AND ducking (which happens when standing up), and we are on the ground not crouch jumping
	if ((player->m_Local.m_bDucked || player->m_Local.m_bDucking) && !(player->m_Local.m_bDucked && player->m_Local.m_bDucking) || (mv->m_nButtons & IN_DUCK) || (GetVancePlayer()->IsSliding() && (player->GetFlags() & FL_ONGROUND))) {
		if (player->m_Local.m_bInDuckJump){
			m_fDuckFraction = 1.0f;
		} else if (GetVancePlayer()->IsSliding()) {
			m_fDuckFraction += gpGlobals->frametime / vance_duck_down_slide_speed.GetFloat();
		} else {
			m_fDuckFraction += gpGlobals->frametime / vance_duck_down_speed.GetFloat();
		}
		m_bWasInDuckJump = (player->m_Local.m_bInDuckJump && !(player->GetFlags() & FL_ONGROUND));
	} else {
		if (m_bWasInDuckJump) {
			m_fDuckFraction = 0.0f;
			m_bWasInDuckJump = false;
		} else {
			m_fDuckFraction -= gpGlobals->frametime / vance_duck_up_speed.GetFloat();
		}
	}
	m_fDuckFraction = Clamp(m_fDuckFraction, 0.0f, 1.0f);
	SetDuckedEyeOffset(m_fDuckFraction < 0.5 ? 2 * m_fDuckFraction * m_fDuckFraction : 1 - powf(-2 * m_fDuckFraction + 2, 2) / 2); //easeInOutQuad
}

void CVanceGameMovement::SetDuckedEyeOffset(float duckFraction)
{
	Vector vDuckHullMin = GetPlayerMins(true);
	Vector vStandHullMin = GetPlayerMins(false);

	float fMore = (vDuckHullMin.z - vStandHullMin.z);

	Vector vecDuckViewOffset = GetPlayerViewOffset(true);
	Vector vecStandViewOffset = GetPlayerViewOffset(false);
	Vector temp = player->GetViewOffset();
	temp.z = ((vecDuckViewOffset.z - fMore) * duckFraction) +
		(vecStandViewOffset.z * (1 - duckFraction));
	player->SetViewOffset(temp);
}

// Expose our interface.
static CVanceGameMovement g_GameMovement;
IGameMovement *g_pGameMovement = (IGameMovement *)&g_GameMovement;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CGameMovement, IGameMovement, INTERFACENAME_GAMEMOVEMENT, g_GameMovement );