//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//
#include "cbase.h"
#include "vance_baseweapon_shared.h"
#include "iviewrender_beams.h"
#include "c_weapon__stubs.h"
#include "materialsystem/imaterial.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar vance_357_scope_speed("vance_357_ads_in_speed", "3", FCVAR_REPLICATED);
ConVar vance_357_scope_out_speed("vance_357_ads_out_speed", "2", FCVAR_REPLICATED);

class C_Weapon357 : public C_BaseVanceWeapon
{
	DECLARE_CLASS(C_Weapon357, C_BaseVanceWeapon);
public:
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();

	float easeInOutSine(float x)
	{
		return -(cosf(3.14159 * x) - 1) / 2;
	}

	float easeOutCubic(float x)
	{
		return 1 - powf(1 - x, 3);
	}

	virtual void CalcIronsight(Vector& origin, QAngle& angles)
	{
		CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

		if (pPlayer) {
			// bad bad, but im lazy
			if (m_bScoped)
			{
				if (m_flScope < 1.0f)
					m_flScope += gpGlobals->frametime * vance_357_scope_speed.GetFloat() * 5 * (1.2 - easeOutCubic(m_flScope));
				else
					m_flScope = 1.0f;
			}
			else
			{
				if (m_flScope > 0.0f)
					m_flScope -= gpGlobals->frametime * vance_357_scope_out_speed.GetFloat();
				else
					m_flScope = 0.0f;
			}

			Vector forward, right, up;
			AngleVectors(pPlayer->EyeAngles(), &forward, &right, &up);

			Vector offset = vec3_origin;
			offset += forward * GetVanceWpnData().vIronsightOffset.x;
			offset += right * GetVanceWpnData().vIronsightOffset.y;
			offset += up * GetVanceWpnData().vIronsightOffset.z;

			float spline = easeInOutSine(m_flScope);
			float curve = SmoothCurve(m_flScope);

			origin += offset * spline;
			angles.z -= curve * 6.0f; //*((int)m_bScoped * 2 - 1) * (m_bScoped ? 1.0f : 0.5f);
			origin.z -= curve * 0.5f;
		}
		else {
			BaseClass::CalcIronsight(origin, angles);
		}
	}
	

private:
	CNetworkVar( bool, m_bScoped );
	float		m_flScope;
};


STUB_WEAPON_CLASS_IMPLEMENT(weapon_357, C_Weapon357);

IMPLEMENT_CLIENTCLASS_DT(C_Weapon357, DT_Weapon357, CWeapon357)
	RecvPropBool( RECVINFO(m_bScoped)),
END_RECV_TABLE()

