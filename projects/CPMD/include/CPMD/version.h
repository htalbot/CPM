#ifndef __CPMD_VERSION_H__
#define __CPMD_VERSION_H__

#define CPMD_MAJOR 2
#define CPMD_MINOR 2
#define CPMD_PATCH 0


#if defined _MSC_VER
#   ifdef _DEBUG
        const char CPMD_DebugFlag[] = "DEBUG";
#   else
        const char CPMD_DebugFlag[] = "RELEASE";
#   endif
#else
#   ifdef DEBUG
        const char CPMD_DebugFlag[] = "DEBUG";
#   else
        const char CPMD_DebugFlag[] = "RELEASE";
#   endif
#endif


#endif // __CPMD_VERSION_H__
