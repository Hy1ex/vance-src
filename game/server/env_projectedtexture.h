//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef ENV_PROJECTEDTEXTURE_H
#define ENV_PROJECTEDTEXTURE_H
#ifdef _WIN32
#pragma once
#endif

#define ENV_PROJECTEDTEXTURE_STARTON			(1<<0)
#define ENV_PROJECTEDTEXTURE_ALWAYSUPDATE		(1<<1)

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CEnvProjectedTexture : public CPointEntity
{
	DECLARE_CLASS( CEnvProjectedTexture, CPointEntity );
public:
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CEnvProjectedTexture();
	bool KeyValue( const char *szKeyName, const char *szValue );
	//virtual bool GetKeyValue( const char *szKeyName, char *szValue, int iMaxLen );

	// Always transmit to clients
	virtual int UpdateTransmitState();
	virtual void Activate( void );

	void InputTurnOn( inputdata_t &inputdata );
	void InputTurnOff( inputdata_t &inputdata );
	void InputAlwaysUpdateOn( inputdata_t &inputdata );
	void InputAlwaysUpdateOff( inputdata_t &inputdata );
	void InputSetFOV( inputdata_t &inputdata );
	void InputSetTarget( inputdata_t &inputdata );
	void InputSetCameraSpace( inputdata_t &inputdata );
	void InputSetLightOnlyTarget( inputdata_t &inputdata );
	void InputSetLightWorld( inputdata_t &inputdata );
	void InputSetEnableShadows( inputdata_t &inputdata );
	//void InputSetLightColor( inputdata_t &inputdata );
	void InputSetSpotlightTexture( inputdata_t &inputdata );
	void InputSetAmbient( inputdata_t &inputdata );
	void InputSetEnableVolumetrics(inputdata_t& inputdata);

	void InitialThink( void );

	CNetworkHandle( CBaseEntity, m_hTargetEntity );

// commented out for create flashlight for 600 years
//private:

	CNetworkVar(	bool,		m_bState );
	CNetworkVar(	bool,		m_bAlwaysUpdate );
	CNetworkVar(	float,		m_flLightFOV );
	CNetworkVar(	bool,		m_bEnableShadows );
	CNetworkVar(	bool,		m_bLightOnlyTarget );
	CNetworkVar(	bool,		m_bLightWorld );
	CNetworkVar(	bool,		m_bCameraSpace );

	//CNetworkColor32(			m_LightColor );
	CNetworkVector(				m_LinearFloatLightColor );
	CNetworkVar(	int,		m_nLinear );
	CNetworkVar(	int,		m_nQuadratic );
	CNetworkVar(	int,		m_nConstant );

	CNetworkVar(	float,		m_flAmbient );
	CNetworkString(				m_SpotlightTextureName, MAX_PATH );
	CNetworkVar(	int,		m_nSpotlightTextureFrame );
	CNetworkVar(	float,		m_flNearZ );
	CNetworkVar(	float,		m_flFarZ );
	CNetworkVar(	int,		m_nShadowQuality );

	CNetworkVar(bool, m_bEnableVolumetrics);
	CNetworkVar(bool, m_bEnableVolumetricsLOD);
	CNetworkVar(float, m_flVolumetricsFadeDistance);
	CNetworkVar(int, m_iVolumetricsQuality);
	CNetworkVar(float, m_flVolumetricsQualityBias);
	CNetworkVar(float, m_flVolumetricsMultiplier);
};

#endif	// ENV_PROJECTEDTEXTURE_H