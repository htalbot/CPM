
#ifndef __TEST_MYDLL_VERSION_H__
#define __TEST_MYDLL_VERSION_H__

#define TEST_MYDLL_MAJOR 2
#define TEST_MYDLL_MINOR 1
#define TEST_MYDLL_PATCH 7


#if defined _MSC_VER
#   ifdef _DEBUG
        const char TEST_MYDLL_DebugFlag[] = "DEBUG";
#   else
        const char TEST_MYDLL_DebugFlag[] = "RELEASE";
#   endif
#else
#   ifdef DEBUG
        const char TEST_MYDLL_DebugFlag[] = "DEBUG";
#   else
        const char TEST_MYDLL_DebugFlag[] = "RELEASE";
#   endif
#endif


#endif // __TEST_MYDLL_VERSION_H__
