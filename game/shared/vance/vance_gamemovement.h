//=============================================================================//
//
// Purpose: Special handling for Vance slide
//
//=============================================================================//

#include "hl_gamemovement.h"
#include "vance_shareddefs.h"

#if defined( CLIENT_DLL )

	#include "c_vance_player.h"
	#define CVancePlayer C_VancePlayer
#else

	#include "vance_player.h"

#endif

//-----------------------------------------------------------------------------
// Purpose: Vance specific movement code
//-----------------------------------------------------------------------------
class CVanceGameMovement : public CHL2GameMovement
{
	typedef CHL2GameMovement BaseClass;

public:
	CVanceGameMovement();

protected:

	// Overrides
	void			HandleSpeedCrop();
	virtual void	FullWalkMove();
	virtual bool	CheckJumpButton(void);
	virtual void	Duck(void);
	virtual bool	CanUnduck();
	virtual void	HandleDuckingSpeedCrop();

	virtual void	PlayerMove();
	Vector			GetDuckedEyeOffset(float duckFraction);
	float			m_fDuckFraction = 0.0f;
	bool			m_bWasInDuckJump = false;
	Vector			m_vecVaultOffset = Vector(0, 0, 0);
	float			m_fVaultOFfsetBlend = 0.0f;

private:

	CVancePlayer *GetVancePlayer();
};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
inline CVancePlayer *CVanceGameMovement::GetVancePlayer()
{
	return reinterpret_cast<CVancePlayer *>( player );
}