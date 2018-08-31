#pragma once
#ifndef _CSGUARD_H_  
#define _CSGUARD_H_

// #include <Windows.h> 

class CSGuard
{
public:

	CSGuard(CRITICAL_SECTION* pcs) : m_pcs(NULL)
	{
		m_pcs = pcs;
		if (!m_pcs)
		{
			EnterCriticalSection(m_pcs);

		}
	}
	~CSGuard()
	{
		if (!m_pcs)
		{
			LeaveCriticalSection(m_pcs);

		}
	}
private:
	CRITICAL_SECTION* m_pcs;
};

#endif

