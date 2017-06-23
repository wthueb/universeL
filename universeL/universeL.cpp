#include "Hooks.h"
#include "Options.h"

void Unload(HMODULE hModule)
{
	Options::Unload();
	// wait for all loops to finish
	Sleep(100);

	// unhook
	Hooks::Restore();

	// sleep to be sure that threads are not running
	// this means that every thread can't sleep for more then 5 seconds
	Sleep(5000);

	// unload
	FreeLibraryAndExitThread(hModule, 0);

	// exit this thread
	ExitThread(0);
}

void WaitForUnload(HMODULE hModule)
{
	while (true)
	{
		if (Interfaces::InputSystem()->IsButtonDown(ButtonCode_t::KEY_DELETE))
		{
			Unload(hModule);
		}

		Sleep(1000);
	}
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Hooks::Init), hModule, 0, 0);
		CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(WaitForUnload), hModule, 0, 0);
		return TRUE;
		break;

	case DLL_PROCESS_DETACH:
		return TRUE;
		break;
	}

	return FALSE;
}
