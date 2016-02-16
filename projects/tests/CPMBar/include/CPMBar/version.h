#ifndef __CPMBAR_VERSION_H__
#define __CPMBAR_VERSION_H__

#define CPMBAR_MAJOR 2
#define CPMBAR_MINOR 2
#define CPMBAR_PATCH 0


#if defined _MSC_VER
#   ifdef _DEBUG
        const char CPMBAR_DebugFlag[] = "DEBUG";
#   else
        const char CPMBAR_DebugFlag[] = "RELEASE";
#   endif
#else
#   ifdef DEBUG
        const char CPMBAR_DebugFlag[] = "DEBUG";
#   else
        const char CPMBAR_DebugFlag[] = "RELEASE";
#   endif
#endif


#endif // __CPMBAR_VERSION_H__
