#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ace/Init_ACE.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"

#pragma region Includes
#include <stdio.h>
#include <windows.h>
#include "CPMDService/ServiceInstaller.h"
#include "CPMDService/ServiceBase.h"
#include "CPMDService/Service.h"
#pragma endregion


//
// Settings of the service
//

//// Internal name of the service
//#define SERVICE_NAME             L"CPMDService"
//
//// Displayed name of the service
//#define SERVICE_DISPLAY_NAME     L"CPMDService Service"
//
//// Service start options.
//#define SERVICE_START_TYPE       SERVICE_DEMAND_START
//
//// List of service dependencies - "dep1\0dep2\0\0"
//#define SERVICE_DEPENDENCIES     L""
//
//// The name of the account under which the service should run
//#define SERVICE_ACCOUNT          L"NT AUTHORITY\\LocalService"
//
//// The password to the service account name
//#define SERVICE_PASSWORD         NULL
//
// Internal name of the service
#define SERVICE_NAME             "CPMDService"

// Displayed name of the service
#define SERVICE_DISPLAY_NAME     "CPMDService Service"

// Service start options.
#define SERVICE_START_TYPE       SERVICE_DEMAND_START

// List of service dependencies - "dep1\0dep2\0\0"
#define SERVICE_DEPENDENCIES     ""

// The name of the account under which the service should run
#define SERVICE_ACCOUNT          "NT AUTHORITY\\LocalService"

// The password to the service account name
#define SERVICE_PASSWORD         NULL


//
//  FUNCTION: wmain(int, wchar_t *[])
//
//  PURPOSE: entrypoint for the application.
//
//  PARAMETERS:
//    argc - number of command line arguments
//    argv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    wmain() either performs the command line task, or run the service.
//
int main(int argc, char *argv[])
{
    ACE::init();

//    if ((argc > 1) && ((*argv[1] == L'-' || (*argv[1] == L'/'))))
    if ((argc > 1) && ((*argv[1] == '-' || (*argv[1] == '/'))))
    {
//        if (_wcsicmp(L"install", argv[1] + 1) == 0)
        if (stricmp("install", argv[1] + 1) == 0)
        {
            // Install the service when the command is
            // "-install" or "/install".
            InstallService(
                SERVICE_NAME,               // Name of service
                SERVICE_DISPLAY_NAME,       // Name to display
                SERVICE_START_TYPE,         // Service start type
                SERVICE_DEPENDENCIES,       // Dependencies
                SERVICE_ACCOUNT,            // Service running account
                SERVICE_PASSWORD            // Password of the account
                );
        }
        //else if (_wcsicmp(L"remove", argv[1] + 1) == 0)
        else if (stricmp("remove", argv[1] + 1) == 0)
        {
            // Uninstall the service when the command is
            // "-remove" or "/remove".
            UninstallService(SERVICE_NAME);
        }
        else
        {
            printf("Bad argument. Expected: -install/-remove\n");
        }
    }
    else
    {
        //wprintf(L"Parameters:\n");
        //wprintf(L" -install  to install the service.\n");
        //wprintf(L" -remove   to remove the service.\n");
        printf("Parameters:\n");
        printf(" -install  to install the service.\n");
        printf(" -remove   to remove the service.\n");

        CSampleService service(SERVICE_NAME);
        if (!CServiceBase::Run(service))
        {
            //wprintf(L"Service failed to run w/err 0x%08lx\n", GetLastError());
            printf("Service failed to run w/err 0x%08lx\n", GetLastError());
        }
    }

    ACE::fini();

    return 0;
}
