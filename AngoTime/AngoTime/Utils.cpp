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

///< 将GBK字符串转化为UTF8编码
string	CUtils::GBK_UTF8(string& strMsg)
{
	return GBK_UTF8((char*)strMsg.c_str());
}

string	CUtils::GBK_UTF8(char* strMsg)
{
	if (strMsg == NULL)
	{
		return "";
	}

	string strOutUTF8 = "";

	int nWsize = MultiByteToWideChar(CP_ACP, 0, strMsg, -1, NULL, 0);
	WCHAR * strW = new WCHAR[nWsize + 1];
	strW[nWsize] = 0;
	MultiByteToWideChar(CP_ACP, 0, strMsg, -1, strW, nWsize);
	int nMSize = WideCharToMultiByte(CP_UTF8, 0, strW, -1, NULL, 0, NULL, NULL);
	char * strM = new char[nMSize + 1];
	strM[nMSize] = 0;
	WideCharToMultiByte(CP_UTF8, 0, strW, -1, strM, nMSize, NULL, NULL);
	strOutUTF8 = strM;
	delete[]strW;
	delete[]strM;
	return strOutUTF8;
}
///< 将UTF8编码转化为GBK编码
string	CUtils::UTF8_GBK(string& strMsg)
{
	return UTF8_GBK((char*)strMsg.c_str());
}

string	CUtils::UTF8_GBK(char* strMsg)
{
	if (strMsg == NULL)
	{
		return "";
	}

	string strOutGBK = "";

	int nWsize = MultiByteToWideChar(CP_UTF8, 0, strMsg, -1, NULL, 0);
	WCHAR * strW = new WCHAR[nWsize + 1];
	strW[nWsize] = 0;
	MultiByteToWideChar(CP_UTF8, 0, strMsg, -1, strW, nWsize);
	int nMSize = WideCharToMultiByte(CP_ACP, 0, strW, -1, NULL, 0, NULL, NULL);
	char * strM = new char[nMSize + 1];
	strM[nMSize] = 0;
	WideCharToMultiByte(CP_ACP, 0, strW, -1, strM, nMSize, NULL, NULL);
	strOutGBK = strM;
	delete[]strW;
	delete[]strM;
	return strOutGBK;
}
