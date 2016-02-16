
#ifndef __CPMDSERVICE_VERSION_H__
#define __CPMDSERVICE_VERSION_H__

#define CPMDSERVICE_MAJOR 2
#define CPMDSERVICE_MINOR 2
#define CPMDSERVICE_PATCH 0


#if defined _MSC_VER
#   ifdef _DEBUG
        const char CPMDSERVICE_DebugFlag[] = "DEBUG";
#   else
        const char CPMDSERVICE_DebugFlag[] = "RELEASE";
#   endif
#else
#   ifdef DEBUG
        const char CPMDSERVICE_DebugFlag[] = "DEBUG";
#   else
        const char CPMDSERVICE_DebugFlag[] = "RELEASE";
#   endif
#endif


#endif // __CPMDSERVICE_VERSION_H__
