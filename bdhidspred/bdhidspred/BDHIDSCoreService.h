// BDHIDSCoreService.h: interface for the CBDHIDSCoreService class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BDHIDSCORESERVICE_H__4B1BC9EA_E011_4908_B169_650D8D1E6CD2__INCLUDED_)
#define AFX_BDHIDSCORESERVICE_H__4B1BC9EA_E011_4908_B169_650D8D1E6CD2__INCLUDED_

#include "BDHIDSService.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define MAX_STRING 1024 


class CBDHIDSCoreService  : public CBDHIDSService
{
public:
	CBDHIDSCoreService();
	virtual ~CBDHIDSCoreService();

	virtual void Run();
	virtual void OnStop();
	virtual BOOL OnUserControl(DWORD dwOpcode);
	// 	int SNAREDEBUG;
	void OnShutdown();
	virtual BOOL OnInit();

	
	
protected:
	DWORD m_dwThreadID;
	



private:
	
	
 	
};

#endif // !defined(AFX_BDHIDSCORESERVICE_H__4B1BC9EA_E011_4908_B169_650D8D1E6CD2__INCLUDED_)


