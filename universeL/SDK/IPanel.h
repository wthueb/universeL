#pragma once

#include "Definitions.h"

class IPanel
{
public:
	const char* GetName(unsigned int vguiPanel)
	{
		using oGetName = const char*(__thiscall*)(void*, unsigned int);
		return CallVFunction<oGetName>(this, 36)(this, vguiPanel);
	}
#ifdef GetClassName
#undef GetClassName
#endif
	const char* GetClassName(unsigned int vguiPanel)
	{
		using oGetClassName = const char*(__thiscall*)(void*, unsigned int);
		return CallVFunction<oGetClassName>(this, 37)(this, vguiPanel);
	}
};
