#ifndef __CPMTRACEERASER_VERSION_H__
#define __CPMTRACEERASER_VERSION_H__

#define CPMTRACEERASER_MAJOR 2
#define CPMTRACEERASER_MINOR 2
#define CPMTRACEERASER_PATCH 0


#if defined _MSC_VER
#   ifdef _DEBUG
        const char CPMTRACEERASER_DebugFlag[] = "DEBUG";
#   else
        const char CPMTRACEERASER_DebugFlag[] = "RELEASE";
#   endif
#else
#   ifdef DEBUG
        const char CPMTRACEERASER_DebugFlag[] = "DEBUG";
#   else
        const char CPMTRACEERASER_DebugFlag[] = "RELEASE";
#   endif
#endif


#endif // __CPMTRACEERASER_VERSION_H__
