//============================ AdV Software, 2019 ============================//
//
//	Vance player entity
//
//============================================================================//

#ifndef C_VANCE_PLAYER_H
#define C_VANCE_PLAYER_H

#include "c_basehlplayer.h"
#include "flashlighteffect.h"
#include "c_bobmodel.h"
#include "vance_shareddefs.h"

class C_VancePlayer : public C_BaseHLPlayer
{
	DECLARE_CLASS(C_VancePlayer, C_BaseHLPlayer);
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();
public:
	C_VancePlayer();
	~C_VancePlayer();

	virtual void UpdateFlashlight();

	virtual const char *GetFlashlightTextureName() const { return "effects/flashlight002"; }
	virtual float GetFlashlightFOV() const;
	virtual float GetFlashlightFarZ() const { return 750.0f; }
	virtual float GetFlashlightLinearAtten() const { return 100.0f; }
	virtual bool CastsFlashlightShadows() { return true; }

	void CalcFlashlightLag(Vector& vecForward, Vector& vecRight, Vector& vecUp);

	virtual void OverrideView(CViewSetup *pSetup);

	C_BaseCombatWeapon *GetDeployingWeapon() const;

	virtual ShadowType_t	ShadowCastType() { return SHADOWS_RENDER_TO_TEXTURE_DYNAMIC; }

	float				GetKickAnimationLength() { return m_flKickAnimLength; }

	virtual Vector		GetAutoaimVector(float flScale);


	bool IsSliding()
	{
		return m_ParkourAction == ParkourAction::Slide;
	}

	Vector GetVaultCameraAdjustment(){
		return m_vecVaultCameraAdjustment;
	}

	bool IsVaulting()
	{
		return m_ParkourAction == ParkourAction::Climb && m_bVaulting;
	}

protected:

	void				AddViewLandingKick(Vector& eyeOrigin, QAngle& eyeAngles);
	void				AddViewSlide(Vector& eyeOrigin, QAngle& eyeAngles);
	void				AddViewBob(Vector& eyeOrigin, QAngle& eyeAngles, bool calculate = false);

	virtual void		CalcPlayerView(Vector& eyeOrigin, QAngle& eyeAngles, float& fov);
	virtual void		CalcViewModelView(const Vector& eyeOrigin, const QAngle& eyeAngles);

private:
	//landing kick
	float  m_fLandingKickEaseIn = 2.0f; //first half is down, second half is up
	float  m_fLandingKickEaseOut = 0.0f;
	float  m_fLandingKickLastOffset = 0.0f;
	float  m_fLandingKickLastVelocity = 0.0f;

	//slide
	float  m_fSlideBlend = 0.0f;

	//bob
	C_BobModel *m_pBobViewModel;
	float m_flBobModelAmount;
	QAngle m_angLastBobAngle;
	Vector m_vecLastBobPos;

	float m_fBobTime;
	float m_fLastBobTime;

	float m_flKickAnimLength;
	QAngle m_vecLagAngles;
	CInterpolatedVar<QAngle> m_LagAnglesHistory;

	ParkourAction m_ParkourAction;

	float m_flSlideEndTime;
	float m_flSlideFrictionScale;

	//parkour stuff
	bool m_bVaulting;
	Vector m_vecVaultCameraAdjustment;

	friend class CVanceGameMovement;
};

#endif // C_VANCE_PLAYER_H