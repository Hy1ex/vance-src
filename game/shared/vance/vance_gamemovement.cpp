#include "cbase.h"
#include "vance_gamemovement.h"
#include "in_buttons.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar vance_slide_time( "vance_slide_time", "2.0", FCVAR_CHEAT );
ConVar vance_slide_movescale( "vance_slide_movescale", "0.05", FCVAR_CHEAT );
ConVar vance_slide_eyeoffsetspeed( "vance_slide_eyeoffsetspeed", "0.1", FCVAR_CHEAT );

CVanceGameMovement::CVanceGameMovement()
{
	m_flFrictionScale = 1.0f;
}

void CVanceGameMovement::HandleSpeedCrop()
{
#ifdef CLIENT_DLL
	if ( !( m_iSpeedCropped & SPEED_CROPPED_PARKOUR ) && ( GetVancePlayer()->m_ParkourAction != ParkourAction::None ) )
#else
	if ( !( m_iSpeedCropped & SPEED_CROPPED_PARKOUR ) && ( GetVancePlayer()->m_ParkourAction.Get() != ParkourAction::None ) )
#endif
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

	if ( GetVancePlayer()->m_flSlideEndTime > gpGlobals->curtime )
	{
		float flSlideInFraction = ( GetVancePlayer()->m_flSlideEndTime - gpGlobals->curtime ) / vance_slide_time.GetFloat();
		flSlideInFraction = clamp(flSlideInFraction, 0, 1);
		float flSlideOutFraction = 1.0f - flSlideInFraction;

		float offsetSpeed = vance_slide_eyeoffsetspeed.GetFloat();
		flSlideInFraction = RemapValClamped(flSlideInFraction, 0, offsetSpeed, 0, 1);
		flSlideOutFraction = RemapValClamped(flSlideOutFraction, offsetSpeed, 0, 1, 0);

		SetDuckedEyeOffset(SimpleSpline(flSlideInFraction * flSlideOutFraction));
	}

	HandleSpeedCrop();

	BaseClass::FullWalkMove();
}

bool CVanceGameMovement::CheckJumpButton( void )
{
#ifdef CLIENT_DLL
	if ( GetVancePlayer()->m_ParkourAction != ParkourAction::None )
#else
	if ( GetVancePlayer()->m_ParkourAction.Get() != ParkourAction::None )
#endif
		return false;
	else
		return BaseClass::CheckJumpButton();
}

void CVanceGameMovement::Duck( void )
{
	if (IsDead())
		return;

	// dont crouch when running, we are sliding now
#ifdef CLIENT_DLL
	if ( mv->m_nButtons & IN_SPEED || GetVancePlayer()->m_ParkourAction != ParkourAction::None )
#else
	if ( mv->m_nButtons & IN_SPEED || GetVancePlayer()->m_ParkourAction.Get() != ParkourAction::None )
#endif
		return;

	BaseClass::Duck();
}

// Expose our interface.
static CVanceGameMovement g_GameMovement;
IGameMovement *g_pGameMovement = (IGameMovement *)&g_GameMovement;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CGameMovement, IGameMovement, INTERFACENAME_GAMEMOVEMENT, g_GameMovement );