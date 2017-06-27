#include "Options.h"

namespace Options
{
	void Unload()
	{
		Options::Aim::bAimbotEnabled = false;
		Options::Aim::bRCSEnabled = false;

		Options::ESP::bGlowEnabled = false;
		Options::ESP::bDrawEnabled = false;

		Options::Misc::bBhopEnabled = false;
		Options::Misc::bShowRanks = false;
		Options::Misc::bAutoAccept = false;
	}

	// all vars here must be declared in Options.h and *should* be implemented in the gui

	bool bMainWindowOpen = false;

	namespace Aim
	{
		bool bAimbotEnabled = false;
		bool bRCSEnabled = false;
		
		float flFov = 1.f;
		bool bSmooth = true;
		float flSmooth = 2.f;

		bool bSilent = false;

		bool bOnShoot = true;
		bool bOnAimkey = false;
		int nAimkey = ButtonCode_t::MOUSE_4;

		int nBone = static_cast<int>(Bone::BONE_NECK);

		bool bVisibleCheck = true;

		bool bLockOnFriendly = false;
	}

	namespace ESP
	{
		bool bGlowEnabled = false;
		
		bool bDrawEnabled = false;
		bool bCornerBoxes = false;

		bool bDrawPlayers = true;
		bool bDrawPlayerBox = true;
		bool bDrawSkeleton = false;
		bool bOnlyEnemies = false;

		bool bShowName = false;
		bool bShowHealth = false;
		bool bShowHealthText = false;

		bool bDrawBomb = true;
		bool bDrawNades = false;

		float fAllyColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
		float fEnemyColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	}
	
	namespace Misc
	{
		bool bBhopEnabled = false;
		bool bShowRanks = true;
		bool bAutoAccept = false;
	}
}
