#include "ClantagChanger.h"

#include "Options.h"
#include "Utils.h"
#include "XorStr.h"

#include <string>
#include <algorithm>

#include "SDK\SDK.h"

class StringBanner
{
public:
	StringBanner(std::string str)
	{
		m_str = str.append(std::string(str.length(), ' '));
	}

	std::string next()
	{
		std::rotate(m_str.begin(), m_str.begin() + 1, m_str.end());

		return m_str;
	}

private:
	std::string m_str;
};

void ClantagChanger::SetClanTag()
{
	if (!Interfaces::Engine()->IsInGame())
		return;

	using SetClanTagFn = void(__fastcall*)(const char*, const char*);
	
	static SetClanTagFn SetClanTag =
		reinterpret_cast<SetClanTagFn>(Utils::FindSignature(XorStr("engine.dll"), XorStr("53 56 57 8B DA 8B F9 FF 15 ? ? ? ? 6A 24 8B C8 8B 30")));

	if (Options::Misc::strClantag.empty()) // is empty
		return;

	if (Options::Misc::bClantagScroll)
	{
		static StringBanner bannertag(Options::Misc::strClantag);

		static std::string oldstring = Options::Misc::strClantag;

		if (Options::Misc::strClantag.compare(oldstring)) // if they are different
		{
			bannertag = StringBanner(Options::Misc::strClantag);
			oldstring = Options::Misc::strClantag;
		}

		static int counter = 0;

		if (counter % 100 == 0)
		{
			SetClanTag(bannertag.next().c_str(), "");
			counter = 0;
		}

		++counter;
	}
	else
		SetClanTag(Options::Misc::strClantag.c_str(), "");
}
