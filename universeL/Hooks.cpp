#include "Hooks.h"

#include "Hacks.h"
#include "Draw.h"

#include "DrawManager.h"
#include "NetvarManager.h"
#include "Options.h"
#include "Utils.h"
#include "XorStr.h"

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_dx9.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 12
#define VERSION_PATCH 4

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Hooks
{
	std::unique_ptr<VFTableHook>        pD3DDevice9Hook = nullptr;
	std::unique_ptr<VFTableHook>        pClientModeHook = nullptr;
	std::unique_ptr<VFTableHook>        pVGUIPanelHook = nullptr; 
	std::unique_ptr<VFTableHook>        pMatSurfaceHook = nullptr;

	std::unique_ptr<DrawManager>        pRenderer = nullptr;

	EndSceneFn                          oEndScene = nullptr;
	ResetFn                             oReset = nullptr;
	CreateMoveFn                        oCreateMove = nullptr;
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
		pClientModeHook = std::make_unique<VFTableHook>(reinterpret_cast<DWORD**>(Interfaces::ClientMode()), true);
		pVGUIPanelHook = std::make_unique<VFTableHook>(reinterpret_cast<DWORD**>(Interfaces::VGUIPanel()), true);
		pMatSurfaceHook = std::make_unique<VFTableHook>(reinterpret_cast<DWORD**>(Interfaces::MatSurface()), true);

		for (; !hWindow; hWindow = FindWindowA(XorStr("Valve001"), NULL))
			Sleep(200);

		oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(hkWndProc)));

		oReset = pD3DDevice9Hook->Hook(16, hkReset);
		oEndScene = pD3DDevice9Hook->Hook(42, hkEndScene);
		oCreateMove = pClientModeHook->Hook(24, reinterpret_cast<CreateMoveFn>(hkCreateMove));
		oPaintTraverse = pVGUIPanelHook->Hook(41, reinterpret_cast<PaintTraverseFn>(hkPaintTraverse));
		oPlaySound = pMatSurfaceHook->Hook(82, reinterpret_cast<PlaySoundFn>(hkPlaySound));

		Fonts::Init();
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
			// FIXMEW: put in own file & make prettier
			ImGui::GetIO().MouseDrawCursor = Options::bMainWindowOpen;

			ImGui_ImplDX9_NewFrame();

			if (Options::bMainWindowOpen)
			{
				ImGui::Begin(XorStr(name), &Options::bMainWindowOpen, ImVec2(300, 350), 1.f);

				if (ImGui::CollapsingHeader(XorStr("aim")))
				{
					ImGui::Checkbox(XorStr("aimbot enabled"), &Options::Aim::bAimbotEnabled);
					ImGui::Checkbox(XorStr("rcs enabled"), &Options::Aim::bRCSEnabled);
					ImGui::Separator();
					ImGui::InputFloat(XorStr("fov"), &Options::Aim::flFov, 0.f, 0.f, 1);
					Options::Aim::flFov = min(max(0.f, Options::Aim::flFov), 180.f);
					ImGui::Checkbox(XorStr("smooth"), &Options::Aim::bSmooth);
					ImGui::Checkbox(XorStr("smooth rcs (experimental)"), &Options::Aim::bSmoothRCS);
					ImGui::SliderFloat(XorStr("smooth amount"), &Options::Aim::flSmooth, 1.f, 10.f, "%.1f");
					//ImGui::Checkbox(XorStr("silent"), &Options::Aim::bSilent);
					ImGui::Separator();
					ImGui::Checkbox(XorStr("on shoot"), &Options::Aim::bOnShoot);
					ImGui::Checkbox(XorStr("on aimkey"), &Options::Aim::bOnAimkey);
					ImGui::InputInt(XorStr("aimkey"), &Options::Aim::nAimkey);
					ImGui::Text(XorStr("%s"), XorStr("mouse4 = 110"));
					ImGui::Separator();
					ImGui::InputInt(XorStr("bone"), &Options::Aim::nBone);
					ImGui::Text(XorStr("%s"), XorStr("8 is head, 7 is neck, 6 is upper chest..."));
					ImGui::Separator();
					ImGui::Checkbox(XorStr("visible check"), &Options::Aim::bVisibleCheck);
					ImGui::Separator();
					ImGui::Checkbox(XorStr("re-aim"), &Options::Aim::bReAim);
					ImGui::Checkbox(XorStr("lock on teammates"), &Options::Aim::bLockOnFriendly);
				}

				if (ImGui::CollapsingHeader(XorStr("visuals")))
				{
					ImGui::Checkbox(XorStr("glow enabled"), &Options::ESP::bGlowEnabled);
					ImGui::Separator();
					ImGui::Checkbox(XorStr("esp enabled"), &Options::ESP::bDrawEnabled);
					ImGui::Checkbox(XorStr("corner boxes"), &Options::ESP::bCornerBoxes);
					ImGui::Separator();
					ImGui::Checkbox(XorStr("draw players"), &Options::ESP::bDrawPlayers);
					ImGui::Checkbox(XorStr("draw player box"), &Options::ESP::bDrawPlayerBox);
					ImGui::Checkbox(XorStr("draw skeleton"), &Options::ESP::bDrawSkeleton);
					ImGui::Checkbox(XorStr("only enemies"), &Options::ESP::bOnlyEnemies);
					ImGui::Separator();
					ImGui::Checkbox(XorStr("show name"), &Options::ESP::bShowName);
					ImGui::Checkbox(XorStr("show health"), &Options::ESP::bShowHealth);
					ImGui::Checkbox(XorStr("show health text"), &Options::ESP::bShowHealthText);
					ImGui::Separator();
					ImGui::Checkbox(XorStr("draw bomb"), &Options::ESP::bDrawBomb);
					ImGui::Checkbox(XorStr("draw nades"), &Options::ESP::bDrawNades);
					ImGui::Separator();
					ImGui::ColorEdit3(XorStr("ally color"), Options::ESP::fAllyColor);
					ImGui::ColorEdit3(XorStr("enemy color"), Options::ESP::fEnemyColor);
				}

				if (ImGui::CollapsingHeader(XorStr("misc")))
				{
					ImGui::Checkbox(XorStr("bhop enabled"), &Options::Misc::bBhopEnabled);
					ImGui::Separator();
					ImGui::Checkbox(XorStr("show ranks"), &Options::Misc::bShowRanks);
					ImGui::Separator();
					ImGui::Checkbox(XorStr("auto accept"), &Options::Misc::bAutoAccept);
				}

				ImGui::End();
			}

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
	
	void __fastcall hkPaintTraverse(void* thisptr, void*, VPANEL vguiPanel, bool forceRepaint, bool allowForce)
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
