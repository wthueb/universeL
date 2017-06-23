#pragma once

#include "Definitions.h"

#include "Vector.h"


class IVDebugOverlay
{
public:
	bool ScreenPosition(const Vector& point, Vector& screen)
	{
		typedef bool(__thiscall* OriginalFn)(void*, const Vector&, Vector&);
		return CallVFunction<OriginalFn>(this, 13)(this, point, screen);
	}

	bool ScreenPosition(float flXPos, float flYPos, Vector& screen)
	{
		typedef bool(__thiscall* OriginalFn)(void*, float, float, Vector&);
		return CallVFunction<OriginalFn>(this, 14)(this, flXPos, flYPos, screen);
	}
};
