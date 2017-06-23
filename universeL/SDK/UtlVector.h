#pragma once

#include "UtlMemory.h"

template<class T, class A = CUtlMemory<T>>
class CUtlVector
{
public:
	typedef A CAllocator;

	T& operator[](int i) {
		return m_Memory[i];
	};

	CAllocator m_Memory;
	int m_Size;
	T* m_pElements;
};
