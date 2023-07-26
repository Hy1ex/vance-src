//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
//
// Health.cpp
//
// implementation of CHudHealth class
//
#include "cbase.h"
#include "hud.h"
#include "hud_macros.h"
#include "view.h"

#include "iclientmode.h"

#include <KeyValues.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/AnimationController.h>

#include <vgui/ILocalize.h>

using namespace vgui;

#include "hudelement.h"
#include "hud_numericdisplay.h"

#include "convar.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
#include <c_basehlplayer.h>

#define INIT_HEALTH -1
#define INIT_BAT	-1

int m_iNewBat;
int m_iBat;

//-----------------------------------------------------------------------------
// Purpose: Health panel
//-----------------------------------------------------------------------------
class CHudHealth : public CHudElement, public CHudNumericDisplay
{
	DECLARE_CLASS_SIMPLE(CHudHealth, CHudNumericDisplay);

public:
	CHudHealth(const char *pElementName);
	virtual void Init(void);
	virtual void VidInit(void);
	virtual void Reset(void);
	virtual void OnThink();
	void MsgFunc_Damage(bf_read &msg);
	void MsgFunc_Battery(bf_read& msg);

private:
	// old variables
	int		m_iHealth;
	int		m_iBat;
	int		m_bitsDamage;
};

DECLARE_HUDELEMENT(CHudHealth);
DECLARE_HUD_MESSAGE(CHudHealth, Damage);
DECLARE_HUD_MESSAGE(CHudHealth, Battery);


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudHealth::CHudHealth(const char *pElementName) : CHudElement(pElementName), CHudNumericDisplay(NULL, "HudHealth")
{
	SetHiddenBits(HIDEHUD_HEALTH | HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudHealth::Init()
{
	HOOK_HUD_MESSAGE(CHudHealth, Damage);
	usermessages->HookMessage("Battery", __MsgFunc_CHudHealth_Battery);
	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudHealth::Reset()
{
	m_iHealth = INIT_HEALTH;
	m_iBat = INIT_BAT;
	m_bitsDamage = 0;

	wchar_t *tempString = g_pVGuiLocalize->Find("+");

	if (tempString)
	{
		SetLabelText(tempString);
	}
	else
	{
		SetLabelText(L"+");
	}
	m_VanceNewHealth = true;
	g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("HealthPulse");
	SetDisplayValue(m_iHealth);
	SetSecondaryValue(m_iNewBat);
	SetShouldDisplaySecondaryValue(true);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudHealth::VidInit()
{
	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudHealth::OnThink()
{
	int newHealth = 0;


	C_BasePlayer *local = C_BasePlayer::GetLocalPlayer();


	if (local)
	{
		// Never below zero
		newHealth = MAX(local->GetHealth(), 0);
	}

	// Only update the fade if we've changed health

	if (newHealth == m_iHealth && m_iNewBat == m_iBat)
	{
		return;
	}

	m_iHealth = newHealth;

	if (m_iHealth >= 20)
	{

	}
	else if (m_iHealth > 0)
	{
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("HealthIncreasedBelow20");
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("HealthLow");
	}

	SetDisplayValue(m_iHealth);
	SetSecondaryValue(m_iNewBat);
	m_iBat = m_iNewBat;

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudHealth::MsgFunc_Damage(bf_read &msg)
{

	int armor = msg.ReadByte();	// armor
	int damageTaken = msg.ReadByte();	// health
	long bitsDamage = msg.ReadLong(); // damage bits
	bitsDamage; // variable still sent but not used

	Vector vecFrom;

	vecFrom.x = msg.ReadBitCoord();
	vecFrom.y = msg.ReadBitCoord();
	vecFrom.z = msg.ReadBitCoord();

	// Actually took damage?
	if (damageTaken > 0 || armor > 0)
	{
		if (damageTaken > 0)
		{
			// start the animation
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("HealthDamageTaken");
		}
	}
}

void CHudHealth::MsgFunc_Battery(bf_read& msg)
{
	m_iNewBat = msg.ReadShort();
}
