
#ifndef __CPMG_VERSION_H__
#define __CPMG_VERSION_H__

#define CPMG_MAJOR 2
#define CPMG_MINOR 2
#define CPMG_PATCH 0


#if defined _MSC_VER
#   ifdef _DEBUG
        const char CPMG_DebugFlag[] = "DEBUG";
#   else
        const char CPMG_DebugFlag[] = "RELEASE";
#   endif
#else
#   ifdef DEBUG
        const char CPMG_DebugFlag[] = "DEBUG";
#   else
        const char CPMG_DebugFlag[] = "RELEASE";
#   endif
#endif


#endif // __CPMG_VERSION_H__
