
#include "stdafx.h"
#include "dllmain.h"



ANGO_PLUGIN_API int GetModuleVer(void)
{
	return ANGO_CURRENT_MODULE_VER;
}

ANGO_PLUGIN_API TCHAR * GetModuleName(void)
{
	return ANGO_CURRENT_MODULE_NAME;
}

//---------------------------------------------------------------------------------------------------------------------
ANGO_PLUGIN_API int AngoMessageBox(CString strMsg, CString strTitle/* =L"" */, UINT nType/* =MB_POST_BESIDE | MB_OK */)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CMsgBoxEx * pMsgBox = new CMsgBoxEx;
	pMsgBox->InitMsgBox(strMsg, strTitle, nType);
	int nReturn = (int)pMsgBox->DoModal();
	delete pMsgBox;

	return nReturn;
}
//---------------------------------------------------------------------------------------------------------------------