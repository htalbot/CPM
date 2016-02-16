#pragma once

#include "CPMDService/ServiceBase.h"

class CSampleService : public CServiceBase
{
public:

    //CSampleService(PWSTR pszServiceName, 
    CSampleService(PSTR pszServiceName, 
        BOOL fCanStop = TRUE, 
        BOOL fCanShutdown = TRUE, 
        BOOL fCanPauseContinue = FALSE);
    virtual ~CSampleService(void);

protected:

    //virtual void OnStart(DWORD dwArgc, PWSTR *pszArgv);
    virtual void OnStart(DWORD dwArgc, PSTR *pszArgv);
    virtual void OnStop();

    void ServiceWorkerThread(void);

private:

    BOOL m_fStopping;
    HANDLE m_hStoppedEvent;
};