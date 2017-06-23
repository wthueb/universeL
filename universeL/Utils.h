#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <codecvt>
#include <sstream>

namespace Utils
{
	static uint64_t FindSignature(const char* szModule, const char* szSignature)
	{
		//CREDITS: learn_more
#define INRANGE(x,a,b)  (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

		MODULEINFO modInfo;
		GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(szModule), &modInfo, sizeof(MODULEINFO));
		DWORD startAddress = (DWORD)modInfo.lpBaseOfDll;
		DWORD endAddress = startAddress + modInfo.SizeOfImage;
		const char* pat = szSignature;
		DWORD firstMatch = 0;
		for (DWORD pCur = startAddress; pCur < endAddress; pCur++)
		{
			if (!*pat) return firstMatch;
			if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat))
			{
				if (!firstMatch) firstMatch = pCur;
				if (!pat[2]) return firstMatch;
				if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
				else pat += 2;    //one ?
			}
			else
			{
				pat = szSignature;
				firstMatch = 0;
			}
		}
		return NULL;
	}

	static std::wstring StringToWstring(std::string str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

		try
		{
			return converter.from_bytes(str);
		}
		catch (std::range_error)
		{
			std::wostringstream s;
			s << str.c_str();
			return s.str();
		}
	}
}
