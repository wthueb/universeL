#pragma once

class IClientNetworkable;
class IClientEntity;

class IClientEntityList
{
public:
    virtual IClientNetworkable*           GetClientNetworkable(int nEntNum) = 0;
    virtual void*                         Unknown() = 0;
    virtual void*                         Unknown1() = 0;
    virtual IClientEntity*                GetClientEntity(int nEntNum) = 0;
    virtual IClientEntity*                GetClientEntityFromHandle(CBaseHandle hEntity) = 0;
    virtual int                           NumberOfEntities(bool bIncludeNonNetworkable) = 0;
    virtual int                           GetHighestEntityIndex() = 0;
    virtual void                          SetMaxEntities(int nMaxEnts) = 0;
    virtual int                           GetMaxEntities() = 0;
};
