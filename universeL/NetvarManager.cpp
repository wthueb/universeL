#include "NetvarManager.h"

#include <fstream>
#include <utility>

#include "SDK\SDK.h"
#include "XorStr.h"


using namespace std;


#pragma region Helper classes

typedef unordered_map<string, unique_ptr<NetvarTable>>            TableMap;
typedef unordered_map<string, uint32_t>                           PropMap;

typedef unordered_map<string, unique_ptr<NetvarTable>>::iterator  Iter;

struct NetvarTable
{
	TableMap  m_ChildTables;
	PropMap   m_ChildProps;
	uint32_t  m_uOffset = 0;

	bool is_empty()
	{
		return m_ChildTables.size() == 0 && m_ChildProps.size() == 0;
	}

	void insert_table(string &&name, unique_ptr<NetvarTable> &&pTable)
	{
		m_ChildTables.emplace(name, std::move(pTable));
	}

	void insert_prop(string &&name, uint32_t offset)
	{
		m_ChildProps.emplace(name, offset);
	}
};

class NetvarDatabase
{
public:
	TableMap m_Tables;

	void insert(string &&name, unique_ptr<NetvarTable> &&pTable)
	{
		m_Tables.emplace(name, std::move(pTable));
	}

	Iter find(const string &key) { return m_Tables.find(key); }
	
	Iter begin() { return m_Tables.begin(); }
	Iter end() { return m_Tables.end(); }
};

#pragma endregion


NetvarManager* NetvarManager::instance = nullptr;

NetvarManager::NetvarManager()
{

}

NetvarManager::~NetvarManager()
{

}

void NetvarManager::CreateDatabase()
{
	m_pDatabase = make_unique<NetvarDatabase>();
	auto pClient = Interfaces::Client();

	if (pClient)
	{
		for (auto pClass = pClient->GetAllClasses(); pClass; pClass = pClass->m_pNext)
		{
			if (pClass->m_pRecvTable)
			{
				// insert new entry into the database
				m_pDatabase->insert(pClass->m_pRecvTable->m_pNetTableName,
					                InternalLoadTable(pClass->m_pRecvTable, 0));
				m_tableCount++;
			}
		}
	}
}

void NetvarManager::Dump(std::ostream &output)
{
	for (auto &entry : *m_pDatabase)
	{
		auto &pTable = entry.second;

		if (pTable->is_empty())
		{
			continue;
		}

		output << entry.first << std::endl;
		Dump(output, *pTable, 1);
		output << "==============================================================" << std::endl;
	}
}

void NetvarManager::Dump(const std::string &file)
{
	std::ofstream output(file);
	Dump(output);
}

unique_ptr<NetvarTable> NetvarManager::InternalLoadTable(RecvTable* pRecvTable, uint32_t offset)
{
	auto pTable = make_unique<NetvarTable>();
	pTable->m_uOffset = offset;

	for (auto i = 0; i < pRecvTable->m_nProps; ++i)
	{
		auto pProp = &pRecvTable->m_pProps[i];

		// skip shitty array items / baseclass
		if (!pProp || isdigit(pProp->m_pVarName[0]))
		{
			continue;
		}

		if (strcmp(pProp->m_pVarName, "baseclass") == 0)
		{
			continue;
		}

		// if the prop is a table and isn't null and starts with D
		// (there are some shitty tables that don't start with D
		if (pProp->m_RecvType == SendPropType::DPT_DataTable &&
			pProp->m_pDataTable != NULL &&
			pProp->m_pDataTable->m_pNetTableName[0] == 'D')
		{
			// load the table and insert it
			pTable->insert_table(pProp->m_pVarName, InternalLoadTable(pProp->m_pDataTable, pProp->m_Offset));
		}
		else
		{
			// insert the prop
			pTable->insert_prop(pProp->m_pVarName, pProp->m_Offset);
		}

		++m_netvarCount;
	}

	return pTable;
}

void NetvarManager::Dump(std::ostream &output, NetvarTable &table, int level)
{
	char line[512] = "";
	char fmt[512] = "";

	sprintf(fmt, "%%-%ds: 0x%%08X\n", 50 - level * 4);

	for (auto &prop : table.m_ChildProps)
	{
		for (int i = 0; i < level; ++i)
		{
			if (i != level - 1)
			{
				output << "    ";
			}
			else
			{
				output << "|___";
			}
		}

		sprintf(line, fmt, prop.first.c_str(), prop.second + table.m_uOffset);

		output << line;
	}

	for (auto &child : table.m_ChildTables)
	{
		for (int i = 0; i < level; ++i)
		{
			if (i != level - 1)
			{
				output << "    ";
			}
			else
			{
				output << "|___";
			}
		}

		sprintf(line, fmt, child.first.c_str(), child.second->m_uOffset);

		output << line;

		Dump(output, *child.second, level + 1);
	}
}

uint32_t NetvarManager::GetOffset(const std::string &szTableName, const std::initializer_list<std::string> &props)
{
	auto table = m_pDatabase->find(szTableName);
	if (table == m_pDatabase->end())
	{
		return -1;
	}

	int tableOffset = table->second->m_uOffset;
	if (props.size() == 0)
	{
		return tableOffset;
	}

	int totalOffset = tableOffset;

	NetvarTable* curTable = table->second.get();

	for (size_t i = 0; i < props.size(); ++i)
	{
		std::string propName = *(props.begin() + i);

		// if not the last index
		if (i + 1 < props.size())
		{
			auto childTable = curTable->m_ChildTables.find(propName);
			if (childTable == curTable->m_ChildTables.end())
			{
				throw std::runtime_error("Prop not found");
			}
			totalOffset += childTable->second->m_uOffset;

			curTable = childTable->second.get();
		}
		else
		{
			auto childProp = curTable->m_ChildProps.find(propName);
			if (childProp == curTable->m_ChildProps.end())
			{
				throw std::runtime_error("Prop not found");
			}
			totalOffset += childProp->second;
		}
	}

	return totalOffset;
}
