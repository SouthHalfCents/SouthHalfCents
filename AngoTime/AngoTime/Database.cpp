#include "stdafx.h"
#include "Database.h"

#include "resource.h"
//playsound
#include <mmsystem.h>
#pragma comment( lib, "Winmm.lib" )


HANDLE			g_hThread_Alarm		=	NULL;
HANDLE			g_hThread_SayTime	=	NULL;
HANDLE			g_hThread_CusJob	=	NULL;

HANDLE			g_hSemaph_Alarm		=	NULL;	//闹钟
HANDLE			g_hSemaph_SayTime	=	NULL;	//报时
HANDLE			g_hSemaph_CusJob	=	NULL;	//定时任务

mutex			g_MutexSound;
BOOL			g_bWork = FALSE;

mutex			g_MutexTaskList;
list<TASKEXE>	g_TaskExeList;

//-------------------------------------------------------------------------------------------------------------------------
void InitThread()
{
	if ( g_hThread_Alarm || g_hThread_SayTime || g_hThread_CusJob )
		return;

	g_bWork = TRUE;

	//创建信号量
	g_hSemaph_Alarm		= CreateSemaphore(NULL, 0, 2, _T("AngoTime_Alarm"));
	g_hSemaph_SayTime	= CreateSemaphore(NULL, 0, 2, _T("AngoTime_SayTime"));
	g_hSemaph_CusJob	= CreateSemaphore(NULL, 0, 8, _T("AngoTime_CusJob"));
	/*
	第一个参数：安全属性，如果为NULL则是默认安全属性 
	第二个参数：信号量的初始值，要>=0且<=第三个参数
	第三个参数：信号量的最大值
	第四个参数：信号量的名称
	*/
	

	//创建线程
	g_hThread_Alarm		= (HANDLE)_beginthreadex(NULL, 0, Thread_Alarm, NULL, 0, NULL);
	g_hThread_SayTime	= (HANDLE)_beginthreadex(NULL, 0, Thread_SayTime, NULL, 0, NULL);
	g_hThread_CusJob	= (HANDLE)_beginthreadex(NULL, 0, Thread_CusJob, NULL, 0, NULL);

	
	
}

void CleanThread()
{
	g_bWork = FALSE;

	ReleaseSemaphore(g_hSemaph_Alarm, 1, NULL);		//释放1个信号量
	ReleaseSemaphore(g_hSemaph_SayTime, 1, NULL);	//释放1个信号量
	ReleaseSemaphore(g_hSemaph_CusJob, 1, NULL);	//释放1个信号量

	WaitForSingleObject(g_hThread_Alarm, INFINITE);
	WaitForSingleObject(g_hThread_SayTime, INFINITE);
	WaitForSingleObject(g_hThread_CusJob, INFINITE);

	CloseHandle(g_hThread_Alarm);
	CloseHandle(g_hThread_SayTime);
	CloseHandle(g_hThread_CusJob);

	CloseHandle(g_hSemaph_Alarm);
	CloseHandle(g_hSemaph_SayTime);
	CloseHandle(g_hSemaph_CusJob);
}

unsigned int __stdcall Thread_Alarm(LPVOID pParam)
{
	CString strPath;
	while (g_bWork)
	{
		WaitForSingleObject(g_hSemaph_Alarm,INFINITE);
		if (!g_bWork)
		{
			break;
		}

		g_MutexSound.lock();
		strPath = "c:\\win95\\media\\The Microsoft Sound.wav";
		PlaySound(strPath, NULL, SND_FILENAME | SND_ASYNC);
// 		PlaySound(MAKEINTRESOURCE(IDR_WAVESOUND), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
// 		Sleep(7000);
// 		PlaySound(MAKEINTRESOURCE(IDR_WAVESOUND), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
// 		Sleep(8000);
		
		g_MutexSound.unlock();
	}
	

	return 0;
}
unsigned int __stdcall Thread_SayTime(LPVOID pParam)
{
	while (g_bWork)
	{
		WaitForSingleObject(g_hSemaph_SayTime, INFINITE);
		if (!g_bWork)
		{
			break;
		}

		g_MutexSound.lock();

		CTime m_NowTime;
		m_NowTime = CTime::GetCurrentTime();
		int n_Hour = m_NowTime.GetHour();
		//以下为报时代码,原理为通过对当前时间的判断,调用不同的声音文件(资源文件)进行组合播放,形成报时效果
		PlaySound(MAKEINTRESOURCE(IDR_WAVE_TIMENOW), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		Sleep(1550);
		if (n_Hour < 8)
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_MORNING), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		else if (n_Hour < 13)
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_AM), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		else if (n_Hour < 19)
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_PM), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		else
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_EM), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		if (n_Hour > 12)
		{
			n_Hour = n_Hour - 12;
		}
		int n_Minute = m_NowTime.GetMinute();
		int n_Second = m_NowTime.GetSecond();



		Sleep(580);
		switch (n_Hour)
		{
		case 0:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T00), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 1:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T01), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 2:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T2), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 3:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T03), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 4:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T04), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 5:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T05), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 6:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T06), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 7:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T07), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 8:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T08), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 9:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T09), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 10:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T10), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 11:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T11), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		case 12:
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_T12), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			break;
		}
		if (n_Hour < 10)
			Sleep(450);
		else
			Sleep(550);
		PlaySound(MAKEINTRESOURCE(IDR_WAVE_POINT), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		Sleep(500);

		int n_Front_Min;
		int n_Behind_Min;
		int n_FTemp = n_Minute;

		n_Front_Min = n_Minute / 10;
		n_Behind_Min = n_Minute % 10;

		if (n_Front_Min <= 0)
		{
			if (n_FTemp != 0)
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T00), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
			Sleep(400);
			switch (n_FTemp)
			{
			case 0:
				break;
			case 1:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T01), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 2:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T02), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 3:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T03), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 4:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T04), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 5:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T05), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 6:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T06), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 7:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T07), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 8:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T08), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 9:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T09), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			default:
				break;
			}
			Sleep(500);
			if (n_FTemp != 0)
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_MIN), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);

		}
		else
		{
			switch (n_Front_Min)
			{
			case 1:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T10), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 2:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T20), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 3:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T30), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 4:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T40), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 5:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T50), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			}
			if (n_Front_Min == 1)
				Sleep(470);
			else if (n_Front_Min == 4)
				Sleep(565);
			else if (n_Behind_Min > 0)
				Sleep(520);
			switch (n_Behind_Min)
			{
			case 1:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T01), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 2:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T02), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 3:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T03), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 4:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T04), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 5:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T05), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 6:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T06), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 7:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T07), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 8:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T08), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			case 9:
				PlaySound(MAKEINTRESOURCE(IDR_WAVE_T09), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
				break;
			}

			Sleep(560);
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_MIN), AfxGetApp()->m_hInstance, SND_RESOURCE | SND_ASYNC | SND_NODEFAULT);
		}
		
		g_MutexSound.unlock();
		

	}

	return 0;
}
unsigned int __stdcall Thread_CusJob(LPVOID pParam)
{
	list<TASKEXE>::iterator task_itr;
	TASKEXE taskexe = {0};
	bool bExecute = false;

	while (g_bWork)
	{
		WaitForSingleObject(g_hSemaph_CusJob, INFINITE);
		if (!g_bWork)
		{
			break;
		}

		bExecute = false;
		g_MutexTaskList.lock();	
		task_itr = g_TaskExeList.begin();
		if (task_itr != g_TaskExeList.end())
		{
			bExecute = true;
			taskexe.nTaskType	= task_itr->nTaskType;
			taskexe.pParameter	= task_itr->pParameter;
			taskexe.taskFun		= task_itr->taskFun;
			g_TaskExeList.erase(task_itr);
		}
		g_MutexTaskList.unlock();

		if (bExecute)
		{
			taskexe.taskFun(taskexe.pParameter);
		}


		
	}

	return 0;
}



