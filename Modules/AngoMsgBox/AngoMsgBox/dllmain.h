
#include "AngoMsgBox.h"
#include "MsgBoxEx.h"

#define ANGO_PLUGIN_EXPORTS
#define ANGO_CURRENT_MODULE_VER		0x0100
#define ANGO_CURRENT_MODULE_NAME	_T("AngoMsgBox")


#ifdef ANGO_PLUGIN_EXPORTS
#define ANGO_PLUGIN_API __declspec(dllexport)
#else
#define ANGO_PLUGIN_API __declspec(dllimport)
#endif

#ifdef __cplusplus 
extern "C"
{
#endif


	extern ANGO_PLUGIN_API int GetModuleVer(void);
	extern ANGO_PLUGIN_API TCHAR * GetModuleName(void);
	extern ANGO_PLUGIN_API int AngoMessageBox(CString strMsg, CString strTitle=_T(""), UINT nType= MB_POST_BESIDE | MB_OK);

#ifdef __cplusplus 
}
#endif