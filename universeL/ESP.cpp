#include "ESP.h"

#include "Options.h"
#include "Draw.h"

#include <iomanip>

// box outline struct
struct Box
{
	int x, y, w, h;
};

// function prototypes
void DrawPlayer(C_BasePlayer* player, player_info_t playerinfo);
void DrawPlantedBomb(C_PlantedC4* plantedbomb);
void DrawThrowable(C_BaseEntity* throwable, ClientClass* client);
void DrawEntity(C_BaseEntity* entity, const char* name, Color color);

Color GetPlayerColor(C_BasePlayer* player);
bool GetBox(C_BaseEntity* entity, Box &result);

void DrawBox(Box box, Color color);
void DrawSkeleton(C_BasePlayer* player, Color color);
void DrawPlayerInfo(player_info_t playerinfo, Box box);
void DrawHealth(C_BasePlayer* player, Box box, Color color);
void DrawHealthText(C_BasePlayer* player, Box box);

void ESP::Glow()
{
	if (!Options::ESP::bGlowEnabled || !Interfaces::Engine()->IsInGame())
		return;
	
	auto localplayer = static_cast<C_BasePlayer*>(Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer()));
	if (!localplayer)
		return;

	for (auto i = 0; i < Interfaces::GlowObjManager()->m_GlowObjectDefinitions.m_Size; ++i)
	{
		GlowObjectDefinition_t& glow_object = Interfaces::GlowObjManager()->m_GlowObjectDefinitions[i];
		if (glow_object.IsUnused() || !glow_object.m_pEntity)
			continue;
		
		auto clientclass = glow_object.m_pEntity->GetClientClass();

		// set color
		Color color;
		if (clientclass->m_ClassID == EClassIds::CCSPlayer)
		{
			auto player = static_cast<C_BasePlayer*>(glow_object.m_pEntity);

			if (player->IsDormant() || !player->IsAlive())
				continue;

			if (player->GetTeam() != localplayer->GetTeam())
				color = Color(Options::ESP::fEnemyColor);
			else
				color = Color(Options::ESP::fAllyColor);
		}
		// nade or bomb
		else if (clientclass->m_ClassID == EClassIds::CBaseCSGrenadeProjectile || clientclass->m_ClassID == EClassIds::CDecoyProjectile ||
			clientclass->m_ClassID == EClassIds::CMolotovProjectile || clientclass->m_ClassID == EClassIds::CSmokeGrenadeProjectile ||
			clientclass->m_ClassID == EClassIds::CPlantedC4)
		{
			color = Color(255, 255, 255, 255);
		}
		else
		{
			continue;
		}

		glow_object.m_vecGlowColor = Vector(color.r / 255.f, color.g / 255.f, color.b / 255.f);
		glow_object.m_flGlowAlpha = .85f;
		glow_object.m_flBloomAmount = 1.f;
		glow_object.m_bRenderWhenOccluded = true;
		glow_object.m_bRenderWhenUnoccluded = false;
	}
}

void ESP::Draw()
{
	if (!Options::ESP::bDrawEnabled || !Interfaces::Engine()->IsInGame())
		return;

	auto localplayer = static_cast<C_BasePlayer*>(Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer()));
	if (!localplayer)
		return;

	for (auto i = 0; i < Interfaces::EntityList()->GetHighestEntityIndex(); ++i)
	{
		// get the entity at the current index
		auto entity = static_cast<C_BaseEntity*>(Interfaces::EntityList()->GetClientEntity(i));
		if (!entity)
			continue;
		
		// get what type of entity it is
		auto clientclass = entity->GetClientClass();

		// if it is a player & player drawing is enabled
		if (clientclass->m_ClassID == EClassIds::CCSPlayer && Options::ESP::bDrawPlayers)
		{
			auto player = static_cast<C_BasePlayer*>(entity);

			if (player->IsDormant() || !player->IsAlive())
				continue;

			// player info. steamid, name, etc
			player_info_t playerInfo;
			if (Interfaces::Engine()->GetPlayerInfo(i, &playerInfo))
				DrawPlayer(player, playerInfo);
		}
		// if it is a planted bomb & bomb drawing is enabled
		else if (clientclass->m_ClassID == EClassIds::CPlantedC4 && Options::ESP::bDrawBomb)
		{
			auto plantedbomb = static_cast<C_PlantedC4*>(entity);
			DrawPlantedBomb(plantedbomb);
		}
		// if it is a projectile (nade) & nade drawing is enabled
		else if (strstr(clientclass->m_pNetworkName, "Projectile") && Options::ESP::bDrawNades)
		{
			DrawThrowable(entity, clientclass);
		}
	}
}

void ESP::ShowRanks(CUserCmd* pCmd)
{
	if (!Options::Misc::bShowRanks || !(pCmd->buttons & IN_SCORE))
		return;

	static int nArray[3] = { 0, 0, 0 };

	using MsgFunc_ServerRankRevealAllFn = bool(__cdecl*)(int*);
	static MsgFunc_ServerRankRevealAllFn MsgFunc_ServerRankRevealAll = reinterpret_cast<MsgFunc_ServerRankRevealAllFn>(Utils::FindSignature(XorStr("client.dll"), XorStr("55 8B EC 8B 0D ? ? ? ? 68")));

	MsgFunc_ServerRankRevealAll(nArray);
}

void DrawPlayer(C_BasePlayer* player, player_info_t playerinfo)
{
	auto localplayer = static_cast<C_BasePlayer*>(Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer()));
	if (!localplayer)
		return;

	// if they are ourself
	if (player == localplayer)
		return;

	// if they're on our team & enemies only is enabled
	if (Options::ESP::bOnlyEnemies && player->GetTeam() == localplayer->GetTeam())
		return;

	// get box to draw
	Box box;
	if (!GetBox(player, box))
		return;

	// get color of the player
	auto color = GetPlayerColor(player);

	if (Options::ESP::bDrawPlayerBox)
		DrawBox(box, color);

	if (Options::ESP::bDrawSkeleton)
		DrawSkeleton(player, color);

	if (Options::ESP::bShowName)
		DrawPlayerInfo(playerinfo, box);

	if (Options::ESP::bShowHealth)
		DrawHealth(player, box, color);

	if (Options::ESP::bShowHealthText)
		DrawHealthText(player, box);
}

void DrawPlantedBomb(C_PlantedC4* plantedbomb)
{
	// if bomb is defused/exploded, set color to green, else red
	auto color = plantedbomb->GetBombDefusing() != -1 || plantedbomb->IsBombDefused() ? Color(0, 255, 0, 255) : Color(255, 0, 0, 255);

	// get bomb time
	float bombTimer = plantedbomb->GetBombTime() - Interfaces::GlobalVars()->curtime;

	std::stringstream displaytext;
	// if it's defused/exploded
	if (plantedbomb->IsBombDefused() || !plantedbomb->IsBombTicking() || bombTimer <= 0.f)
	{
		displaytext << "bomb";
	}
	else
	{
		displaytext << "bomb: " << std::fixed << std::showpoint << std::setprecision(1) << bombTimer;
	}

	DrawEntity(plantedbomb, displaytext.str().c_str(), color);
}

void DrawThrowable(C_BaseEntity* throwable, ClientClass* client)
{
	model_t* model = throwable->GetModel();
	if (!model)
		return;

	studiohdr_t* hdr = Interfaces::ModelInfo()->GetStudioModel(model);
	if (!hdr)
		return;

	if (!strstr(hdr->name, "thrown") && !strstr(hdr->name, "dropped"))
		return;

	auto color = Color(255, 255, 255, 255);
	std::string nadeName = "unknown";

	IMaterial* mats[32];
	Interfaces::ModelInfo()->GetModelMaterials(model, hdr->numtextures, mats);

	for (auto i = 0; i < hdr->numtextures; ++i)
	{
		IMaterial* mat = mats[i];
		if (!mat)
			continue;

		if (strstr(mat->GetName(), "flashbang"))
		{
			nadeName = "flashbang";
			// yellow
			color = Color(255, 255, 0, 255);
			break;
		}
		else if (strstr(mat->GetName(), "m67_grenade") || strstr(mat->GetName(), "hegrenade"))
		{
			nadeName = "he grenade";
			// red
			color = Color(255, 0, 0, 255);
			break;
		}
		else if (strstr(mat->GetName(), "smoke"))
		{
			nadeName = "smoke";
			// blue
			color = Color(0, 0, 255, 255);
			break;
		}
		else if (strstr(mat->GetName(), "decoy"))
		{
			nadeName = "decoy";
			// white
			color = Color(255, 255, 255, 255);
			break;
		}
		else if (strstr(mat->GetName(), "incendiary") || strstr(mat->GetName(), "molotov"))
		{
			nadeName = "molotov";
			// orange
			color = Color(255, 165, 0, 255);
			break;
		}
	}

	DrawEntity(throwable, nadeName.c_str(), color);
}

void DrawEntity(C_BaseEntity* entity, const char* name, Color color)
{
	Box box;
	if (!GetBox(entity, box))
		return;

	DrawBox(box, color);

	Vector2D nameSize = Draw::GetTextSize(name, espfont);
	Draw::Text(static_cast<int>(box.x + box.w / 2 - nameSize.x / 2), box.y + box.h + 2, name, espfont, Color(255, 255, 255, 255));
}

Color GetPlayerColor(C_BasePlayer* player)
{
	auto localplayer = static_cast<C_BasePlayer*>(Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer()));
	
	auto color = player->GetTeam() == localplayer->GetTeam() ? Color(Options::ESP::fAllyColor) : Color(Options::ESP::fEnemyColor);

	return color;
}

bool GetBox(C_BaseEntity* entity, Box &result)
{
	Vector vOrigin, min, max, flb, brt, blb, frt, frb, brb, blt, flt;
	float left, top, right, bottom;

	vOrigin = entity->GetVecOrigin();
	min = entity->GetCollideable()->OBBMins() + vOrigin;
	max = entity->GetCollideable()->OBBMaxs() + vOrigin;

	Vector points[] = { Vector(min.x, min.y, min.z),
						Vector(min.x, max.y, min.z),
						Vector(max.x, max.y, min.z),
						Vector(max.x, min.y, min.z),
						Vector(max.x, max.y, max.z),
						Vector(min.x, max.y, max.z),
						Vector(min.x, min.y, max.z),
						Vector(max.x, min.y, max.z) };

	if (Interfaces::DebugOverlay()->ScreenPosition(points[3], flb) || Interfaces::DebugOverlay()->ScreenPosition(points[5], brt) ||
	    Interfaces::DebugOverlay()->ScreenPosition(points[0], blb) || Interfaces::DebugOverlay()->ScreenPosition(points[4], frt) ||
	    Interfaces::DebugOverlay()->ScreenPosition(points[2], frb) || Interfaces::DebugOverlay()->ScreenPosition(points[1], brb) ||
	    Interfaces::DebugOverlay()->ScreenPosition(points[6], blt) || Interfaces::DebugOverlay()->ScreenPosition(points[7], flt))
		return false;

	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	for (int i = 1; i < 8; i++)
	{
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}

	result.x = static_cast<int>(left);
	result.y = static_cast<int>(top);
	result.w = static_cast<int>(right - left);
	result.h = static_cast<int>(bottom - top);

	return true;
}

void DrawBox(Box box, Color color)
{
	if (Options::ESP::bCornerBoxes)
	{
		int VertLine = static_cast<int>(box.w * 0.33f);
		int HorzLine = static_cast<int>(box.h * 0.33f);
		int squareLine = min(VertLine, HorzLine);

		// top-left corner / color
		Draw::Rectangle(box.x, box.y, box.x + squareLine, box.y + 1, color);
		Draw::Rectangle(box.x, box.y, box.x + 1, box.y + squareLine, color);

		// top-left corner / outer outline
		Draw::Rectangle(box.x - 1, box.y - 1, box.x + squareLine, box.y, Color(0, 0, 0, 190));
		Draw::Rectangle(box.x - 1, box.y, box.x, box.y + squareLine, Color(0, 0, 0, 190));

		// top-left corner / inner outline
		Draw::Rectangle(box.x + 1, box.y + 1, box.x + squareLine, box.y + 2, Color(0, 0, 0, 190));
		Draw::Rectangle(box.x + 1, box.y + 2, box.x + 2, box.y + squareLine, Color(0, 0, 0, 190));

		// top-left corner / missing edges
		Draw::Rectangle(box.x + squareLine, box.y - 1, box.x + squareLine + 1, box.y + 2, Color(0, 0, 0, 190));
		Draw::Rectangle(box.x - 1, box.y + squareLine, box.x + 2, box.y + squareLine + 1, Color(0, 0, 0, 190));


		// top-right corner / color
		Draw::Rectangle(box.x + box.w - squareLine, box.y, box.x + box.w, box.y + 1, color);
		Draw::Rectangle(box.x + box.w - 1, box.y, box.x + box.w, box.y + squareLine, color);

		// top-right corner / outer outline
		Draw::Rectangle(box.x + box.w - squareLine, box.y - 1, box.x + box.w + 1, box.y, Color(0, 0, 0, 190));
		Draw::Rectangle(box.x + box.w, box.y, box.x + box.w + 1, box.y + squareLine, Color(0, 0, 0, 190));

		// top-right corner / inner outline
		Draw::Rectangle(box.x + box.w - squareLine, box.y + 1, box.x + box.w - 1, box.y + 2, Color(0, 0, 0, 190));
		Draw::Rectangle(box.x + box.w - 2, box.y + 2, box.x + box.w - 1, box.y + squareLine, Color(0, 0, 0, 190));

		// top-right corner / missing edges
		Draw::Rectangle(box.x + box.w - squareLine - 1, box.y - 1, box.x + box.w - squareLine, box.y + 2, Color(0, 0, 0, 190));
		Draw::Rectangle(box.x + box.w - 2, box.y + squareLine, box.x + box.w + 1, box.y + squareLine + 1, Color(0, 0, 0, 190));


		// bottom-left corner / color
		Draw::Rectangle(box.x, box.y + box.h - 1, box.x + squareLine, box.y + box.h, color);
		Draw::Rectangle(box.x, box.y + box.h - squareLine, box.x + 1, box.y + box.h, color);

		// bottom-left corner / outer outline
		Draw::Rectangle(box.x - 1, box.y + box.h, box.x + squareLine, box.y + box.h + 1, Color(0, 0, 0, 190));
		Draw::Rectangle(box.x - 1, box.y + box.h - squareLine, box.x, box.y + box.h, Color(0, 0, 0, 190));

		// bottom-left corner / inner outline
		Draw::Rectangle(box.x + 1, box.y + box.h - 2, box.x + squareLine, box.y + box.h - 1, Color(0, 0, 0, 190));
		Draw::Rectangle(box.x + 1, box.y + box.h - squareLine, box.x + 2, box.y + box.h - 2, Color(0, 0, 0, 190));

		// bottom-left corner / missing edges
		Draw::Rectangle(box.x + squareLine, box.y + box.h - 2, box.x + squareLine + 1, box.y + box.h + 1, Color(0, 0, 0, 190));
		Draw::Rectangle(box.x - 1, box.y + box.h - squareLine - 1, box.x + 2, box.y + box.h - squareLine, Color(0, 0, 0, 190));


		// bottom-right corner / color
		Draw::Rectangle(box.x + box.w - squareLine, box.y + box.h - 1, box.x + box.w, box.y + box.h, color);
		Draw::Rectangle(box.x + box.w - 1, box.y + box.h - squareLine, box.x + box.w, box.y + box.h, color);

		// bottom-right corner / outer outline
		Draw::Rectangle(box.x + box.w - squareLine, box.y + box.h, box.x + box.w + 1, box.y + box.h + 1, Color(0, 0, 0, 190));
		Draw::Rectangle(box.x + box.w, box.y + box.h - squareLine, box.x + box.w + 1, box.y + box.h + 1, Color(0, 0, 0, 190));

		// bottom-right corner / inner outline
		Draw::Rectangle(box.x + box.w - squareLine, box.y + box.h - 2, box.x + box.w - 1, box.y + box.h - 1, Color(0, 0, 0, 190));
		Draw::Rectangle(box.x + box.w - 2, box.y + box.h - squareLine, box.x + box.w - 1, box.y + box.h - 2, Color(0, 0, 0, 190));

		// bottom-right corner / missing edges
		Draw::Rectangle(box.x + box.w - squareLine, box.y + box.h - 2, box.x + box.w - squareLine + 1, box.y + box.h + 1, Color(0, 0, 0, 190));
		Draw::Rectangle(box.x + box.w - 2, box.y + box.h - squareLine - 1, box.x + box.w + 1, box.y + box.h - squareLine, Color(0, 0, 0, 190));
	}
	// regular 2d box
	else
	{
		// color
		Draw::Rectangle(box.x, box.y, box.x + box.w, box.y + box.h, color);
		// outer outline
		Draw::Rectangle(box.x + 1, box.y + 1, box.x + box.w - 1, box.y + box.h - 1, Color(0, 0, 0, 190));
		// inner outline
		Draw::Rectangle(box.x - 1, box.y - 1, box.x + box.w + 1, box.y + box.h + 1, Color(0, 0, 0, 190));
	}
	// 3d boxes
	/*else if (Settings::ESP::Boxes::type == BoxType::BOX_3D)
	{
		Vector vOrigin = entity->GetVecOrigin();
		Vector min = entity->GetCollideable()->OBBMins() + vOrigin;
		Vector max = entity->GetCollideable()->OBBMaxs() + vOrigin;

		Vector points[] = { Vector(min.x, min.y, min.z),
			Vector(min.x, max.y, min.z),
			Vector(max.x, max.y, min.z),
			Vector(max.x, min.y, min.z),
			Vector(min.x, min.y, max.z),
			Vector(min.x, max.y, max.z),
			Vector(max.x, max.y, max.z),
			Vector(max.x, min.y, max.z) };

		int edges[12][2] = { { 0, 1 },{ 1, 2 },{ 2, 3 },{ 3, 0 },
		{ 4, 5 },{ 5, 6 },{ 6, 7 },{ 7, 4 },
		{ 0, 4 },{ 1, 5 },{ 2, 6 },{ 3, 7 }, };

		for (auto it : edges)
		{
			Vector p1, p2;
			if (Interfaces::DebugOverlay()->ScreenPosition(points[it[0]], p1) || Interfaces::DebugOverlay()->ScreenPosition(points[it[1]], p2))
				return;
			Draw::Line(Vector2D(p1.x, p1.y), Vector2D(p2.x, p2.y), color);
		}
	}*/
}

void DrawSkeleton(C_BasePlayer* player, Color color)
{
	studiohdr_t* hdr = Interfaces::ModelInfo()->GetStudioModel(player->GetModel());
	if (!hdr)
		return;

	static matrix3x4_t bonetoworldout[128];
	if (player->SetupBones(bonetoworldout, 128, 256, 0))
	{
		for (int i = 0; i < hdr->numbones; i++)
		{
			mstudiobone_t* pBone = hdr->pBone(i);
			if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
				continue;

			Vector vBonePos1;
			if (Interfaces::DebugOverlay()->ScreenPosition(Vector(bonetoworldout[i][0][3], bonetoworldout[i][1][3], bonetoworldout[i][2][3]), vBonePos1))
				continue;

			Vector vBonePos2;
			if (Interfaces::DebugOverlay()->ScreenPosition(Vector(bonetoworldout[pBone->parent][0][3], bonetoworldout[pBone->parent][1][3], bonetoworldout[pBone->parent][2][3]), vBonePos2))
				continue;

			Draw::Line(Vector2D(vBonePos1.x, vBonePos1.y), Vector2D(vBonePos2.x, vBonePos2.y), color);
		}
	}
}

void DrawPlayerInfo(player_info_t playerinfo, Box box)
{
	// what line to draw on if you want multiple lines (name, rank, etc)
	int multiplier = 1;

	if (Options::ESP::bShowName)
	{
		Vector2D nameSize = Draw::GetTextSize(playerinfo.szName, espfont);
		Draw::Text(static_cast<int>(box.x + box.w / 2 - nameSize.x / 2), static_cast<int>(box.y - nameSize.y), playerinfo.szName, espfont, Color(255, 255, 255));

		multiplier++;
	}
}

void DrawHealth(C_BasePlayer* player, Box box, Color color)
{
	// clamp it
	int healthvalue = max(0, min(player->GetHealth(), 100));
	float healthpercent = healthvalue / 100.f;

	int barx = box.x;
	int bary = box.y;
	int barw = box.w;
	int barh = box.h;

	int boxSpacing = 3;
	Vector2D barsSpacing = Vector2D(0, 0);

	// right
	if (!Options::ESP::bShowHealthText)
	{
		barx += barw + boxSpacing; // spacing(1px) + outline(1px) + bar(2px) + outline (1px) = 8 px
		barw = 4; // outline(1px) + bar(2px) + outline(1px) = 6px;

		// white outline with black fill
		Draw::Rectangle(barx - 1, bary - 1, barx + barw + 1, bary + barh + 1, Color(255, 255, 255, 255));
		Draw::FilledRectangle(barx, bary, barx + barw, bary + barh, Color(0, 0, 0, 190));

		if (healthpercent > 0)
			Draw::FilledRectangle(barx + 1, bary + static_cast<int>(barh * (1.f - healthpercent)) + 1, barx + barw - 1, bary + barh - 1, color);

		barsSpacing.x += barw;
	}
	// bottom
	else
	{
		bary += barh + boxSpacing; // player box(?px) + spacing(1px) + outline(1px) + bar(2px) + outline (1px) = 5 px
		barh = 4; // outline(1px) + bar(2px) + outline(1px) = 4px;

		// white outline with black fill
		Draw::Rectangle(barx - 1, bary - 1, barx + barw + 1, bary + barh + 1, Color(255, 255, 255, 255));
		Draw::FilledRectangle(barx, bary, barx + barw, bary + barh, Color(0, 0, 0, 190));

		if (healthpercent > 0)
		{
			barw = static_cast<int>(barw * healthpercent);
			Draw::Rectangle(barx + 1, bary + 1, barx + barw - 1, bary + barh - 1, color);
		}
		barsSpacing.y += barh;
	}
}

void DrawHealthText(C_BasePlayer* player, Box box)
{
	std::string hptext = std::to_string(player->GetHealth()) + " hp";

	// draws at bottom right corner, offset to the right by 3 pixels
	static Vector2D textsize = Draw::GetTextSize("xd lmao", espfont);
	Draw::Text(box.x + box.w + 3, static_cast<int>(box.y + box.h - textsize.y), hptext.c_str(), espfont, Color(255, 255, 255));
}
