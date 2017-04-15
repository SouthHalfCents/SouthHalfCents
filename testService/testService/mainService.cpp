#include <windows.h>
#include <stdio.h>
#pragma comment(lib, "Advapi32")

#define SLEEP_TIME 1000*15
#define LOGFILE "C://AngoTestService.txt"
#define SERVICE_NAME TEXT("AngoTestService")

SERVICE_STATUS			g_ServiceStatus = {0};
SERVICE_STATUS_HANDLE	g_hStatusHandle = NULL;
HANDLE					g_hServiceStopEvent = INVALID_HANDLE_VALUE;

void WINAPI ServiceMain(int argc, char** argv);
void WINAPI ServiceCtrlHandler(DWORD CtrlCode);
DWORD WINAPI ServiceWorkerThread (LPVOID lpParam);
int WriteToLog(char* str);

//services.msc  �鿴����
//SC�����޸ģ� ������ϵͳ�ʺš��ҡ����������潻����   sc config TestService type= interact type= own
//��������
//sc create angoTestService binPath= "F:\Visual Studio 2013\Projects\testService\Debug\testService.exe"

//�����������VS���ط�����̽��е���

//��������Ϊ�Զ�������sc config ������ start= auto


int main(int argc, char* argv[])
{
	SERVICE_TABLE_ENTRY ServiceTable[] =
	{
		{ SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};

	if (StartServiceCtrlDispatcher(ServiceTable) == FALSE)
	{
		DWORD dwError = GetLastError();
		return dwError;
	}

	return 0;
}


void WINAPI ServiceMain(int argc, char** argv)
{
	DWORD Status = E_FAIL;



	// Tell the service controller we are starting
// 	ZeroMemory(&g_ServiceStatus, sizeof(g_ServiceStatus));
// 	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
// 	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
// 	g_ServiceStatus.dwControlsAccepted = 0;
// 	g_ServiceStatus.dwWin32ExitCode = 0;
// 	g_ServiceStatus.dwServiceSpecificExitCode = 0;
// 	g_ServiceStatus.dwCheckPoint = 0;




	int error = 0;

	ZeroMemory(&g_ServiceStatus, sizeof(g_ServiceStatus));
	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;
	g_ServiceStatus.dwWaitHint = 0;


	g_hStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME,(LPHANDLER_FUNCTION)ServiceCtrlHandler);
	if (g_hStatusHandle == NULL)
	{
		// Registering Control Handler failed
		return;
	}


	if (SetServiceStatus(g_hStatusHandle, &g_ServiceStatus) == FALSE)
	{
		OutputDebugString(TEXT("My Sample Service: ServiceMain: SetServiceStatus returned error"));
		WriteToLog(("My Sample Service: ServiceMain: SetServiceStatus returned error"));
	}
	WriteToLog("Monitoring started.");


	 /*
     * Perform tasks necessary to start the service here
     */
 
    // Create a service stop event to wait on later
    g_hServiceStopEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
    if (g_hServiceStopEvent == NULL) 
    {   
        // Error creating event
        // Tell service controller we are stopped and exit
        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        g_ServiceStatus.dwWin32ExitCode = GetLastError();
        g_ServiceStatus.dwCheckPoint = 1;
 
		if (SetServiceStatus(g_hStatusHandle, &g_ServiceStatus) == FALSE)
		{
			OutputDebugString(TEXT("My Sample Service: ServiceMain: SetServiceStatus returned error"));
			WriteToLog("My Sample Service: ServiceMain: SetServiceStatus returned error");
		}
        return; 
    }    


	// Tell the service controller we are started
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;

	// We report the running status to SCM. 
	if (SetServiceStatus(g_hStatusHandle, &g_ServiceStatus) == FALSE)
	{
		OutputDebugString(TEXT("My Sample Service: ServiceMain: SetServiceStatus returned error"));
		WriteToLog("My Sample Service: ServiceMain: SetServiceStatus returned error");
	}

	// Start a thread that will perform the main task of the service
	HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);

	// Wait until our worker thread exits signaling that the service needs to stop
	WaitForSingleObject(hThread, INFINITE);



	 /*
     * Perform any cleanup tasks 
     */
 
    CloseHandle (g_hServiceStopEvent);
 
    // Tell the service controller we are stopped
    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 3;
 
    if (SetServiceStatus (g_hStatusHandle, &g_ServiceStatus) == FALSE)
    {
        OutputDebugString(TEXT("My Sample Service: ServiceMain: SetServiceStatus returned error"));
		WriteToLog("My Sample Service: ServiceMain: SetServiceStatus returned error");
    }

	return;

}

void WINAPI ServiceCtrlHandler(DWORD CtrlCode)
{
	switch (CtrlCode)
	{
	case SERVICE_CONTROL_STOP:
	{
		if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
			break;
		WriteToLog("Monitoring stopped.");
		g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCheckPoint = 4;
		g_ServiceStatus.dwWin32ExitCode = 0;
		
		if (SetServiceStatus(g_hStatusHandle, &g_ServiceStatus) == FALSE)
		{
			OutputDebugString(TEXT("My Sample Service: ServiceCtrlHandler: SetServiceStatus returned error"));
			WriteToLog("My Sample Service: ServiceCtrlHandler: SetServiceStatus returned error");
		}

		// This will signal the worker thread to start shutting down
		SetEvent(g_hServiceStopEvent);
		break;
	}


	default:
		break;
	}

	// Report current status
	SetServiceStatus(g_hStatusHandle, &g_ServiceStatus);
	return;
}


DWORD WINAPI ServiceWorkerThread (LPVOID lpParam)
{
	int nCount = 0;
	char szBuff[MAX_PATH] = {0};
	MEMORYSTATUS status;//�������ڴ���Ϣ�ı���

    //  Periodically check if the service has been requested to stop
    while (WaitForSingleObject(g_hServiceStopEvent, 0) != WAIT_OBJECT_0)
    {        
        /* 
         * Perform main service function here
         */
		if( ++nCount > 65535 )
			nCount = 1;
		
		memset(szBuff,0x00,sizeof(szBuff));

		GlobalMemoryStatus(&status);
		sprintf_s(szBuff, "��%d�Σ�  ���������ڴ��СΪ %d KB", nCount, status.dwAvailPhys/1000);
		WriteToLog(szBuff);
		HINSTANCE hInstance = 0;
		hInstance = ShellExecute(NULL, TEXT("open"), TEXT("E:\\github\\SouthHalfCents.git\\trunk\\Run\\AngoTime.exe"), NULL, NULL, SW_SHOW|SW_RESTORE);
		hInstance = ShellExecute(NULL, TEXT("open"), TEXT("E:\\github\\SouthHalfCents.git\\trunk\\Run\\Ango.exe"), NULL, NULL, SW_SHOW|SW_RESTORE);

        //  Simulate some work by sleeping
        Sleep(SLEEP_TIME);
    }
 
    return ERROR_SUCCESS;
} 


int WriteToLog(char* str)
{
	FILE* fLog = NULL;
	int error = fopen_s(&fLog, LOGFILE, "a+");
	if (fLog == NULL)
		return -1;
	fprintf(fLog, "%s \n", str);
	fclose(fLog);
	return 0;
}
