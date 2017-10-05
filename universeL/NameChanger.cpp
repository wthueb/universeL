#include "NameChanger.h"

#include "Options.h"

#include <string>

#include "SDK\SDK.h"

void NameChanger::SetName()
{
	if (!Interfaces::Engine()->IsInGame() || !Options::Misc::bNameChanger)
		return;

	static auto convar = Interfaces::CVar()->FindVar("name");
	convar->m_fnChangeCallbacks.m_Size = 0;

	static auto do_once = (convar->SetValue("\n"), true);

	convar->SetValue(Options::Misc::strName.c_str());
}
