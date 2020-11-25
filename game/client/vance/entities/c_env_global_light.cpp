//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Sunlight shadow control entity.
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "c_baseplayer.h"
#include "c_env_global_light.h"
#include "viewrender.h"
#include "renderparm.h"
#include "materialsystem/imesh.h"
#include "materialsystem/itexture.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialvar.h"
#include "materialsystem/imaterialsystem.h"
#include "vprof.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

C_GlobalLight* g_pCSMLight;

static ConVar csm_enabled("r_csm", "1", 0, "0 = off, 1 = on/force");


IMPLEMENT_CLIENTCLASS_DT(C_GlobalLight, DT_GlobalLight, CGlobalLight)
RecvPropVector(RECVINFO(m_shadowDirection)),
RecvPropBool(RECVINFO(m_bEnabled)),
RecvPropString(RECVINFO(m_TextureName)),
RecvPropVector(RECVINFO(m_LinearFloatLightColor)),
RecvPropVector(RECVINFO(m_LinearFloatAmbientColor)),
RecvPropFloat(RECVINFO(m_flColorTransitionTime)),
RecvPropFloat(RECVINFO(m_flSunDistance)),
RecvPropFloat(RECVINFO(m_flFOV)),
RecvPropFloat(RECVINFO(m_flNearZ)),
RecvPropFloat(RECVINFO(m_flNorthOffset)),
RecvPropBool(RECVINFO(m_bEnableShadows)),
RecvPropBool(RECVINFO(m_bEnableVolumetrics)),
END_RECV_TABLE()

C_GlobalLight::C_GlobalLight()
	: m_angSunAngles(vec3_angle)
	, m_vecLight(vec3_origin)
	, m_vecAmbient(vec3_origin)
	, m_bCascadedShadowMappingEnabled(false)
{
}

C_GlobalLight::~C_GlobalLight()
{
	if (g_pCSMLight == this)
	{
		g_pCSMLight = NULL;
	}
}


bool C_GlobalLight::IsCascadedShadowMappingEnabled() const
{
	const int iCSMCvarEnabled = csm_enabled.GetInt();
	return m_bCascadedShadowMappingEnabled && iCSMCvarEnabled >= 1;
}

bool C_GlobalLight::IsVolumetricsEnabled() const
{
	return m_bEnableVolumetrics;
}

void C_GlobalLight::OnDataChanged(DataUpdateType_t updateType)
{
	if (updateType == DATA_UPDATE_CREATED)
	{
		m_SpotlightTexture.Init(m_TextureName, TEXTURE_GROUP_OTHER, true);
	}
	if (g_pCSMLight == NULL)
	{
		g_pCSMLight = this;
	}

	BaseClass::OnDataChanged(updateType);
}

void C_GlobalLight::Spawn()
{
	BaseClass::Spawn();

	m_bOldEnableShadows = m_bEnableShadows;

	SetNextClientThink(CLIENT_THINK_ALWAYS);
}

//------------------------------------------------------------------------------
// We don't draw...
//------------------------------------------------------------------------------
bool C_GlobalLight::ShouldDraw()
{
	return false;
}

void C_GlobalLight::ClientThink()
{
	if (!m_bEnabled)
	{
		m_bCascadedShadowMappingEnabled = m_bEnabled;
		return;
	}
	m_bCascadedShadowMappingEnabled = m_bEnabled;

	Vector vDirection = m_shadowDirection;
	VectorNormalize(vDirection);

	QAngle angAngles;
	VectorAngles(vDirection, angAngles);

	m_angSunAngles = angAngles;
	m_vecLight = Vector(m_LinearFloatLightColor[0], m_LinearFloatLightColor[1], m_LinearFloatLightColor[2]);
	m_vecAmbient = Vector(m_LinearFloatAmbientColor[0], m_LinearFloatAmbientColor[1], m_LinearFloatAmbientColor[2]);

	BaseClass::ClientThink();
}