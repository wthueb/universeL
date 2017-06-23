#pragma once

#include "Vector.h"
#include "UtlVector.h"

#define END_OF_FREE_LIST -1
#define ENTRY_IN_USE -2

class C_BaseEntity;

struct GlowObjectDefinition_t
{
	C_BaseEntity* m_pEntity;
	Vector m_vecGlowColor;
	float m_flGlowAlpha;
	char _unknown[4];
	float _flUnk;
	float m_flBloomAmount;
	float _flUnk1;
	bool m_bRenderWhenOccluded;
	bool m_bRenderWhenUnoccluded;
	bool m_bFullBloomRender;
	char _unknown1;
	int m_nFullBloomStencilTestValue;
	int m_nGlowStyle;
	int m_nSplitScreenSlot;
	int m_nNextFreeSlot;

	bool IsUnused() const {
		return m_nNextFreeSlot != ENTRY_IN_USE;
	}
};

class CGlowObjectManager
{
public:
	int RegisterGlowObject(C_BaseEntity* entity)
	{
		// FIXME: No remaining slots, see issue #158.
		if (m_nFirstFreeSlot == END_OF_FREE_LIST)
			return -1;

		int index = m_nFirstFreeSlot;
		m_nFirstFreeSlot = m_GlowObjectDefinitions[index].m_nNextFreeSlot;

		m_GlowObjectDefinitions[index].m_pEntity = entity;
		m_GlowObjectDefinitions[index]._flUnk = 0.0f;
		m_GlowObjectDefinitions[index]._flUnk1 = 0.0f;
		m_GlowObjectDefinitions[index].m_bFullBloomRender = false;
		m_GlowObjectDefinitions[index].m_nFullBloomStencilTestValue = 0;
		m_GlowObjectDefinitions[index].m_nSplitScreenSlot = -1;
		m_GlowObjectDefinitions[index].m_nNextFreeSlot = ENTRY_IN_USE;

		return index;
	}

	void UnregisterGlowObject(int index)
	{
		m_GlowObjectDefinitions[index].m_nNextFreeSlot = m_nFirstFreeSlot;
		m_GlowObjectDefinitions[index].m_pEntity = NULL;
		m_nFirstFreeSlot = index;
	}

	CUtlVector<GlowObjectDefinition_t> m_GlowObjectDefinitions;
	int m_nFirstFreeSlot;
};
