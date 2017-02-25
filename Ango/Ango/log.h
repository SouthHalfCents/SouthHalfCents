#pragma once

#include <mutex>          // std::mutex
#include <fstream>
#include <string>

using namespace std;

/////////////////////////// ��־���� ////////////////////////////

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
	static mutex					m_Mutex;				// �Զ����ٽ�������
	static CLog* volatile			m_pInstance;			// The one and only instance, ��volatile�ؼ��ʷ�ֹ�������Ż�
	static ofstream					m_Fout;
	string							m_strFileName;
public:
	static CLog*					GetInstance(void);							//��ȡ��������
	bool							WriteLog(char szMsg[], int nError = LOG_LEVEL_INFO);	
};
//------------------------------------------------------------------------------------------
