#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "SDK\SDK.h"
#include "VFTableHook.h"

#ifdef PlaySound
#undef PlaySound
#endif

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

namespace Hooks
{
	void Init();
	void Restore();
	
	void GUI_Init(IDirect3DDevice9* pDevice);

	// hook prototypes
	typedef long(__stdcall* EndSceneFn)(IDirect3DDevice9*);
	typedef long(__stdcall* ResetFn)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	typedef bool(__thiscall* CreateMoveFn)(void*, float, CUserCmd*);
	typedef void(__thiscall* PaintTraverseFn)(void*, VPANEL, bool, bool);
	typedef void(__thiscall* PlaySoundFn)(void*, const char*);

	// hooked functions
	HRESULT   __stdcall  hkEndScene(IDirect3DDevice9* pDevice);
	HRESULT   __stdcall  hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	bool      __stdcall  hkCreateMove(float sample_input_frametime, CUserCmd* cmd);
	void      __fastcall hkPaintTraverse(void* thisptr, void*, VPANEL vgui_panel, bool force_repaint, bool allow_force);
	void      __stdcall  hkPlaySound(const char* szFileName);
	
	// have to hook WndProc to get user input to pass to the gui
	LRESULT   __stdcall hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// handle to csgo window
	extern HWND hWindow;
}
