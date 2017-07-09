#include "Options.h"

namespace Options
{
	std::vector<bone_t> bones;
	std::vector<key_t> keys;

	void Init()
	{
		bones.push_back({ Bone::BONE_HEAD, "head" });
		bones.push_back({ Bone::BONE_NECK, "neck" });
		bones.push_back({ Bone::BONE_UPPER_SPINAL_COLUMN, "chest" });
		bones.push_back({ Bone::BONE_MIDDLE_SPINAL_COLUMN, "nips xd" });
		bones.push_back({ Bone::BONE_LOWER_SPINAL_COLUMN, "stomach" });
		bones.push_back({ Bone::BONE_HIP, "hip" });
		bones.push_back({ Bone::BONE_PELVIS, "pelvis" });

		keys.push_back({ ButtonCode_t::KEY_XBUTTON_RTRIGGER, "none" }); // xd
		keys.push_back({ ButtonCode_t::MOUSE_4, "mouse4" });
		keys.push_back({ ButtonCode_t::MOUSE_5, "mouse5" });
		keys.push_back({ ButtonCode_t::MOUSE_MIDDLE, "mouse3" });
		keys.push_back({ ButtonCode_t::KEY_LALT, "alt" });
		keys.push_back({ ButtonCode_t::KEY_W, "w" } );
	}

	void Unload()
	{
		Options::Aim::bAimbotEnabled = false;
		Options::Aim::bRCSEnabled = false;

		Options::ESP::bGlowEnabled = false;
		Options::ESP::bDrawEnabled = false;

		Options::Skins::Gloves::bEnabled = false;

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

		int nBone = Bone::BONE_NECK;

		bool bVisibleCheck = true;

		bool bReAim = false;
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

	namespace Skins
	{
		namespace Gloves
		{
			bool bEnabled = false;

			int nItemDefinitionIndex = 5030; // sport gloves
			int nPaintkit = -1;
			float flWear = 0.000001f;
		}
	}
	
	namespace Misc
	{
		bool bBhopEnabled = false;
		bool bShowRanks = true;
		bool bAutoAccept = false;
	}
}
