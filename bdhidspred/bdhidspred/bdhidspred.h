
#if !defined(AFX_BDHIDSPRED_H__7E6AAC0C_890C_4AE7_A9C9_10C4C2D39D31__INCLUDED_)
#define AFX_BDHIDSPRED_H__7E6AAC0C_890C_4AE7_A9C9_10C4C2D39D31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

extern void SleeperFunc(DWORD dwMilliseconds);
extern BOOL FileExists(char * szFile);
extern void ProcessMessages();
extern void DebugView(const char* pszFormat, ...);

#endif // !defined(AFX_BDHIDSPRED_H__7E6AAC0C_890C_4AE7_A9C9_10C4C2D39D31__INCLUDED_)
