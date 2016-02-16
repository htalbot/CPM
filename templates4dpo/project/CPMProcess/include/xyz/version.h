#ifndef __XYZ_VERSION_H__
#define __XYZ_VERSION_H__

#define XYZ_MAJOR 2
#define XYZ_MINOR 1
#define XYZ_PATCH 7


#if defined _MSC_VER
#   ifdef _DEBUG
        const char XYZ_DebugFlag[] = "DEBUG";
#   else
        const char XYZ_DebugFlag[] = "RELEASE";
#   endif
#else
#   ifdef DEBUG
        const char XYZ_DebugFlag[] = "DEBUG";
#   else
        const char XYZ_DebugFlag[] = "RELEASE";
#   endif
#endif


#endif // __XYZ_VERSION_H__
