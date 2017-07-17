#pragma once

#include "Definitions.h"

#include "CGlobalVarsBase.h"

class IClientNetworkable;
using CreateClientClassFn = IClientNetworkable*(*)(int entnum, int serialnum);
using CreateEventFn = IClientNetworkable*(*)();

class RecvTable;
enum EClassIds;

class ClientClass
{
public:
	CreateClientClassFn    m_pCreateFn;
	CreateEventFn*         m_pCreateEventFn;
	char*                  m_pNetworkName;
	RecvTable*             m_pRecvTable;
	ClientClass*           m_pNext;
	EClassIds              m_ClassID;
};

class IBaseClientDLL
{
public:
    virtual int              Connect(CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals) = 0;
    virtual int              Disconnect(void) = 0;
    virtual int              Init(CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals) = 0;
    virtual void             PostInit() = 0;
    virtual void             Shutdown(void) = 0;
    virtual void             LevelInitPreEntity(char const* pMapName) = 0;
    virtual void             LevelInitPostEntity() = 0;
    virtual void             LevelShutdown(void) = 0;
    virtual ClientClass*     GetAllClasses(void) = 0;
	virtual void			 HudProcessInput(bool bActive) = 0;
};
