#pragma once

#include "Definitions.h"

class Vector;

class IVDebugOverlay
{
public:
	bool ScreenPosition(const Vector& point, Vector& screen)
	{
		using oScreenPosition = bool(__thiscall*)(void*, const Vector&, Vector&);
		return CallVFunction<oScreenPosition>(this, 13)(this, point, screen);
	}

	bool ScreenPosition(float flXPos, float flYPos, Vector& screen)
	{
		using oScreenPosition = bool(__thiscall*)(void*, float, float, Vector&);
		return CallVFunction<oScreenPosition>(this, 14)(this, flXPos, flYPos, screen);
	}
};
