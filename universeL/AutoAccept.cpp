#include "AutoAccept.h"

#include "SDK\SDK.h"
#include "Hooks.h"
#include "Options.h"
#include "Utils.h"


void AutoAccept::PlaySound(const char* szFileName)
{
	if (!Options::Misc::bAutoAccept || Interfaces::Engine()->IsInGame())
		return;

	// if the sound is competitive_accept_beep.wav
	if (!strcmp(szFileName, "UI/competitive_accept_beep.wav"))
	{
		using IsReadyCallbackFn = void(__cdecl*)();

		// func that is called when you press accept
		static IsReadyCallbackFn IsReadyCallback = reinterpret_cast<IsReadyCallbackFn>(Utils::FindSignature(XorStr("client.dll"), XorStr("55 8B EC 83 E4 F8 83 EC 08 56 8B 35 ? ? ? ? 57 8B 8E")));

		IsReadyCallback();

		// flash csgo on taskbar
		FLASHWINFO fi;
		fi.cbSize = sizeof(FLASHWINFO);
		fi.hwnd = Hooks::hWindow;
		fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
		fi.uCount = 0;
		fi.dwTimeout = 0;
		FlashWindowEx(&fi);
	}
}
