#pragma once

#include "Definitions.h"

#include "..\NetvarManager.h"

#include "IEngineTrace.h"
#include "IHandleEntity.h"
#include "Vector.h"
#include "VMatrix.h"

#define MAX_SHOOT_SOUNDS 16
#define MAX_WEAPON_STRING 80
#define MAX_WEAPON_PREFIX 16
#define MAX_WEAPON_AMMO_NAME 32

enum WeaponSound_t
{
	EMPTY,
	SINGLE,
	SINGLE_NPC,
	WPN_DOUBLE, // Can't be "DOUBLE" because windows.h uses it.
	DOUBLE_NPC,
	BURST,
	RELOAD,
	RELOAD_NPC,
	MELEE_MISS,
	MELEE_HIT,
	MELEE_HIT_WORLD,
	SPECIAL1,
	SPECIAL2,
	SPECIAL3,
	TAUNT,
	FAST_RELOAD,
	
	// Add new shoot sound types here
	REVERSE_THE_NEW_SOUND,

	NUM_SHOOT_SOUND_TYPES,
};

enum MoveType_t
{
	MOVETYPE_NONE = 0,
	MOVETYPE_ISOMETRIC,
	MOVETYPE_WALK,
	MOVETYPE_STEP,
	MOVETYPE_FLY,
	MOVETYPE_FLYGRAVITY,
	MOVETYPE_VPHYSICS,
	MOVETYPE_PUSH,
	MOVETYPE_NOCLIP,
	MOVETYPE_LADDER,
	MOVETYPE_OBSERVER,
	MOVETYPE_CUSTOM,
	MOVETYPE_LAST = MOVETYPE_CUSTOM,
	MOVETYPE_MAX_BITS = 4
};

enum DataUpdateType_t
{
	DATA_UPDATE_CREATED = 0,
	DATA_UPDATE_DATATABLE_CHANGED,
};

class ICollideable
{
public:
	virtual void                       pad() = 0;
	virtual const Vector&              OBBMins() const = 0;
	virtual const Vector&              OBBMaxs() const = 0;
};

class IClientUnknown;

typedef unsigned short ClientShadowHandle_t;
typedef unsigned short ClientRenderHandle_t;
typedef unsigned short ModelInstanceHandle_t;
typedef unsigned char uint8;

struct model_t;

class IClientRenderable
{
public:
	virtual IClientUnknown*            GetIClientUnknown() = 0;
	virtual Vector const&              GetRenderOrigin() = 0;
	virtual QAngle const&              GetRenderAngles() = 0;
	virtual bool                       ShouldDraw() = 0;
	virtual int                        GetRenderFlags() = 0; // ERENDERFLAGS_xxx
	virtual void                       Unused() const {}
	virtual ClientShadowHandle_t       GetShadowHandle() const = 0;
	virtual ClientRenderHandle_t&      RenderHandle() = 0;
	virtual model_t*                   GetModel() = 0;
	virtual int                        DrawModel(int flags, const int /*RenderableInstance_t*/ &instance) = 0;
	virtual int                        GetBody() = 0;
	virtual void                       GetColorModulation(float* color) = 0;
	virtual bool                       LODTest() = 0;
	virtual bool                       SetupBones(matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) = 0;
	virtual void                       SetupWeights(const matrix3x4_t* pBoneToWorld, int nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights) = 0;
	virtual void                       DoAnimationEvents() = 0;
	virtual void* /*IPVSNotify*/       GetPVSNotifyInterface() = 0;
	virtual void                       GetRenderBounds(Vector &mins, Vector &maxs) = 0;
	virtual void                       GetRenderBoundsWorldspace(Vector &mins, Vector &maxs) = 0;
	virtual void                       GetShadowRenderBounds(Vector &mins, Vector &maxs, int /*ShadowType_t*/ shadowType) = 0;
	virtual bool                       ShouldReceiveProjectedTextures(int flags) = 0;
	virtual bool                       GetShadowCastDistance(float* pDist, int /*ShadowType_t*/ shadowType) const = 0;
	virtual bool                       GetShadowCastDirection(Vector* pDirection, int /*ShadowType_t*/ shadowType) const = 0;
	virtual bool                       IsShadowDirty() = 0;
	virtual void                       MarkShadowDirty(bool bDirty) = 0;
	virtual IClientRenderable*         GetShadowParent() = 0;
	virtual IClientRenderable*         FirstShadowChild() = 0;
	virtual IClientRenderable*         NextShadowPeer() = 0;
	virtual int /*ShadowType_t*/       ShadowCastType() = 0;
	virtual void                       CreateModelInstance() = 0;
	virtual ModelInstanceHandle_t      GetModelInstance() = 0;
	virtual const matrix3x4_t&         RenderableToWorldTransform() = 0;
	virtual int                        LookupAttachment(const char* pAttachmentName) = 0;
	virtual   bool                     GetAttachment(int number, Vector &origin, QAngle &angles) = 0;
	virtual bool                       GetAttachment(int number, matrix3x4_t &matrix) = 0;
	virtual float*                     GetRenderClipPlane() = 0;
	virtual int                        GetSkin() = 0;
	virtual void                       OnThreadedDrawSetup() = 0;
	virtual bool                       UsesFlexDelayedWeights() = 0;
	virtual void                       RecordToolMessage() = 0;
	virtual bool                       ShouldDrawForSplitScreenUser(int nSlot) = 0;
	virtual uint8                      OverrideAlphaModulation(uint8 nAlpha) = 0;
	virtual uint8                      OverrideShadowAlphaModulation(uint8 nAlpha) = 0;
};

class ClientClass;
class bf_read;

class IClientNetworkable
{
public:
	virtual IClientUnknown*           GetIClientUnknown() = 0;
	virtual void                      Release() = 0;
	virtual ClientClass*              GetClientClass() = 0;
	virtual void                      NotifyShouldTransmit(int state) = 0;
	virtual void                      OnPreDataChanged(int updateType) = 0;
	virtual void                      OnDataChanged(int updateType) = 0;
	virtual void                      PreDataUpdate(int updateType) = 0;
	virtual void                      PostDataUpdate(int updateType) = 0;
	virtual void                      Unknown() = 0;
	virtual bool                      IsDormant() = 0;
	virtual int                       EntIndex() const = 0;
	virtual void                      ReceiveMessage(int classID, bf_read &msg) = 0;
	virtual void*                     GetDataTableBasePtr() = 0;
	virtual void                      SetDestroyedOnRecreateEntities() = 0;
};

class IClientUnknown;
class CClientThinkHandlePtr;
typedef CClientThinkHandlePtr* ClientThinkHandle_t;

class IClientThinkable
{
public:
	virtual IClientUnknown*		        GetIClientUnknown() = 0;
	virtual void				        ClientThink() = 0;
	virtual ClientThinkHandle_t	        GetThinkHandle() = 0;
	virtual void				        SetThinkHandle(ClientThinkHandle_t hThink) = 0;
	virtual void				        Release() = 0;
};

class IClientAlphaProperty;
class IClientEntity;
class C_BaseEntity;

class IClientUnknown : public IHandleEntity
{
public:
	virtual ICollideable*              GetCollideable() = 0;
	virtual IClientNetworkable*        GetClientNetworkable() = 0;
	virtual IClientRenderable*         GetClientRenderable() = 0;
	virtual IClientEntity*             GetIClientEntity() = 0;
	virtual C_BaseEntity*              GetBaseEntity() = 0;
	virtual IClientThinkable*          GetClientThinkable() = 0;
	//virtual IClientModelRenderable*  GetClientModelRenderable() = 0;
	virtual IClientAlphaProperty*      GetClientAlphaProperty() = 0;
};

class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	virtual ~IClientEntity() {};
};

class C_BaseEntity : public IClientEntity
{
public:

	IClientNetworkable* GetNetworkable()
	{
		return reinterpret_cast<IClientNetworkable*>(reinterpret_cast<uintptr_t>(this) + 0x10);
	}

	void SetModelIndex(int index)
	{
		typedef void(*oSetModelIndex)(void*, int);
		return CallVFunction<oSetModelIndex>(this, 111)(this, index);
	}

	int* GetModelIndex()
	{
		return reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseViewModel"), XorStr("m_nModelIndex")));
	}

	float GetAnimTime()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_flAnimTime")));
	}

	float GetSimulationTime()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_flSimulationTime")));
	}

	TeamID GetTeam()
	{
		return *reinterpret_cast<TeamID*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_iTeamNum")));
	}

	Vector GetVecOrigin()
	{
		return *reinterpret_cast<Vector*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_vecOrigin")));
	}

	MoveType_t GetMoveType()
	{
		return *reinterpret_cast<MoveType_t*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_MoveType")));
	}

	ICollideable* GetCollideable()
	{
		return reinterpret_cast<ICollideable*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_Collision")));
	}

	bool* GetSpotted()
	{
		return reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_bSpotted")));
	}
};

class C_BasePlayer : public C_BaseEntity
{
public:
	QAngle* GetViewPunch()
	{
		return reinterpret_cast<QAngle*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_Local"), XorStr("m_viewPunchAngle")));
	}

	QAngle* GetAimPunch()
	{
		return reinterpret_cast<QAngle*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_Local"), XorStr("m_aimPunchAngle")));
	}

	Vector GetVecViewOffset()
	{
		return *reinterpret_cast<Vector*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_vecViewOffset[0]")));
	}

	unsigned int GetTickBase()
	{
		return *reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_nTickBase")));
	}

	Vector GetVelocity()
	{
		return *reinterpret_cast<Vector*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_vecVelocity")));
	}

	int GetHealth()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_iHealth")));
	}

	unsigned char GetLifeState()
	{
		return *reinterpret_cast<unsigned char*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_lifeState")));
	}

	int GetFlags()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_fFlags")));
	}

	ObserverMode_t* GetObserverMode()
	{
		return reinterpret_cast<ObserverMode_t*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_iObserverMode")));
	}

	void* GetObserverTarget()
	{
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_hObserverTarget")));
	}

	void* GetViewModel()
	{
		return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_hViewModel")));
	}

	const char* GetLastPlaceName()
	{
		return reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_szLastPlaceName")));
	}

	int GetShotsFired()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_iShotsFired")));
	}

	QAngle GetEyeAngle()
	{
		return *reinterpret_cast<QAngle*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_angEyeAngles[0]")));
	}

	int GetMoney()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_iAccount")));
	}

	int GetHits()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_totalHitsOnServer")));
	}

	int GetArmor()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_ArmorValue")));
	}

	int HasDefuser()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_bHasDefuser")));
	}

	bool IsDefusing()
	{
		return *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_bIsDefusing")));
	}

	bool IsGrabbingHostage()
	{
		return *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_bIsGrabbingHostage")));
	}

	bool IsScoped()
	{
		return *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_bIsScoped")));
	}

	bool GetImmune()
	{
		return *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_bGunGameImmunity")));
	}

	bool IsRescuing()
	{
		return *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_bIsRescuing")));
	}

	int HasHelmet()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_bHasHelmet")));
	}

	float GetFlashBangTime()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0xABE4);
	}

	float GetFlashDuration()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_flFlashDuration")));
	}

	float* GetFlashMaxAlpha()
	{
		return reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_flFlashMaxAlpha")));
	}

	float* GetLowerBodyYawTarget()
	{
		return reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_flLowerBodyYawTarget")));
	}

	CBaseHandle GetActiveWeaponHandle()
	{
		return *reinterpret_cast<CBaseHandle*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseCombatCharacter"), XorStr("m_hActiveWeapon")));
	}

	int* GetWeapons()
	{
		return reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseCombatCharacter"), XorStr("m_hMyWeapons")));
	}

	int* GetWearables()
	{
		return reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseCombatCharacter"), XorStr("m_hMyWearables")));
	}

	bool IsAlive()
	{
		return this->GetHealth() > 0 && this->GetLifeState() == 0;
	}

	Vector GetEyePosition()
	{
		return this->GetVecOrigin() + this->GetVecViewOffset();
	}

	inline Vector GetBonePosition(int boneIndex)
	{
		matrix3x4_t BoneMatrix[MAXSTUDIOBONES];

		if (!this->SetupBones(BoneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0))
			return this->GetVecOrigin();

		matrix3x4_t hitbox = BoneMatrix[boneIndex];

		return Vector(hitbox[0][3], hitbox[1][3], hitbox[2][3]);
	}

	QAngle* GetVAngles()
	{
		return reinterpret_cast<QAngle*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("deadflag + 0x4")));
	}
};

class C_PlantedC4 : public C_BaseEntity
{
public:
	bool IsBombTicking()
	{
		return *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_PlantedC4"), XorStr("m_bBombTicking")));
	}

	float GetBombTime()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_PlantedC4"), XorStr("m_flC4Blow")));
	}

	bool IsBombDefused()
	{
		return *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_PlantedC4"), XorStr("m_bBombDefused")));
	}

	int GetBombDefusing()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_PlantedC4"), XorStr("m_hBombDefuser")));
	}
};

class C_BaseAttributableItem : public C_BaseEntity
{
public:
	EItemDefinitionIndex* GetItemDefinitionIndex()
	{
		return reinterpret_cast<EItemDefinitionIndex*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseAttributableItem"),
			XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iItemDefinitionIndex")));
	}

	int* GetItemIDHigh()
	{
		return reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iItemIDHigh")));
	}

	int* GetEntityQuality()
	{
		return reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iEntityQuality")));
	}

	char* GetCustomName()
	{
		return reinterpret_cast<char*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_szCustomName")));
	}

	int* GetFallbackPaintKit()
	{
		return reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_nFallbackPaintKit")));
	}

	int* GetFallbackSeed()
	{
		return reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_nFallbackSeed")));
	}

	float* GetFallbackWear()
	{
		return reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_flFallbackWear")));
	}

	int* GetFallbackStatTrak()
	{
		return reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_nFallbackStatTrak")));
	}
};

class C_BaseViewModel : public C_BaseEntity
{
public:
	int GetWeapon()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseViewModel"), XorStr("m_hWeapon")));
	}

	int GetOwner()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseViewModel"), XorStr("m_hOwner")));
	}
};

class CHudTexture;
class KeyValues;

class FileWeaponInfo_t
{
public:
	FileWeaponInfo_t() {}

	// Each game can override this to get whatever values it wants from the script.
	virtual void Parse(KeyValues *pKeyValuesData, const char *szWeaponName) = 0;

	bool bParsedScript;
	bool bLoadedHudElements;

	char szClassName[MAX_WEAPON_STRING];
	char szPrintName[MAX_WEAPON_STRING];

	char szViewModel[MAX_WEAPON_STRING];
	char szWorldModel[MAX_WEAPON_STRING];
	char szAmmo1[MAX_WEAPON_AMMO_NAME];
	char szWorldDroppedModel[MAX_WEAPON_STRING];
	char szAnimationPrefix[MAX_WEAPON_PREFIX];
	int iSlot;
	int iPosition;
	int iMaxClip1;
	int iMaxClip2;
	int iDefaultClip1;
	int iDefaultClip2;
	int iWeight;
	int iRumbleEffect;
	bool bAutoSwitchTo;
	bool bAutoSwitchFrom;
	int iFlags;
	char szAmmo2[MAX_WEAPON_AMMO_NAME];
	char szAIAddOn[MAX_WEAPON_STRING];

	// Sound blocks
	char aShootSounds[NUM_SHOOT_SOUND_TYPES][MAX_WEAPON_STRING];

	int iAmmoType;
	int iAmmo2Type;
	bool m_bMeleeWeapon;

	// This tells if the weapon was built right-handed (defaults to true).
	// This helps cl_righthand make the decision about whether to flip the model or not.
	bool m_bBuiltRightHanded;
	bool m_bAllowFlipping;

	// Sprite data, read from the data file
	int iSpriteCount;
	CHudTexture* iconActive;
	CHudTexture* iconInactive;
	CHudTexture* iconAmmo;
	CHudTexture* iconAmmo2;
	CHudTexture* iconCrosshair;
	CHudTexture* iconAutoaim;
	CHudTexture* iconZoomedCrosshair;
	CHudTexture* iconZoomedAutoaim;
	CHudTexture* iconSmall;
};

class CCSWeaponInfo : public FileWeaponInfo_t
{
public:
	CSWeaponType GetWeaponType()
	{
		return *reinterpret_cast<CSWeaponType*>(reinterpret_cast<uintptr_t>(this) + 0x864);
	}

	bool IsFullAuto()
	{
		return *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + 0x870);
	}

	float GetWeaponArmorRatio()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0x87C);
	}

	float GetMaxPlayerSpeed()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0x880);
	}

	float GetMaxPlayerSpeedAlt()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0x884);
	}

	float GetPenetration()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0x890);
	}

	int GetDamage()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + 0x894);
	}

	float GetRange()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0x898);
	}

	float GetRangeModifier()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0x89C);
	}

	float GetSpread()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0xA4C);
	}

	int GetZoomLevels()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + 0xEE0);
	}
};

class C_BaseCombatWeapon : public C_BaseAttributableItem
{
public:
	int GetOwner()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseCombatWeapon"), XorStr("m_hOwner")));
	}

	int GetAmmo()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseCombatWeapon"), XorStr("m_iClip1")));
	}

	float GetNextPrimaryAttack()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseCombatWeapon"), XorStr("m_flNextPrimaryAttack")));
	}

	bool GetInReload()
	{
		return *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseCombatWeapon"), XorStr("m_bInReload")));
	}

	float GetAccuracyPenalty()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_WeaponCSBase"), XorStr("m_fAccuracyPenalty")));
	}

	bool IsPistol()
	{
		int weaponid = *this->GetItemDefinitionIndex();

		return weaponid == EItemDefinitionIndex::weapon_deagle || weaponid == EItemDefinitionIndex::weapon_elite ||
			weaponid == EItemDefinitionIndex::weapon_fiveseven || weaponid == EItemDefinitionIndex::weapon_p250 ||
			weaponid == EItemDefinitionIndex::weapon_glock || weaponid == EItemDefinitionIndex::weapon_hkp2000 ||
			weaponid == EItemDefinitionIndex::weapon_cz75a || weaponid == EItemDefinitionIndex::weapon_usp_silencer ||
			weaponid == EItemDefinitionIndex::weapon_tec9 || weaponid == EItemDefinitionIndex::weapon_revolver;
	}

	bool IsShotgun()
	{
		int weaponid = *this->GetItemDefinitionIndex();

		return weaponid == EItemDefinitionIndex::weapon_xm1014 || weaponid == EItemDefinitionIndex::weapon_nova ||
			weaponid == EItemDefinitionIndex::weapon_sawedoff || weaponid == EItemDefinitionIndex::weapon_mag7;
	}

	bool IsSniper()
	{
		int weaponid = *this->GetItemDefinitionIndex();

		return weaponid == EItemDefinitionIndex::weapon_awp || weaponid == EItemDefinitionIndex::weapon_scar20 ||
			weaponid == EItemDefinitionIndex::weapon_g3sg1 || weaponid == EItemDefinitionIndex::weapon_ssg08;
	}

	bool IsNade()
	{
		int weaponid = *this->GetItemDefinitionIndex();

		return weaponid == EItemDefinitionIndex::weapon_flashbang || weaponid == EItemDefinitionIndex::weapon_hegrenade ||
			weaponid == EItemDefinitionIndex::weapon_smokegrenade || weaponid == EItemDefinitionIndex::weapon_molotov ||
			weaponid == EItemDefinitionIndex::weapon_decoy || weaponid == EItemDefinitionIndex::weapon_incgrenade;
	}

	bool IsKnife()
	{
		int weaponid = *this->GetItemDefinitionIndex();

		return weaponid == EItemDefinitionIndex::weapon_knife || weaponid == EItemDefinitionIndex::weapon_knife_t ||
			weaponid >= 500;
	}

	bool IsBomb()
	{
		int weaponid = *this->GetItemDefinitionIndex();

		return weaponid == EItemDefinitionIndex::weapon_c4;
	}

	CCSWeaponInfo* GetCSWpnData()
	{
		typedef CCSWeaponInfo* (*oGetCSWpnData)(void*);
		return CallVFunction<oGetCSWpnData>(this, 524)(this);
	}

	float GetInaccuracy()
	{
		typedef float(*oGetInaccuracy)(void*);
		return CallVFunction<oGetInaccuracy>(this, 552)(this);
	}

	float GetSpread()
	{
		typedef float(*oGetSpread)(void*);
		return CallVFunction<oGetSpread>(this, 553)(this);
	}

	void UpdateAccuracyPenalty()
	{
		typedef void(*oUpdateAccuracyPenalty)(void*);
		return CallVFunction<oUpdateAccuracyPenalty>(this, 554)(this);
	}
};

class C_WeaponC4 : C_BaseCombatWeapon
{
public:
	bool GetStartedArming()
	{
		return *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_WeaponC4"), XorStr("m_bStartedArming")));
	}
};

class C_Chicken : C_BaseEntity
{
public:
	bool* GetShouldGlow()
	{
		return reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_DynamicProp"), XorStr("m_bShouldGlow")));
	}
};

class C_BaseCSGrenade : C_BaseCombatWeapon
{
public:
	bool IsHeldByPlayer()
	{
		return *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseCSGrenade"), XorStr("m_bIsHeldByPlayer")));
	}

	bool GetPinPulled()
	{
		return *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseCSGrenade"), XorStr("m_bPinPulled")));
	}

	float GetThrowTime()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseCSGrenade"), XorStr("m_fThrowTime")));
	}

	float GetThrowStrength()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + GET_NETVAR(XorStr("DT_BaseCSGrenade"), XorStr("m_flThrowStrength")));
	}
};
