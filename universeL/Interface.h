#pragma once

#include "ImGui\imgui.h"

#include "Hooks.h"
#include "Options.h"

#include "XorStr.h"

void DrawInterface()
{
	ImGui::Begin(XorStr(Hooks::name), &Options::bMainWindowOpen, ImVec2(300, 350), 1.f);

	if (ImGui::CollapsingHeader(XorStr("aim")))
	{
		ImGui::Checkbox(XorStr("aimbot enabled"), &Options::Aim::bAimbotEnabled);
		ImGui::Checkbox(XorStr("rcs enabled"), &Options::Aim::bRCSEnabled);
		ImGui::Separator();
		ImGui::InputFloat(XorStr("fov"), &Options::Aim::flFov, 0.f, 0.f, 1);
		Options::Aim::flFov = min(max(0.f, Options::Aim::flFov), 180.f);
		ImGui::Checkbox(XorStr("smooth"), &Options::Aim::bSmooth);
		ImGui::SliderFloat(XorStr("smooth amount"), &Options::Aim::flSmooth, 1.f, 10.f, "%.1f");
		//ImGui::Checkbox(XorStr("silent"), &Options::Aim::bSilent);
		ImGui::Separator();
		ImGui::Checkbox(XorStr("on shoot"), &Options::Aim::bOnShoot);
		ImGui::Checkbox(XorStr("on aimkey"), &Options::Aim::bOnAimkey);

		static int aimkey_selection = 0;

		ImGui::Combo(XorStr("aimkey"), &aimkey_selection, [](void* data, int idx, const char** out_text)
		{
			*out_text = Options::keys.at(idx).name;
			return true;
		}, nullptr, Options::keys.size());

		Options::Aim::nAimkey = Options::keys.at(aimkey_selection).key;

		ImGui::Separator();

		static int bone_selection = 1;

		ImGui::Combo(XorStr("bone"), &bone_selection, [](void* data, int idx, const char** out_text)
		{
			*out_text = Options::bones.at(idx).name;
			return true;
		}, nullptr, Options::bones.size());

		Options::Aim::nBone = Options::bones.at(bone_selection).bone;

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

	if (ImGui::CollapsingHeader(XorStr("misc")))
	{
		ImGui::Checkbox(XorStr("bhop enabled"), &Options::Misc::bBhopEnabled);
		ImGui::Separator();
		ImGui::Checkbox(XorStr("show ranks"), &Options::Misc::bShowRanks);
		ImGui::Separator();
		ImGui::Checkbox(XorStr("auto accept"), &Options::Misc::bAutoAccept);
	}

	ImGui::End();
}
