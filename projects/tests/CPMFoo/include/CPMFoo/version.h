#ifndef __CPMFOO_VERSION_H__
#define __CPMFOO_VERSION_H__

#define CPMFOO_MAJOR 2
#define CPMFOO_MINOR 2
#define CPMFOO_PATCH 0


#if defined _MSC_VER
#   ifdef _DEBUG
        const char CPMFOO_DebugFlag[] = "DEBUG";
#   else
        const char CPMFOO_DebugFlag[] = "RELEASE";
#   endif
#else
#   ifdef DEBUG
        const char CPMFOO_DebugFlag[] = "DEBUG";
#   else
        const char CPMFOO_DebugFlag[] = "RELEASE";
#   endif
#endif


#endif // __CPMFOO_VERSION_H__
