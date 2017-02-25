#pragma once

#include <mutex>          // std::mutex
#include <fstream>
#include <string>

using namespace std;

/////////////////////////// 日志管理 ////////////////////////////

#define LOG_LEVEL_DEBUG		0
#define LOG_LEVEL_INFO		1
#define LOG_LEVEL_WARN		2
#define LOG_LEVEL_ERROR		3
#define LOG_LEVEL_FATAL		4


//------------------------------------------------------------------------------------------
class CLog
{
private:
	CLog(void);
	CLog(const CLog&);										//Not Implemented
	CLog & operator=(const CLog&);							//Not Implemented
	~CLog(void);
	static void						Destroy();				// Destroy the only instance 
	static mutex					m_Mutex;				// 自定义临界区对象
	static CLog* volatile			m_pInstance;			// The one and only instance, 加volatile关键词防止编译器优化
	static ofstream					m_Fout;
	string							m_strFileName;
public:
	static CLog*					GetInstance(void);							//获取单例对象
	bool							WriteLog(char szMsg[], int nError = LOG_LEVEL_INFO);	
};
//------------------------------------------------------------------------------------------
