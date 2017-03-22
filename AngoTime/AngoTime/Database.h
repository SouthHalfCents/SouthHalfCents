#pragma once
#ifndef _DATA_BASE_H_
#define _DATA_BASE_H_


#include <mutex>          // std::mutex
#include <list>
#include "MsgBoxEx.h"

using namespace std;






#define PI 3.1415926535897932384626433832795028841971693993751058209

//报时策略
#define SAYTIME_CLOSE	0
#define SAYTIME_ALL		1
#define SAYTIME_HALF	2


//注册表信息section和entry
#define ANGO_SECTION_SET		_T("settings")	//基础设置目录名
#define ANGO_SECTION_CLOCK		_T("clocks")	//闹钟目录名
#define ANGO_SECTION_TASK		_T("tasks")		//任务目录名

#define CLOCK_MUSIC			_T("clock_music")
#define SAYTIME_ENTRY		_T("saytime")
#define CLOCK_ENTRY			_T("clock")
#define AUTORUN_ENTRY		_T("autorun")
#define RECENT_FILE_PATH	_T("RecentFilePath")

#define ANGO_CLOCK_NAME		_T("clock_name")	//闹钟基础名
#define ANGO_TASK_NAME		_T("task_name")		//任务基础名

//执行时间频率
#define TIMES_ONCE		0
#define TIMES_DAY		1
#define TIMES_WEEK		2
#define TIMES_MONTH		3
#define TIMES_YEAR		4




//菜单对应位置
#define MENU_NO_FIRST	0	//菜单第一列

#define MENU_VIEW		0	//第一列的第一个位置
#define MENU_CLOCK		5
#define MENU_SAYTIME	6
#define MENU_QUICK_OP	7
#define MENU_CONFIG		8

//id对应字符串
typedef struct tagNumStrEx
{
	int		nValue;
	CString	strValue;
} num_string_t;

static num_string_t g_Clock_Freq[] =
{
	{ TIMES_ONCE, _T("一次") },
	{ TIMES_DAY, _T("每天") },
	{ TIMES_WEEK, _T("每周") }
};




extern HANDLE				g_hThread_Alarm;
extern HANDLE				g_hThread_SayTime;
extern HANDLE				g_hThread_CusJob;
extern HANDLE				g_hThread_Clock;

extern HANDLE				g_hSemaph_Clock;		//时钟
extern HANDLE				g_hSemaph_Alarm;		//闹钟
extern HANDLE				g_hSemaph_SayTime;		//报时
extern HANDLE				g_hSemaph_CusJob;		//定时任务

extern BOOL					g_bWork;

unsigned int __stdcall		Thread_Alarm(LPVOID pParam);
unsigned int __stdcall		Thread_SayTime(LPVOID pParam);
unsigned int __stdcall		Thread_CusJob(LPVOID pParam);
unsigned int __stdcall		Thread_Clock(LPVOID pParam);					// 执行时钟操作

void						StartWork(void *pThis);
void						StopWork();
extern mutex				g_MutexSound;			//声音锁

//定时任务函数指针 
typedef void(*TASKFUN)(void * pData);
typedef struct tagTaskExe
{
	TASKFUN		taskFun;
	int			nTaskType;
	void*		pParameter;
}TASKEXE;
extern list<TASKEXE>		g_TaskExeList;			//待执行任务
extern mutex				g_MutexTaskList;		//任务锁

//extern mutex				gMutexClockList;		//闹钟锁

/*
CWinApp类中提供了一组用于读写应用程序配置的方法：
GetProfileInt
WriteProfileInt
GetProfileString
WriteProfileString
可方便的用于读写应用程序配置。


SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
该函数将为以上提到的几个方法建立工作环境,此时如果用WriteProfileInt写入数据，将会
被写入到如下注册表位置：
HKEY_CURRENT_USER\Software\应用程序向导生成的本地应用程序\应用程序名称\
如果在InitInstance中不执行SetRegistryKey，则用WriteProfileInt写入数据时，将写入到
%windir%\应用程序名称.ini中。
*/


/*
读取配置文件
GetPrivateProfileString

*/

#endif	// _DATA_BASE_H_