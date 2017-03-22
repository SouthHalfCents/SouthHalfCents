
#include "stdafx.h"
#include "log.h"
#include <time.h>

//------------------------------------------------------------------------------------------
CLog* volatile CLog::m_pInstance = NULL;
mutex CLog::m_Mutex;
ofstream CLog::m_Fout;
//------------------------------------------------------------------------------------------
CLog::CLog(void) 
{
	
	m_strFileName = "运行日志.log";
}
//------------------------------------------------------------------------------------------
CLog::~CLog(void)
{
}
//------------------------------------------------------------------------------------------
void CLog::Destroy()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;

		if (!m_Fout.is_open())
		{
			m_Fout.close();
		}
	}
}
//------------------------------------------------------------------------------------------
CLog* CLog::GetInstance()
{
	if (NULL == m_pInstance)
	{
		m_Mutex.lock();
		if (NULL == m_pInstance)
		{
			m_pInstance = new CLog();
			atexit(CLog::Destroy);		// Register Destroy function

		}
		m_Mutex.unlock();
	}
	return m_pInstance;
}
//------------------------------------------------------------------------------------------
bool CLog::WriteLog(char szMsg[], int nError /* = LOG_LEVEL_INFO */)
{
	m_Mutex.lock();
	if (!m_Fout.is_open())
	{
		m_Fout.open(m_strFileName.c_str(), ios::out | ios::app);
	}

	string strType = "         ";
	switch (nError)
	{
	case LOG_LEVEL_DEBUG:
		strType = "DEBUG:    ";
		break;
	case LOG_LEVEL_INFO:
		strType = "INFO :    ";
		break;
	case LOG_LEVEL_WARN:
		strType = "WARN :    ";
		break;
	case LOG_LEVEL_ERROR:
		strType = "ERROR:    ";
		break;
	case LOG_LEVEL_FATAL:
		strType = "FATAL:    ";
		break;
	default:
		break;
	}

	if (m_Fout.is_open())
	{
		time_t tTime = time(NULL);			//获取日历时间  	
		struct tm tmCurrent;
		localtime_s(&tmCurrent, &tTime);	//获取当前系统时间  
		char szTime[MAX_PATH] = {0};
		ctime_s(szTime, MAX_PATH, &tTime) ; 

		m_Fout <<szTime <<strType <<szMsg<<endl; 
		m_Fout.flush();
	}

	m_Mutex.unlock();
	return true;
}
//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------