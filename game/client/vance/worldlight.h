//========= Copyright (C) 2021, CSProMod Team, All rights reserved. =========//
// Written: November 2011
// Author: Saul Rennison
#pragma once

#include "igamesystem.h"

class Vector;
struct dworldlight_t;

class CWorldLights : public CAutoGameSystem
{
public:
	CWorldLights();
	~CWorldLights() { Clear(); }

	bool GetBrightestLightSource( const Vector &vecPosition, Vector &vecLightPos, Vector &vecLightBrightness );

	bool Init() override;
	void LevelInitPreEntity() override;
	void LevelShutdownPostEntity() override { Clear(); }

private:
	void Clear();

	int m_nWorldLights;
	dworldlight_t *m_pWorldLights;
};

// Singleton accessor
extern CWorldLights *g_pWorldLights;
