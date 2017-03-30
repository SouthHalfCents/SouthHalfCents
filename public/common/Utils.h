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


//工具类
class CUtils
{
public:
	CUtils();
	~CUtils();

	static int			TranDayweekToInt(int nDayofWeek, int & nDayValue);			//把dayofweek转为DAY_MONDAY的值

	static CString		GetAppPath();								//获取应用程序全路径包括文件名
	static CString 		GetAppDir();								//获得应用程序的当前目录路径
	static CString 		FileNameFromPath(CString strPath);			//从文件路径提取文件名称

	static BOOL    		FileExist(LPCTSTR   pszFileName);			//检查文件是否存在
	static BOOL			IsFileType(LPCTSTR lpsz, LPCTSTR lpszType); //检查文件是否为指定类型（lpszType为后缀名）

	static std::string	GBK_UTF8(std::string& strMsg);///< 将GBK字符串转化为UTF8编码
	static std::string	GBK_UTF8(char* strMsg);
	static std::string	UTF8_GBK(std::string& strMsg);///< 将UTF8编码转化为GBK编码
	static std::string	UTF8_GBK(char* strMsg);

	static BOOL			IsWow64();						//判断是否64位系统
	static BOOL			SetRegAutoStart(BOOL bEnable, CString strKeyName, CString strKeyValue);

	static BOOL			IsRunAsAdmin();					//是否以管理员权限运行中
	static BOOL			IsInAdminGroup(BOOL bCheckAdminMode = FALSE);//如果bCheckAdminMode是TRUE， 则除了检测Admin账号外，还检测是真的运行在Admin环境， 否则只是检测Admin账号。
	static BOOL			EnablePrivilege(HANDLE hToken, LPCTSTR lpszPrivilegeName);		//按项提升权限
	static BOOL			UpPrivilege();
};

