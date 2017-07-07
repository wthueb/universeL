#include "SkinChanger.h"

#include "Options.h"
#include "PaintKitParser.h"

#include <map>
#include <vector>

const std::map<size_t, SkinChanger::Item_t> gloveinfo =
{
	{ 5027, { "Bloodhound Gloves", "studded_bloodhound_gloves", "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl" } },
	{ 5028, { "T Gloves", "t_gloves", "models/weapons/v_models/arms/glove_fingerless/v_glove_fingerless.mdl" } },
	{ 5029, { "CT Gloves", "ct_gloves", "models/weapons/v_models/arms/glove_hardknuckle/v_glove_hardknuckle.mdl" } },
	{ 5030, { "Sport Gloves", "sporty_gloves", "models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl" } },
	{ 5031, { "Driver Gloves", "slick_gloves", "models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl" } },
	{ 5032, { "Hand Wraps", "leather_handwraps", "models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl" } },
	{ 5033, { "Moto Gloves", "motorcycle_gloves", "models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl" } },
	{ 5034, { "Specialist Gloves", "specialist_gloves", "models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl" } }
};

const std::vector<SkinChanger::WeaponName_t> SkinChanger::glovenames =
{
	{ 5027, "Bloodhound Gloves" },
	{ 5028, "T Gloves" },
	{ 5029, "CT Gloves" },
	{ 5030, "Sport Gloves" },
	{ 5031, "Driver Gloves" },
	{ 5032, "Hand Wraps" },
	{ 5033, "Moto Gloves" },
	{ 5034, "Specialist Gloves" }
};

const std::vector<SkinChanger::QualityName_t> SkinChanger::qualities =
{
	{ 0, "Default" },
	{ 1, "Genuine" },
	{ 2, "Vintage" },
	{ 3, "Unusual" },
	{ 5, "Community" },
	{ 6, "Developer" },
	{ 7, "Self-Made" },
	{ 8, "Customized" },
	{ 9, "Strange" },
	{ 10, "Completed" },
	{ 12, "Tournament" }
};

inline int RandomInt(int low, int high)
{
	return rand() % (high - low + 1) + low;
}

void SkinChanger::FrameStageNotify(ClientFrameStage_t stage)
{
	if (stage != FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		return;

	if (!Interfaces::Engine()->IsInGame() || !Options::Skins::Gloves::bEnabled)
		return;

	static bool glovesupdated = false;
	static bool shouldfullupdate = false;

	auto localplayer = static_cast<C_BasePlayer*>(Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer()));
	if (!localplayer)
		return;

	if (!localplayer->IsAlive())
	{
		auto glove = static_cast<C_BaseAttributableItem*>(Interfaces::EntityList()->GetClientEntity(localplayer->GetWearables()[0] & 0xFFF));
		if (!glove)
			return;

		glove->GetNetworkable()->SetDestroyedOnRecreateEntities();
		glove->GetNetworkable()->Release();

		return;
	}

	player_info_t playerinfo;
	if (!Interfaces::Engine()->GetPlayerInfo(Interfaces::Engine()->GetLocalPlayer(), &playerinfo))
		return;

	auto glove = static_cast<C_BaseAttributableItem*>(Interfaces::EntityList()->GetClientEntity(localplayer->GetWearables()[0] & 0xFFF));

	if (!glove)
	{
		for (ClientClass* pClass = Interfaces::Client()->GetAllClasses(); pClass; pClass = pClass->m_pNext)
		{
			if (pClass->m_ClassID != EClassIds::CEconWearable)
				continue;

			int entry = (Interfaces::EntityList()->GetHighestEntityIndex() + 1);
			int serial = RandomInt(0x0, 0xFFF);

			pClass->m_pCreateFn(entry, serial);
			localplayer->GetWearables()[0] = entry | (serial << 16);

			glovesupdated = true;
			shouldfullupdate = true;

			glove = static_cast<C_BaseAttributableItem*>(Interfaces::EntityList()->GetClientEntity(localplayer->GetWearables()[0] & 0xFFF));

			if (*glove->GetItemDefinitionIndex() != Options::Skins::Gloves::nItemDefinitionIndex)
			{	
				if (gloveinfo.find(Options::Skins::Gloves::nItemDefinitionIndex) != gloveinfo.end())
					glove->SetModelIndex(Interfaces::ModelInfo()->GetModelIndex(gloveinfo.at(Options::Skins::Gloves::nItemDefinitionIndex).szModel));
				else
					return;

				*glove->GetItemDefinitionIndex() = static_cast<EItemDefinitionIndex>(Options::Skins::Gloves::nItemDefinitionIndex);
			}

			break;
		}
	}

	if (*glove->GetItemDefinitionIndex() != Options::Skins::Gloves::nItemDefinitionIndex ||
		*glove->GetFallbackPaintKit() != Options::Skins::Gloves::nPaintkit ||
		*glove->GetFallbackWear() != Options::Skins::Gloves::flWear)
	{
		*glove->GetItemDefinitionIndex() = static_cast<EItemDefinitionIndex>(Options::Skins::Gloves::nItemDefinitionIndex);
		*glove->GetFallbackPaintKit() = Options::Skins::Gloves::nPaintkit;
		*glove->GetFallbackWear() = Options::Skins::Gloves::flWear;

		shouldfullupdate = true;
	}
	
	*glove->GetFallbackSeed() = 0;
	*glove->GetFallbackStatTrak() = -1;
	*glove->GetEntityQuality() = 4;

	*glove->GetItemIDHigh() = -1; // use fallback values
	*glove->GetAccountID() = playerinfo.xuidlow;

	if (glovesupdated)
	{
		glove->GetNetworkable()->PreDataUpdate(DATA_UPDATE_CREATED);
		glovesupdated = false;
	}

	if (shouldfullupdate)
	{
		Interfaces::ClientState()->ForceFullUpdate();
		shouldfullupdate = false;
	}
}

/*void SkinChanger::FireEventClientSide(IGameEvent* event)
{
	if (!Options::Skins::bKnivesEnabled)
		return;

	if (!Interfaces::Engine()->IsInGame())
		return;

	if (!event || strcmp(event->GetName(), "player_death"))
		return;

	if (!event->GetInt("attacker") ||
		Interfaces::Engine()->GetPlayerForUserID(event->GetInt("attacker")) != Interfaces::Engine()->GetLocalPlayer())
		return;

	std::string weapon = event->GetString("weapon");

	event->SetString("weapon", killIcons.find(weapon) != killIcons.end() ? killIcons.at(weapon).c_str() : weapon.c_str());
}*/
