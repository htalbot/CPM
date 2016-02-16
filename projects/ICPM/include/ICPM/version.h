
#ifndef __ICPM_VERSION_H__
#define __ICPM_VERSION_H__

#define ICPM_MAJOR 2
#define ICPM_MINOR 2
#define ICPM_PATCH 0

#if defined _MSC_VER
#   ifdef _DEBUG
        const char ICPM_DebugFlag[] = "DEBUG";
#   else
        const char ICPM_DebugFlag[] = "RELEASE";
#   endif
#else
#   ifdef DEBUG
        const char ICPM_DebugFlag[] = "DEBUG";
#   else
        const char ICPM_DebugFlag[] = "RELEASE";
#   endif
#endif



#endif // __ICPM_VERSION_H__
