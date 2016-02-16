
#ifndef __TEST_CPMEVENTRAISER_VERSION_H__
#define __TEST_CPMEVENTRAISER_VERSION_H__

#define TEST_CPMEVENTRAISER_MAJOR 2
#define TEST_CPMEVENTRAISER_MINOR 2
#define TEST_CPMEVENTRAISER_PATCH 0


#if defined _MSC_VER
#   ifdef _DEBUG
        const char TEST_CPMEVENTRAISER_DebugFlag[] = "DEBUG";
#   else
        const char TEST_CPMEVENTRAISER_DebugFlag[] = "RELEASE";
#   endif
#else
#   ifdef DEBUG
        const char TEST_CPMEVENTRAISER_DebugFlag[] = "DEBUG";
#   else
        const char TEST_CPMEVENTRAISER_DebugFlag[] = "RELEASE";
#   endif
#endif


#endif // __TEST_CPMEVENTRAISER_VERSION_H__
