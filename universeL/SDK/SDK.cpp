#include "SDK.h"

#include "..\Utils.h"
#include "..\XorStr.h"


IVEngineClient*     Interfaces::pEngine = nullptr;
IBaseClientDLL*     Interfaces::pClient = nullptr;
IClientEntityList*  Interfaces::pEntityList = nullptr;
IClientMode*        Interfaces::pClientMode = nullptr;
CGlobalVarsBase*    Interfaces::pGlobals = nullptr;
IInputSystem*		Interfaces::pInputSystem = nullptr;
IPanel*             Interfaces::pVGUIPanel = nullptr;
ISurface*           Interfaces::pMatSurface = nullptr;
IVDebugOverlay*     Interfaces::pDebugOverlay = nullptr;
CGlowObjectManager* Interfaces::pGlowObjManager = nullptr;
IVModelInfo*		Interfaces::pModelInfo = nullptr;
IEngineTrace*       Interfaces::pEngineTrace = nullptr;
ICvar*              Interfaces::pCVar = nullptr;


CreateInterfaceFn GetFactory(HMODULE hMod)
{
    return reinterpret_cast<CreateInterfaceFn>(GetProcAddress(hMod, XorStr("CreateInterface")));
}

template<typename T>
T* CaptureInterface(CreateInterfaceFn f, const char* szInterfaceVersion)
{
    return static_cast<T*>(f(szInterfaceVersion, NULL));
}


IVEngineClient* Interfaces::Engine()
{
    if(!pEngine)
	{
        CreateInterfaceFn pfnFactory = GetFactory(GetModuleHandleA(XorStr("engine.dll")));
        pEngine = CaptureInterface<IVEngineClient>(pfnFactory, XorStr("VEngineClient014"));
    }
    return pEngine;
}

IBaseClientDLL* Interfaces::Client()
{
    if(!pClient)
	{
        CreateInterfaceFn pfnFactory = GetFactory(GetModuleHandleA(XorStr("client.dll")));
        pClient = CaptureInterface<IBaseClientDLL>(pfnFactory, XorStr("VClient018"));
    }
    return pClient;
}

IClientEntityList* Interfaces::EntityList()
{
    if(!pEntityList)
	{
        CreateInterfaceFn pfnFactory = GetFactory(GetModuleHandleA(XorStr("client.dll")));
        pEntityList = CaptureInterface<IClientEntityList>(pfnFactory, XorStr("VClientEntityList003"));
    }
    return pEntityList;
}

IClientMode* Interfaces::ClientMode()
{
	if (!pClientMode)
	{
		uint32_t* pClientVFTable = *reinterpret_cast<uint32_t**>(Client());
		pClientMode = **reinterpret_cast<IClientMode***>(pClientVFTable[10] + 0x5);
	}
	return pClientMode;
}

CGlobalVarsBase* Interfaces::GlobalVars()
{
    if(!pGlobals)
	{
		uint64_t address = Utils::FindSignature(XorStr("engine.dll"), XorStr("68 ? ? ? ? 68 ? ? ? ? FF 50 08 85 C0")) + 1;
        pGlobals = *reinterpret_cast<CGlobalVarsBase**>(address);
    }
    return pGlobals;
}

IInputSystem* Interfaces::InputSystem()
{
	if (!pInputSystem)
	{
		CreateInterfaceFn pfnFactory = GetFactory(GetModuleHandleA(XorStr("inputsystem.dll")));
		pInputSystem = CaptureInterface<IInputSystem>(pfnFactory, XorStr("InputSystemVersion001"));
	}
	return pInputSystem;
}

IPanel* Interfaces::VGUIPanel()
{
	if (!pVGUIPanel)
	{
		CreateInterfaceFn pfnFactory = GetFactory(GetModuleHandleA(XorStr("vgui2.dll")));
		pVGUIPanel = CaptureInterface<IPanel>(pfnFactory, XorStr("VGUI_Panel009"));
	}
	return pVGUIPanel;
}

ISurface* Interfaces::MatSurface()
{
	if (!pMatSurface)
	{
		CreateInterfaceFn pfnFactory = GetFactory(GetModuleHandleA(XorStr("vguimatsurface.dll")));
		pMatSurface = CaptureInterface<ISurface>(pfnFactory, XorStr("VGUI_Surface031"));
	}
	return pMatSurface;
}

IVDebugOverlay* Interfaces::DebugOverlay()
{
	if (!pDebugOverlay)
	{
		CreateInterfaceFn pfnFactory = GetFactory(GetModuleHandleA(XorStr("engine.dll")));
		pDebugOverlay = CaptureInterface<IVDebugOverlay>(pfnFactory, XorStr("VDebugOverlay004"));
	}
	return pDebugOverlay;
}

CGlowObjectManager* Interfaces::GlowObjManager()
{
	if (!pGlowObjManager)
	{
		uint64_t address = Utils::FindSignature(XorStr("client.dll"), XorStr("0F 11 05 ? ? ? ? 83 C8 01")) + 3;
		pGlowObjManager = *reinterpret_cast<CGlowObjectManager**>(address);
	}
	return pGlowObjManager;
}

IVModelInfo* Interfaces::ModelInfo()
{
	if (!pModelInfo)
	{
		CreateInterfaceFn pfnFactory = GetFactory(GetModuleHandleA(XorStr("engine.dll")));
		pModelInfo = CaptureInterface<IVModelInfo>(pfnFactory, XorStr("VModelInfoClient004"));
	}
	return pModelInfo;
}

IEngineTrace* Interfaces::EngineTrace()
{
    if(!pEngineTrace)
	{
        CreateInterfaceFn pfnFactory = GetFactory(GetModuleHandleA(XorStr("engine.dll")));
        pEngineTrace = CaptureInterface<IEngineTrace>(pfnFactory, XorStr("EngineTraceClient004"));
    }
    return pEngineTrace;
}

ICvar* Interfaces::CVar()
{
    if(!pCVar)
	{
        CreateInterfaceFn pfnFactory = GetFactory(GetModuleHandleA(XorStr("vstdlib.dll")));
        pCVar = CaptureInterface<ICvar>(pfnFactory, XorStr("VEngineCvar007"));
    }
    return pCVar;
}
