#pragma once
#include "Database.h"

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
};

