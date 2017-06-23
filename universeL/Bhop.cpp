#include "Bhop.h"

#include "Options.h"


void Bhop::CreateMove(CUserCmd* pCmd)
{
	if (!Options::Misc::bBhopEnabled || !Interfaces::Engine()->IsInGame())
		return;

	// get localplayer
	auto localplayer = (C_BasePlayer*)Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer());
	if (!localplayer)
		return;

	// if jump is down and you're in the air
	if ((pCmd->buttons & IN_JUMP) && !(localplayer->GetFlags() & static_cast<int>(EntityFlags::FL_ONGROUND)))
	{
		// release the jump button. whenever you're not in the air (on the ground), jump button gets pressed
		pCmd->buttons &= ~IN_JUMP;
	}
}
