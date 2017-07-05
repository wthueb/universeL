#include "Hooks.h"

#include "Hacks.h"
#include "Draw.h"
#include "Interface.h"

#include "DrawManager.h"
#include "NetvarManager.h"
#include "Utils.h"

#include "ImGui\imgui_impl_dx9.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 13
#define VERSION_PATCH 0

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Hooks
{
	std::unique_ptr<VFTableHook>        pD3DDevice9Hook = nullptr;
	std::unique_ptr<VFTableHook>        pBaseClientHook = nullptr;
	std::unique_ptr<VFTableHook>        pGameEventManagerHook = nullptr;
	std::unique_ptr<VFTableHook>        pClientModeHook = nullptr;
	std::unique_ptr<VFTableHook>        pVGUIPanelHook = nullptr; 
	std::unique_ptr<VFTableHook>        pMatSurfaceHook = nullptr;

	std::unique_ptr<DrawManager>        pRenderer = nullptr;

	EndSceneFn                          oEndScene = nullptr;
	ResetFn                             oReset = nullptr;
	CreateMoveFn                        oCreateMove = nullptr;
	FrameStageNotifyFn                  oFrameStageNotify = nullptr;
	FireEventClientSideFn               oFireEventClientSide = nullptr;
	PaintTraverseFn                     oPaintTraverse = nullptr;
	PlaySoundFn                         oPlaySound = nullptr;

	WNDPROC                             oWndProc = nullptr;
	HWND                                hWindow = nullptr;

	bool                                vecPressedKeys[256] = {};
	bool                                bWasInitialized = false;

	char                                name[32];

	void Init()
	{
		//AllocConsole();
		AttachConsole(GetCurrentProcessId());
		FILE* newstdout;
		freopen_s(&newstdout, "CON", "w", stdout);
		SetConsoleTitleA(XorStr("universeL debug"));

		sprintf_s(name, sizeof(name), "universeL v%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

		NetvarManager::Instance()->CreateDatabase();

#ifdef _DEBUG
		NetvarManager::Instance()->Dump(XorStr("netvars.txt"));
#endif

		uint32_t dwDevice = **reinterpret_cast<uint32_t**>(Utils::FindSignature(XorStr("shaderapidx9.dll"), XorStr("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 1);

		pD3DDevice9Hook = std::make_unique<VFTableHook>(reinterpret_cast<DWORD**>(dwDevice), true);
		pBaseClientHook = std::make_unique<VFTableHook>(reinterpret_cast<DWORD**>(Interfaces::Client()), true);
		pGameEventManagerHook = std::make_unique<VFTableHook>(reinterpret_cast<DWORD**>(Interfaces::GameEventManager()), true);
		pClientModeHook = std::make_unique<VFTableHook>(reinterpret_cast<DWORD**>(Interfaces::ClientMode()), true);
		pVGUIPanelHook = std::make_unique<VFTableHook>(reinterpret_cast<DWORD**>(Interfaces::VGUIPanel()), true);
		pMatSurfaceHook = std::make_unique<VFTableHook>(reinterpret_cast<DWORD**>(Interfaces::MatSurface()), true);

		for (; !hWindow; hWindow = FindWindowA(XorStr("Valve001"), NULL))
			Sleep(200);

		oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(hkWndProc)));

		oReset = pD3DDevice9Hook->Hook(16, hkReset);
		oEndScene = pD3DDevice9Hook->Hook(42, hkEndScene);
		oCreateMove = pClientModeHook->Hook(24, reinterpret_cast<CreateMoveFn>(hkCreateMove));
		oFrameStageNotify = pBaseClientHook->Hook(36, reinterpret_cast<FrameStageNotifyFn>(hkFrameStageNotify));
		oFireEventClientSide = pGameEventManagerHook->Hook(9, reinterpret_cast<FireEventClientSideFn>(hkFireEventClientSide));
		oPaintTraverse = pVGUIPanelHook->Hook(41, reinterpret_cast<PaintTraverseFn>(hkPaintTraverse));
		oPlaySound = pMatSurfaceHook->Hook(82, reinterpret_cast<PlaySoundFn>(hkPlaySound));

		Fonts::Init();

		Options::Init();
	}

	void Restore()
	{
		SetWindowLongPtr(hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oWndProc));

		pRenderer->InvalidateObjects();

		pD3DDevice9Hook->RestoreTable();
		pClientModeHook->RestoreTable();
		pVGUIPanelHook->RestoreTable();
		pMatSurfaceHook->RestoreTable();
	}

	void GUI_Init(IDirect3DDevice9* pDevice)
	{
		ImGui_ImplDX9_Init(hWindow, pDevice);

		pRenderer = std::make_unique<DrawManager>(pDevice);
		pRenderer->CreateObjects();

		bWasInitialized = true;
	}

	LRESULT __stdcall hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_KEYUP && wParam == VK_INSERT)
		{
			Options::bMainWindowOpen ^= true;

			// disable in-game mouse when in menu
			static ConVar* cl_mouseenable = Interfaces::CVar()->FindVar(XorStr("cl_mouseenable"));
			cl_mouseenable->SetValue(!Options::bMainWindowOpen);
		}

		if (bWasInitialized && Options::bMainWindowOpen && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true; // input was taken and used, return

		return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
	}

	HRESULT __stdcall hkEndScene(IDirect3DDevice9* pDevice)
	{
		HRESULT ret = oEndScene(pDevice);

		if (!bWasInitialized)
		{
			GUI_Init(pDevice);
		}
		else
		{
			ImGui::GetIO().MouseDrawCursor = Options::bMainWindowOpen;

			ImGui_ImplDX9_NewFrame();

			if (Options::bMainWindowOpen)
				DrawInterface();

			pRenderer->BeginRendering();
			ImGui::Render();
			pRenderer->EndRendering();
		}

		return ret;
	}

	HRESULT __stdcall hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		// IDirect3DDevice9::Reset is called when you minimize the game, alt-tab, or change resolutions
		// have to recreate resources or else you crash
		
		HRESULT hR = oReset(pDevice, pPresentationParameters);
		
		if (!bWasInitialized)
			return oReset(pDevice, pPresentationParameters);

		pRenderer->InvalidateObjects();
		ImGui_ImplDX9_InvalidateDeviceObjects();
		
		pRenderer->CreateObjects();
		ImGui_ImplDX9_CreateDeviceObjects();

		return hR;
	}

	bool __stdcall hkCreateMove(float sample_input_frametime, CUserCmd* pCmd)
	{
		bool ret = oCreateMove(Interfaces::ClientMode(), sample_input_frametime, pCmd);

		if (pCmd && pCmd->command_number)
		{
			Aimbot::CreateMove(pCmd);
			Bhop::CreateMove(pCmd);
			ESP::ShowRanks(pCmd);
		}

		if (Options::Aim::bSilent)
			return false;

		return ret;
	}

	void __stdcall hkFrameStageNotify(ClientFrameStage_t stage)
	{
		SkinChanger::FrameStageNotify(stage);

		oFrameStageNotify(Interfaces::Client(), stage);
	}

	bool __fastcall hkFireEventClientSide(void* thisptr, void* edx, IGameEvent* event)
	{
		return oFireEventClientSide(thisptr, event);;
	}
	
	void __fastcall hkPaintTraverse(void* thisptr, void* edx, VPANEL vguiPanel, bool forceRepaint, bool allowForce)
	{
		oPaintTraverse(thisptr, vguiPanel, forceRepaint, allowForce);

		// only draw on FocusOverlayPanel
		static VPANEL FocusOverlayPanel;
		if (!FocusOverlayPanel)
		{
			const char* panelName = Interfaces::VGUIPanel()->GetName(vguiPanel);
			if (!strcmp(panelName, "FocusOverlayPanel"))
				FocusOverlayPanel = vguiPanel;
		}
		else if (vguiPanel != FocusOverlayPanel)
			return;

		ESP::Draw();
		ESP::Glow(); // FIXMEW: move to [somewhere] to avoid flashing
	}

	void __stdcall hkPlaySound(const char* szFileName)
	{
		oPlaySound(Interfaces::MatSurface(), szFileName);
		
		AutoAccept::PlaySound(szFileName);
	}
}
