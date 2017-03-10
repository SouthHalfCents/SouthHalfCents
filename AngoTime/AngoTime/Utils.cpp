#include "stdafx.h"
#include "Utils.h"


CUtils::CUtils()
{
}


CUtils::~CUtils()
{
}

int CUtils::TranDayweekToInt(int nDayofWeek, int & nDayValue)
{
	if (nDayofWeek <= 0 || nDayofWeek > 7)
	{
		nDayValue = 0;
		return nDayValue;
	}

	switch (nDayofWeek)
	{
	case 1:
		nDayValue = DAY_SUNDAY;
		break;
	case 2:
		nDayValue = DAY_MONDAY;
		break;
	case 3:
		nDayValue = DAY_TUESDAY;
		break;
	case 4:
		nDayValue = DAY_WEDNESDAY;
		break;
	case  5:
		nDayValue = DAY_THURSDAY;
		break;
	case  6:
		nDayValue = DAY_FRIDAY;
		break;
	case 7:
		nDayValue = DAY_SATURDAY;
		break;

	default:
		nDayValue = DAY_NONE;
		break;
	}

	return nDayValue;
}

CString CUtils::FileNameFromPath(CString strPath)
{
	CString filename;

	int pos = strPath.ReverseFind('\\');
	if (pos == -1)
		return strPath;

	filename = strPath.Mid(pos + 1);
	return filename;
}

BOOL CUtils::FileExist(LPCTSTR   pszFileName)
{
	if (GetFileAttributes(pszFileName) == -1)
	{
		return FALSE;
	}
	return TRUE;
}



CString CUtils::GetAppDir()
{
	TCHAR szLocalPath[MAX_PATH];
	GetModuleFileName(0, szLocalPath, MAX_PATH);
	CString strTemp;
	strTemp = szLocalPath;
	strTemp = strTemp.Left(strTemp.ReverseFind('\\'));
	return strTemp;
}

BOOL CUtils::IsFileType(LPCTSTR lpsz, LPCTSTR lpszType)
{
	CString strType = lpszType;
	CString str = lpsz;
	if (str.GetLength() >= 5){
		int nFind = str.ReverseFind('.');
		if (nFind != -1) {

			str = str.Right(str.GetLength() - nFind);
			str.MakeLower();

			if (str == strType){
				return TRUE;
			}

		}
	}
	return FALSE;
}