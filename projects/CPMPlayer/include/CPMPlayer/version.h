
#ifndef __CPMPLAYER_VERSION_H__
#define __CPMPLAYER_VERSION_H__

#define CPMPLAYER_MAJOR 2
#define CPMPLAYER_MINOR 2
#define CPMPLAYER_PATCH 0


#if defined _MSC_VER
#   ifdef _DEBUG
        const char CPMPLAYER_DebugFlag[] = "DEBUG";
#   else
        const char CPMPLAYER_DebugFlag[] = "RELEASE";
#   endif
#else
#   ifdef DEBUG
        const char CPMPLAYER_DebugFlag[] = "DEBUG";
#   else
        const char CPMPLAYER_DebugFlag[] = "RELEASE";
#   endif
#endif


#endif // __CPMPLAYER_VERSION_H__
