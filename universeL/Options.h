#pragma once

#include "Fonts.h"

#include "SDK\IInputSystem.h"
#include "PaintKitParser.h"

#include <string>
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

	struct key_t
	{
		ButtonCode_t key;
		const char* name;
	};

	extern std::vector<bone_t> bones;
	extern std::vector<key_t> keys;

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

	namespace Skins
	{
		namespace Gloves
		{
			extern bool bEnabled;

			extern int nItemDefinitionIndex;
			extern int nPaintkit;
			extern float flWear;
		}
	}

	namespace Misc
	{
		extern std::string strClantag;
		extern bool bClantagScroll;

		extern bool bBhopEnabled;
		extern bool bShowRanks;
		extern bool bAutoAccept;
	}
}
