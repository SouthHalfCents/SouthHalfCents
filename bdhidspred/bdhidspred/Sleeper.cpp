
#include "stdafx.h"
#include "Sleeper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSleeper::CSleeper()
{
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CSleeper::~CSleeper()
{
	CloseHandle(m_hEvent);
}

void CSleeper::Sleep(DWORD dwMilliseconds)
{
	WaitForSingleObject(m_hEvent, dwMilliseconds);
}
