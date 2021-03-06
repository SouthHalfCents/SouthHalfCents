#include "stdafx.h"
#include "Database.h"
#include "AngoTimeDlg.h"

#include "resource.h"
//playsound
#include <mmsystem.h>
#pragma comment( lib, "Winmm.lib" )


HANDLE			g_hThread_Alarm		=	NULL;
HANDLE			g_hThread_SayTime	=	NULL;
HANDLE			g_hThread_CusJob	=	NULL;
HANDLE			g_hThread_Clock		=	NULL;

HANDLE			g_hSemaph_Clock		=	NULL;
HANDLE			g_hSemaph_Alarm		=	NULL;	//闹钟
HANDLE			g_hSemaph_SayTime	=	NULL;	//报时
HANDLE			g_hSemaph_CusJob	=	NULL;	//定时任务

mutex			g_MutexSound;
BOOL			g_bWork = FALSE;

mutex			g_MutexTaskList;
list<TASKEXE>	g_TaskExeList;

 mutex			gMutexClockList;		//闹钟锁

//-------------------------------------------------------------------------------------------------------------------------
void StartWork(void *pThis)
{
	if ( g_bWork )
		return;
	g_bWork = TRUE;

	//创建信号量
	g_hSemaph_Alarm		= CreateSemaphore(NULL, 0, 2, _T("AngoTime_Alarm"));
	g_hSemaph_SayTime	= CreateSemaphore(NULL, 0, 2, _T("AngoTime_SayTime"));
	g_hSemaph_CusJob	= CreateSemaphore(NULL, 0, 8, _T("AngoTime_CusJob"));
	g_hSemaph_Clock		= CreateSemaphore(NULL, 0, 2, _T("AngoTime_Clock"));
	/*
	第一个参数：安全属性，如果为NULL则是默认安全属性 
	第二个参数：信号量的初始值，要>=0且<=第三个参数
	第三个参数：信号量的最大值
	第四个参数：信号量的名称
	*/


	//创建线程
	g_hThread_Clock		= (HANDLE)_beginthreadex(NULL, 0, Thread_Clock, pThis, 0, NULL);
	g_hThread_Alarm		= (HANDLE)_beginthreadex(NULL, 0, Thread_Alarm, NULL, 0, NULL);
	g_hThread_SayTime	= (HANDLE)_beginthreadex(NULL, 0, Thread_SayTime, NULL, 0, NULL);
	g_hThread_CusJob	= (HANDLE)_beginthreadex(NULL, 0, Thread_CusJob, NULL, 0, NULL);

	
	
}

void StopWork()
{
	if ( !g_bWork )
		return;
	g_bWork = FALSE;

	ReleaseSemaphore(g_hSemaph_Alarm, 1, NULL);		//释放1个信号量
	ReleaseSemaphore(g_hSemaph_SayTime, 1, NULL);	//释放1个信号量
	ReleaseSemaphore(g_hSemaph_CusJob, 1, NULL);	//释放1个信号量
	ReleaseSemaphore(g_hSemaph_Clock, 1, NULL);		//释放1个信号量


	WaitForSingleObject(g_hThread_Alarm, INFINITE);
	WaitForSingleObject(g_hThread_SayTime, INFINITE);
	WaitForSingleObject(g_hThread_CusJob, INFINITE);
	WaitForSingleObject(g_hThread_Clock, INFINITE);

	CloseHandle(g_hThread_Alarm);
	CloseHandle(g_hThread_SayTime);
	CloseHandle(g_hThread_CusJob);
	CloseHandle(g_hThread_Clock);

	CloseHandle(g_hSemaph_Alarm);
	CloseHandle(g_hSemaph_SayTime);
	CloseHandle(g_hSemaph_CusJob);
	CloseHandle(g_hSemaph_Clock);
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

		CString strPath = AfxGetApp()->GetProfileStringW(ANGO_SECTION_SET, CLOCK_MUSIC, 0);
		CString strTemp = _T("open ") + strPath;
		g_MutexSound.lock();

		if (strPath.GetLength() >= 4)
		{
			mciSendString(strTemp , NULL, 0, 0);
		}
		else
		{
			PlaySound(MAKEINTRESOURCE(IDR_WAVE_SOUND),AfxGetApp()->m_hInstance,SND_RESOURCE|SND_ASYNC|SND_NODEFAULT);
			ReleaseSemaphore(g_hSemaph_SayTime, 1, NULL);	//释放1个信号量
		}
		
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


unsigned int __stdcall  Thread_Clock(LPVOID pParam)
{
	CAngoTimeDlg* pBase = (CAngoTimeDlg*)pParam;

	//获得当前系统时间。
	CTime tTime = CTime::GetCurrentTime();
	int   S = 0;
	float M = 0;
	float H = 0;

	pBase->m_Point_Start.x = 65;
	pBase->m_Point_Start.y = 64;
	COLORREF cRgb = 0;

	CPen *pPenOld = NULL;
	CBrush *pBrushOld = NULL;
	CPen PenNew;
	CBrush BrushNew;
	CClientDC dc(pBase);

	while (g_bWork)
	{
		WaitForSingleObject(g_hSemaph_Clock, INFINITE);

		tTime = CTime::GetCurrentTime();
		S = tTime.GetSecond();
		M = float(tTime.GetMinute() + S / 60.0);
		H = float(tTime.GetHour() + M / 60.0);
		if (H > 12)
			H = H - 12;
		H = H * 5;


		//方法为画每根针前用背景色擦去上一次画的针（由于背景色渐变，所以加入了计算）
		//从图片获取背景色rgb
		//COLORREF cRgb = dc.GetPixel(m_Point_End);
		//BYTE byRed   = GetRValue(cRgb);
		//BYTE byGreen = GetGValue(cRgb);
		//BYTE byBlue	 = GetBValue(cRgb);


		//////////////////////////////////////////////	
		if (!pBase->m_bFirstClock)
		{
			//用背景色擦去上一次画的针
			pBase->m_Point_End = pBase->m_point_lastHour;
			cRgb = pBase->m_vBmpColor[pBase->m_Point_End.x][pBase->m_Point_End.y];

			PenNew.CreatePen(PS_SOLID, 4, cRgb);
			BrushNew.CreateSolidBrush(cRgb);
			pBrushOld = dc.SelectObject(&BrushNew);
			pPenOld = dc.SelectObject(&PenNew);
			dc.MoveTo(pBase->m_Point_Start);
			dc.LineTo(pBase->m_Point_End);
		}

		//画时针
		PenNew.DeleteObject();
		PenNew.CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
		pPenOld = dc.SelectObject(&PenNew);
		BrushNew.DeleteObject();
		BrushNew.CreateSolidBrush(RGB(0, 0, 0));
		pBrushOld = dc.SelectObject(&BrushNew);
		pBase->m_Point_End.x = 65 + LONG(22 * sin(H*PI / 30));
		pBase->m_Point_End.y = 64 - LONG(22 * cos(H*PI / 30));

		//保存原有的位置
		pBase->m_point_lastHour = pBase->m_Point_End;

		dc.MoveTo(pBase->m_Point_Start);
		dc.LineTo(pBase->m_Point_End);



		///////////////////////////////////////////////
		if (!pBase->m_bFirstClock)
		{
			//擦除上次的指针
			pBase->m_Point_End = pBase->m_point_lastMin;
			cRgb = pBase->m_vBmpColor[pBase->m_Point_End.x][pBase->m_Point_End.y];

			BrushNew.DeleteObject();
			BrushNew.CreateSolidBrush(cRgb);
			pBrushOld = dc.SelectObject(&BrushNew);
			PenNew.DeleteObject();
			PenNew.CreatePen(PS_SOLID, 3, cRgb);
			pPenOld = dc.SelectObject(&PenNew);
			dc.MoveTo(pBase->m_Point_Start);
			dc.LineTo(pBase->m_Point_End);
		}

		//画分针
		BrushNew.DeleteObject();
		BrushNew.CreateSolidBrush(RGB(0, 0, 0));
		pBrushOld = dc.SelectObject(&BrushNew);
		PenNew.DeleteObject();
		PenNew.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
		pPenOld = dc.SelectObject(&PenNew);
		pBase->m_Point_End.x = 65 + LONG(30 * sin(M*PI / 30));
		pBase->m_Point_End.y = 64 - LONG(30 * cos(M*PI / 30));

		pBase->m_point_lastMin = pBase->m_Point_End;

		dc.MoveTo(pBase->m_Point_Start);
		dc.LineTo(pBase->m_Point_End);


		////////////////////////////////////////////	画秒针的短轴	
		S = (S + 30) % 60;

		if (!pBase->m_bFirstClock)
		{
			//擦除上次的指针
			pBase->m_Point_End = pBase->m_point_lastSecS;
			cRgb = pBase->m_vBmpColor[pBase->m_Point_End.x][pBase->m_Point_End.y];

			PenNew.DeleteObject();
			PenNew.CreatePen(PS_DASHDOTDOT, 2, cRgb);
			pPenOld = dc.SelectObject(&PenNew);
			dc.MoveTo(pBase->m_Point_Start);
			dc.LineTo(pBase->m_Point_End);
		}


		//画秒针短轴
		PenNew.DeleteObject();
		PenNew.CreatePen(PS_DASHDOTDOT, 2, RGB(255, 0, 0));
		pPenOld = dc.SelectObject(&PenNew);
		pBase->m_Point_End.x = 65 + LONG(6 * sin(S*PI / 30));
		pBase->m_Point_End.y = 64 - LONG(6 * cos(S*PI / 30));

		pBase->m_point_lastSecS = pBase->m_Point_End;

		dc.MoveTo(pBase->m_Point_Start);
		dc.LineTo(pBase->m_Point_End);



		///////////////////////画秒针的长轴
		S = (S + 30) % 60;

		if (!pBase->m_bFirstClock)
		{
			//擦除上次的指针
			pBase->m_Point_End = pBase->m_point_lastSecL;
			cRgb = pBase->m_vBmpColor[pBase->m_Point_End.x][pBase->m_Point_End.y];

			//		BrushNew.DeleteObject();
			//		BrushNew.CreateSolidBrush(RGB(C,C,C));
			//		BrushOld=dc.SelectObject(&BrushNew);
			PenNew.DeleteObject();
			PenNew.CreatePen(PS_DASHDOTDOT, 2, cRgb);
			pPenOld = dc.SelectObject(&PenNew);
			dc.MoveTo(pBase->m_Point_Start);
			dc.LineTo(pBase->m_Point_End);
		}


		//画秒针长轴
		//		BrushNew.DeleteObject();
		//		BrushNew.CreateSolidBrush(RGB(255,0,0));
		//		BrushOld=dc.SelectObject(&BrushNew);
		PenNew.DeleteObject();
		PenNew.CreatePen(PS_DASHDOTDOT, 2, RGB(255, 0, 0));
		pPenOld = dc.SelectObject(&PenNew);
		pBase->m_Point_End.x = 65 + LONG(30 * sin(S*PI / 30));
		pBase->m_Point_End.y = 64 - LONG(30 * cos(S*PI / 30));

		pBase->m_point_lastSecL = pBase->m_Point_End;

		dc.MoveTo(pBase->m_Point_Start);
		dc.LineTo(pBase->m_Point_End);


		PenNew.DeleteObject();
		BrushNew.DeleteObject();
		//////////////////////////////////////////////////////////////////
		pBase->m_bFirstClock = FALSE;

		dc.SetPixel(pBase->m_Point_Start, RGB(0, 0, 0));
		dc.SetPixel(pBase->m_Point_Start.x + 1, pBase->m_Point_Start.y, RGB(0, 0, 0));
		dc.SetPixel(pBase->m_Point_Start.x, pBase->m_Point_Start.y + 1, RGB(0, 0, 0));
		dc.SetPixel(pBase->m_Point_Start.x + 1, pBase->m_Point_Start.y + 1, RGB(0, 0, 0));
		dc.SetPixel(pBase->m_Point_Start.x - 1, pBase->m_Point_Start.y, RGB(0, 0, 0));
		dc.SetPixel(pBase->m_Point_Start.x, pBase->m_Point_Start.y - 1, RGB(0, 0, 0));
		dc.SetPixel(pBase->m_Point_Start.x - 1, pBase->m_Point_Start.y - 1, RGB(0, 0, 0));

		//报时
		if (!g_bWork)
		{
			break;
		}
		if (pBase->m_nSayTime == SAYTIME_ALL &&S == 0 && M == 0)//判断是否整点报时
			pBase->OnSaytimeNow();

		if (pBase->m_nSayTime == SAYTIME_HALF &&S == 0 && (M == 0 || M == 30))
			pBase->OnSaytimeNow();

		//定时任务
		if (!g_bWork)
		{
			break;
		}
		CTask::CheckTask(tTime);

		//闹钟
		if (!g_bWork)
		{
			break;
		}
		CClock::CheckClock(tTime);

	}
	return 0;

}
