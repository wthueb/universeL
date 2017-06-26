#include "Aimbot.h"

#include "Options.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define RAD2DEG(x) (static_cast<float>(x) * static_cast<float>(180.f / M_PI))
#define DEG2RAD(x) (static_cast<float>(x) * static_cast<float>(M_PI / 180.f))

void FindTarget();
void DropTarget();
void CorrectAim();
void RCS();

bool IsVisible(C_BasePlayer* player);
bool IsValidPlayer(C_BasePlayer* player);
void ClampAngle(QAngle& angle);
float GetFov(const QAngle& viewAngle, const QAngle& aimAngle);
void VectorAngles(const Vector& forward, QAngle &angles);
void AngleVectors(const QAngle& angles, Vector* forward);

C_BasePlayer* localplayer = nullptr;
C_BaseCombatWeapon* activeweapon = nullptr;
CUserCmd* cmd = nullptr;
QAngle oldangle;
int besttarget = -1;

void Aimbot::CreateMove(CUserCmd* pCmd)
{
	if (!Interfaces::Engine()->IsInGame() || !Options::Aim::bAimbotEnabled && !Options::Aim::bRCSEnabled)
		return;
	
	DropTarget();

	localplayer = static_cast<C_BasePlayer*>(Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer()));
	if (!localplayer)
		return;

	activeweapon = static_cast<C_BaseCombatWeapon*>(Interfaces::EntityList()->GetClientEntityFromHandle(localplayer->GetActiveWeaponHandle()));
	if (!activeweapon)
		return;

	cmd = pCmd;
	
	if (activeweapon->IsKnife() || activeweapon->IsNade() || activeweapon->IsBomb())
		return;

	bool shouldcorrect = false;

	if (Options::Aim::bAimbotEnabled)
	{
		static bool found = false;

		if ((Options::Aim::bOnShoot && cmd->buttons & IN_ATTACK) ||
			(Options::Aim::bOnAimkey && Interfaces::InputSystem()->IsButtonDown(static_cast<ButtonCode_t>(Options::Aim::nAimkey))))
		{
			if (!found)
			{
				FindTarget();
				found = true;
			}

			shouldcorrect = true;
		}
		else
			found = false;

		if (shouldcorrect)
			CorrectAim();
	}

	if (Options::Aim::bRCSEnabled && !shouldcorrect &&
		!activeweapon->IsPistol() && !activeweapon->IsShotgun() &&
		!activeweapon->IsSniper())
		RCS();

	if (!Options::Aim::bSilent)
		Interfaces::Engine()->SetViewAngles(cmd->viewangles);
}

void RCS()
{
	if (cmd->buttons & IN_ATTACK)
	{
		QAngle aimpunch = *localplayer->GetAimPunch() * 2.f;

		QAngle dst = cmd->viewangles + oldangle - aimpunch;

		ClampAngle(dst);

		cmd->viewangles = dst;

		oldangle = aimpunch;
	}
	else
	{
		oldangle.x = oldangle.y = oldangle.z = 0;
	}
}

void FindTarget()
{
	auto bestfov = Options::Aim::flFov;

	for (auto i = 1; i <= Interfaces::GlobalVars()->maxClients; ++i)
	{
		auto potential = static_cast<C_BasePlayer*>(Interfaces::EntityList()->GetClientEntity(i));

		if (!IsValidPlayer(potential))
			continue;

		if (!Options::Aim::bLockOnFriendly &&
			potential->GetTeam() == localplayer->GetTeam())
			continue;

		if (Options::Aim::bVisibleCheck &&
			!IsVisible(potential))
			continue;

		Vector potentialpos = potential->GetBonePosition(Options::Aim::nBone);
		Vector eyepos = localplayer->GetEyePosition();
		Vector relative = eyepos - potentialpos;

		QAngle angle;
		VectorAngles(relative, angle);

		auto fov = GetFov(cmd->viewangles, angle);

		if (fov <= bestfov)
		{
			bestfov = fov;
			besttarget = i;
		}
	}
}

void DropTarget()
{
	if (besttarget == -1)
		return;

	auto target = static_cast<C_BasePlayer*>(Interfaces::EntityList()->GetClientEntity(besttarget));

	if (!IsValidPlayer(target))
		besttarget = -1;

	if (!Options::Aim::bLockOnFriendly &&
		target->GetTeam() == localplayer->GetTeam())
		besttarget = -1;
}

void CorrectAim()
{
	if (besttarget == -1)
		return;

	auto target = static_cast<C_BasePlayer*>(Interfaces::EntityList()->GetClientEntity(besttarget));

	if (!IsValidPlayer(target))
		return;
	
	if (Options::Aim::bVisibleCheck &&
		!IsVisible(target))
		return;

	Vector eyepos = localplayer->GetEyePosition();
	Vector targetpos = target->GetBonePosition(Options::Aim::nBone);
	Vector relative = eyepos - targetpos;

	QAngle dst;
	VectorAngles(relative, dst);

	if (GetFov(cmd->viewangles, dst) > Options::Aim::flFov)
		return;
	
	if (!activeweapon->IsSniper() && !activeweapon->IsPistol() && !activeweapon->IsShotgun())
		dst -= *localplayer->GetAimPunch() * 2.f;

	ClampAngle(dst);
	
	if (Options::Aim::bSmooth)
	{
		QAngle delta = dst - cmd->viewangles;
		ClampAngle(delta);
		delta /= max(Options::Aim::flSmooth, 1.f);
		dst = cmd->viewangles + delta;
	}

	ClampAngle(dst);

	if (dst != cmd->viewangles)
		cmd->viewangles = dst;
}

bool IsVisible(C_BasePlayer* player)
{
	Vector start = localplayer->GetEyePosition();
	Vector end = player->GetBonePosition(Options::Aim::nBone);

	Ray_t ray;
	ray.Init(start, end);

	CTraceFilter filter;
	filter.pSkip = localplayer;

	trace_t tr;
	Interfaces::EngineTrace()->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &tr);

	return tr.m_pEnt == player || tr.fraction > .99f;
}

bool IsValidPlayer(C_BasePlayer* player)
{
	if (!player)
		return false;

	if (player == localplayer)
		return false;

	if (!player->IsAlive())
		return false;

	if (player->GetImmune())
		return false;

	if (player->IsDormant())
		return false;

	return true;
}

void ClampAngle(QAngle& angle)
{
	for (auto i = 0; i < 3; ++i)
		if (!std::isfinite(angle[i])) // if it is infinite or NaN
			angle[i] = 0.f;

	// pitch
	angle.x = std::clamp(std::remainder(angle.x, 180.f), -89.f, 89.f);

	// yaw
	angle.y = std::clamp(std::remainder(angle.y, 360.f), -180.f, 180.f);

	angle.z = 0;
}

float GetFov(const QAngle& viewAngle, const QAngle& aimAngle)
{
	Vector ang, aim;

	AngleVectors(viewAngle, &aim);
	AngleVectors(aimAngle, &ang);

	return RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
}

void VectorAngles(const Vector& forward, QAngle &angles)
{
	if (forward[1] == 0.0f && forward[0] == 0.0f)
	{
		angles[0] = (forward[2] > 0.0f) ? 270.0f : 90.0f; // pitch
		angles[1] = 0.0f; // yaw
	}
	else
	{
		// pitch
		angles[0] = static_cast<vec_t>(atan2(-forward[2], forward.Length2D()) * -180 / M_PI);
		// yaw
		angles[1] = static_cast<vec_t>(atan2(forward[1], forward[0]) * 180 / M_PI);

		if (angles[1] > 90)
			angles[1] -= 180;
		else if (angles[1] < 90)
			angles[1] += 180;
		else if (angles[1] == 90)
			angles[1] = 0;
	}

	angles[2] = 0.0f;
}

void inline SinCos(float radians, float* sine, float* cosine)
{
	*sine = sin(radians);
	*cosine = cos(radians);
}

void AngleVectors(const QAngle& angles, Vector* forward)
{
	float sp, sy, cp, cy;

	SinCos(DEG2RAD(angles[0]), &sp, &cp);
	SinCos(DEG2RAD(angles[1]), &sy, &cy);

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}
