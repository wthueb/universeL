#pragma once

#include "Fonts.h"

#include "SDK\IInputSystem.h"

#include <vector>

namespace Options
{
	void Init();
	void Unload();
	
	// all vars here must be declared in Options.cpp and *should* be implemented in the gui

	extern bool bMainWindowOpen;

	struct bone_t
	{
		Bone bone;
		const char* name;
	};

	extern std::vector<bone_t> bones;

	namespace Aim
	{
		extern bool bAimbotEnabled;
		extern bool bRCSEnabled;
		
		extern float flFov;
		extern bool bSmooth;
		extern float flSmooth;

		extern bool bSilent;

		extern bool bOnShoot;
		extern bool bOnAimkey;
		extern int nAimkey;

		extern int nBone;

		extern bool bVisibleCheck;

		extern bool bReAim;
		extern bool bLockOnFriendly;
	}

	namespace ESP
	{
		extern bool bGlowEnabled;

		extern bool bDrawEnabled;
		extern bool bCornerBoxes;

		extern bool bDrawPlayers;
		extern bool bDrawPlayerBox;
		extern bool bDrawSkeleton;
		extern bool bOnlyEnemies;
		
		extern bool bShowName;
		extern bool bShowHealth;
		extern bool bShowHealthText;

		extern bool bDrawBomb;
		extern bool bDrawNades;

		extern float fAllyColor[4];
		extern float fEnemyColor[4];
	}

	namespace Misc
	{
		extern bool bBhopEnabled;
		extern bool bShowRanks;
		extern bool bAutoAccept;
	}
}
