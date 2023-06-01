#include "cbase.h"
#include "vance_gamemovement.h"
#include "in_buttons.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar vance_slide_time( "vance_slide_time", "2.0", FCVAR_CHEAT );
ConVar vance_slide_movescale( "vance_slide_movescale", "0.05", FCVAR_CHEAT );
ConVar vance_crouch_speed_scale("vance_duck_speed_scale", "0.5", FCVAR_CHEAT);

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


// Expose our interface.
static CVanceGameMovement g_GameMovement;
IGameMovement *g_pGameMovement = (IGameMovement *)&g_GameMovement;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CGameMovement, IGameMovement, INTERFACENAME_GAMEMOVEMENT, g_GameMovement );