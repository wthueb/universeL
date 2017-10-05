#pragma once

#include "ImGui\imgui.h"

#include "Hooks.h"
#include "Options.h"

#include "XorStr.h"

#include "SkinChanger.h"

void DrawInterface()
{
	ImGui::Begin(XorStr(Hooks::name), &Options::bMainWindowOpen, ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar);

	if (ImGui::CollapsingHeader(XorStr("aim")))
	{
		ImGui::Checkbox(XorStr("aimbot enabled"), &Options::Aim::bAimbotEnabled);

		ImGui::Checkbox(XorStr("rcs enabled"), &Options::Aim::bRCSEnabled);

		ImGui::Separator();

		ImGui::InputFloat(XorStr("fov"), &Options::Aim::flFov, 0.f, 0.f, 1);
		std::clamp(Options::Aim::flFov, 0.1f, 180.f);

		ImGui::Checkbox(XorStr("smooth"), &Options::Aim::bSmooth);
		ImGui::SliderFloat(XorStr("smooth amount"), &Options::Aim::flSmooth, 1.f, 10.f, "%.1f");

		//ImGui::Checkbox(XorStr("silent"), &Options::Aim::bSilent);

		ImGui::Separator();

		ImGui::Checkbox(XorStr("on shoot"), &Options::Aim::bOnShoot);
		
		{
			static int aimkeyselection = 0;

			ImGui::Combo(XorStr("aimkey"), &aimkeyselection, [](void* data, int idx, const char** out_text)
			{
				*out_text = Options::keys.at(idx).name;
				return true;
			}, nullptr, Options::keys.size(), Options::keys.size());

			Options::Aim::nAimkey = Options::keys.at(aimkeyselection).key;
		}

		ImGui::Separator();

		{
			static int boneselection = 1;

			ImGui::Combo(XorStr("bone"), &boneselection, [](void* data, int idx, const char** out_text)
			{
				*out_text = Options::bones.at(idx).name;
				return true;
			}, nullptr, Options::bones.size(), Options::bones.size());

			Options::Aim::nBone = Options::bones.at(boneselection).bone;
		}

		ImGui::Separator();

		ImGui::Checkbox(XorStr("visible check"), &Options::Aim::bVisibleCheck);

		ImGui::Separator();

		ImGui::Checkbox(XorStr("re-aim"), &Options::Aim::bReAim);

		ImGui::Checkbox(XorStr("lock on teammates"), &Options::Aim::bLockOnFriendly);
	}

	if (ImGui::CollapsingHeader(XorStr("visuals")))
	{
		ImGui::Checkbox(XorStr("glow enabled"), &Options::ESP::bGlowEnabled);

		ImGui::Separator();

		ImGui::Checkbox(XorStr("esp enabled"), &Options::ESP::bDrawEnabled);

		ImGui::Checkbox(XorStr("corner boxes"), &Options::ESP::bCornerBoxes);

		ImGui::Separator();

		ImGui::Checkbox(XorStr("draw players"), &Options::ESP::bDrawPlayers);
		ImGui::Checkbox(XorStr("draw player box"), &Options::ESP::bDrawPlayerBox);
		ImGui::Checkbox(XorStr("draw skeleton"), &Options::ESP::bDrawSkeleton);

		ImGui::Checkbox(XorStr("only enemies"), &Options::ESP::bOnlyEnemies);

		ImGui::Separator();

		ImGui::Checkbox(XorStr("show name"), &Options::ESP::bShowName);

		ImGui::Checkbox(XorStr("show health"), &Options::ESP::bShowHealth);
		ImGui::Checkbox(XorStr("show health text"), &Options::ESP::bShowHealthText);

		ImGui::Separator();

		ImGui::Checkbox(XorStr("draw bomb"), &Options::ESP::bDrawBomb);

		ImGui::Checkbox(XorStr("draw nades"), &Options::ESP::bDrawNades);

		ImGui::Separator();

		ImGui::ColorEdit3(XorStr("ally color"), Options::ESP::fAllyColor);
		ImGui::ColorEdit3(XorStr("enemy color"), Options::ESP::fEnemyColor);
	}

	if (ImGui::CollapsingHeader(XorStr("skin changer")))
	{
		ImGui::Checkbox(XorStr("gloves enabled"), &Options::Skins::Gloves::bEnabled);

		{
			static int gloveselection = -1;

			ImGui::Combo(XorStr("glove"), &gloveselection, [](void* data, int idx, const char** out_text)
			{
				*out_text = SkinChanger::glovenames[idx].szName;
				return true;
			}, nullptr, SkinChanger::glovenames.size(), SkinChanger::glovenames.size());

			Options::Skins::Gloves::nItemDefinitionIndex = gloveselection == -1 ? -1 : SkinChanger::glovenames.at(gloveselection).iDefinitionIndex;
		}

		{
			static int paintkitselection = -1;

			ImGui::Combo(XorStr("glove paint kit"), &paintkitselection, [](void* data, int idx, const char** out_text)
			{
				*out_text = gloveskins[idx].name.c_str();
				return true;
			}, nullptr, gloveskins.size(), 10);

			Options::Skins::Gloves::nPaintkit = paintkitselection == -1 ? -1 : gloveskins.at(paintkitselection).id;
		}

		ImGui::InputFloat(XorStr("glove float"), &Options::Skins::Gloves::flWear, 0.f, 0.f, 6);
		std::clamp(Options::Skins::Gloves::flWear, 0.000001f, 0.999999f);
	}

	if (ImGui::CollapsingHeader(XorStr("misc")))
	{
		{
			static char buf[64];

			ImGui::InputText(XorStr("clantag"), buf, sizeof(buf));

			Options::Misc::strClantag = buf;
		}
		
		ImGui::Checkbox(XorStr("scrolling clantag"), &Options::Misc::bClantagScroll);

		ImGui::Separator();

		ImGui::Checkbox(XorStr("name changer"), &Options::Misc::bNameChanger);

		{
			static char buf[64];

			if (ImGui::InputText(XorStr("name"), buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue))
				Options::Misc::strName = buf;
		}

		ImGui::Separator();

		ImGui::Checkbox(XorStr("bhop enabled"), &Options::Misc::bBhopEnabled);

		ImGui::Separator();

		ImGui::Checkbox(XorStr("show ranks"), &Options::Misc::bShowRanks);

		ImGui::Separator();

		ImGui::Checkbox(XorStr("auto accept"), &Options::Misc::bAutoAccept);
	}

	ImGui::End();
}
