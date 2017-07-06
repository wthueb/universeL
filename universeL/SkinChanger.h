#pragma once

#include "SDK\SDK.h"

namespace SkinChanger
{
	void FrameStageNotify(ClientFrameStage_t stage);

	typedef struct Item_s
	{
		constexpr Item_s(const char* szDisplayName, const char* szEntityName, const char* szModel, const char* szIcon = nullptr) :
			szDisplayName(szDisplayName),
			szEntityName(szEntityName),
			szModel(szModel),
			szIcon(szIcon)
		{}

		const char* szDisplayName = nullptr;
		const char* szEntityName = nullptr;
		const char* szModel = nullptr;
		const char* szIcon = nullptr;
	} Item_t;

	typedef struct WeaponName_s
	{
		constexpr WeaponName_s(int iDefinitionIndex, const char* szName) :
			iDefinitionIndex(iDefinitionIndex),
			szName(szName)
		{}

		int iDefinitionIndex = 0;
		const char* szName = nullptr;
	} WeaponName_t;

	typedef struct QualityName_s
	{
		constexpr QualityName_s(int iIndex, const char* szName) :
			iIndex(iIndex),
			szName(szName)
		{}

		int iIndex = 0;
		const char* szName = nullptr;
	} QualityName_t;

	extern const std::vector<WeaponName_t> glovenames;
	extern const std::vector<QualityName_t> qualities;
}
