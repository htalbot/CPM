
#include "CPMCore/CPMUtils.h"
#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ace/Process.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"

#pragma region Includes
#include "CPMDService/Service.h"
#include "CPMDService/ThreadPool.h"
#pragma endregion

//CSampleService::CSampleService(PWSTR pszServiceName, 
CSampleService::CSampleService(PSTR pszServiceName, 
                               BOOL fCanStop, 
                               BOOL fCanShutdown, 
                               BOOL fCanPauseContinue)
: CServiceBase(pszServiceName, fCanStop, fCanShutdown, fCanPauseContinue)
{
    m_fStopping = FALSE;

    // Create a manual-reset event that is not signaled at first to indicate 
    // the stopped signal of the service.
    m_hStoppedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (m_hStoppedEvent == NULL)
    {
        throw GetLastError();
    }
}


CSampleService::~CSampleService(void)
{
    if (m_hStoppedEvent)
    {
        CloseHandle(m_hStoppedEvent);
        m_hStoppedEvent = NULL;
    }
}


//
//   FUNCTION: CSampleService::OnStart(DWORD, LPWSTR *)
//
//   PURPOSE: The function is executed when a Start command is sent to the 
//   service by the SCM or when the operating system starts (for a service 
//   that starts automatically). It specifies actions to take when the 
//   service starts. In this code sample, OnStart logs a service-start 
//   message to the Application log, and queues the main service function for 
//   execution in a thread pool worker thread.
//
//   PARAMETERS:
//   * dwArgc   - number of command line arguments
//   * lpszArgv - array of command line arguments
//
//   NOTE: A service application is designed to be long running. Therefore, 
//   it usually polls or monitors something in the system. The monitoring is 
//   set up in the OnStart method. However, OnStart does not actually do the 
//   monitoring. The OnStart method must return to the operating system after 
//   the service's operation has begun. It must not loop forever or block. To 
//   set up a simple monitoring mechanism, one general solution is to create 
//   a timer in OnStart. The timer would then raise events in your code 
//   periodically, at which time your service could do its monitoring. The 
//   other solution is to spawn a new thread to perform the main service 
//   functions, which is demonstrated in this code sample.
//
//void CSampleService::OnStart(DWORD dwArgc, LPWSTR *lpszArgv)
void CSampleService::OnStart(DWORD dwArgc, LPSTR *lpszArgv)
{
    FILE * pf = fopen("C:\\dddd", "wt");
    ACE_Process_Options options;
    options.command_line("CPMDPlayer.exe boot-cpmd");
    //options.creation_flags(CREATE_NEW_CONSOLE|DETACHED_PROCESS|CREATE_NEW_PROCESS_GROUP);
    //STARTUPINFO * si = options.startup_info();
    //memset(si, 0, sizeof(si));
    //si->cb = sizeof(STARTUPINFO);
    //si->dwFlags = STARTF_USESHOWWINDOW;
    //si->wShowWindow = SW_HIDE;
    //si->wShowWindow = SW_SHOW;
    
    ACE_Process process;
    pid_t pid = process.spawn(options);
    if (pid == -1)
    {
        WriteEventLogEntry("CPMD FAILED to started", 
            EVENTLOG_INFORMATION_TYPE);
    }
    else
    {
        WriteEventLogEntry("CPMD started", 
            EVENTLOG_INFORMATION_TYPE);
    }

    //if (CPMUtils_ns::CPMUtils::boot_cpm())
    //{
    //    //WriteEventLogEntry(L"CPM started", 
    //    WriteEventLogEntry("CPM started", 
    //        EVENTLOG_INFORMATION_TYPE);
    //}
    //else
    //{
    //    //WriteEventLogEntry(L"CPM not started", 
    //    WriteEventLogEntry("CPM not started", 
    //        EVENTLOG_ERROR_TYPE);
    //}

    // Queue the main service function for execution in a worker thread.
    CThreadPool::QueueUserWorkItem(&CSampleService::ServiceWorkerThread, this);
}


//
//   FUNCTION: CSampleService::ServiceWorkerThread(void)
//
//   PURPOSE: The method performs the main function of the service. It runs 
//   on a thread pool worker thread.
//
void CSampleService::ServiceWorkerThread(void)
{
    // Periodically check if the service is stopping.
    while (!m_fStopping)
    {
        // Perform main service function here...

        ::Sleep(2000);  // Simulate some lengthy operations.
    }

    // Signal the stopped event.
    SetEvent(m_hStoppedEvent);
}


//
//   FUNCTION: CSampleService::OnStop(void)
//
//   PURPOSE: The function is executed when a Stop command is sent to the 
//   service by SCM. It specifies actions to take when a service stops 
//   running. In this code sample, OnStop logs a service-stop message to the 
//   Application log, and waits for the finish of the main service function.
//
//   COMMENTS:
//   Be sure to periodically call ReportServiceStatus() with 
//   SERVICE_STOP_PENDING if the procedure is going to take long time. 
//
void CSampleService::OnStop()
{
    if (CPMUtils_ns::CPMUtils::shutdown_cpm())
    {
        //WriteEventLogEntry(L"CPM shutdown", 
        WriteEventLogEntry("CPM shutdown (cpmm)", 
            EVENTLOG_INFORMATION_TYPE);
    }
    else
    {
        //WriteEventLogEntry(L"CPM not shutdown", 
        WriteEventLogEntry("CPM not shutdown (cpmm)", 
            EVENTLOG_ERROR_TYPE);
    }

    if (!CPMUtils_ns::CPMUtils::shutdown_cpmd())
    {
        WriteEventLogEntry("CPM shutdown (cpmd)", 
            EVENTLOG_INFORMATION_TYPE);
    }
    else
    {
        WriteEventLogEntry("CPM not shutdown (cpmd)", 
            EVENTLOG_ERROR_TYPE);
    }

    // Indicate that the service is stopping and wait for the finish of the 
    // main service function (ServiceWorkerThread).
    m_fStopping = TRUE;
    if (WaitForSingleObject(m_hStoppedEvent, INFINITE) != WAIT_OBJECT_0)
    {
        throw GetLastError();
    }
}