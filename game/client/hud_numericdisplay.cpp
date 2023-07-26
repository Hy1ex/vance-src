//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "hud_numericdisplay.h"
#include "iclientmode.h"
#include <convar.h>
#include <Color.h>
#include <KeyValues.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/IVGui.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
#include <cmath>

using namespace vgui;


wchar_t suit = 'C';
wchar_t health = '+';

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudNumericDisplay::CHudNumericDisplay(vgui::Panel *parent, const char *name) : BaseClass(parent, name)
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();

	SetParent(pParent);

	m_iValue = 0;
	m_VanceNewHealth = false;
	m_VanceDivider = false;
	m_LabelText[0] = 0;
	m_iSecondaryValue = 0;
	m_bDisplayValue = true;
	m_bDisplaySecondaryValue = false;
	m_bIndent = false;
	m_bIsTime = false;
}

//-----------------------------------------------------------------------------
// Purpose: Resets values on restore/new map
//-----------------------------------------------------------------------------
void CHudNumericDisplay::Reset()
{

	m_flBlur = 0.0f;


}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudNumericDisplay::SetDisplayValue(int value)
{
	m_iValue = value;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudNumericDisplay::SetSecondaryValue(int value)
{
	m_iSecondaryValue = value;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudNumericDisplay::SetShouldDisplayValue(bool state)
{
	m_bDisplayValue = state;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudNumericDisplay::SetShouldDisplaySecondaryValue(bool state)
{
	m_bDisplaySecondaryValue = state;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudNumericDisplay::SetLabelText(const wchar_t *text)
{
	wcsncpy(m_LabelText, text, sizeof(m_LabelText) / sizeof(wchar_t));
	m_LabelText[(sizeof(m_LabelText) / sizeof(wchar_t)) - 1] = 0;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudNumericDisplay::SetIndent(bool state)
{
	m_bIndent = state;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudNumericDisplay::SetIsTime(bool state)
{
	m_bIsTime = state;
}

//-----------------------------------------------------------------------------
// Purpose: paints a number at the specified position
//-----------------------------------------------------------------------------
void CHudNumericDisplay::PaintNumbers(HFont font, int xpos, int ypos, int value)
{

	surface()->DrawSetTextFont(font);
	wchar_t unicode[6];
	if (!m_bIsTime)
	{
		V_snwprintf(unicode, ARRAYSIZE(unicode), L"%d", value);
	}
	else
	{
		int iMinutes = value / 60;
		int iSeconds = value - iMinutes * 60;
#ifdef PORTAL
		// portal uses a normal font for numbers so we need the seperate to be a renderable ':' char
		if (iSeconds < 10)
			V_snwprintf(unicode, ARRAYSIZE(unicode), L"%d:0%d", iMinutes, iSeconds);
		else
			V_snwprintf(unicode, ARRAYSIZE(unicode), L"%d:%d", iMinutes, iSeconds);
#else
		if (iSeconds < 10)
			V_snwprintf(unicode, ARRAYSIZE(unicode), L"%d`0%d", iMinutes, iSeconds);
		else
			V_snwprintf(unicode, ARRAYSIZE(unicode), L"%d`%d", iMinutes, iSeconds);
#endif
	}

	// adjust the position to take into account 3 characters
	int charWidth = surface()->GetCharacterWidth(font, '0');

	if (value < 100 && m_bIndent)
	{
		xpos += charWidth;
	}
	if (value < 10 && m_bIndent)
	{
		xpos += charWidth;

	}






	surface()->DrawSetTextFont(m_hNumberGlowFont);
	surface()->DrawSetTextColor(Color(255, 0, 0, 255));
	surface()->DrawSetTextPos(xpos, ypos);
	surface()->DrawUnicodeString(unicode);
	surface()->DrawSetTextFont(m_hNumberFont);
	//	surface()->DrawStr


	surface()->DrawSetTextColor(Color(255, 95, 95, 255));
	surface()->DrawSetTextPos(xpos, ypos);
	surface()->DrawUnicodeString(unicode);
	//	surface()->Draw


}

//-----------------------------------------------------------------------------
// Purpose: draws the text
//-----------------------------------------------------------------------------
void CHudNumericDisplay::PaintLabel(void)
{

	//Error("FinNum is: %i, YPOS is: %f, ratio is: %f, fonTall is: %f, tall is %i", finNum, text_ypos, (664.f / 480.f), fonTall, tall);

	//	Error("%i", surface()->GetFontTall(m_hLabelFont));

	surface()->DrawSetTextFont(m_hLabelGlowFont);
	surface()->DrawSetTextColor(GetFgColor());
	surface()->DrawSetTextPos(text_xpos, text_ypos + 1);
	surface()->DrawUnicodeString(m_LabelText);

	if (m_VanceNewHealth == false)
	{
		surface()->DrawSetTextFont(m_hLabelFont);
		surface()->DrawSetTextPos(text_xpos, text_ypos);
		surface()->DrawSetTextColor(Color(255, 95, 95, 255));
		surface()->DrawUnicodeString(m_LabelText);
	}

	if (m_VanceNewHealth == true)
	{

		surface()->DrawSetTextPos(((digit2_xpos - digit_xpos) / 2) + digit_xpos, separator_ypos);
		surface()->DrawSetTextColor(GetFgColor());
		surface()->DrawUnicodeString(L"¡");

		surface()->DrawSetTextFont(m_hLabelFont);
		surface()->DrawSetTextPos(((digit2_xpos - digit_xpos) / 2) + digit_xpos, separator_ypos);
		surface()->DrawUnicodeString(L"¡");

		surface()->DrawSetTextFont(m_hLabelGlowFont);
		surface()->DrawSetTextColor(GetFgColor());
		surface()->DrawSetTextPos(text2_xpos, text_ypos);
		surface()->DrawUnicodeString(L"C");

		float value = m_iValue / 100.0f;
		float value2 = m_iSecondaryValue / 100.0f;

		//BUG: Only needed because of how screwy the battery icon in the default HL2 iconset is. WILL NOT BE NEEDED LATER!!!!!!
		float newval = Lerp(value, 0.01f, 0.76f);
		float newval2 = Lerp(value2, 0.1f, 0.76f);

		gHUD.DrawIconProgressBar2ColorVanceSpecific(text_xpos, text_ypos, "HudLabel", health, newval, Color(96, 34, 34, 175), Color(255, 95, 95, 255), CHud::HUDPB_VERTICAL);
		gHUD.DrawIconProgressBar2ColorVanceSpecific(text2_xpos, text_ypos, "HudLabel", suit, newval2, Color(96, 34, 34, 175), Color(255, 95, 95, 255), CHud::HUDPB_VERTICAL);

	}

	if (m_VanceDivider == true)
	{
		surface()->DrawSetTextFont(m_hLabelGlowFont);
		int ofs = 0;
		int ofs2 = 0;
		int charWidth = surface()->GetCharacterWidth(m_hLabelFont, '0');

		if (m_iValue >= 10)
		{
			ofs += charWidth * 0.75;
		}


		//	if (m_iSecondaryValue < 10)
		//	{
		//		ofs2 -= charWidth * 0.75;
		//	}

		//	if (m_iSecondaryValue < 100)
		//	{
		//		ofs2 -= charWidth * 0.75;
		//}

		surface()->DrawSetTextPos(((((digit2_xpos + ofs2) - (digit_xpos + ofs)) / 2) + digit_xpos) + ofs, separator_ypos);
		surface()->DrawSetTextColor(GetFgColor());
		surface()->DrawUnicodeString(L"¡");

		surface()->DrawSetTextFont(m_hLabelFont);
		surface()->DrawSetTextPos(((((digit2_xpos + ofs2) - (digit_xpos + ofs)) / 2) + digit_xpos) + ofs, separator_ypos);
		surface()->DrawUnicodeString(L"¡");
	}

}

//-----------------------------------------------------------------------------
// Purpose: renders the vgui panel
//-----------------------------------------------------------------------------
void CHudNumericDisplay::Paint()
{
	if (m_bDisplayValue)
	{
		// draw our numbers
		surface()->DrawSetTextColor(GetFgColor());
		PaintNumbers(m_hNumberFont, digit_xpos, digit_ypos, m_iValue);
	}

	// total ammo
	if (m_bDisplaySecondaryValue)
	{
		surface()->DrawSetTextColor(GetFgColor());
		PaintNumbers(m_hNumberFont, digit2_xpos, digit_ypos, m_iSecondaryValue);

	}

	PaintLabel();
}



