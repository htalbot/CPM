
#include "CPMObjDummy/CPMObjDummy_i.h"

void CPMObjDummy_get_version(int & major, int & minor, int & patch)
{
    major = CPMOBJDUMMY_MAJOR;
    minor = CPMOBJDUMMY_MINOR;
    patch = CPMOBJDUMMY_PATCH;
}

CPMObjDummy_i::CPMObjDummy_i()
{
}

CPMObjDummy_i::~CPMObjDummy_i()
{
}

void CPMObjDummy_i::get_version (
    ::CORBA::Long_out major,
    ::CORBA::Long_out minor,
    ::CORBA::Long_out patch)
{
    CPMObjDummy_get_version(major, minor, patch);
}

void CPMObjDummy_i::ctrl_ping (
      void)
{
}

CORBA::Long CPMObjDummy_i::get()
{
    static long i(0);
    printf("CPMObjDummy_i get(%ld)...\n", i);

    return i++;
}

bool CPMObjDummy_i::init_i()
{
    //ACE_DEBUG((LM_DEBUG, "CPMObjDummy_i::init_i()\n"));
    return true;
}


