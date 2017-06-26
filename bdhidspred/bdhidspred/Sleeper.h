
#if !defined(AFX_SLEEPER_H_INCLUDED_)
#define AFX_SLEEPER_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSleeper
{
public:
				CSleeper();
				~CSleeper();

	void		Sleep(DWORD dwMilliseconds = 1000);

private:
	HANDLE		m_hEvent;
};

#endif