#pragma once
#ifndef _DATA_BASE_H_
#define _DATA_BASE_H_


#include <mutex>          // std::mutex
#include <list>
#include "MsgBoxEx.h"

using namespace std;






#define PI 3.1415926535897932384626433832795028841971693993751058209

//��ʱ����
#define SAYTIME_CLOSE	0
#define SAYTIME_ALL		1
#define SAYTIME_HALF	2


//ע�����Ϣsection��entry
#define ANGO_SECTION_SET		_T("settings")	//��������Ŀ¼��
#define ANGO_SECTION_CLOCK		_T("clocks")	//����Ŀ¼��
#define ANGO_SECTION_TASK		_T("tasks")		//����Ŀ¼��

#define CLOCK_MUSIC			_T("clock_music")
#define SAYTIME_ENTRY		_T("saytime")
#define CLOCK_ENTRY			_T("clock")
#define AUTORUN_ENTRY		_T("autorun")
#define RECENT_FILE_PATH	_T("RecentFilePath")

#define ANGO_CLOCK_NAME		_T("clock_name")	//���ӻ�����
#define ANGO_TASK_NAME		_T("task_name")		//���������

//ִ��ʱ��Ƶ��
#define TIMES_ONCE		0
#define TIMES_DAY		1
#define TIMES_WEEK		2
#define TIMES_MONTH		3
#define TIMES_YEAR		4




//�˵���Ӧλ��
#define MENU_NO_FIRST	0	//�˵���һ��

#define MENU_VIEW		0	//��һ�еĵ�һ��λ��
#define MENU_CLOCK		5
#define MENU_SAYTIME	6
#define MENU_QUICK_OP	7
#define MENU_CONFIG		8

//id��Ӧ�ַ���
typedef struct tagNumStrEx
{
	int		nValue;
	CString	strValue;
} num_string_t;

static num_string_t g_Clock_Freq[] =
{
	{ TIMES_ONCE, _T("һ��") },
	{ TIMES_DAY, _T("ÿ��") },
	{ TIMES_WEEK, _T("ÿ��") }
};




extern HANDLE				g_hThread_Alarm;
extern HANDLE				g_hThread_SayTime;
extern HANDLE				g_hThread_CusJob;
extern HANDLE				g_hThread_Clock;

extern HANDLE				g_hSemaph_Clock;		//ʱ��
extern HANDLE				g_hSemaph_Alarm;		//����
extern HANDLE				g_hSemaph_SayTime;		//��ʱ
extern HANDLE				g_hSemaph_CusJob;		//��ʱ����

extern BOOL					g_bWork;

unsigned int __stdcall		Thread_Alarm(LPVOID pParam);
unsigned int __stdcall		Thread_SayTime(LPVOID pParam);
unsigned int __stdcall		Thread_CusJob(LPVOID pParam);
unsigned int __stdcall		Thread_Clock(LPVOID pParam);					// ִ��ʱ�Ӳ���

void						StartWork(void *pThis);
void						StopWork();
extern mutex				g_MutexSound;			//������

//��ʱ������ָ�� 
typedef void(*TASKFUN)(void * pData);
typedef struct tagTaskExe
{
	TASKFUN		taskFun;
	int			nTaskType;
	void*		pParameter;
}TASKEXE;
extern list<TASKEXE>		g_TaskExeList;			//��ִ������
extern mutex				g_MutexTaskList;		//������

//extern mutex				gMutexClockList;		//������

/*
CWinApp�����ṩ��һ�����ڶ�дӦ�ó������õķ�����
GetProfileInt
WriteProfileInt
GetProfileString
WriteProfileString
�ɷ�������ڶ�дӦ�ó������á�


SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
�ú�����Ϊ�����ᵽ�ļ�������������������,��ʱ�����WriteProfileIntд�����ݣ�����
��д�뵽����ע���λ�ã�
HKEY_CURRENT_USER\Software\Ӧ�ó��������ɵı���Ӧ�ó���\Ӧ�ó�������\
�����InitInstance�в�ִ��SetRegistryKey������WriteProfileIntд������ʱ����д�뵽
%windir%\Ӧ�ó�������.ini�С�
*/


/*
��ȡ�����ļ�
GetPrivateProfileString

*/

#endif	// _DATA_BASE_H_