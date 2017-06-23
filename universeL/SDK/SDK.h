#pragma once

#include "Definitions.h"

#include "CGlobalVarsBase.h"
#include "CGlowObjectManager.h"
#include "CHandle.h"
#include "CInput.h"
#include "Color.h"
#include "CRC.h"
#include "IBaseClientDll.h"
#include "IClientEntity.h"
#include "IClientEntityList.h"
#include "IClientMode.h"
#include "ICvar.h"
#include "IEngineTrace.h"
#include "IEngineVGui.h"
#include "IInputSystem.h"
#include "IMaterial.h"
#include "IPanel.h"
#include "ISurface.h"
#include "IVDebugOverlay.h"
#include "IVEngineClient.h"
#include "IVModelInfo.h"
#include "IVModelRender.h"
#include "PlayerInfo.h"
#include "Recv.h"
#include "Vector.h"


class Interfaces
{
public:
    static IVEngineClient*        Engine();
    static IBaseClientDLL*        Client();
    static IClientEntityList*     EntityList();
    static IClientMode*           ClientMode();
    static CGlobalVarsBase*       GlobalVars();
	static IInputSystem*		  InputSystem();
    static IPanel*				  VGUIPanel();
    static ISurface*              MatSurface();
    static IVDebugOverlay*        DebugOverlay();
	static CGlowObjectManager*    GlowObjManager();
    static IVModelInfo*			  ModelInfo();
    static IEngineTrace*          EngineTrace();
    static ICvar*                 CVar();

private:
    static IVEngineClient*        pEngine;
    static IBaseClientDLL*        pClient;
    static IClientEntityList*     pEntityList;
	static IClientMode*           pClientMode;
    static CGlobalVarsBase*       pGlobals;
	static IInputSystem*		  pInputSystem;
	static IPanel*				  pVGUIPanel;
    static ISurface*              pMatSurface;
	static IVDebugOverlay*		  pDebugOverlay;
	static CGlowObjectManager*    pGlowObjManager;
	static IVModelInfo*			  pModelInfo;
    static IEngineTrace*          pEngineTrace;
    static ICvar*                 pCVar;
};
