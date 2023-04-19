//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_ENVPROJECTEDTEXTURE_H
#define C_ENVPROJECTEDTEXTURE_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseentity.h"
#include "basetypes.h"
#include "c_light_manager.h"

#ifdef ASW_PROJECTED_TEXTURES

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class C_EnvProjectedTexture : public C_BaseEntity
{
	DECLARE_CLASS( C_EnvProjectedTexture, C_BaseEntity );
public:
	DECLARE_CLIENTCLASS();

	void SetMaterial( IMaterial *pMaterial );
	void SetLightColor( byte r, byte g, byte b, byte a );
	void SetSize( float flSize );
	void SetRotation( float flRotation );

	virtual void OnDataChanged( DataUpdateType_t updateType );
	void	ShutDownLightHandle( void );

#ifdef MAPBASE
	virtual void Simulate();
#else
	virtual bool Simulate();
#endif

	void	UpdateLight( void );

	C_EnvProjectedTexture();
	~C_EnvProjectedTexture();

	static void SetVisibleBBoxMinHeight( float flVisibleBBoxMinHeight ) { m_flVisibleBBoxMinHeight = flVisibleBBoxMinHeight; }
	static float GetVisibleBBoxMinHeight( void ) { return m_flVisibleBBoxMinHeight; }
	static C_EnvProjectedTexture *Create( );

	virtual bool					IsTransparent() { return false; }
	virtual bool					IsTwoPass() { return false; }

	virtual void UpdateOnRemove();

	virtual void GetRenderBoundsWorldspace(Vector& mins, Vector& maxs)
	{
		if (m_bEnableVolumetrics)
		{
			mins = m_vecRenderBoundsMin;
			maxs = m_vecRenderBoundsMax;
		}
		else
		{
			BaseClass::GetRenderBoundsWorldspace(mins, maxs);
		}
	}

	virtual void GetRenderBounds(Vector& mins, Vector& maxs)
	{
		if (m_bEnableVolumetrics)
		{
			mins = m_vecRenderBoundsMin - GetAbsOrigin();
			maxs = m_vecRenderBoundsMax - GetAbsOrigin();
		}
		else
		{
			BaseClass::GetRenderBounds(mins, maxs);
		}
	}

	virtual bool ShouldDraw(void) { return false; }

	virtual bool ShouldReceiveProjectedTextures(int flags) { return false; }


private:

	inline bool IsBBoxVisible( void );
	bool IsBBoxVisible( Vector vecExtentsMin,
						Vector vecExtentsMax );

	void GetShadowViewSetup( CViewSetup &setup );
	void UpdateVolumetricsState();
	void RemoveVolumetrics();

	Vector m_vecRenderBoundsMin, m_vecRenderBoundsMax;
	ClientShadowHandle_t m_LightHandle;
	bool m_bForceUpdate;

	EHANDLE	m_hTargetEntity;
#ifdef MAPBASE
	bool m_bDontFollowTarget;
#endif

	bool		m_bState;
	bool		m_bAlwaysUpdate;
	float		m_flLightFOV;
#ifdef MAPBASE
	float		m_flLightHorFOV;
#endif
	bool		m_bEnableShadows;
	bool		m_bLightOnlyTarget;
	bool		m_bLightWorld;
	bool		m_bCameraSpace;
	float		m_flBrightnessScale;
	color32		m_LightColor;
	Vector		m_CurrentLinearFloatLightColor;
	float		m_flCurrentLinearFloatLightAlpha;
#ifdef MAPBASE
	float		m_flCurrentBrightnessScale;
#endif
	float		m_flColorTransitionTime;
	float		m_flAmbient;
	float		m_flNearZ;
	float		m_flFarZ;
	char		m_SpotlightTextureName[ MAX_PATH ];
	CTextureReference m_SpotlightTexture;
	int			m_nSpotlightTextureFrame;
	int			m_nShadowQuality;
#ifdef MAPBASE
	float		m_flConstantAtten;
	float		m_flLinearAtten;
	float		m_flQuadraticAtten;
	float		m_flShadowAtten;
	float		m_flShadowFilter;

	bool		m_bAlwaysDraw;
	//bool		m_bProjectedTextureVersion;
#endif

	Vector	m_vecExtentsMin;
	Vector	m_vecExtentsMax;

	static float m_flVisibleBBoxMinHeight;

	bool m_bEnableVolumetrics;
	bool m_bEnableVolumetricsLOD;
	float m_flVolumetricsFadeDistance;
	int m_iVolumetricsQuality;
	float m_flVolumetricsMultiplier;
	float m_flVolumetricsQualityBias;
	int m_iCurrentVolumetricsSubDiv;
	volume_light_t m_volumelight;

};



bool C_EnvProjectedTexture::IsBBoxVisible( void )
{
	return IsBBoxVisible( GetAbsOrigin() + m_vecExtentsMin, GetAbsOrigin() + m_vecExtentsMax );
}

#else

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class C_EnvProjectedTexture : public C_BaseEntity
{
	DECLARE_CLASS( C_EnvProjectedTexture, C_BaseEntity );
public:
	DECLARE_CLIENTCLASS();

	C_EnvProjectedTexture();
	~C_EnvProjectedTexture();

	virtual void OnDataChanged( DataUpdateType_t updateType );
	void	ShutDownLightHandle( void );

	virtual void Simulate();

	void	UpdateLight( bool bForceUpdate );

	bool	ShadowsEnabled();

	float	GetFOV();

private:

	ClientShadowHandle_t m_LightHandle;

	EHANDLE	m_hTargetEntity;

	bool	m_bState;
	float	m_flLightFOV;
	bool	m_bEnableShadows;
	bool	m_bLightOnlyTarget;
	bool	m_bLightWorld;
	bool	m_bCameraSpace;
	color32	m_cLightColor;
	float	m_flAmbient;
	char	m_SpotlightTextureName[ MAX_PATH ];
	int		m_nSpotlightTextureFrame;
	int		m_nShadowQuality;
	bool	m_bCurrentShadow;

public:
	C_EnvProjectedTexture  *m_pNext;
};

C_EnvProjectedTexture* GetEnvProjectedTextureList();

#endif

#endif // C_ENVPROJECTEDTEXTURE_H
