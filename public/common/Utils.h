#pragma once

#include <afxwin.h>
#include <string>

#define DAY_NONE		0x00000000
#define DAY_MONDAY		0x00000001
#define DAY_TUESDAY		0x00000002
#define DAY_WEDNESDAY	0x00000004
#define DAY_THURSDAY	0x00000008
#define DAY_FRIDAY		0x00000010
#define DAY_SATURDAY	0x00000020
#define DAY_SUNDAY		0x00000040


//������
class CUtils
{
public:
	CUtils();
	~CUtils();

	static int			TranDayweekToInt(int nDayofWeek, int & nDayValue);			//��dayofweekתΪDAY_MONDAY��ֵ

	static CString		GetAppPath();								//��ȡӦ�ó���ȫ·�������ļ���
	static CString 		GetAppDir();								//���Ӧ�ó���ĵ�ǰĿ¼·��
	static CString 		FileNameFromPath(CString strPath);			//���ļ�·����ȡ�ļ�����

	static BOOL    		FileExist(LPCTSTR   pszFileName);			//����ļ��Ƿ����
	static BOOL			IsFileType(LPCTSTR lpsz, LPCTSTR lpszType); //����ļ��Ƿ�Ϊָ�����ͣ�lpszTypeΪ��׺����

	static std::string	GBK_UTF8(std::string& strMsg);///< ��GBK�ַ���ת��ΪUTF8����
	static std::string	GBK_UTF8(char* strMsg);
	static std::string	UTF8_GBK(std::string& strMsg);///< ��UTF8����ת��ΪGBK����
	static std::string	UTF8_GBK(char* strMsg);

	static BOOL			IsWow64();						//�ж��Ƿ�64λϵͳ
	static BOOL			SetRegAutoStart(BOOL bEnable, CString strKeyName, CString strKeyValue);

	static BOOL			IsRunAsAdmin();					//�Ƿ��Թ���ԱȨ��������
	static BOOL			IsInAdminGroup(BOOL bCheckAdminMode = FALSE);//���bCheckAdminMode��TRUE�� ����˼��Admin�˺��⣬����������������Admin������ ����ֻ�Ǽ��Admin�˺š�
	static BOOL			EnablePrivilege(HANDLE hToken, LPCTSTR lpszPrivilegeName);		//��������Ȩ��
	static BOOL			UpPrivilege();
};

