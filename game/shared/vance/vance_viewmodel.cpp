//============================ AdV Software, 2019 ============================//
//
//	Vance View-model
//
//============================================================================//

#include "cbase.h"
#include "hl2_player_shared.h"
#include "vance_viewmodel.h"
#include "shareddefs.h"
#include "bone_setup.h"

#ifdef CLIENT_DLL
#include "prediction.h"
#include "view.h"
#include "flashlighteffect.h"
#include "viewmodel_attachment.h"

#include "ivieweffects.h"
#else
#include "vance_player.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS(vance_viewmodel, CVanceViewModel);

IMPLEMENT_NETWORKCLASS_ALIASED(VanceViewModel, DT_Vance_ViewModel)

BEGIN_NETWORK_TABLE(CVanceViewModel, DT_Vance_ViewModel)
#ifdef CLIENT_DLL
	RecvPropEHandle(RECVINFO(m_hOwnerEntity)),
	RecvPropInt(RECVINFO(m_iViewModelType)),
	RecvPropInt(RECVINFO(m_iViewModelAddonModelIndex)),
	RecvPropBool(RECVINFO(m_bIsSprinting)),
	RecvPropBool(RECVINFO(m_bIsSliding)),
#else
	SendPropEHandle(SENDINFO(m_hOwnerEntity)),
	SendPropInt(SENDINFO(m_iViewModelType), 3),
	SendPropModelIndex(SENDINFO(m_iViewModelAddonModelIndex)),
	SendPropBool(SENDINFO(m_bIsSprinting)),
	SendPropBool(SENDINFO(m_bIsSliding)),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CVanceViewModel)
#ifdef CLIENT_DLL
	DEFINE_PRED_FIELD(m_iViewModelType, FIELD_INTEGER, FTYPEDESC_INSENDTABLE),
	DEFINE_PRED_FIELD(m_iViewModelAddonModelIndex, FIELD_INTEGER, FTYPEDESC_INSENDTABLE | FTYPEDESC_MODELINDEX),
#endif
END_PREDICTION_DATA()

#if defined( CLIENT_DLL )
ConVar cl_vm_sway( "cl_vm_sway", "1.0" );
ConVar cl_vm_sway_rate( "cl_vm_sway_rate", "1.0" );
ConVar cl_vm_sway_wiggle_rate( "cl_vm_sway_wiggle_rate", "1.0" );
ConVar cl_vm_sway_tilt( "cl_vm_sway_tilt", "280.0" );
ConVar cl_vm_sway_offset( "cl_vm_sway_offset", "5.0" );
ConVar cl_vm_sway_jump_velocity_division( "cl_vm_sway_jump_velocity_division", "50.0" );
ConVar cl_vm_crouch_rotatespeed("cl_vm_crouch_rotatespeed", "0.4");
ConVar cl_vm_crouch_angle("cl_vm_crouch_angle", "-6");
ConVar cl_vm_crouch_offset("cl_vm_crouch_offset", "-1");
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CVanceViewModel::CVanceViewModel()
{
	m_iViewModelType = VMTYPE_NONE;
	m_iViewModelAddonModelIndex = -1;

	// View-bobbing and swaying.
	m_flSideTiltResult = 0.0f;
	m_flSideTiltDifference = 0.0f;
	m_flForwardOffsetResult = 0.0f;
	m_flForwardOffsetDifference = 0.0f;

	// Wall collision thingy like in tarkov and stuff
	m_flCurrentDistance = 0.0f;
	m_flDistanceDifference = 0.0f;

	m_angEyeAngles = QAngle( 0.0f, 0.0f, 0.0f );
	m_angViewPunch = QAngle( 0.0f, 0.0f, 0.0f );
	m_angOldFacing = QAngle( 0.0f, 0.0f, 0.0f );
	m_angDelta = QAngle( 0.0f, 0.0f, 0.0f );
	m_angMotion = QAngle( 0.0f, 0.0f, 0.0f );
	m_angCounterMotion = QAngle( 0.0f, 0.0f, 0.0f );
	m_angCompensation = QAngle( 0.0f, 0.0f, 0.0f );

	SetNextThink(gpGlobals->curtime);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVanceViewModel::SetWeaponModel( const char *modelname, CBaseCombatWeapon *weapon )
{
	if ( !modelname || !modelname[0] || ViewModelIndex() > VM_WEAPON )
	{
		RemoveViewmodelAddon();
	}
#ifndef CLIENT_DLL
	else
	{
		CVancePlayer *pOwner = (CVancePlayer *)( GetOwnerEntity() );
		if ( pOwner )
		{
			UpdateViewmodelAddon( modelinfo->GetModelIndex( pOwner->GetArmsViewModel() ) );
		}
	}
#endif

	BaseClass::SetWeaponModel( modelname, weapon );
}

void CVanceViewModel::Think()
{
	BaseClass::Think();
	#ifdef CLIENT_DLL
	#else
	CVancePlayer *pOwner = (CVancePlayer *)(GetOwnerEntity());
	if (pOwner)
	{
		m_bIsSprinting = pOwner->IsSprinting();
		m_bIsSliding = pOwner->IsSliding();
	}
	SetNextThink(gpGlobals->curtime + 0.05f);
	#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVanceViewModel::UpdateViewmodelAddon( int iModelIndex )
{
	if ( ViewModelIndex() > VM_WEAPON )
		return;

	m_iViewModelAddonModelIndex = iModelIndex;

#ifdef CLIENT_DLL
	if ( m_iViewModelAddonModelIndex == GetModelIndex() )
		return;

	C_ViewmodelAttachmentModel *pAddon = m_hViewmodelAddon.Get();
	if ( pAddon )
	{
		pAddon->SetModelIndex( iModelIndex );
		return;
	}

	pAddon = new C_ViewmodelAttachmentModel();
	if ( !pAddon->InitializeAsClientEntity( NULL, RENDER_GROUP_VIEW_MODEL_OPAQUE ) )
	{
		pAddon->Release();
		return;
	}

	m_hViewmodelAddon = pAddon;
	pAddon->SetModelIndex( iModelIndex );
	pAddon->FollowEntity( this );
	pAddon->SetViewModel( this );
	pAddon->SetOwnerEntity( GetOwnerEntity() );
	pAddon->UpdateVisibility();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVanceViewModel::RemoveViewmodelAddon( void )
{
	m_iViewModelAddonModelIndex = -1;

#ifdef CLIENT_DLL
	if ( m_hViewmodelAddon.Get() )
	{
		m_hViewmodelAddon->Release();
	}
#endif
}

#ifdef CLIENT_DLL
void CVanceViewModel::ProcessMuzzleFlashEvent(void)
{
	FlashlightEffectManager().TriggerMuzzleFlash();
	BaseClass::ProcessMuzzleFlashEvent();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVanceViewModel::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( m_iViewModelAddonModelIndex != -1 )
	{
		UpdateViewmodelAddon( m_iViewModelAddonModelIndex );
	}
	else
	{
		RemoveViewmodelAddon();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVanceViewModel::SetDormant( bool bDormant )
{
	if ( bDormant )
	{
		RemoveViewmodelAddon();
	}

	BaseClass::SetDormant( bDormant );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVanceViewModel::UpdateOnRemove( void )
{
	RemoveViewmodelAddon();
	BaseClass::UpdateOnRemove();
}
#endif

#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: Don't rens supposed to be lowered and we have 
// finished the lowering animation
//-----------------------------------------------------------------------------
int CVanceViewModel::DrawModel( int flags )
{
	// Check for lowering the weapon
	C_BaseHLPlayer *pPlayer = dynamic_cast<C_BaseHLPlayer *>( GetOwner() );

	// Don't draw viewmodels of dead players.
	if ( !pPlayer || !pPlayer->IsAlive() )
		return 0;

	return BaseClass::DrawModel( flags );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CVanceViewModel::InternalDrawModel( int flags )
{
	CMatRenderContextPtr pRenderContext( materials );

	/*if ( ShouldFlipViewModel() )
		pRenderContext->CullMode( MATERIAL_CULLMODE_CW );*/

	// Draw the attachments together with the viewmodel so any effects applied to VM are applied to attachments as well.
	if ( m_hViewmodelAddon.Get() )
	{
		// Necessary for lighting blending
		m_hViewmodelAddon->CreateModelInstance();
		m_hViewmodelAddon->InternalDrawModel( flags );
	}

	int ret = BaseClass::InternalDrawModel( flags );

	//pRenderContext->CullMode( MATERIAL_CULLMODE_CCW );

	return ret;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CVanceViewModel::OnInternalDrawModel( ClientModelRenderInfo_t *pInfo )
{
	// Use camera position for lighting origin.
	pInfo->pLightingOrigin = &MainViewOrigin();

	// Duplicate the info onto the attachment as well.
	if ( m_hViewmodelAddon.Get() )
	{
		m_hViewmodelAddon->OnInternalDrawModel( pInfo );
	}

	return BaseClass::OnInternalDrawModel( pInfo );
}

//-----------------------------------------------------------------------------
// Purpose
//-----------------------------------------------------------------------------
void CVanceViewModel::FireEvent( const Vector& origin, const QAngle& angles, int event, const char *options )
{
	// Don't process animevents if it's not drawn.
	C_BasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if ( !pOwner || pOwner->ShouldDrawThisPlayer() )
		return;

	BaseClass::FireEvent( origin, angles, event, options );
}

//-----------------------------------------------------------------------------
// Purpose
//-----------------------------------------------------------------------------
void CVanceViewModel::AddViewModelBob( CBasePlayer *owner, Vector& eyePosition, QAngle& eyeAngles )
{
	if ( !owner )
		return;

	float dotForward = RemapVal(DotProduct(owner->GetLocalVelocity(), MainViewForward()), -cl_vm_sway_offset.GetFloat(), cl_vm_sway_offset.GetFloat(), -1.0f, 1.0f);
	float movement = abs(dotForward) > 0.5f ? cl_vm_sway_offset.GetFloat() : 0;
	m_flForwardOffsetResult = Approach(movement, m_flForwardOffsetResult, gpGlobals->frametime * 10.0f * m_flForwardOffsetDifference);
	m_flForwardOffsetDifference = fabs(movement - m_flForwardOffsetResult);

	float dotRight = RemapVal( DotProduct( owner->GetLocalVelocity(), MainViewRight() ), -cl_vm_sway_tilt.GetFloat(), cl_vm_sway_tilt.GetFloat(), -1.0f, 1.0f ) * 15 * 0.5f;
	m_flSideTiltResult = Approach( dotRight, m_flSideTiltResult, gpGlobals->frametime * 10.0f * m_flSideTiltDifference );
	m_flSideTiltDifference = fabs( dotRight - m_flSideTiltResult);

	float rollZOffset = -clamp(m_flSideTiltResult, -10, 0) * 0.1f;
	eyePosition -= MainViewUp() * rollZOffset;
	eyeAngles[ROLL] += m_flSideTiltResult;

	eyePosition -= MainViewForward() * abs(m_flForwardOffsetResult) * 0.1f;
	eyePosition -= MainViewUp() * abs(m_flForwardOffsetResult) * 0.075f;
}

#define LAG_POSITION_COMPENSATION	0.2f
#define LAG_FLIP_FACTOR				1.0f

//-----------------------------------------------------------------------------
// Purpose
//-----------------------------------------------------------------------------
void CVanceViewModel::CalcViewModelLag( Vector& origin, QAngle& angles, QAngle& original_angles )
{
	CBasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if ( !pPlayer )
		return;

	Vector dirForward, dirRight, dirUp;
	AngleVectors( pPlayer->EyeAngles(), &dirForward, &dirRight, &dirUp );

	if ( gpGlobals->frametime != 0.0f )
	{
		float flFrametime = clamp( gpGlobals->frametime, 0.001, 1.0f / 20.0f );
		float flWiggleFactor = ( 1.0f - cl_vm_sway_wiggle_rate.GetFloat() ) / 0.6f + 0.15f;
		float flSwayRate = powf( cl_vm_sway_rate.GetFloat(), 1.5f ) * 10.0f;
		float clampFac = 1.1f - MIN( ( fabs( m_angMotion[PITCH] ) + fabs( m_angMotion[YAW] ) + fabs( m_angMotion[ROLL] ) ) / 20.0f, 1.0f );

		m_angViewPunch = pPlayer->m_Local.m_vecPunchAngle;
		m_angEyeAngles = pPlayer->EyeAngles() - m_angViewPunch;

		m_angDelta[PITCH] = UTIL_AngleDiff( m_angEyeAngles[PITCH], m_angOldFacing[PITCH] ) / flFrametime / 120.0f * clampFac;
		m_angDelta[YAW] = UTIL_AngleDiff( m_angEyeAngles[YAW], m_angOldFacing[YAW] ) / flFrametime / 120.0f * clampFac;
		m_angDelta[ROLL] = UTIL_AngleDiff( m_angEyeAngles[ROLL], m_angOldFacing[ROLL] ) / flFrametime / 120.0f * clampFac;

		Vector deltaForward;
		AngleVectors( m_angDelta, &deltaForward, NULL, NULL );
		VectorNormalize( deltaForward );

		m_angOldFacing = m_angEyeAngles;

		m_angOldFacing[PITCH] -= ( pPlayer->GetLocalVelocity().z / MAX( 1, cl_vm_sway_jump_velocity_division.GetFloat() ) );

		m_angCounterMotion = Lerp( flFrametime * ( flSwayRate * ( 0.75f + ( 0.5f - flWiggleFactor ) ) ), m_angCounterMotion, -m_angMotion );
		m_angCompensation[PITCH] = AngleDiff( m_angMotion[PITCH], -m_angCounterMotion[PITCH] );
		m_angCompensation[YAW] = AngleDiff( m_angMotion[YAW], -m_angCounterMotion[YAW] );

		m_angMotion = Lerp( flFrametime * flSwayRate, m_angMotion, m_angDelta + m_angCompensation );
	}

	float flFraction = cl_vm_sway.GetFloat();
	origin += ( m_angMotion[YAW] * LAG_POSITION_COMPENSATION * 0.66f * dirRight * LAG_FLIP_FACTOR ) * flFraction;
	origin += ( m_angMotion[PITCH] * LAG_POSITION_COMPENSATION * dirUp ) * flFraction;

	angles[PITCH] += ( m_angMotion[PITCH] ) * flFraction;
	angles[YAW] += ( m_angMotion[YAW] * 0.66f * LAG_FLIP_FACTOR ) * flFraction;
	angles[ROLL] += ( m_angCounterMotion[ROLL] * 0.5f * LAG_FLIP_FACTOR ) * flFraction;
}

//-----------------------------------------------------------------------------
// Purpose
//-----------------------------------------------------------------------------
void CVanceViewModel::CalcViewModelCollision(Vector& origin, QAngle& angles, CBasePlayer* owner)
{
	CBaseVanceWeapon* pWeapon = GetWeapon();
	if (pWeapon->GetVanceWpnData().iWeaponLength == 0)
		return;

	if (owner->IsInAVehicle())
		return;

	Vector forward, right, up;
	AngleVectors(owner->EyeAngles(), &forward, &right, &up);
	trace_t tr;
	UTIL_TraceLine(owner->EyePosition(), owner->EyePosition() + forward * pWeapon->GetVanceWpnData().iWeaponLength, MASK_SHOT, owner, COLLISION_GROUP_NONE, &tr);
	m_flCurrentDistance = Approach(tr.fraction, m_flCurrentDistance, gpGlobals->frametime * 10.0f * m_flDistanceDifference);
	m_flDistanceDifference = fabs(tr.fraction - m_flCurrentDistance);

	origin += forward * pWeapon->GetVanceWpnData().vCollisionOffset.x * (1.0f - m_flCurrentDistance);
	origin += right * pWeapon->GetVanceWpnData().vCollisionOffset.y * (1.0f - m_flCurrentDistance);
	origin += up * pWeapon->GetVanceWpnData().vCollisionOffset.z * (1.0f - m_flCurrentDistance);

	angles += pWeapon->GetVanceWpnData().angCollisionRotation * (1.0f - m_flCurrentDistance);
}

//deals with additional offsets from crouching and jumping and the like
void CVanceViewModel::CalcViewModelBasePose(Vector& origin, QAngle& angles, CBasePlayer* owner)
{
	Vector forward, right, up;
	AngleVectors(owner->EyeAngles(), &forward, &right, &up);

	//sprinting (weapons should stay more level while sprinting)
	if (GetSequenceActivity(GetSequence()) == ACT_VM_SPRINT || GetSequenceActivity(GetSequence()) == ACT_VM_SPRINT2 || GetSequenceActivity(GetSequence()) == ACT_VM_SPRINT_EXTENDED || GetSequenceActivity(GetSequence()) == ACT_VM_SPRINT2_EXTENDED) {
		m_flSprinting += gpGlobals->frametime / 0.3f;
	} else if (GetSequenceActivity(GetSequence()) == ACT_VM_PRIMARYATTACK || GetSequenceActivity(GetSequence()) == ACT_VM_FIRE_EXTENDED || GetSequenceActivity(GetSequence()) == ACT_VM_SECONDARYATTACK) {
		m_flSprinting = 0.0f;
	} else {
		m_flSprinting -= gpGlobals->frametime / 0.3f;
	}
	m_flSprinting = Clamp(m_flSprinting, 0.0f, 1.0f);
	float fSprintingScale = ((1.0f - (m_flSprinting < 0.5 ? 2 * m_flSprinting * m_flSprinting : 1 - powf(-2 * m_flSprinting + 2, 2) / 2)) * 0.6f) + 0.4f;
	fSprintingScale = Clamp((angles.x * fSprintingScale) - angles.x, 0.0f, 90.0f) / 90.0f;
	fSprintingScale *= fSprintingScale;
	angles.x += fSprintingScale * 90.0f; //easeInOutQuad with some extra stuff

	//movement view bob
	if (GetSequenceActivity(GetSequence()) == ACT_VM_SPRINT || GetSequenceActivity(GetSequence()) == ACT_VM_SPRINT2 || GetSequenceActivity(GetSequence()) == ACT_VM_SPRINT_EXTENDED || GetSequenceActivity(GetSequence()) == ACT_VM_SPRINT2_EXTENDED) {
		if (!m_bSprintSeqTracking) {
			m_flSprintSeqLastStart = gpGlobals->curtime;
			m_bSprintSeqTracking = true;
		}
	} else if (m_bSprintSeqTracking) {
		m_bSprintSeqTracking = false;
	}
	if (m_bSprintSeqTracking && m_flSprintBob == 0.0f) {
		m_flSprintSeqLastStartActive = m_flSprintSeqLastStart;
	}
	if (GetSequenceActivity(GetSequence()) == ACT_VM_WALK || GetSequenceActivity(GetSequence()) == ACT_VM_WALK_EXTENDED) {
		if (!m_bWalkSeqTracking) {
			m_flWalkSeqLastStart = gpGlobals->curtime;
			m_bWalkSeqTracking = true;
		}
	} else if (m_bWalkSeqTracking) {
		m_bWalkSeqTracking = false;
	}
	if (m_bWalkSeqTracking && m_flWalkBob == 0.0f) {
		m_flWalkSeqLastStartActive = m_flWalkSeqLastStart;
	}
	if (owner->GetWaterLevel() != 3 && (owner->GetFlags() & FL_ONGROUND) && owner->GetLocalVelocity().Length2D() >= 300 && m_bIsSprinting.Get() && !m_bIsSliding.Get() &&
		!(GetSequenceActivity(GetSequence()) == ACT_VM_SPRINT || GetSequenceActivity(GetSequence()) == ACT_VM_SPRINT2 || GetSequenceActivity(GetSequence()) == ACT_VM_SPRINT_EXTENDED || GetSequenceActivity(GetSequence()) == ACT_VM_SPRINT2_EXTENDED))
	{
		m_flSprintBob += gpGlobals->frametime / 0.2f;
		m_flWalkBob -= gpGlobals->frametime / 0.2f;
	} else if (owner->GetWaterLevel() != 3 && (owner->GetFlags() & FL_ONGROUND) && owner->GetLocalVelocity().Length2D() >= 100 && !m_bIsSliding.Get() && !(GetSequenceActivity(GetSequence()) == ACT_VM_WALK || GetSequenceActivity(GetSequence()) == ACT_VM_WALK_EXTENDED)) {
		m_flWalkBob += gpGlobals->frametime / 0.2f;
		m_flSprintBob -= gpGlobals->frametime / 0.2f;
	} else {
		m_flSprintBob -= gpGlobals->frametime / 0.2f;
		m_flWalkBob -= gpGlobals->frametime / 0.2f;
	}
	if (GetSequenceActivity(GetSequence()) == ACT_VM_PRIMARYATTACK || GetSequenceActivity(GetSequence()) == ACT_VM_FIRE_EXTENDED || GetSequenceActivity(GetSequence()) == ACT_VM_SECONDARYATTACK) {
		m_flSprintBob = 0.0f;
		m_flWalkBob = 0.0f;
	}
	m_flSprintBob = Clamp(m_flSprintBob, 0.0f, 1.0f);
	m_flWalkBob = Clamp(m_flWalkBob, 0.0f, 1.0f);
	float flSprintBobTimeline = ((m_flSprintSeqLastStartActive - gpGlobals->curtime) / 0.66666666f); //length of the sprint anims (40 frames)
	flSprintBobTimeline = flSprintBobTimeline - floorf(flSprintBobTimeline);
	origin += right * sinf((flSprintBobTimeline + 0.25f) * 6.28318) * 0.7f * m_flSprintBob; //main side to side sway
	origin += up * ((-cosf((flSprintBobTimeline - 0.25f)*12.56636f) + sinf((flSprintBobTimeline - 0.25f)*6.28318) * 0.2f) + 0.8f * (-cosf((flSprintBobTimeline - 0.25f)*12.56636f) + sinf((flSprintBobTimeline - 0.25f)*6.28318) * 0.2f)) * 0.3f * m_flSprintBob;//step jolts
	angles.z += sinf(((flSprintBobTimeline + 0.1 - floorf(flSprintBobTimeline + 0.1)) + 0.25f) * 6.28318) * 2.0f * m_flSprintBob; //rotational sway
	origin += up * sinf(((flSprintBobTimeline + 0.1 - floorf(flSprintBobTimeline + 0.1)) + 0.25f) * 6.28318) * 0.28f * m_flSprintBob; //vertical sway to counteract rotational sway
	float flWalkBobTimeline = ((m_flWalkSeqLastStartActive - gpGlobals->curtime) / 0.93333333f); //length of the walk anims (56 frames)
	flWalkBobTimeline = flWalkBobTimeline - floorf(flWalkBobTimeline);
	origin += right * sinf((flWalkBobTimeline + 0.25f) * 6.28318) * 0.45f * m_flWalkBob; //main side to side sway
	origin += up * ((-cosf((flWalkBobTimeline - 0.25f)*12.56636f) + sinf((flWalkBobTimeline - 0.25f)*6.28318) * 0.2f) + 0.8f * (-cosf((flWalkBobTimeline - 0.25f)*12.56636f) + sinf((flWalkBobTimeline - 0.25f)*6.28318) * 0.2f)) * 0.15f * m_flWalkBob;//step jolts
	angles.z += sinf(((flWalkBobTimeline + 0.05 - floorf(flWalkBobTimeline + 0.05)) + 0.25f) * 6.28318) * 1.5f * m_flWalkBob; //rotational tilt sway
	origin += up * sinf(((flWalkBobTimeline + 0.05 - floorf(flWalkBobTimeline + 0.05)) + 0.25f) * 6.28318) * 0.21f * m_flWalkBob; //vertical sway to counteract rotational tilt sway
	flWalkBobTimeline = flWalkBobTimeline - 0.05 - floorf(flWalkBobTimeline - 0.05); //offset it for the rotation step jolts
	angles.x += ((-cosf((flWalkBobTimeline - 0.25f)*12.56636f) + sinf((flWalkBobTimeline - 0.25f)*6.28318) * 0.2f) + 0.8f * (-cosf((flWalkBobTimeline - 0.25f)*12.56636f) + sinf((flWalkBobTimeline - 0.25f)*6.28318) * 0.2f)) * 0.15f * m_flWalkBob;//rotation step jolts

	//jump offset
	bool bInAir = false;
	if (owner->GetFlags() & FL_ONGROUND) 
	{
		bInAir = false; 
	}
	else {
		bInAir = true;
	};
	if (bInAir != m_bJumpModeInAir)
	{
		m_bJumpModeInAir = bInAir;
		if (bInAir) 
		{
			m_fJumpBlendIn = 0.0f;
		} else {
			m_fJumpBlendOut = 0.0f;
		}
	}
	//calculated offset based on velocity, which is faded in and out
	float jumpOffset = Clamp(owner->GetLocalVelocity().z * -0.003f, -1.0f, 1.0f);
	if (jumpOffset >= 0.0f) 
	{
		jumpOffset = sinf((jumpOffset * 3.14159) / 2); //easeOutSine
	}
	else {
		jumpOffset *= -1.0f;
		jumpOffset = sinf((jumpOffset * 3.14159) / 2); //easeOutSine
		jumpOffset *= -1.0f;
	}
	if (bInAir)
	{
		//ease in the jump, ease out the effects of the previous blend out
		m_fJumpBlendIn += gpGlobals->frametime / 0.2f;
		m_fJumpBlendIn = Clamp(m_fJumpBlendIn, 0.0f, 1.0f);

		jumpOffset *= -(cosf(3.141590894f * m_fJumpBlendIn) - 1) / 2; //easeInOutSine
		jumpOffset += m_fJumpBlendOutFinalPrevious * (-(cosf(3.14159 * m_fJumpBlendIn) - 1) / 2); //m_fJumpBlendOutFinalPrevious * easeInOutSine

		//save final jump offset
		m_fJumpOffsetFinalPrevious = jumpOffset;
	}
	else {
		//ease out the saved final offset
		m_fJumpBlendOut += gpGlobals->frametime / 1.3f;
		m_fJumpBlendOut = Clamp(m_fJumpBlendOut, 0.0f, 1.0f);

		jumpOffset = m_fJumpOffsetFinalPrevious;
		jumpOffset *= 1 - (m_fJumpBlendOut == 0 ? 0 : m_fJumpBlendOut == 1 ? 1 : powf(2, -10 * m_fJumpBlendOut) * sinf((m_fJumpBlendOut * 10 - 0.75) * ((2 * 3.14159f) / 3)) + 1); //inverse easeOutElastic

		m_fJumpBlendOutFinalPrevious = jumpOffset;
	}
	//origin += Vector(0, 0, jumpOffset) * 0.8;
	origin += jumpOffset * forward * -0.2f;
	origin += jumpOffset * up * 1.0f;

	//crouching: we are ducked or ducking, but we arent ducked AND ducking (which happens when standing up), and we are on the ground not crouch jumping
	if ((owner->GetFlags() & FL_DUCKING || owner->m_Local.m_bDucking) && !(owner->GetFlags() & FL_DUCKING && owner->m_Local.m_bDucking) && owner->GetFlags() & FL_ONGROUND) 
	{
		m_flDucking += gpGlobals->frametime / cl_vm_crouch_rotatespeed.GetFloat();
	}
	else {
		m_flDucking -= gpGlobals->frametime / cl_vm_crouch_rotatespeed.GetFloat();
	}
	m_flDucking = Clamp(m_flDucking, 0.0f, 1.0f);
	float flDuckingEased = (m_flDucking < 0.5 ? 4 * m_flDucking * m_flDucking * m_flDucking : 1 - powf(-2 * m_flDucking + 2, 3) / 2); //easeInOutCubic
	angles += QAngle(0.0f, 0.0f, cl_vm_crouch_angle.GetFloat()) * flDuckingEased;
	origin += right * cl_vm_crouch_offset.GetFloat() * flDuckingEased;
}


//-----------------------------------------------------------------------------
// Purpose
//-----------------------------------------------------------------------------
void CVanceViewModel::CalcViewModelView(CBasePlayer* owner, const Vector& eyePosition, const QAngle& eyeAngles)
{
	QAngle vmangoriginal = eyeAngles;
	QAngle vmangles = eyeAngles;
	Vector vmorigin = eyePosition;

	CBaseVanceWeapon* pWeapon = GetWeapon();
	//Allow weapon lagging
	if (pWeapon != NULL)
	{
		CalcViewModelBasePose(vmorigin, vmangles, owner);
		CalcViewModelCollision(vmorigin, vmangles, owner);

		pWeapon->CalcIronsight(vmorigin, vmangles);
	}
	// Add model-specific bob even if no weapon associated (for head bob for off hand models)
	AddViewModelBob(owner, vmorigin, vmangles);
	// This was causing weapon jitter when rotating in updated CS:S; original Source had this in above InPrediction block  07/14/10
	// Add lag
	CalcViewModelLag(vmorigin, vmangles, vmangoriginal);

	if (!prediction->InPrediction())
	{
		// Let the viewmodel shake at about 10% of the amplitude of the player's view
		vieweffects->ApplyShake(vmorigin, vmangles, 0.1);
	}

	SetLocalOrigin(vmorigin);
	SetLocalAngles(vmangles);
}
#endif // CLIENT_DLL
