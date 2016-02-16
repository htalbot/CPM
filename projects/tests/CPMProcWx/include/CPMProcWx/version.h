#ifndef __CPMPROCWX_VERSION_H__
#define __CPMPROCWX_VERSION_H__

#define CPMPROCWX_MAJOR 2
#define CPMPROCWX_MINOR 2
#define CPMPROCWX_PATCH 0


#if defined _MSC_VER
#   ifdef _DEBUG
        const char CPMPROCWX_DebugFlag[] = "DEBUG";
#   else
        const char CPMPROCWX_DebugFlag[] = "RELEASE";
#   endif
#else
#   ifdef DEBUG
        const char CPMPROCWX_DebugFlag[] = "DEBUG";
#   else
        const char CPMPROCWX_DebugFlag[] = "RELEASE";
#   endif
#endif


#endif // __CPMPROCWX_VERSION_H__
