#include "cbase.h"
#include "IDeferredExt.h"
#include "viewrender.h"
#include "view_scene.h"
#include "entities/c_env_global_light.h"
#include "mathlib/vmatrix.h"
#include "materialsystem/imaterialvar.h"
#include "c_light_manager.h"

CON_COMMAND(deferred_lights_count, "Get amount of deferred light sources")
{
	Msg("amount of deferred lights: %i\n", GetLightingManager()->CountLight());
}

static void Volumetrics_change_Callback(IConVar* var, const char* pOldValue, float flOldValue)
{
	if (GetLightingManager()->GetViewVolumetrics() != NULL)
	{
		GetLightingManager()->ClearVolumetricsMesh();
	}
}
ConVar r_volumetrics_subdiv("r_volumetrics_subdiv", "128", 0, "", Volumetrics_change_Callback);
ConVar r_volumetrics_distance("r_volumetrics_distance", "1024", 0, "", Volumetrics_change_Callback);

static CLightingManager __g_lightingMan;

CLightingManager* GetLightingManager()
{
	return &__g_lightingMan;
}

static void MatrixSourceToDeviceSpace(VMatrix& m)
{
	Vector x, y, z;
	m.GetBasisVectors(x, y, z);
	m.SetBasisVectors(-y, z, -x);
}

void CLightingManager::AddLight(volume_light_t* l)
{
	Assert(!m_hVolumetricLights.HasElement(l));

	m_hVolumetricLights.AddToTail(l);
}

bool CLightingManager::RemoveLight(volume_light_t* l)
{
	return m_hVolumetricLights.FindAndRemove(l);
}

void CLightingManager::CommitLights()
{
}

void CLightingManager::FlushLights()
{
	m_hVolumetricLights.RemoveAll();
}

void CLightingManager::PrepareShadowLights()
{
	/*FOR_EACH_VEC_FAST(def_light_t*, m_hDeferredLights, l)
	{
		if (l->bSpotShadow)
			l->UpdateMatrix();
	}
	FOR_EACH_VEC_FAST_END*/
	return;
}

void CLightingManager::LevelInitPreEntity()
{
	if (!m_matVolumetricsMaterial.IsValid())
	{
		m_matVolumetricsMaterial.Init(materials->FindMaterial("shaders/light_volumetrics", TEXTURE_GROUP_OTHER));
		if (m_matVolumetricsMaterial->IsErrorMaterial())
			Warning("Couldn't find volumetric light material: %s\n", "shaders/light_volumetrics");
	}

	FlushLights();
}

void CLightingManager::RenderVolumetrics(const CViewSetup &view)
{
	CMatRenderContextPtr pRenderContext(materials);
	pRenderContext->SetFlashlightMode(true);

	FOR_EACH_VEC_FAST(volume_light_t*, m_hVolumetricLights, l)
	{
		if (m_meshViewVolumetrics == NULL)
		{
			RebuildVolumetricMesh();
		}

		// skip rendering this volumetric light if handle is invalid
		// it means that light is not even in our view or we cant see it anyway
		if (*l->handle == CLIENTSHADOW_INVALID_HANDLE)
			continue;
		
		UpdateScreenEffectTexture(0, view.x, view.y, view.width, view.height);

		FlashlightState_t state = *l->state;
		state.m_Color[0] *= l->intensity;
		state.m_Color[1] *= l->intensity;
		state.m_Color[2] *= l->intensity;
		state.m_Color[3] *= l->intensity;

		pRenderContext->SetFlashlightStateEx(state, l->spotWorldToTex, l->depth);

		//var = m_matVolumetricsMaterial->FindVar("$iscsm", NULL);
		//var->SetIntValue(1);
		ShadowHandle_t shadowHandle = g_pClientShadowMgr->GetShadowHandle(*l->handle);
		const Frustum_t& frustum = shadowmgr->GetFlashlightFrustum(shadowHandle);

		for (int i = 0; i < 6; i++)
		{
			Vector4D plane;
			VectorMultiply(frustum.GetPlane(i)->normal, 1, plane.AsVector3D());
			plane.w = frustum.GetPlane(i)->dist + 0.1f;
			pRenderContext->PushCustomClipPlane(plane.Base());
		}
		

		matrix3x4_t viewTransform;
		AngleMatrix(view.angles, view.origin, viewTransform);

		pRenderContext->MatrixMode(MATERIAL_MODEL);
		pRenderContext->PushMatrix();
		pRenderContext->LoadMatrix(viewTransform);

		pRenderContext->Bind(m_matVolumetricsMaterial);
		m_meshViewVolumetrics->Draw();

		pRenderContext->MatrixMode(MATERIAL_MODEL);
		pRenderContext->PopMatrix();		
		
		for (int i = 0; i < 6; i++)
		{
			pRenderContext->PopCustomClipPlane();
		}

	}
	FOR_EACH_VEC_FAST_END;

	pRenderContext->SetFlashlightMode(false);
	return;
}


void CLightingManager::ClearVolumetricsMesh()
{
	if (m_meshViewVolumetrics != NULL)
	{
		CMatRenderContextPtr pRenderContext(materials);
		pRenderContext->DestroyStaticMesh(m_meshViewVolumetrics);
		m_meshViewVolumetrics = NULL;
	}
}

void CLightingManager::RebuildVolumetricMesh()
{
	ClearVolumetricsMesh();

	CViewSetup setup;
	setup = m_view;
	setup.origin = vec3_origin;
	setup.angles = vec3_angle;
	setup.zFar = r_volumetrics_distance.GetFloat();

	VMatrix world2View, view2Proj, world2Proj, world2Pixels;
	render->GetMatricesForView(setup, &world2View, &view2Proj, &world2Proj, &world2Pixels);
	VMatrix proj2world;
	MatrixInverseGeneral(world2Proj, proj2world);

	const Vector origin(vec3_origin);
	QAngle angles(vec3_angle);
	Vector fwd, right, up;
	AngleVectors(angles, &fwd, &right, &up);

	Vector nearPlane[4];
	Vector farPlane[4];
	Vector3DMultiplyPositionProjective(proj2world, Vector(-1, -1, 1), farPlane[0]);
	Vector3DMultiplyPositionProjective(proj2world, Vector(1, -1, 1), farPlane[1]);
	Vector3DMultiplyPositionProjective(proj2world, Vector(1, 1, 1), farPlane[2]);
	Vector3DMultiplyPositionProjective(proj2world, Vector(-1, 1, 1), farPlane[3]);

	Vector3DMultiplyPositionProjective(proj2world, Vector(-1, -1, 0), nearPlane[0]);
	Vector3DMultiplyPositionProjective(proj2world, Vector(1, -1, 0), nearPlane[1]);
	Vector3DMultiplyPositionProjective(proj2world, Vector(1, 1, 0), nearPlane[2]);
	Vector3DMultiplyPositionProjective(proj2world, Vector(-1, 1, 0), nearPlane[3]);
	/*
	DebugDrawLine( farPlane[ 0 ], farPlane[ 1 ], 0, 0, 255, true, -1 );
	DebugDrawLine( farPlane[ 1 ], farPlane[ 2 ], 0, 0, 255, true, -1 );
	DebugDrawLine( farPlane[ 2 ], farPlane[ 3 ], 0, 0, 255, true, -1 );
	DebugDrawLine( farPlane[ 3 ], farPlane[ 0 ], 0, 0, 255, true, -1 );
	DebugDrawLine( nearPlane[ 0 ], nearPlane[ 1 ], 0, 0, 255, true, -1 );
	DebugDrawLine( nearPlane[ 1 ], nearPlane[ 2 ], 0, 0, 255, true, -1 );
	DebugDrawLine( nearPlane[ 2 ], nearPlane[ 3 ], 0, 0, 255, true, -1 );
	DebugDrawLine( nearPlane[ 3 ], nearPlane[ 0 ], 0, 0, 255, true, -1 );
	DebugDrawLine( farPlane[ 0 ], nearPlane[ 0 ], 0, 0, 255, true, -1 );
	DebugDrawLine( farPlane[ 1 ], nearPlane[ 1 ], 0, 0, 255, true, -1 );
	DebugDrawLine( farPlane[ 2 ], nearPlane[ 2 ], 0, 0, 255, true, -1 );
	DebugDrawLine( farPlane[ 3 ], nearPlane[ 3 ], 0, 0, 255, true, -1 );
	*/
	const Vector vecDirections[3] = {
		(farPlane[0] - vec3_origin),
		(farPlane[1] - farPlane[0]),
		(farPlane[3] - farPlane[0]),
	};

	const VertexFormat_t vertexFormat = VERTEX_POSITION | VERTEX_TEXCOORD_SIZE(0, 2);
	CMatRenderContextPtr pRenderContext(materials);
	m_meshViewVolumetrics = pRenderContext->CreateStaticMesh(vertexFormat, TEXTURE_GROUP_OTHER);

	const int iCvarSubDiv = r_volumetrics_subdiv.GetInt();
	int iCurrentVolumetricsSubDiv = (iCvarSubDiv > 2) ? iCvarSubDiv : 3;

	CMeshBuilder meshBuilder;
	meshBuilder.Begin(m_meshViewVolumetrics, MATERIAL_TRIANGLES, iCurrentVolumetricsSubDiv * 6 - 4);

	for (int x = 1; x < iCurrentVolumetricsSubDiv * 2; x++)
	{
		// never 0.0 or 1.0
		float flFracX = float(x) / (iCurrentVolumetricsSubDiv * 2);
		flFracX = powf( flFracX, 3.0f );
		//flFracX = powf(flFracX, m_flVolumetricsQualityBias);

		Vector v00 = origin + vecDirections[0] * flFracX;
		Vector v10 = v00 + vecDirections[1] * flFracX;
		Vector v11 = v10 + vecDirections[2] * flFracX;
		Vector v01 = v00 + vecDirections[2] * flFracX;

		meshBuilder.Position3f(XYZ(v00));
		meshBuilder.TexCoord2f(0, 0.0f, 0.0f);
		meshBuilder.AdvanceVertex();

		meshBuilder.Position3f(XYZ(v10));
		meshBuilder.TexCoord2f(0, 0.0f, 0.0f);
		meshBuilder.AdvanceVertex();

		meshBuilder.Position3f(XYZ(v11));
		meshBuilder.TexCoord2f(0, 0.0f, 0.0f);
		meshBuilder.AdvanceVertex();

		meshBuilder.Position3f(XYZ(v00));
		meshBuilder.TexCoord2f(0, 0.0f, 0.0f);
		meshBuilder.AdvanceVertex();

		meshBuilder.Position3f(XYZ(v11));
		meshBuilder.TexCoord2f(0, 0.0f, 0.0f);
		meshBuilder.AdvanceVertex();

		meshBuilder.Position3f(XYZ(v01));
		meshBuilder.TexCoord2f(0, 0.0f, 0.0f);
		meshBuilder.AdvanceVertex();
	}

	meshBuilder.End();
}

void CLightingManager::SetRenderConstants(const VMatrix& ScreenToWorld,
	const CViewSetup& setup)
{
	m_matScreenToWorld = ScreenToWorld;

	m_vecViewOrigin = setup.origin;
	AngleVectors(setup.angles, &m_vecForward);

	m_flzNear = setup.zNear;
	m_view = setup;
}