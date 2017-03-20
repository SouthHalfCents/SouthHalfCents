#pragma once
#include "Database.h"
#include <string>

//������
class CUtils
{
public:
	CUtils();
	~CUtils();

	static int TranDayweekToInt(int nDayofWeek, int & nDayValue);			//��dayofweekתΪDAY_MONDAY��ֵ

	static	CString 	GetAppDir();								//���Ӧ�ó���ĵ�ǰĿ¼·��
	static	CString 	FileNameFromPath(CString strPath);			//���ļ�·����ȡ�ļ�����

	static	BOOL    	FileExist(LPCTSTR   pszFileName);			//����ļ��Ƿ����
	static	BOOL		IsFileType(LPCTSTR lpsz, LPCTSTR lpszType); //����ļ��Ƿ�Ϊָ�����ͣ�lpszTypeΪ��׺����

	static	std::string	GBK_UTF8(string& strMsg);///< ��GBK�ַ���ת��ΪUTF8����
	static	std::string	GBK_UTF8(char* strMsg);
	static	std::string	UTF8_GBK(string& strMsg);///< ��UTF8����ת��ΪGBK����
	static	std::string	UTF8_GBK(char* strMsg);
};

