
#ifndef __CPMOBJDUMMY_VERSION_H__
#define __CPMOBJDUMMY_VERSION_H__

#define CPMOBJDUMMY_MAJOR 2
#define CPMOBJDUMMY_MINOR 2
#define CPMOBJDUMMY_PATCH 0

#if defined _MSC_VER
#   ifdef _DEBUG
        const char CPMOBJDUMMY_DebugFlag[] = "DEBUG";
#   else
        const char CPMOBJDUMMY_DebugFlag[] = "RELEASE";
#   endif
#else
#   ifdef DEBUG
        const char CPMOBJDUMMY_DebugFlag[] = "DEBUG";
#   else
        const char CPMOBJDUMMY_DebugFlag[] = "RELEASE";
#   endif
#endif



#endif // __CPMOBJDUMMY_VERSION_H__
