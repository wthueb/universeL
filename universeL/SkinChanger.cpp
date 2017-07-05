#include "SkinChanger.h"

bool glovesupdated;
bool shouldfullupdate;

inline int RandomInt(int low, int high)
{
	return rand() % (high - low + 1) + low;
}

void SkinChanger::FrameStageNotify(ClientFrameStage_t stage)
{
	if (stage != FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		return;

	auto localplayer = static_cast<C_BasePlayer*>(Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer()));
	if (!localplayer)
		return;

	player_info_t playerinfo;
	if (!Interfaces::Engine()->GetPlayerInfo(Interfaces::Engine()->GetLocalPlayer(), &playerinfo))
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

	auto glove = static_cast<C_BaseAttributableItem*>(Interfaces::EntityList()->GetClientEntity(localplayer->GetWearables()[0] & 0xFFF));
	if (!glove)
	{
		for (ClientClass *pClass = Interfaces::Client()->GetAllClasses(); pClass; pClass = pClass->m_pNext)
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

			break;
		}
	}

	if (*glove->GetItemDefinitionIndex() != EItemDefinitionIndex::glove_sporty)
	{
		glove->SetModelIndex(Interfaces::ModelInfo()->GetModelIndex("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
		*glove->GetItemDefinitionIndex() = EItemDefinitionIndex::glove_sporty;
	}

	*glove->GetFallbackPaintKit() = 10018;
	*glove->GetFallbackWear() = 0.000002f;

	*glove->GetFallbackSeed() = 0;
	*glove->GetFallbackStatTrak() = -1;
	*glove->GetEntityQuality() = 4;
	*glove->GetItemIDHigh() = -1;
	*glove->GetAccountID() = playerinfo.xuidlow;

	if (glovesupdated)
	{
		glove->GetNetworkable()->PreDataUpdate(DATA_UPDATE_CREATED);
		glovesupdated = false;
	}

	if (shouldfullupdate)
	{
		// FIXMEW: set m_nDeltaTick to ForceFullUpdate
		Interfaces::Engine()->ClientCmd_Unrestricted("record x;stop", false);
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
