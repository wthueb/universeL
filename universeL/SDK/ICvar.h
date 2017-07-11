#pragma once

#include "IAppSystem.h"

#define FORCEINLINE_CVAR            inline

#define FCVAR_NONE					0

#define FCVAR_UNREGISTERED			(1 << 0)
#define FCVAR_DEVELOPMENTONLY		(1 << 1)
#define FCVAR_GAMEDLL				(1 << 2)
#define FCVAR_CLIENTDLL				(1 << 3)
#define FCVAR_HIDDEN				(1 << 4)

#define FCVAR_PROTECTED				(1 << 5)
#define FCVAR_SPONLY				(1 << 6)
#define FCVAR_ARCHIVE				(1 << 7)
#define FCVAR_NOTIFY				(1 << 8)
#define FCVAR_USERINFO				(1 << 9)
#define FCVAR_CHEAT					(1 << 14)

#define FCVAR_PRINTABLEONLY			(1 << 10)
#define FCVAR_UNLOGGED				(1 << 11)
#define FCVAR_NEVER_AS_STRING		(1 << 12)
#define FCVAR_RELEASE				(1 << 19)

#define FCVAR_REPLICATED			(1 << 13)
#define FCVAR_DEMO					(1 << 16)
#define FCVAR_DONTRECORD			(1 << 17)

#define FCVAR_NOT_CONNECTED			(1 << 22)
#define FCVAR_ARCHIVE_XBOX			(1 << 24)
#define FCVAR_SERVER_CAN_EXECUTE	(1 << 28)
#define FCVAR_SERVER_CANNOT_QUERY	(1 << 29)
#define FCVAR_CLIENTCMD_CAN_EXECUTE	(1 << 30)

// FIXMEW: should implement proper cutlstring
struct CUtlString_t
{
	char*   szBuffer;	//0x0000 
	int     nCapacity;	//0x0004 
	int     _unknown;	//0x0008 
	int     nLength;	//0x000C 
};

class ConVar;
class CCommand;
class ConCommand;
class ConCommandBase;
struct characterset_t;
class IConVar;

using FnChangeCallback_t = void(*)(IConVar* var, const char* pOldValue, float flOldValue);

class IConVar
{
public:
	virtual void        SetValue(const char* pValue) = 0;
	virtual void        SetValue(float flValue) = 0;
	virtual void        SetValue(int nValue) = 0;
	virtual void        SetValue(Color value) = 0;
	virtual const char* GetName() const = 0;
	virtual const char* GetBaseName() const = 0;
	virtual bool        IsFlagSet(int nFlag) const = 0;
	virtual int         GetSplitScreenPlayerSlot() const = 0;
};

class CCommand
{
public:
	CCommand();
	CCommand(int nArgC, const char** ppArgV);

	bool                       Tokenize(const char* pCommand, characterset_t* pBreakSet = NULL);
	void                       Reset();

	int                        ArgC() const;
	const char**               ArgV() const;
	const char*                ArgS() const;					  // All args that occur after the 0th arg, in string form
	const char*                GetCommandString() const;		  // The entire command in string form, including the 0th arg
	const char*                operator[](int nIndex) const;	  // Gets at arguments
	const char*                Arg(int nIndex) const;		      // Gets at arguments

	// Helper functions to parse arguments to commands.
	const char*                FindArg(const char* pName) const;
	int                        FindArgInt(const char* pName, int nDefaultVal) const;

	static int                 MaxCommandLength();
	static characterset_t*     DefaultBreakSet();

private:
	enum
	{
		COMMAND_MAX_ARGC = 64,
		COMMAND_MAX_LENGTH = 512,
	};

	int		      m_nArgc;
	int		      m_nArgv0Size;
	char	      m_pArgSBuffer[COMMAND_MAX_LENGTH];
	char	      m_pArgvBuffer[COMMAND_MAX_LENGTH];
	const char*	  m_ppArgv[COMMAND_MAX_ARGC];
};

inline int CCommand::MaxCommandLength()
{
	return COMMAND_MAX_LENGTH - 1;
}

inline int CCommand::ArgC() const
{
	return m_nArgc;
}

inline const char** CCommand::ArgV() const
{
	return m_nArgc ? const_cast<const char**>(m_ppArgv) : NULL;
}

inline const char* CCommand::ArgS() const
{
	return m_nArgv0Size ? &m_pArgSBuffer[m_nArgv0Size] : "";
}

inline const char* CCommand::GetCommandString() const
{
	return m_nArgc ? m_pArgSBuffer : "";
}

inline const char* CCommand::Arg(int nIndex) const
{
	// FIXME: Many command handlers appear to not be particularly careful
	// about checking for valid argc range. For now, we're going to
	// do the extra check and return an empty string if it's out of range
	if (nIndex < 0 || nIndex >= m_nArgc)
		return "";
	return m_ppArgv[nIndex];
}

inline const char* CCommand::operator[](int nIndex) const
{
	return Arg(nIndex);
}

//-----------------------------------------------------------------------------
// Any executable that wants to use ConVars need to implement one of
// these to hook up access to console variables.
//-----------------------------------------------------------------------------
class IConCommandBaseAccessor
{
public:
	// Flags is a combination of FCVAR flags in cvar.h.
	// hOut is filled in with a handle to the variable.
	virtual bool RegisterConCommandBase(ConCommandBase* pVar) = 0;
};

//-----------------------------------------------------------------------------
// Called when a ConCommand needs to execute
//-----------------------------------------------------------------------------
using FnCommandCallbackV1_t = void(*)();
using FnCommandCallback_t = void(*)(const CCommand &command);

#define COMMAND_COMPLETION_MAXITEMS       64
#define COMMAND_COMPLETION_ITEM_LENGTH    64

//-----------------------------------------------------------------------------
// Returns 0 to COMMAND_COMPLETION_MAXITEMS worth of completion strings
//-----------------------------------------------------------------------------
using FnCommandCompletionCallback = int(*)(const char* partial, char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH]);


//-----------------------------------------------------------------------------
// Interface version
//-----------------------------------------------------------------------------
class ICommandCallback
{
public:
	virtual void CommandCallback(const CCommand &command) = 0;
};

class ICommandCompletionCallback
{
public:
	virtual int CommandCompletionCallback(const char* pPartial, CUtlString_t &commands) = 0;
};

using CVarDLLIdentifier_t = int;

//-----------------------------------------------------------------------------
// Purpose: The base console invoked command/cvar interface
//-----------------------------------------------------------------------------
class ConCommandBase
{
	friend class CCvar;
	friend class ConVar;
	friend class ConCommand;
	friend void ConVar_Register(int nCVarFlag, IConCommandBaseAccessor *pAccessor);

	// FIXME: Remove when ConVar changes are done
	friend class CDefaultCvar;

public:
	ConCommandBase();
	ConCommandBase(const char* pName, const char* pHelpString = 0, int flags = 0);

	virtual                     ~ConCommandBase();
	virtual bool                IsCommand() const;
	virtual bool                IsFlagSet(int flag) const;
	virtual void                AddFlags(int flags);
	virtual void                RemoveFlags(int flags);
	virtual int                 GetFlags() const;
	virtual const char*         GetName() const;
	virtual const char*         GetHelpText() const;
	const ConCommandBase*       GetNext() const;
	ConCommandBase*             GetNext();
	virtual bool                IsRegistered() const;
	virtual CVarDLLIdentifier_t GetDLLIdentifier() const;

	//protected:
	virtual void                Create(const char* pName, const char* pHelpString = 0, int flags = 0);
	virtual void                Init();
	void                        Shutdown();
	char*                       CopyString(const char* from);

	//private:
	// Next ConVar in chain
	// Prior to register, it points to the next convar in the DLL.
	// Once registered, though, m_pNext is reset to point to the next
	// convar in the global list
	ConCommandBase*             m_pNext;
	bool                        m_bRegistered;
	const char*                 m_pszName;
	const char*                 m_pszHelpString;
	int                         m_nFlags;

protected:
	// ConVars add themselves to this list for the executable. 
	// Then ConVar_Register runs through  all the console variables 
	// and registers them into a global list stored in vstdlib.dll
	static ConCommandBase* s_pConCommandBases;

	// ConVars in this executable use this 'global' to access values.
	static IConCommandBaseAccessor* s_pAccessor;

public:
	// This list will hold all the registered commands.
	// It is not from the official SDK. I've added this so that
	// we can parse all convars we have created if we want to
	// save them to a file later on, for example.
	static ConCommandBase* s_pRegisteredCommands;
};

//-----------------------------------------------------------------------------
// Purpose: The console invoked command
//-----------------------------------------------------------------------------
class ConCommand : public ConCommandBase
{
	friend class CCvar;

public:
	using BaseClass = ConCommandBase;

	ConCommand(const char* pName, FnCommandCallbackV1_t callback,
		const char* pHelpString = 0, int flags = 0, FnCommandCompletionCallback completionFunc = 0);
	ConCommand(const char* pName, FnCommandCallback_t callback,
		const char* pHelpString = 0, int flags = 0, FnCommandCompletionCallback completionFunc = 0);
	ConCommand(const char* pName, ICommandCallback *pCallback,
		const char* pHelpString = 0, int flags = 0, ICommandCompletionCallback *pCommandCompletionCallback = 0);

	virtual         ~ConCommand();
	virtual bool    IsCommand() const;
	virtual int     AutoCompleteSuggest(const char* partial, CUtlVector<CUtlString_t> &commands);
	virtual bool    CanAutoComplete();
	virtual void    Dispatch(const CCommand &command);

	//private:
	// NOTE: To maintain backward compat, we have to be very careful:
	// All public virtual methods must appear in the same order always
	// since engine code will be calling into this code, which *does not match*
	// in the mod code; it's using slightly different, but compatible versions
	// of this class. Also: Be very careful about adding new fields to this class.
	// Those fields will not exist in the version of this class that is instanced
	// in mod code.

	// Call this function when executing the command
	union
	{
		FnCommandCallbackV1_t       m_fnCommandCallbackV1;
		FnCommandCallback_t         m_fnCommandCallback;
		ICommandCallback*           m_pCommandCallback;
	};

	union
	{
		FnCommandCompletionCallback m_fnCompletionCallback;
		ICommandCompletionCallback* m_pCommandCompletionCallback;
	};

	bool m_bHasCompletionCallback : 1;
	bool m_bUsingNewCommandCallback : 1;
	bool m_bUsingCommandCallbackInterface : 1;
};


//-----------------------------------------------------------------------------
// Purpose: A console variable
//-----------------------------------------------------------------------------
class ConVar : public ConCommandBase, public IConVar
{
	friend class CCvar;
	friend class ConVarRef;
	friend class SplitScreenConVarRef;

public:
	using BaseClass = ConCommandBase;

	ConVar(const char* pName, const char* pDefaultValue, int flags = 0);

	ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString);
	ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString, bool bMin, float fMin, bool bMax, float fMax);
	ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString, FnChangeCallback_t callback);
	ConVar(const char* pName, const char* pDefaultValue, int flags, const char* pHelpString, bool bMin, float fMin, bool bMax, float fMax, FnChangeCallback_t callback);

	virtual                     ~ConVar();
	virtual bool                IsFlagSet(int flag) const;
	virtual const char*         GetHelpText() const;
	virtual bool                IsRegistered() const;
	virtual const char*         GetName() const;
	virtual const char*         GetBaseName() const;
	virtual int                 GetSplitScreenPlayerSlot() const;

	virtual void                AddFlags(int flags);
	virtual int                 GetFlags() const;
	virtual bool                IsCommand() const;

	// Install a change callback (there shouldn't already be one....)
	void InstallChangeCallback(FnChangeCallback_t callback, bool bInvoke = true);
	void RemoveChangeCallback(FnChangeCallback_t callbackToRemove);

	int GetChangeCallbackCount() const { return m_pParent->m_fnChangeCallbacks.m_Size; }
	FnChangeCallback_t GetChangeCallback(int slot) const { return m_pParent->m_fnChangeCallbacks[slot]; }

	// Retrieve value
	virtual float                   GetFloat() const;
	virtual int                     GetInt() const;
	FORCEINLINE_CVAR Color          GetColor() const;
	FORCEINLINE_CVAR bool           GetBool() const { return !!GetInt(); }
	FORCEINLINE_CVAR char const*    GetString() const;

	// Compiler driven selection for template use
	template <typename T> T Get() const;
	template <typename T> T Get(T*) const;

	// Any function that allocates/frees memory needs to be virtual or else you'll have crashes
	// from alloc/free across dll/exe boundaries.

	// These just call into the IConCommandBaseAccessor to check flags and set the var (which ends up calling InternalSetValue).
	virtual void                    SetValue(const char* value);
	virtual void                    SetValue(float value);
	virtual void                    SetValue(int value);
	virtual void                    SetValue(Color value);

	// Reset to default value
	void                            Revert();
	bool                            HasMin() const;
	bool                            HasMax() const;
	bool                            GetMin(float& minVal) const;
	bool                            GetMax(float& maxVal) const;
	float                           GetMinValue() const;
	float                           GetMaxValue() const;
	const char*                     GetDefault() const;

	struct CVValue_t
	{
		char*   m_pszString;
		int     m_StringLength;
		float   m_fValue;
		int     m_nValue;
	};

	FORCEINLINE_CVAR CVValue_t &GetRawValue()
	{
		return m_Value;
	}
	FORCEINLINE_CVAR const CVValue_t &GetRawValue() const
	{
		return m_Value;
	}

	//private:
	bool                        InternalSetColorFromString(const char* value);
	virtual void                InternalSetValue(const char* value);
	virtual void                InternalSetFloatValue(float fNewValue);
	virtual void                InternalSetIntValue(int nValue);
	virtual void                InternalSetColorValue(Color value);
	virtual bool                ClampValue(float &value);
	virtual void                ChangeStringValue(const char* tempVal, float flOldValue);
	virtual void                Create(const char* pName, const char* pDefaultValue, int flags = 0, const char* pHelpString = 0, bool bMin = false, float fMin = 0.0, bool bMax = false, float fMax = false, FnChangeCallback_t callback = 0);

	// Used internally by OneTimeInit to initialize.
	virtual void                Init();

	//protected:
	ConVar*                     m_pParent;
	const char*                 m_pszDefaultValue;
	CVValue_t                   m_Value;
	bool                        m_bHasMin;
	float                       m_fMinVal;
	bool                        m_bHasMax;
	float                       m_fMaxVal;

	// Call this function when ConVar changes
	CUtlVector<FnChangeCallback_t> m_fnChangeCallbacks;
};


//-----------------------------------------------------------------------------
// Purpose: Return ConVar value as a float
// Output : float
//-----------------------------------------------------------------------------
FORCEINLINE_CVAR float ConVar::GetFloat() const
{
	uint32_t xored = *reinterpret_cast<uint32_t*>(&m_pParent->m_Value.m_fValue) ^ reinterpret_cast<uint32_t>(this);
	return *reinterpret_cast<float*>(&xored);
}

//-----------------------------------------------------------------------------
// Purpose: Return ConVar value as an int
// Output : int
//-----------------------------------------------------------------------------
FORCEINLINE_CVAR int ConVar::GetInt() const
{
	return static_cast<int>(m_pParent->m_Value.m_nValue ^ reinterpret_cast<int>(this));
}

//-----------------------------------------------------------------------------
// Purpose: Return ConVar value as a color
// Output : Color
//-----------------------------------------------------------------------------
FORCEINLINE_CVAR Color ConVar::GetColor() const
{
	int value = GetInt();
	unsigned char* pColorElement = ((unsigned char*)&value);
	return Color(pColorElement[0], pColorElement[1], pColorElement[2], pColorElement[3]);
}


//-----------------------------------------------------------------------------

template <> FORCEINLINE_CVAR float          ConVar::Get<float>() const { return GetFloat(); }
template <> FORCEINLINE_CVAR int            ConVar::Get<int>() const { return GetInt(); }
template <> FORCEINLINE_CVAR bool           ConVar::Get<bool>() const { return GetBool(); }
template <> FORCEINLINE_CVAR const char*    ConVar::Get<const char* >() const { return GetString(); }
template <> FORCEINLINE_CVAR float          ConVar::Get<float>(float *p) const { return (*p = GetFloat()); }
template <> FORCEINLINE_CVAR int            ConVar::Get<int>(int *p) const { return (*p = GetInt()); }
template <> FORCEINLINE_CVAR bool           ConVar::Get<bool>(bool *p) const { return (*p = GetBool()); }
template <> FORCEINLINE_CVAR const char*    ConVar::Get<const char* >(char const **p) const { return (*p = GetString()); }

//-----------------------------------------------------------------------------
// Purpose: Return ConVar value as a string, return "" for bogus string pointer, etc.
// Output : const char* 
//-----------------------------------------------------------------------------
FORCEINLINE_CVAR const char* ConVar::GetString() const
{
	if (m_nFlags & FCVAR_NEVER_AS_STRING)
		return "FCVAR_NEVER_AS_STRING";
	char const *str = m_pParent->m_Value.m_pszString;
	return str ? str : "";
}

//-----------------------------------------------------------------------------
// Called by the framework to register ConCommands with the ICVar
//-----------------------------------------------------------------------------
void ConVar_Register(int nCVarFlag = 0, IConCommandBaseAccessor* pAccessor = NULL);
void ConVar_Unregister();

class IConsoleDisplayFunc
{
public:
	virtual void ColorPrint(const uint8_t* clr, const char* pMessage) = 0;
	virtual void Print(const char* pMessage) = 0;
	virtual void DPrint(const char* pMessage) = 0;
};

class ICvar : public IAppSystem
{
public:
	virtual CVarDLLIdentifier_t        AllocateDLLIdentifier() = 0; // 9
	virtual void                       RegisterConCommand(ConCommandBase *pCommandBase) = 0; // 10
	virtual void                       UnregisterConCommand(ConCommandBase *pCommandBase) = 0; // 11
	virtual void                       UnregisterConCommands(CVarDLLIdentifier_t id) = 0; // 12
	virtual const char*                GetCommandLineValue(const char* pVariableName) = 0; // 13
	virtual ConCommandBase*            FindCommandBase(const char* name) = 0; // 14
	virtual const ConCommandBase*      FindCommandBase(const char* name) const = 0; // 15
	virtual ConVar*                    FindVar(const char* var_name) = 0; // 16
	virtual const ConVar*              FindVar(const char* var_name) const = 0; // 17
	virtual ConCommand*                FindCommand(const char* name) = 0; // 18
	virtual const ConCommand*          FindCommand(const char* name) const = 0; // 19
	virtual void                       InstallGlobalChangeCallback(FnChangeCallback_t callback) = 0; // 20
	virtual void                       RemoveGlobalChangeCallback(FnChangeCallback_t callback) = 0; // 21
	virtual void                       CallGlobalChangeCallbacks(ConVar *var, const char* pOldString, float flOldValue) = 0; // 22
	virtual void                       InstallConsoleDisplayFunc(IConsoleDisplayFunc* pDisplayFunc) = 0; // 23
	virtual void                       RemoveConsoleDisplayFunc(IConsoleDisplayFunc* pDisplayFunc) = 0; // 24
	virtual void                       ConsoleColorPrintf(const uint8_t* clr, const char* pFormat, ...) const = 0; // 25
	virtual void                       ConsolePrintf(const char* pFormat, ...) const = 0; // 26
	virtual void                       ConsoleDPrintf(const char* pFormat, ...) const = 0; // 27
	virtual void                       RevertFlaggedConVars(int nFlag) = 0; // 28
};
